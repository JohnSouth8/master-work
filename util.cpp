/*
 * util.cpp
 *
 *  Created on: Jul 26, 2017
 *      Author: south
 */

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <Eigen/Dense>

#include "util.h"

#include "Organism.h"
#include "Animat.h"
#include "Habitat.h"

using std::string;
using std::istringstream;


namespace util {


	Eigen::VectorXf sigmoid( const Eigen::VectorXf in ) {

		const unsigned int size = in.size();
		Eigen::VectorXf out(size);

		for ( unsigned int i = 0; i < size; ++i ) {
			out(i) = 1.0 / (1 + exp( -in(i) ));
		}

		return out;

	}



	Eigen::VectorXf tanh( const Eigen::VectorXf in ) {

		const unsigned int size = in.size();
		Eigen::VectorXf out( size );

		for ( unsigned int i = 0; i < size; ++i ){
			out(i) = std::tanh( in(i) );
		}

		return out;

	}



	int getWrappedIndex( int index, int domainSize ) {

		int retval = index;

//		if ( index >= 0 && index < domainSize ) {
//			retval = index;
//		}
//		else
		if ( retval < 0 ) {
			retval = domainSize + index;
		}
		else if ( retval >= domainSize ) {
			retval = index % domainSize;
		}

		return retval;

	}



	float getWrappedCoordinate( float coordinate, int domainSize ) {

		double retval = coordinate;

		if ( retval < 0 ) {
			retval = domainSize + coordinate; // note: because it's negative, this is actually subtraction
		}
		while ( retval >= domainSize ) {
			retval -= domainSize;
		}

		return retval;

	}



	float distanceInPeriodicBoundary( float x1, float y1, float x2, float y2, int maxX, int maxY ) {

		float dx = x2 - x1;
		dx = dx - maxX*round( dx / maxX );

		float dy = y2 - y1;
		dy = dy - maxY*round( dy / maxY );

		return sqrt( pow( dx, 2 ) + pow( dy, 2 ) );

	}



	float getAngleBetweenVectors( std::vector<float> v1, std::vector<float> v2 ) {

		float v_angle = atan2( v1[1], v1[0] );
		float g_angle = atan2( v2[1], v2[0] );

		return g_angle - v_angle;

	}



	float getStimulusAngle( ecosystem::Animat* agent, ecosystem::Organism* target ) {

		float maxX = HABITAT->sizeX;
		float maxY = HABITAT->sizeY;
		float dX = target->posX - agent->posX;
		float dY = target->posY - agent->posY;

		// correct deltas if stimulus is over the edge
		if ( dX > agent->visionRange ) {
			if ( target->posX > agent->posX )
				dX = target->posX - (agent->posX + maxX);
			else
				dX = (target->posX + maxX) - agent->posX;
		}
		if ( dY > agent->visionRange ) {
			if ( target->posY > agent->posY )
				dY = target->posY - (agent->posY + maxY);
			else
				dY = (target->posY + maxY) - agent->posY;
		}

		float angle = atan2( dY, dX ) - agent->direction;

		// when angles are more or less to the left of the vertical, atan2 returns large values which
		//  lead to turning in the wrong direction (over the x-axis at 0rad) rather than shorter turns
		//  (over the x-axis at PIrad), thus those values are added to the opposite-signed full circle
		if ( angle > PI )
			angle = -2*PI + angle;
		if ( angle < -PI )
			angle = 2*PI + angle;

		return angle;

	}



	float stimulusVisualActivation( float angle, float distance ) {
		// TODO: subject to testing and change!
//		return (PI - fabs( angle ) / PI) / pow( distance, 2 );
		return (PI - fabs( angle ) / PI) / distance;
	}



	float stimulusOlfactoryActivation( float distance ) {
		// TODO: subject to testing and change, currently just a placeholder
		return 1 / distance;
	}



	std::vector<float> getLineParameters( coordinate point, float angle ) {
		// find parameters to line equation of form y=ax+b

		std::vector<float> params ( 2 );

		float tanang = tan( angle );
		if ( tanang > 1000 )
			tanang = 1000;

		params[0] = -tanang;
		params[1] = point.y + point.x * tanang;

		return params;

	}



	int countNonZeroElements( Eigen::MatrixXf mat ) {

		int rows = mat.rows();
		int cols = mat.cols();
		int count = 0;

		for ( int i = 0; i < rows; ++i ) {
			for ( int j = 0; j < cols; ++j ) {
				if ( mat(i,j) != 0 )
					++count;
			}
		}

		return count;

	}



	std::vector<float> getColourFromGradient( std::vector<float> minColour, std::vector<float> maxColour, float value ) {

		std::vector<float> outColour(3, 0.0f);
		for ( int i = 0; i < 3; ++i ){
			outColour[i] = minColour[i] + value * (maxColour[i] - minColour[i]);
		}

		return outColour;

	}



	bool compareStimuli( stimulus arg1, stimulus arg2 ) {
		return arg1.distance < arg2.distance;
	}



	bool compareAnimatEnergies( ecosystem::Animat* ani1, ecosystem::Animat* ani2 ) {
		return ani1->energy > ani2->energy;
	}



