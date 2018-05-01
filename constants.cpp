/*
 * constants.cpp
 *
 *  Created on: May 1, 2018
 *      Author: south
 */


#include "constants.h"

#include <random>
#include "Chance.h"
#include "Habitat.h"


std::random_device RNGESUS;
util::Chance* const FATE = new util::Chance( RNGESUS() );

ecosystem::Habitat* const HABITAT = new ecosystem::Habitat();

const float PI = 3.14159;


