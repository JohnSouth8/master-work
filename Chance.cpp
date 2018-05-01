/*
 * Chance.cpp
 *
 *  Created on: Jan 28, 2018
 *      Author: south
 */

#include <array>
#include <vector>

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
	// returns from range [min, max]

	std::uniform_int_distribution<int> dist( min, max );
	return dist( twister );

}


std::vector<int> Chance::nUniformRandomIntsFrom( int n, int min, int max ) {

	std::vector<int> retvec( n );
	std::uniform_int_distribution<int> dist( min, max );

	for ( int i = 0; i < n; ++i )
		retvec[i] = dist( twister );

	return retvec;
}


float Chance::uniformRandomFloatFrom( float min, float max ) {
	// returns from range [min, max)

	std::uniform_real_distribution<float> dist( min, max );
	return dist( twister );

}


std::vector<float> Chance::nUniformRandomFloatsFrom( int n, float min, float max ) {

	std::vector<float> retvec( n );
	std::uniform_real_distribution<float> dist( min, max );

	for ( int i = 0; i < n; ++i )
		retvec[i] = dist( twister );

	return retvec;

}


float Chance::linearDescRandomFloatFrom( float start, float end ) {

	std::array<float,2> intervals {start, end};
	std::array<float,2> weights {5.0, 0.0};
	std::piecewise_linear_distribution<float> dist( intervals.begin(), intervals.end(), weights.begin() );

	return dist( twister );

}

float Chance::normalFloat( float mean, float std ) {

	std::normal_distribution<float> dist( mean, std );

	return dist( twister );

}

std::vector<float> Chance::normalFloatsString( int n, float mean, float std ) {

	std::normal_distribution<float> dist( mean, std );
	std::vector<float> retvec( n );

	for ( int i = 0; i < n; ++i )
		retvec[i] = dist( twister );

	return retvec;

}

int Chance::normalInt( float mean, float std ) {

	return static_cast<int>( round( normalFloat( mean, std ) ) );

}

std::vector<int> Chance::normalIntsString( int n, float mean, float std ) {

	std::vector<int> retvec ( n );
	std::vector<float> fvec = normalFloatsString( n, mean, std );

	for ( int i = 0; i < n; ++i )
		retvec[i] = static_cast<int>( round( fvec[i] ) );

	return retvec;

}


bool Chance::bernoulliBoolean( float p ) {

	std::bernoulli_distribution dist ( p );
	return dist( twister );

}

std::vector<bool> Chance::bernoulliBooleanString( int n, float p ) {

	std::bernoulli_distribution dist ( p );
	std::vector<bool> retvec ( n );

	for ( int i = 0; i < n; ++i )
		retvec[i] = dist( twister );

	return retvec;

}






} /* namespace util */
