/*
 * Animat.cpp
 *
 *  Created on: Jul 3, 2017
 *      Author: south
 */

#include <cstdlib>
#include <string>
#include <cmath>
#include <iostream>
#include <Eigen/Dense>
#include "util.h"

#include "Animat.h"
#include "Habitat.h"


using namespace std;
using namespace Eigen;

namespace ecosystem {


//Animat::Animat() {
//
//	name = generateName();
//	posX = 0.0;
//	posY = 0.0;
//	velocity = 0.0;
//	direction = 0.0;
//	energy = 0;
//	maxEnergy = 100;
//	senseRadius = 1;
//	senseAngle = 2*M_PI;
//
//}


//Animat::Animat( float px, float py, float v, float maxV, float dir, float e, float senseR, float senseA, float rch, Habitat* hab ) {
//
////	name = generateName();
//	posX = px;
//	posY = py;
//	velocity = v;
//	maxVelocity = maxV;
//	direction = dir;
//	energy = e;
//	maxEnergy = e;	// TODO: sort out
//	senseRadius = senseR;
//	senseAngle = senseA;
//	reach = rch;
//	environment = hab;
//
//}

Animat::Animat( std::string nm, float sz, float r_v, float a_v, float r_o, float max_v, float max_e, int max_a, float px, float py, float dir, float v, float e, Habitat* env ) :
	Organism( px, py ),
	name( nm ),
	size( sz ),
	reach( 2*sz ),
	visionRange( r_v ),
	visionAngle( a_v ),
	olfactoryRange( r_o ),
	maxVelocity( max_v ),
	maxEnergy( max_e ),
	maxAge( max_a ),
	direction( dir ),
	velocity( v ),
	age( 0 ),
	energy( e ),
	comfort( 50 ),
	fatigue( 0 ),
	environment( env )
{}

Animat::~Animat() {

}



void Animat::adjustVelocity( float factor ) {

	if ( velocity == 0)
		velocity += factor*maxVelocity;
	else
		velocity += factor*velocity;

	if ( velocity > maxVelocity )
		velocity = maxVelocity;
	else if ( velocity < 0 )
		velocity = 0;

}



void Animat::changeVelocityAbsolute( float delta ) {
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
	energy -= 1;	// TODO: energy loss should be proportional to action (so not int? or rather some linguistic classes i.e. 'some', 'a lot of' energy lost)

}


int Animat::eat() {

	int indexX = floor( posX );
	int indexY = floor( posY );

	if ( environment->foodReserve( indexX, indexY ) != 0 ) {
		float deltaE = environment->consumeFood( indexX, indexY );
		energy += deltaE;
		return deltaE;
	}

	return 0;

}


int Animat::eat( int indexX, int indexY ) {

	if ( environment->foodReserve( indexX, indexY ) != 0 ) {
		float deltaE = environment->consumeFood( indexX, indexY );
		energy += deltaE;
		return deltaE;
	}

	return 0;

}


void Animat::turn( float rads ) {

	direction += rads;
	direction = fmod( direction, 2*PI );
	// when angles exceed +- PI, they should be mirrored across the x-axis
	if ( direction > PI )
		direction = -2*PI - direction;
	if ( direction < -PI )
		direction = 2*PI + direction;

}



void Animat::sense_analytic() {


	MatrixXf foods = environment->getFoodReserve();
	int env_x = environment->getXSize();
	int env_y = environment->getYSize();

	// check all points in local (+- r squarely) neighbourhood if they are inside the circle
	int x_max = std::ceil( posX + visionRange );
	int y_max = std::ceil( posY + visionRange );
	int x_min = std::floor( posX - visionRange );
	int y_min = std::floor( posY - visionRange );

	float sense_sq = pow( visionRange, 2 );
	// TODO: do this with an Eigen block, not looping
	for ( int x = x_min; x <= x_max; ++x ) {
		for ( int y = y_min; y <= y_max; ++y ) {

			// correct indices for env's out of bounds
			int ix = util::getWrappedIndex( x, env_x );
			int iy = util::getWrappedIndex( y, env_y );

			if ( foods(ix, iy) != 0 ) {
				float dist = pow( (x - posX), 2 ) + pow( (y - posY), 2 );
				if ( dist <= sense_sq ) {
					addSensedObject( {
							ix,
							iy,
							sqrt( dist )
					} );
				}
			}

		}
	}

	std::sort( sensedObjs.begin(), sensedObjs.end(), util::compareFoodSensations );

}


// for FCM version 0.1
//void Animat::sense() {
//
//	forgetSensedObjects();
//	sense_analytic();
//
//	// plan to reach the closest food target
//	std::vector<f_sens>::iterator it;
//	int index = -1, counter = 0;
//	double min_dist = environment->sizeX*10; // definitely farther than any point on map
//
//	for ( it=sensedObjs.begin(); it != sensedObjs.end(); ++it ) {
//		if ( it->d < min_dist ) {
//			index = counter;
//			min_dist = it->d;
//		}
//		++counter;
//	}
//
//	if ( index == -1 )
//		min_dist = senseRadius;
//
//	Eigen::Vector2d v( cos( direction ), sin( direction ) );
//	Eigen::Vector2d g( sensedObjs[index].x - posX, sensedObjs[index].y - posY );
//	double dAngle = util::getAngleBetween( v, g );
//
//	// TODO: these things should be in own functions for clarity and documentation
////	sensations(0) = 1 - 2*min_dist/senseRadius;
//	if ( velocity == 0 )
//		sensations(0) = 0; //  should be 1 when on a cell containing food
//	else {
//		double dv = min_dist / velocity;
//		if ( dv > 2*maxVelocity )
//			sensations(0) = -1;
//		else
//			sensations(0) = 1 - 2 * (dv / (2 * maxVelocity));
//	}
//
//	if ( dAngle > 0 )
//		sensations(1) = dAngle / M_PI;
//
//	if ( dAngle < 0 )
//		sensations(2) = -dAngle / M_PI;
//
//	sensations(3) = -1 + 2.0*energy / maxEnergy;
//
//}



void Animat::reason() {

//	int nConcepts = cognition.getNConcepts();  // TODO nconcepts should be animat's value too... or should it?
//	int nSensual = cognition.getNInput();
//	int nPadding = nConcepts - nSensual;

	sense();

//	cout << cognition.getState().transpose() << endl;

	cognition.applySensations( sensations );

//	cout << cognition.getState().transpose() << endl;

	VectorXf motor = cognition.getOutput();

	react( motor );

}



void Animat::sense() {

	forgetSensedObjects();
	sense_analytic();

	// plan to reach the closest food target
//	std::vector<f_sens>::iterator it;
//	int index = -1, counter = 0;
//	double min_dist = environment->sizeX*10; // definitely farther than any point on map

//	for ( it=sensedObjs.begin(); it != sensedObjs.end(); ++it ) {
//		if ( it->d < min_dist ) {
//			index = counter;
//			min_dist = it->d;
//		}
//		++counter;
//	}
//
//	if ( index == -1 )
//		min_dist = senseRadius;

	bool isFood = false;
	float min_dist = visionRange;
	float dAngle = util::randFromUnitInterval() - 0.5;		// if no food, angle is random somewhere ahead
	if ( sensedObjs.size() > 0 ) {

		min_dist = sensedObjs[0].d;
		int foodX = sensedObjs[0].x;
		int foodY = sensedObjs[0].y;

		float dX = foodX - posX;
		float dY = foodY - posY;

		// if food is over the edge
		if ( dX > visionRange ) {
			if ( foodX > posX )
				dX = foodX - (posX + environment->getXSize());
			else
				dX = (foodX + environment->getXSize()) - posX;
		}
		if ( dY > visionRange ) {
			if ( foodY > posY )
				dY = foodY - (posY + environment->getYSize());
			else
				dY = (foodY + environment->getYSize()) - posY;
		}

		// unit vector in animat's direction
		Eigen::Vector2f v( cos( direction ), sin( direction ) );
		// vector from animat to food
		Eigen::Vector2f g( dX, dY );

		dAngle = util::getAngleBetween( v, g );
		// when angles are more or less to the left of the vertical, atan2 returns large values which
		//  lead to turning in the wrong direction (over the x-axis at 0rad) rather than shorter turns
		//  (over the x-axis at PIrad), thus those values are added to the opposite-signed full circle
		if ( dAngle > PI )
			dAngle = -2*PI + dAngle;
		if ( dAngle < -PI )
			dAngle = 2*PI + dAngle;

		isFood = true;

//		std::cout << "closest food distance: " << min_dist << ", delta angle: " << dAngle << std::endl;
//		std::cout << "animat vector:\n" << v << "\nfood vector:\n" << g << std::endl;
//		std::cout << "######################" << std::endl;
	}


	// sensations
	// s0 fInReach
	// s1 fClose
	// s2 fLeft
	// s3 fRight
	// s4 fForward
	// s5 speed


	// TODO: these things should be in own functions for clarity and documentation
//	sensations(0) = 1 - 2*min_dist/senseRadius;

	if ( min_dist <= reach )
		sensations(0) = 1;
	else
		sensations(0) = 0;

	double dv = min_dist / velocity;
	if ( dv > 3*maxVelocity )
		sensations(1) = -1;
	else
		sensations(1) = 1 - 2 * (dv / (3 * maxVelocity));


	if ( dAngle > 0 )
		sensations(2) = dAngle / PI;
	else
		sensations(2) = 0;

	if ( dAngle < 0 )
		sensations(3) = -dAngle / PI;
	else
		sensations(3) = 0;

//	sensations(3) = -1 + 2.0*energy / maxEnergy;

	if ( isFood && abs( dAngle ) < 0.05 )
		sensations(4) = 1;
	else
		sensations(4) = 0;

	// speed
	float speed = -1 + 2 * ( velocity / maxVelocity );
	sensations(5) = speed;

}



// for FCM v0.2
void Animat::react( VectorXf motor ) {

	// defuzzify motor concepts' commands and apply them to the world

//	m0 eat
//	m1 turnLeft
//	m2 turnRight
//	m3 adjustVelocity

	if ( motor(0) > 0.5 ) {
		// if eat action is successful (some energy is gained), the animat can no longer act
		if ( sensedObjs.size() > 0 && sensedObjs[0].d <= reach && eat( sensedObjs[0].x, sensedObjs[0].y ) != 0 ) return;
	}

	// only one turn action at a time: 		<< TODO: maybe both? opposing forces, ya'know....
	if ( motor(1) > motor(2) && motor(1) > 0.15 ) {
		float ta = (motor(1) - 0.15) / 0.75;
		turn( ta );
	}
	else if ( motor(2) > motor(1) && motor(2) > 0.15 ) {
		float ta = (motor(2) - 0.15) / 0.75;
		turn( -ta );
	}

	// speed change
	adjustVelocity( motor(3) );

	move();

}


// for FCM v0.1
//void Animat::react( VectorXd motor ) {
//
//	// defuzzify motor concepts' commands and apply them to the world
//
////	m0 turnLeft
////	m1 turnRight
////	m2 slowDown
////	m3 speedUp
////	m4 eat
//
//	if ( motor(4) > 0.8 ) {
//		// if eat action is successful (some energy is gained), the animat can no longer act
//		if ( eat() != 0 ) return;
//	}
//
//	// only one turn action at a time: 		<< TODO: maybe both? opposing forces, ya'know....
//	if ( motor(0) > motor(1) && motor(0) > 0.25 ) {
//		double ta = (motor(0) - 0.25) / 0.75;
//		turn( ta );
//	}
//	else if ( motor(1) > motor(0) && motor(1) > 0.25 ) {
//		double ta = (motor(1) - 0.25) / 0.75;
//		turn( -ta );
//	}
//
//	// speed change
//	double dvf = 0.0;
//	if ( motor(2) > 0.25 && motor(2) <= 0.75 ) {
//		dvf -= (motor(2) - 0.25) / 0.5;
//	}
//	else if ( motor(2) > 0.75 ){
//		dvf -= 1;
//	}
//
//	if ( motor(3) > 0.25 && motor(3) <= 0.75 ) {
//		dvf += (motor(3) - 0.25) / 0.5;
//	}
//	else if ( motor(3) > 0.75 ) {
//		dvf += 1;
//	}
//	changeVelocity( dvf );
//
//	move();
//
//}



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

		float randturn = util::randFromUnitInterval() * PI;
		float randvel = util::randFromUnitInterval() * 5;

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


	Vector2f v( cos( direction ), sin( direction ) );
	Vector2f g( sensedObjs[index].x - posX, sensedObjs[index].y - posY );

	// get absolute angles
	turn( util::getAngleBetween( v, g ) );

	setVelocity( g.norm() );

	if ( velocity > maxVelocity )
		setVelocity( maxVelocity );


}



void Animat::forgetSensations() {
	sensations = Eigen::VectorXf::Zero( cognition.getNInput() );
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
	sensations = Eigen::VectorXf::Zero( cognition.getNInput() );
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
