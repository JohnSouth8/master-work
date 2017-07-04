/*
 * Environment.h
 *
 *  Created on: Jul 4, 2017
 *      Author: south
 */

#ifndef ENVIRONMENT_H_
#define ENVIRONMENT_H_

#include <map>

#include "Animat.h"

using namespace std;

namespace ecosystem {

class Environment {
public:

	// constructors
	Environment();
	Environment( int, int );
	virtual ~Environment();

	// member variables
	int sizeX;
	int sizeY;
	map<int, Animat> population;

	// member functions
};

} /* namespace ecosystem */

#endif /* ENVIRONMENT_H_ */
