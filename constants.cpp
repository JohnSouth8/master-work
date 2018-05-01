/*
 * constants.cpp
 *
 *  Created on: May 1, 2018
 *      Author: south
 */

#include <random>
#include "Chance.h"
#include "constants.h"

std::random_device RNGESUS;
util::Chance* const FATE = new util::Chance( RNGESUS() );

const float PI = 3.14159;

