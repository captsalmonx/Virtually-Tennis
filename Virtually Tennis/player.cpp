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
#define SWINGREACH 20.0f

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
	// Initialize text
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
	// Reset player to spawn point
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

	// Reset mouse position to centre of window
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
	vec3 up = cross( right, direction );

	// Move forward
	if (glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS){
		position += direction * delta * MOVESPEED; 
	}
	// Move backwards
	if (glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS){
		position -= direction * delta * MOVESPEED; 
	}
	// Move right
	if (glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS){
		position += right * delta * MOVESPEED;
	}
	// Move left
	if (glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS){
		position -= right * delta * MOVESPEED;
	}

	// Check swing
	char tmp[52];
	// Is a swing charging?
	if (glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_LEFT ) == GLFW_PRESS){
		// Increase swing amount by a constantly increasing velocity
		// This gives that nice quick build up
		swingVelocity += SWINGACCEL * delta;
		swing += swingVelocity * delta;
		swing = min(swing, 1.0f);

		// If fully charged flicker the text indicator
		if(swing == 1.0f){
			change_text_position(powerText, powerTextPos.x + randRange(-0.01f, 0.01f), powerTextPos.y + randRange(-0.01f, 0.01f));
		}
		// Update text to change to ! when charged
		update_text(powerText, swing == 1.0f ? "!" : ".");
		// Gradually become opaque while charing
		change_text_alpha(powerText, swing);
	}
	// Is there a swing to be processed?
	else if (glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_LEFT ) == GLFW_RELEASE && swing > 0.0f){
		// Is the ball close enough to the player to be hit?
		if(isBallNearPlayer()){
			hitBall(position, direction, SWINGPOWER, swing);
		}else missBall();

		// Reset the swing
		swing = 0.0f;
		swingVelocity = 0.0f;
		change_text_position(powerText, powerTextPos.x, powerTextPos.y);
		change_text_alpha(powerText, swing);
	}

	// Throttle the player to remain within the court
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
	// Only thing to draw here is the power text
	draw_text(powerText);
}

/*bool isBallOnScreen()
{
	// This was evidently unused, but used to be used to check if the player 
	// was looking at the ball when they swung
	// Checks if the angle between the player's direction and the difference
	// in position is within a certain range
	return dot(direction, normalize(ball->pos - position)) >= 0.9f;
}*/

bool isBallNearPlayer()
{
	// Can the player even reach the ball?
	return distance(ball->pos, position) <= SWINGREACH;
}