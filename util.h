/*
 * util.h
 *
 *  Created on: Jul 26, 2017
 *      Author: south
 */

#ifndef UTIL_H_
#define UTIL_H_


#include <Eigen/Dense>

namespace util {

	int randIntFrom( int, int );
	double randFromUnitInterval();
	void printMatrixToFile( Eigen::MatrixXf, char* );

}



#endif /* UTIL_H_ */
