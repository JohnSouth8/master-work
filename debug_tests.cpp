/*
 * debug_tests.cpp
 *
 *  Created on: May 1, 2018
 *      Author: south
 */


#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <map>
#include <random>
#include <ctime>
#include <chrono>
#include <cmath>
#include <array>
#include <thread>
#include <algorithm>
#include <Eigen/Dense>

#include "debug_tests.h"
#include "util.h"
#include "Chance.h"
#include "QuadTree.h"
#include "graphx.h"



#include "FCM.h"
#include "Organism.h"
#include "Grass.h"
#include "Animat.h"
#include "Habitat.h"




using namespace std;
using namespace Eigen;
using namespace ecosystem;
using util::coordinate;





extern GLFWwindow* simWindow;
extern GLFWwindow* fcmWindow;
extern bool simulationProceed;


namespace ecosystem {



int test_whatever() {

	FCM f = FCM();

	cout << "brain completed!" << endl;
	cout << "number of concepts: " << f.nConcepts << endl;
	cout << "sensory: " << f.nSensory << ", mental: " << f.nMental << ", motor: " << f.nMotor << endl;


	return 0;

}



int test_randomness() {

	int n_stars = 100;
	int n_rolls = 10000;


	int testBool[2] = {0};
	int testInt[10] = {0};
	int testFloat[10] = {0};
	int testLinear[10] = {0};
	int testNormal[10] = {0};
	int testNormal2[10] = {0};
	int index = 0;

	for ( int i = 0; i < n_rolls; i++ ) {

		bool randbool = RNGESUS->randomBoolean();
		++testBool[randbool];

		int randint = RNGESUS->uniformRandomIntFrom( 0, 100 );
		index = (int) floor( randint / 10 );
		++testInt[index];

		float randf = RNGESUS->uniformRandomUnitFloat();
		index = (int) floor( randf * 10 );
		++testFloat[index];

		float randlin = RNGESUS->linearDescRandomFloatFrom( 0.0, 10.0 );
		index = (int) floor( randlin );
		++testLinear[index];

		float randGauss = RNGESUS->normalFloat( 10, 0.1 );
		if ( randGauss > 0.0 && randGauss < 20.0 ) {
			index = (int) floor( randGauss / 2 );
			++testNormal[index];
		}
	}

	vector<float> rands = RNGESUS->normalFloatsString( n_rolls, 5.0, 3.0 );
	for ( auto rnd : rands ) {
		if ( rnd > 0.0 && rnd < 10.0 ) {
			index = (int) floor( rnd );
			++testNormal2[index];
		}
	}

	cout << "Uniform boolean distribution results:" << endl;
	for ( int i = 0; i < 2; ++i ) {
		cout << i << ":\t";
		cout << string( testInt[i]*n_stars/n_rolls, '*' ) << endl;
	}

	cout << "Uniform integer distribution from 0 to 100 results:" << endl;
	for ( int i = 0; i < 10; ++i ) {
		cout << i*10 << "-" << (i+1)*10 << ":\t";
		cout << string( testInt[i]*n_stars/n_rolls, '*' ) << endl;
	}

	cout << "Uniform real distribution from 0.0 to 1.0 results:" << endl;
	for ( int i = 0; i < 10; ++i ) {
		cout << i*0.1 << "-" << (i+1)*0.1 << ":\t";
		cout << string( testFloat[i]*n_stars/n_rolls, '*' ) << endl;
	}

	cout << "Descending linear distribution from 0 to 10 results:" << endl;
	for ( int i = 0; i < 10; ++i ) {
		cout << i << "-" << (i+1) << ":\t";
		cout << string( testLinear[i]*n_stars/n_rolls, '*' ) << endl;
	}

	cout << "Normal distribution with m=10 and std=0.1 results: " << endl;
	for ( int i = 0; i < 10; ++i ) {
		cout << (float)i*2 << "-" << (float)(i+1)*2 << ":\t";
		cout << string( testNormal[i]*n_stars/n_rolls, '*' ) << endl;
	}

	cout << "Normal distribution with m=5.0 and std=2.2 results: " << endl;
	for ( int i = 0; i < 10; ++i ) {
		cout << i << "-" << (i+1) << ":\t";
		cout << string( testNormal2[i]*n_stars/n_rolls, '*' ) << endl;
	}

	return 0;

}



int test_quadTreeAnimats() {


	int n_animats = 500;
//	int e_size = 1000;
	int n_caps = 20;
	int n_tests = 100;

	long test_results[n_caps][n_tests][2];

	Habitat env ( ENVIRONMENT_INI );

	// init animats
	env.populateWorld( n_animats );


	for ( int i = 0; i < n_caps; ++i ) {

		for ( int j = 0; j < n_tests; ++j ) {


			auto start = chrono::steady_clock::now();

			util::QuadTree qt( i+1, util::coordinate( 0.0, 0.0 ), util::coordinate( static_cast<float>( env.sizeX ), static_cast<float>( env.sizeY ) ) );
//			unsigned int count = 0;
			for ( auto &nm_ani : env.population ) {
				qt.insert( nm_ani.second );
//				bool ins = qt.insert( nm_ani.second );
//				if ( ins )
//					++count;
			}
//			if ( count == env.population.size() )
//				cout << "all animats succesfully added to the tree" << endl;

			auto end = chrono::steady_clock::now();
			auto elapsed = chrono::duration_cast<chrono::microseconds>( end - start );
//			cout << "Quad tree construction: " << elapsed.count() << " us" << endl << endl;

			test_results[i][j][0] = static_cast<long>( elapsed.count() );

		//	qt.print( 0 );


			// select an animat at the edges
			Animat* ani;
		////	int ani_no = 0;
		//	for ( auto &nm_ani : env.population ) {
		//		ani = nm_ani.second;
		//		if ( ani->posX - ani->visionRange < 0 || ani->posX + ani->visionRange > env.sizeX
		//				|| ani->posY - ani->visionRange < 0 || ani->posY + ani->visionRange > env.sizeY )
		//			break;
		////		ani_no++;
		//	}

			//	Animat* ani = env.population.begin()->second;


			auto start2 = chrono::steady_clock::now();

			// compute distances from all the animats on the list
			for ( auto &nm_ani : env.population ) {

				ani = nm_ani.second;

				util::coordinate rng_start ( util::getWrappedCoordinate( ani->posX - ani->visionRange, env.sizeX ),
											 util::getWrappedCoordinate( ani->posY - ani->visionRange, env.sizeY ) );
				util::coordinate rng_end ( util::getWrappedCoordinate( ani->posX + ani->visionRange, env.sizeX ),
										   util::getWrappedCoordinate( ani->posY + ani->visionRange, env.sizeY ) );
				util::coordinate limits ( env.sizeX, env.sizeY );

				vector<Organism*> closest = qt.rangeQuery( rng_start, rng_end, limits );

//				if ( closest.empty() ) {
//					cout << "Failed query for animat [" << ani->posX << "," << ani->posY << "] !" << endl;
//					closest = qt.rangeQuery( rng_start, rng_end, limits );
//				}

		//		cout << "Animat at " << ani->posX << "," << ani->posY << " sees (r=" << ani->visionRange << ") animats:" << endl;
		//		for ( auto cani : closest ) {
		//			// distance computation resistant to wrapped coordinates
		//			float dx = ani->posX-cani->posX;
		//			dx = dx - env.sizeX*round( dx / env.sizeX );
		//			float dy = ani->posY-cani->posY;
		//			dy = dy - env.sizeY*round( dy / env.sizeY );
		//			float dist = sqrt( pow( dx, 2 ) + pow( dy, 2 ) );
		//			cout << " - animat at " << cani->posX << "," << cani->posY << ", distance = " << dist << endl;
		//		}

			}

			auto end2 = chrono::steady_clock::now();
			elapsed = chrono::duration_cast<chrono::microseconds>( end2 - start2 );
//			cout << "Ranges of all animats query: " << elapsed.count() << " us" << endl << endl;

			test_results[i][j][1] = static_cast<long>( elapsed.count() );

		}

	}

	for ( int i = 0; i < n_caps; ++i ) {

		long sum_constr = 0;
		long sum_query = 0;

		for ( int j = 0; j < n_tests; ++j ) {
			sum_constr += test_results[i][j][0];
			sum_query += test_results[i][j][1];
		}

		double mean_constr = (double)sum_constr / n_tests;
		double mean_query = ( (double)sum_query / n_tests ) / (double)n_animats;

		cout << "Bucket capacity " << i+1 << ":" << endl;
		cout << "\t - mean construction time: " << mean_constr << endl;
		cout << "\t - mean range query time: " << mean_query << endl;

	}

//	cout << endl << endl << endl;
//
//
//	vector<float> xps;
//	vector<float> yps;
//
//	for ( auto &nm_ani : env.population ) {
//		xps.push_back( nm_ani.second->posX );
//		yps.push_back( nm_ani.second->posY );
//	}
//
//	MatrixXf ds = MatrixXf::Zero( n_tests, n_tests );
//
//	auto start3 = chrono::steady_clock::now();
//
//	for ( int i = 0; i < n_tests; ++i ) {
//		for ( int j = 0; j < n_tests; ++j ) {
//
//			if ( i == j )
//				continue;
//
//			float d = sqrt( pow( xps[i] - xps[j], 2 ) + pow( yps[i] - yps[j], 2 ) );
//			ds( i, j ) = d;
//			ds( j, i ) = d;
//
//		}
//	}
//
//	auto end3 = chrono::steady_clock::now();
//	elapsed = chrono::duration_cast<chrono::microseconds>( end3 - start3 );
//	cout << "Distances computation: " << elapsed.count() << " ms" << endl << endl;
//
//
//	auto start4 = chrono::steady_clock::now();
//	vector<int> idxs;
//	for ( int i = 0; i < n_tests; ++i )
//		if ( ds(ani_no, i) < ani->visionRange && ds(ani_no, i) > 0 )
//			idxs.push_back( i );
//
//	if ( idxs.size() > 0 )
//		cout << "Animats less than " << ani->visionRange << "m from this animat(" << ani->posX << "," << ani->posY << "):" << endl;
//	for ( int i = 0; i < (int) idxs.size(); ++i )
//		cout << "\t - " << xps[idxs[i]] << ", "<< yps[idxs[i]] << ", d=" << ds(ani_no, idxs[i]) << endl;
//
//	cout << endl;
//	auto end4 = chrono::steady_clock::now();
//	elapsed = chrono::duration_cast<chrono::microseconds>( end4 - start4 );
//	cout << "Closest search: " << elapsed.count() << " ms" << endl << endl;


	return 0;

}



int test_quadTreeFood() {


	auto start = chrono::steady_clock::now();

	Habitat env( ENVIRONMENT_INI );

	// preinit the food
	for ( int i = 0; i < 5000; ++i ) {
		env.growMeadows();
	}


	auto tm1 = chrono::steady_clock::now();
	auto elapsed = chrono::duration_cast<chrono::microseconds>( tm1 - start );

//	auto tmp1 = chrono::steady_clock::now();
//	cout << "amount of vegetation: " << env.vegetation.size() << endl;
//	auto tmp2 = chrono::steady_clock::now();
//	cout << "food quad tree count: " << env.foodTree.count() << endl;
//	auto tmp3 = chrono::steady_clock::now();
//
//	auto elapsedtmp1 = chrono::duration_cast<chrono::microseconds>( tmp2 - tmp1 );
//	auto elapsedtmp2 = chrono::duration_cast<chrono::microseconds>( tmp3 - tmp2 );
//
//	cout << "time for vector size: " << elapsedtmp1.count() << endl;
//	cout << "time for quad count: " << elapsedtmp2.count() << endl << endl;

	cout << "Food growth: " << elapsed.count() << " us" << endl << endl;


	int cnt = 0;
	float r = 30;
	float r_sq = pow( r, 2 );
	float rx = RNGESUS->uniformRandomFloatFrom( 1+r, env.sizeX-1-r );
	float ry = RNGESUS->uniformRandomFloatFrom( 1+r, env.sizeY-1-r );

	util::coordinate rng0 ( rx - r, ry - r );
	util::coordinate rng1 ( rx + r, ry + r );
	util::coordinate limits ( env.sizeX, env.sizeY );

	auto tm2 = chrono::steady_clock::now();

//	int cnt = 0;
//	for ( int x = rx-r; x < rx+r; x++ ) {
//		for ( int y = ry-r; y < ry+r; y++ ) {
//			if ( env.foodReserve( x, y ) == 1 ) {
//				float dist = pow( (x - rx), 2 ) + pow( (y - ry), 2 );
//				if ( dist <= r_sq )
//					cnt++;
//			}
//		}
//	}
//
//	cout << "found " << cnt << " food nearby." << endl << endl;
//
//	auto tm3 = chrono::steady_clock::now();
//	elapsed = chrono::duration_cast<chrono::microseconds>( tm3 - tm2 );
//	cout << "restricted search: " << elapsed.count() << " us" << endl << endl;

	// get all the foods
	vector<Organism*> allFood = env.grassTree.rangeQuery( util::coordinate( 0, 0 ), limits, limits );

	auto tm3 = chrono::steady_clock::now();
	elapsed = chrono::duration_cast<chrono::microseconds>( tm3 - tm2 );
	cout << "amount of food queried: " << allFood.size() << endl;
	cout << "all food query: " << elapsed.count() << " us" << endl << endl;


	auto tm4 = chrono::steady_clock::now();

	vector<Organism*> results = env.grassTree.rangeQuery( rng0, rng1, limits );

	cnt = 0;
	for ( auto gr : results ) {
		float dist =  pow( (gr->posX - rx), 2 ) + pow( (gr->posY - ry), 2 );
		if ( dist <= r_sq )
			cnt++;
	}

	cout << "found " << cnt << " food nearby." << endl << endl;

	auto tm5 = chrono::steady_clock::now();
	elapsed = chrono::duration_cast<chrono::microseconds>( tm5 - tm4 );
	cout << "quad tree search: " << elapsed.count() << " us" << endl << endl;



	return 0;

}



int test_angles() {

	int n_tests = 50;

	vector<float> rxs = RNGESUS->nUniformRandomFloatsFrom( n_tests, 0, 2 );
	vector<float> rys = RNGESUS->nUniformRandomFloatsFrom( n_tests, 0, 2 );
//	vector<float> ccs = FATE->nUniformRandomFloatsFrom( n_tests / 2, 0, PI );
//	vector<float> cls = FATE->nUniformRandomFloatsFrom( n_tests / 2, -PI, 0 );
//	vector<float> ras;
//	ras.insert( ras.end(), ccs.begin(), ccs.end() );
//	ras.insert( ras.end(), cls.begin(), cls.end() );

	coordinate p( 0.5, 2.5 );
	float dir = PI / 4 + 0.1;
	float v_ang = PI / 2;
	float v_r = 1.5;

	float e0 = dir - v_ang / 2;
	float e1 = dir + v_ang / 2;
	vector<float> le0 = util::getLineParameters( p, e0 );
	vector<float> le1 = util::getLineParameters( p, e1 );

	int cnt = 1;
	for ( int i = 0; i < n_tests; ++i ) {
		if ( rys[i] <= le0[0]*rxs[i] + le0[1] &&
				rys[i] <= le1[0]*rxs[i] + le1[1] &&
				sqrt( pow( rxs[i] - p.x, 2 ) + pow( rys[i] - p.y, 2 ) ) <= v_r ) {
			cout << cnt << ": (" << rxs[i] << "," << rys[i] << ") seen" << endl;
			cnt++;
		}
	}

	// TO BE CONTINUED: how can we determine when we need above or below the line?

	return 0;

}


} /* namespace ecosystem */
