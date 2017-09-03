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
	state = VectorXd::Zero( nConcepts );		// TODO: rename
	L = MatrixXd::Zero( nConcepts, nConcepts );

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



void FCM::setState( VectorXd fcm ) {
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



void FCM::applySensations( VectorXd sensations ) {

	// sensations is not the input vector
	if ( sensations.size() != nInput )
		return;

	// put sensations into current state
	for ( int i = 0; i < nInput; ++i ) {
		state(i) = sensations(i);
	}

//	VectorXd nonSensorState = state.tail( nInternal + nOutput );
//	VectorXd input(nConcepts);
//	input << sensations, nonSensorState;

	// delta = link^T * state
	// newstate = transform(state + delta)
	MatrixXd Lt = L.transpose();
	VectorXd dS = Lt * state;
	state = util::tanh( state + dS );

}



VectorXd FCM::getOutput() {
	return state.tail( nOutput );
}




VectorXd FCM::getState() {
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
