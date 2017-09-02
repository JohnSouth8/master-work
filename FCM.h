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

private:
	// member variables
	int nConcepts;
	int nInput;
	int nInternal;
	int nOutput;

	std::vector<std::string> concepts;
	Eigen::VectorXf state;
	Eigen::MatrixXf L;
//	int initialized;

public:
	// member functions
//	void setConcepts( std::vector<std::string> );
	void loadConceptsFromFile( std::string );
	void loadLinkMatrixFromFile( std::string );
	void propagateInput( Eigen::VectorXf );

	// getters & setters
	void setState( Eigen::VectorXf );
	Eigen::VectorXf getState();
	int getNConcepts();
	int getNInput();
	int getNInternal();
	int getNOutput();



};

} /* namespace ecosystem */

#endif /* FCM_H_ */
