/******************************************************************************\
| Mesh MUST be triangulated - quads not accepted                               |
| Mesh MUST contain vertex points, normals, and texture coordinates            |
| Faces MUST come after all other data in the .obj file                        |
\******************************************************************************/
#include "obj_parser.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

bool load_obj_file  (
	const char* file_name,
	float*& points,
	float*& tex_coords,
	float*& normals,
	int& point_count
) {

	float* unsorted_vp_array = NULL;
	float* unsorted_vt_array = NULL;
	float* unsorted_vn_array = NULL;
	int current_unsorted_vp = 0;
	int current_unsorted_vt = 0;
	int current_unsorted_vn = 0;

	FILE* fp = fopen (file_name, "r");
	if (!fp) {
		fprintf (stderr, "ERROR: could not find file %s\n", file_name);
		return false;
	}
	
	// first count points in file so we know how much mem to allocate
	point_count = 0;
	int unsorted_vp_count = 0;
	int unsorted_vt_count = 0;
	int unsorted_vn_count = 0;
	int face_count = 0;
	char line[1024];
	while (fgets (line, 1024, fp)) {
		if (line[0] == 'v') {
			if (line[1] == ' ') {
				unsorted_vp_count++;
			} else if (line[1] == 't') {
				unsorted_vt_count++;
			} else if (line[1] == 'n') {
				unsorted_vn_count++;
			}
		} else if (line[0] == 'f') {
			face_count++;
		}
	}
	printf (
		"found %i vp %i vt %i vn unique in obj. allocating memory...\n",
		unsorted_vp_count, unsorted_vt_count, unsorted_vn_count
	);
	unsorted_vp_array = (float*)malloc (unsorted_vp_count * 3 * sizeof (float));
	unsorted_vt_array = (float*)malloc (unsorted_vt_count * 2 * sizeof (float));
	unsorted_vn_array = (float*)malloc (unsorted_vn_count * 3 * sizeof (float));
	points = (float*)malloc (3 * face_count * 3 * sizeof (float));
	tex_coords = (float*)malloc (3 * face_count * 2 * sizeof (float));
	normals = (float*)malloc (3 * face_count * 3 * sizeof (float));
	printf (
		"allocated %i bytes for mesh\n",
		(int)(3 * face_count * 8 * sizeof (float))
	);
	
	rewind (fp);
	while (fgets (line, 1024, fp)) {
		// vertex
		if (line[0] == 'v') {
		
			// vertex point
			if (line[1] == ' ') {
				float x, y, z;
				x = y = z = 0.0f;
				sscanf (line, "v %f %f %f", &x, &y, &z);
				unsorted_vp_array[current_unsorted_vp * 3] = x;
				unsorted_vp_array[current_unsorted_vp * 3 + 1] = y;
				unsorted_vp_array[current_unsorted_vp * 3 + 2] = z;
				current_unsorted_vp++;
				
			// vertex texture coordinate
			} else if (line[1] == 't') {
				float s, t;
				s = t = 0.0f;
				sscanf (line, "vt %f %f", &s, &t);
				unsorted_vt_array[current_unsorted_vt * 2] = s;
				unsorted_vt_array[current_unsorted_vt * 2 + 1] = t;
				current_unsorted_vt++;
				
			// vertex normal
			} else if (line[1] == 'n') {
				float x, y, z;
				x = y = z = 0.0f;
				sscanf (line, "vn %f %f %f", &x, &y, &z);
				unsorted_vn_array[current_unsorted_vn * 3] = x;
				unsorted_vn_array[current_unsorted_vn * 3 + 1] = y;
				unsorted_vn_array[current_unsorted_vn * 3 + 2] = z;
				current_unsorted_vn++;
			}
			
		// faces
		} else if (line[0] == 'f') {
			// work out if using quads instead of triangles and print a warning
			int slashCount = 0;
			int len = strlen (line);
			for (int i = 0; i < len; i++) {
				if (line[i] == '/') {
					slashCount++;
				}
			}
			if (slashCount != 6) {
				fprintf (
					stderr,
					"ERROR: file contains quads or does not match v vp/vt/vn layout - \
					make sure exported mesh is triangulated and contains vertex points, \
					texture coordinates, and normals\n"
				);
				return false;
			}

			int vp[3], vt[3], vn[3];
			sscanf (
				line,
				"f %i/%i/%i %i/%i/%i %i/%i/%i",
				&vp[0], &vt[0], &vn[0], &vp[1], &vt[1], &vn[1], &vp[2], &vt[2], &vn[2]
			);

			/* start reading points into a buffer. order is -1 because obj starts from
			   1, not 0 */
			// NB: assuming all indices are valid
			for (int i = 0; i < 3; i++) {
				if ((vp[i] - 1 < 0) || (vp[i] - 1 >= unsorted_vp_count)) {
					fprintf (stderr, "ERROR: invalid vertex position index in face\n");
					return false;
				}
				if ((vt[i] - 1 < 0) || (vt[i] - 1 >= unsorted_vt_count)) {
					fprintf (stderr, "ERROR: invalid texture coord index %i in face.\n", vt[i]);
					return false;
				}
				if ((vn[i] - 1 < 0) || (vn[i] - 1 >= unsorted_vn_count)) {
					printf ("ERROR: invalid vertex normal index in face\n");
					return false;
				}
				points[point_count * 3] = unsorted_vp_array[(vp[i] - 1) * 3];
				points[point_count * 3 + 1] = unsorted_vp_array[(vp[i] - 1) * 3 + 1];
				points[point_count * 3 + 2] = unsorted_vp_array[(vp[i] - 1) * 3 + 2];
				tex_coords[point_count * 2] = unsorted_vt_array[(vt[i] - 1) * 2];
				tex_coords[point_count * 2 + 1] = unsorted_vt_array[(vt[i] - 1) * 2 + 1];
				normals[point_count * 3] = unsorted_vn_array[(vn[i] - 1) * 3];
				normals[point_count * 3 + 1] = unsorted_vn_array[(vn[i] - 1) * 3 + 1];
				normals[point_count * 3 + 2] = unsorted_vn_array[(vn[i] - 1) * 3 + 2];
				point_count++;
			}
		}
	}
	fclose (fp);
	free (unsorted_vp_array);
	free (unsorted_vn_array);
	free (unsorted_vt_array);
	printf (
		"allocated %i points\n",
		point_count
	);
	return true;
}

