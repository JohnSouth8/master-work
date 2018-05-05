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

#include "structs.h"


// forward declaration
namespace ecosystem {
	class Organism;
}

namespace util {


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
	bool remove( ecosystem::Organism* );

	int count();
	ecosystem::Organism* find( coordinate );
	std::vector<ecosystem::Organism*> rangeQuery( coordinate, coordinate, coordinate );
	// TODO: optimize with an additional function of move() (requires parent node to be known)
	std::vector<ecosystem::Organism*> getAll();

	void print( int );

};

} /* namespace util */

#endif /* QUADTREE_H_ */
