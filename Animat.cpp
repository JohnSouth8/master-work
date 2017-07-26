/*
 * Animat.cpp
 *
 *  Created on: Jul 3, 2017
 *      Author: south
 */

#include <cstdlib>
#include <cmath>
#include <iostream>
#include "util.h"

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

	int charmin = 65, charmax = 90, randint;
	char* cname = new char[11];

	for ( int i = 0; i < 10; i++ ) {
		randint = util::randIntFrom( charmin, charmax );
		cname[i] = randint;
	}
	cname[10] = '\0';

	return cname;

}


const char* Animat::getName() {
	return name;
}



int Animat::getEnergy() {
	return energy;
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
	energy -= 1;

}


void Animat::turn( float rads ) {

	direction += rads;
	direction = fmod( direction, M_PI );

}



void Animat::makeDecision() {



}



void Animat::toString() {

	cout << name << endl;

}






} /* namespace ecosystem */
