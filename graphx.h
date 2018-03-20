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


// Include GLM		don't need it, don't have it
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>

// forward declarations
namespace ecosystem {
	class Animat;
	class Habitat;
	class Organism;
}


//class ecosystem::Habitat;


namespace gx {



	GLFWwindow* createWindow( int, int, const char* );
	GLFWwindow* createWindow( int, int, std::string );
	int destroyWindow( void );

	GLuint loadShaders( const char*, const char* );
	GLuint loadShaders( std::string, std::string );

	GLuint createVBO( void );
	GLuint createAndBindVAO( void );
	void bindVAO( GLuint );

	int switchContextToWindow( GLFWwindow* );

	void loadHabitatIntoBuffer( ecosystem::Habitat*, GLuint, GLuint );
	void loadFCMIntoBuffer( ecosystem::Animat*, GLuint, GLuint, GLuint );

	void setBackground( float, float, float, float );
	void setupKeyboard( GLFWwindow* );
	void setupKeyboard( GLFWwindow*, GLFWkeyfun );

	void drawingLoop();
	void drawHabitat( GLFWwindow*, ecosystem::Habitat*, GLuint );
	void drawFCM( GLFWwindow*, ecosystem::Animat*, GLuint, GLuint, GLuint );

	int waitForInput();





}



#endif /* GRAPHX_H_ */
