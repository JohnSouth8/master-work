/*
 * structs.h
 *
 *  Created on: Aug 8, 2017
 *      Author: south
 */

#ifndef STRUCTS_H_
#define STRUCTS_H_

namespace ecosystem {

struct coordinate {
	int x;
	int y;
} typedef coord;

struct foodSensation {
	int x;		// x coordinate
	int y;		// y coordinate
	double d;	// distance to sensation
} typedef f_sens;

}

#endif /* STRUCTS_H_ */
