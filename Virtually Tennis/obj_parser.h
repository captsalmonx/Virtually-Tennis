/******************************************************************************\                                                         |
| Mesh MUST be triangulated - quads not accepted                               |
| Mesh MUST contain vertex points, normals, and texture coordinates            |
| Faces MUST come after all other data in the .obj file                        |
\******************************************************************************/

#ifndef _OBJ_PARSER_H_
#define _OBJ_PARSER_H_

#include "gl_utils.h"
#include <GLFW\glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm; // Allow us to not have to type glm::

struct object {
	int point_count;
	GLuint vao, sp, mvp, textureID;
	vec3 pos, rot;
};

struct plane {
	object obj;
	vec2 dim;
};

struct dynamicObject {
	object obj;
	vec3 vel;
};

bool create_plane(plane * planePointer, 
	float*& points, float*& uvs,
	vec2 dim, vec3 pos, vec3 rot
	);

bool bind_object(
	object * obj,
	const char * vertShader,
	const char * fragShader,
	const char * imagePath,
	float*& points,
	float*& uvs,
	float*& normals
	);

bool load_obj_file (
	const char* file_name,
	float*& points,
	float*& tex_coords,
	float*& normals,
	int& point_count
	);

bool draw_object (
	object * obj,
	mat4 projectionMatrix, mat4 viewMatrix,
	GLenum drawMode
	);

mat4 getRotationMatrix(vec3 rot);

bool delete_object (
	object * obj
	);

#endif
