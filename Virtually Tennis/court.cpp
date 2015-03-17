#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <GL\glew.h>
#include <GLFW\glfw3.h>

// Include GLM (OpenGL Mathematics)
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
using namespace glm; // Save having to type glm:: everywhere

#include "gl_utils.h"
#include "obj_parser.h"
#include "player.h"
#include "court.h"

#define VERT_SHADER "Shaders/base.vert"
#define FRAG_SHADER "Shaders/base.frag"
#define BALL_MESH "Assets/ball.obj"
#define BALL_TEXTURE "Assets/ball.jpg"
#define FLOOR_TEXTURE "Assets/tennis-court.png"

// Court dimensions, 36 feet by 78 feet
const vec2 court_dimensions = vec2(36.0f, 78.0f);
const vec3 ball_spawn = vec3(-2.0f, 5.0f, 27.0f);
const vec3 gravity = vec3(0.0f, -32.0f, 0.0f);

object court;
dynamicObject ball;

bool init_court() {
	float* points = NULL;
	float* uvs = NULL;
	float* normals = NULL;

	setSwingFunction(&swingCheck);

	// CREATE FLOOR
	if(!create_plane(&court, points, uvs, court_dimensions, vec3(0.0f, 0.0f, 0.0f),	vec3(-90.0f, 0.0f, 0.0f))){
		fprintf(stderr, "Error loading court\n");
		return false;
	}
	bind_object(&court, VERT_SHADER, FRAG_SHADER, FLOOR_TEXTURE, points, uvs, normals);

	// CREATE BALL
	if(!load_obj_file(BALL_MESH, points, uvs, normals,	ball.obj.point_count)){
		fprintf(stderr, "Error loading ball mesh\n");
		return false;
	}
	bind_object(&ball.obj, VERT_SHADER, FRAG_SHADER, BALL_TEXTURE, points, uvs, normals);
	ball.obj.pos = ball_spawn;
	return true;
}

void update_ball(float delta) {
	// Add gravity
	ball.vel += gravity * delta;
	ball.obj.pos += ball.vel * delta;
	checkCollision_ball();
}

bool checkCollision_ball(){
	bool collision = false;
	if(ball.obj.pos.y < 0.0)
	{
		ball.obj.pos.y = -ball.obj.pos.y;
		ball.vel.y = -ball.vel.y * 0.95f;
		collision = true;
	}

	return collision;
}

void swingCheck(float power)
{
	
}

void draw_court() {
	// Use our shader
	mat4 projectionMatrix = getProjectionMatrix();
	mat4 viewMatrix = getViewMatrix();

	draw_object(&court, projectionMatrix, viewMatrix, GL_TRIANGLE_STRIP);
	draw_object(&ball.obj, projectionMatrix, viewMatrix, GL_TRIANGLES);
}

void clean_court() {
	delete_object(&court);
	delete_object(&ball.obj);
}