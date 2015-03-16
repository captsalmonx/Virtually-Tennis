// Include GLFW
#include <GLFW/glfw3.h>
extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" as declared in main.cpp

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm; // Allow us to not have to type glm::

#include "player.h"

// Variables to be passed back into the game loop in main.cpp
mat4 ViewMatrix;
mat4 ProjectionMatrix;

mat4 getViewMatrix(){
	return ViewMatrix;
}

mat4 getProjectionMatrix(){
	return ProjectionMatrix;
}

const float playerHeight = 5.0f;
vec3 position = vec3( 0.0f, playerHeight, 15.0f ); // Initial position pulled back on Z to start near court edge
float horizontalAngle = 3.14f; // Initial horizontal angle towards -Z
float verticalAngle = 0.0f; // Initial vertical angle 0
float initialFoV = 45.0f; // Initial Field of View
float speed = 15.0f; // 3 units per second

float mouseSpeed = 0.005f; // Mouse/camera rotation speed

void playerLoop()
{
	// Static definition means that this is only given a value from glfwGetTime once
	// afterwards it's updated at the end of this function
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	// so that it doesn't matter whether you have a fast or slow computer
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	// Reset mouse position for next frame (this middle of window)
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	glfwSetCursorPos(window, width / 2, height / 2);

	// Compute new orientation
	horizontalAngle += mouseSpeed * float( width / 2 - xpos );
	verticalAngle   += mouseSpeed * float( height / 2 - ypos );

	// Direction: Spherical coordinates to Cartesian coordinates conversion
	vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
		);

	// Right vector
	vec3 right = vec3(
		sin(horizontalAngle - 3.14f / 2.0f),
		0,
		cos(horizontalAngle - 3.14f / 2.0f)
		);

	// Up vector
	// Remember that the cross product of the two vectors gives a vector
	// perpenticular to the surface / plane consisting of the two vectors
	vec3 up = cross( right, direction );

	// Move forward
	if (glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS){
		position += direction * deltaTime * speed; 
	}
	if (glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS){
		position -= direction * deltaTime * speed; 
	}
	if (glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS){
		position += right * deltaTime * speed;
	}
	if (glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS){
		position -= right * deltaTime * speed;
	}

	float FoV = initialFoV * (glfwGetKey( window, GLFW_KEY_F ) == GLFW_PRESS ? 0.99f : 1.0f);

	// Projection matrix : 45 degree field of view, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = perspective(FoV, 4.0f / 3.0f, 0.1f, 100.0f);

	position.y = playerHeight; // Throttle the player to remain on ground

	// Camera/view matrix
	ViewMatrix = lookAt(
		position,				// Camera position
		position + direction,	// Where is it facing?
		up						// Up orientation, up is up
		);

	// For the next frame last time will be current time, i.e. now
	lastTime = currentTime;
}