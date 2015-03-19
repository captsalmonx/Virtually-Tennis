#include "collision.h"
#include "obj_parser.h"

#include <stdio.h>

bool planeObjectCollision(vec3 pPosition, vec3 pRotation, vec2 pDimensions, 
						  vec3 * oPosition, vec3 * oVelocity, float delta)
{
	bool collided = false;
	mat4 rotMatrix = getRotationMatrix(pRotation);
	vec3 pNormal = (vec3)(rotMatrix * vec4(0, 0, 1, 1));
	
	vec3 oDirection = normalize(*oVelocity);
	float oMagnitude = length(*oVelocity * delta);

	float collDistance = dot(pNormal, (pPosition - *oPosition)) / dot(pNormal, oDirection);
	
	if(collDistance <= oMagnitude && collDistance >= 0.0f){
		vec3 hitPosition = *oPosition + (oDirection * collDistance);
		vec3 unrotHitPos = (vec3)(-rotMatrix * vec4(hitPosition, 1));

		if(unrotHitPos.x <= pPosition.x + (pDimensions.x / 2) && unrotHitPos.x >= pPosition.x - (pDimensions.x / 2) && 
		unrotHitPos.y <= pPosition.y + (pDimensions.y / 2) && unrotHitPos.y >= pPosition.y - (pDimensions.y / 2)){
			*oVelocity = reflectVelocity(*oVelocity, pNormal);
			*oPosition = hitPosition + (normalize(*oVelocity) * collDistance);
			collided = true;
		}
	}
	
	return collided;
}

vec3 reflectVelocity(vec3 velocity, vec3 normal)
{
	return velocity - (2.0f * dot(velocity, normal) * normal);
}