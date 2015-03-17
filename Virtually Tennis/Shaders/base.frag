#version 400 core

// Interpolated values from the vertex shaders
in vec2 UV;

// Ouput data
out vec3 color;

// Values that stay constant for the whole mesh.
uniform sampler2D texture_map;

void main(){

	// Output color = color of the texture at the specified UV
	color = texture2D( texture_map, UV ).rgb;
}