/******************************************************************************\                                                         |
| Mesh MUST be triangulated - quads not accepted                               |
| Mesh MUST contain vertex points, normals, and texture coordinates            |
| Faces MUST come after all other data in the .obj file                        |
\******************************************************************************/

#ifndef _OBJ_PARSER_H_
#define _OBJ_PARSER_H_

#include "gl_utils.h"

struct object {
	mat4 modelMatrix;
	int point_count;
	GLuint vao, sp, mvp, textureID;
	vec3 pos, rot;
};

struct dynamicObject {
	object obj;
	vec3 dir, vel;
};

bool create_plane(object * planePointer, 
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
	object * objPointer,
	mat4 projectionMatrix, mat4 viewMatrix,
	GLenum drawMode
	);

#endif
