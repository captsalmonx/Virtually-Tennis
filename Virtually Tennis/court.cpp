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

#define VERT_SHADER "Shaders/base.vert"
#define FRAG_SHADER "Shaders/base.frag"

#define BALL_MESH "Assets/ball.obj"
#define BALL_TEXTURE "Assets/ball.jpg"
#define BALL_SPAWN vec3(-2.0f, 5.0f, 27.0f)
#define GRAVITY vec3(0.0f, -32.0f, 0.0f)

#define COURT_TEXTURE "Assets/tennis-court.png"
const vec2 court_dimensions = vec2(36.0f, 78.0f);

#define TARGET_EDGEBUFFER 0.1f
#define TARGET_SPAWNPROB 0.5f

const float target_ranges[3] = { 5.0f, 10.0f, 25.0f };
const char* target_textures[3] = {"Assets/target_1.png", "Assets/target_2.png", "Assets/target_3.png"};
const vec2 target_dimensions[3] = {vec2(6.0f, 9.0f), vec2(7.0f, 11.0f), vec2(8.0f, 9.0f)};
int target_texIDs[sizeof(target_textures) / sizeof(char*)];

plane court;
plane targets[10]; // Maximum 10 targets
object ball;

int *scorePointer, *comboPointer, *difficultyPointer;

bool hit = false;

bool init_court(int * score, int * combo, int * difficulty) {
	float* points = NULL;
	float* uvs = NULL;
	float* normals = NULL;

	setBall(&ball);
	setDimensions(&court_dimensions);
	scorePointer = score;
	comboPointer = combo;
	difficultyPointer = difficulty;

	// CREATE FLOOR
	if(!create_plane(&court, points, uvs, normals, court_dimensions, vec3(0.0f, 0.0f, 0.0f),	vec3(-90.0f, 0.0f, 0.0f))){
		fprintf(stderr, "Error loading court\n");
		return false;
	}
	bind_object(&court.obj, VERT_SHADER, FRAG_SHADER, points, uvs, normals);
	court.obj.textureID = create_texture_from_file(COURT_TEXTURE);;

	// CREATE BALL
	if(!load_obj_file(BALL_MESH, points, uvs, normals,	ball.point_count)){
		fprintf(stderr, "Error loading ball mesh\n");
		return false;
	}
	bind_object(&ball, VERT_SHADER, FRAG_SHADER, points, uvs, normals);
	ball.textureID = create_texture_from_file(BALL_TEXTURE);
	ball.pos = BALL_SPAWN;

	// CREATE TARGETS
	for(int i = 0; i < 3; i++){
		target_texIDs[i] = create_texture_from_file(target_textures[i]);
	}

	return true;
}

void reset_court(){
	ball.pos = BALL_SPAWN;
	ball.vel = vec3(0.0f);

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
	float xhalf = court.dim.x / 2.0f, yhalf = court.dim.y / 2.0f;
	float xbuffer = xhalf * TARGET_EDGEBUFFER, ybuffer = yhalf * TARGET_EDGEBUFFER;

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

	iref = rand() % 3;
	
	// Find position on the court
	position.x = randRange(court.obj.pos.x - xhalf + xbuffer, court.obj.pos.x + xhalf - xbuffer);
	position.y = randRange(court.obj.pos.y - yhalf + ybuffer, court.obj.pos.y - ybuffer);
	position = (vec3)(vec4(position, 1) * getRotationMatrix(court.obj.rot));

	// Adjust position to account for texture
	position.y += target_dimensions[iref].y / 2;

	rotation.x = randRange(-target_ranges[*difficultyPointer] / 2, target_ranges[*difficultyPointer] / 2);
	rotation.y = randRange(-target_ranges[*difficultyPointer] / 2, target_ranges[*difficultyPointer] / 2);
	rotation.z = 180.0f;

	if(!create_plane(&targets[i], points, uvs, normals, target_dimensions[iref], position, rotation)){
		fprintf(stderr, "Error loading target\n");
		return false;
	}
	bind_object(&targets[i].obj, VERT_SHADER, FRAG_SHADER, points, uvs, normals);
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
			int randomValue = rand() % (int)((TARGET_SPAWNPROB / delta) * ((i + 1) * (i + 1) / 2.0f));
			if(randomValue == 0){
				create_target();
			}
			break;
		}
	}
}

void delete_target(plane * target){
	target->dim = vec2(0.0f);
	delete_object(&target->obj);
}

void update_ball(float delta) {
	ball.vel += GRAVITY * delta;
	checkCollision_ball(delta);

	ball.pos += ball.vel * delta;
	
	if (ball.pos.y <= -5.0f){
		reset_ball();
	}
}

void reset_ball(){
	ball.pos = BALL_SPAWN;
	ball.vel = vec3(0.0f, 0.0f, 0.0f);
	hit = false;
	*comboPointer = 0;
}

bool checkCollision_ball(float delta){
	if(planeObjectCollision(court.obj.pos, court.obj.rot, court.dim, &ball.pos, &ball.vel, delta)){
		return true;
	}

	for(int i = 0; i < sizeof(targets) / sizeof(plane); i++){
		if(targets[i].dim.x != 0.0f){
			if(planeObjectCollision(targets[i].obj.pos, targets[i].obj.rot, targets[i].dim, &ball.pos, &ball.vel, delta)){
				delete_target(&targets[i]);
				*comboPointer += 1;
				*scorePointer += *comboPointer;
				checkCombo();
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

	draw_object(&court.obj, projectionMatrix, viewMatrix, GL_TRIANGLE_STRIP);
	draw_object(&ball, projectionMatrix, viewMatrix, GL_TRIANGLES);
	for(int i = 0; i < sizeof(targets) / sizeof(plane); i++){
		if(targets[i].dim.x != 0.0f && targets[i].dim.y != 0.0f){
			draw_object(&targets[i].obj, projectionMatrix, viewMatrix, GL_TRIANGLE_STRIP);
		}
	}
}

void clean_court() {
	delete_object(&court.obj);
	delete_object(&ball);
	for(int i = 0; i < sizeof(targets) / sizeof(plane); i++){
		delete_object(&targets[i].obj);
	}
}