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
	Habitat( int, int, int, double );
	virtual ~Habitat();

	// member variables
	int sizeX;
	int sizeY;
	int foodEnergyVal;
	map<const char*, Animat*> population;
	Eigen::MatrixXd foodReserve;			// possibly swap this with Eigen::Array


	// member functions
	void distributeFood( double );
	void birth( Animat* );
	void death( const char* );
	int consumeFood( int, int );
	int getXSize();
	int getYSize();
	Eigen::MatrixXd getFoodReserve();

};

} /* namespace ecosystem */

#endif /* HABITAT_H_ */
