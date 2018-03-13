/*
 * Environment.cpp
 *
 *  Created on: Jul 4, 2017
 *      Author: south
 */


#include "Habitat.h"

#include <cstdlib>
#include <iostream>
#include <iterator>
#include <string>
#include <map>
#include <Eigen/Dense>

#include "util.h"
#include "Chance.h"
#include "QuadTree.h"

#include "Grass.h"
#include "Animat.h"
#include "Meadow.h"

using namespace std;
using namespace Eigen;

namespace ecosystem {

/* constructors */


//Habitat::Habitat( int sx, int sy, int fe, double density, util::Chance* ch ) {
//
//	sizeX = sx;
//	sizeY = sy;
//	foodEnergyVal = fe;
//	foodReserve = MatrixXf::Zero( sizeX, sizeY );
//	distributeFood( density );
//	fate = ch;
//
//}
//
//Habitat::Habitat( int sx, int sy, int fe, int n_meadows, int rAvg_meadows, float grAvg_meadows, util::Chance* ch ) {
//
//	sizeX = sx;
//	sizeY = sy;
//	foodEnergyVal = fe;
//	foodReserve = MatrixXf::Zero( sizeX, sizeY );
//	fate = ch;
//
//	std::vector<int> rs_m = fate->normalIntsString( n_meadows, rAvg_meadows, rAvg_meadows/5 );
//	std::vector<float> grs_m = fate->normalFloatsString( n_meadows, grAvg_meadows, grAvg_meadows/10 );
//
//	for ( int i = 0; i < n_meadows; ++i ) {
//
//		int randX = util::randIntFrom( 0, sizeX );
//		int randY = util::randIntFrom( 0, sizeY );
//
//		Meadow* m = new Meadow( randX, randY, rs_m[i], grs_m[i], this );
//		meadows.push_back( m );
//
//	}
//
//}

Habitat::Habitat( std::string iniFileName, util::Chance* ch ) {

	std::map<std::string, float> ini = util::readSimpleIni( iniFileName );

	// basic params
	sizeX = static_cast<int>( ini["size_x"] );
	sizeY = static_cast<int>( ini["size_y"] );
	foodEnergyVal = static_cast<int>( ini["food_energy"] );

	// member structures
	fate = ch;
	foodReserve = MatrixXf::Zero( sizeX, sizeY );

	// quad trees
	unsigned int qt_bucketsize = static_cast<unsigned int>( ini["quadtree_bucketsize"] );
	util::coordinate env0( 0, 0 );
	util::coordinate env1( sizeX, sizeY );
	foodTree = util::QuadTree( qt_bucketsize, env0, env1 );
	populationTree = util::QuadTree( qt_bucketsize, env0, env1 );

	// food emanators - meadows
	int n_mdw = static_cast<int>( ini["n_meadows"] );
	p_newMeadow = ini["probability_new_meadow"];
	mean_rMeadows = ini["radius_mean_meadows"];
	std_rMeadows = ini["radius_std_meadows"];
	mean_grMeadows = ini["growrate_mean_meadows"];
	std_grMeadows = ini["growrate_std_meadows"];

	std::vector<int> xs = fate->nUniformRandomIntsFrom( n_mdw, 0, sizeX-1 );
	std::vector<int> ys = fate->nUniformRandomIntsFrom( n_mdw, 0, sizeY-1 );
	std::vector<int> rs = fate->normalIntsString( n_mdw, mean_rMeadows, std_rMeadows );
	std::vector<float> grs = fate->normalFloatsString( n_mdw, mean_grMeadows, std_grMeadows );

	for ( int i = 0; i < n_mdw; ++i ) {
		Meadow* m = new Meadow( xs[i], ys[i], rs[i], grs[i], this );
		meadows.push_back( m );
	}


}


/* destructor */

Habitat::~Habitat() {
	// destroy objects that pointers in data structures are pointing at: animats, meadows,...
	for ( auto g: vegetation )
		delete g;

	for ( auto m : meadows )
		delete m;

	for ( auto a : population )
		delete a.second;
}



/* member functions */


void Habitat::populateWorld( int n_animats, std::string animat_iniFileName, std::string fcm_concepts_fileName, std::string fcm_fileName ) {

	std::map<std::string, float> ini = util::readSimpleIni( animat_iniFileName );

	if ( n_animats == 0 )
		n_animats = static_cast<int>( ini["n_animats"] );

	float avg_size = ini["size"];
	float max_energy = ini["max_energy"];
	float start_energy = ini["start_energy"];
	float max_velocity = ini["max_velocity"];
//	float start_velocity = ini["start_velocity"];
	int max_age = static_cast<int>( ini["max_age"] );
	float vision_range = ini["vision_range"];
	float vision_angle = ini["vision_angle"];
	float olfactory_range = ini["olfactory_range"];
	float std_degree = ini["init_std_degree"];

	std::vector<float> sizes = fate->normalFloatsString( n_animats, avg_size, avg_size*std_degree );
	std::vector<float> max_vels = fate->normalFloatsString( n_animats, max_velocity, max_velocity*std_degree );
	std::vector<int> max_ages = fate->normalIntsString( n_animats, max_age, max_age*std_degree );
	std::vector<float> vis_ranges = fate->normalFloatsString( n_animats, vision_range, vision_range*std_degree );
	std::vector<float> vis_angles = fate->normalFloatsString( n_animats, vision_angle, vision_angle*std_degree );
	std::vector<float> olf_ranges = fate->normalFloatsString( n_animats, olfactory_range, olfactory_range*std_degree);

	std::vector<float> pxs = fate->nUniformRandomFloatsFrom( n_animats, 0, sizeX );
	std::vector<float> pys = fate->nUniformRandomFloatsFrom( n_animats, 0, sizeY );
	std::vector<float> dirs = fate->nUniformRandomFloatsFrom( n_animats, -PI, PI );

	for ( int i = 0; i < n_animats; ++i ) {

		std::string name = generateAnimatName();
		Animat* ani = new Animat (
			name,
			sizes[i],
			vis_ranges[i],
			vis_angles[i],
			olf_ranges[i],
			max_vels[i],
			max_energy,
			max_ages[i],
			pxs[i],
			pys[i],
			dirs[i],
			0,
			start_energy,
			this
		);

		int nConcepts = 10;	// TODO: make this number automatic
		ani->initFCM( nConcepts, fcm_concepts_fileName, fcm_fileName );

		population[name] = ani;

	}

}


void Habitat::birth( Animat* ani ) {

	population[ani->name] = ani;

}



void Habitat::death( std::string name ) {

	delete population[name];
	population.erase( name );

}



std::string Habitat::generateAnimatName() {

	int charmin = 65, charmax = 90, randint;
	std::string sname;

	for ( int i = 0; i < 10; ++i ) {
		randint = util::randIntFrom( charmin, charmax );
		sname.push_back( randint );
	}

	return sname;

}



void Habitat::growMeadows() {

	int grown = 0;
	for ( auto meadow : meadows ){

		grown += meadow->grow();

	}

	// fractional chance of a new meadow appearing
	if ( fate->uniformRandomUnitFloat() < p_newMeadow ) {

		int cx = fate->uniformRandomIntFrom( 0, sizeX-1 );
		int cy = fate->uniformRandomIntFrom( 0, sizeY-1 );
		int r = fate->normalInt( mean_rMeadows, std_rMeadows );
		float gr = fate->normalFloat( mean_grMeadows, std_grMeadows );

		Meadow* m = new Meadow( cx, cy, r, gr, this );
		meadows.push_back( m );

	}

//  debug
//	std::cout << "Amount of new food: " << grown << std::endl;

}


// deprecated !!
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



void Habitat::growGrass( int x, int y ) {

	Grass* ng = new Grass( x, y );
	vegetation.push_back( ng );
	foodTree.insert( ng );

}



int Habitat::consumeFood( int x, int y ){

	if ( foodReserve( x, y ) == 0 ) {
		return 0;
	}
	else {
		foodReserve( x, y ) = 0.0;
		return foodEnergyVal;
	}

}



void Habitat::measureDistances() {
	// TODO: to be debugged...
	if ( population.empty() )
		return;

	int n_animats = population.size();
	animatDistances = Eigen::MatrixXf::Zero( n_animats, n_animats );
	std::vector<std::string>( n_animats ).swap( animatOrder );	// reset the animat ordering vector

	int cnt1 = 0;
	map<std::string, Animat*>::iterator it1;
	for ( it1 = population.begin(); it1 != population.end(); ++it1 ) {

		float x1 = it1->second->posX;
		float y1 = it1->second->posY;

		int cnt2 = cnt1+1;
		map<std::string, Animat*>::iterator it2;
		for ( it2 = std::next( it1 ); it2 != population.end(); ++it2 ) {

			float x2 = it2->second->posX;
			float y2 = it2->second->posY;

			float d = sqrt( pow( x2-x1, 2 ) + pow( y2-y1, 2 ) );
			animatDistances( cnt1, cnt2 ) = d;
			animatDistances( cnt2, cnt1 ) = d;
		}

		animatOrder.push_back( it1->first );
		cnt1++;
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
