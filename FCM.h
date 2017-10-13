/*
 * FCM.h
 *
 *  Created on: Aug 20, 2017
 *      Author: south
 */

#ifndef FCM_H_
#define FCM_H_

#include <Eigen/Dense>
#include <vector>

namespace ecosystem {

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

	std::vector<int> inputConcepts;		// state indices
	std::vector<int> internalConcepts;
	std::vector<int> outputConcepts;

	std::vector<std::string> concepts;
	Eigen::VectorXd state;
	Eigen::MatrixXd L;
//	int initialized;

	// member functions
//	void setConcepts( std::vector<std::string> );
	void loadConceptsFromFile( std::string );
	void loadLinkMatrixFromFile( std::string );
	void applySensations( Eigen::VectorXd );
	Eigen::VectorXd getOutput();

	// getters & setters
	void setState( Eigen::VectorXd );
	Eigen::VectorXd getState();
	int getNConcepts();
	int getNInput();
	int getNInternal();
	int getNOutput();

	// some fix for some eigen alignment, don't fully understand it: https://eigen.tuxfamily.org/dox/group__TopicStructHavingEigenMembers.html
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW



};

} /* namespace ecosystem */

#endif /* FCM_H_ */
