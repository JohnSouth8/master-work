/*
 * Chance.h
 *
 *  Created on: Jan 28, 2018
 *      Author: south
 */

#ifndef CHANCE_H_
#define CHANCE_H_

#include <random>

namespace util {

class Chance {
public:

	Chance( int );
	virtual ~Chance();

	// member variables
	std::mt19937 twister;


	// member functions
	int uniformRandomInt( int, int );
	int linearDescRandomInt( int, int );
	int linearAscRandomInt( int, int );

	float uniformRandomFloat( float, float );
	float linearDescRandomFloat( float, float );
	float linearAscRandomFloat( float, float );


};

} /* namespace util */

#endif /* CHANCE_H_ */
