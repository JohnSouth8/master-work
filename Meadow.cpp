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

// merge this into environment! No need for this to be on its own... Or is there? Don't complicate things unnecessarily

Meadow::Meadow( int cx, int cy, int r, float gr, Habitat* env ) {

	centerX = cx;
	centerY = cy;
	radius = abs( r );		// in case of some random negative outlier
	growRate = fabs( gr );	// in case of some random negative outlier
	environment = env;
	// TODO: implement over-grazing - death of a meadow when no food is left

}

Meadow::~Meadow() {
	// TODO Auto-generated destructor stub
}


int Meadow::grow() {

	int grownFood = 0;
	int envSizeX = environment->sizeX;
	int envSizeY = environment->sizeY;

	while ( environment->fate->uniformRandomUnitFloat() < growRate ) {

//		float offset = environment->fate->linearDescRandomFloatFrom( 0, radius );
		float offset = environment->fate->uniformRandomFloatFrom( 0, radius );
		float angle = environment->fate->uniformRandomFloatFrom( -PI, PI );

		int offsetX = offset * cos( angle );
		int offsetY = offset * sin( angle );

		int x = util::getWrappedIndex( centerX + offsetX, envSizeX );
		int y = util::getWrappedIndex( centerY + offsetY, envSizeY );

//		if ( environment->foodReserve(x, y) == 0 ) {
//			environment->foodReserve(x, y) = 1;
//			environment->growGrass( x, y );
//			grownFood += 1;
//		}

		if ( environment->foodTree.find( util::coordinate( x, y ) ) == nullptr ) {
			environment->foodReserve(x, y) = 1;
			environment->growGrass( x, y );
			grownFood++;
		}

	}

	return grownFood;

}



} /* namespace ecosystem */
