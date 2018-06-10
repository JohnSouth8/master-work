/*
 * habitat_tests.cpp
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

#include "habitat_tests.h"
#include "util.h"
#include "Chance.h"
#include "QuadTree.h"
#include "graphx.h"



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

// window controls
bool simulationProceed = false;	// deprecated
enum simulationMode {
		PAUSE,
		STEP,
		RUN
};
simulationMode simState = PAUSE;
int simulationPause = 100;
int simulationPauseStep = 20;
Animat* tracked;


namespace ecosystem {


void keyActions( GLFWwindow* window, int key, int scancode, int action, int mods ) {
	if ( key == GLFW_KEY_ENTER && action == GLFW_PRESS ) {
		if ( simulationProceed )
			simulationProceed = false;
		else
			simulationProceed = true;

		simState = STEP;
	}
	if ( key == GLFW_KEY_SPACE && action == GLFW_PRESS ) {
		if ( simState == RUN )
			simState = PAUSE;
		else
			simState = RUN;
	}
	if ( key == GLFW_KEY_RIGHT ) {
		Organism* trorg = HABITAT->populationTree.getRandomOrganism();
		while ( trorg == nullptr )
			trorg = HABITAT->populationTree.getRandomOrganism();
		tracked = dynamic_cast<Animat*>( trorg );
	}
	if ( key == GLFW_KEY_UP ) {
		simulationPause -= simulationPauseStep;
	}
	if ( key == GLFW_KEY_DOWN ) {
		simulationPause += simulationPauseStep;
	}
}



int test() {


	HABITAT->populateWorld( 0 );	//!!
//	int n_animats = HABITAT->population.size();
	tracked = dynamic_cast<Animat*>( HABITAT->populationTree.getRandomOrganism() );

	for ( int i = 0; i < 500; ++i ) HABITAT->growMeadows();



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
	gx::loadHabitatIntoBuffer( vaoEnv, dataBufEnv, tracked->name );
	gx::drawHabitat( simWindow, shaderProg1 );


	// create new window for fcm
	fcmWindow = gx::createWindow( 500, 500, "fcm " + tracked->name );
	gx::setBackground( 1.0f, 1.0f, 1.0f, 1.0f );
	gx::setupKeyboard( fcmWindow, keyActions );
	// load stuff
	GLuint vaoFCM = gx::createAndBindVAO();
	GLuint dataBufFCM = gx::createVBO();
	GLuint linesBufFCM = gx::createVBO();
	GLuint shaderProg2 = gx::loadShaders( "shaders/shader.vert", "shaders/shader.frag" );

	gx::loadFCMIntoBuffer( tracked, vaoFCM, dataBufFCM, linesBufFCM );
	gx::drawFCM( fcmWindow, tracked, shaderProg2, dataBufFCM, linesBufFCM );



	int time_counter = 0;
	int n_deaths = 0;

	while( HABITAT->population.size() > 0 && !glfwWindowShouldClose( simWindow ) && !glfwWindowShouldClose( fcmWindow ) ) {

		if ( simState != PAUSE )
		{

			HABITAT->growMeadows();

			// cannot remove dead animats in their execution loop, hence they are added to obituary and buried later
			vector<Animat*> obituary;

			for ( auto &nm_ani : HABITAT->population )
			{

				Animat* ani = nm_ani.second;
				ani->reason();
				ani->age++;

				if ( ani->energy <= 0 || ani->age >= ani->maxAge ) {
					obituary.push_back( ani );
					++n_deaths;
					cout << endl << "Animat " << ani->name << " died after " << time_counter << " steps of the simulation" << endl;
				}

			}

			if ( obituary.size() > 0 )
				for ( auto obt : obituary ) {
					HABITAT->death( obt );
				}

			// if tracked animat died, transfer tracking to random animat if any
			if ( HABITAT->population.find( tracked->name ) == HABITAT->population.end() && HABITAT->population.size() > 0 ) {
				Organism* trorg = HABITAT->populationTree.getRandomOrganism();
				while ( trorg == nullptr )
					trorg = HABITAT->populationTree.getRandomOrganism();
				tracked = dynamic_cast<Animat*>( trorg );
				glfwSetWindowTitle( fcmWindow, ("fcm " + tracked->name).c_str() );
			}


			glfwMakeContextCurrent( simWindow );
			gx::loadHabitatIntoBuffer( vaoEnv, dataBufEnv, tracked->name );
			gx::drawHabitat( simWindow, shaderProg1 );

			if ( HABITAT->population.size() > 0 ) {
				glfwMakeContextCurrent( fcmWindow );
				glfwSetWindowTitle( fcmWindow, ("fcm " + tracked->name).c_str() );
				gx::loadFCMIntoBuffer( tracked, vaoFCM, dataBufFCM, linesBufFCM );
				gx::drawFCM( fcmWindow, tracked, shaderProg2, dataBufFCM, linesBufFCM );
			}

			++time_counter;

			std::this_thread::sleep_for( std::chrono::milliseconds( simulationPause ) );

			cout << "#";
			cout.flush();
			if ( time_counter % 100 == 0 )
				cout << " " << time_counter << endl;

			// pause simulation if STEP
			if ( simState == STEP )
				simState = PAUSE;


		}


		glfwPollEvents();

		if ( glfwGetKey( simWindow, GLFW_KEY_ESCAPE ) == GLFW_PRESS )
			glfwSetWindowShouldClose( simWindow, GL_TRUE );

//		if ( glfwGetKey( fcmWindow, GLFW_KEY_ESCAPE ) == GLFW_PRESS )
//			glfwSetWindowShouldClose( fcmWindow, GL_TRUE );




	}


	// cleanup buffers	TODO: move to gx
	glDeleteBuffers( 1, &dataBufEnv );
//	glDeleteBuffers( 1, &dataBufFCM );
//	glDeleteBuffers( 1, &linesBufFCM );
	glDeleteProgram( shaderProg1 );
//	glDeleteProgram( shaderProg2 );
	glDeleteVertexArrays( 1, &vaoEnv );
//	glDeleteVertexArrays( 1, &vaoFCM );

	gx::destroyWindow();







	return 0;


}



int test_random_animats() {

	// init animats
	HABITAT->populateWorld( 0 );
	int n_animats = HABITAT->population.size();

	// preinit the food
	for ( int i = 0; i < 500; ++i ) {
		HABITAT->growMeadows();
	}

	string tracked_name = HABITAT->population.begin()->first;	// track 0-th animat for FCM display

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
	gx::loadHabitatIntoBuffer( vaoEnv, dataBufEnv, tracked->name );
	gx::drawHabitat( simWindow, shaderProg1 );


	// create new window for fcm
//	fcmWindow = gx::createWindow( 500, 500, "fcm visualisation" );
//	gx::setBackground( 1.0f, 1.0f, 1.0f, 1.0f );
//	// load stuff
//	GLuint vaoFCM = gx::createAndBindVAO();
//	GLuint dataBufFCM = gx::createVBO();
//	GLuint linesBufFCM = gx::createVBO();
//	GLuint shaderProg2 = gx::loadShaders( "shaders/shader.vert", "shaders/shader.frag" );

//	Animat* tracked = HABITAT->population[tracked_name];
//	gx::loadFCMIntoBuffer( tracked, vaoFCM, dataBufFCM, linesBufFCM );
//	gx::drawFCM( fcmWindow, tracked, shaderProg2, dataBufFCM, linesBufFCM );





	int time_counter = 0;
	int n_deaths = 0;

	// drawing loop
	while ( n_deaths < n_animats && !glfwWindowShouldClose( simWindow ) /*&& !glfwWindowShouldClose( fcmWindow )*/ ) {


//		if ( simulationProceed )
		{

			HABITAT->growMeadows();

			// cannot remove dead animats in their execution loop, hence they are added to obituary and buried later
			vector<std::string> obituary;

			for ( auto &nm_ani : HABITAT->population )
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
					HABITAT->deathOld( obt );

			// if tracked animat died, transfer tracking to the next animat if any
			if ( HABITAT->population.find( tracked_name ) == HABITAT->population.end() && HABITAT->population.size() > 0 ) {
				tracked_name = HABITAT->population.begin()->first;
//				tracked = HABITAT->population[tracked_name];
			}


			glfwMakeContextCurrent( simWindow );
			gx::loadHabitatIntoBuffer( vaoEnv, dataBufEnv, tracked->name );
			gx::drawHabitat( simWindow, shaderProg1 );

//			if ( HABITAT->population.size() > 0 ) {
//				glfwMakeContextCurrent( fcmWindow );
//				gx::loadFCMIntoBuffer( tracked, vaoFCM, dataBufFCM, linesBufFCM );
//				gx::drawFCM( fcmWindow, tracked, shaderProg2, dataBufFCM, linesBufFCM );
//			}

			simulationProceed = false;
			++time_counter;

			std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );

			cout << "#";
			cout.flush();
			if ( time_counter % 100 == 0 )
				cout << " " << time_counter << endl;

		}


		glfwPollEvents();

		if ( glfwGetKey( simWindow, GLFW_KEY_ESCAPE ) == GLFW_PRESS )
			glfwSetWindowShouldClose( simWindow, GL_TRUE );

