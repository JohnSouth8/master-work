/*
 * Environment.h
 *
 *  Created on: Jul 4, 2017
 *      Author: south
 */

#ifndef ENVIRONMENT_H_
#define ENVIRONMENT_H_

#include <map>
#include <Eigen/Dense>

#include "Animat.h"

using namespace std;

namespace ecosystem {

class Environment {
public:

	// constructors
	Environment();
	Environment( int, int, float );
	virtual ~Environment();

	// member variables
	int sizeX;
	int sizeY;
	map<const char*, Animat> population;
	Eigen::MatrixXf foodReserve;


	// member functions
	void distributeFood( float );
	void birth( Animat );
	Eigen::MatrixXf getFoodReserve();

};

} /* namespace ecosystem */

#endif /* ENVIRONMENT_H_ */
