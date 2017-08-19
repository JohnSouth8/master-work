/*
 * main.cpp
 *
 *  Created on: Jul 4, 2017
 *      Author: south
 */


#include <iostream>
#include <fstream>
#include <cstdlib>
#include <map>
#include <ctime>
#include <cmath>
#include <Eigen/Dense>

#include "Animat.h"
#include "Habitat.h"
#include "util.h"

using namespace std;
using namespace Eigen;
using namespace ecosystem;



string fname_food0 = "foodReserve0.txt";
string fname_food1 = "foodReserve1.txt";


string fname_pop = "population.txt";
string fname_sens = "sensations.txt";



int main( void ) {

	srand(time(0));

	// TODO: sort out smooth operation and visual debugging with switches and stuff

	// first empty files
	util::cleanFile( fname_pop );
	util::cleanFile( fname_sens );


	int sx = 250;
	int sy = 250;
	float density = 0.01;

	Habitat env ( sx, sy, density );
	util::printMatrixToFile( env.getFoodReserve(), fname_food0 );


//	float allFood = foods.sum()/( sx * sy );
//	cout << allFood << endl;

	// TODO: implement one moving agent who is searching for food until it dies

	double randx = util::randFromUnitInterval() * sx;
	double randy = util::randFromUnitInterval() * sy;
	double randdir = util::randFromUnitInterval() * M_PI;

	Animat ani(
			randx,		// x
			randy, 		// y
			0, 			// velocity
			randdir, 	// direction
			20, 		// energy
			15, 		// vision range
			2*M_PI, 	// vision angle  TODO: implement it's usage
			&env		// world pointer
	);

	ani.toString();
	env.birth( &ani );

	util::printAnimatLocationsToFile( env.population, fname_pop );

//	ani.sense();
//	ani.printSensations();

//	util::printSensationsToFile( ani.sensedObjs, fname_sens );

	// TODO: sort out smooth operation and visual debugging with switches and stuff

	int time_counter = 0;

	// life loop
	while ( true ) {
//	while ( time_counter < 5 ) {

		ani.sense();
		ani.makeDecision();
		ani.move();

		// TODO: printed food matrix contains weird numbers, check into it

		util::printAnimatLocationsToFile( env.population, fname_pop );
		util::printSensationsToFile( ani.sensedObjs, fname_sens );

		if ( ani.getEnergy() <= 0 )
			break;

		ani.forgetSensations();
		++time_counter;

	}

	cout << "The animat survived " << time_counter << " steps of the simulation" << endl;
	util::printMatrixToFile( env.getFoodReserve(), fname_food1 );

//    getchar();

	return 0;

}


// TODO: destruct everything in classes with the 'new' keyword



