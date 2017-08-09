/*
 * Environment.h
 *
 *  Created on: Jul 4, 2017
 *      Author: south
 */

#ifndef HABITAT_H_
#define HABITAT_H_

#include <map>
#include <Eigen/Dense>

using namespace std;

namespace ecosystem {

// forward declarations
class Animat;

// class declaration
class Habitat {
public:

	// constructors
	Habitat();
	Habitat( int, int, float );
	virtual ~Habitat();

	// member variables
	int sizeX;
	int sizeY;
	map<const char*, Animat> population;
	Eigen::MatrixXf foodReserve;


	// member functions
	void distributeFood( float );
	void birth( Animat );
	Eigen::MatrixXf getFoodReserve();

};

} /* namespace ecosystem */

#endif /* HABITAT_H_ */
