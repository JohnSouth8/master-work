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
#include "Animat.h"


namespace util {

QuadTree::QuadTree( unsigned int capacity, coordinate upright, coordinate downleft ) {

	bucketSize = capacity;
	start = upright;
	end = downleft;

	northWest = nullptr;
	northEast = nullptr;
	southWest = nullptr;
	southEast = nullptr;

}

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
	northWest = new QuadTree( bucketSize, nw_start, nw_end );

	coordinate ne_start( halfX, start.y );
	coordinate ne_end( end.x, halfY );
	northEast = new QuadTree( bucketSize, ne_start, ne_end );

	coordinate sw_start( start.x, halfY );
	coordinate sw_end( halfX, end.y );
	southWest = new QuadTree( bucketSize, sw_start, sw_end );

	coordinate se_start( halfX, halfY );
	coordinate se_end = end;
	southEast = new QuadTree( bucketSize, se_start, se_end );


	// divide contaning animats among subtrees
	for ( unsigned int i = 0; i < bucket.size(); ++i ) {

		if ( northWest->insert( bucket[i] ) )
			continue;
		if ( northEast ->insert( bucket[i] ) )
			continue;
		if ( southWest->insert( bucket[i] ) )
			continue;
		southEast->insert( bucket[i] );

	}

	// this node has subnodes and not animats, therefore empty its bucket
	std::vector<ecosystem::Animat*>().swap( bucket );

}


bool QuadTree::insert( ecosystem::Animat* ani ) {

	coordinate pos( ani->posX, ani->posY );

	// check if animat belongs in this node
	if ( !containsCoordinate( pos ) )
		return false;

	// check whether this is a node or a leaf
	if ( northWest == nullptr ) {

		// if leaf with space remaining, add this animat and return
		if ( bucket.size() < bucketSize ) {
			bucket.push_back( ani );
			return true;
		}
		else
			subdivide();

	}

	if ( northWest->insert( ani ) ) return true;
	if ( northEast->insert( ani ) ) return true;
	if ( southWest->insert( ani ) ) return true;
	if ( southEast->insert( ani ) ) return true;

	return false;

}



std::vector<ecosystem::Animat*> QuadTree::rangeQuery( coordinate range_start, coordinate range_stop ) {

	std::vector<ecosystem::Animat*> results;

	if ( !containsCoordinate( range_start ) && !containsCoordinate( range_stop ) )
		return results;

	// if we are in a leaf
	if ( northWest == nullptr ) {

		for ( auto ani : bucket )
			// if animat is in the queried range
			if ( ani->posX >= range_start.x && ani->posX < range_stop.x && ani->posY >= range_start.y && ani->posY < range_stop.y )
				results.push_back( ani );

		return results;

	}

	// otherwise, get results from subtrees
	std::vector<ecosystem::Animat*> nw_results = northWest->rangeQuery( range_start, range_stop );
	std::vector<ecosystem::Animat*> ne_results = northEast->rangeQuery( range_start, range_stop );
	std::vector<ecosystem::Animat*> sw_results = southWest->rangeQuery( range_start, range_stop );
	std::vector<ecosystem::Animat*> se_results = southEast->rangeQuery( range_start, range_stop );

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
