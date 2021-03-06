/*
 * util.h
 *
 *  Created on: Jul 26, 2017
 *      Author: south
 */

#ifndef UTIL_H_
#define UTIL_H_


#include <vector>
#include <string>
#include <map>
#include <Eigen/Dense>

#include "structs.h"


extern const float PI;
extern const float STD_DEGREE;
extern const std::string FCM_MASK_FILE;

namespace ecosystem {
	class Organism;
	class Animat;
	class Habitat;
}


namespace util {

	Eigen::VectorXf sigmoid( Eigen::VectorXf );
	Eigen::VectorXf tanh( Eigen::VectorXf );

	int getWrappedIndex( int, int );
	float getWrappedCoordinate( float, int );
	float distanceInPeriodicBoundary( float, float, float, float, int, int );

	float getAngleBetweenVectors( std::vector<float>, std::vector<float> );
	float getStimulusAngle( ecosystem::Animat*, ecosystem::Organism* );
	std::vector<float> getLineParameters( coordinate, float );

	float stimulusVisualActivation( float, float );
	float stimulusOlfactoryActivation( float );
	bool compareStimuli( stimulus, stimulus );
	bool compareAnimatEnergies( ecosystem::Animat*, ecosystem::Animat* );

	std::vector<float> constructGenome( std::map<std::string, float> );
	std::vector<float> constructRandomGenome( std::map<std::string, float> );

	int countNonZeroElements( Eigen::MatrixXf );

	std::vector<float> getColourFromGradient( std::vector<float>, std::vector<float>, float );

	std::string generateName( uint );

	std::string readFileContent( std::string );
	std::map<std::string, float> readSimpleIni( std::string );
	void cleanFile ( std::string );
	void printMatrixToFile( Eigen::MatrixXf, std::string, bool = true );
	void printSensationsToFile( std::vector<stimulus>, std::string );
	void printAnimatLocationsToFile( std::map<const char*, ecosystem::Animat*>, std::string );

}



#endif /* UTIL_H_ */
