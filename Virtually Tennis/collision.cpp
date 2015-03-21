#include "collision.h"
#include "obj_parser.h"

#include <stdio.h>

bool planeObjectCollision(vec3 pPosition, vec3 pRotation, vec2 pDimensions, 
						  vec3 * oPosition, vec3 * oVelocity, float delta)
{
	bool collided = false;
	mat4 rotMatrix = getRotationMatrix(pRotation);
	vec3 pNormal = (vec3)(rotMatrix * vec4(0, 0, 1, 1));
	
	// Break the velocity into its two components
	vec3 oDirection = normalize(*oVelocity);
	float oMagnitude = length(*oVelocity * delta);

	// Calculate the distance until a collision with an infinite plane
	float collDistance = dot(pNormal, (pPosition - *oPosition)) / dot(pNormal, oDirection);
	
	// A collision distance of 0 means the velocity is parallel to the plane, and thus will never collide
	// A negative collision distance means that a collision will only happen with the opposity velocity
	// If the distance to collision is less than the magnitude of the current velocity, then the object
	// has collided in this frame
	if(collDistance <= oMagnitude && collDistance > 0.0f){
		// The hit position can be calculated by using the existing velocity direction
		vec3 hitPosition = *oPosition + (oDirection * collDistance);

		// Unrotate both the plane and the object to a uniform x, y coordinate system to check dimensions
		vec3 unrotHitPos = (vec3)(-rotMatrix * vec4(hitPosition, 1));
		vec3 unrotPlPos = (vec3)(-rotMatrix * vec4(pPosition, 1));

		// Check if the collision position falls within the dimensions of the plane object
		if(unrotHitPos.x <= unrotPlPos.x + (pDimensions.x / 2) && unrotHitPos.x >= unrotPlPos.x - (pDimensions.x / 2) && 
		unrotHitPos.y <= unrotPlPos.y + (pDimensions.y / 2) && unrotHitPos.y >= unrotPlPos.y - (pDimensions.y / 2)){
			// If so, reflect the velocity against the normal
			*oVelocity = reflectVelocity(*oVelocity, pNormal);

			// Update the position to fulfil the remaining 'magnitude' from this frame's action
			*oPosition = hitPosition + (normalize(*oVelocity) * collDistance);
			collided = true;
		}
	}
	
	return collided;
}

vec3 reflectVelocity(vec3 velocity, vec3 normal)
{
	// Descartes’s Law 
	return velocity - (2.0f * dot(velocity, normal) * normal);
}

