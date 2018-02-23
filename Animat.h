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
#include <string>
#include <Eigen/Dense>



extern const float PI;


namespace ecosystem {

// forward declaration
class Habitat;

class Animat {
	// TODO: write inits based on config files rather than passing of a myriad of parameters
public:

	// constructors
	Animat( std::string, float, float, float, float, float, float, float, float, float, float, float, Habitat* );
	virtual ~Animat();


	//// -- member vars --

	// characteristics
	std::string name;
	float size;
	float reach;
	float visionRange;
	float visionAngle;
	float olfactoryRange;
	float maxVelocity;
	float maxEnergy;

	// physical parameters
	float posX;
	float posY;
	float direction;
	float velocity;

	// homeostasis variables
	float energy;
	float comfort;
	float fatigue;

	// perception model
	std::vector<f_sens> sensedObjs;
	Eigen::VectorXf sensations;

	// brain
	FCM cognition;

	// environment reference
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
	void react( Eigen::VectorXf );

	// movement functions
	void changeVelocityAbsolute( float );
	void adjustVelocity( float );
	void setVelocity( float );
	void move();
	void turn( float );

	// utility functions
//	void initFCM( int );
//	void initFCM( int, std::vector<std::string> );
//	void initFCM( int, std::vector<std::string>, Eigen::MatrixXd );
	void initFCM( int, std::string, std::string );
//	void setFCM( Eigen::MatrixXd );

	void toString();
	void forgetSensations();
	void addSensedObject( f_sens );
	void forgetSensedObjects();
	void printSensedObjects();


};

} /* namespace ecosystem */

#endif /* ANIMAT_H_ */
