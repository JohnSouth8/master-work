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
extern util::Chance* const RNGESUS;
extern const std::string ENVIRONMENT_INI;
extern const std::string ANIMAT_INI;



namespace ecosystem {

// forward declarations
class Grass;
class Animat;
class Meadow;

// class declaration
class Habitat {
public:

	// constructors
	Habitat();
	Habitat( std::string );

	virtual ~Habitat();


	// member variables
	int sizeX;
	int sizeY;
	std::map<std::string, Animat*> population;

	int foodEnergyVal;
	std::vector<Meadow*> meadows;

	// QuadTrees for easy storage and search
	util::QuadTree grassTree;
	util::QuadTree populationTree;

	// ini data
	std::map<std::string, float> env_ini;
	std::map<std::string, float> ani_ini;

	// tracking variables
	int foodConsumed;


	// member functions
	void growMeadows();
	void growGrass( int, int );
	float consumeFood( int, int );


	void populateWorld( int );
	void populateWorldOld( int, std::string, std::string, std::string );
	bool breed( Animat*, Animat*, bool );
	void birth( std::vector<float>, util::coordinate location, float direction );
	void death( Animat* );

	float distanceBetweenOrganisms( ecosystem::Organism*, ecosystem::Organism* );

	// deprecated
	void deathOld( std::string );

};

} /* namespace ecosystem */

#endif /* HABITAT_H_ */
