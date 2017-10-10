/*
 * graphx.cpp
 *
 *  Created on: Sep 14, 2017
 *      Author: south
 */


#include "graphx.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "util.h"



using namespace ecosystem;


namespace gx {

//	GLFWwindow* gwindow;
//	bool doProceed = false;
//	bool doClose = false;


//	void drawingLoop() {
//
//		GLuint shaderProg = gx::loadShaders( "shader.vert", "shader.frag" );
//
//		while ( !glfwWindowShouldClose( gwindow ) ) {
//
//			glClear( GL_COLOR_BUFFER_BIT );
//
//			glUseProgram( shaderProg );
//
//			// swap buffers
//			glfwSwapBuffers( gwindow );
//			glfwPollEvents();
//
//			if ( glfwGetKey( gwindow, GLFW_KEY_ESCAPE ) == GLFW_PRESS )
//			    glfwSetWindowShouldClose( gwindow, GL_TRUE );
//
//		}
//
//	}


	// draws one step of simulation
	void drawHabitat( GLFWwindow* context, ecosystem::Habitat* env, GLuint shaderProg ) {

		// in case it is not so
		glfwMakeContextCurrent( context );

		// draw
		glClear( GL_COLOR_BUFFER_BIT );
		glUseProgram( shaderProg );

		// position attributes
		GLint posAttr = glGetAttribLocation( shaderProg, "position" );
		glEnableVertexAttribArray( posAttr );
		glVertexAttribPointer( posAttr, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), 0 );

		// color attributes
		GLint colAttr = glGetAttribLocation( shaderProg, "inColor" );
		glEnableVertexAttribArray( colAttr );
		glVertexAttribPointer( colAttr, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(2*sizeof(float)) );

		int n_points = env->foodReserve.sum() + env->population.size();

		glDrawArrays( GL_POINTS, 0, n_points );

		// swap buffers
		glfwSwapBuffers( context );

