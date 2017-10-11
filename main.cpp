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
#include <ctime>
#include <cmath>
#include <Eigen/Dense>
#include <unistd.h>

#include "Animat.h"
#include "Habitat.h"
#include "util.h"
#include "graphx.h"

using namespace std;
using namespace Eigen;
using namespace ecosystem;



string fname_food0 = "foodReserve0.txt";
string fname_food1 = "foodReserve1.txt";


string fname_pop = "population.txt";
string fname_sens = "sensations.txt";

string fname_fcm_cs = "fcm_0.1.concepts.txt";
string fname_fcm = "fcm_0.1.txt";
string fname_test = "test.txt";


GLFWwindow* simWindow;
GLFWwindow* fcmWindow;
bool simulationProceed = false;



int test();
int fcm_test( Animat* );
int animat_test( Animat*, Habitat* );
int test_algebra();
int test_graphics();
void keyActions( GLFWwindow*, int, int, int, int );



int main( void ) {

	srand(time(0));

	// empty files
	util::cleanFile( fname_pop );
	util::cleanFile( fname_sens );

//	string fcontent = util::readFileContent( fname_fcm );


//	return test_algebra();
	return test();

	// TODO: nicely put stuff together - happening and rendering


	// graphical output debug
	return test_graphics();

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



int test() {

	// TODO: sort out smooth operation and visual debugging with switches and stuff --!! maybe with config files??

	int sx = 100;
	int sy = 100;
	int foodEnergy = 5;
	double density = 0.01;

	Habitat env ( sx, sy, foodEnergy, density );
	util::printMatrixToFile( env.getFoodReserve(), fname_food0 );


//	float allFood = foods.sum()/( sx * sy );
//	cout << allFood << endl;

	double randx = util::randFromUnitInterval() * sx;
	double randy = util::randFromUnitInterval() * sy;
	double randdir = util::randFromUnitInterval() * M_PI;

	Animat ani(
			randx,		// x
			randy, 		// y
			0, 			// velocity
			3,			// max velocity
			randdir, 	// direction
			50, 		// energy
			12, 		// vision range
			2*M_PI, 	// vision angle  TODO: implement it's usage
			&env		// world pointer
	);

	ani.toString();
	env.birth( &ani );

	util::printAnimatLocationsToFile( env.population, fname_pop );


	int nConcepts = 9;
	ani.initFCM( nConcepts, fname_fcm_cs, fname_fcm );





	// TODO: sort out smooth operation and visual debugging with switches and stuff


	// initialize graphics output for simulation
	// initialize glfw first
	if ( !glfwInit() ) {
		std::cout << "Failed to initialize GLFW" << std::endl;
		return -1;
	}
	simWindow = gx::createWindow( 800, 800, "simulation" );
//	fcmWindow = gx::createWindow( 400, 400, "fcm visualisation" );


	gx::setBackground( 0.0f, 0.0f, 0.0f, 1.0f );
	gx::setupKeyboard( simWindow, keyActions );
//	gx::setupKeyboard( fcmWindow, keyActions );

	// we already have food and animat - lets show them :)

	// load stuff
	GLuint vaoEnv = gx::createAndBindVAO();
	GLuint dataBufEnv = gx::createVBO();
	GLuint shaderProg1 = gx::loadShaders( "shader.vert", "shader.frag" );
	gx::loadHabitatIntoBuffer( &env, vaoEnv, dataBufEnv );
	gx::drawHabitat( simWindow, &env, shaderProg1 );


	// create new window for fcm
	fcmWindow = gx::createWindow( 400, 400, "fcm visualisation" );
	gx::setBackground( 1.0f, 1.0f, 1.0f, 1.0f );
	GLuint vaoFCM = gx::createAndBindVAO();
	GLuint dataBufFCM = gx::createVBO();
	GLuint colourBufFCM = gx::createVBO();
	GLuint shaderProg2 = gx::loadShaders( "shader.vert", "shader.frag" );
	gx::loadFCMIntoBuffer( &ani, vaoFCM, dataBufFCM, colourBufFCM );
	gx::drawFCM( fcmWindow, &ani, shaderProg2, dataBufFCM, colourBufFCM );


//	glfwMakeContextCurrent( simWindow );

	int time_counter = 0;

	// drawing loop
	while ( !glfwWindowShouldClose( simWindow ) && !glfwWindowShouldClose( fcmWindow ) ) {

//		ani.reason();


		if ( simulationProceed ){

			ani.reason();

			// upload habitat data
			glfwMakeContextCurrent( simWindow );
			gx::loadHabitatIntoBuffer( &env, vaoEnv, dataBufEnv );

			// upload fcm data
			glfwMakeContextCurrent( fcmWindow );
			gx::loadFCMIntoBuffer( &ani, vaoFCM, dataBufFCM, colourBufFCM );

			++time_counter;
			cout << "simulation step no" << time_counter << "!" << endl;
			simulationProceed = false;

		}


		glfwMakeContextCurrent( simWindow );
//		gx::bindVAO( vaoEnv );
		gx::drawHabitat( simWindow, &env, shaderProg1 );

		glfwMakeContextCurrent( fcmWindow );
//		gx::bindVAO( vaoFCM );
		gx::drawFCM( fcmWindow, &ani, shaderProg2, dataBufFCM, colourBufFCM );

//		glfwMakeContextCurrent( simWindow );
//		gx::switchContextToWindow( simWindow );
//		int inputret = gx::waitForInput();
//		cout << inputret << endl;

//		util::printAnimatLocationsToFile( env.population, fname_pop );
//		util::printSensationsToFile( ani.sensedObjs, fname_sens );

		if ( ani.getEnergy() <= 0 )
			break;

		glfwPollEvents();

		if ( glfwGetKey( simWindow, GLFW_KEY_ESCAPE ) == GLFW_PRESS )
			glfwSetWindowShouldClose( simWindow, GL_TRUE );

		if ( glfwGetKey( fcmWindow, GLFW_KEY_ESCAPE ) == GLFW_PRESS )
			glfwSetWindowShouldClose( fcmWindow, GL_TRUE );



//		++time_counter;
	}

	cout << "The animat survived " << time_counter << " steps of the simulation" << endl;
	util::printMatrixToFile( env.getFoodReserve(), fname_food1 );



	glDeleteBuffers( 1, &dataBufEnv );
	glDeleteBuffers( 1, &dataBufFCM );
	glDeleteBuffers( 1, &colourBufFCM );
	glDeleteProgram( shaderProg1 );
	glDeleteVertexArrays( 1, &vaoEnv );
	glDeleteVertexArrays( 1, &vaoFCM );

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



