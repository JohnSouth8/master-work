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
using std::string;

using util::coordinate;
using util::stimulus;

namespace ecosystem {


Animat::Animat( string nm, std::vector<float> gnm, float px, float py, float dir ) :
	Organism( px, py ),
	direction( dir ),
	velocity( 0 ),
	age( 0 ),
	fatigue( 0 ),
	comfort( 50 )
{

	name = nm;
	genome = gnm;
	cognition = FCM();
	sensation = Eigen::VectorXf::Zero( cognition.nSensory );

	formPhenotype();

	// debug only!
//	util::printMatrixToFile( cognition.L, "debug_output/"+name+"_L.txt", false );

	age = 0;
	// TODO temporarirly removed
//	energy = maxEnergy / 2;
	energy = 0;
	comfort = 50;
	fatigue = 0;
	direction = dir;
	velocity = 0;

}

// deprecated
Animat::Animat( string nm, float sz, float max_v, float max_e, int max_a, float r_v, float e_a, float e_fov, float r_o, float px, float py, float dir, float v, float e ) :
	Organism( px, py ),
	name( nm ),
	size( sz ),
	reach( 2*sz ),
	maxEnergy( max_e ),
	maxVelocity( max_v ),
	maxAge( max_a ),
	visionRange( r_v ),
	eyeOffsetAngle( e_a ),
	eyeFieldOfView( e_fov ),
	olfactoryRange( r_o ),
	direction( dir ),
	velocity( v ),
	age( 0 ),
	energy( e ),
	fatigue( 0 ),
	comfort( 50 )
{
	cognition = FCM();
	sensation = Eigen::VectorXf::Zero( cognition.nSensory );
}

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

	float oldX, oldY, newX, newY, deltaX, deltaY;

	oldX = posX;
	oldY = posY;
	deltaX = velocity * cos( direction );
	deltaY = velocity * sin( direction );
	newX = oldX + deltaX;
	newY = oldY + deltaY;


	// Animats should move within the QuadTree as well... remove the animat at this location and insert it again
	// TODO: it should be ensured that absolutely no 2 animats share the exact same position

	HABITAT->populationTree.remove( this );


	posX = util::getWrappedCoordinate( newX, HABITAT->sizeX );
	posY = util::getWrappedCoordinate( newY, HABITAT->sizeY );

	// currently energy is of minor importance
//	energy -= 1;	// TODO: energy loss should be proportional to action (so not int? or rather some linguistic classes i.e. 'some', 'a lot of' energy lost)
					//  also, if no movement is performed, very little energy should be subtracted


	HABITAT->populationTree.insert( this );

}


int Animat::eat() {

	int indexX = floor( posX );
	int indexY = floor( posY );

	float deltaE = HABITAT->consumeFood( indexX, indexY );
	energy += deltaE;
	return deltaE;

}


int Animat::eat( int indexX, int indexY ) {

	float deltaE = HABITAT->consumeFood( indexX, indexY );
	energy += deltaE;
	return deltaE;

}


void Animat::turn( float rads ) {

	// can't turn more than 180 degrees in one turn
	if ( rads > PI )
		rads = PI;
	if ( rads < -PI )
		rads = -PI;

	direction += rads + 2*PI; 	// ensure positive values by adding a full circle
	direction = fmod( direction, 2*PI );
	// TODO: revise all changes to angles! This should work as intended!
	// when angles exceed +- PI, they should be mirrored across the x-axis. Or should they?
//	if ( direction > PI )
//		direction = -2*PI + direction;
//	if ( direction < -PI )
//		direction = 2*PI + direction;

}



void Animat::procreate() {

	// look for appropriate mate
	for ( auto mates : nearbyKin ) {
		if ( mates.distance <= reach ) {
			Animat* mate = static_cast<Animat*>( mates.entity );
			if ( court( mate ) )
				HABITAT->breed( this, mate, true );
		}
	}


}



bool Animat::court( Animat* mate ) {

	return mate->matingResponse();

}



