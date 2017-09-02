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

	float getAngleBetween( Eigen::Vector2f v1, Eigen::Vector2f v2 ) {

		float v_angle = atan2( v1(1), v1(0) );
		float g_angle = atan2( v2(1), v2(0) );

		return g_angle - v_angle;

	}



	std::string readFileContent( std::string fname ) {

		std::ifstream inputFile;
		inputFile.open( fname.c_str(), std::ifstream::in );

		std::string fileContent = "";
		std::string line;
		while( std::getline( inputFile, line ) ) {
			fileContent += line;
			fileContent += '\n';
		}

		inputFile.close();
		return fileContent;

	}



	void cleanFile( std::string fname ) {
		std::ofstream outputFile;
		outputFile.open( fname.c_str(), std::ofstream::out | std::ofstream::trunc );
		outputFile.close();
	}

	void printMatrixToFile( Eigen::MatrixXf data, std::string fname, bool transpose ) {

		Eigen::MatrixXf outputData = data;
		if ( transpose )
			// print transposed version of the matrix because matrix with origin in upper-left is mirrored in coordinate system with origin in lower-left
			// 		i.e. row-major order is not compatible with x-axis first order in coordinate system
			outputData = data.transpose();

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