		// here onward does not belong here
//		while ( !glfwWindowShouldClose( gwindow ) ) {
//			glfwPollEvents();
//			if ( glfwGetKey( gwindow, GLFW_KEY_ESCAPE ) == GLFW_PRESS )
//				glfwSetWindowShouldClose( gwindow, GL_TRUE );
//		}


	}



	void drawFCM( GLFWwindow* context, ecosystem::Animat* ani, GLuint shaderProg, GLuint posBuffer, GLuint colBuffer ) {

		// in case it is not so
		glfwMakeContextCurrent( context );

		// draw
		glClear( GL_COLOR_BUFFER_BIT );
		glUseProgram( shaderProg );

		// position attributes
		GLint posAttr = glGetAttribLocation( shaderProg, "position" );
		glBindBuffer( GL_ARRAY_BUFFER, posBuffer );
		glEnableVertexAttribArray( posAttr );
		glVertexAttribPointer( posAttr, 2, GL_FLOAT, GL_FALSE, 0, 0 );

		// color attributes
		GLint colAttr = glGetAttribLocation( shaderProg, "inColor" );
		glBindBuffer( GL_ARRAY_BUFFER, colBuffer );
		glEnableVertexAttribArray( colAttr );
		glVertexAttribPointer( colAttr, 3, GL_FLOAT, GL_FALSE, 0, 0 );

		int n_points = ani->cognition.nConcepts;

		glDrawArrays( GL_POINTS, 0, n_points );

		// swap buffers
		glfwSwapBuffers( context );

	}



	GLuint createVBO() {
		GLuint vbo;
		glGenBuffers( 1, &vbo );
		return vbo;
	}



	GLuint createAndBindVAO() {
		GLuint vao;
		glGenVertexArrays( 1, &vao );
		glBindVertexArray( vao );
		return vao;
	}



	void bindVAO( GLuint vao ) {
		glBindVertexArray( vao );
	}



	void loadHabitatIntoBuffer( Habitat* environment, GLuint vao, GLuint vbo ) {

		glBindVertexArray( vao );

		int n_data_food = environment->foodReserve.sum(),		// foodReserve.sum() works for now, when food is 0 or 1
			n_data_pop = environment->population.size(),
			n_data = n_data_food + n_data_pop,
			env_w = environment->sizeX,
			env_h = environment->sizeY,
			counter = 0;

		static GLfloat points_buffer_data[50000]; //= new GLfloat[n_data*5];		// << each point has 5 datums: X Y R G B

		// add green food points
		for ( int i = 0; i < env_w; ++i ) {
			for ( int j = 0; j < env_h; ++j ) {
				if ( environment->foodReserve(i,j) != 0 ) {

					points_buffer_data[counter*5 + 0] = 2.0f*i / float(env_w) - 1; 	// X
					points_buffer_data[counter*5 + 1] = 2.0f*j / float(env_h) - 1; 	// Y
					points_buffer_data[counter*5 + 2] = 0.0f;						// R
					points_buffer_data[counter*5 + 3] = 0.9f;						// G
					points_buffer_data[counter*5 + 4] = 0.0f;						// B

					++counter;

				}
				if ( counter == n_data_food )
					break;
			}
			if ( counter == n_data_food )
				break;
		}

		// add red animat points
		std::map<const char*, Animat*>::iterator it;
		for (  it = environment->population.begin(); it != environment->population.end(); ++it ) {

			points_buffer_data[counter*5 + 0] = float( 2.0f * it->second->posX / float(env_w) - 1 );	// X
			points_buffer_data[counter*5 + 1] = float( 2.0f * it->second->posY / float(env_h) - 1 );	// Y
			points_buffer_data[counter*5 + 2] = 0.95f;													// R
			points_buffer_data[counter*5 + 3] = 0.0f;													// G
			points_buffer_data[counter*5 + 4] = 0.0f;													// B

			++counter;

		}

		// load the points into buffer
		glBindBuffer( GL_ARRAY_BUFFER, vbo );
		glBufferData( GL_ARRAY_BUFFER, n_data*5*sizeof(float), points_buffer_data, GL_STATIC_DRAW );

		// enable gl_PointSize in the shader
		glEnable( GL_PROGRAM_POINT_SIZE );

	}



	void loadFCMIntoBuffer( ecosystem::Animat* ani, GLuint vao, GLuint vbo_pos, GLuint vbo_col ) {

		glBindVertexArray( vao );

		int n_in = ani->cognition.nInput,
			n_int = ani->cognition.nInternal,
			n_out = ani->cognition.nOutput,
//			n_links = util::countNonZeroElements( ani->cognition.L ),
			n_data = (n_in + n_int + n_out) * 2,
			n_colours = (n_in + n_int + n_out) * 3,
			counter = 0;

		Eigen::VectorXd state = ani->cognition.state;
		std::vector<float> colour0 (3, 0.0f);
		std::vector<float> colour1 (3, 0.0f);
		std::vector<float> gradColour;
		colour0[2] = 1.0f;
		colour1[1] = 0.0f;

		static GLfloat points_buffer_data[1000];
		static GLfloat colours_buffer_data[1000];

		// create a triangle
//		GLfloat triangle_vertices[] = {
//				-0.1f, -0.1f,
//				0.1f, -0.1f,
//				0.0f, 0.14f
//		};

		// input concepts
		float x_cur = -0.5f;
		float y_step = 2.0f/float(n_in + 1);
		float y_cur;

		for ( int i = 0; i < n_in; ++i ) {

			y_cur = -1 + (i+1)*y_step;
			gradColour = util::getColourFromGradient( colour0, colour1, ani->cognition.state(counter) );

			// vertex 1 position
			points_buffer_data[counter*2 + 0] = x_cur;
			points_buffer_data[counter*2 + 1] = y_cur;

			// vertex 1 color
			colours_buffer_data[counter*3 + 0] = gradColour[0];
			colours_buffer_data[counter*3 + 1] = gradColour[1];
			colours_buffer_data[counter*3 + 2] = gradColour[2];

			++counter;

		}

		// internal concepts
		x_cur = 0.0f;
		y_step = 2.0f/float(n_int + 1);
		for ( int i = 0; i < n_int; ++i ) {

			y_cur = -1 + (i+1)*y_step;
			gradColour = util::getColourFromGradient( colour0, colour1, state(counter) );

			// vertex 1
			points_buffer_data[counter*2 + 0] = x_cur;
			points_buffer_data[counter*2 + 1] = y_cur;


			// vertex 1 color
			colours_buffer_data[counter*3 + 0] = gradColour[0];
			colours_buffer_data[counter*3 + 1] = gradColour[1];
			colours_buffer_data[counter*3 + 2] = gradColour[2];

			++counter;

		}

		// output concepts
		x_cur = 0.5f;
		y_step = 2.0f/float(n_out + 1);
		for ( int i = 0; i < n_out; ++i ) {

			y_cur = -1 + (i+1)*y_step;
			gradColour = util::getColourFromGradient( colour0, colour1, state(counter) );

			// vertex 1
			points_buffer_data[counter*2 + 0] = x_cur;
			points_buffer_data[counter*2 + 1] = y_cur;


			// vertex 1 color
			colours_buffer_data[counter*3 + 0] = gradColour[0];
			colours_buffer_data[counter*3 + 1] = gradColour[1];
			colours_buffer_data[counter*3 + 2] = gradColour[2];

			++counter;

		}

		// load data points into position buffer
		glBindBuffer( GL_ARRAY_BUFFER, vbo_pos );
		glBufferData( GL_ARRAY_BUFFER, n_data*sizeof(float), points_buffer_data, GL_STATIC_DRAW );

		// load colours data into colour buffer
		glBindBuffer( GL_ARRAY_BUFFER, vbo_col );
		glBufferData( GL_ARRAY_BUFFER, n_colours*sizeof(float), colours_buffer_data, GL_STATIC_DRAW );

	}




	GLFWwindow* createWindow( int widthPx, int heightPx, std::string title ) {
		return createWindow( widthPx, heightPx, title.c_str() );
	}

	GLFWwindow* createWindow( int widthPx, int heightPx, const char* title ) {

		// set some properties
//		glfwWindowHint( GLFW_SAMPLES, 4 );									// anti-aliasing
		glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );					// ogl version n1
		glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );					// ogl version n2
		glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );	// only the new style opengl
		glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
		glfwWindowHint( GLFW_RESIZABLE, GL_FALSE );							// self-explanatory

		// create a window context			the two NULLs are for fullscreen and resource sharing with existing context, respectively
		GLFWwindow* gwindow = glfwCreateWindow( widthPx, heightPx, title, NULL, NULL );
		if ( gwindow == NULL ){
			std::cout << "Failed to open a window" << std::endl;
			glfwTerminate();
			return NULL;
		}

		glfwMakeContextCurrent( gwindow );

		// initialize GLEW
		glewExperimental = true;				// something to do with experimental extensions, better keep to it on (supposedly)
		if ( glewInit() != GLEW_OK ) {
			std::cout << "Failed to initialize GLEW" << std::endl;
			return NULL;
		}


		return gwindow;

	}



