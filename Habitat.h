/*
 * Environment.h
 *
 *  Created on: Jul 4, 2017
 *      Author: south
 */

#ifndef HABITAT_H_
#define HABITAT_H_


#include "Chance.h"

#include <map>
#include <vector>
#include <string>
#include <Eigen/Dense>




namespace ecosystem {

// forward declarations
class Animat;
class Meadow;

// class declaration
class Habitat {
public:

	// constructors
	Habitat( int, int, int, double, util::Chance* );
	Habitat( int, int, int, int, int, float, util::Chance* );
	Habitat( std::string, util::Chance* );
	virtual ~Habitat();

	// member variables
	int sizeX;
	int sizeY;
	int foodEnergyVal;
	std::map<std::string, Animat*> population;
	Eigen::MatrixXf foodReserve;			// possibly swap this with Eigen::Array
	std::vector<Meadow*> meadows;
	util::Chance* fate;



	// member functions
	void distributeFood( double );
	void growMeadows();
	void growFood_stable();
	void growFoodSlow();
	float consumeFood( int, int );

	void birth( Animat* );
	void death( std::string );

	int getXSize();
	int getYSize();
	Eigen::MatrixXf getFoodReserve();

};

} /* namespace ecosystem */

#endif /* HABITAT_H_ */
