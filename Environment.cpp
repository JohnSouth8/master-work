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

using namespace std;

namespace ecosystem {

/* constructors */

Environment::Environment() {

	sizeX = 1000;
	sizeY = 1000;
	population = map<const char*, Animat>();
	foodReserve = new bool[sizeX*sizeY] {false};
	distributeFood( 0.3 );

}

Environment::Environment( int sx, int sy ) {

	sizeX = sx;
	sizeY = sy;
	population = map<const char*, Animat>();
	foodReserve = new bool[sx*sy] {false};
	distributeFood( 0.3 );

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

	int n_cells = sizeX * sizeY;
	double fraction;

	for ( int i = 0; i < n_cells; i++ ) {

		fraction = double( rand() ) / double( RAND_MAX );
		if ( fraction < density )
			foodReserve[i] = true;
		else
			foodReserve[i] = false;

	}

}






} /* namespace ecosystem */
