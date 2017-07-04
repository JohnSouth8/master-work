/*
 * Environment.cpp
 *
 *  Created on: Jul 4, 2017
 *      Author: south
 */

#include "Environment.h"

#include <iostream>
#include <map>

using namespace std;

namespace ecosystem {

Environment::Environment() {

	sizeX = 1000;
	sizeY = 1000;
	population = map<int, Animat>();

}

Environment::~Environment() {
	// TODO Auto-generated destructor stub
}

} /* namespace ecosystem */
