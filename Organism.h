/*
 * Organism.h
 *
 *  Created on: Mar 12, 2018
 *      Author: south
 */

#ifndef ORGANISM_H_
#define ORGANISM_H_

namespace ecosystem {

class Organism {

public:

	Organism() : posX( 0 ), posY( 0 ) {}
	Organism( float px, float py ) : posX( px ), posY( py ) {}
	virtual ~Organism() {};

	float posX;
	float posY;

};

} /* namespace ecosystem */

#endif /* ORGANISM_H_ */
