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
	Animat( double, double, double, double, int, double, double, Habitat* );
	virtual ~Animat();

	// member vars
	const char* name;
	double posX;
	double posY;
	double velocity;
	double maxVelocity;
	double direction;
	int energy;
	int maxEnergy;
	double senseAngle;
	double senseRadius;
	std::vector<f_sens> sensedObjs;
	Eigen::VectorXd sensations;
	FCM cognition;
	Habitat* environment;


	///*****************
	// member functions

	// life functions
	int eat();
	void sense();
	void sense_analytic();
	void makeDecision();
	void reason();
	void react( Eigen::VectorXd );

	// movement functions
	void changeVelocityAbsolute( double );
	void changeVelocity( double );
	void setVelocity( double );
	void move();
	void turn( double );

	// utility functions
//	void initFCM( int );
//	void initFCM( int, std::vector<std::string> );
//	void initFCM( int, std::vector<std::string>, Eigen::MatrixXd );
	void initFCM( int, std::string, std::string );
//	void setFCM( Eigen::MatrixXd );
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
