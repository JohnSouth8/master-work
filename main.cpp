/*
 * main.cpp
 *
 *  Created on: Jul 4, 2017
 *      Author: south
 */


#include <iostream>
#include <cstdlib>
#include <map>
#include <ctime>
#include <gnuplot_i.hpp>

#include "Animat.h"
#include "Environment.h"

using namespace std;
using namespace ecosystem;



int main( void ) {

	srand(time(0));

//	map<int, Animat> somemap;
	Animat ani;
	ani.toString();

	int sx = 16;
	int sy = 16;
	float density = 0.8;

	Environment env ( sx, sy, density );
	unsigned char* foods = (unsigned char*)env.getFoodReserve();		// TODO: this brings errors, resolve

	Gnuplot gp;
	gp.set_xrange( -1, sx+1 ).set_yrange( -1, sy+1 ).set_cbrange( 0, 1 );
	gp.set_xautoscale();
	gp.set_yautoscale();
	gp.cmd( "set palette gray" );
	gp.cmd( "set pm3d" );
	gp.plot_image( foods, sx, sy, "food reserves" );


    getchar();

	return 0;

}


