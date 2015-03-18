// Include GLFW
#include <GL\glew.h>
#include <GLFW\glfw3.h>
extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" as declared in main.cpp

// Include GLM
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
using namespace glm; // Allow us to not have to type glm::

#include "obj_parser.h"
#include "player.h"
#include "text.h"
#include "gl_utils.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

// Variables to be passed back into the game loop in main.cpp
mat4 ViewMatrix;
mat4 ProjectionMatrix;

mat4 getViewMatrix(){
	return ViewMatrix;
}
mat4 getProjectionMatrix(){
	return ProjectionMatrix;
}

dynamicObject * ball; // constant reference to ball
void setBall(dynamicObject * ballPointer){
	ball = ballPointer;
}

const float playerHeight = 4.0f;
const vec3 playerSpawn = vec3(-5.0f, playerHeight, 35.0f);

vec3 position = playerSpawn; // Initial position pulled back on Z to start near court edge
vec3 direction;

float horizontalAngle = 3.14f; // Initial horizontal angle towards -Z
float verticalAngle = 0.0f; // Initial vertical angle 0
float moveSpeed = 15.0f, mouseSpeed = 0.005f; // Mouse/camera rotation speed
float power = 2.0f, powerVelocity = 0.0f, powerAcceleration = 0.1f;

int powerText;

void init_player()
{
	int width, height;

	glfwGetWindowSize(window, &width, &height);

	powerText = add_text (
		"TEST",
		-200.0f / (float)width,
		1.0f,
		110.0f,
		0.9f,
		0.9f,
		0.0f,
		0.8f
	);
	// Projection matrix : 45 degree field of view, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
}

void update_player(float delta)
{
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
	direction = vec3(
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
		position += direction * delta * moveSpeed; 
	}
	if (glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS){
		position -= direction * delta * moveSpeed; 
	}
	if (glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS){
		position += right * delta * moveSpeed;
	}
	if (glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS){
		position -= right * delta * moveSpeed;
	}

	if (glfwGetMouseButton( window, GLFW_RELEASE )){
		if(swingCheck()){
			ball->vel = ball->vel - (2.0f * (dot(ball->vel, direction) * direction));	
		}
	}

	position.y = playerHeight; // Throttle the player to remain on ground

	// Camera/view matrix
	ViewMatrix = lookAt(
		position,				// Camera position
		position + direction,	// Where is it facing?
		up						// Up orientation, up is up
		);
}

bool swingCheck()
{
	return dot(direction, normalize(ball->obj.pos - position)) >= 0.9f &&
		distance(ball->obj.pos, position) <= 5.0f;
}