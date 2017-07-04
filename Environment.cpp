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

}

Environment::Environment( int sx, int sy ) {

	sizeX = sx;
	sizeY = sy;
	population = map<const char*, Animat>();

}

/* destructor */

Environment::~Environment() {

}



/* member functions */

void Environment::birth( Animat ani ) {

	const char* name = ani.getName();
	population[name] = ani;

}










} /* namespace ecosystem */
