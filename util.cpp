/*
 * util.cpp
 *
 *  Created on: Jul 26, 2017
 *      Author: south
 */

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <Eigen/Dense>

#include "util.h"

#include "Animat.h"


namespace util {


	Eigen::VectorXd sigmoid( const Eigen::VectorXd in ) {

		const unsigned int size = in.size();
		Eigen::VectorXd out(size);

		for ( unsigned int i = 0; i < size; ++i ) {
			out(i) = 1.0 / (1 + exp( -in(i) ));
		}

		return out;

	}

	Eigen::VectorXd tanh( const Eigen::VectorXd in ) {

		const unsigned int size = in.size();
		Eigen::VectorXd out( size );

		for ( unsigned int i = 0; i < size; ++i ){
			out(i) = std::tanh( in(i) );
		}

		return out;

	}

	// deprecated, use util::Chance
	int randIntFrom( int min, int max ) {
		int range = max - min + 1;
		return rand() % range + min;
	}

	// deprecated, use util::Chance
	float randFromUnitInterval() {
		return float( rand() ) / float( RAND_MAX );
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

	double getWrappedCoordinate( double coordinate, int domainSize ) {

		double retval = coordinate;

		if ( retval < 0 ) {
			retval = domainSize + coordinate; // note: because it's negative, this is actually subtraction
		}
		while ( retval >= domainSize ) {
			retval -= domainSize;
		}

		return retval;

	}

	double getAngleBetween( Eigen::Vector2d v1, Eigen::Vector2d v2 ) {

		double v_angle = atan2( v1(1), v1(0) );
		double g_angle = atan2( v2(1), v2(0) );

		return g_angle - v_angle;

	}



	int countNonZeroElements( Eigen::MatrixXd mat ) {

		int rows = mat.rows();
		int cols = mat.cols();
		int count = 0;

		for ( int i = 0; i < rows; ++i ) {
			for ( int j = 0; j < cols; ++j ) {
				if ( mat(i,j) != 0 )
					++count;
			}
		}

		return count;

	}



	std::vector<float> getColourFromGradient( std::vector<float> minColour, std::vector<float> maxColour, float value ) {

		std::vector<float> outColour(3, 0.0f);
		for ( int i = 0; i < 3; ++i ){
			outColour[i] = minColour[i] + value * (maxColour[i] - minColour[i]);
		}

		return outColour;

	}



	bool compareFoodSensations( ecosystem::f_sens arg1, ecosystem::f_sens arg2 ) {
		return arg1.d < arg2.d;
	}



	std::string readFileContent( std::string fname ) {

		std::ifstream inputFile;
		inputFile.open( fname.c_str(), std::ifstream::in );

		std::string fileContent = "";
		std::string line;
		while ( std::getline( inputFile, line ) ) {
			fileContent += line;
			fileContent += '\n';
		}

		inputFile.close();
		return fileContent;

	}



	std::map<std::string, float> readSimpleIni( std::string fname ) {

		std::ifstream inputFile;
		inputFile.open( fname.c_str(), std::ifstream::in );

		std::map<std::string, float> retmap;

		std::string line;
		while ( std::getline( inputFile, line ) ) {

			int eqPos = line.find( "=" );
			std::string keyword = line.substr( 0, eqPos );
			std::string value = line.substr( eqPos + 1 );
			float fval = atof( value.c_str() );

			retmap[keyword] = fval;

		}

		inputFile.close();
		return retmap;

	}



	void cleanFile( std::string fname ) {
		std::ofstream outputFile;
		outputFile.open( fname.c_str(), std::ofstream::out | std::ofstream::trunc );
		outputFile.close();
	}

	void printMatrixToFile( Eigen::MatrixXd data, std::string fname, bool transpose ) {

		Eigen::MatrixXd outputData = data;
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


