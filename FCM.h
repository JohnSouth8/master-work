/*
 * FCM.h
 *
 *  Created on: Aug 20, 2017
 *      Author: south
 */

#ifndef FCM_H_
#define FCM_H_

#include <map>
#include <Eigen/Dense>
#include <vector>

#include "Chance.h"


extern util::Chance* const FATE;

namespace ecosystem {

class Animat;

class FCM {
public:

	// constructors
	FCM();
	FCM( int );
	virtual ~FCM();

	// member variables
	int nConcepts;
	int nInput;
	int nInternal;
	int nOutput;


	// concepts' names
	std::map<std::string, int> concepts;

	// indices in the state vector
	std::vector<int> inputConceptIdxs;
	std::vector<int> internalConceptIdxs;
	std::vector<int> outputConceptIdxs;

	Eigen::VectorXf state;
	Eigen::MatrixXf L;
//	int initialized;

	// member functions
//	void setConcepts( std::vector<std::string> );
	void loadConceptsFromFile( std::string );
	void setRandomLinkMatrix( float );
	void loadLinkMatrixFromFile( std::string );
	void applySensations( Eigen::VectorXf );
	Eigen::VectorXf getOutput();

	// getters & setters
	void setState( Eigen::VectorXf );
	Eigen::VectorXf getState();
	int getNConcepts();
	int getNInput();
	int getNInternal();
	int getNOutput();

	// some fix for some eigen alignment, don't fully understand it: https://eigen.tuxfamily.org/dox/group__TopicStructHavingEigenMembers.html
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW



};

} /* namespace ecosystem */

#endif /* FCM_H_ */
