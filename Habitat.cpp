/*
 * Environment.cpp
 *
 *  Created on: Jul 4, 2017
 *      Author: south
 */


#include "Habitat.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <string>
#include <map>
#include <Eigen/Dense>

#include "util.h"
#include "Chance.h"
#include "QuadTree.h"

#include "Grass.h"
#include "Animat.h"
#include "Meadow.h"


using util::coordinate;
using util::QuadTree;


namespace ecosystem {

/* constructors */


Habitat::Habitat( std::string iniFileName, util::Chance* ch ) {

	std::map<std::string, float> ini = util::readSimpleIni( iniFileName );

	// basic params
	sizeX = static_cast<int>( ini["size_x"] );
	sizeY = static_cast<int>( ini["size_y"] );
	foodEnergyVal = static_cast<int>( ini["food_energy"] );

	// member structures
	fate = ch;

	// quad trees
	unsigned int qt_bucketsize = static_cast<unsigned int>( ini["quadtree_bucketsize"] );
	coordinate env0( 0, 0 );
	coordinate env1( sizeX, sizeY );
	grassTree = QuadTree( qt_bucketsize, env0, env1 );
	populationTree = QuadTree( qt_bucketsize, env0, env1 );

	// food emanators - meadows
	int n_mdw = static_cast<int>( ini["n_meadows"] );
	p_newMeadow = ini["probability_new_meadow"];
	mean_rMeadows = ini["radius_mean_meadows"];
	std_rMeadows = ini["radius_std_meadows"];
	mean_grMeadows = ini["growrate_mean_meadows"];
	std_grMeadows = ini["growrate_std_meadows"];

	std::vector<int> xs = fate->nUniformRandomIntsFrom( n_mdw, 0, sizeX-1 );
	std::vector<int> ys = fate->nUniformRandomIntsFrom( n_mdw, 0, sizeY-1 );
	std::vector<int> rs = fate->normalIntsString( n_mdw, mean_rMeadows, std_rMeadows );
	std::vector<float> grs = fate->normalFloatsString( n_mdw, mean_grMeadows, std_grMeadows );

	for ( int i = 0; i < n_mdw; ++i ) {
		Meadow* m = new Meadow( xs[i], ys[i], rs[i], grs[i], this );
		meadows.push_back( m );
	}


}


/* destructor */

Habitat::~Habitat() {
	// destroy objects that pointers in data structures are pointing at: animats, meadows,...
//	for ( auto g: vegetation )
//		delete g;

	for ( auto m : meadows )
		delete m;

	for ( auto a : population )
		delete a.second;
}



/* member functions */


void Habitat::populateWorld( int n_animats, std::string animat_iniFileName, std::string fcm_concepts_fileName, std::string fcm_fileName ) {

	std::map<std::string, float> ini = util::readSimpleIni( animat_iniFileName );

	if ( n_animats == 0 )
		n_animats = static_cast<int>( ini["n_animats"] );

	float avg_size = ini["size"];
	float max_energy = ini["max_energy"];
	float start_energy = ini["start_energy"];
	float max_velocity = ini["max_velocity"];
//	float start_velocity = ini["start_velocity"];
	int max_age = static_cast<int>( ini["max_age"] );
	float vision_range = ini["vision_range"];
	float vision_angle = ini["vision_angle"];
	float olfactory_range = ini["olfactory_range"];
	float std_degree = ini["init_std_degree"];

	std::vector<float> sizes = fate->normalFloatsString( n_animats, avg_size, avg_size*std_degree );
	std::vector<float> max_vels = fate->normalFloatsString( n_animats, max_velocity, max_velocity*std_degree );
	std::vector<int> max_ages = fate->normalIntsString( n_animats, max_age, max_age*std_degree );
	std::vector<float> vis_ranges = fate->normalFloatsString( n_animats, vision_range, vision_range*std_degree );
	std::vector<float> vis_angles = fate->normalFloatsString( n_animats, vision_angle, vision_angle*std_degree );
	std::vector<float> olf_ranges = fate->normalFloatsString( n_animats, olfactory_range, olfactory_range*std_degree);

	std::vector<float> pxs = fate->nUniformRandomFloatsFrom( n_animats, 0, sizeX );
	std::vector<float> pys = fate->nUniformRandomFloatsFrom( n_animats, 0, sizeY );
	std::vector<float> dirs = fate->nUniformRandomFloatsFrom( n_animats, -PI, PI );

	for ( int i = 0; i < n_animats; ++i ) {

		std::string name = generateAnimatName();
		Animat* ani = new Animat (
			name,
			sizes[i],
			vis_ranges[i],
			vis_angles[i],
			olf_ranges[i],
			max_vels[i],
			max_energy,
			max_ages[i],
			pxs[i],
			pys[i],
			dirs[i],
			0,
			start_energy,
			this
		);

		int nConcepts = 10;	// TODO: make this number automatic
		ani->initFCM( nConcepts, fcm_concepts_fileName, fcm_fileName );

		population[name] = ani;

	}

}


void Habitat::birth( Animat* ani ) {

	population[ani->name] = ani;

}



void Habitat::death( std::string name ) {

	delete population[name];
	population.erase( name );

}



std::string Habitat::generateAnimatName() {

	int charmin = 65, charmax = 90, randint;
	std::string sname;

	for ( int i = 0; i < 10; ++i ) {
		randint = util::randIntFrom( charmin, charmax );
		sname.push_back( randint );
	}

	return sname;

}



void Habitat::growMeadows() {

	int grown = 0;
	for ( auto meadow : meadows ){

		grown += meadow->grow();

	}

	// fractional chance of a new meadow appearing
	if ( fate->uniformRandomUnitFloat() < p_newMeadow ) {

		int cx = fate->uniformRandomIntFrom( 0, sizeX-1 );
		int cy = fate->uniformRandomIntFrom( 0, sizeY-1 );
		int r = fate->normalInt( mean_rMeadows, std_rMeadows );
		float gr = fate->normalFloat( mean_grMeadows, std_grMeadows );

		Meadow* m = new Meadow( cx, cy, r, gr, this );
		meadows.push_back( m );

	}

//  debug
//	std::cout << "Amount of new food: " << grown << std::endl;

}



void Habitat::growGrass( int x, int y ) {

	Grass* ng = new Grass( x, y );
//	vegetation.push_back( ng );
	grassTree.insert( ng );

}



float Habitat::consumeFood( int x, int y ){
	// TODO: set up mutual exclusions here

	Organism* gr = grassTree.find( util::coordinate( x, y ) );

	if ( gr == nullptr )
		return 0;
	else {
		grassTree.remove( gr );
//		vegetation.find
		return foodEnergyVal;
	}

}




} /* namespace ecosystem */
