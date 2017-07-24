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
#include <Eigen/Dense>

#include "Animat.h"
#include "Environment.h"

using namespace std;
using namespace Eigen;
using namespace ecosystem;




void printToFile( MatrixXf, char*  );



int main( void ) {

	srand(time(0));

//	map<int, Animat> somemap;
	Animat ani;
	ani.toString();

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
	printToFile( foods, fname );

	// TODO: implement one moving agent who is searching for food until it dies

//    getchar();

	return 0;

}



void printToFile( MatrixXf data, char* fname ) {

	ofstream outputFile;
	outputFile.open(fname);
	outputFile << data;
	outputFile.close();

}









