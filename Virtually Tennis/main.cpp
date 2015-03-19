#include <ctime>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

// Include GLEW (OpenGL Extension Wrangler)
#include <GL\glew.h>

// Include GLFW (OpenGL Framework)
#include <GLFW\glfw3.h>
GLFWwindow* window;

// Include GLM (OpenGL Mathematics)
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
using namespace glm; // Save having to type glm:: everywhere

#include "gl_utils.h"
#include "obj_parser.h"
#include "game.h"
#include "court.h"
#include "text.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 800 // Dimensions in pixels

int main( void )
{
	// Seed the random generator
	srand(static_cast <unsigned>(time(0)));

	// Initialise GLFW
	if( !glfwInit() ){
		fprintf( stderr, "Failed to initialize GLFW\n" );
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow( WINDOW_WIDTH, WINDOW_HEIGHT, "Virtually Tennis - TJ Matthews", NULL, NULL);
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
	glfwSetCursorPos(window, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

	// Enable depth test
	// Accept fragment if it is closer to the camera than any other
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CCW);

	// Paint the background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	assert(init_text_rendering(
		"Fonts/freemono.png",
		"Fonts/freemono.meta",
		WINDOW_WIDTH,
		WINDOW_HEIGHT));

	init_game();
	
	do {
		static double lastTime = glfwGetTime();
		double currentTime = glfwGetTime();
		float deltaTime = float(currentTime - lastTime);
		
		update_game(deltaTime);

		glClear( GL_DEPTH_BUFFER_BIT );
		draw_texts();

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

		lastTime = currentTime;
	}
	while ( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0 );

	clean_court();

	// Close OpenGL window and terminate GLFS
	glfwTerminate();

	return 0;
}