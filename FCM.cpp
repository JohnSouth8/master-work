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

FCM::FCM() {}

FCM::FCM( int nc ) {

	nConcepts = nc;
	concepts = std::vector<std::string>( nConcepts );
	FCMap = MatrixXf::Zero( nConcepts, nConcepts );

}

FCM::FCM( int nc, std::vector<std::string> cs ) {

	nConcepts = nc;
	concepts = cs;
	FCMap = MatrixXf::Zero( nConcepts, nConcepts );

}

FCM::~FCM() {
	// TODO Auto-generated destructor stub
}



void FCM::setConcepts( std::vector<std::string> cs ) {
	concepts = cs;
}



void FCM::setFCMap( MatrixXf fcm ) {
	FCMap = fcm;
}



void FCM::loadFCMapFromFile( string filename ) {

	string fcontent = util::readFileContent( filename );

	istringstream strstream( fcontent );
	string buf;

	for ( int i = 0; i < nConcepts; i++ ){
		for ( int j = 0; j < nConcepts; j++ ){
			if ( !strstream.eof() ) {
				strstream >> buf;
				FCMap( i, j ) = atof( buf.c_str() );
			}
		}
	}

}




MatrixXf FCM::getFCMap() {
	return FCMap;
}





} /* namespace ecosystem */