bool create_plane(
	object * pl,
	float*& points, float*& uvs,
	vec2 dim,	vec3 pos,	vec3 rot
){
	pl->pos = pos;
	pl->rot = rot;

	points = (float*)malloc(12 * sizeof(float));
	points[0] = -dim.x / 2.0f;	points[1] = -dim.y / 2.0f;	points[2] = 0.0f;
	points[3] = dim.x / 2.0f;	points[4] = -dim.y / 2.0f;	points[5] = 0.0f;
	points[6] = -dim.x / 2.0f;	points[7] = dim.y / 2.0f;	points[8] = 0.0f;
	points[9] = dim.x / 2.0f;	points[10] = dim.y / 2.0f;	points[11] = 0.0f;

	uvs = (float*)malloc(8 * sizeof(float));
	uvs[0] = 0.0f;	uvs[1] = 0.0f;
	uvs[2] = 1.0f;	uvs[3] = 0.0f;
	uvs[4] = 0.0f;	uvs[5] = 1.0f;
	uvs[6] = 1.0f;	uvs[7] = 1.0f;

	mat4 transMat = translate(mat4(1.0f), pos);
	mat4 rotXMat = rotate(mat4(1.0f), radians(rot.x), vec3(1, 0, 0));
	mat4 rotYMat = rotate(mat4(1.0f), radians(rot.y), vec3(0, 1, 0));
	mat4 rotZMat = rotate(mat4(1.0f), radians(rot.z), vec3(0, 0, 1));

	pl->modelMatrix = transMat * (rotXMat * rotYMat * rotZMat);
	pl->point_count = 4;
	return true;
}

bool bind_object(
	object * obj,
	const char * vertShader,
	const char * fragShader,
	const char * imagePath,
	float*& points,
	float*& uvs,
	float*& normals
){
	GLuint vp_vbo, vu_vbo, vn_vbo;

	glGenBuffers(1, &vp_vbo);
	glGenBuffers(1, &vu_vbo);
	glGenBuffers(1, &vn_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	glBufferData(GL_ARRAY_BUFFER, obj->point_count * sizeof(float) * 3, points, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vu_vbo);
	glBufferData(GL_ARRAY_BUFFER, obj->point_count * sizeof(float) * 2, uvs, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
	glBufferData(GL_ARRAY_BUFFER, obj->point_count * sizeof(float) * 3, normals, GL_STATIC_DRAW);

	glGenVertexArrays(1, &obj->vao);
	glBindVertexArray(obj->vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vu_vbo);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	obj->sp = link_programme_from_files( vertShader, fragShader );
	obj->textureID = create_texture_from_file ( imagePath );
	obj->mvp = glGetUniformLocation(obj->sp, "MVP");

	free(points);
	free(uvs);
	free(normals);
	return true;
}

bool draw_object(
	object * obj,
	const mat4 projectionMatrix, const mat4 viewMatrix,
	GLenum drawMode
){
	glUseProgram(obj->sp);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, obj->textureID);
	glBindVertexArray(obj->vao);
	mat4 MVP = projectionMatrix * viewMatrix * obj->modelMatrix;
	glUniformMatrix4fv(obj->mvp, 1, GL_FALSE, &MVP[0][0]);
	glDrawArrays(drawMode, 0, obj->point_count);
	return true;
}