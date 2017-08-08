/*
 * SensationModel.h
 *
 *  Created on: Aug 4, 2017
 *      Author: south
 */

#ifndef SENSATIONMODEL_H_
#define SENSATIONMODEL_H_


#include <vector>

#include "Animat.h"
#include "Environment.h"
#include "structs.h"


namespace ecosystem {



class SensationModel {
public:

	// constructors
	SensationModel();
	SensationModel( float, float );
	virtual ~SensationModel();

	// member variables
	float angle;
	float radius;
	std::vector<coord> pObjs;


	// member functions
	void sense( Animat*, Environment* );


};

} /* namespace ecosystem */

#endif /* SENSATIONMODEL_H_ */
