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
	void printMatrixToFile( Eigen::MatrixXf, const char* );
	void printSensationsToFile( std::vector<ecosystem::coord>, const char* );
	void printAnimatLocationsToFile( std::map<const char*, ecosystem::Animat*>, const char* );

}



#endif /* UTIL_H_ */
