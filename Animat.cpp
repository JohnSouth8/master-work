/*
 * Animat.cpp
 *
 *  Created on: Jul 3, 2017
 *      Author: south
 */

#include <cstdlib>
#include <cmath>
#include <iostream>

#include "Animat.h"

using namespace std;

namespace ecosystem {


Animat::Animat() {

	name = generateName();
	posX = 0.0;
	posY = 0.0;
	velocity = 0.0;
	direction = 0.0;
	energy = 0;

}

Animat::~Animat() {
	// TODO Auto-generated destructor stub
}



char* Animat::generateName() {

	int charmin = 65, nchar = 25, randint;
	char* cname = new char[10];

	for ( int i = 0; i < 10; i++ ) {
		randint = rand();
		double x = double( randint ) / double( RAND_MAX );
		randint = int( round( charmin + x * nchar ) );
		cname[i] = randint;
	}

	return cname;

}


const char* Animat::getName() {
	return name;
}



void Animat::toString() {

	cout << name << endl;

}






} /* namespace ecosystem */
