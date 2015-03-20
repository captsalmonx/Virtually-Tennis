#version 400 core

// Input vertex data, different for all executions of this shader.

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal;

// Output data ; will be interpolated for each fragment.
out vec2 UV;
out vec3 position_eye, normal_eye;

// Values that stay constant for the whole mesh.
uniform mat4 M, V, P;

void main(){
	position_eye = vec3( V * M * vec4(vertexPosition_modelspace, 1.0));
	normal_eye = vec3( V * M * vec4(vertexNormal, 0.0));

	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  P * vec4(position_eye, 1.0);
	
	// UV of the vertex. No special space for this one.
	UV = vertexUV;
}
