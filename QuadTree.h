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

#include "Chance.h"
#include "structs.h"


extern util::Chance* const RNGESUS;


// forward declaration
namespace ecosystem {
	class Organism;
}

namespace util {


class QuadTree {

public:

	QuadTree();
	QuadTree( unsigned int, coordinate, coordinate );
	QuadTree( unsigned int, coordinate, coordinate, QuadTree* );
	virtual ~QuadTree();

	// params
	unsigned int bucketSize;
	coordinate start;
	coordinate end;
	QuadTree* parent;

	// data
	std::vector<ecosystem::Organism*> bucket;

	// subtrees
	QuadTree* northWest;
	QuadTree* northEast;
	QuadTree* southWest;
	QuadTree* southEast;


	bool containsCoordinate( coordinate );
	void subdivide();
	bool merge();

	bool insert( ecosystem::Organism* );
	bool remove( ecosystem::Organism* );

	unsigned int count();
	ecosystem::Organism* find( coordinate );
	std::vector<ecosystem::Organism*> rangeQuery( coordinate, coordinate, coordinate );
	// TODO: optimize with an additional function of move() (requires parent node to be known)
	std::vector<ecosystem::Organism*> getAll();
	ecosystem::Organism* getRandomOrganism();

	void print( int );

};

} /* namespace util */

#endif /* QUADTREE_H_ */
