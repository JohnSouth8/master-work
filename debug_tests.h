/*
 * debug_tests.h
 *
 *  Created on: May 1, 2018
 *      Author: south
 */

#ifndef DEBUG_TESTS_H_
#define DEBUG_TESTS_H_


#include "Chance.h"
#include "Habitat.h"


extern util::Chance* const FATE;
extern ecosystem::Habitat* const HABITAT;
extern const float PI;


namespace ecosystem {


int test_randomness();
int test_quadTreeAnimats();
int test_quadTreeFood();
int test_angles();


} /* namespace ecosystem */


#endif /* DEBUG_TESTS_H_ */
