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
	virtual ~Animat();
	// member vars
	float pos_x;
	float pos_y;
	float velocity;
	float direction;
	int energy;
};

} /* namespace ecosystem */

#endif /* ANIMAT_H_ */
