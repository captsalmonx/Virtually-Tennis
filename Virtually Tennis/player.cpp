// Include GLFW
#include <GL\glew.h>
#include <GLFW\glfw3.h>
extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" as declared in main.cpp

// Include GLM
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
using namespace glm; // Allow us to not have to type glm::

#include "obj_parser.h"
#include "collision.h"
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

object * ball; // constant reference to ball
void setBall(object * ballPointer){
	ball = ballPointer;
}

#define PLAYERHEIGHT 4.0f
#define PLAYERSPAWN vec3(-5.0f, PLAYERHEIGHT, 35.0f)
#define MOVESPEED 15.0f
#define ROTATESPEED 0.005f

#define SWINGPOWER 35.0f
#define SWINGACCEL 4.0f

vec3 position = PLAYERSPAWN; // Initial position pulled back on Z to start near court edge
vec3 direction;

float horizontalAngle = 3.14f; // Initial horizontal angle towards -Z
float verticalAngle = 0.0f; // Initial vertical angle 0
float swing, swingVelocity;
bool swingHit;

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
	horizontalAngle += ROTATESPEED * float( width / 2 - xpos );
	verticalAngle   += ROTATESPEED * float( height / 2 - ypos );

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
		position += direction * delta * MOVESPEED; 
	}
	if (glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS){
		position -= direction * delta * MOVESPEED; 
	}
	if (glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS){
		position += right * delta * MOVESPEED;
	}
	if (glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS){
		position -= right * delta * MOVESPEED;
	}

	char tmp[52];
	if (glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_LEFT ) == GLFW_PRESS){
		if(swing <= 0.0f){
			swingHit = isBallOnScreen();
		}
		swingVelocity += SWINGACCEL * delta;
		swing += swingVelocity * delta;
		swing = min(swing, 1.0f);

		sprintf(tmp, "%02f", swing);
		update_text(powerText, tmp);
	}
	else if (glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_LEFT ) == GLFW_RELEASE && swing > 0.0f){
		if(swingHit && isBallNearPlayer()){
			vec3 ballDir = normalize(ball->pos - position);
			vec3 swingVec = direction * SWINGPOWER;
			vec3 swingRef = reflectVelocity(ball->vel, ballDir);

			ball->vel += (swingVec * swing) + (swingRef * (1.0f - swing));	
		}
		sprintf(tmp, "%02f RELEASE!", swing);
		update_text(powerText, tmp);
		swing = 0.0f;
		swingVelocity = 0.0f;
	}

	position.y = PLAYERHEIGHT; // Throttle the player to remain on ground

	// Camera/view matrix
	ViewMatrix = lookAt(
		position,				// Camera position
		position + direction,	// Where is it facing?
		up						// Up orientation, up is up
		);
}

bool isBallOnScreen()
{
	return dot(direction, normalize(ball->pos - position)) >= 0.9f;
}

bool isBallNearPlayer()
{
	return distance(ball->pos, position) <= 5.0f;
}