/*
 * main.cpp
 *
 *  Created on: Jul 4, 2017
 *      Author: south
 */


#include <iostream>
#include <fstream>
#include <cstdlib>
#include <map>
#include <random>
#include <ctime>
#include <chrono>
#include <cmath>
#include <array>
#include <algorithm>
#include <Eigen/Dense>
#include <unistd.h>

#include "Organism.h"
#include "Grass.h"
#include "Animat.h"
#include "Habitat.h"
#include "Chance.h"
#include "QuadTree.h"
#include "util.h"
#include "graphx.h"

using namespace std;
using namespace Eigen;
using namespace ecosystem;


const float PI = 3.14159;


string fname_environment_ini = "environment.ini";
string fname_animat_ini = "animat.ini";
string fname_fcm_cs = "fcm_0.2.concepts.txt";
string fname_fcm = "fcm_0.2.txt";


GLFWwindow* simWindow;
GLFWwindow* fcmWindow;
bool simulationProceed = false;


// random number generator
util::Chance* fate;


int test();
int test_with_visuals();
void keyActions( GLFWwindow*, int, int, int, int );

// debug
int test_foodGrowth_visual();
int test_randomness();
int test_quadTreeAnimats();
int test_quadTreeFood();



int main( void ) {


	// init timer
	auto t_start = chrono::steady_clock::now();

	// initiate randomness
	std::random_device rd;
	fate = new util::Chance( rd() );

	// TODO: nicely put stuff together - happening and rendering

//	test_quadTreeAnimats();
	test_quadTreeFood();

	delete fate;	// TODO: organize destructors everywhere!!

	// stop timer and print exec tieme
	auto t_end = chrono::steady_clock::now();
	auto elapsed = chrono::duration_cast<chrono::milliseconds>( t_end - t_start );
	cout << endl << endl << "Runtime: " << elapsed.count() << " milliseconds" << endl;

	return 0;

}



int test_foodGrowth_visual() {


	Habitat env( fname_environment_ini, fate );



	if ( !glfwInit() ) {
		std::cout << "Failed to initialise GLFW" << std::endl;
		return -1;
	}
	simWindow = gx::createWindow( 1000, 1000, "simulation" );
	gx::setBackground( 0.0f, 0.0f, 0.0f, 1.0f );
	gx::setupKeyboard( simWindow );

	// load stuff
	GLuint vaoEnv = gx::createAndBindVAO();
	GLuint dataBufEnv = gx::createVBO();
	GLuint shaderProg1 = gx::loadShaders( "shaders/shader.vert", "shaders/shader.frag" );
	gx::loadHabitatIntoBuffer( &env, vaoEnv, dataBufEnv );
	gx::drawHabitat( simWindow, &env, shaderProg1 );


	int time_counter = 0;
	while ( !glfwWindowShouldClose( simWindow ) ) {

//		if ( simulationProceed )
		{

			env.growMeadows();

			gx::loadHabitatIntoBuffer( &env, vaoEnv, dataBufEnv );

//			simulationProceed = false;
		}

		gx::drawHabitat( simWindow, &env, shaderProg1 );
		time_counter++;

		glfwPollEvents();

		if ( glfwGetKey( simWindow, GLFW_KEY_ESCAPE ) == GLFW_PRESS )
			glfwSetWindowShouldClose( simWindow, GL_TRUE );

	}


	glDeleteBuffers( 1, &dataBufEnv );
	glDeleteProgram( shaderProg1 );
	glDeleteVertexArrays( 1, &vaoEnv );

	gx::destroyWindow();

	cout << "Number of steps: " << time_counter << endl;

	return 0;

}



int test() {


	Habitat env( fname_environment_ini, fate );

	// init animats
	env.populateWorld( 0, fname_animat_ini, fname_fcm_cs, fname_fcm );

	// preinit the food
	for ( int i = 0; i < 500; ++i ) {
		env.growMeadows();
	}

	int time_counter = 0;
	int n_animats = env.population.size();
	int n_deaths = 0;
	// while at least one animat is alive
	while ( n_deaths < n_animats )
	{


		env.growMeadows();

		// cannot remove dead animats in their execution loop, hence they are added to obituary and buried later
		vector<std::string> obituary;

		for ( auto &nm_ani : env.population )
		{
			Animat* ani = nm_ani.second;
			ani->reason();
			ani->age++;

			if ( ani->energy <= 0 || ani->age >= ani->maxAge )
			{
				obituary.push_back( ani->name );
				++n_deaths;
				cout << "The animat " << ani->name << " survived " << time_counter << " steps of the simulation" << endl;
			}
		}

		// remove (bury) dead animats
		if ( obituary.size() > 0 )
			for ( auto &name : obituary )
				env.death( name );

		++time_counter;

		// print development
		cout << "#";
		cout.flush();
		if ( time_counter % 100 == 0 )
			cout << " " << time_counter << endl;

	}


	return 0;

}



