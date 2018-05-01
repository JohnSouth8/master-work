/*
 * Meadow.h
 *
 *  Created on: Jan 28, 2018
 *      Author: south
 */

#ifndef MEADOW_H_
#define MEADOW_H_

#include "Chance.h"
#include "Habitat.h"


extern const float PI;
extern util::Chance* const FATE;
extern ecosystem::Habitat* const HABITAT;


namespace ecosystem {

// forward declaration
class Habitat;

class Meadow {
public:

	Meadow( int, int, int, float );
	virtual ~Meadow();

	int centerX;
	int centerY;
	int radius;
	float growRate;

	int grow();

};

} /* namespace ecosystem */

#endif /* MEADOW_H_ */