bool Animat::matingResponse() {

	// TODO: complicate with pickiness - depends on who courts
	if ( cognition.state( cognition.concepts["m_mate"] ) > 0.5 )
		return true;

	return false;

}



void Animat::sense() {

	// first clear previous sensations
	forgetSensation();
	forgetStimuli();

	// create a range for quad tree query
	int env_x = HABITAT->sizeX,
		env_y = HABITAT->sizeY;
	int x_max = util::getWrappedIndex( std::ceil( posX + visionRange ), env_x ),
		y_max = util::getWrappedIndex( std::ceil( posY + visionRange ), env_y ),
		x_min = util::getWrappedIndex( std::floor( posX - visionRange ), env_x ),
		y_min = util::getWrappedIndex( std::floor( posY - visionRange ), env_y );


	coordinate rng0( x_min, y_min );
	coordinate rng1( x_max, y_max );
	coordinate lmts( env_x, env_y );

	std::vector<Organism*> grasses = HABITAT->grassTree.rangeQuery( rng0, rng1, lmts );
	std::vector<Organism*> animats = HABITAT->populationTree.rangeQuery( rng0, rng1, lmts );


	// analyze nearby organisms
	// TODO: parallelization of this part
	for ( auto grs : grasses ) {
		float dist = HABITAT->distanceBetweenOrganisms( this, grs );
		if ( dist < visionRange ) {
			float angle = util::getStimulusAngle( this, grs );
			nearbyFood.push_back( {grs, 0, dist, angle} );
		}
	}
	for ( auto ani : animats ) {
		if ( name == static_cast<Animat*>(ani)->name )
			continue;
		float dist = HABITAT->distanceBetweenOrganisms( this, ani );
		if ( dist < visionRange ) {
			float angle = util::getStimulusAngle( this, ani );
			nearbyKin.push_back( {ani, 1, dist, angle} );		// TODO: separate for kin and foe, when foes are implemented
		}
	}

	float foodActivation = 0,		// olfactory
		kinActivation = 0,			// olfactory
		foodActivation_l = 0,		// visual
		kinActivation_l = 0,		// visual
		foodActivation_r = 0,		// visual
		kinActivation_r = 0,		// visual
		pain = 0;					// proprioceptive

	int foodInReach = 0,
		kinInReach = 0;

	// angles
	float minLimit_l = eyeOffsetAngle - eyeFieldOfView / 2.0,
		maxLimit_l = eyeOffsetAngle + eyeFieldOfView / 2.0,
		minLimit_r = -eyeOffsetAngle - eyeFieldOfView / 2.0,
		maxLimit_r = -eyeOffsetAngle + eyeFieldOfView / 2.0;

	// TODO: parallelization of this part
	for ( auto nfd : nearbyFood ) {

		// visual input
		if ( minLimit_l <= nfd.angle && nfd.angle <= maxLimit_l )
			foodActivation_l += util::stimulusVisualActivation( nfd.angle, nfd.distance );
		if ( minLimit_r <= nfd.angle && nfd.angle <= maxLimit_r )
			foodActivation_r += util::stimulusVisualActivation( nfd.angle, nfd.distance );

		// olfactory input
		if ( nfd.distance <= olfactoryRange )
			foodActivation += util::stimulusOlfactoryActivation( nfd.distance );

		// affordance
		if ( nfd.distance <= reach )
			foodInReach = 1;

	}

	for ( auto nkn : nearbyKin ) {

		// visual input
		if ( minLimit_l <= nkn.angle && nkn.angle <= maxLimit_l )
			kinActivation_l += util::stimulusVisualActivation( nkn.angle, nkn.distance );
		if ( minLimit_r <= nkn.angle && nkn.angle <= maxLimit_r )
			kinActivation_r += util::stimulusVisualActivation( nkn.angle, nkn.distance );

		// olfactory input
		if ( nkn.distance <= olfactoryRange )
			kinActivation += util::stimulusOlfactoryActivation( nkn.distance );

		// affordance
		if ( nkn.distance <= reach )
			kinInReach = 1;

		// tactile input
		Animat* an = static_cast<Animat*>(nkn.entity);
		if ( nkn.distance <= size + an->size ) {
			pain = (nkn.angle / PI) * velocity * an->velocity;
		}

	}

	comfort -= pain;

	// proprioceptive sensation. BUT! this should be somehow done by a drive function...
	float speedActivation = velocity / maxVelocity;
	float energyActivation = energy / maxEnergy;
	float fatigueActivation = fatigue / 100;
	float comfortActivation = comfort / 100;




	// add activations to sensation
	sensation(cognition.concepts["s_foodNearby"]) 	+= foodActivation;
//	sensation(cognition.concepts["s_kinNearby"]) 	+= kinActivation;
	sensation(cognition.concepts["s_foodInReach"])	+= foodInReach;
//	sensation(cognition.concepts["s_kinInReach"]) 	+= kinInReach;
	sensation(cognition.concepts["s_foodLeft"]) 	+= foodActivation_l;
	sensation(cognition.concepts["s_foodRight"]) 	+= foodActivation_r;
//	sensation(cognition.concepts["s_kinLeft"]) 		+= kinActivation_l;
//	sensation(cognition.concepts["s_kinRight"]) 	+= kinActivation_r;
	sensation(cognition.concepts["s_speed"])		+= speedActivation;
//	sensation(cognition.concepts["s_energy"])		+= energyActivation;
//	sensation(cognition.concepts["s_fatigue"])		+= fatigueActivation;
//	sensation(cognition.concepts["s_comfort"])		+= comfortActivation;


}



