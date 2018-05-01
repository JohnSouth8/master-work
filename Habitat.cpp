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
Habitat::Habitat() {

	env_ini = util::readSimpleIni( "environment.ini" );

	// basic params
	sizeX = static_cast<int>( env_ini["size_x"] );
	sizeY = static_cast<int>( env_ini["size_y"] );
	foodEnergyVal = static_cast<int>( env_ini["food_energy"] );

	// quad trees
	unsigned int qt_bucketsize = static_cast<unsigned int>( env_ini["quadtree_bucketsize"] );
	coordinate env0( 0, 0 );
	coordinate env1( sizeX, sizeY );
	grassTree = QuadTree( qt_bucketsize, env0, env1 );
	populationTree = QuadTree( qt_bucketsize, env0, env1 );

	// food emanators - meadows
	int n_mdw = static_cast<int>( env_ini["n_meadows"] );
	std::vector<int> xs = FATE->nUniformRandomIntsFrom( n_mdw, 0, sizeX-1 );
	std::vector<int> ys = FATE->nUniformRandomIntsFrom( n_mdw, 0, sizeY-1 );
	std::vector<int> rs = FATE->normalIntsString( n_mdw, env_ini["radius_mean_meadows"], env_ini["radius_std_meadows"] );
	std::vector<float> grs = FATE->normalFloatsString( n_mdw, env_ini["growrate_mean_meadows"], env_ini["growrate_std_meadows"] );

	for ( int i = 0; i < n_mdw; ++i ) {
		Meadow* m = new Meadow( xs[i], ys[i], rs[i], grs[i] );
		meadows.push_back( m );
	}

}

