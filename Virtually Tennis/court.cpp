#include <stdio.h>
#include <random>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" as declared in main.cpp

// Include GLM (OpenGL Mathematics)
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
using namespace glm; // Save having to type glm:: everywhere

#include "gl_utils.h"
#include "obj_parser.h"
#include "game.h"
#include "player.h"
#include "text.h"
#include "court.h"
#include "collision.h"
#include "audioPlayer.h"

#define VERT_SHADER "Shaders/base.vert"
#define FRAG_SHADER "Shaders/base.frag"

#define BALL_MESH "Assets/ball.obj"
#define BALL_SOUND_COURT "Assets/ball.wav"
#define BALL_SOUND_TARGET "Assets/target.wav"
#define BALL_SPAWN vec3(-2.0f, 5.0f, 27.0f)
#define GRAVITY vec3(0.0f, -32.0f, 0.0f)

const vec2 court_dimensions = vec2(36.0f, 78.0f);

#define TARGET_EDGEBUFFER 0.1f
#define TARGET_SPAWNPROB 0.5f
#define TARGET_SOUND "Assets/spawn.ogg"

#define HIT_SOUND "Assets/swing.wav"
#define MISS_SOUND "Assets/miss.flac"

const char* ball_textures[3] = {"Assets/ball_1.png", "Assets/ball_2.png", "Assets/ball_3.png"};
GLuint ball_texIDs[3];
const char* court_textures[3] = {"Assets/court_1.png", "Assets/court_2.png", "Assets/court_3.png"};
GLuint court_texIDs[3];

const float target_ranges[3] = { 5.0f, 10.0f, 25.0f };
const char* target_textures[3] = {"Assets/target_1.png", "Assets/target_2.png", "Assets/target_3.png"};
const vec2 target_dimensions[3] = {vec2(6.0f, 9.0f), vec2(7.0f, 11.0f), vec2(8.0f, 9.0f)};
GLuint target_texIDs[3];

plane court;
plane targets[10]; // Maximum 10 targets
object ball;

int *scorePointer, *comboPointer, *difficultyPointer, *levelPointer;

bool hit = false;

bool init_court(int * score, int * combo, int * difficulty, int * level) {
	float* points = NULL;
	float* uvs = NULL;
	float* normals = NULL;

	// Give pointers to player class
	setBall(&ball);
	setDimensions(&court_dimensions);

	// Take pointers from game class
	scorePointer = score;
	comboPointer = combo;
	difficultyPointer = difficulty;
	levelPointer = level;

	// Create and load court floor
	if(!create_plane(&court, points, uvs, normals, court_dimensions, vec3(0.0f, 0.0f, 0.0f),	vec3(-90.0f, 0.0f, 0.0f))){
		fprintf(stderr, "Error loading court\n");
		return false;
	}
	bind_object(&court.obj, VERT_SHADER, FRAG_SHADER, points, uvs, normals);

	// Create and load ball
	if(!load_obj_file(BALL_MESH, points, uvs, normals,	ball.point_count)){
		fprintf(stderr, "Error loading ball mesh\n");
		return false;
	}
	bind_object(&ball, VERT_SHADER, FRAG_SHADER, points, uvs, normals);
	ball.pos = BALL_SPAWN;

	// Load all textures now so that be switched between at ease
	// This isn't memory effecient, but is easier for a game of this scale
	for(int i = 0; i < 3; i++){
		ball_texIDs[i] = create_texture_from_file(ball_textures[i]);
		court_texIDs[i] = create_texture_from_file(court_textures[i]);
		target_texIDs[i] = create_texture_from_file(target_textures[i]);
	}

	return true;
}

void reset_court(){
	// Reset ball back to it's start position
	reset_ball();

	// Give the court the texture of the current level
	court.obj.textureID = court_texIDs[*levelPointer];
	ball.textureID = ball_texIDs[*levelPointer];

	// Remove any remaining targets from the court
	for(int i = 0; i < sizeof(targets) / sizeof(plane); i++){
		if(targets[i].dim.x > 0.0f){
			delete_target(&targets[i]);
		}
	}
}