int test_with_visuals() {

	Habitat env( fname_environment_ini, fate );

	// init animats
	env.populateWorld( 0, fname_animat_ini, fname_fcm_cs, fname_fcm );
	int n_animats = env.population.size();

	// preinit the food
	for ( int i = 0; i < 500; ++i ) {
		env.growMeadows();
	}

	string tracked_name = env.population.begin()->first;	// track 0-th animat for FCM display

	// TODO: place the buffers stuff into gx module, here we only need initEnvironment() etc...
	// initialize graphics output for simulation
	// initialize glfw first
	if ( !glfwInit() ) {
		std::cout << "Failed to initialise GLFW" << std::endl;
		return -1;
	}
	simWindow = gx::createWindow( 1000, 1000, "simulation" );
	gx::setBackground( 0.0f, 0.0f, 0.0f, 1.0f );
	gx::setupKeyboard( simWindow, keyActions );
//	gx::setupKeyboard( simWindow );

	// load stuff
	GLuint vaoEnv = gx::createAndBindVAO();
	GLuint dataBufEnv = gx::createVBO();
	GLuint shaderProg1 = gx::loadShaders( "shaders/shader.vert", "shaders/shader.frag" );
	gx::loadHabitatIntoBuffer( &env, vaoEnv, dataBufEnv );
	gx::drawHabitat( simWindow, &env, shaderProg1 );


	// create new window for fcm
	fcmWindow = gx::createWindow( 500, 500, "fcm visualisation" );
	gx::setBackground( 1.0f, 1.0f, 1.0f, 1.0f );
	// load stuff
	GLuint vaoFCM = gx::createAndBindVAO();
	GLuint dataBufFCM = gx::createVBO();
	GLuint linesBufFCM = gx::createVBO();
	GLuint shaderProg2 = gx::loadShaders( "shaders/shader.vert", "shaders/shader.frag" );

	Animat* tracked = env.population[tracked_name];
	gx::loadFCMIntoBuffer( tracked, vaoFCM, dataBufFCM, linesBufFCM );
	gx::drawFCM( fcmWindow, tracked, shaderProg2, dataBufFCM, linesBufFCM );



	int time_counter = 0;
	int n_deaths = 0;

	// drawing loop
	while ( n_deaths < n_animats && !glfwWindowShouldClose( simWindow ) && !glfwWindowShouldClose( fcmWindow ) ) {


//		if ( simulationProceed )
		{

			env.growMeadows();

			// cannot remove dead animats in their execution loop, hence they are added to obituary and buried later
			vector<std::string> obituary;

			for ( auto &nm_ani : env.population )
			{

				Animat* ani = nm_ani.second;
				ani->reason();
				ani->age++;

				if ( ani->energy <= 0 || ani->age >= ani->maxAge ) {
					obituary.push_back( ani->name );
					++n_deaths;
					cout << endl << "Animat " << ani->name << " died after " << time_counter << " steps of the simulation" << endl;
				}

			}

			if ( obituary.size() > 0 )
				for ( auto obt : obituary ) // for ( it = obituary.begin(); it != obituary.end(); ++it )
					env.death( obt );

			// if tracked animat died, transfer tracking to the next animat if any
			if ( env.population.find( tracked_name ) == env.population.end() && env.population.size() > 0 ) {
				tracked_name = env.population.begin()->first;
				tracked = env.population[tracked_name];
			}


			glfwMakeContextCurrent( simWindow );
			gx::loadHabitatIntoBuffer( &env, vaoEnv, dataBufEnv );
			gx::drawHabitat( simWindow, &env, shaderProg1 );

			if ( env.population.size() > 0 ) {
				glfwMakeContextCurrent( fcmWindow );
				gx::loadFCMIntoBuffer( tracked, vaoFCM, dataBufFCM, linesBufFCM );
				gx::drawFCM( fcmWindow, tracked, shaderProg2, dataBufFCM, linesBufFCM );
			}

			simulationProceed = false;
			++time_counter;

			cout << "#";
			cout.flush();
			if ( time_counter % 100 == 0 )
				cout << " " << time_counter << endl;

		}


		glfwPollEvents();

		if ( glfwGetKey( simWindow, GLFW_KEY_ESCAPE ) == GLFW_PRESS )
			glfwSetWindowShouldClose( simWindow, GL_TRUE );

		if ( glfwGetKey( fcmWindow, GLFW_KEY_ESCAPE ) == GLFW_PRESS )
			glfwSetWindowShouldClose( fcmWindow, GL_TRUE );




	}


	// cleanup buffers	TODO: move to gx
	glDeleteBuffers( 1, &dataBufEnv );
	glDeleteBuffers( 1, &dataBufFCM );
	glDeleteBuffers( 1, &linesBufFCM );
	glDeleteProgram( shaderProg1 );
	glDeleteProgram( shaderProg2 );
	glDeleteVertexArrays( 1, &vaoEnv );
	glDeleteVertexArrays( 1, &vaoFCM );

	gx::destroyWindow();


	return 0;

}


