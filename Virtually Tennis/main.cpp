// Include standard header
#include <stdio.h>

// Include GLEW (OpenGL Extension Wrangler)
#include <GL\glew.h>

// Include GLFW (OpenGL Framework)
#include <GLFW\glfw3.h>

// Include GLM (OpenGL Mathematics)
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

using namespace glm; // Save having to type glm:: everywhere

#include "loadShader.h"

int main( void )
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow( 800, 800, "Virtually Tennis - TJ Matthews", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, then get another GPU.\n" );
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW. GLEW is not OK.\n");
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "vertexShader.glsl", "fragmentShader.glsl" );

}