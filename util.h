/*
 * util.h
 *
 *  Created on: Jul 26, 2017
 *      Author: south
 */

#ifndef UTIL_H_
#define UTIL_H_


#include <vector>
#include <string>
#include <map>
#include <Eigen/Dense>

#include "structs.h"

namespace ecosystem {
	class Animat;
}

namespace util {

	Eigen::VectorXf sigmoid( Eigen::VectorXf );
	Eigen::VectorXf tanh( Eigen::VectorXf );

	// following functions are deprecated since implementation of util::Chance
	int randIntFrom( int, int );
	float randFromUnitInterval();

	int getWrappedIndex( int, int );
	double getWrappedCoordinate( double, int );


	double getAngleBetween( Eigen::Vector2f, Eigen::Vector2f );

	int countNonZeroElements( Eigen::MatrixXf );

	std::vector<float> getColourFromGradient( std::vector<float>, std::vector<float>, float );

	bool compareFoodSensations( ecosystem::f_sens, ecosystem::f_sens );

	std::string readFileContent( std::string );
	std::map<std::string, float> readSimpleIni( std::string );
	void cleanFile ( std::string );
	void printMatrixToFile( Eigen::MatrixXf, std::string, bool = true );
	void printSensationsToFile( std::vector<ecosystem::f_sens>, std::string );
	void printAnimatLocationsToFile( std::map<const char*, ecosystem::Animat*>, std::string );

}



#endif /* UTIL_H_ */