	std::vector<float> constructGenome( std::map<string, float> ini ) {

		int genomeLength = 8 + pow( ini["n_concepts"], 2 );
		std::vector<float> genome( genomeLength );

		int gene = 0;

		genome[gene++] = ini["size"];
		genome[gene++] = ini["reach"];
		genome[gene++] = ini["max_energy"];
		genome[gene++] = ini["max_velocity"];
		genome[gene++] = static_cast<int>( ini["max_age"] );
		genome[gene++] = ini["vision_range"];
		genome[gene++] = ini["eye_angle_offset"];
		genome[gene++] = ini["eye_vision_angle"];
		genome[gene++] = ini["olfactory_range"];


		// read genome weight after weight
		string fcmMask = readFileContent( FCM_MASK_FILE );
		istringstream strstream( fcmMask );
		string buf;

		for ( ; gene < genomeLength; gene++ )
			if ( !strstream.eof() ) {
				strstream >> buf;
				genome[gene] = stoi( buf );
			}

		return genome;

	}



	std::vector<float> constructRandomGenome( std::map<string, float> ini ) {

		int genomeLength = 8 + pow( ini["n_concepts"], 2 );
		std::vector<float> genome( genomeLength );

		// prepare genome randomization
		float avg_size = ini["size"];
		float reach_factor = ini["reach"];
		float max_velocity = ini["max_velocity"];
		int max_age = static_cast<int>( ini["max_age"] );
		float vision_range = ini["vision_range"];
		float eye_offset_angle = ini["eye_angle_offset"];
		float eye_vision_angle = ini["eye_vision_angle"];
		float olfactory_range = ini["olfactory_range"];

		int gene = 0;

		genome[gene++] = RNGESUS->normalFloat( avg_size, avg_size*STD_DEGREE );
		genome[gene++] = RNGESUS->normalFloat( reach_factor, reach_factor*STD_DEGREE );
		genome[gene++] = ini["max_energy"];
		genome[gene++] = RNGESUS->normalFloat( max_velocity, max_velocity*STD_DEGREE );
		genome[gene++] = RNGESUS->normalInt  ( max_age, max_age*STD_DEGREE );
		genome[gene++] = RNGESUS->normalFloat( vision_range, vision_range*STD_DEGREE );
		genome[gene++] = RNGESUS->normalFloat( eye_offset_angle, eye_offset_angle*STD_DEGREE );
		genome[gene++] = RNGESUS->normalFloat( eye_vision_angle, eye_vision_angle*STD_DEGREE );
		genome[gene++] = RNGESUS->normalFloat( olfactory_range, olfactory_range*STD_DEGREE );


		// read genome weight after weight
		string fcmMask = readFileContent( FCM_MASK_FILE );
		istringstream strstream( fcmMask );
		string buf;

		for ( ; gene < genomeLength; gene++ ) {

			if ( !strstream.eof() ) strstream >> buf;
			int mask = stoi( buf );
			if ( mask == 1 )
				genome[gene] = RNGESUS->uniformRandomFloatFrom( -1, 1 );
			else
				genome[gene] = 0;

		}

		return genome;

	}



	string generateName( int length ) {

		int charmin = 65, charmax = 90;
		string sname;

		std::vector<int> chars = RNGESUS->nUniformRandomIntsFrom( length, charmin, charmax );

		for ( int i = 0; i < length; ++i )
			sname += chars[i];

		return sname;

	}



	string readFileContent( string fname ) {

		std::ifstream inputFile;
		inputFile.open( fname.c_str(), std::ifstream::in );

		string fileContent = "";
		string line;
		while ( std::getline( inputFile, line ) ) {
			fileContent += line;
			fileContent += '\n';
		}

		inputFile.close();
		return fileContent;

	}



	std::map<string, float> readSimpleIni( string fname ) {

		std::ifstream inputFile;
		inputFile.open( fname.c_str(), std::ifstream::in );

		std::map<string, float> retmap;

		string line;
		while ( std::getline( inputFile, line ) ) {

			int eqPos = line.find( "=" );
			if ( eqPos == -1 )
				continue;		// ignore lines without '='˘

			string keyword = line.substr( 0, eqPos );
			string value = line.substr( eqPos + 1 );
			float fval = atof( value.c_str() );

			retmap[keyword] = fval;

		}

		inputFile.close();
		return retmap;

	}



	void cleanFile( string fname ) {
		std::ofstream outputFile;
		outputFile.open( fname.c_str(), std::ofstream::out | std::ofstream::trunc );
		outputFile.close();
	}



	void printMatrixToFile( Eigen::MatrixXf data, string fname, bool transpose ) {

		Eigen::MatrixXf outputData = data;
		if ( transpose )
			// print transposed version of the matrix because matrix with origin in upper-left is mirrored in coordinate system with origin in lower-left
			// 		i.e. row-major order is not compatible with x-axis first order in coordinate system
			outputData = data.transpose();

		std::ofstream outputFile;
		outputFile.open( fname.c_str() );
		outputFile << outputData;
		outputFile.close();
	}



	void printSensationsToFile( std::vector<stimulus> sss, string fname ) {
		std::ofstream outputFile;
		outputFile.open( fname.c_str(), std::ofstream::out | std::ofstream::app );

		std::vector<stimulus>::iterator it;

		for ( it = sss.begin(); it != sss.end(); ++it ) {
			outputFile << it->entity->posX << " " << it->entity->posY << std::endl;
		}

		outputFile.close();
	}



	void printAnimatLocationsToFile( std::map<const char*, ecosystem::Animat*> pop, string fname ) {
		std::ofstream outputFile;
		outputFile.open( fname.c_str(), std::ofstream::out | std::ofstream::app );

		std::map<const char*, ecosystem::Animat*>::iterator it;

		for ( it = pop.begin(); it != pop.end(); ++it ) {
			outputFile << it->second->posX << " " << it->second->posY << std::endl;
		}

		outputFile.close();
	}



}


