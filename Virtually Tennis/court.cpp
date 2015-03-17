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
#include "player.h"
#include "court.h"

#define BALL_MESH "Assets/ball.obj"
#define FLOOR_TEXTURE "Assets/tennis-court.png"

// Court dimensions, 36 feet by 78 feet
const vec2 court_dimensions = vec2(36.0f, 78.0f);

plane court_floor;
tennis_ball ball;

GLuint floor_vao, ball_vao;
GLuint floor_sp, ball_sp;
GLuint floor_mvp, ball_mvp;

bool init_court() {
	float* points = NULL;
	float* uvs = NULL;
	float* normals = NULL;
	GLuint vp_vbo, vu_vbo;

	// CREATE FLOOR
	createPlane(&court_floor, 
		points,
		uvs,
		court_dimensions, 
		vec3(0.0f, 0.0f, 0.0f),
		vec3(-90.0f, 0.0f, 0.0f),
		FLOOR_TEXTURE); 

	glGenBuffers(1, &vp_vbo);
	glGenBuffers(1, &vu_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	glBufferData(GL_ARRAY_BUFFER, court_floor.point_count * sizeof(float) * 3, points, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vu_vbo);
	glBufferData(GL_ARRAY_BUFFER, court_floor.point_count * sizeof(float) * 2, uvs, GL_STATIC_DRAW);

	glGenVertexArrays(1, &floor_vao);
	glBindVertexArray(floor_vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vu_vbo);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	floor_sp = link_programme_from_files( "base.vert", "base.frag" );
	floor_mvp = glGetUniformLocation(floor_sp, "MVP");

	// CREATE BALL
	//if(!load_obj_file(
	//	BALL_MESH,
	//	points,
	//	uvs,
	//	normals,
	//	ball.point_count)){
	//		fprintf(stderr, "Error loading ball mesh\n");
	//		return false;
	//}

	//glGenBuffers(1, &vp_vbo);
	//glGenBuffers(1, &vu_vbo);
	//glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
	//glBindBuffer(GL_ARRAY_BUFFER, vu_vbo);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(uvs), uvs, GL_STATIC_DRAW);

	//free(points);
	//free(uvs);

	return true;
}

void draw_court() {
	// Use our shader
	mat4 projectionMatrix = getProjectionMatrix();
	mat4 viewMatrix = getViewMatrix();

	// Draw floor
	glUseProgram(floor_sp);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, court_floor.textureID);
	glBindVertexArray(floor_vao);

	mat4 MVP = projectionMatrix * viewMatrix * court_floor.model_matrix;
	glUniformMatrix4fv(floor_mvp, 1, GL_FALSE, &MVP[0][0]);

	// Draw the triangle !
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void clean_court() {
	glDeleteProgram(floor_sp);
	glDeleteVertexArrays(1, &floor_vao);
}