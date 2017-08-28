/*
 * util.h
 *
 *  Created on: Jul 26, 2017
 *      Author: south
 */

#ifndef UTIL_H_
#define UTIL_H_


#include <vector>
#include <map>
#include <Eigen/Dense>

#include "structs.h"

namespace ecosystem {
	class Animat;
}

namespace util {

	int randIntFrom( int, int );
	double randFromUnitInterval();
	int getWrappedIndex( int, int );
	float getWrappedCoordinate( float, int );
	float getAngleBetween( Eigen::Vector2f, Eigen::Vector2f );

	std::string readFileContent( std::string );
	void cleanFile ( std::string );
	void printMatrixToFile( Eigen::MatrixXf, std::string, bool = true );
	void printSensationsToFile( std::vector<ecosystem::f_sens>, std::string );
	void printAnimatLocationsToFile( std::map<const char*, ecosystem::Animat*>, std::string );

}



#endif /* UTIL_H_ */
