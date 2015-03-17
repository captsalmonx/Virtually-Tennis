// Include standard header
#include <stdio.h>

// Include GLEW (OpenGL Extension Wrangler)
#include <GL\glew.h>

// Include GLFW (OpenGL Framework)
#include <GLFW\glfw3.h>
GLFWwindow* window;

// Include GLM (OpenGL Mathematics)
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
using namespace glm; // Save having to type glm:: everywhere

#include "player.h"
#include "court.h"

GLuint width = 1280;
GLuint height = 800;

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

	window = glfwCreateWindow( width, height, "Virtually Tennis - TJ Matthews", NULL, NULL);
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
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN); // Hide the mouse cursor
	glfwSetCursorPos(window, width / 2, height / 2);

	// Paint the background
	glClearColor(0.1f, 0.2f, 0.2f, 0.0f);

	// Enable depth test
	// Accept fragment if it is closer to the camera than any other
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	init_court();

	do {
		// Clear screen
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		playerLoop();
		draw_court();

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	while ( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0 );

	clean_court();

	// Close OpenGL window and terminate GLFS
	glfwTerminate();

	return 0;
}