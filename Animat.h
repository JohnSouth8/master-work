/*
 * Animat.h
 *
 *  Created on: Jul 3, 2017
 *      Author: south
 */

#ifndef ANIMAT_H_
#define ANIMAT_H_

namespace ecosystem {

class Animat {
public:

	// constructors
	Animat();
	Animat( float, float, float, float, int );
	virtual ~Animat();

	// member vars
	float posX;
	float posY;
	float velocity;
	float direction;
	int energy;

	// member functions


};

} /* namespace ecosystem */

#endif /* ANIMAT_H_ */
