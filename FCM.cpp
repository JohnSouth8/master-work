/*
 * FCM.cpp
 *
 *  Created on: Aug 20, 2017
 *      Author: south
 */


#include <iostream>


#include "FCM.h"

#include "util.h"
#include "Animat.h"
#include "Habitat.h"


using namespace std;
using namespace Eigen;

namespace ecosystem {

FCM::FCM() {

	// reads concepts and sets their numbers
	loadConceptsFromFCMFile();

	state = VectorXf::Zero( nConcepts );
	L = MatrixXf::Zero( nConcepts, nConcepts );

}

FCM::FCM( int nc ) {

	nConcepts = nc;
//	nInput = 0;
//	nInternal = 0;
//	nOutput = 0;
	state = VectorXf::Zero( nConcepts );		// TODO: rename. Edit: why? Edit2: don't know anymore..
	L = MatrixXf::Zero( nConcepts, nConcepts );

}



FCM::~FCM() {
	// TODO Auto-generated destructor stub
}



//void FCM::setConcepts( std::vector<string> cs ) {
//	concepts = cs;
//}



void FCM::loadConceptsFromFCMFile() {

	string fcontent = util::readFileContent( FCM_CONCEPTS_FILE );

	istringstream strstream( fcontent );
	string buf;

	int i = 0;
	while ( strstream >> buf ) {

		concepts[buf] = i;
		char type = buf[0];
		switch ( type ) {
			case 's':
				++nSensory;
				sensoryConceptIdxs.push_back( i );
				break;
			case 'i':
				++nMental;
				mentalConceptIdxs.push_back( i );
				break;
			case 'm':
				++nMotor;
				motorConceptIdxs.push_back( i );
				break;
		}

		//debug:
		std::cout << buf << std::endl;

		i++;

	}

	nConcepts = i;
}



void FCM::setState( VectorXf fcm ) {
	state = fcm;
}



void FCM::loadLinkMatrixFromFile( string filename ) {

	string fcontent = util::readFileContent( filename );

	istringstream strstream( fcontent );
	string buf;

	for ( int i = 0; i < nConcepts; ++i ){
		for ( int j = 0; j < nConcepts; ++j ){
			if ( !strstream.eof() ) {
				strstream >> buf;
				L(i, j) = stof( buf );
			}
		}
	}

}



void FCM::setRandomLinkMatrix( float p_nonzero ) {

	int nLinks = pow( nConcepts, 2 );
	std::vector<bool> nonZeroPool = RNGESUS->bernoulliBooleanString( nLinks, p_nonzero );

	for ( int i = 0; i < nLinks; ++i ) {
		if ( nonZeroPool[i] ) {
			float link = RNGESUS->uniformRandomUnitFloat();
			int row = i / nConcepts;
			int col = i % nConcepts;
			L(row, col) = link;
		}
	}

}



void FCM::setLinkMatrixFromMask( std::vector<float> genome, int maskStart ) {

	int gene = maskStart;

	for ( int row = 0; row < nConcepts; ++row ) {
		for ( int col = 0; col < nConcepts; ++col ) {

			if ( genome[gene++] != 0 )
				L(row, col) = RNGESUS->uniformRandomFloatFrom( -1, 1 );

		}
	}

}



void FCM::applySensations( VectorXf sensations ) {

	// if sensations is not the input vector
	if ( sensations.size() != nSensory )
		return;

	// put sensations into current state
	for ( int i = 0; i < nSensory; ++i ) state(i) = sensations(i);

	// delta = state^T * L
	// newstate = transform(state + delta)
	VectorXf dS = state.transpose() * L;
	state = util::tanh( state + dS );		// << modified Kosko rule: A(t+1) = f(A(t) + dA); f:= tanh

	// TODO: maybe more than iteration with the same sensations will be needed to reach a decision

}



VectorXf FCM::getOutput() {
	return state.tail( nMotor );
}





} /* namespace ecosystem */
