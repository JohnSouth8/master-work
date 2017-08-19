/*
 * util.cpp
 *
 *  Created on: Jul 26, 2017
 *      Author: south
 */

#include <cstdlib>
#include <iostream>
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

	int getWrappedIndex( int index, int domainSize ) {

		int retval = index;

//		if ( index >= 0 && index < domainSize ) {
//			retval = index;
//		}
//		else
		if ( retval < 0 ) {
			retval = domainSize + index;
		}
		else if ( retval >= domainSize ) {
			retval = index % domainSize;
		}

		return retval;

	}

	float getWrappedCoordinate( float coordinate, int domainSize ) {

		float retval = coordinate;

//		if ( coordinate >= 0 && coordinate < domainSize ) {
//			retval = coordinate;
//		}
//		else
		if ( retval < 0 ) {
			retval = domainSize + coordinate;
		}
		while ( retval >= domainSize ) {
			retval -= domainSize;
		}

		return retval;

	}


	void cleanFile( std::string fname ) {
		std::ofstream outputFile;
		outputFile.open( fname.c_str(), std::ofstream::out | std::ofstream::trunc );
		outputFile.close();
	}

	void printMatrixToFile( Eigen::MatrixXf data, std::string fname ) {

		// print transposed version of the matrix because matrix with origin in up-left is mirrored in coordinate system with origin in low-left
		// 		i.e. row-major order is not compatible with x-axis first order in coordinate system
		Eigen::MatrixXf outputData = data.transpose();

		std::ofstream outputFile;
		outputFile.open( fname.c_str() );
		outputFile << outputData;
		outputFile.close();
	}

	void printSensationsToFile( std::vector<ecosystem::f_sens> sss, std::string fname ) {
		std::ofstream outputFile;
		outputFile.open( fname.c_str(), std::ofstream::out | std::ofstream::app );

		std::vector<ecosystem::f_sens>::iterator it;

		for ( it = sss.begin(); it != sss.end(); ++it ) {
			outputFile << it->x << " " << it->y << std::endl;
		}

		outputFile.close();
	}

	void printAnimatLocationsToFile( std::map<const char*, ecosystem::Animat*> pop, std::string fname ) {
		std::ofstream outputFile;
		outputFile.open( fname.c_str(), std::ofstream::out | std::ofstream::app );

		std::map<const char*, ecosystem::Animat*>::iterator it;

		for ( it = pop.begin(); it != pop.end(); ++it ) {
			outputFile << it->second->posX << " " << it->second->posY << std::endl;
		}

		outputFile.close();
	}

}