// in deprecation
void Animat::senseFood() {

	int env_x = HABITAT->sizeX;
	int env_y = HABITAT->sizeY;

	// create a range for quad tree query
	int x_max = util::getWrappedIndex( std::ceil( posX + visionRange ), env_x );
	int y_max = util::getWrappedIndex( std::ceil( posY + visionRange ), env_y );
	int x_min = util::getWrappedIndex( std::floor( posX - visionRange ), env_x );
	int y_min = util::getWrappedIndex( std::floor( posY - visionRange ), env_y );

	coordinate rng0( x_min, y_min );
	coordinate rng1( x_max, y_max );
	coordinate lmts( env_x, env_y );

	std::vector<Organism*> foods = HABITAT->grassTree.rangeQuery( rng0, rng1, lmts );

//	for ( auto food : foods ) {
//		float dist = environment->distanceBetweenOrganisms( this, food );
//		if ( dist < visionRange )
//			foodStimulus( { food, dist, 0.0 } );
//	}

	// TODO: is this necessary or just overhead?
	std::sort( nearbyFood.begin(), nearbyFood.end(), util::compareStimuli );


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

	cognition.applySensations( sensation );

//	cout << cognition.getState().transpose() << endl;

	VectorXf motor = cognition.getOutput();

	react( motor );

}



