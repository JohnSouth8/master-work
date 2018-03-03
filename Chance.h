/*
 * Chance.h
 *
 *  Created on: Jan 28, 2018
 *      Author: south
 */

#ifndef CHANCE_H_
#define CHANCE_H_

#include <random>
#include <vector>

namespace util {

class Chance {
public:

	Chance( int );
	virtual ~Chance();

	// member variables
	std::mt19937 twister;
	std::uniform_int_distribution<int> booleanIntDistribution;
	std::uniform_real_distribution<float> uniformUnitDistribution;


	// member functions
	// default distributions
	bool randomBoolean();
	float uniformRandomUnitFloat();

	// custom distributions
	int uniformRandomIntFrom( int, int );
	std::vector<int> nUniformRandomIntsFrom( int, int, int );
	int linearDescRandomIntFrom( int, int );
	int linearAscRandomIntFrom( int, int );

	float uniformRandomFloatFrom( float, float );
	std::vector<float> nUniformRandomFloatsFrom( int, float, float );
	float linearDescRandomFloatFrom( float, float );
	float linearAscRandomFloatFrom( float, float );

	float normalFloat( float, float );
	std::vector<float> normalFloatsString( int, float, float );

	int normalInt( float, float );
	std::vector<int> normalIntsString( int, float, float );


};

} /* namespace util */

#endif /* CHANCE_H_ */
