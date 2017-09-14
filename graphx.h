/*
 * graphx.h
 *
 *  Created on: Sep 14, 2017
 *      Author: south
 */

#ifndef GRAPHX_H_
#define GRAPHX_H_

#include <stdlib.h>
#include <string>




#define GLEW_STATIC			// static link is probably better with graphics...
#include <GL/glew.h>		// must be first of GL stuff
#include <GLFW/glfw3.h>


// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>



namespace gx {



	int openWindow( int, int, const char* );
	int openWindow( int, int, std::string );
	int closeWindow( void );

	GLuint loadShaders( const char*, const char* );
	GLuint loadShaders( std::string, std::string );

	void setBackground( float, float, float, float );

	void enableKeyboard( void );
	void drawingLoop();





}



#endif /* GRAPHX_H_ */