bool create_target(){
	float* points = NULL;
	float* uvs = NULL;
	float* normals = NULL;

	bool inactiveTarget = false;
	int i, iref;
	vec3 position, rotation;

	// Half the dimensions of the court
	float xhalf = court.dim.x / 2.0f, yhalf = court.dim.y / 2.0f;

	// Add a buffer so they don't actually spawn right on the edge of the court
	float xbuffer = xhalf * TARGET_EDGEBUFFER, ybuffer = yhalf * TARGET_EDGEBUFFER;

	// Find an empty target
	for(i = 0; i < sizeof(targets) / sizeof(plane); i++){
		// Has this target been initialized and thus in action?
		if(targets[i].dim.x == 0.0f){
			inactiveTarget = true;
			break;
		}
	}

	if(!inactiveTarget){
		fprintf(stderr, "Too many targets already\n");
		return false;
	}

	// Pick a random target model
	iref = rand() % 3;
	
	// Find a random position on the court
	position.x = randRange(court.obj.pos.x - xhalf + xbuffer, court.obj.pos.x + xhalf - xbuffer);
	position.y = randRange(court.obj.pos.y - yhalf + ybuffer, court.obj.pos.y - ybuffer);
	position = (vec3)(vec4(position, 1) * getRotationMatrix(court.obj.rot));

	// Adjust position to account for texture
	position.y += target_dimensions[iref].y / 2;

	// Pick a random rotation - range here is dependant on game difficulty
	rotation.x = randRange(-target_ranges[*difficultyPointer] / 2, target_ranges[*difficultyPointer] / 2);
	rotation.y = randRange(-target_ranges[*difficultyPointer] / 2, target_ranges[*difficultyPointer] / 2);
	// Rotate round to display the texture properly
	rotation.z = 180.0f;

	// Create and bind this new object
	if(!create_plane(&targets[i], points, uvs, normals, target_dimensions[iref], position, rotation)){
		fprintf(stderr, "Error loading target\n");
		return false;
	}
	bind_object(&targets[i].obj, VERT_SHADER, FRAG_SHADER, points, uvs, normals);

	// Load the texture of the chosen model
	targets[i].obj.textureID = target_texIDs[iref];
	return true;
}

void update_court(float delta){
	update_targets(delta);
	update_ball(delta);
}

void update_targets(float delta){
	for(int i = 0; i < sizeof(targets) / sizeof(plane); i++){
		// Has the target been initalized and thus in action?
		if(targets[i].dim.x == 0.0f){
			// Check the probability to creating a new target
			// Targets are less likely to spawn if there are more targets existing
			// This isn't a perfect checker, otherwise it would require running this for loop twice
			if(rand() % (int)((TARGET_SPAWNPROB / delta) * ((i + 1) * (i + 1) / 2.0f)) == 0){
				if(create_target()){
					// Play a spawning sound
					playSound(TARGET_SOUND);
				}
			}
			break;
		}
	}
}

void hitBall(vec3 position, vec3 direction, float power, float swing){
	vec3 ballDir = normalize(ball.pos - position);
	vec3 swingVec = direction * power;
	vec3 swingRef = reflectVelocity(ball.vel, ballDir);

	// The ball's new velocity is between a directed shot, and a vague reflection, depending on the fullness of the swing
	ball.vel = (swingVec * swing) + (swingRef * (1.0f - swing));
	playSound(HIT_SOUND);
}

void missBall(){
	playSound(MISS_SOUND);
}

void delete_target(plane * target){
	target->dim = vec2(0.0f);
	delete_object(&target->obj);
}

void update_ball(float delta) {
	// Add gravity to the velocity
	ball.vel += GRAVITY * delta;

	// Check collision before moving the ball position
	// If it's collided then it's alrighted moved
	if(!checkCollision_ball(delta)){
		ball.pos += ball.vel * delta;
	}
	
	// If the ball misses the court it will fall below the y = 0 mark
	if (ball.pos.y <= -5.0f){
		reset_ball();
	}
}

void reset_ball(){
	// Reset back to it's spawn poitn
	ball.pos = BALL_SPAWN;
	ball.vel = vec3(0.0f, 0.0f, 0.0f);

	// Any hit is invalidated
	hit = false;

	// The combo streak has been lost
	*comboPointer = 0;
}

bool checkCollision_ball(float delta){
	// Has it collided with the court?
	if(planeObjectCollision(court.obj.pos, court.obj.rot, court.dim, &ball.pos, &ball.vel, delta)){
		playSound(BALL_SOUND_COURT);
		return true;
	}

	// Has it collided with a target?
	for(int i = 0; i < sizeof(targets) / sizeof(plane); i++){
		if(targets[i].dim.x != 0.0f){
			if(planeObjectCollision(targets[i].obj.pos, targets[i].obj.rot, targets[i].dim, &ball.pos, &ball.vel, delta)){
				delete_target(&targets[i]);
				// Increase score and combo
				*comboPointer += 1;
				*scorePointer += *comboPointer;
				// New highscore?
				checkCombo();
				playSound(BALL_SOUND_TARGET);
				return true;
			}
		}
	}

	return false;
}

void draw_court() {
	// Use our shader
	mat4 projectionMatrix = getProjectionMatrix();
	mat4 viewMatrix = getViewMatrix();

	// Draw game objects
	draw_object(&court.obj, projectionMatrix, viewMatrix, GL_TRIANGLE_STRIP);
	draw_object(&ball, projectionMatrix, viewMatrix, GL_TRIANGLES);
	for(int i = 0; i < sizeof(targets) / sizeof(plane); i++){
		if(targets[i].dim.x != 0.0f && targets[i].dim.y != 0.0f){
			draw_object(&targets[i].obj, projectionMatrix, viewMatrix, GL_TRIANGLE_STRIP);
		}
	}
}

void clean_court() {
	// Free memory
	delete_object(&court.obj);
	delete_object(&ball);
	for(int i = 0; i < sizeof(targets) / sizeof(plane); i++){
		delete_object(&targets[i].obj);
	}

	for(int i = 0; i < 3; i++){
		glDeleteTextures(1, &ball_texIDs[i]);
		glDeleteTextures(1, &court_texIDs[i]);
		glDeleteTextures(1, &target_texIDs[i]);
	}
}