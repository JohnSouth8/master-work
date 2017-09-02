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
//	concepts = std::vector<std::string>( nConcepts );
	state = VectorXf::Zero( nConcepts );		// TODO: rename
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
					break;
				case 'i':
					++nInternal;
					break;
				case 'm':
					++nOutput;
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



void FCM::propagateInput( VectorXf input ) {



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
