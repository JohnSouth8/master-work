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
	foodReserve = MatrixXd::Zero( sizeX, sizeY );
	distributeFood( 0.01 );

}

Habitat::Habitat( int sx, int sy, int fe, double density ) {

	sizeX = sx;
	sizeY = sy;
	foodEnergyVal = fe;
	foodReserve = MatrixXd::Zero( sizeX, sizeY );
	distributeFood( density );

}

/* destructor */

Habitat::~Habitat() {

}



/* member functions */

void Habitat::birth( Animat* ani ) {

	const char* name = ani->getName();
	population[name] = ani;

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



int Habitat::consumeFood( int x, int y ) {

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



Eigen::MatrixXd Habitat::getFoodReserve() {
	return foodReserve;
}





} /* namespace ecosystem */
