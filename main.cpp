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

#include "Animat.h"
#include "Environment.h"

using namespace std;
using namespace ecosystem;

int main( void ) {

	srand(time(0));

//	map<int, Animat> somemap;
	Animat ani;
	ani.toString();

	Environment env ( 500, 420 );

	return 0;

}


