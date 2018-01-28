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

// forward declaration
class Habitat;

class Animat {
	// TODO: write inits based on config files rather than passing of a myriad of parameters
public:

	// constructors
	Animat();
	Animat( double, double, double, double, double, float, double, double, double, Habitat* );
	virtual ~Animat();

	// member vars
	const char* name;
	double posX;
	double posY;
	double velocity;
	double maxVelocity;
	double direction;
	float energy;
	float maxEnergy;
	double senseAngle;
	double senseRadius;
	double reach;
	std::vector<f_sens> sensedObjs;
	Eigen::VectorXd sensations;
	FCM cognition;
	Habitat* environment;


	///*****************
	// member functions

	// life functions
	int eat();
	int eat( int, int );
	void sense();
	void sense_analytic();
	void calculateDecision();
	void reason();
	void react( Eigen::VectorXd );

	// movement functions
	void changeVelocityAbsolute( double );
	void adjustVelocity( double );
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

	void toString();
	void forgetSensations();
	void addSensedObject( f_sens );
	void forgetSensedObjects();
	void printSensedObjects();


};

} /* namespace ecosystem */

#endif /* ANIMAT_H_ */
