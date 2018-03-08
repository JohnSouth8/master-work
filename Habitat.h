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



extern const float PI;

namespace ecosystem {

// forward declarations
class Animat;
class Meadow;

// class declaration
class Habitat {
public:

	// constructors
//	Habitat( int, int, int, double, util::Chance* );
//	Habitat( int, int, int, int, int, float, util::Chance* );
	Habitat( std::string, util::Chance* );
	virtual ~Habitat();

	// member variables
	int sizeX;
	int sizeY;
	std::map<std::string, Animat*> population;
	Eigen::MatrixXf foodReserve;			// possibly swap this with Eigen::Array
	util::Chance* fate;

	int foodEnergyVal;
	std::vector<Meadow*> meadows;
	float mean_rMeadows;
	float std_rMeadows;
	float mean_grMeadows;
	float std_grMeadows;
	float p_newMeadow;

	// helper variables for distance computation
	Eigen::MatrixXf animatDistances;
	std::vector<std::string> animatOrder;		// order of animats in the population map


	// member functions
	void distributeFood( double );
	void growMeadows();
	void growFood_stable();
	void growFoodSlow();
	int consumeFood( int, int );

	void populateWorld( int, std::string, std::string, std::string );
	void birth( Animat* );
	void death( std::string );
	std::string generateAnimatName();

	void measureDistances();

	int getXSize();
	int getYSize();
	Eigen::MatrixXf getFoodReserve();

};

} /* namespace ecosystem */

#endif /* HABITAT_H_ */
