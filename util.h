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
	class Organism;
	class Animat;
	class Habitat;
}

namespace util {

	Eigen::VectorXf sigmoid( Eigen::VectorXf );
	Eigen::VectorXf tanh( Eigen::VectorXf );

	// following functions are deprecated since implementation of util::Chance
	int randIntFrom( int, int );
	float randFromUnitInterval();

	int getWrappedIndex( int, int );
	float getWrappedCoordinate( float, int );
	float distanceInPeriodicBoundary( float, float, float, float, int, int );

	float distanceBetweenOrganisms( ecosystem::Organism*, ecosystem::Organism*, ecosystem::Habitat* );
	float getAngleBetween( Eigen::Vector2f, Eigen::Vector2f );

	int countNonZeroElements( Eigen::MatrixXf );

	std::vector<float> getColourFromGradient( std::vector<float>, std::vector<float>, float );

	bool compareFoodSensations( sensation, sensation );

	std::string readFileContent( std::string );
	std::map<std::string, float> readSimpleIni( std::string );
	void cleanFile ( std::string );
	void printMatrixToFile( Eigen::MatrixXf, std::string, bool = true );
	void printSensationsToFile( std::vector<sensation>, std::string );
	void printAnimatLocationsToFile( std::map<const char*, ecosystem::Animat*>, std::string );

}



#endif /* UTIL_H_ */
