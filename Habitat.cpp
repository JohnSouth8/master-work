/*
 * Environment.cpp
 *
 *  Created on: Jul 4, 2017
 *      Author: south
 */


#include "Habitat.h"

#include <cstdlib>
#include <iostream>
#include <map>
#include <Eigen/Dense>

#include "util.h"

#include "Animat.h"

using namespace std;
using namespace Eigen;

namespace ecosystem {

/* constructors */

Habitat::Habitat() {

	sizeX = 1000;
	sizeY = 1000;
	foodEnergyVal = 10;
	foodReserve = MatrixXf::Zero( sizeX, sizeY );
	distributeFood( 0.01 );

}

Habitat::Habitat( int sx, int sy, int fe, double density ) {

	sizeX = sx;
	sizeY = sy;
	foodEnergyVal = fe;
	foodReserve = MatrixXf::Zero( sizeX, sizeY );
	distributeFood( density );

}

/* destructor */

Habitat::~Habitat() {

}



/* member functions */

void Habitat::birth( Animat* ani ) {

	population[ani->name] = ani;

}



void Habitat::death( const char* name ) {

	population.erase( name );

}



void Habitat::distributeFood( double density ) {

	double fraction;

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


// optimize the shit out of this!!!
void Habitat::growFood() {

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
