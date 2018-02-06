/*
 * Chance.cpp
 *
 *  Created on: Jan 28, 2018
 *      Author: south
 */

#include <array>

#include "Chance.h"

namespace util {

Chance::Chance( int seed ) :
		twister( seed ),
		booleanIntDistribution( 0, 1 ),
		uniformUnitDistribution( 0, 1 )
{}

Chance::~Chance() {
	// TODO Auto-generated destructor stub
}



bool Chance::randomBoolean() {

	return booleanIntDistribution( twister );

}


float Chance::uniformRandomUnitFloat() {

	return uniformUnitDistribution( twister );

}


int Chance::uniformRandomIntFrom( int min, int max ) {

	std::uniform_int_distribution<int> dist( min, max );
	return dist( twister );

}


float Chance::uniformRandomFloatFrom( float min, float max ) {

	std::uniform_real_distribution<float> dist( min, max );
	return dist( twister );

}

float Chance::linearDescRandomFloatFrom( float start, float end ) {

	std::array<float,2> intervals {start, end};
	std::array<float,2> weights {5.0, 0.0};
	std::piecewise_linear_distribution<float> dist( intervals.begin(), intervals.end(), weights.begin() );

	return dist( twister );

}






} /* namespace util */
