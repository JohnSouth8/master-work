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


//using namespace std;

using Eigen::VectorXf;
using Eigen::MatrixXf;

using util::coordinate;
using util::stimulus;

namespace ecosystem {

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

	float newX, newY, deltaX, deltaY;

	deltaX = velocity * cos( direction );
	deltaY = velocity * sin( direction );
	newX = posX + deltaX;
	newY = posY + deltaY;

	posX = util::getWrappedCoordinate( newX, environment->sizeX );
	posY = util::getWrappedCoordinate( newY, environment->sizeY );
	energy -= 1;	// TODO: energy loss should be proportional to action (so not int? or rather some linguistic classes i.e. 'some', 'a lot of' energy lost)

}


int Animat::eat() {

	int indexX = floor( posX );
	int indexY = floor( posY );

	float deltaE = environment->consumeFood( indexX, indexY );
	energy += deltaE;
	return deltaE;

}


int Animat::eat( int indexX, int indexY ) {

	float deltaE = environment->consumeFood( indexX, indexY );
	energy += deltaE;
	return deltaE;

}


void Animat::turn( float rads ) {

	direction += rads;
	direction = fmod( direction, 2*PI );
	// when angles exceed +- PI, they should be mirrored across the x-axis
	if ( direction > PI )
		direction = -2*PI + direction;
	if ( direction < -PI )
		direction = 2*PI + direction;

}



void Animat::senseOld() {

	int env_x = environment->sizeX;
	int env_y = environment->sizeY;

	// create a range for quad tree query
	int x_max = std::ceil( posX + visionRange );
	int y_max = std::ceil( posY + visionRange );
	int x_min = std::floor( posX - visionRange );
	int y_min = std::floor( posY - visionRange );

	coordinate rng0( x_min, y_min );
	coordinate rng1( x_max, y_max );
	coordinate lmts( env_x, env_y );

	std::vector<Organism*> grasses = environment->grassTree.rangeQuery( rng0, rng1, lmts );
	std::vector<Organism*> animats = environment->populationTree.rangeQuery( rng0, rng1, lmts );

	// TODO: paralelization of this part
	for ( auto gr : grasses ) {
		float dist = environment->distanceBetweenOrganisms( this, gr );
		if ( dist < visionRange )
			foodStimulus( { gr, dist } );
	}
	for ( auto an : animats ) {
		float dist = environment->distanceBetweenOrganisms( this, an );
		if ( dist < visionRange )
			kinStimulus( { an, dist } );		// TODO: separate for kin and foe, when foes are implemented
	}

	// TODO: is this necessary or just overhead?
	std::sort( sensedFood.begin(), sensedFood.end(), util::compareStimuli );
	std::sort( sensedKin.begin(), sensedKin.end(), util::compareStimuli );

	// TODO: prepare sensations vector

}



void Animat::senseFood() {

	int env_x = environment->sizeX;
	int env_y = environment->sizeY;

	// create a range for quad tree query
	int x_max = std::ceil( posX + visionRange );
	int y_max = std::ceil( posY + visionRange );
	int x_min = std::floor( posX - visionRange );
	int y_min = std::floor( posY - visionRange );

	coordinate rng0( x_min, y_min );
	coordinate rng1( x_max, y_max );
	coordinate lmts( env_x, env_y );

	std::vector<Organism*> foods = environment->grassTree.rangeQuery( rng0, rng1, lmts );

	for ( auto food : foods ) {
		float dist = environment->distanceBetweenOrganisms( this, food );
		if ( dist < visionRange )
			foodStimulus( { food, dist } );
	}

	// TODO: is this necessary or just overhead?
	std::sort( sensedFood.begin(), sensedFood.end(), util::compareStimuli );


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

	// TODO: split this function to provide better optimmization for multithreading

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

	forgetStimuli();
	senseFood();

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
	float dAngle = 0;//util::randFromUnitInterval() - 0.5;		// if no food, angle is random somewhere ahead
	if ( sensedFood.size() > 0 ) {

		min_dist = sensedFood[0].distance;
		int foodX = sensedFood[0].entity->posX;
		int foodY = sensedFood[0].entity->posY;

		float dX = foodX - posX;
		float dY = foodY - posY;

		// if food is over the edge
		if ( dX > visionRange ) {
			if ( foodX > posX )
				dX = foodX - (posX + environment->sizeX);
			else
				dX = (foodX + environment->sizeX) - posX;
		}
		if ( dY > visionRange ) {
			if ( foodY > posY )
				dY = foodY - (posY + environment->sizeY);
			else
				dY = (foodY + environment->sizeY) - posY;
		}

		// unit vector in animat's direction TODO: this is actually redundant, we already have angle in direction
		std::vector<float> v = {static_cast<float>( cos( direction ) ), static_cast<float>( sin( direction ) )};
		// vector from animat to food
		std::vector<float> g = {dX, dY};

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

	if ( isFood && fabs( dAngle ) < 0.05 )
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
		if ( sensedFood.size() > 0 && sensedFood[0].distance <= reach && eat( sensedFood[0].entity->posX, sensedFood[0].entity->posY ) != 0 ) return;
	}

	// only one turn action at a time: 		<< TODO: maybe both? opposing forces....
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



// deprecated, used before FCM
//void Animat::calculateDecision() {
//
//	// if there is food, eat it
//	int indexX = floor( posX );
//	int indexY = floor( posY );
//
//	if ( environment->grassTree.find( util::coordinate( indexX, indexY ) ) != nullptr ) {
//		eat();
//		return;
//	}
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
//	if ( index == -1 ) {
//
//		float randturn = util::randFromUnitInterval() * PI;
//		float randvel = util::randFromUnitInterval() * 5;
//
//		if ( velocity == 0 ) {
//			// turn in random direction and with random speed
//			turn( randturn );
//			setVelocity( randvel );
//		}
//		else {
//			// continue in this direction or turn with small probability
//			if ( util::randFromUnitInterval() < 0.2 )
//				turn ( randturn );
//
//			// change speed with small probability
//			if ( util::randFromUnitInterval() < 0.3 )
//				setVelocity( randvel );
//
//		}
//		return;
//	}
//
//
//	Vector2f v( cos( direction ), sin( direction ) );
//	Vector2f g( sensedObjs[index].x - posX, sensedObjs[index].y - posY );
//
//	// get absolute angles
//	turn( util::getAngleBetween( v, g ) );
//
//	setVelocity( g.norm() );
//
//	if ( velocity > maxVelocity )
//		setVelocity( maxVelocity );
//
//
//}



void Animat::forgetSensation() {
	sensations = Eigen::VectorXf::Zero( cognition.getNInput() );
}



void Animat::foodStimulus( stimulus s ) {
	sensedFood.push_back( s );
}



void Animat::kinStimulus( stimulus s ) {
	sensedKin.push_back( s );
}



void Animat::forgetStimuli() {
	sensedFood.clear();
	sensedKin.clear();
	sensedFoes.clear();
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



void Animat::initFCM( int nConcepts, std::string filename_cs, std::string filename_fcm ) {
	cognition = FCM( nConcepts );
	cognition.loadConceptsFromFile( filename_cs );
	cognition.loadLinkMatrixFromFile( filename_fcm );
	sensations = Eigen::VectorXf::Zero( cognition.getNInput() );
}



//void Animat::setFCM( MatrixXd newfcm ) {
//	cognition.setFCMap( newfcm );
//}



void Animat::toString() {

	std::cout << name << "; ";
	std::cout << posX << ", " << posY << "; ";
	std::cout << "a: " << direction << " rad; ";
	std::cout << std::endl;

}






} /* namespace ecosystem */
