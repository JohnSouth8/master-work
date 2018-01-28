/*
 * Chance.cpp
 *
 *  Created on: Jan 28, 2018
 *      Author: south
 */

#include <array>

#include "Chance.h"

namespace util {

Chance::Chance( int seed ) : twister( seed )
{}

Chance::~Chance() {
	// TODO Auto-generated destructor stub
}


int Chance::uniformRandomInt( int min, int max ) {

	std::uniform_int_distribution<int> dist( min, max );
	return dist( twister );

}


float Chance::uniformRandomFloat( float min, float max ) {

	std::uniform_real_distribution<float> dist( min, max );
	return dist( twister );

}

float Chance::linearDescRandomFloat( float start, float end ) {

	std::array<float,2> intervals {start, end};
	std::array<float,2> weights {10.0, 0.0};
	std::piecewise_linear_distribution<float> dist( intervals.begin(), intervals.end(), weights.begin() );

	return dist( twister );

}






} /* namespace util */
