/******************************************************************************\                                                         |
| Mesh MUST be triangulated - quads not accepted                               |
| Mesh MUST contain vertex points, normals, and texture coordinates            |
| Faces MUST come after all other data in the .obj file                        |
\******************************************************************************/

#ifndef _OBJ_PARSER_H_
#define _OBJ_PARSER_H_

bool load_obj_file (
	const char* file_name,
	float*& points,
	float*& tex_coords,
	float*& normals,
	int& point_count
	);

bool createPlane(
	GLfloat points[],
	GLfloat uvs[],
	mat4 * model_matrix,
	float width, 
	float height, 
	float x, 
	float y, 
	float z, 
	float pitch, 
	float yaw, 
	float roll
	);

#endif
