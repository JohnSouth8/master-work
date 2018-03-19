/*
 * Environment.h
 *
 *  Created on: Jul 4, 2017
 *      Author: south
 */

#ifndef HABITAT_H_
#define HABITAT_H_


#include "Chance.h"
#include "QuadTree.h"

#include <map>
#include <vector>
#include <string>
#include <Eigen/Dense>



extern const float PI;

namespace ecosystem {

// forward declarations
class Grass;
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
	std::vector<Grass*> vegetation;				// Possibly not really required? All food is already in food tree.. But it's faster for graphx
	util::Chance* fate;

	int foodEnergyVal;
	std::vector<Meadow*> meadows;
	float mean_rMeadows;
	float std_rMeadows;
	float mean_grMeadows;
	float std_grMeadows;
	float p_newMeadow;

	// QuadTrees for easy storage and search
	util::QuadTree foodTree;
	util::QuadTree populationTree;

	// deprecated as of QuadTree implementation
	// helper variables for distance computation
	Eigen::MatrixXf animatDistances;
	std::vector<std::string> animatOrder;		// order of animats in the population map


	// member functions
	void growMeadows();
	void growGrass( int, int );
	int consumeFood( int, int );

	void populateWorld( int, std::string, std::string, std::string );
	void birth( Animat* );
	void death( std::string );
	std::string generateAnimatName();

};

} /* namespace ecosystem */

#endif /* HABITAT_H_ */
