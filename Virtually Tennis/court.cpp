#include "court.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <GL\glew.h>
#include <GLFW\glfw3.h>

// Include GLM (OpenGL Mathematics)
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
using namespace glm; // Save having to type glm:: everywhere

#include "loadShader.h"
#include "player.h"
#include "loadBMPTexture.h"
#include "obj_parser.h"

mat4 modelMatrix;
GLuint programID, vertexArrayID, matrixID;
GLuint vertexbuffer, uvbuffer;

GLuint floor_texture, floor_textureID;
GLfloat floor_vertex_buffer_data[12], floor_uv_buffer_data[8];

bool init_court() {
	createPlane(floor_vertex_buffer_data, floor_uv_buffer_data, &modelMatrix, 
		36.0f, 78.0f, 
		0.0f, 0.0f, 0.0f, 
		-90.0f, 0.0f, 0.0f); 

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	// Create and compile our GLSL program from the shaders
	programID = LoadShaders( "court_vert.glsl", "court_frag.glsl" );

	matrixID = glGetUniformLocation(programID, "MVP");

	// Load the texture and get a handle for the uniform variable
	floor_texture = loadBMP_custom("Assets/tennis-court.bmp");
	floor_textureID = glGetUniformLocation(programID, "myTextureSampler");

	// Init vertex buffer and array
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floor_vertex_buffer_data), floor_vertex_buffer_data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floor_uv_buffer_data), floor_uv_buffer_data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, floor_texture);
	glUniform1i(floor_textureID, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	return true;
}

void draw_court() {
	// Use our shader
	glUseProgram(programID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

	mat4 projectionMatrix = getProjectionMatrix();
	mat4 viewMatrix = getViewMatrix();
	mat4 MVP = projectionMatrix * viewMatrix * modelMatrix;

	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform
	glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MVP[0][0]);

	// 1st attribute buffer : vertices
	glEnableVertexAttribArray(0);

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);

	// Draw the triangle !
	glDrawArrays(GL_TRIANGLE_STRIP, 0, sizeof(floor_vertex_buffer_data) / 12);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void clean_court() {
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteProgram(programID);
	glDeleteTextures(1, &floor_textureID);
	glDeleteVertexArrays(1, &vertexArrayID);
}