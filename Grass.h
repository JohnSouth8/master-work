/*
 * Grass.h
 *
 *  Created on: Mar 12, 2018
 *      Author: south
 */

#ifndef GRASS_H_
#define GRASS_H_

#include "Organism.h"

namespace ecosystem {

class Grass : public Organism {

public:

	Grass();
	Grass( float, float );
	virtual ~Grass();

};

} /* namespace ecosystem */

#endif /* GRASS_H_ */
