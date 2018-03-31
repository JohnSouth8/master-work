/*
 * Animat.h
 *
 *  Created on: Jul 3, 2017
 *      Author: south
 */

#ifndef ANIMAT_H_
#define ANIMAT_H_


#include "Organism.h"
#include "structs.h"
#include "FCM.h"

#include <vector>
#include <string>
#include <Eigen/Dense>



extern const float PI;


namespace ecosystem {

// forward declaration
class Habitat;

class Animat : public Organism {
	// TODO: write inits based on config files rather than passing of a myriad of parameters
public:

	// constructors
	Animat( std::string, float, float, float, float, float, float, int, float, float, float, float, float, Habitat* );
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
	int maxAge;

	// physical parameters
	float direction;
	float velocity;
	int age;

	// homeostasis variables
	float energy;
	float comfort;
	float fatigue;
//	float integrity;			// possible synergy with pain and fight...

	// perception model
	std::vector<util::stimulus> sensedFood;
	std::vector<util::stimulus> sensedKin;
	std::vector<util::stimulus> sensedFoes;
	Eigen::VectorXf sensations;

	// brain
	FCM cognition;

	// environment reference
	Habitat* environment;



	//// -- member functions --


	// TODO: remove deprecated functions
	// life functions
	int eat();
	int eat( int, int );
	void sense();
	void senseFood();
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

	void forgetSensation();
	void foodStimulus( util::stimulus );
	void kinStimulus( util::stimulus );
	void forgetStimuli();

	void toString();


	// temp funcs
	void senseOld();

};

} /* namespace ecosystem */

#endif /* ANIMAT_H_ */
