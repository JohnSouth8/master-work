/*
 * habitat_tests.h
 *
 *  Created on: May 1, 2018
 *      Author: south
 */

#ifndef HABITAT_TESTS_H_
#define HABITAT_TESTS_H_


#include "graphx.h"
#include "Chance.h"


extern util::Chance* const FATE;
extern const float PI;


namespace ecosystem {


void keyActions( GLFWwindow* window, int key, int scancode, int action, int mods );
int test_random_animats();
int test_foodGrowth_visual();
int test();
int test_with_visuals();




} /* namespace ecosystem */


#endif /* HABITAT_TESTS_H_ */
