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



void Animat::setVelocity( float v_new ) {
	velocity = v_new;
}



void Animat::move() {

	int env_x = environment->getXSize();
	int env_y = environment->getYSize();
	float newX, newY, deltaX, deltaY;

	deltaX = velocity * cos( direction );
	deltaY = velocity * sin( direction );
	newX = posX + deltaX;
	newY = posY + deltaY;

	posX = util::getWrappedCoordinate( newX, env_x );
	posY = util::getWrappedCoordinate( newY, env_y );
	energy -= 1;

}


void Animat::eat() {

	int indexX = floor( posX );
	int indexY = floor( posY );

	if ( environment->foodReserve( indexX, indexY ) != 0 ) {
		energy += environment->consumeFood( indexX, indexY );
//		cout << "I ate!" << endl;
	}

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
	int x_min = std::floor( posX - senseRadius );
	int y_min = std::floor( posY - senseRadius );

	int ix, iy;
	float dist, reach = pow( senseRadius, 2 );

	for ( int x = x_min; x <= x_max; ++x ) {
		for ( int y = y_min; y <= y_max; ++y ) {

			// correct indices for env's out of bounds
			ix = util::getWrappedIndex( x, env_x );
			iy = util::getWrappedIndex( y, env_y );

			if ( foods(ix, iy) != 0 ) {
				dist = pow( (x - posX), 2 ) + pow( (y - posY), 2 );
				if ( dist <= reach ) {
					addSensation( {
							ix,
							iy,
							sqrt( dist )
					} );
				}
			}

		}
	}

}



void Animat::makeDecision() {

	// if there is food, eat it
	int indexX = floor( posX );
	int indexY = floor( posY );

	if ( environment->foodReserve( indexX, indexY ) != 0 ) {
		eat();
		return;
	}

	// plan to reach the closest food target
	vector<f_sens>::iterator it;
	int index = -1, counter = 0;
	float min_dist = environment->sizeX*10; // definitely farther than any point on map

	for ( it=sensedObjs.begin(); it != sensedObjs.end(); ++it ) {
		if ( it->d < min_dist ) {
			index = counter;
			min_dist = it->d;
		}
		++counter;
	}

	if ( index == -1 ) {
		//turn in random direction and with random speed
		double randturn = util::randFromUnitInterval() * M_PI;
		double randvel = util::randFromUnitInterval() * 5;
		turn( randturn );
		setVelocity( randvel );
		return;
	}


	Eigen::Vector2f v( cos( direction ), sin( direction ) );
	Eigen::Vector2f g( sensedObjs[index].x - posX, sensedObjs[index].y - posY );

//	float v_norm = v.norm(), g_norm = g.norm(), turn_angle = 0;
//	if ( v_norm != 0 && g_norm != 0 ) turn_angle = acos( v.dot( g )/(v_norm*g_norm) );

	// get absolute angles
	float v_angle = atan2( v(1), v(0) );
	float g_angle = atan2( g(1), g(0) );

	float turn_angle = g_angle - v_angle;

	turn(turn_angle);
	setVelocity( g.norm() );

	if ( velocity > 5 )		// TODO: add maxVelocity attribute
		setVelocity( 5 );


}



void Animat::addSensation( f_sens loc ) {
	sensedObjs.push_back( loc );
}



void Animat::forgetSensations() {
	sensedObjs.clear();
}



void Animat::toString() {

	cout << name << "; ";
	cout << posX << ", " << posY << "; ";
	cout << "a: " << direction << " rad; ";
	cout << endl;

}



void Animat::printSensations() {

	vector<f_sens>::iterator it;

	for ( it = sensedObjs.begin(); it != sensedObjs.end(); ++it ) {
		cout << it->x << " " << it->y << "" << it->d << endl;
	}

}






} /* namespace ecosystem */
