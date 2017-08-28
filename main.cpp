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

string fname_fcm = "fcm_0.1.txt";
string fname_test = "test.txt";

int test();
int fcm_test( Animat*, Habitat* );
int animat_test( Animat*, Habitat* );



int main( void ) {

	srand(time(0));

	// empty files
	util::cleanFile( fname_pop );
	util::cleanFile( fname_sens );

//	string fcontent = util::readFileContent( fname_fcm );

	return test();
}



int test() {

	// TODO: sort out smooth operation and visual debugging with switches and stuff --!! maybe with config files??

	int sx = 250;
	int sy = 250;
	float density = 0.005;

	Habitat env ( sx, sy, 5, density );
	util::printMatrixToFile( env.getFoodReserve(), fname_food0 );


//	float allFood = foods.sum()/( sx * sy );
//	cout << allFood << endl;

	double randx = util::randFromUnitInterval() * sx;
	double randy = util::randFromUnitInterval() * sy;
	double randdir = util::randFromUnitInterval() * M_PI;

	Animat ani(
			randx,		// x
			randy, 		// y
			0, 			// velocity
			randdir, 	// direction
			20, 		// energy
			12, 		// vision range
			2*M_PI, 	// vision angle  TODO: implement it's usage
			&env		// world pointer
	);

	ani.toString();
	env.birth( &ani );

	util::printAnimatLocationsToFile( env.population, fname_pop );


	int nConcepts = 5;
	// TODO move this onto a file as well
	std::vector<std::string> concepts( nConcepts );
	concepts[0] = "sensAngle";
	concepts[1] = "sensDist";
	concepts[2] = "sensEnergy";
	concepts[3] = "changeVel";
	concepts[4] = "turn";

	ani.initFCM( nConcepts, concepts );

	ani.cognition.loadFCMapFromFile( fname_fcm );


//	ani.setFCM( fcm );




	// TODO: sort out smooth operation and visual debugging with switches and stuff

	int time_counter = 0;

	// life loop
//	while ( true ) {
	while ( time_counter < 5 ) {

//		animat_test( &ani, &env );
		fcm_test( &ani, &env );

		if ( ani.getEnergy() <= 0 )
			break;

		++time_counter;
	}

	cout << "The animat survived " << time_counter << " steps of the simulation" << endl;
	util::printMatrixToFile( env.getFoodReserve(), fname_food1 );

//    getchar();
	return 0;

}



int fcm_test( Animat* ani, Habitat* env ) {

	ani->sense();


	return 0;

}



int animat_test( Animat* ani, Habitat* env ) {

	ani->sense();
	ani->makeDecision();
	ani->move();

	util::printAnimatLocationsToFile( env->population, fname_pop );
	util::printSensationsToFile( ani->sensedObjs, fname_sens );

	ani->forgetSensations();

	return 0;

}


// TODO: destruct everything in classes with the 'new' keyword



