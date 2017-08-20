/*
 * FCM.cpp
 *
 *  Created on: Aug 20, 2017
 *      Author: south
 */

#include "FCM.h"



using namespace Eigen;

namespace ecosystem {

FCM::FCM( int nc ) {

	nConcepts = nc;
	FCMap = MatrixXf::Zero( nConcepts, nConcepts );

}

FCM::~FCM() {
	// TODO Auto-generated destructor stub
}

} /* namespace ecosystem */
