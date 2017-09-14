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
#include <unistd.h>

#include "Animat.h"
#include "Habitat.h"
#include "util.h"
#include "graphx.h"

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
int fcm_test( Animat* );
int animat_test( Animat*, Habitat* );
int test_algebra();
int test_graphics();



int main( void ) {

	srand(time(0));

//	// empty files
//	util::cleanFile( fname_pop );
//	util::cleanFile( fname_sens );
//
////	string fcontent = util::readFileContent( fname_fcm );
//
//
////	return test_algebra();
//	return test();



	// graphical output debug
	return test_graphics();

}



int test_graphics() {

	gx::openWindow( 800, 600, "testtest" );

	gx::loadShaders( "shader.vert", "shader.frag" );
	gx::setBackground( 1.0f, 0.95f, 0.9f, 1.0f );
	gx::enableKeyboard();

	gx::drawingLoop();

	gx::closeWindow();

	return 0;
}



int test() {

	// TODO: sort out smooth operation and visual debugging with switches and stuff --!! maybe with config files??

	int sx = 50;
	int sy = 50;
	int foodEnergy = 5;
	double density = 0.001;

	Habitat env ( sx, sy, foodEnergy, density );
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
			3,			// max velocity
			randdir, 	// direction
			50, 		// energy
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
	while ( true ) {
//	while ( time_counter < 5 ) {

//		animat_test( &ani, &env );
		fcm_test( &ani );

		util::printAnimatLocationsToFile( env.population, fname_pop );
		util::printSensationsToFile( ani.sensedObjs, fname_sens );

		if ( ani.getEnergy() <= 0 )
			break;

		++time_counter;
	}

	cout << "The animat survived " << time_counter << " steps of the simulation" << endl;
	util::printMatrixToFile( env.getFoodReserve(), fname_food1 );

//    getchar();
	return 0;

}



int fcm_test( Animat* ani ) {

	ani->reason();

	return 0;

}



int animat_test( Animat* ani, Habitat* env ) {

	ani->forgetSensedObjects();

	ani->sense();
	ani->calculateDecision();
	ani->move();


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

//	MatrixXd Lt = L.transpose();
	VectorXd dS = vec.transpose() * L;

	cout << "activation propagation ~ input^t * L ~:" << endl;
	cout << dS << endl;

	for ( int i = 0; i < 3; ++i ){

		dS = vec.transpose() * L;
		state = util::tanh( state + dS );

		cout << "activation propagation" << endl;
		cout << dS << endl;
		cout << "delta state:" << endl;
		cout << dS << endl;
		cout << "new state:" << endl;
		cout << state << endl;

	}



	return 0;

}



