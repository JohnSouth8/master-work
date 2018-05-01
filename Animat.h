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
	Animat( std::string, float, float, float, int, float, float, float, float, float, float, float, float, float, Habitat* );
	virtual ~Animat();


	//// -- member vars --

	// characteristics
	std::string name;
	float size;
	float reach;
	float maxVelocity;
	float maxEnergy;
	int maxAge;

	// senses
	float visionRange;
	float eyeOffsetAngle;
	float eyeFieldOfView;
	float olfactoryRange;

	// physical parameters
	float direction;
	float velocity;
	int age;

	// homeostasis variables
	float energy;
	float comfort;				// 0-100
	float fatigue;				// 0-100
//	float integrity;			// possible synergy with pain and fight...

	// perception model
	std::vector<util::stimulus> nearbyFood;
	std::vector<util::stimulus> nearbyKin;
	std::vector<util::stimulus> nearbyFoes;
	Eigen::VectorXf sensation;

	// brain
	FCM cognition;

	// environment reference
	Habitat* environment;



	//// -- member functions --


	// TODO: remove deprecated functions
	// life functions
	int eat();
	int eat( int, int );
	void calculateDecision();
	void reason();
	void react( Eigen::VectorXf );

	// perception model
	void sense();


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
	void initFCMrandom( int, std::string );

	void forgetSensation();
	void forgetStimuli();

	void toString();


	// temp funcs
	void senseUpdate();
	void senseFood();
	void senseOld();

};

} /* namespace ecosystem */

#endif /* ANIMAT_H_ */
