/*
 * Environment.cpp
 *
 *  Created on: Jul 4, 2017
 *      Author: south
 */

#include "Environment.h"

#include <cstdlib>
#include <iostream>
#include <map>
#include <Eigen/Dense>
#include "util.h"

using namespace std;
using namespace Eigen;

namespace ecosystem {

/* constructors */

Environment::Environment() {

	sizeX = 1000;
	sizeY = 1000;
//	population = map<const char*, Animat>();
	foodReserve = MatrixXf( sizeX, sizeY );
	distributeFood( 0.5 );

}

Environment::Environment( int sx, int sy, float density ) {

	sizeX = sx;
	sizeY = sy;
//	population = map<const char*, Animat>();
	foodReserve = MatrixXf( sizeX, sizeY );
	distributeFood( density );

}

/* destructor */

Environment::~Environment() {

}



/* member functions */

void Environment::birth( Animat ani ) {

	const char* name = ani.getName();
	population[name] = ani;

}



void Environment::distributeFood( float density ) {

	double fraction;

	for ( int x = 0; x < sizeX; x++ ) {
		for ( int y = 0; y < sizeY; y++ ) {

			fraction = util::randFromUnitInterval();
			if ( fraction < density ) {
				foodReserve(x,y) = 1.0;
			}

		}
	}

}


Eigen::MatrixXf Environment::getFoodReserve() {
	return foodReserve;
}





} /* namespace ecosystem */
