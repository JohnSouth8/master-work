/*
 * Animat.h
 *
 *  Created on: Jul 3, 2017
 *      Author: south
 */

#ifndef ANIMAT_H_
#define ANIMAT_H_


#include "structs.h"
#include "FCM.h"

#include <vector>

namespace ecosystem {

class Habitat;

class Animat {
	// TODO: think about public and private vars - are they required? (for clean code purposes they should be, for efficiency maybe rather not)
public:

	// constructors
	Animat();
	Animat( float, float, float, float, int, float, float, Habitat* );
	virtual ~Animat();

	// member vars
	const char* name;
	float posX;
	float posY;
	float velocity;
	float direction;
	int energy;
	float senseAngle;
	float senseRadius;
	std::vector<f_sens> sensedObjs;
	FCM cognition;
	Habitat* environment;


	///*****************
	// member functions

	// life functions
	void eat();
	void sense();
	void makeDecision();

	// movement functions
	void changeVelocity( float );
	void setVelocity( float );
	void move();
	void turn( float );

	// utility functions
	void initFCM( int );
	void initFCM( int, std::vector<std::string> );
	void initFCM( int, std::vector<std::string>, Eigen::MatrixXf );
	void setFCM( Eigen::MatrixXf );
	char* generateName();
	const char* getName();
	int getEnergy();
	void toString();
	void addSensation( f_sens );
	void forgetSensations();
	void printSensations();


};

} /* namespace ecosystem */

#endif /* ANIMAT_H_ */
