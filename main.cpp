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
#include <cmath>
#include <array>
#include <algorithm>
#include <Eigen/Dense>
#include <unistd.h>

#include "Animat.h"
#include "Habitat.h"
#include "Chance.h"
#include "util.h"
#include "graphx.h"

using namespace std;
using namespace Eigen;
using namespace ecosystem;



string fname_food0 = "foodReserve0.txt";
string fname_food1 = "foodReserve1.txt";


string fname_pop = "population.txt";
string fname_sens = "sensations.txt";

string fname_fcm_cs = "fcm_0.2.concepts.txt";
string fname_fcm = "fcm_0.2.txt";
string fname_test = "test.txt";

string fname_environment_ini = "environment.ini";

GLFWwindow* simWindow;
GLFWwindow* fcmWindow;
bool simulationProceed = false;


// random number generator
util::Chance* fate;


int test();
int test_foodGrowth_visual();
int test_with_visuals();
int fcm_test( Animat* );
int animat_test( Animat*, Habitat* );
int test_algebra();
int test_graphics();
void keyActions( GLFWwindow*, int, int, int, int );
int test_random();



int main( void ) {


	// init timer
	time_t t_start = time( NULL );


//	srand( time(0) );	deprecated

	// initiate randomness
	std::random_device rd;
	fate = new util::Chance( rd() );

	// empty files
//	util::cleanFile( fname_pop );
//	util::cleanFile( fname_sens );

//	string fcontent = util::readFileContent( fname_fcm );

//	return test_algebra();
	test_foodGrowth_visual();
//	test_with_visuals();
//	test_random();

	// TODO: nicely put stuff together - happening and rendering



	delete fate;	// TODO: organize destructors everywhere!!

	// stop timer and print exec tieme
	time_t t_end = time( NULL );
	cout << endl << endl << "Runtime: " << difftime( t_end, t_start ) << " seconds" << endl;

	return 0;

}



int test_graphics() {

	simWindow = gx::createWindow( 800, 600, "testtest" );

	gx::loadShaders( "shader.vert", "shader.frag" );
	gx::setBackground( 0.0f, 0.0f, 0.0f, 1.0f );
	gx::setupKeyboard( simWindow, keyActions );

//	gx::drawingLoop();

	gx::destroyWindow();

	return 0;
}



int test_foodGrowth_visual() {

//	int sx = 1000;
//	int sy = 1000;
//	int foodEnergy = 1;
//	double foodDensity = 0.0001;

//	Habitat env( sx, sy, foodEnergy, foodDensity );
//	Habitat env( sx, sy, foodEnergy, 15, 100, 0.05, fate );
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


	while ( !glfwWindowShouldClose( simWindow ) ) {

//		if ( simulationProceed )
		{

//			env.growFood_stable();
			env.growMeadows();

			gx::loadHabitatIntoBuffer( &env, vaoEnv, dataBufEnv );

//			simulationProceed = false;

		}

		gx::drawHabitat( simWindow, &env, shaderProg1 );

		glfwPollEvents();

		if ( glfwGetKey( simWindow, GLFW_KEY_ESCAPE ) == GLFW_PRESS )
			glfwSetWindowShouldClose( simWindow, GL_TRUE );

	}


	glDeleteBuffers( 1, &dataBufEnv );
	glDeleteProgram( shaderProg1 );
	glDeleteVertexArrays( 1, &vaoEnv );

	gx::destroyWindow();


	return 0;

}