//	int waitForInput(){
//
//		while ( true ) {
//
//			glfwPollEvents();
//
//			if ( glfwGetKey( gwindow, GLFW_KEY_ESCAPE ) == GLFW_PRESS )
//				return 1;
//
//			if ( glfwWindowShouldClose( gwindow ) )
//				return 1;
//
//			if ( glfwGetKey( gwindow, GLFW_KEY_ENTER ) == GLFW_PRESS )
//				return 2;
//
//		}
//
//	}



	int destroyWindow() {
		// Close OpenGL window and terminate GLFW
		glfwTerminate();
		return 0;
	}



	void setBackground( float r, float g, float b, float a ){
		glClearColor( r, g, b, a );
	}



	void setupKeyboard( GLFWwindow* window, GLFWkeyfun keyCallback ) {
		glfwSetInputMode( window, GLFW_STICKY_KEYS, GL_TRUE );
		glfwSetKeyCallback( window, keyCallback );
	}







	GLuint loadShaders( std::string vertexShaderFile, std::string fragmentShaderFile ) {
		return loadShaders( vertexShaderFile.c_str(), fragmentShaderFile.c_str() );
	}



	GLuint loadShaders( const char* vertexShaderFile, const char* fragmentShaderFile ) {

		// load and compile vertex shader
		const char* vertexShaderSrc = util::readFileContent( vertexShaderFile ).c_str();
		GLuint vertexShader = glCreateShader( GL_VERTEX_SHADER );
		glShaderSource( vertexShader, 1, &vertexShaderSrc, NULL );
		glCompileShader( vertexShader );

		GLint status;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
		if ( status != GL_TRUE ) {
			std::cout << "Vertex shader compilation failed!" << std::endl;
			return 0;
		}

		// load and compile fragment shader
		const char* fragmentShaderSrc = util::readFileContent( fragmentShaderFile ).c_str();
		GLuint fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );
		glShaderSource( fragmentShader, 1, &fragmentShaderSrc, NULL );
		glCompileShader( fragmentShader );

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
		if ( status != GL_TRUE ) {
			std::cout << "Fragment shader compilation failed!" << std::endl;
			return 0;
		}

		// combine shaders into program
		GLuint shaderProgram = glCreateProgram();
		glAttachShader( shaderProgram, vertexShader );
		glAttachShader( shaderProgram, fragmentShader );

		// link the program
		glLinkProgram( shaderProgram );

		// the shader objects are linked into the program and thus not needed anymore
		glDetachShader( shaderProgram, vertexShader );
		glDetachShader( shaderProgram, fragmentShader );
		glDeleteShader( vertexShader );
		glDeleteShader( fragmentShader );

		return shaderProgram;

	}



}



