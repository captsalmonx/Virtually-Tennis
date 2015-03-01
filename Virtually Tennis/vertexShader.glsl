#version 400 core

// Input vertex data, different for all executions of this shader.
// 'layout' uses a qualifier 'location', defining where the storage for a variable comes from
// Attributes that take up multiple attribute slots will be given a sequential block of that number of attributes
// in order starting with the given attribute
// 'location = 0' specifies the attribut index '0'
layout(location = 0) in vec3 vertexPosition_modelspace;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;

void main(){

	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  MVP * vec4(vertexPosition_modelspace,1);

}

