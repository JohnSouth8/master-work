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





int main( void ) {

	srand(time(0));


	int sx = 16;
	int sy = 16;
	float density = 0.02;

	Habitat env ( sx, sy, density );
	MatrixXf foods = env.getFoodReserve();


	float allFood = foods.sum()/( sx * sy );
	cout << allFood << endl;

	const char* fname = "foodReserve.txt";
	util::printMatrixToFile( foods, fname );

	// TODO: implement one moving agent who is searching for food until it dies

	double randx = ( double( rand() ) / double( RAND_MAX ) ) * sx;
	double randy = ( double( rand() ) / double( RAND_MAX ) ) * sy;
	double randdir = ( double( rand() ) / double( RAND_MAX ) ) * M_PI;

	Animat ani( randx, randy, 0.0, randdir, 100, 5, 2*M_PI, &env );
	env.birth( &ani );

	ani.sense();
	ani.toString();
//	ani.printSensations();

	const char* fname2 = "sensations.txt";
	util::printSensationsToFile( ani.sensedObjs, fname2 );

	const char* fname3 = "population.txt";
	util::printAnimatLocationsToFile( env.population, fname3 );

	// life loop
//	while ( true ) {
//
//
//
//		if ( ani.getEnergy() <= 0 )
//			break;
//
//	}

//    getchar();

	return 0;

}


// TODO: destruct everything in classes with new keyword



