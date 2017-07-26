/*
 * util.cpp
 *
 *  Created on: Jul 26, 2017
 *      Author: south
 */

#include <cstdlib>
#include <fstream>
#include <Eigen/Dense>

#include "util.h"


namespace util {

	int randIntFrom( int min, int max ) {
		int range = max - min + 1;
		return rand() % range + min;
	}

	double randFromUnitInterval() {
		return double( rand() ) / double( RAND_MAX );
	}

	void printToFile( Eigen::MatrixXf data, char* fname ) {
		std::ofstream outputFile;
		outputFile.open(fname);
		outputFile << data;
		outputFile.close();
	}

}


