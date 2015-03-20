#version 400 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 normal_eye, position_eye;

// Ouput data
out vec4 colour;

uniform mat4 V;

// Values that stay constant for the whole mesh.
uniform sampler2D texture_map;

vec3 light_position_world = vec3(0.0, 30.0, 40.0);
vec3 Ls = vec3 (1.0, 1.0, 0.7);
vec3 Ld = vec3 (0.8, 0.8, 0.8);
vec3 La = vec3 (1.0, 1.0, 0.9);

vec3 Ks = vec3 (0.9, 0.9, 0.9);

float specular_exponent = 50.0;

void main(){
	// Output color = color of the texture at the specified UV

	// AMBIENT
	vec3 Ia = La * texture2D( texture_map, UV ).rgb;

	// DIFFUSE
	vec3 Kd = texture2D( texture_map, UV ).rgb;
	vec3 light_position_eye = vec3 (V * vec4(light_position_world, 1.0));
	vec3 distance_to_light_eye = light_position_eye - position_eye;
	vec3 direction_to_light_eye = normalize(distance_to_light_eye);
	float dot_prod = dot(direction_to_light_eye, normal_eye);
	dot_prod = max(dot_prod, 0.0);
	vec3 Id = Ld * Kd * dot_prod;

	// SPECULAR
	vec3 surface_to_viewer_eye = normalize(-position_eye);
	vec3 reflection_eye = reflect(-direction_to_light_eye, normal_eye);
	float dot_prod_specular = dot(reflection_eye, surface_to_viewer_eye);
	dot_prod_specular = max(dot_prod_specular, 0.0);
	float specular_factor = pow(dot_prod_specular, specular_exponent);
	vec3 Is = Ls * Ks * specular_factor;

	// FINAL COLOUR
	colour = vec4(Is + Id + Ia, 1.0);
}