void keyActions( GLFWwindow* window, int key, int scancode, int action, int mods ) {
	if ( key == GLFW_KEY_ENTER && action == GLFW_PRESS )
		simulationProceed = true;
}














///////////////////////////////////////////
////// 		DEBUG PURPOSES ONLY		///////



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

		bool randbool = fate->randomBoolean();
		++testBool[randbool];

		int randint = fate->uniformRandomIntFrom( 0, 100 );
		index = (int) floor( randint / 10 );
		++testInt[index];

		float randf = fate->uniformRandomUnitFloat();
		index = (int) floor( randf * 10 );
		++testFloat[index];

		float randlin = fate->linearDescRandomFloatFrom( 0.0, 10.0 );
		index = (int) floor( randlin );
		++testLinear[index];

		float randGauss = fate->normalFloat( 10, 0.1 );
		if ( randGauss > 0.0 && randGauss < 20.0 ) {
			index = (int) floor( randGauss / 2 );
			++testNormal[index];
		}
	}

	vector<float> rands = fate->normalFloatsString( n_rolls, 5.0, 3.0 );
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

	Habitat env ( fname_environment_ini, fate );

	// init animats
	env.populateWorld( n_animats, fname_animat_ini, fname_fcm_cs, fname_fcm );


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

	Habitat env( fname_environment_ini, fate );

	// preinit the food
	for ( int i = 0; i < 5000; ++i ) {
		env.growMeadows();
	}


	auto tm1 = chrono::steady_clock::now();
	auto elapsed = chrono::duration_cast<chrono::microseconds>( tm1 - start );

	auto tmp1 = chrono::steady_clock::now();
	cout << "amount of food reserve: " << env.foodReserve.sum() << endl;
	auto tmp2 = chrono::steady_clock::now();
	cout << "food quad tree count: " << env.foodTree.count() << endl;
	auto tmp3 = chrono::steady_clock::now();

	auto elapsedtmp1 = chrono::duration_cast<chrono::microseconds>( tmp2 - tmp1 );
	auto elapsedtmp2 = chrono::duration_cast<chrono::microseconds>( tmp3 - tmp2 );

	cout << "time for reserve sum: " << elapsedtmp1.count() << endl;
	cout << "time for quad count: " << elapsedtmp2.count() << endl << endl;

	cout << "Food growth: " << elapsed.count() << " us" << endl << endl;


	float r = 30;
	float r_sq = pow( r, 2 );
	float rx = fate->uniformRandomFloatFrom( 1+r, env.sizeX-1-r );
	float ry = fate->uniformRandomFloatFrom( 1+r, env.sizeY-1-r );


	auto tm2 = chrono::steady_clock::now();

	int cnt = 0;
	for ( int x = rx-r; x < rx+r; x++ ) {
		for ( int y = ry-r; y < ry+r; y++ ) {
			if ( env.foodReserve( x, y ) == 1 ) {
				float dist = pow( (x - rx), 2 ) + pow( (y - ry), 2 );
				if ( dist <= r_sq )
					cnt++;
			}
		}
	}

	cout << "found " << cnt << " food nearby." << endl << endl;

	auto tm3 = chrono::steady_clock::now();
	elapsed = chrono::duration_cast<chrono::microseconds>( tm3 - tm2 );
	cout << "restricted search: " << elapsed.count() << " us" << endl << endl;


	util::coordinate rng0 ( rx - r, ry - r );
	util::coordinate rng1 ( rx + r, ry + r );
	util::coordinate limits ( env.sizeX, env.sizeY );

	auto tm4 = chrono::steady_clock::now();

	vector<Organism*> results = env.foodTree.rangeQuery( rng0, rng1, limits );

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






