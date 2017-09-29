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

#include "util.h"
#include "Habitat.h"



using namespace ecosystem;


namespace gx {

	GLFWwindow* gwindow;



	void drawingLoop() {

		GLuint shaderProg = gx::loadShaders( "shader.vert", "shader.frag" );

		while ( !glfwWindowShouldClose( gwindow ) ) {

			glClear( GL_COLOR_BUFFER_BIT );

			glUseProgram( shaderProg );

			// swap buffers
			glfwSwapBuffers( gwindow );
			glfwPollEvents();

			if ( glfwGetKey( gwindow, GLFW_KEY_ESCAPE ) == GLFW_PRESS )
			    glfwSetWindowShouldClose( gwindow, GL_TRUE );

		}

	}



	void draw( GLuint shaderProgramID, GLuint dataBuffer ) {

		glClear( GL_COLOR_BUFFER_BIT );

		glUseProgram( shaderProgramID );



		// swap buffers
		glfwSwapBuffers( gwindow );
		glfwPollEvents();


	}



	GLuint loadDataIntoBuffer( Habitat* environment ) {

		int n_data_food = environment->foodReserve.sum(),		// foodReserve.sum() works for now, when food is 0 or 1
			n_data_pop = environment->population.size(),
			n_data = n_data_food + n_data_pop,
			env_w = environment->sizeX,
			env_h = environment->sizeY,
			counter = 0;

		static GLfloat points_buffer_data[n_data*5];		// << each point has 5 datums: X Y R G B

		for ( int i = 0; i < env_w; i++ ) {
			for ( int j = 0; j < env_h; j++ ) {
				if ( environment->foodReserve(i,j) != 0 ) {

					points_buffer_data[counter*5 + 0] = 2.0f*i / float(env_w) - 1; 	// X
					points_buffer_data[counter*5 + 1] = 2.0f*j / float(env_h) - 1; 	// Y
					points_buffer_data[counter*5 + 2] = 0.2f;						// R
					points_buffer_data[counter*5 + 3] = 0.0f;						// G
					points_buffer_data[counter*5 + 4] = 1.0f;						// B

				}
				if ( counter == n_data_food )
					break;
			}
			if ( counter == n_data_food )
				break;
		}

		// to be continued

	}



	int openWindow( int widthPx, int heightPx, std::string title ) {
		return openWindow( widthPx, heightPx, title.c_str() );
	}

	int openWindow( int widthPx, int heightPx, const char* title ) {


		// initialize glfw first
		if ( !glfwInit() ) {
			std::cout << "Failed to initialize GLFW" << std::endl;
			return -1;
		}

		// set some properties
//		glfwWindowHint( GLFW_SAMPLES, 4 );									// anti-aliasing
		glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );					// ogl version n1
		glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );					// ogl version n2
		glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );	// only the new style opengl
		glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);							// self-explanatory

		// create a window context			the two NULLs are for fullscreen and resource sharing with existing context, respectively
		gwindow = glfwCreateWindow( widthPx, heightPx, title, NULL, NULL );
		if ( gwindow == NULL ){
			std::cout << "Failed to open a window" << std::endl;
			glfwTerminate();
			return -1;
		}

		glfwMakeContextCurrent( gwindow );


		glewExperimental=true;				// something to do with experimental extensions, better keep to it on (supposedly)
		// initialize GLEW
		if ( glewInit() != GLEW_OK ) {
			std::cout << "Failed to initialize GLEW" << std::endl;
			return -1;
		}


		return 0;

	}



	int closeWindow() {
		// Close OpenGL window and terminate GLFW
		glfwTerminate();
		return 0;
	}



	void setBackground( float r, float g, float b, float a ){
		glClearColor( r, g, b, a );
	}



	void enableKeyboard() {
		glfwSetInputMode( gwindow, GLFW_STICKY_KEYS, GL_TRUE );
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



