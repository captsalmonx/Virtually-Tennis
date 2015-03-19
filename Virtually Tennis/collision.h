#ifndef COLLISION_H
#define COLLISION_H

// Include GLM (OpenGL Mathematics)
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
using namespace glm; // Save having to type glm:: everywhere

bool planeObjectCollision(vec3 pPosition, vec3 pRotation, vec2 pDimensions, 
						  vec3 * oPosition, vec3 * oVelocity, float delta);
vec3 reflectVelocity(vec3 velocity, vec3 normal);

#endif