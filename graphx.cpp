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




namespace gx {

	GLFWwindow* gwindow;



	void drawingLoop() {

		while ( !glfwWindowShouldClose( gwindow ) ) {

			glClear( GL_COLOR_BUFFER_BIT );

			// swap buffers
			glfwSwapBuffers( gwindow );
			glfwPollEvents();

			if ( glfwGetKey( gwindow, GLFW_KEY_ESCAPE ) == GLFW_PRESS )
			    glfwSetWindowShouldClose( gwindow, GL_TRUE );

		}

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



