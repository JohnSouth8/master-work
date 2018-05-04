/*
 * constants.h
 *
 *  Created on: May 1, 2018
 *      Author: south
 */

#ifndef CONSTANTS_H_
#define CONSTANTS_H_


#include <string>
#include <random>
#include "Chance.h"
#include "Habitat.h"


// math
extern const float PI;

// filenames
extern const std::string ENVIRONMENT_INI;
extern const std::string ANIMAT_INI;
extern const std::string FCM_CONCEPTS_FILE;
extern const std::string FCM_MASK_FILE;

// randomness
extern util::Chance* const RNGESUS;
extern const float STD_DEGREE;

// world
extern ecosystem::Habitat* const HABITAT;



#endif /* CONSTANTS_H_ */
