/*
 * Meadow.cpp
 *
 *  Created on: Jan 28, 2018
 *      Author: south
 */

#include "Meadow.h"
#include "Habitat.h"

#include "util.h"

namespace ecosystem {


Meadow::Meadow( int cx, int cy, int r, float gr, Habitat* env ) {

	centerX = cx;
	centerY = cy;
	radius = r;
	growRate = gr;
	environment = env;

}

Meadow::~Meadow() {
	// TODO Auto-generated destructor stub
}


int Meadow::grow() {

	int grownFood = 0;
	float randf = util::randFromUnitInterval();
	int envSizeX = environment->sizeX;
	int envSizeY = environment->sizeY;

	while ( randf > growRate ) {

		int randOffX = util::randIntFrom( 0, radius );
		int randOffY = util::randIntFrom( 0, radius );

		int x = util::getWrappedIndex( centerX + randOffX, envSizeX );
		int y = util::getWrappedIndex( centerY + randOffY, envSizeY );

		if ( environment->foodReserve(x, y) == 0 ) {
			environment->foodReserve(x, y) = 1;
			grownFood += 1;
		}

		randf = util::randFromUnitInterval();
	}

	return grownFood;

}



} /* namespace ecosystem */
