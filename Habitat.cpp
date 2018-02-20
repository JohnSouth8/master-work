/*
 * Environment.cpp
 *
 *  Created on: Jul 4, 2017
 *      Author: south
 */


#include "Habitat.h"

#include <cstdlib>
#include <iostream>
#include <string>
#include <map>
#include <Eigen/Dense>

#include "util.h"
#include "Chance.h"

#include "Animat.h"
#include "Meadow.h"

using namespace std;
using namespace Eigen;

namespace ecosystem {

/* constructors */


Habitat::Habitat( int sx, int sy, int fe, double density, util::Chance* ch ) {

	sizeX = sx;
	sizeY = sy;
	foodEnergyVal = fe;
	foodReserve = MatrixXf::Zero( sizeX, sizeY );
	distributeFood( density );
	fate = ch;

}

Habitat::Habitat( int sx, int sy, int fe, int n_meadows, int rAvg_meadows, float grAvg_meadows, util::Chance* ch ) {

	sizeX = sx;
	sizeY = sy;
	foodEnergyVal = fe;
	foodReserve = MatrixXf::Zero( sizeX, sizeY );
	fate = ch;

	std::vector<int> rs_m = fate->normalIntsString( n_meadows, rAvg_meadows, rAvg_meadows/5 );
	std::vector<float> grs_m = fate->normalFloatsString( n_meadows, grAvg_meadows, grAvg_meadows/10 );

	for ( int i = 0; i < n_meadows; ++i ) {

		int randX = util::randIntFrom( 0, sizeX );
		int randY = util::randIntFrom( 0, sizeY );

		Meadow* m = new Meadow( randX, randY, rs_m[i], grs_m[i], this );
		meadows.push_back( m );

	}

}

Habitat::Habitat( std::string iniFileName, util::Chance* ch ) {

	std::map<std::string, float> ini = util::readSimpleIni( iniFileName );

	// basic params
	sizeX = static_cast<int>( ini["size_x"] );
	sizeY = static_cast<int>( ini["size_y"] );
	foodEnergyVal = static_cast<int>( ini["food_energy"] );

	// member structures
	fate = ch;
	foodReserve = MatrixXf::Zero( sizeX, sizeY );

	// food emanators - meadows
	int n_mdw = static_cast<int>( ini["n_meadows"] );
	float r_mean_mdw = ini["radius_mean_meadows"];
	float r_std_mdw = ini["radius_std_meadows"];
	float gr_mean_mdw = ini["growrate_mean_meadows"];
	float gr_std_mdw = ini["growrate_std_meadows"];

	std::vector<int> xs = fate->nUniformRandomIntsFrom( n_mdw, 0, sizeX );
	std::vector<int> ys = fate->nUniformRandomIntsFrom( n_mdw, 0, sizeY );
	std::vector<int> rs = fate->normalIntsString( n_mdw, r_mean_mdw, r_std_mdw );
	std::vector<float> grs = fate->normalFloatsString( n_mdw, gr_mean_mdw, gr_std_mdw );

	for ( int i = 0; i < n_mdw; ++i ) {
		Meadow* m = new Meadow( xs[i], ys[i], rs[i], grs[i], this );
		meadows.push_back( m );
	}


}


/* destructor */

Habitat::~Habitat() {
	// destroy objects that pointers in data structures are pointing at: animats, meadows,...
	for ( auto m : meadows )
		delete m;

	for ( auto a : population )
		delete a.second;
}



/* member functions */

void Habitat::birth( Animat* ani ) {

	population[ani->name] = ani;

}



void Habitat::death( std::string name ) {

	delete population[name];
	population.erase( name );

}



void Habitat::growMeadows() {

	int grown = 0;
	for ( auto meadow : meadows ){

		grown += meadow->grow();

	}

	// fractional chance of a new meadow appearing
	if ( fate->uniformRandomUnitFloat() < 0.01 ) {

		int randX = fate->uniformRandomIntFrom( 0, sizeX );
		int randY = fate->uniformRandomIntFrom( 0, sizeY );

		Meadow* m = new Meadow( randX, randY, 100, 0.05, this );	// TODO: parameterize this!
		meadows.push_back( m );

	}


//	std::cout << "Amount of new food: " << grown << std::endl;

}



void Habitat::distributeFood( double density ) {

	float fraction;

	for ( int x = 0; x < sizeX; ++x ) {
		for ( int y = 0; y < sizeY; ++y ) {

			fraction = util::randFromUnitInterval();
			if ( fraction < density ) {
				foodReserve(x,y) = 1.0;
//				cout << "x: " << x << ", y: " << y << ", val: " << foodReserve(x,y) << endl;
			}

		}
	}

}


void Habitat::growFood_stable() {

	MatrixXf newFood = MatrixXf::Zero( sizeX, sizeY );

	float highDensity = 0.001;
	float lowDensity = 0.000001;
//	int num_food = foodReserve.sum();
//
//	int num_newFood = (int) round( num_food * someDensity );

	for ( int x = 0; x < sizeX; ++x ) {
		for ( int y = 0; y < sizeY; ++y ) {

			float randf = util::randFromUnitInterval();
			if ( foodReserve(x, y) > 0 && randf < highDensity ) {
				int randOffX = util::randIntFrom( -10, 10 );
				int randOffY = util::randIntFrom( -10, 10 );
				int nx = util::getWrappedIndex( x + randOffX, sizeX );
				int ny = util::getWrappedIndex( y + randOffY, sizeY );
				if ( newFood( nx, ny ) == 0 )
					newFood( nx, ny ) = 1;
			}

			if ( foodReserve(x, y) == 0 && randf < lowDensity ) {
				newFood(x, y) = 1;
			}

		}
	}

	foodReserve = foodReserve + newFood;

	std::cout << "Amount of new food: " << newFood.sum() << std::endl;


}


// optimize the shit out of this!!!
void Habitat::growFoodSlow() {

	// TODO: get all these numbers into parameters!

	MatrixXf newFood = MatrixXf::Zero( sizeX, sizeY );
//	MatrixXf highProbMask = MatrixXf::Constant( 5, 5, 0.005 );
//	MatrixXf lowProbMask = MatrixXf::Constant( 5, 5, 0.001 );

	for ( int i = 0; i < sizeX; ++i ) {
		for ( int j = 0; j < sizeY; ++j ) {

			// generate food in 5-square surroundings, but not where there is food already
			for ( int offx = -2; offx < 3; ++offx ){
				for ( int offy = -2; offy < 3; ++offy ){

					int i_x = i+offx;
					int i_y = j+offy;
					if ( i < 2 || i > sizeX-3 )
						i_x = util::getWrappedIndex( i_x, sizeX );
					if ( j < 2 || j > sizeY-3 )
						i_y = util::getWrappedIndex( i_y, sizeY );

					float prob = 0.000001;
					if ( foodReserve(i,j) > 0 )
						prob = 0.0001;

					// suspiciously high number of activations... check out. Preferably by visual inspection - heatmap of randoms
					float randf = util::randFromUnitInterval();
					if ( foodReserve(i_x, i_y) == 0 && newFood(i_x, i_y) == 0 && randf < prob ){
						newFood(i_x, i_y) = 1.0;
					}

				}
			}

		}
	}

	foodReserve = foodReserve + newFood;

	std::cout << "Amount of new food: " << newFood.sum() << std::endl;

}



float Habitat::consumeFood( int x, int y ) {

	if ( foodReserve( x, y ) == 0 ) {
		return 0;
	}
	else {
		foodReserve( x, y ) = 0.0;
		return foodEnergyVal;
	}

}



int Habitat::getXSize() {
	return sizeX;
}



int Habitat::getYSize() {
	return sizeY;
}



Eigen::MatrixXf Habitat::getFoodReserve() {
	return foodReserve;
}



} /* namespace ecosystem */
