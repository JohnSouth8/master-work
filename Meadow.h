/*
 * Meadow.h
 *
 *  Created on: Jan 28, 2018
 *      Author: south
 */

#ifndef MEADOW_H_
#define MEADOW_H_

extern const float PI;

namespace ecosystem {

// forward declaration
class Habitat;

class Meadow {
public:

	Meadow( int, int, int, float, Habitat* );
	virtual ~Meadow();

	int centerX;
	int centerY;
	int radius;
	float growRate;
	Habitat* environment;

	int grow();

};

} /* namespace ecosystem */

#endif /* MEADOW_H_ */
