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

#include "Organism.h"
#include "Animat.h"
#include "Habitat.h"


namespace util {


	Eigen::VectorXf sigmoid( const Eigen::VectorXf in ) {

		const unsigned int size = in.size();
		Eigen::VectorXf out(size);

		for ( unsigned int i = 0; i < size; ++i ) {
			out(i) = 1.0 / (1 + exp( -in(i) ));
		}

		return out;

	}



	Eigen::VectorXf tanh( const Eigen::VectorXf in ) {

		const unsigned int size = in.size();
		Eigen::VectorXf out( size );

		for ( unsigned int i = 0; i < size; ++i ){
			out(i) = std::tanh( in(i) );
		}

		return out;

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

		double retval = coordinate;

		if ( retval < 0 ) {
			retval = domainSize + coordinate; // note: because it's negative, this is actually subtraction
		}
		while ( retval >= domainSize ) {
			retval -= domainSize;
		}

		return retval;

	}



	float distanceInPeriodicBoundary( float x1, float y1, float x2, float y2, int maxX, int maxY ) {

		float dx = x2 - x1;
		dx = dx - maxX*round( dx / maxX );

		float dy = y2 - y1;
		dy = dy - maxY*round( dy / maxY );

		return sqrt( pow( dx, 2 ) + pow( dy, 2 ) );

	}



	float getAngleBetween( std::vector<float> v1, std::vector<float> v2 ) {

		float v_angle = atan2( v1[1], v1[0] );
		float g_angle = atan2( v2[1], v2[0] );

		return g_angle - v_angle;

	}



	std::vector<float> getLineParameters( coordinate point, float angle ) {
		// find parameters to line equation of form y=ax+b

		std::vector<float> params ( 2 );

		if ( angle >= 0 ) {
			// TODO: tan is correctly +-, use this
		}
		else {

		}

		float a = 1 / tan( angle );
		float yb = point.x / tan( angle );
		float b = point.y + point.x / tan( angle );

		params[0] = a;
		params[1] = b;

		return params;

	}



	int countNonZeroElements( Eigen::MatrixXf mat ) {

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



	bool compareStimuli( stimulus arg1, stimulus arg2 ) {
		return arg1.distance < arg2.distance;
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
			if ( eqPos == -1 )
				continue;		// ignore lines without '='˘

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



	void printSensationsToFile( std::vector<stimulus> sss, std::string fname ) {
		std::ofstream outputFile;
		outputFile.open( fname.c_str(), std::ofstream::out | std::ofstream::app );

		std::vector<stimulus>::iterator it;

		for ( it = sss.begin(); it != sss.end(); ++it ) {
			outputFile << it->entity->posX << " " << it->entity->posY << std::endl;
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