void Animat::senseOld() {

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
	if ( nearbyFood.size() > 0 ) {

		min_dist = nearbyFood[0].distance;
		int foodX = nearbyFood[0].entity->posX;
		int foodY = nearbyFood[0].entity->posY;

		float dX = foodX - posX;
		float dY = foodY - posY;

		// if food is over the edge
		if ( dX > visionRange ) {
			if ( foodX > posX )
				dX = foodX - (posX + HABITAT->sizeX);
			else
				dX = (foodX + HABITAT->sizeX) - posX;
		}
		if ( dY > visionRange ) {
			if ( foodY > posY )
				dY = foodY - (posY + HABITAT->sizeY);
			else
				dY = (foodY + HABITAT->sizeY) - posY;
		}

		// unit vector in animat's direction TODO: this is actually redundant, we already have angle in direction
		std::vector<float> v = {static_cast<float>( cos( direction ) ), static_cast<float>( sin( direction ) )};
		// vector from animat to food
		std::vector<float> g = {dX, dY};

		dAngle = util::getAngleBetweenVectors( v, g );
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
		sensation(0) = 1;
	else
		sensation(0) = 0;

	double dv = min_dist / velocity;
	if ( dv > 3*maxVelocity )
		sensation(1) = -1;
	else
		sensation(1) = 1 - 2 * (dv / (3 * maxVelocity));


	if ( dAngle > 0 )
		sensation(2) = dAngle / PI;
	else
		sensation(2) = 0;

	if ( dAngle < 0 )
		sensation(3) = -dAngle / PI;
	else
		sensation(3) = 0;

//	sensations(3) = -1 + 2.0*energy / maxEnergy;

	if ( isFood && fabs( dAngle ) < 0.05 )
		sensation(4) = 1;
	else
		sensation(4) = 0;

	// speed
	float speed = -1 + 2 * ( velocity / maxVelocity );
	sensation(5) = speed;

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
		if ( nearbyFood.size() > 0 && nearbyFood[0].distance <= reach && eat( nearbyFood[0].entity->posX, nearbyFood[0].entity->posY ) != 0 ) {
//			std::cout << "I, " << name << ", ate!" << std::endl;
			return;
		}
	}

	// both turns add up, to represent opposing forces
	float ta = 0;
	if ( motor(1) > motor(2) && motor(1) > 0.15 )
		ta += (motor(1) - 0.15) / 0.75;
	if ( motor(2) > motor(1) && motor(2) > 0.15 )
		ta -= (motor(2) - 0.15) / 0.75;
	turn( ta );

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
	sensation = Eigen::VectorXf::Zero( cognition.nSensory );
}


void Animat::forgetStimuli() {
	nearbyFood.clear();
	nearbyKin.clear();
	nearbyFoes.clear();
}



void Animat::formPhenotype() {

	int gene = 0;

	size 			= genome[gene++];
	reach			= genome[gene++];
	maxEnergy 		= genome[gene++];
	maxVelocity 	= genome[gene++];
	maxAge 			= genome[gene++];
	visionRange  	= genome[gene++];
	eyeOffsetAngle  = genome[gene++];
	eyeFieldOfView  = genome[gene++];
	olfactoryRange	= genome[gene++];

	// randomize attributes
	size 			= RNGESUS->normalFloat( size, STD_DEGREE*size );
	reach 			= RNGESUS->normalFloat( reach, STD_DEGREE*reach );
	maxEnergy 		= RNGESUS->normalFloat( maxEnergy, STD_DEGREE*maxEnergy );
	maxVelocity 	= RNGESUS->normalFloat( maxVelocity, STD_DEGREE*maxVelocity );
	maxAge 			= RNGESUS->normalFloat( maxAge, STD_DEGREE*maxAge );
	visionRange		= RNGESUS->normalFloat( visionRange, STD_DEGREE*visionRange );
	eyeOffsetAngle	= RNGESUS->normalFloat( eyeOffsetAngle, STD_DEGREE*eyeOffsetAngle );
	eyeFieldOfView	= RNGESUS->normalFloat( eyeFieldOfView, STD_DEGREE*eyeFieldOfView );
	olfactoryRange	= RNGESUS->normalFloat( olfactoryRange, STD_DEGREE*olfactoryRange );

//	cognition.setLinkMatrixFromMask( genome, gene );
	cognition.setLinkMatrixFromParentGenome( genome, gene );

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



//void Animat::initFCM( int n_concepts, string filename_cs, string filename_fcm ) {
//	cognition = FCM( n_concepts );
//	cognition.loadConceptsFromFile(  );
//	cognition.loadLinkMatrixFromFile( filename_fcm );
//	sensation = Eigen::VectorXf::Zero( cognition.nSensory );
//}



//void Animat::initFCMrandom( int n_concepts, string filename_cs ) {
//	cognition = FCM( n_concepts );
//	cognition.loadConceptsFromFile( filename_cs );
//	cognition.setRandomLinkMatrix( 0.6 );
//	sensation = Eigen::VectorXf::Zero( cognition.nSensory );
//}



void Animat::toString() {

	std::cout << name << "; ";
	std::cout << posX << ", " << posY << "; ";
	std::cout << "a: " << direction << " rad; ";
	std::cout << std::endl;

}






} /* namespace ecosystem */
