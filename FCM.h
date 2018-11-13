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


extern util::Chance* const RNGESUS;
extern const std::string FCM_CONCEPTS_FILE;
extern const std::string FCM_MASK_FILE;


namespace ecosystem {

class Animat;

class FCM {
public:

	// constructors
	FCM();
	FCM( int );
	virtual ~FCM();

	// member variables
	int nConcepts = 0;
	int nSensory = 0;
	int nMental = 0;
	int nMotor = 0;


	// concepts' names
	std::map<std::string, int> concepts;

	// indices in the state vector
	std::vector<int> sensoryConceptIdxs;
	std::vector<int> mentalConceptIdxs;
	std::vector<int> motorConceptIdxs;

	Eigen::VectorXf state;
	Eigen::MatrixXf L;
//	int initialized;

	// member functions
//	void setConcepts( std::vector<std::string> );
	void loadConceptsFromFCMFile();
	void setRandomLinkMatrix( float );
	void loadLinkMatrixFromFile( std::string );
	void setLinkMatrixFromMask( std::vector<float>, int );
	void setLinkMatrixFromParentGenome( std::vector<float>, int );
	void applySensations( Eigen::VectorXf );
	Eigen::VectorXf getOutput();

	// getters & setters
	void setState( Eigen::VectorXf );


	// some fix for some eigen alignment, don't fully understand it: https://eigen.tuxfamily.org/dox/group__TopicStructHavingEigenMembers.html
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW



};

} /* namespace ecosystem */

#endif /* FCM_H_ */
