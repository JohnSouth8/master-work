/*
 * FCM.h
 *
 *  Created on: Aug 20, 2017
 *      Author: south
 */

#ifndef FCM_H_
#define FCM_H_

#include <Eigen/Dense>

namespace ecosystem {

class FCM {
public:

	// constructors
	FCM( int );
	virtual ~FCM();

	// member variables
	int nConcepts;
	Eigen::MatrixXf FCMap;

};

} /* namespace ecosystem */

#endif /* FCM_H_ */
