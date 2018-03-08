/*
 * FCM.cpp
 *
 *  Created on: Aug 20, 2017
 *      Author: south
 */

#include "FCM.h"

#include "util.h"


using namespace std;
using namespace Eigen;

namespace ecosystem {

FCM::FCM() {
	nConcepts = 0;
	nInput = 0;
	nInternal = 0;
	nOutput = 0;
}

FCM::FCM( int nc ) {

	nConcepts = nc;
	nInput = 0;
	nInternal = 0;
	nOutput = 0;
//	concepts = std::vector<string>( nConcepts );
	state = VectorXf::Zero( nConcepts );		// TODO: rename. Edit: why? Edit2: don't know anymore..
	L = MatrixXf::Zero( nConcepts, nConcepts );

}


FCM::~FCM() {
	// TODO Auto-generated destructor stub
}



//void FCM::setConcepts( std::vector<string> cs ) {
//	concepts = cs;
//}



void FCM::loadConceptsFromFile( string filename ) {

	string fcontent = util::readFileContent( filename );

	istringstream strstream( fcontent );
	string buf;

	for ( int i = 0; i < nConcepts; ++i ) {
		if ( !strstream.eof() ) {
			strstream >> buf;
			concepts.push_back( buf );
			char type = buf[0];
			switch ( type ) {
				case 's':
					++nInput;
					inputConcepts.push_back( i );
					break;
				case 'i':
					++nInternal;
					internalConcepts.push_back( i );
					break;
				case 'm':
					++nOutput;
					outputConcepts.push_back( i );
					break;
			}
		}
	}
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
				L( i, j ) = atof( buf.c_str() );
			}
		}
	}

}



void FCM::applySensations( VectorXf sensations ) {

	// if sensations is not the input vector
	if ( sensations.size() != nInput )
		return;

	// put sensations into current state
	for ( int i = 0; i < nInput; ++i ) {
		state(i) = sensations(i);
	}

	// delta = state^T * L
	// newstate = transform(state + delta)
	VectorXf dS = state.transpose() * L;
	state = util::tanh( state + dS );		// << modified Kosko rule: A(t+1) = f(A(t) + dA); f:= tanh

	// TODO: maybe more than iteration with the same sensations will be needed to reach a decision

}



VectorXf FCM::getOutput() {
	return state.tail( nOutput );
}




VectorXf FCM::getState() {
	return state;
}




int FCM::getNConcepts() {
	return nConcepts;
}




int FCM::getNInput() {
	return nInput;
}




int FCM::getNInternal() {
	return nInternal;
}




int FCM::getNOutput() {
	return nOutput;
}





} /* namespace ecosystem */
