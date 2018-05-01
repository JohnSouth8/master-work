/*
 * main.cpp
 *
 *  Created on: Jul 4, 2017
 *      Author: south
 */


#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <map>
#include <random>
#include <ctime>
#include <chrono>
#include <cmath>
#include <array>
#include <thread>
#include <algorithm>
#include <Eigen/Dense>
#include <unistd.h>

#include "util.h"
#include "Chance.h"
#include "QuadTree.h"
#include "graphx.h"
#include "constants.h"



GLFWwindow* simWindow;
GLFWwindow* fcmWindow;
bool simulationProceed = false;



#include "Organism.h"
#include "Grass.h"
#include "Animat.h"
#include "Habitat.h"



using namespace std;
using namespace Eigen;
using namespace ecosystem;
using util::coordinate;



string fname_environment_ini = "environment.ini";
string fname_animat_ini = "animat.ini";
string fname_fcm_cs = "fcm_0.2.concepts.txt";
string fname_fcm = "fcm_0.2.txt";



#include "habitat_tests.h"
#include "debug_tests.h"




int main( void ) {


	// init timer
	auto t_start = chrono::steady_clock::now();



	// TODO: nicely put stuff together - happening and rendering

//	test_quadTreeAnimats();
//	test_quadTreeFood();
//	test_with_visuals();
	test_random_animats();
//	test_angles();



	// stop timer and print exec time
	auto t_end = chrono::steady_clock::now();
	auto elapsed = chrono::duration_cast<chrono::milliseconds>( t_end - t_start );
	cout << endl << endl << "Runtime: " << elapsed.count() << " milliseconds" << endl;


	delete FATE;
	return 0;

}










