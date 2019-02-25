/*
 * graphx.cpp
 *
 *  Created on: Sep 14, 2017
 *      Author: south
 */


#include "graphx.h"

#include <iostream>
#include <iterator>
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <unistd.h>

#include "util.h"
#include "structs.h"
#include "Organism.h"
#include "Habitat.h"
#include "Animat.h"


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
	void drawHabitat( GLFWwindow* context, GLuint shaderProg ) {

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

		int n_food = HABITAT->grassTree.count();
		int n_anis = HABITAT->population.size();
//		int n_points = n_food + n_anis;

		glPointSize( 1.0f );
		glDrawArrays( GL_POINTS, 0, n_food );

		glPointSize( 5.0f );
		glDrawArrays( GL_POINTS, n_food, n_anis );

		// swap buffers
		glfwSwapBuffers( context );


	}



	void drawFCM( GLFWwindow* context, Animat* ani, GLuint shaderProg, GLuint posBuffer, GLuint linBuffer ) {

		// in case it is not so
		glfwMakeContextCurrent( context );

		// draw
		glClear( GL_COLOR_BUFFER_BIT );
		glUseProgram( shaderProg );


		// first draw the links
		glBindBuffer( GL_ARRAY_BUFFER, linBuffer );

		// position attributes
		GLint posAttr = glGetAttribLocation( shaderProg, "position" );
		glEnableVertexAttribArray( posAttr );
		glVertexAttribPointer( posAttr, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), 0 );

		// color attributes
		GLint colAttr = glGetAttribLocation( shaderProg, "inColor" );
		glEnableVertexAttribArray( colAttr );
		glVertexAttribPointer( colAttr, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(2*sizeof(float)) );

		int n_line_v = util::countNonZeroElements( ani->cognition.L ) * 2;

