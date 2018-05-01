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
extern bool simulationProceed;
extern string fname_environment_ini;
extern string fname_animat_ini;
extern string fname_fcm_cs;
extern string fname_fcm;


namespace ecosystem {


void keyActions( GLFWwindow* window, int key, int scancode, int action, int mods ) {
	if ( key == GLFW_KEY_ENTER && action == GLFW_PRESS )
		simulationProceed = true;
}



int test_random_animats() {

	Habitat env( fname_environment_ini );

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
//	fcmWindow = gx::createWindow( 500, 500, "fcm visualisation" );
//	gx::setBackground( 1.0f, 1.0f, 1.0f, 1.0f );
//	// load stuff
//	GLuint vaoFCM = gx::createAndBindVAO();
//	GLuint dataBufFCM = gx::createVBO();
//	GLuint linesBufFCM = gx::createVBO();
//	GLuint shaderProg2 = gx::loadShaders( "shaders/shader.vert", "shaders/shader.frag" );

//	Animat* tracked = env.population[tracked_name];
//	gx::loadFCMIntoBuffer( tracked, vaoFCM, dataBufFCM, linesBufFCM );
//	gx::drawFCM( fcmWindow, tracked, shaderProg2, dataBufFCM, linesBufFCM );





	int time_counter = 0;
	int n_deaths = 0;

	// drawing loop
	while ( n_deaths < n_animats && !glfwWindowShouldClose( simWindow ) /*&& !glfwWindowShouldClose( fcmWindow )*/ ) {


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
//				tracked = env.population[tracked_name];
			}


			glfwMakeContextCurrent( simWindow );
			gx::loadHabitatIntoBuffer( &env, vaoEnv, dataBufEnv );
			gx::drawHabitat( simWindow, &env, shaderProg1 );

//			if ( env.population.size() > 0 ) {
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


	Habitat env( fname_environment_ini );



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


	Habitat env( fname_environment_ini );

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

	Habitat env( fname_environment_ini );

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




} /* namespace ecosystem */





