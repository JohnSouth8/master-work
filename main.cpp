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

string fname_fcm_cs = "fcm_0.1.concepts.txt";
string fname_fcm = "fcm_0.1.txt";
string fname_test = "test.txt";

int test();
int fcm_test( Animat*, Habitat* );
int animat_test( Animat*, Habitat* );
int test_algebra();



int main( void ) {

	srand(time(0));

	// empty files
	util::cleanFile( fname_pop );
	util::cleanFile( fname_sens );

//	string fcontent = util::readFileContent( fname_fcm );


	return test_algebra();
//	return test();
}



int test() {

	// TODO: sort out smooth operation and visual debugging with switches and stuff --!! maybe with config files??

	int sx = 250;
	int sy = 250;
	double density = 0.05;

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


	int nConcepts = 9;
	ani.initFCM( nConcepts, fname_fcm_cs, fname_fcm );





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

	ani->reason();


	return 0;

}



int animat_test( Animat* ani, Habitat* env ) {

	ani->sense();
	ani->makeDecision();
	ani->move();

	util::printAnimatLocationsToFile( env->population, fname_pop );
	util::printSensationsToFile( ani->sensedObjs, fname_sens );

	ani->forgetSensedObjects();

	return 0;

}



int test_algebra() {

	MatrixXd L( 5, 5 );

	L << 	0, 0, -0.5, 0.75, 0,
			0, 0, 0, 1, -0.5,
			0, 0, 0.5, 0.66, 0,
			0, 0, 0.33, 0, 1,
			0, 0, -1, 0.25, 0.25;

	cout << "Link matrix:" << endl;
	cout << L << endl;

	VectorXd vec(5);

	vec << 0.5, 0.7, 0, 0, 0;

	cout << "input vector:" << endl;
	cout << vec << endl;

	VectorXd state = VectorXd::Zero( 5 );

	cout << "state:" << endl;
	cout << state << endl;

	MatrixXd Lt = L.transpose();
	VectorXd dS = Lt * vec;

	cout << "activation propagation ~ Lt * input ~:" << endl;
	cout << dS << endl;

	for ( int i = 0; i < 3; ++i ){

		state = util::sigmoid( state + dS );

		cout << "delta state:" << endl;
		cout << dS << endl;
		cout << "new state:" << endl;
		cout << state << endl;

	}

	return 0;

}