//		glLineWidth( 80.0f );
		glDrawArrays( GL_LINES, 0, n_line_v );


		// now draw the concepts
		glBindBuffer( GL_ARRAY_BUFFER, posBuffer );

		// position attributes
		glVertexAttribPointer( posAttr, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), 0 );

		// color attributes
		glVertexAttribPointer( colAttr, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(2*sizeof(float)) );

		int n_points = ani->cognition.nConcepts;

		glPointSize( 20.0f );
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



	void loadHabitatIntoBuffer( GLuint vao, GLuint vbo, std::string trackedName ) {

		glBindVertexArray( vao );

		int n_data_food = HABITAT->grassTree.count(),
			n_data_pop = HABITAT->population.size(),
			n_data = n_data_food + n_data_pop,
			env_w = HABITAT->sizeX,
			env_h = HABITAT->sizeY,
			counter = 0;

		util::coordinate rng0 ( 0, 0 );
		util::coordinate lmts ( env_w, env_h );

		// TODO: make sure that this number is never exceeded: maximally 1000000 entities in the world, never more
		static GLfloat points_buffer_data[5000000]; //= new GLfloat[n_data*5];		// << each point has 5 datums: X Y R G B

		// TODO
		// add green food points
		std::vector<Organism*> grass = HABITAT->grassTree.rangeQuery( rng0, lmts, lmts );
		for ( auto herb : grass ) {

			int px = static_cast<int>( herb->posX );
			int py = static_cast<int>( herb->posY );

			points_buffer_data[counter*5 + 0] = 2.0f*px / float(env_w) - 1; 	// X
			points_buffer_data[counter*5 + 1] = 2.0f*py / float(env_h) - 1; 	// Y
			points_buffer_data[counter*5 + 2] = 0.0f;							// R
			points_buffer_data[counter*5 + 3] = 0.9f;							// G
			points_buffer_data[counter*5 + 4] = 0.0f;							// B

			++counter;

		}

//		for ( int i = 0; i < env_w; ++i ) {
//			for ( int j = 0; j < env_h; ++j ) {
//				if ( environment->foodReserve(i,j) != 0 ) {
//
//					points_buffer_data[counter*5 + 0] = 2.0f*i / float(env_w) - 1; 	// X
//					points_buffer_data[counter*5 + 1] = 2.0f*j / float(env_h) - 1; 	// Y
//					points_buffer_data[counter*5 + 2] = 0.0f;						// R
//					points_buffer_data[counter*5 + 3] = 0.9f;						// G
//					points_buffer_data[counter*5 + 4] = 0.0f;						// B
//
//					++counter;
//
//				}
//				if ( counter == n_data_food )
//					break;
//			}
//			if ( counter == n_data_food )
//				break;
//		}

		// add yellow/red animat points
		std::map<std::string, Animat*>::iterator it;
		for ( it = HABITAT->population.begin(); it != HABITAT->population.end(); ++it ) {

			points_buffer_data[counter*5 + 0] = float( 2.0f * it->second->posX / float(env_w) - 1 );	// X
			points_buffer_data[counter*5 + 1] = float( 2.0f * it->second->posY / float(env_h) - 1 );	// Y
			points_buffer_data[counter*5 + 2] = 0.95f;													// R
			if ( trackedName.compare( "" ) != 0 && it->first.compare( trackedName ) == 0 )
				points_buffer_data[counter*5 + 3] = 0.95f;												// G
			else
				points_buffer_data[counter*5 + 3] = 0.0f;												// G
			points_buffer_data[counter*5 + 4] = 0.0f;													// B

			++counter;

		}

		// load the points into buffer
		glBindBuffer( GL_ARRAY_BUFFER, vbo );
		glBufferData( GL_ARRAY_BUFFER, n_data*5*sizeof(float), points_buffer_data, GL_STATIC_DRAW );

	}



	void loadFCMIntoBuffer( ecosystem::Animat* ani, GLuint vao, GLuint vbo_pos, GLuint vbo_lin ) {

		glBindVertexArray( vao );

		int n_concepts = ani->cognition.nConcepts;
		int n_conceptsInLayer[3] = {ani->cognition.nSensory, ani->cognition.nMental, ani->cognition.nMotor};
		std::vector<int> concepts_layers[3] = {ani->cognition.sensoryConceptIdxs, ani->cognition.mentalConceptIdxs, ani->cognition.motorConceptIdxs};

		int n_point_data = n_concepts * 5;
		int n_line_data = util::countNonZeroElements( ani->cognition.L ) * 10;		// 2 vertices per line
		int i_concept = 0;
		int i_link = 0;

		Eigen::VectorXf state = ani->cognition.state;
		Eigen::MatrixXf l_mat = ani->cognition.L;

		// colours
		std::vector<float> colour_n1 (3, 0.0f);
		std::vector<float> colour_0 (3, 0.0f);
		std::vector<float> colour_p1 (3, 0.0f);
		colour_n1[2] = 1.0f;
		colour_p1[0] = 1.0f;

		static GLfloat points_buffer_data[1000];
		static GLfloat lines_buffer_data[5000];

		// concept related vars
		float c_x_pos[3] = {-0.5f, 0.0f, 0.5f};

		// line ending related vars


		for ( int i = 0; i < 3; ++i ) {

			int n_cur = n_conceptsInLayer[i];
			float x_cur = c_x_pos[i];
			float y_step = 2.0f/float(n_cur + 1);

			for ( int j = 0; j < n_cur; ++j ) {

				float y_cur = 1 - (j+1)*y_step;
				float s_val = state(i_concept);
				std::vector<float> gradColour( 3, 0.0f );
				if ( s_val < 0 )
					gradColour = util::getColourFromGradient( colour_0, colour_n1, -s_val );
				else
					gradColour = util::getColourFromGradient( colour_0, colour_p1, s_val );

				// concept vertex data
				points_buffer_data[i_concept*5 + 0] = x_cur;
				points_buffer_data[i_concept*5 + 1] = y_cur;
				points_buffer_data[i_concept*5 + 2] = gradColour[0];
				points_buffer_data[i_concept*5 + 3] = gradColour[1];
				points_buffer_data[i_concept*5 + 4] = gradColour[2];

				// concepts connection lines
				for ( int k = 0; k < n_concepts; ++k ) {

					if ( l_mat(i_concept, k) != 0 ) {

						int end_layer = -1, i_in_end_layer = -1;
						std::vector<int> current_layer;
						std::vector<int>::iterator it;
						// find where exactly is the ending node...
						for ( int l = 0; l < 3; l++ ) {

							current_layer = concepts_layers[l];
							it = std::find( current_layer.begin(), current_layer.end(), k );

							if ( it != current_layer.end() ) {
								end_layer = l;
								i_in_end_layer = it-current_layer.begin();
								break;
							}

						}

						if ( end_layer < 0 || i_in_end_layer < 0 )
							// there was some kind of error, what to do?
							continue;


						// line start
						lines_buffer_data[i_link*10 + 0] = x_cur;
						lines_buffer_data[i_link*10 + 1] = y_cur;
						if ( l_mat(i_concept, k) < 0 ) {
							lines_buffer_data[i_link*10 + 2] = 1.0;
							lines_buffer_data[i_link*10 + 3] = 0.0;
							lines_buffer_data[i_link*10 + 4] = 0.0;
						}
						else {
							lines_buffer_data[i_link*10 + 2] = 0.0;
							lines_buffer_data[i_link*10 + 3] = 1.0;
							lines_buffer_data[i_link*10 + 4] = 0.0;
						}

						// line end
						float x_end = c_x_pos[end_layer];
						float y_end_step = 2.0f/float(n_conceptsInLayer[end_layer] + 1);
						float y_end = 1 - (i_in_end_layer+1)*y_end_step;

						lines_buffer_data[i_link*10 + 5] = x_end;
						lines_buffer_data[i_link*10 + 6] = y_end;
						if ( l_mat(i_concept, k) < 0 ) {
							lines_buffer_data[i_link*10 + 7] = 1.0;
							lines_buffer_data[i_link*10 + 8] = 0.0;
							lines_buffer_data[i_link*10 + 9] = 0.0;
						}
						else {
							lines_buffer_data[i_link*10 + 7] = 0.0;
							lines_buffer_data[i_link*10 + 8] = 1.0;
							lines_buffer_data[i_link*10 + 9] = 0.0;
						}

						++i_link;

					}

				}

				++i_concept;

			}

		}

		// load data points into position buffer
		glBindBuffer( GL_ARRAY_BUFFER, vbo_pos );
		glBufferData( GL_ARRAY_BUFFER, n_point_data*sizeof(float), points_buffer_data, GL_STATIC_DRAW );

		// load lines data into lines buffer
		glBindBuffer( GL_ARRAY_BUFFER, vbo_lin );
		glBufferData( GL_ARRAY_BUFFER, n_line_data*sizeof(float), lines_buffer_data, GL_STATIC_DRAW );

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



	int switchContextToWindow( GLFWwindow* window ){

		glfwMakeContextCurrent( window );

		// initialize GLEW
		glewExperimental = true;
		if ( glewInit() != GLEW_OK ) {
			std::cout << "Failed to initialize GLEW" << std::endl;
			return -1;
		}

		return 0;

	}



	int destroyWindow() {
		// Close OpenGL window and terminate GLFW
		glfwTerminate();
		return 0;
	}



	void setBackground( float r, float g, float b, float a ){
		glClearColor( r, g, b, a );
	}



	void setupKeyboard( GLFWwindow* window ) {
		glfwSetInputMode( window, GLFW_STICKY_KEYS, GL_TRUE );
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
		std::string vertexShaderSrcString = util::readFileContent( vertexShaderFile );
		const char* vertexShaderSrc = vertexShaderSrcString.c_str();
		GLuint vertexShader = glCreateShader( GL_VERTEX_SHADER );
		glShaderSource( vertexShader, 1, &vertexShaderSrc, NULL );
		glCompileShader( vertexShader );

		GLint status;
		glGetShaderiv( vertexShader, GL_COMPILE_STATUS, &status );
		if ( status != GL_TRUE ) {
			std::cout << "Vertex shader compilation failed!" << std::endl;

			GLint maxLength = 0;
			glGetShaderiv( vertexShader, GL_INFO_LOG_LENGTH, &maxLength );

			// The maxLength includes the NULL character
			std::vector<GLchar> errorLog( maxLength );
			glGetShaderInfoLog( vertexShader, maxLength, &maxLength, &errorLog[0] );
			std::copy( errorLog.begin(), errorLog.end(), std::ostream_iterator<GLchar>( std::cout, "" ) );
			std::cout << std::endl;

			std::cout << vertexShaderSrc << std::endl << "###" << std::endl;
			std::cout << vertexShaderSrcString << std::endl;

			// Exit with failure.
			glDeleteShader( vertexShader ); // Don't leak the shader.
			return 0;
		}

		// load and compile fragment shader
		std::string fragmentShaderSrcString = util::readFileContent( fragmentShaderFile );
		const char* fragmentShaderSrc = fragmentShaderSrcString.c_str();
		GLuint fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );
		glShaderSource( fragmentShader, 1, &fragmentShaderSrc, NULL );
		glCompileShader( fragmentShader );

		glGetShaderiv( fragmentShader, GL_COMPILE_STATUS, &status );
		if ( status != GL_TRUE ) {
			std::cout << "Fragment shader compilation failed!" << std::endl;

			GLint maxLength = 0;
			glGetShaderiv( fragmentShader, GL_INFO_LOG_LENGTH, &maxLength );

			// The maxLength includes the NULL character
			std::vector<GLchar> errorLog( maxLength );
			glGetShaderInfoLog( fragmentShader, maxLength, &maxLength, &errorLog[0] );
			std::copy( errorLog.begin(), errorLog.end(), std::ostream_iterator<GLchar>( std::cout, "" ) );
			std::cout << std::endl;

			std::cout << fragmentShaderSrc << std::endl << "###" << std::endl;
			std::cout << fragmentShaderSrcString << std::endl;

			// Exit with failure.
			glDeleteShader( fragmentShader ); // Don't leak the shader.
			return 0;
		}

		// combine shaders into program
		GLuint shaderProgram = glCreateProgram();
		glAttachShader( shaderProgram, vertexShader );
		glAttachShader( shaderProgram, fragmentShader );

		// link the program
		glLinkProgram( shaderProgram );

		// check if link successful
		glGetProgramiv( shaderProgram, GL_LINK_STATUS, &status );
		if ( status != GL_TRUE ) {
			std::cout << "Shader program linking failed!" << std::endl;
			return 0;
		}

		// the shader objects are linked into the program and thus not needed anymore
		glDetachShader( shaderProgram, vertexShader );
		glDetachShader( shaderProgram, fragmentShader );
		glDeleteShader( vertexShader );
		glDeleteShader( fragmentShader );

		return shaderProgram;

	}



}



