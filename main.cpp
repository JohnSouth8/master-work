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
#include "Environment.h"
#include "util.h"

using namespace std;
using namespace Eigen;
using namespace ecosystem;





int main( void ) {

	srand(time(0));


	int sx = 160;
	int sy = 160;
	float density = 0.25;

	Environment env ( sx, sy, density );
	MatrixXf foods = env.getFoodReserve();

//	Gnuplot gp;
//	gp.set_xrange( -1, sx+1 ).set_yrange( -1, sy+1 ).set_cbrange( 0, 1 );
//	gp.set_xautoscale();
//	gp.set_yautoscale();
//	gp.cmd( "set palette gray" );
//	gp.cmd( "set pm3d" );
//	gp.plot_image( foods, sx, sy, "food reserves" );

	float allFood = foods.sum()/( sx * sy );
	cout << allFood << endl;

	char fname[] = "foodReserve.txt";
	util::printToFile( foods, fname );

	// TODO: implement one moving agent who is searching for food until it dies

	double randx = ( double( rand() ) / double( RAND_MAX ) ) * sx;
	double randy = ( double( rand() ) / double( RAND_MAX ) ) * sy;
	double randdir = ( double( rand() ) / double( RAND_MAX ) ) * M_PI;

	Animat ani( randx, randy, 0.0, randdir, 100 );
	ani.toString();

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






