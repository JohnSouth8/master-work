/*
 * Animat.cpp
 *
 *  Created on: Jul 3, 2017
 *      Author: south
 */

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <Eigen/Dense>
#include "util.h"

#include "Animat.h"
#include "Habitat.h"


using namespace std;

namespace ecosystem {


Animat::Animat() {

	name = generateName();
	posX = 0.0;
	posY = 0.0;
	velocity = 0.0;
	direction = 0.0;
	energy = 0;
	senseRadius = 1;
	senseAngle = 2*M_PI;

}

Animat::Animat( float px, float py, float v, float dir, int e, float senseR, float senseA, Habitat* hab ) {

	name = generateName();
	posX = px;
	posY = py;
	velocity = v;
	direction = dir;
	energy = e;
	senseRadius = senseR;
	senseAngle = senseA;
	environment = hab;

}

Animat::~Animat() {
	// TODO Auto-generated destructor stub
}



char* Animat::generateName() {

	int charmin = 65, charmax = 90, randint;
	char* cname = new char[11];

	for ( int i = 0; i < 10; ++i ) {
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



void Animat::sense() {


	Eigen::MatrixXf foods = environment->getFoodReserve();
	int env_x = environment->getXSize();
	int env_y = environment->getYSize();

	// check all points in local (+- r squarely) neighbourhood if they are inside the circle
	int x_max = std::ceil( posX + senseRadius );
	int y_max = std::ceil( posY + senseRadius );
	int x_min = std::ceil( posX - senseRadius );
	int y_min = std::ceil( posY - senseRadius );


	for ( int x = x_min; x <= x_max; ++x ) {
		for ( int y = y_min; y <= y_max; ++y ) {

			// correct indices for env's out of bounds
			int ix = util::getWrappedIndex( x, env_x );
			int iy = util::getWrappedIndex( y, env_y );

			if ( foods(ix, iy) != 0 ) {
				if ( pow( (x - posX), 2 ) + pow( (y - posY), 2 ) <= pow( senseRadius, 2 ) ) {
					coord co = { ix, iy };
					addSensation( co );
				}
			}

		}
	}

}



void Animat::makeDecision() {



}



void Animat::addSensation( coord loc ) {
	sensedObjs.push_back( loc );
}



void Animat::toString() {

	cout << name << "; ";
	cout << posX << ", " << posY << "; ";
	cout << "a: " << direction << " rad; ";
	cout << endl;

}



void Animat::printSensations() {

	vector<coord>::iterator it;

	for ( it = sensedObjs.begin(); it != sensedObjs.end(); ++it ) {
		cout << it->x << " " << it->y << endl;
	}

}






} /* namespace ecosystem */
