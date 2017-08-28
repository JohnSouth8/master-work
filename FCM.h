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
	FCM( int, std::vector<std::string> );
	virtual ~FCM();

private:
	// member variables
	int nConcepts;
	std::vector<std::string> concepts;
	Eigen::MatrixXf FCMap;

public:
	// member functions
	void setConcepts( std::vector<std::string> );
	void setFCMap( Eigen::MatrixXf );
	void loadFCMapFromFile( std::string );
	Eigen::MatrixXf getFCMap();


};

} /* namespace ecosystem */

#endif /* FCM_H_ */
