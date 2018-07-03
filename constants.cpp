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



// math
const float PI = 3.14159;

//filenames
const std::string ENVIRONMENT_INI 		= "environment.ini";
const std::string ANIMAT_INI 			= "animat.ini";
const std::string FCM_CONCEPTS_FILE 	= "fcm_concepts_031";
const std::string FCM_MASK_FILE			= "fcm_mask_031";

// randomness
std::random_device FATE;
util::Chance* const RNGESUS = new util::Chance( FATE() );
const float STD_DEGREE = 0.1;

// world
ecosystem::Habitat* const HABITAT = new ecosystem::Habitat();


