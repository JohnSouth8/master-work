/*
 * QuadTree.h
 *
 *  Created on: Mar 2, 2018
 *      Author: south
 */

#ifndef QUADTREE_H_
#define QUADTREE_H_

namespace util {


struct Point {

	int x;
	int y;

	Point( int x1, int y1 ) : x( x1 ), y( y1 ) {}

};

struct AABB {

	Point center;
	int halfSize;

	AABB( Point c, int hs ) : center( c ), halfSize( hs ) {}

	bool containsPoint( Point xy ) {

		if ( abs( xy.x-center.x ) < halfSize && abs( xy.y-center.y ) < halfSize )
			return true;

		return false;

	}

	// TODO: debug this and test... rethink, because it prolly doesn't work
	bool intersectsAABB( AABB* other, int iter ) {

		if ( containsPoint( other->center ) )
			return true;

		Point otherNW( other->center.x - other->halfSize, other->center.y - other->halfSize );
		Point otherNE( other->center.x + other->halfSize, other->center.y - other->halfSize );
		Point otherSW( other->center.x - other->halfSize, other->center.y + other->halfSize );
		Point otherSE( other->center.x + other->halfSize, other->center.y + other->halfSize );

		if ( containsPoint( otherNW ) || containsPoint( otherNE )|| containsPoint( otherSW ) || containsPoint( otherSE ) )
			return true;

		if ( iter == 0 )
			return other->intersectsAABB( this, 1 );

		return false;

	}

};


class QuadTree {

	const int CAPACITY = 4;

	AABB boundary;
	std::vector<Point> data( CAPACITY );

	QuadTree* northWest;
	QuadTree* northEast;
	QuadTree* southWest;
	QuadTree* southEast;

	QuadTree( AABB bnd ) :
		boundary( bnd ),
		northWest( nullptr ),
		northEast( nullptr ),
		southWest( nullptr ),
		southEast( nullptr )
	{}

	void insert( Point xy ) {




	}

	void subdivide() {

	}

	std::vector<Point> rangeQuery( AABB range ) {

		std::vector<Point> points;
		return points;

	}

};

} /* namespace util */

#endif /* QUADTREE_H_ */
