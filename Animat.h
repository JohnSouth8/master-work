/*
 * Animat.h
 *
 *  Created on: Jul 3, 2017
 *      Author: south
 */

#ifndef ANIMAT_H_
#define ANIMAT_H_


#include "structs.h"

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
	Habitat* environment;

	// member functions
	char* generateName();
	const char* getName();
	int getEnergy();
	void changeVelocity( float );
	void move();
	void eat();
	void turn( float );
	void toString();
	void sense();
	void makeDecision();
	void addSensation( f_sens );
	void printSensations();


};

} /* namespace ecosystem */

#endif /* ANIMAT_H_ */
