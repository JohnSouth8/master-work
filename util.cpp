/*
 * util.cpp
 *
 *  Created on: Jul 26, 2017
 *      Author: south
 */

#include <cstdlib>
#include <fstream>
#include <vector>
#include <map>
#include <Eigen/Dense>

#include "util.h"

#include "Animat.h"


namespace util {

	int randIntFrom( int min, int max ) {
		int range = max - min + 1;
		return rand() % range + min;
	}

	double randFromUnitInterval() {
		return double( rand() ) / double( RAND_MAX );
	}

	void printMatrixToFile( Eigen::MatrixXf data, const char* fname ) {
		std::ofstream outputFile;
		outputFile.open(fname);
		outputFile << data;
		outputFile.close();
	}

	void printSensationsToFile( std::vector<ecosystem::coord> sss, const char* fname ) {
		std::ofstream outputFile;
		outputFile.open(fname);

		std::vector<ecosystem::coord>::iterator it;

		// TODO: because of x/y swap in gnuplot heatmap plot, the coords are printed swapped -- fix it in gnuplot and here
		for ( it = sss.begin(); it != sss.end(); ++it ) {
			outputFile << it->y << " " << it->x << std::endl;
		}

		outputFile.close();
	}

	void printAnimatLocationsToFile( std::map<const char*, ecosystem::Animat*> pop, const char* fname ) {
		std::ofstream outputFile;
		outputFile.open(fname);

		std::map<const char*, ecosystem::Animat*>::iterator it;

		for ( it = pop.begin(); it != pop.end(); ++it ) {
			outputFile << it->second->posY << " " << it->second->posX << std::endl;
		}

		outputFile.close();
	}

}


