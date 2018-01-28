/*
 * Environment.h
 *
 *  Created on: Jul 4, 2017
 *      Author: south
 */

#ifndef HABITAT_H_
#define HABITAT_H_

#include <map>
#include <vector>
#include <Eigen/Dense>

using namespace std;

namespace ecosystem {

// forward declarations
class Animat;
class Meadow;

// class declaration
class Habitat {
public:

	// constructors
	Habitat();
	Habitat( int, int, int, double );
	Habitat( int, int, int, int, int, float );
	virtual ~Habitat();

	// member variables
	int sizeX;
	int sizeY;
	int foodEnergyVal;
	map<const char*, Animat*> population;
	Eigen::MatrixXf foodReserve;			// possibly swap this with Eigen::Array
	vector<Meadow*> meadows;


	// member functions
	void distributeFood( double );
	void growMeadows();
	void growFood_stable();
	void growFoodSlow();
	float consumeFood( int, int );

	void birth( Animat* );
	void death( const char* );

	int getXSize();
	int getYSize();
	Eigen::MatrixXf getFoodReserve();

};

} /* namespace ecosystem */

#endif /* HABITAT_H_ */
