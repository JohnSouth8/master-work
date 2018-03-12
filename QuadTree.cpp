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
	northWest( nullptr ),
	northEast( nullptr ),
	southWest( nullptr ),
	southEast( nullptr )
{}

QuadTree::QuadTree( unsigned int capacity, coordinate upright, coordinate downleft ) :
	bucketSize( capacity ),
	start( upright ),
	end( downleft ),
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
