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

			if ( obituary.size() > 0 ) {
				for ( auto obt : obituary ) {
					HABITAT->death( obt );
				}
			}

			if ( HABITAT->population.size() == 0 )
				break;

			// if tracked animat died, transfer tracking to random animat if any
			if ( HABITAT->population.find( tracked->name ) == HABITAT->population.end() ) {
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

//			std::this_thread::sleep_for( std::chrono::milliseconds( simulationPause ) );

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


	glfwMakeContextCurrent( fcmWindow );
	gx::destroyWindow();
	glfwMakeContextCurrent( simWindow );
	gx::destroyWindow();


	cout << endl << "Simulation concluded cleanly, extinction complete" << endl;


	return 0;


}



int test_fixed_evolution() {


	HABITAT->populateWorld( 500 );	//!!
	int n_animats = HABITAT->population.size();
	tracked = dynamic_cast<Animat*>( HABITAT->populationTree.getRandomOrganism() );

	for ( int i = 0; i < 300; ++i ) HABITAT->growMeadows();



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
	gx::loadHabitatIntoBuffer( vaoEnv, dataBufEnv, "" );
	gx::drawHabitat( simWindow, shaderProg1 );


	// create new window for fcm
//	fcmWindow = gx::createWindow( 500, 500, "fcm " + tracked->name );
//	gx::setBackground( 1.0f, 1.0f, 1.0f, 1.0f );
//	gx::setupKeyboard( fcmWindow, keyActions );
//	// load stuff
//	GLuint vaoFCM = gx::createAndBindVAO();
//	GLuint dataBufFCM = gx::createVBO();
//	GLuint linesBufFCM = gx::createVBO();
//	GLuint shaderProg2 = gx::loadShaders( "shaders/shader.vert", "shaders/shader.frag" );
//
//	gx::loadFCMIntoBuffer( tracked, vaoFCM, dataBufFCM, linesBufFCM );
//	gx::drawFCM( fcmWindow, tracked, shaderProg2, dataBufFCM, linesBufFCM );



	int time_counter = 0;
	int reproduction_cycle = 300;
	float mating_fraction = 0.25;
	uint n_successful = mating_fraction * n_animats;
	uint target_food = 11000;

	while( HABITAT->population.size() > 0 && !glfwWindowShouldClose( simWindow ) /*&& !glfwWindowShouldClose( fcmWindow )*/ ) {

		if ( simState != PAUSE )
		{

//			// just a fun experiment: divide time into 2 seasons - when meadows grow and when they don't
//			if ( (time_counter % reproduction_cycle) > (reproduction_cycle / 2) ) {
//				HABITAT->growMeadows();
//			}

			// a new approach: probability of a meadow growth is inversely proportional to the amount of food in the world
			int n_food = HABITAT->grassTree.count();
			float p_growth = fabs( RNGESUS->normalFloat( (1-(n_food/(float)target_food)), 0.01 ) );
			if ( RNGESUS->uniformRandomUnitFloat() < p_growth )
				HABITAT->growMeadows();

			for ( auto &nm_ani : HABITAT->population )
			{

				Animat* ani = nm_ani.second;
				ani->reason();

			}


//			if ( HABITAT->population.size() == 0 )
//				break;
//
//			// if tracked animat died, transfer tracking to random animat if any
//			if ( HABITAT->population.find( tracked->name ) == HABITAT->population.end() ) {
//				Organism* trorg = HABITAT->populationTree.getRandomOrganism();
//				while ( trorg == nullptr )
//					trorg = HABITAT->populationTree.getRandomOrganism();
//				tracked = dynamic_cast<Animat*>( trorg );
//				glfwSetWindowTitle( fcmWindow, ("fcm " + tracked->name).c_str() );
//			}


			glfwMakeContextCurrent( simWindow );
			gx::loadHabitatIntoBuffer( vaoEnv, dataBufEnv, "" );
			gx::drawHabitat( simWindow, shaderProg1 );

//			if ( HABITAT->population.size() > 0 ) {
//				glfwMakeContextCurrent( fcmWindow );
//				glfwSetWindowTitle( fcmWindow, ("fcm " + tracked->name).c_str() );
//				gx::loadFCMIntoBuffer( tracked, vaoFCM, dataBufFCM, linesBufFCM );
//				gx::drawFCM( fcmWindow, tracked, shaderProg2, dataBufFCM, linesBufFCM );
//			}

			++time_counter;
//			std::this_thread::sleep_for( std::chrono::milliseconds( simulationPause ) );

			cout << "#";
			cout.flush();
			if ( time_counter % 100 == 0 )
				cout << " " << time_counter << endl;


			// pause simulation if STEP
			if ( simState == STEP )
				simState = PAUSE;







			// every <reproduction_cycle> iterations, pair the most successful and kill the old generation
			if ( time_counter % reproduction_cycle == 0 ) {

				// print the successfulness of this generation and reset it
				cout << "Food consumed by current generation: " << HABITAT->foodConsumed << endl;
				HABITAT->foodConsumed = 0;

				cout << "Food left: " << HABITAT->grassTree.count() << endl;
				cout << "New generation hatching!" << endl;


				// first get the most succesful - order the animats by energy
				vector<Organism*> temps = HABITAT->populationTree.getAll();
				vector<Animat*> animates( temps.size() );
				for ( uint i = 0; i < temps.size(); i++ )
					animates[i] = dynamic_cast<Animat*>( temps[i] );
				std::sort( animates.begin(), animates.end(), util::compareAnimatEnergies );

				vector<Animat*> candidates;
				for ( uint i = 0; i < n_successful; ++i ) {
					if ( animates[i]->energy > 0 ) {
						candidates.push_back( animates[i] );
					}
				}

				cout << "Number of successful animats: " << candidates.size() << endl;


				// top <n_successful> get to mate
				uint n_offspring = 0;
				uint n_mates = candidates.size();
				while ( n_offspring < n_animats ) {
					// choose a partner randomly
					int partner1 = n_offspring % n_mates;
					int partner2 = RNGESUS->uniformRandomIntFrom( 0, n_mates );
					while ( partner2 == partner1 )
						// redo if RNGESUS wants hermaphrodism
						partner2 = RNGESUS->uniformRandomIntFrom( 0, n_mates );

					if ( HABITAT->breed( animates[partner1], animates[partner2], false ) )
						n_offspring++;

				}

//				// every successful animat gets to mate at least once ( twice if n_mates < half the population
//				uint n_offspring = 0;
//				uint n_mates = candidates.size();
//				uint n_repeats = ceil( 0.5 / (n_mates / (float)animates.size()) );
//
//				for ( uint b = 0; b < n_repeats; ++b ) {
//					for ( uint i = 0; i < n_mates; ++i ) {
//						uint partner_i = RNGESUS->uniformRandomIntFrom( 0, n_mates-1 );
//						while ( partner_i == i )
//							// redo if RNGESUS wants hermaphrodism
//							partner_i = RNGESUS->uniformRandomIntFrom( 0, n_mates-1 );
//
//						if ( HABITAT->breed( candidates[i], candidates[partner_i], false ) )
//							n_offspring++;
//					}
//				}
//				// populate the rest the same way as at the beginning of the simulation
//				int n_new = n_animats - n_offspring;
//				HABITAT->populateWorld( n_new );
//
//				cout << "Number of offspring: " << n_offspring << ", new: " << n_new << endl;



				// then kill all of them -> remove them from the tree
				for ( auto ani : animates )
					HABITAT->death( ani );

				cout << "Generation revolution complete!" << endl;

			}


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


//	glfwMakeContextCurrent( fcmWindow );
//	gx::destroyWindow();
	glfwMakeContextCurrent( simWindow );
	gx::destroyWindow();


	cout << endl << "Simulation concluded cleanly, extinction is complete" << endl;


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





