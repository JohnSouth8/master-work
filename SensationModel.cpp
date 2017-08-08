/*
 * SensationModel.cpp
 *
 *  Created on: Aug 4, 2017
 *      Author: south
 */

#include <cmath>

#include "SensationModel.h"
#include "structs.h"

namespace ecosystem {

SensationModel::SensationModel() {

	radius = 1;
	angle = M_PI;

}

SensationModel::SensationModel( float r, float a ) {

	radius = r;
	angle = a;

}

SensationModel::~SensationModel() {
	// TODO Auto-generated destructor stub
}




void SensationModel::sense( Animat* self, Environment* env ) {

	float cx = self->posX;
	float cy = self->posY;
//	float dir = self->direction;

	// check all points in local (+- r squarely) neighbourhood if they are inside the circle
	// TODO: take care of edges!!
	int x_max = std::ceil( cx + radius );
	int y_max = std::ceil( cy + radius );
	int x_min = std::ceil( cx - radius );
	int y_min = std::ceil( cy - radius );

	// TODO: debug this!
	for ( int i = x_min; i <= x_max; i++ ) {
		for ( int j = y_min; j <= y_max; j++ ) {
			if ( env->foodReserve(i, j) == 1 ) {

				float x_sq = pow( (i - cx), 2 );
				float y_sq = pow( (j - cy), 2 );

				if ( x_sq + y_sq <= pow( radius, 2 ) ) {
					coord co = { i, j };
					pObjs.push_back( co );
				}
			}
		}
	}

}


} /* namespace ecosystem */
