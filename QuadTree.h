/*
 * QuadTree.h
 *
 *  Created on: Mar 3, 2018
 *      Author: south
 */

#ifndef QUADTREE_H_
#define QUADTREE_H_


#include <vector>
#include <string>


// forward declaration
namespace ecosystem {
	class Organism;
}

namespace util {

// TODO: standardize this and the one in structs..
struct coordinate {

	float x;
	float y;

	coordinate() : x( 0.0 ), y( 0.0 ) {}
	coordinate( float _x, float _y ) : x( _x ), y( _y ) {}

};


class QuadTree {

public:

	QuadTree();
	QuadTree( unsigned int, coordinate, coordinate );
	virtual ~QuadTree();

	// params
	unsigned int bucketSize;
	coordinate start;
	coordinate end;

	// data
	std::vector<ecosystem::Organism*> bucket;

	// subtrees
	QuadTree* northWest;
	QuadTree* northEast;
	QuadTree* southWest;
	QuadTree* southEast;


	bool containsCoordinate( coordinate );
	void subdivide();

	bool insert( ecosystem::Organism* );

	ecosystem::Organism* find( coordinate );
	std::vector<ecosystem::Organism*> rangeQuery( coordinate, coordinate, coordinate );
	// TODO: optimize with an additional function of move() (requires parent node to be known)

	void print( int );

};

} /* namespace util */

#endif /* QUADTREE_H_ */