Habitat::Habitat( std::string iniFileName ) {

	env_ini = util::readSimpleIni( iniFileName );

	// basic params
	sizeX = static_cast<int>( env_ini["size_x"] );
	sizeY = static_cast<int>( env_ini["size_y"] );
	foodEnergyVal = static_cast<int>( env_ini["food_energy"] );

	// quad trees
	unsigned int qt_bucketsize = static_cast<unsigned int>( env_ini["quadtree_bucketsize"] );
	coordinate env0( 0, 0 );
	coordinate env1( sizeX, sizeY );
	grassTree = QuadTree( qt_bucketsize, env0, env1 );
	populationTree = QuadTree( qt_bucketsize, env0, env1 );

	// food emanators - meadows
	int n_mdw = static_cast<int>( env_ini["n_meadows"] );
	std::vector<int> xs = FATE->nUniformRandomIntsFrom( n_mdw, 0, sizeX-1 );
	std::vector<int> ys = FATE->nUniformRandomIntsFrom( n_mdw, 0, sizeY-1 );
	std::vector<int> rs = FATE->normalIntsString( n_mdw, env_ini["radius_mean_meadows"], env_ini["radius_std_meadows"] );
	std::vector<float> grs = FATE->normalFloatsString( n_mdw, env_ini["growrate_mean_meadows"], env_ini["growrate_std_meadows"] );

	for ( int i = 0; i < n_mdw; ++i ) {
		Meadow* m = new Meadow( xs[i], ys[i], rs[i], grs[i] );
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

	ani_ini = util::readSimpleIni( animat_iniFileName );

	if ( n_animats == 0 )
		n_animats = static_cast<int>( ani_ini["n_animats"] );

	float avg_size = ani_ini["size"];
	float max_energy = ani_ini["max_energy"];
	float start_energy = ani_ini["start_energy"];
	float max_velocity = ani_ini["max_velocity"];
//	float start_velocity = ini["start_velocity"];
	int max_age = static_cast<int>( ani_ini["max_age"] );
	float vision_range = ani_ini["vision_range"];
	float eye_offset_angle = ani_ini["eye_offset_angle"];
	float eye_vision_angle = ani_ini["eye_vision_angle"];
	float olfactory_range = ani_ini["olfactory_range"];
	float std_degree = ani_ini["init_std_degree"];

	std::vector<float> sizes = FATE->normalFloatsString( n_animats, avg_size, avg_size*std_degree );
	std::vector<float> max_vels = FATE->normalFloatsString( n_animats, max_velocity, max_velocity*std_degree );
	std::vector<int> max_ages = FATE->normalIntsString( n_animats, max_age, max_age*std_degree );
	std::vector<float> vis_ranges = FATE->normalFloatsString( n_animats, vision_range, vision_range*std_degree );
	std::vector<float> eye_angles = FATE->normalFloatsString( n_animats, eye_offset_angle, eye_offset_angle*std_degree );
	std::vector<float> eye_fovs = FATE->normalFloatsString( n_animats, eye_vision_angle, eye_vision_angle*std_degree );
	std::vector<float> olf_ranges = FATE->normalFloatsString( n_animats, olfactory_range, olfactory_range*std_degree);

	std::vector<float> pxs = FATE->nUniformRandomFloatsFrom( n_animats, 0, sizeX );
	std::vector<float> pys = FATE->nUniformRandomFloatsFrom( n_animats, 0, sizeY );
	std::vector<float> dirs = FATE->nUniformRandomFloatsFrom( n_animats, -PI, PI );

	for ( int i = 0; i < n_animats; ++i ) {

		std::string name = generateAnimatName();
		Animat* ani = new Animat (
			name,
			sizes[i],
			max_vels[i],
			max_energy,
			max_ages[i],
			vis_ranges[i],
			eye_angles[i],
			eye_fovs[i],
			olf_ranges[i],
			pxs[i],
			pys[i],
			dirs[i],
			0,
			start_energy
		);

		int nConcepts = 10;	// TODO: make this number automatic
//		ani->initFCM( nConcepts, fcm_concepts_fileName, fcm_fileName );
		ani->initFCMrandom( nConcepts, fcm_concepts_fileName );

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

	int charmin = 65, charmax = 90;
	std::string sname;

	std::vector<int> chars = FATE->nUniformRandomIntsFrom( 10, charmin, charmax );

	for ( int i = 0; i < 10; ++i )
		sname += chars[i];

	return sname;

}



void Habitat::growMeadows() {

	int grown = 0;
	for ( auto meadow : meadows ){

		grown += meadow->grow();

	}

	// fractional chance of a new meadow appearing
	if ( FATE->uniformRandomUnitFloat() < env_ini["probability_new_meadow"] ) {

		int cx = FATE->uniformRandomIntFrom( 0, sizeX-1 );
		int cy = FATE->uniformRandomIntFrom( 0, sizeY-1 );
		int r = FATE->normalInt( env_ini["radius_mean_meadow"], env_ini["radius_std_meadows"] );
		float gr = FATE->normalFloat( env_ini["growrate_mean_meadows"], env_ini["growrate_std_meadows"] );

		Meadow* m = new Meadow( cx, cy, r, gr );
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



bool Habitat::breed( Animat* groom, Animat* bride ) {

	if ( groom->genome.size() != bride->genome.size() )
		return false;

	int geneSize = groom->genome.size();
	float mutationRate = env_ini["mutation_rate"];
	float mutationEffectSTD = env_ini["mutation_std"];

	float distance = distanceBetweenOrganisms( groom, bride );
	if ( distance > groom->reach || distance > bride->reach )
		return false;

	std::vector<float> gene_offspring ( geneSize );

	std::vector<bool> crossover = FATE->bernoulliBooleanString( geneSize, 0.5 );
	std::vector<bool> mutations = FATE->bernoulliBooleanString( geneSize, mutationRate );
	for ( int i = 0; i < geneSize; ++i ) {

		float new_gene;

		if ( crossover[i] )
			new_gene = groom->genome[i];
		else
			new_gene = bride->genome[i];

		if ( mutations[i] ) {
			bool sign = FATE->bernoulliBoolean( 0.5 );
			float value = FATE->normalFloat( new_gene, mutationEffectSTD );
			new_gene = (-1 * (int)sign) * value;
		}

		gene_offspring[i] = new_gene;

	}


	std::string name_offspring = generateAnimatName();
	Animat* offspring = new Animat( name_offspring, gene_offspring );
	birth( offspring );



	return false;

}



float Habitat::distanceBetweenOrganisms( ecosystem::Organism* org1, ecosystem::Organism* org2 ) {

	return util::distanceInPeriodicBoundary( org1->posX, org1->posY, org2->posX, org2->posY, sizeX, sizeY );

}




} /* namespace ecosystem */
