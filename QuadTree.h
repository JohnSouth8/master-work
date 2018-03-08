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
	class Animat;
}

namespace util {


struct coordinate {

	float x;
	float y;

	coordinate() : x( 0.0 ), y( 0.0 ) {}
	coordinate( float _x, float _y ) : x( _x ), y( _y ) {}

};


class QuadTree {

public:

	QuadTree( unsigned int, coordinate, coordinate );
	virtual ~QuadTree();

	// params
	unsigned int bucketSize;
	coordinate start;
	coordinate end;

	// data
	std::vector<ecosystem::Animat*> bucket;

	// subtrees
	QuadTree* northWest;
	QuadTree* northEast;
	QuadTree* southWest;
	QuadTree* southEast;

	bool containsCoordinate( coordinate );
	void subdivide();
	bool insert( ecosystem::Animat* );
	std::vector<ecosystem::Animat*> rangeQuery( coordinate, coordinate );
	void print( int );

};

} /* namespace util */

#endif /* QUADTREE_H_ */
