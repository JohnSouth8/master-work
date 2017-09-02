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
#include <Eigen/Dense>

namespace ecosystem {

class Habitat;

class Animat {
	// TODO: think about public and private vars - are they required? (for clean code purposes they should be, for efficiency maybe rather not)
	// TODO: write inits based on config files rather than passing of a myriad of parameters
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
	int maxEnergy;
	float senseAngle;
	float senseRadius;
	std::vector<f_sens> sensedObjs;
	Eigen::VectorXf sensations;
	FCM cognition;
	Habitat* environment;


	///*****************
	// member functions

	// life functions
	void eat();
	void sense();
	void sense_analytic();
	void makeDecision();
	void reason();

	// movement functions
	void changeVelocity( float );
	void setVelocity( float );
	void move();
	void turn( float );

	// utility functions
//	void initFCM( int );
//	void initFCM( int, std::vector<std::string> );
//	void initFCM( int, std::vector<std::string>, Eigen::MatrixXf );
	void initFCM( int, std::string, std::string );
//	void setFCM( Eigen::MatrixXf );
	char* generateName();
	const char* getName();
	int getEnergy();
	void toString();
	void forgetSensations();
	void addSensedObject( f_sens );
	void forgetSensedObjects();
	void printSensedObjects();


};

} /* namespace ecosystem */

#endif /* ANIMAT_H_ */
