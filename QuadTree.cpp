/*
 * QuadTree.cpp
 *
 *  Created on: Mar 3, 2018
 *      Author: south
 */


#include <vector>
#include <string>
#include <iostream>


#include "QuadTree.h"
#include "Organism.h"


using ecosystem::Organism;

namespace util {


QuadTree::QuadTree() :
	bucketSize( 0 ),
	start( coordinate( 0, 0 ) ),
	end( coordinate( 1, 1 ) ),
	parent( nullptr ),
	northWest( nullptr ),
	northEast( nullptr ),
	southWest( nullptr ),
	southEast( nullptr )
{}

QuadTree::QuadTree( unsigned int capacity, coordinate upright, coordinate downleft ) :
	bucketSize( capacity ),
	start( upright ),
	end( downleft ),
	parent( nullptr ),
	northWest( nullptr ),
	northEast( nullptr ),
	southWest( nullptr ),
	southEast( nullptr )
{}

QuadTree::QuadTree( unsigned int capacity, coordinate upright, coordinate downleft, QuadTree* parent ) :
	bucketSize( capacity ),
	start( upright ),
	end( downleft ),
	parent( parent ),
	northWest( nullptr ),
	northEast( nullptr ),
	southWest( nullptr ),
	southEast( nullptr )
{}

QuadTree::~QuadTree() {
	delete northWest;
	delete northEast;
	delete southWest;
	delete southEast;
}



bool QuadTree::containsCoordinate( coordinate point ) {

	if ( point.x >= start.x && point.x < end.x && point.y >= start.y && point.y < end.y )
		return true;

	return false;

}



void QuadTree::subdivide() {

	float halfX = start.x + (end.x - start.x) / 2;
	float halfY = start.y + (end.y - start.y) / 2;

	coordinate nw_start = start;
	coordinate nw_end( halfX, halfY );
	northWest = new QuadTree( bucketSize, nw_start, nw_end, this );

	coordinate ne_start( halfX, start.y );
	coordinate ne_end( end.x, halfY );
	northEast = new QuadTree( bucketSize, ne_start, ne_end, this );

	coordinate sw_start( start.x, halfY );
	coordinate sw_end( halfX, end.y );
	southWest = new QuadTree( bucketSize, sw_start, sw_end, this );

	coordinate se_start( halfX, halfY );
	coordinate se_end = end;
	southEast = new QuadTree( bucketSize, se_start, se_end, this );


	// divide contaning organisms among subtrees
	for ( unsigned int i = 0; i < bucket.size(); ++i ) {

		if ( northWest->insert( bucket[i] ) )
			continue;
		if ( northEast ->insert( bucket[i] ) )
			continue;
		if ( southWest->insert( bucket[i] ) )
			continue;
		southEast->insert( bucket[i] );

	}

	// this node has subnodes and not organisms, therefore empty its bucket
	std::vector<Organism*>().swap( bucket );

}



bool QuadTree::merge() {

	// if this is already a leaf node with the bucket
	if ( northWest == nullptr )
		return true;	// do nothing

	// otherwise count the number of members in the child nodes
	unsigned int membersCount = this->count();

	// if too many children, also if this bucket would immediately be full
	if ( membersCount > (bucketSize/2) )
		return true;	// nothing to do here
	else {
		// otherwise gather children from each subtree and delete the subtrees
		bucket = this->getAll();
		delete northWest; northWest = nullptr;
		delete northEast; northEast = nullptr;
		delete southWest; southWest = nullptr;
		delete southEast; southEast = nullptr;
		return true;
	}

	return false;

}



bool QuadTree::insert( Organism* org ) {

	coordinate pos( org->posX, org->posY );

	// check if organism belongs in this node
	if ( !containsCoordinate( pos ) )
		return false;

	// check whether this is a node or a leaf
	if ( northWest == nullptr ) {

		// if leaf with space remaining, add this animat and return
		if ( bucket.size() < bucketSize ) {
			bucket.push_back( org );
			return true;
		}
		else
			subdivide();

	}

	if ( northWest->insert( org ) ) return true;
	if ( northEast->insert( org ) ) return true;
	if ( southWest->insert( org ) ) return true;
	if ( southEast->insert( org ) ) return true;

	return false;

}



// can be safely removed
bool QuadTree::insertDebug( Organism* org ) {

	coordinate pos( org->posX, org->posY );

	// check if organism belongs in this node
	if ( !containsCoordinate( pos ) ) {
		std::cout << "organism does not belong!" << std::endl;
		return false;
	}

	// check whether this is a node or a leaf
	if ( northWest == nullptr ) {

		// if leaf with space remaining, add this animat and return
		if ( bucket.size() < bucketSize ) {
			std::cout << "organism push ...";
			bucket.push_back( org );
			std::cout << " successful!" << std::endl;
			return true;
		}
		else {
			std::cout << "subdividing tree..." << std::endl;
			subdivide();
		}

	}

	std::cout << "inserting into child nodes..." << std::endl;
	if ( northWest->insertDebug( org ) ) return true;
	if ( northEast->insertDebug( org ) ) return true;
	if ( southWest->insertDebug( org ) ) return true;
	if ( southEast->insertDebug( org ) ) return true;

	return false;

}



bool QuadTree::remove( Organism* org ) {

	if ( !containsCoordinate( coordinate( org->posX, org->posY ) ) )
		return false;

	if ( northWest == nullptr ) {

		std::vector<Organism*>::iterator it;
		for ( it = bucket.begin(); it < bucket.end(); ++it ) {

			if ( (*it)->posX == org->posX && (*it)->posY == org->posY ) {

				bucket.erase( it );

				// if the bucket is empty, try to merge parent node
				if ( bucket.empty() == 0 && parent != nullptr && parent->count() < (parent->bucketSize/2) )
					return parent->merge();
				else
					return true;

			}

		}

		return false;	// item was not found

	}

	if ( northWest->remove( org ) )
		return true;

	if ( northEast->remove( org ) )
		return true;

	if ( southWest->remove( org ) )
		return true;

	if ( southEast->remove( org ) )
		return true;

	return false;

}



unsigned int QuadTree::count() {

	int counter = 0;

	if ( northWest == nullptr )
		return bucket.size();

	counter += northWest->count();
	counter += northEast->count();
	counter += southWest->count();
	counter += southEast->count();

	return counter;

}



Organism* QuadTree::find( coordinate pos ) {

	if ( !containsCoordinate( pos ) )
		return nullptr;

	// if we are in a leaf
	if ( northWest == nullptr ) {
		for ( auto org : bucket )
			if ( org->posX == pos.x && org->posY == pos.y )
				return org;
		return nullptr;	// none were found
	}

	// systematically search subtrees
	Organism* org = northWest->find( pos );
	if ( org != nullptr )
		return org;

	org = northEast->find( pos );
	if ( org != nullptr )
		return org;

	org = southWest->find( pos );
	if ( org != nullptr )
		return org;

	org = southEast->find( pos );
	if ( org != nullptr )
		return org;

	return nullptr;

}



std::vector<Organism*> QuadTree::rangeQuery( coordinate r_start, coordinate r_stop, coordinate limit ) {

	// define returning vector
	std::vector<Organism*> results;

	// first handle possible wrapped coordinates by splitting the range and requerying self
	if ( r_start.x > r_stop.x ) {

		coordinate r_start_low ( 0.0, r_start.y );
		coordinate r_stop_low = r_stop;

		coordinate r_start_high = r_start;
		coordinate r_stop_high ( limit.x, r_stop.y );

		std::vector<Organism*> lower_results = this->rangeQuery( r_start_low, r_stop_low, limit );
		std::vector<Organism*> higher_results = this->rangeQuery( r_start_high, r_stop_high, limit );

		if ( lower_results.size() > 0 )
			results.insert( results.end(), lower_results.begin(), lower_results.end() );
		if ( higher_results.size() > 0 )
			results.insert( results.end(), higher_results.begin(), higher_results.end() );

		// return only these results, as they are already made on self and there is no need to go through it again
		return results;

	}
	if ( r_start.y > r_stop.y ) {

		coordinate r_start_low( r_start.x, 0.0 );
		coordinate r_stop_low = r_stop;

		coordinate r_start_high = r_start;
		coordinate r_stop_high( r_stop.x, limit.y );

		std::vector<Organism*> lower_results = this->rangeQuery( r_start_low, r_stop_low, limit );
		std::vector<Organism*> higher_results = this->rangeQuery( r_start_high, r_stop_high, limit );

		if ( lower_results.size() > 0 )
			results.insert( results.end(), lower_results.begin(), lower_results.end() );
		if ( higher_results.size() > 0 )
			results.insert( results.end(), higher_results.begin(), higher_results.end() );

		// return only these results, as they are already made on self and there is no need to go through it again
		return results;

	}


	// SAT check
	if ( start.x > r_stop.x || end.x < r_start.x || start.y > r_stop.y || end.y < r_start.y )
		return results;


	// if we are in a leaf
	if ( northWest == nullptr ) {

		for ( auto org : bucket )
			// if organism is in the queried range
			if ( org->posX >= r_start.x && org->posX < r_stop.x && org->posY >= r_start.y && org->posY < r_stop.y )
				results.push_back( org );

		return results;

	}

	// otherwise, get results from subtrees
	std::vector<Organism*> nw_results = northWest->rangeQuery( r_start, r_stop, limit );
	std::vector<Organism*> ne_results = northEast->rangeQuery( r_start, r_stop, limit );
	std::vector<Organism*> sw_results = southWest->rangeQuery( r_start, r_stop, limit );
	std::vector<Organism*> se_results = southEast->rangeQuery( r_start, r_stop, limit );

	// and append them to results (if they have > 0 elements)
	if ( nw_results.size() > 0 )
		results.insert( results.end(), nw_results.begin(), nw_results.end() );
	if ( ne_results.size() > 0 )
		results.insert( results.end(), ne_results.begin(), ne_results.end() );
	if ( sw_results.size() > 0 )
		results.insert( results.end(), sw_results.begin(), sw_results.end() );
	if ( se_results.size() > 0 )
		results.insert( results.end(), se_results.begin(), se_results.end() );

	return results;

}



std::vector<Organism*> QuadTree::getAll() {

	std::vector<Organism*> results;

	if ( northWest == nullptr ) {

		results.insert( results.end(), bucket.begin(), bucket.end() );

	}
	else {

		std::vector<Organism*> nw_results = northWest->getAll();
		std::vector<Organism*> ne_results = northEast->getAll();
		std::vector<Organism*> sw_results = southWest->getAll();
		std::vector<Organism*> se_results = southEast->getAll();

		if ( nw_results.size() > 0 )
			results.insert( results.end(), nw_results.begin(), nw_results.end() );
		if ( ne_results.size() > 0 )
			results.insert( results.end(), ne_results.begin(), ne_results.end() );
		if ( sw_results.size() > 0 )
			results.insert( results.end(), sw_results.begin(), sw_results.end() );
		if ( se_results.size() > 0 )
			results.insert( results.end(), se_results.begin(), se_results.end() );

	}

	return results;

}



Organism* QuadTree::getRandomOrganism() {

	if ( bucket.size() == 0 && northWest == nullptr )
		return nullptr;

	if ( northWest == nullptr ) {
		int random = floor( RNGESUS->uniformRandomUnitFloat() * bucket.size() );
		return bucket[random];
	}

	int random = floor( RNGESUS->uniformRandomUnitFloat() * 4 );
	if ( random == 0 )
		return northWest->getRandomOrganism();
	if ( random == 1 )
		return northEast->getRandomOrganism();
	if ( random == 2 )
		return southWest->getRandomOrganism();
	if ( random == 3 )
		return southEast->getRandomOrganism();

	return nullptr;

}




void QuadTree::print( int depth ) {

	// print own limits
	std::cout << std::string( depth, '\t' ) << "QT" << depth << " x=(" << start.x << "," << end.x << "], y=(" << start.y << "," << end.y << "]" << std::endl;

	// if we are in a leaf
	if ( northWest == nullptr ) {
		for ( unsigned int i = 0; i < bucket.size(); ++i )
			std::cout << std::string( depth+1, '\t' ) << "- " << bucket[i]->posX << ',' << bucket[i]->posY << std::endl;
	}
	else {

		std::cout << std::string( depth, '\t' ) << "north west subtree: " << std::endl;
		northWest->print( depth+1 );

		std::cout << std::string( depth, '\t' ) << "north east subtree: " << std::endl;
		northEast->print( depth+1 );

		std::cout << std::string( depth, '\t' ) << "south west subtree: " << std::endl;
		southWest->print( depth+1 );

		std::cout << std::string( depth, '\t' ) << "south east subtree: " << std::endl;
		southEast->print( depth+1 );

	}



}




} /* namespace util */
