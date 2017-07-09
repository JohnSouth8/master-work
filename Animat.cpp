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

Animat::Animat( float px, float py, float v, float dir, int e ) {

	name = generateName();
	posX = px;
	posY = py;
	velocity = v;
	direction = dir;
	energy = e;

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



void Animat::changeVelocity( float delta ) {
	velocity += delta;
}


void Animat::move() {

	float newX, newY, deltaX, deltaY;
	deltaX = velocity * sin( direction );
	deltaY = velocity * cos( direction );
	newX = posX + deltaX;
	newY = posY + deltaY;
	posX = newX;
	posY = newY;

}


void Animat::turn( float rads ) {

	direction += rads;
	direction = fmod( direction, M_PI );

}



void Animat::toString() {

	cout << name << endl;

}






} /* namespace ecosystem */
