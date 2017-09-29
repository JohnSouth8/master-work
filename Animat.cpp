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
using namespace Eigen;

namespace ecosystem {


Animat::Animat() {

	name = generateName();
	posX = 0.0;
	posY = 0.0;
	velocity = 0.0;
	direction = 0.0;
	energy = 0;
	maxEnergy = 100;
	senseRadius = 1;
	senseAngle = 2*M_PI;

}

// TODO: think about initializer lists
Animat::Animat( double px, double py, double v, double maxV, double dir, int e, double senseR, double senseA, Habitat* hab ) {

	name = generateName();
	posX = px;
	posY = py;
	velocity = v;
	maxVelocity = maxV;
	direction = dir;
	energy = e;
	maxEnergy = e;	// TODO: sort out
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



int Animat::getEnergy() {		// TODO maxEnergy attribute
	return energy;
}



void Animat::changeVelocity( double factor ) {

	if ( velocity == 0)
		velocity += factor*maxVelocity;
	else
		velocity += factor*velocity;

	if ( velocity > maxVelocity )
		velocity = maxVelocity;
	else if ( velocity < 0 )
		velocity = 0;

}



void Animat::changeVelocityAbsolute( double delta ) {
	velocity += delta;
}



void Animat::setVelocity( double v_new ) {
	velocity = v_new;
}



void Animat::move() {

	int env_x = environment->getXSize();
	int env_y = environment->getYSize();
	double newX, newY, deltaX, deltaY;

	deltaX = velocity * cos( direction );
	deltaY = velocity * sin( direction );
	newX = posX + deltaX;
	newY = posY + deltaY;

	posX = util::getWrappedCoordinate( newX, env_x );
	posY = util::getWrappedCoordinate( newY, env_y );
	energy -= 1;	// TODO: energy loss should be proportional to action (so not int? or rather some linguistic classes i.e. 'some', 'a lot of' energy lost)

}


int Animat::eat() {

	int indexX = floor( posX );
	int indexY = floor( posY );
	int deltaE;

	if ( environment->foodReserve( indexX, indexY ) != 0 ) {
		deltaE = environment->consumeFood( indexX, indexY );
		energy += deltaE;
		return deltaE;
	}

	return 0;

}


void Animat::turn( double rads ) {

	direction += rads;
	direction = fmod( direction, M_PI );

}



void Animat::sense_analytic() {


	MatrixXd foods = environment->getFoodReserve();
	int env_x = environment->getXSize();
	int env_y = environment->getYSize();

	// check all points in local (+- r squarely) neighbourhood if they are inside the circle
	int x_max = std::ceil( posX + senseRadius );
	int y_max = std::ceil( posY + senseRadius );
	int x_min = std::floor( posX - senseRadius );
	int y_min = std::floor( posY - senseRadius );

	int ix, iy;
	double dist, reach = pow( senseRadius, 2 );
	// TODO: do this with an Eigen block, not looping
	for ( int x = x_min; x <= x_max; ++x ) {
		for ( int y = y_min; y <= y_max; ++y ) {

			// correct indices for env's out of bounds
			ix = util::getWrappedIndex( x, env_x );
			iy = util::getWrappedIndex( y, env_y );

			if ( foods(ix, iy) != 0 ) {
				dist = pow( (x - posX), 2 ) + pow( (y - posY), 2 );
				if ( dist <= reach ) {
					addSensedObject( {
							ix,
							iy,
							sqrt( dist )
					} );
				}
			}

		}
	}

}



void Animat::sense() {

	forgetSensedObjects();
	sense_analytic();

	// plan to reach the closest food target
	std::vector<f_sens>::iterator it;
	int index = -1, counter = 0;
	double min_dist = environment->sizeX*10; // definitely farther than any point on map

	for ( it=sensedObjs.begin(); it != sensedObjs.end(); ++it ) {
		if ( it->d < min_dist ) {
			index = counter;
			min_dist = it->d;
		}
		++counter;
	}

	if ( index == -1 )
		min_dist = senseRadius;

	Eigen::Vector2d v( cos( direction ), sin( direction ) );
	Eigen::Vector2d g( sensedObjs[index].x - posX, sensedObjs[index].y - posY );
	double dAngle = util::getAngleBetween( v, g );

//	sensations(0) = 1 - 2*min_dist/senseRadius;
	if ( velocity == 0 )
		sensations(0) = 0; // TODO should be 1 when on a cell containing food
	else {
		double dv = min_dist / velocity;
		if ( dv > 2*maxVelocity )
			sensations(0) = -1;
		else
			sensations(0) = 1 - 2 * (dv / (2 * maxVelocity));
	}

	if ( dAngle > 0 )
		sensations(1) = dAngle / M_PI;

	if ( dAngle < 0 )
		sensations(2) = -dAngle / M_PI;

	sensations(3) = -1 + 2.0*energy / maxEnergy;

}



void Animat::reason() {

//	int nConcepts = cognition.getNConcepts();  // TODO nconcepts should be animat's value too... or should it?
//	int nSensual = cognition.getNInput();
//	int nPadding = nConcepts - nSensual;

	sense();

	cout << cognition.getState().transpose() << endl;

	cognition.applySensations( sensations );

	cout << cognition.getState().transpose() << endl;

	VectorXd motor = cognition.getOutput();

	react( motor );

}



void Animat::react( VectorXd motor ) {

	// defuzzify motor concepts' commands and apply them to the world

//	m0 turnLeft
//	m1 turnRight
//	m2 slowDown
//	m3 speedUp
//	m4 eat

	if ( motor(4) > 0.8 ) {
		// if eat action is successful (some energy is gained), the animat can no longer act
		if ( eat() != 0 ) return;
	}

	// only one turn action at a time: 		<< TODO: maybe both? opposing forces, ya'know....
	double ta;
	if ( motor(0) > motor(1) && motor(0) > 0.25 ) {
		ta = (motor(0) - 0.25) / 0.75;
		turn( ta );
	}
	else if ( motor(1) > motor(0) && motor(1) > 0.25 ) {
		ta = (motor(1) - 0.25) / 0.75;
		turn( -ta );
	}

	// speed change
	double dvf = 0.0;
	if ( motor(2) > 0.25 && motor(2) <= 0.75 ) {
		dvf -= (motor(2) - 0.25) / 0.5;
	}
	else if ( motor(2) > 0.75 ){
		dvf -= 1;
	}

	if ( motor(3) > 0.25 && motor(3) <= 0.75 ) {
		dvf += (motor(3) - 0.25) / 0.5;
	}
	else if ( motor(3) > 0.75 ) {
		dvf += 1;
	}
	changeVelocity( dvf );

	move();

}



void Animat::calculateDecision() {

	// if there is food, eat it
	int indexX = floor( posX );
	int indexY = floor( posY );

	if ( environment->foodReserve( indexX, indexY ) != 0 ) {
		eat();
		return;
	}

	// plan to reach the closest food target
	std::vector<f_sens>::iterator it;
	int index = -1, counter = 0;
	double min_dist = environment->sizeX*10; // definitely farther than any point on map

	for ( it=sensedObjs.begin(); it != sensedObjs.end(); ++it ) {
		if ( it->d < min_dist ) {
			index = counter;
			min_dist = it->d;
		}
		++counter;
	}

	if ( index == -1 ) {

		double randturn = util::randFromUnitInterval() * M_PI;
		double randvel = util::randFromUnitInterval() * 5;

		if ( velocity == 0 ) {
			// turn in random direction and with random speed
			turn( randturn );
			setVelocity( randvel );
		}
		else {
			// continue in this direction or turn with small probability
			if ( util::randFromUnitInterval() < 0.2 )
				turn ( randturn );

			// change speed with small probability
			if ( util::randFromUnitInterval() < 0.3 )
				setVelocity( randvel );

		}
		return;
	}


	Vector2d v( cos( direction ), sin( direction ) );
	Vector2d g( sensedObjs[index].x - posX, sensedObjs[index].y - posY );

	// get absolute angles
	turn( util::getAngleBetween( v, g ) );

	setVelocity( g.norm() );

	if ( velocity > 5 )		// TODO: add maxVelocity attribute
		setVelocity( 5 );


}



void Animat::forgetSensations() {
	sensations = Eigen::VectorXd::Zero( cognition.getNInput() );
}



void Animat::addSensedObject( f_sens loc ) {
	sensedObjs.push_back( loc );
}



void Animat::forgetSensedObjects() {
	sensedObjs.clear();
}



//void Animat::initFCM( int nConcepts ) {
//	cognition = FCM( nConcepts );
//}
//
//
//
//void Animat::initFCM( int nConcepts, std::vector<string> concepts ) {
//	cognition = FCM( nConcepts, concepts );
//}
//
//
//
//void Animat::initFCM( int nConcepts, std::vector<string> concepts, MatrixXd fcm ) {
//	cognition = FCM( nConcepts, concepts );
//	cognition.setFCMap( fcm );
//}



void Animat::initFCM( int nConcepts, string filename_cs, string filename_fcm ) {
	cognition = FCM( nConcepts );
	cognition.loadConceptsFromFile( filename_cs );
	cognition.loadLinkMatrixFromFile( filename_fcm );
	sensations = Eigen::VectorXd::Zero( cognition.getNInput() );
}



//void Animat::setFCM( MatrixXd newfcm ) {
//	cognition.setFCMap( newfcm );
//}



void Animat::toString() {

	cout << name << "; ";
	cout << posX << ", " << posY << "; ";
	cout << "a: " << direction << " rad; ";
	cout << endl;

}



void Animat::printSensedObjects() {

	vector<f_sens>::iterator it;

	for ( it = sensedObjs.begin(); it != sensedObjs.end(); ++it ) {
		cout << it->x << " " << it->y << "" << it->d << endl;
	}

}






} /* namespace ecosystem */