int test() {

	int sx = 1000;
	int sy = 1000;
	int foodEnergy = 8;
	double density = 0.001;
	int n_animats = 20;

	Habitat env( sx, sy, foodEnergy, density, fate );

	// init animats
	for ( int i = 0; i < n_animats; ++i )
	{
		float randx = util::randFromUnitInterval() * sx;
		float randy = util::randFromUnitInterval() * sy;
		float randdir = util::randFromUnitInterval() * M_PI;
		if ( util::randFromUnitInterval() > 0.5 )
			randdir *= -1;

		Animat* ani = new Animat (
				randx,		// x
				randy, 		// yGrassland();
				0, 			// velocity
				10,			// max velocity
				randdir, 	// direction
				150.0, 		// energy
				40, 		// vision range
				2*M_PI, 	// vision angle  TODO: implement it's usage
				4.0,		// reach
				&env		// world pointer
		);

		int nConcepts = 10;
		ani->initFCM( nConcepts, fname_fcm_cs, fname_fcm );

		env.birth( ani );
		env.population[ani->name]->toString();
	}


	int time_counter = 0;
	int n_deaths;
	// while at least one animat is alive
	while ( n_deaths < n_animats )
	{

		// cannot remove dead animats in their execution loop, hence they are added to obituary and buried later
		vector<std::string> obituary;
		for ( auto &nm_ani : env.population )
		{
			Animat* ani = nm_ani.second;
			ani->reason();

			if ( ani->energy <= 0 )
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

	// TODO: sort out smooth operation and visual debugging with switches and stuff --!! maybe with config files??

	int sx = 1000;
	int sy = 1000;
	int foodEnergy = 5;
//	double density = 0.001;

//	Habitat env ( sx, sy, foodEnergy, density, fate );
	Habitat env( sx, sy, foodEnergy, 15, 150, 0.05, fate );

	int n_animats = 20;

//	const char* trName;

	for ( int i = 0; i < n_animats; ++i ) {

		float randx = util::randFromUnitInterval() * sx;
		float randy = util::randFromUnitInterval() * sy;
		float randdir = util::randFromUnitInterval() * M_PI;
		if ( util::randFromUnitInterval() > 0.5 )
			randdir *= -1;

		Animat* ani = new Animat (
				randx,		// x
				randy, 		// y
				0, 			// velocity
				10,			// max velocity
				randdir, 	// direction
				400, 		// energy
				40, 		// vision range
				2*M_PI, 	// vision angle  TODO: implement it's usage
				4.0,		// reach
				&env		// world pointer
		);

		int nConcepts = 10;
		ani->initFCM( nConcepts, fname_fcm_cs, fname_fcm );

		env.birth( ani );
		env.population[ani->name]->toString();
//		trName = ani->name;

	}





	// TODO: sort out smooth operation and visual debugging with switches and stuff

	// TODO: place the buffers stuff into gx module, here we only need initEnvironment() etc...
	// initialize graphics output for simulation
	// initialize glfw first
	if ( !glfwInit() ) {
		std::cout << "Failed to initialise GLFW" << std::endl;
		return -1;
	}
	simWindow = gx::createWindow( 1000, 1000, "simulation" );
	gx::setBackground( 0.0f, 0.0f, 0.0f, 1.0f );
//	gx::setupKeyboard( simWindow, keyActions );
	gx::setupKeyboard( simWindow );

	// load stuff
	GLuint vaoEnv = gx::createAndBindVAO();
	GLuint dataBufEnv = gx::createVBO();
	GLuint shaderProg1 = gx::loadShaders( "shaders/shader.vert", "shaders/shader.frag" );
	gx::loadHabitatIntoBuffer( &env, vaoEnv, dataBufEnv );
	gx::drawHabitat( simWindow, &env, shaderProg1 );


	// create new window for fcm
//	fcmWindow = gx::createWindow( 400, 400, "fcm visualisation" );
//	gx::setBackground( 1.0f, 1.0f, 1.0f, 1.0f );
//	// load stuff
//	GLuint vaoFCM = gx::createAndBindVAO();
//	GLuint dataBufFCM = gx::createVBO();
//	GLuint linesBufFCM = gx::createVBO();
//	GLuint shaderProg2 = gx::loadShaders( "shaders/shader.vert", "shaders/shader.frag" );
//
//	Animat* tracked = env.population[trName];
//	gx::loadFCMIntoBuffer( tracked, vaoFCM, dataBufFCM, linesBufFCM );
//	gx::drawFCM( fcmWindow, tracked, shaderProg2, dataBufFCM, linesBufFCM );


	glfwMakeContextCurrent( simWindow );


	// preinit the food a bit
	for ( int i = 0; i < 150; ++i ) {
		env.growMeadows();
	}


	int time_counter = 0;
	int n_deaths;

	// drawing loop
	while ( n_deaths < n_animats && !glfwWindowShouldClose( simWindow ) /*&& !glfwWindowShouldClose( fcmWindow )*/ ) {


//		if ( simulationProceed ) {

			env.growMeadows();

			vector<std::string> obituary;
			map<std::string, Animat*>::iterator it;
			for ( it = env.population.begin(); it != env.population.end(); ++it ){

				it->second->reason();

				// upload habitat data
	//			glfwMakeContextCurrent( simWindow );

				// upload fcm data
	//			glfwMakeContextCurrent( fcmWindow );
	//			gx::loadFCMIntoBuffer( &ani, vaoFCM, dataBufFCM, linesBufFCM );

				if ( it->second->energy <= 0 ) {
					obituary.push_back( it->second->name );
					++n_deaths;
					cout << "The animat " << it->second->name << " survived " << time_counter << " steps of the simulation" << endl;
				}


			}

			if ( obituary.size() > 0 ){
				vector<std::string>::iterator it;
				for ( it = obituary.begin(); it != obituary.end(); ++it )
					env.death( *it );
			}


			glfwMakeContextCurrent( simWindow );
			gx::loadHabitatIntoBuffer( &env, vaoEnv, dataBufEnv );

//			glfwMakeContextCurrent( fcmWindow );
//			gx::loadFCMIntoBuffer( tracked, vaoFCM, dataBufFCM, linesBufFCM );

//			simulationProceed = false;
			++time_counter;

//		}
		gx::drawHabitat( simWindow, &env, shaderProg1 );

//		glfwMakeContextCurrent( fcmWindow );
//		gx::drawFCM( fcmWindow, &ani, shaderProg2, dataBufFCM, linesBufFCM );

//		util::printAnimatLocationsToFile( env.population, fname_pop );
//		util::printSensationsToFile( ani.sensedObjs, fname_sens );

//		if ( ani.getEnergy() <= 0 )
//			break;

		glfwPollEvents();

		if ( glfwGetKey( simWindow, GLFW_KEY_ESCAPE ) == GLFW_PRESS )
			glfwSetWindowShouldClose( simWindow, GL_TRUE );

//		if ( glfwGetKey( fcmWindow, GLFW_KEY_ESCAPE ) == GLFW_PRESS )
//			glfwSetWindowShouldClose( fcmWindow, GL_TRUE );

		cout << "#";
		cout.flush();
		if ( time_counter % 100 == 0 )
			cout << " " << time_counter << endl;
//		usleep( 50000 );


	}


//	util::printMatrixToFile( env.getFoodReserve(), fname_food1 );



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


void keyActions( GLFWwindow* window, int key, int scancode, int action, int mods ) {
	if ( key == GLFW_KEY_ENTER && action == GLFW_PRESS )
		simulationProceed = true;
}



int fcm_test( Animat* ani ) {



	return 0;

}



int animat_test( Animat* ani, Habitat* env ) {

	ani->forgetSensedObjects();

	ani->sense();
	ani->calculateDecision();
	ani->move();


	return 0;

}



int test_random() {

	int n_stars = 100;
	int n_rolls = 10000;

//	std::mt19937 twister( rd() );
//
//	std::uniform_int_distribution<int> intdist( 0, 100 );
//	std::uniform_real_distribution<float> floatdist( 0.0, 1.0 );
//
//	std::array<float,3> intervals {0.0, 5.0, 10.0};
//	std::array<float,3> weights {5.0, 8.0, 2.0};
//	std::piecewise_linear_distribution<float> lindist( intervals.begin(), intervals.end(), weights.begin() );


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

//		int randint = intdist( twister );
		int randint = fate->uniformRandomIntFrom( 0, 100 );
		index = (int) floor( randint / 10 );
		++testInt[index];

//		float randf = floatdist( twister );
		float randf = fate->uniformRandomUnitFloat();
		index = (int) floor( randf * 10 );
		++testFloat[index];

//		float randlin = lindist( twister );
		float randlin = fate->linearDescRandomFloatFrom( 0.0, 10.0 );
		index = (int) floor( randlin );
		++testLinear[index];

		float randGauss = fate->normalFloat( 0.05, 0.01 );
		if ( randGauss > 0.0 && randGauss < 0.1 ) {
			index = (int) floor( randGauss * 100 );
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

	cout << "Normal distribution with m=0.05 and std=0.005 results: " << endl;
	for ( int i = 0; i < 10; ++i ) {
		cout << (float)i/100 << "-" << (float)(i+1)/100 << ":\t";
		cout << string( testNormal[i]*n_stars/n_rolls, '*' ) << endl;
	}

	cout << "Normal distribution with m=5.0 and std=2.2 results: " << endl;
	for ( int i = 0; i < 10; ++i ) {
		cout << i << "-" << (i+1) << ":\t";
		cout << string( testNormal2[i]*n_stars/n_rolls, '*' ) << endl;
	}

	return 0;

}



int test_algebra() {

	MatrixXd L( 5, 5 );

	L << 	0, 0, -0.5, 0.75, 0,
			0, 0, 0, 1, -0.5,
			0, 0, 0.5, 0.66, 0,
			0, 0, 0.33, 0, 1,
			0, 0, -1, 0.25, 0.25;

	cout << "Link matrix:" << endl;
	cout << L << endl;

	VectorXd vec(5);

	vec << 0.5, 0.7, 0, 0, 0;

	cout << "input vector:" << endl;
	cout << vec << endl;

	VectorXd state = VectorXd::Zero( 5 );

	cout << "state:" << endl;
	cout << state << endl;

//	MatrixXd Lt = L.transpose();
	VectorXd dS = vec.transpose() * L;

	cout << "activation propagation ~ input^t * L ~:" << endl;
	cout << dS << endl;

	for ( int i = 0; i < 3; ++i ){

		dS = vec.transpose() * L;
		state = util::tanh( state + dS );

		cout << "activation propagation" << endl;
		cout << dS << endl;
		cout << "delta state:" << endl;
		cout << dS << endl;
		cout << "new state:" << endl;
		cout << state << endl;

	}



	return 0;

}