//		if ( glfwGetKey( fcmWindow, GLFW_KEY_ESCAPE ) == GLFW_PRESS )
//			glfwSetWindowShouldClose( fcmWindow, GL_TRUE );




	}


	// cleanup buffers	TODO: move to gx
	glDeleteBuffers( 1, &dataBufEnv );
//	glDeleteBuffers( 1, &dataBufFCM );
//	glDeleteBuffers( 1, &linesBufFCM );
	glDeleteProgram( shaderProg1 );
//	glDeleteProgram( shaderProg2 );
	glDeleteVertexArrays( 1, &vaoEnv );
//	glDeleteVertexArrays( 1, &vaoFCM );

	gx::destroyWindow();







	return 0;

}



int test_foodGrowth_visual() {


	Habitat env( ENVIRONMENT_INI );



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
	gx::loadHabitatIntoBuffer( vaoEnv, dataBufEnv, tracked->name );
	gx::drawHabitat( simWindow, shaderProg1 );


	int time_counter = 0;
	while ( !glfwWindowShouldClose( simWindow ) ) {

//		if ( simulationProceed )
		{

			HABITAT->growMeadows();

			gx::loadHabitatIntoBuffer( vaoEnv, dataBufEnv, tracked->name );

//			simulationProceed = false;
		}

		gx::drawHabitat( simWindow, shaderProg1 );
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



int test_old() {


	Habitat env( ENVIRONMENT_INI );

	// init animats
	HABITAT->populateWorld( 0 );

	// preinit the food
	for ( int i = 0; i < 500; ++i ) {
		HABITAT->growMeadows();
	}

	int time_counter = 0;
	int n_animats = HABITAT->population.size();
	int n_deaths = 0;
	// while at least one animat is alive
	while ( n_deaths < n_animats )
	{


		HABITAT->growMeadows();

		// cannot remove dead animats in their execution loop, hence they are added to obituary and buried later
		vector<std::string> obituary;

		for ( auto &nm_ani : HABITAT->population )
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
				HABITAT->deathOld( name );

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

	Habitat env( ENVIRONMENT_INI );

	// init animats
	HABITAT->populateWorld( 0 );
	int n_animats = HABITAT->population.size();

	// preinit the food
	for ( int i = 0; i < 500; ++i ) {
		HABITAT->growMeadows();
	}

	string tracked_name = HABITAT->population.begin()->first;	// track 0-th animat for FCM display

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
	gx::loadHabitatIntoBuffer( vaoEnv, dataBufEnv, tracked->name );
	gx::drawHabitat( simWindow, shaderProg1 );


	// create new window for fcm
	fcmWindow = gx::createWindow( 500, 500, "fcm visualisation" );
	gx::setBackground( 1.0f, 1.0f, 1.0f, 1.0f );
	// load stuff
	GLuint vaoFCM = gx::createAndBindVAO();
	GLuint dataBufFCM = gx::createVBO();
	GLuint linesBufFCM = gx::createVBO();
	GLuint shaderProg2 = gx::loadShaders( "shaders/shader.vert", "shaders/shader.frag" );

	Animat* tracked = HABITAT->population[tracked_name];
	gx::loadFCMIntoBuffer( tracked, vaoFCM, dataBufFCM, linesBufFCM );
	gx::drawFCM( fcmWindow, tracked, shaderProg2, dataBufFCM, linesBufFCM );



	int time_counter = 0;
	int n_deaths = 0;

	// drawing loop
	while ( n_deaths < n_animats && !glfwWindowShouldClose( simWindow ) && !glfwWindowShouldClose( fcmWindow ) ) {


//		if ( simulationProceed )
		{

			HABITAT->growMeadows();

			// cannot remove dead animats in their execution loop, hence they are added to obituary and buried later
			vector<std::string> obituary;

			for ( auto &nm_ani : HABITAT->population )
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
					HABITAT->deathOld( obt );

			// if tracked animat died, transfer tracking to the next animat if any
			if ( HABITAT->population.find( tracked_name ) == HABITAT->population.end() && HABITAT->population.size() > 0 ) {
				tracked_name = HABITAT->population.begin()->first;
				tracked = HABITAT->population[tracked_name];
			}


			glfwMakeContextCurrent( simWindow );
			gx::loadHabitatIntoBuffer( vaoEnv, dataBufEnv, tracked->name );
			gx::drawHabitat( simWindow, shaderProg1 );

			if ( HABITAT->population.size() > 0 ) {
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




} /* namespace ecosystem */





