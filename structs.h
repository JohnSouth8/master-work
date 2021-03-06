/*
 * structs.h
 *
 *  Created on: Aug 8, 2017
 *      Author: south
 */

#ifndef STRUCTS_H_
#define STRUCTS_H_


// forward declaration
namespace ecosystem {
	class Organism;
}


namespace util {

struct coordinate {

	float x;
	float y;

	coordinate() : x( 0.0 ), y( 0.0 ) {}
	coordinate( float _x, float _y ) : x( _x ), y( _y ) {}

};

struct stimulus {

	ecosystem::Organism* entity;
	int type;						// 0 - grass, 1 - animat
	float distance;					// distance to sensed organism
	float angle;					// angle to sensed organism

	stimulus() : entity(), type(0), distance( 0.0 ), angle( 0.0 ) {}
	stimulus( ecosystem::Organism* sorg, int t, float d, float a ) : entity( sorg ), type( t ), distance( d ), angle( a ) {}

};

}

#endif /* STRUCTS_H_ */
