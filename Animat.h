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
	std::vector<coord> sensedObjs;
	Habitat* environment;

	// member functions
	char* generateName();
	const char* getName();
	int getEnergy();
	void changeVelocity( float );
	void move();
	void turn( float );
	void toString();
	void sense();
	void makeDecision();
	void addSensation( coord );


};

} /* namespace ecosystem */

#endif /* ANIMAT_H_ */
