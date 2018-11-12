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
using std::string;


namespace ecosystem {

/* constructors */
Habitat::Habitat() {

	env_ini = util::readSimpleIni( ENVIRONMENT_INI );

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
	std::vector<int> xs = RNGESUS->nUniformRandomIntsFrom( n_mdw, 0, sizeX-1 );
	std::vector<int> ys = RNGESUS->nUniformRandomIntsFrom( n_mdw, 0, sizeY-1 );
	std::vector<int> rs = RNGESUS->normalIntsString( n_mdw, env_ini["radius_mean_meadows"], env_ini["radius_std_meadows"] );
	std::vector<float> grs = RNGESUS->normalFloatsString( n_mdw, env_ini["growrate_mean_meadows"], env_ini["growrate_std_meadows"] );

	for ( int i = 0; i < n_mdw; ++i ) {
		Meadow* m = new Meadow( xs[i], ys[i], rs[i], grs[i] );
		meadows.push_back( m );
	}

}

Habitat::Habitat( string iniFileName ) {

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
	std::vector<int> xs = RNGESUS->nUniformRandomIntsFrom( n_mdw, 0, sizeX-1 );
	std::vector<int> ys = RNGESUS->nUniformRandomIntsFrom( n_mdw, 0, sizeY-1 );
	std::vector<int> rs = RNGESUS->normalIntsString( n_mdw, env_ini["radius_mean_meadows"], env_ini["radius_std_meadows"] );
	std::vector<float> grs = RNGESUS->normalFloatsString( n_mdw, env_ini["growrate_mean_meadows"], env_ini["growrate_std_meadows"] );

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


void Habitat::populateWorld( int n_animats ) {

	ani_ini = util::readSimpleIni( ANIMAT_INI );

	if ( n_animats == 0 )
		n_animats = static_cast<int>( ani_ini["n_animats"] );

	std::vector<float> pxs = RNGESUS->nUniformRandomFloatsFrom( n_animats, 0, sizeX );
	std::vector<float> pys = RNGESUS->nUniformRandomFloatsFrom( n_animats, 0, sizeY );
	std::vector<float> dirs = RNGESUS->nUniformRandomFloatsFrom( n_animats, -PI, PI );

	std::vector<float> ancestor_genome = util::constructGenome( ani_ini );

	for ( int i = 0; i < n_animats; ++i ) {
		birth( ancestor_genome, coordinate( pxs[i], pys[i] ), dirs[i] );
	}

}


void Habitat::populateWorldOld( int n_animats, string animat_iniFileName, string fcm_concepts_fileName, string fcm_fileName ) {

	ani_ini = util::readSimpleIni( animat_iniFileName );

	if ( n_animats == 0 )
		n_animats = static_cast<int>( ani_ini["n_animats"] );

	float avg_size = ani_ini["size"];
	float max_energy = ani_ini["max_energy"];
	float start_energy = ani_ini["start_energy"];
	float max_velocity = ani_ini["max_velocity"];
	int max_age = static_cast<int>( ani_ini["max_age"] );
	float vision_range = ani_ini["vision_range"];
	float eye_offset_angle = ani_ini["eye_offset_angle"];
	float eye_vision_angle = ani_ini["eye_vision_angle"];
	float olfactory_range = ani_ini["olfactory_range"];

	std::vector<float> sizes = RNGESUS->normalFloatsString( n_animats, avg_size, avg_size*STD_DEGREE );
	std::vector<float> max_vels = RNGESUS->normalFloatsString( n_animats, max_velocity, max_velocity*STD_DEGREE );
	std::vector<int> max_ages = RNGESUS->normalIntsString( n_animats, max_age, max_age*STD_DEGREE );
	std::vector<float> vis_ranges = RNGESUS->normalFloatsString( n_animats, vision_range, vision_range*STD_DEGREE );
	std::vector<float> eye_angles = RNGESUS->normalFloatsString( n_animats, eye_offset_angle, eye_offset_angle*STD_DEGREE );
	std::vector<float> eye_fovs = RNGESUS->normalFloatsString( n_animats, eye_vision_angle, eye_vision_angle*STD_DEGREE );
	std::vector<float> olf_ranges = RNGESUS->normalFloatsString( n_animats, olfactory_range, olfactory_range*STD_DEGREE);

	std::vector<float> pxs = RNGESUS->nUniformRandomFloatsFrom( n_animats, 0, sizeX );
	std::vector<float> pys = RNGESUS->nUniformRandomFloatsFrom( n_animats, 0, sizeY );
	std::vector<float> dirs = RNGESUS->nUniformRandomFloatsFrom( n_animats, -PI, PI );

	for ( int i = 0; i < n_animats; ++i ) {

		string name = util::generateName( 10 );
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

//		ani->initFCM( nConcepts, fcm_concepts_fileName, fcm_fileName );
//		ani->initFCMrandom( nConcepts, fcm_concepts_fileName );

		population[name] = ani;

	}

}


void Habitat::birth( std::vector<float> genome, coordinate location, float direction ) {

	string name = util::generateName( 10 );
	Animat* ani = new Animat( name, genome, location.x, location.y, direction );
	populationTree.insert( ani );
	population[ani->name] = ani;	// staged to be deprecated

	// debug
	int n = populationTree.count();
	std::cout << "Born animat number "<< n << ":\t";
	ani->toString();

}



void Habitat::death( Animat* ani ) {

	string name = ani->name;
	populationTree.remove( ani );
	delete population[name];
	population.erase( name );

}



void Habitat::deathOld( string name ) {

	delete population[name];
	population.erase( name );

}



void Habitat::growMeadows() {

	int grown = 0;
	for ( auto meadow : meadows ){

		grown += meadow->grow();

	}

	// fractional chance of a new meadow appearing
	if ( RNGESUS->uniformRandomUnitFloat() < env_ini["probability_new_meadow"] ) {

		int cx = RNGESUS->uniformRandomIntFrom( 0, sizeX-1 );
		int cy = RNGESUS->uniformRandomIntFrom( 0, sizeY-1 );
		int r = RNGESUS->normalInt( env_ini["radius_mean_meadow"], env_ini["radius_std_meadows"] );
		float gr = RNGESUS->normalFloat( env_ini["growrate_mean_meadows"], env_ini["growrate_std_meadows"] );

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

	// distance is not important yet
//	float distance = distanceBetweenOrganisms( groom, bride );
//	if ( distance > groom->reach || distance > bride->reach )
//		return false;

	std::vector<float> gene_offspring ( geneSize );

	std::vector<bool> crossover = RNGESUS->bernoulliBooleanString( geneSize, 0.5 );
	std::vector<bool> mutations = RNGESUS->bernoulliBooleanString( geneSize, mutationRate );
	for ( int i = 0; i < geneSize; ++i ) {

		float new_gene;

		if ( crossover[i] )
			new_gene = groom->genome[i];
		else
			new_gene = bride->genome[i];

		if ( mutations[i] ) {
			bool sign = RNGESUS->bernoulliBoolean( 0.5 );
			float value = RNGESUS->normalFloat( new_gene, mutationEffectSTD );
			new_gene = (-1 * (int)sign) * value;
		}

		gene_offspring[i] = new_gene;

	}

	birth( gene_offspring, coordinate( bride->posX, bride->posY ), bride->direction );
	return true;

}



float Habitat::distanceBetweenOrganisms( ecosystem::Organism* org1, ecosystem::Organism* org2 ) {

	return util::distanceInPeriodicBoundary( org1->posX, org1->posY, org2->posX, org2->posY, sizeX, sizeY );

}




} /* namespace ecosystem */
