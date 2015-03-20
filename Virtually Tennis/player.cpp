#include <stdio.h>
#include <random>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
// Include GLFW
#include <GL\glew.h>
#include <GLFW\glfw3.h>
extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" as declared in main.cpp

// Include GLM
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
using namespace glm; // Allow us to not have to type glm::

#include "court.h"
#include "obj_parser.h"
#include "collision.h"
#include "player.h"
#include "text.h"
#include "gl_utils.h"

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
const vec2 * courtDimensions; // constant reference to court dimensions

void setBall(object * ballPointer){
	ball = ballPointer;
}

void setDimensions(const vec2 * dimPointer){
	courtDimensions = dimPointer;
}

#define PLAYERHEIGHT 4.0f
#define PLAYERSPAWN vec3(-5.0f, PLAYERHEIGHT, 35.0f)
#define MOVESPEED 30.0f
#define ROTATESPEED 0.005f

#define SWINGPOWER 35.0f
#define SWINGACCEL 20.0f

vec3 position = PLAYERSPAWN; // Initial position pulled back on Z to start near court edge
vec3 direction;

float horizontalAngle = 3.14f; // Initial horizontal angle towards -Z
float verticalAngle = 0.0f; // Initial vertical angle 0
float swing, swingVelocity;
bool swingHit;

int powerText;
const vec2 powerTextPos = vec2(-0.005f, 0.7f);

bool init_player()
{
	int width, height;

	glfwGetWindowSize(window, &width, &height);

	powerText = add_text (
		"Charge",
		powerTextPos.x, powerTextPos.y, 300.0f,
		1.0f, 1.0f, 1.0f, 0.0f
	);
	// Projection matrix : 45 degree field of view, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = perspective(45.0f, 4.0f / 3.0f, 0.1f, 200.0f);
	return true;
}

void reset_player()
{
	position = PLAYERSPAWN;
	direction = vec3(0.0f);
	horizontalAngle = 3.14f;
	verticalAngle = 0.0f;

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
		swingVelocity += SWINGACCEL * delta;
		swing += swingVelocity * delta;
		swing = min(swing, 1.0f);

		if(swing == 1.0f){
			change_text_position(powerText, powerTextPos.x + randRange(-0.01f, 0.01f), powerTextPos.y + randRange(-0.01f, 0.01f));
		}
		update_text(powerText, swing == 1.0f ? "!" : ".");
		change_text_alpha(powerText, swing);
	}
	else if (glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_LEFT ) == GLFW_RELEASE && swing > 0.0f){
		if(isBallNearPlayer()){
			hitBall(position, direction, SWINGPOWER, swing);
		}else missBall();
		swing = 0.0f;
		swingVelocity = 0.0f;
		change_text_alpha(powerText, swing);
	}

	position.x = clamp(position.x, -courtDimensions->x / 2.0f, courtDimensions->x / 2.0f);
	position.z = clamp(position.z, 0.0f, courtDimensions->y / 2.0f);
	position.y = PLAYERHEIGHT; // Throttle the player to remain on ground

	// Camera/view matrix
	ViewMatrix = lookAt(
		position,				// Camera position
		position + direction,	// Where is it facing?
		up						// Up orientation, up is up
		);
}

void draw_player()
{
	draw_text(powerText);
}

/*bool isBallOnScreen()
{
	return dot(direction, normalize(ball->pos - position)) >= 0.9f;
}*/

bool isBallNearPlayer()
{
	return distance(ball->pos, position) <= 20.0f;
}