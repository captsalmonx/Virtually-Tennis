#include "collision.h"
#include "obj_parser.h"

#include <stdio.h>

bool planeObjectCollision(vec3 pPosition, vec3 pRotation, vec2 pDimensions, 
						  vec3 * oPosition, vec3 * oVelocity, float delta)
{
	mat4 rotMatrix = getRotationMatrix(pRotation);
	vec3 pNormal = (vec3)(rotMatrix * vec4(0, 0, 1, 1));

	vec3 oUnrotPosition = (vec3)(rotMatrix * vec4(*oPosition, 1));

	//printf("%f, %f, %f\n",  (vec3)(*oPosition).x, (vec3)(*oPosition).y, (vec3)(*oPosition).z);
	//printf("%f, %f, %f\n", oUnrotPosition.x, oUnrotPosition.y, oUnrotPosition.z);

	vec3 oDirection = normalize(*oVelocity * delta);
	float oMagnitude = length(*oVelocity * delta);

	float collDistance = dot(pNormal, (pPosition - *oPosition)) / dot(pNormal, oDirection);

	if(collDistance < oMagnitude){
		vec3 hitPosition = *oPosition + (oDirection * collDistance);
		vec3 unrotHitPosition = (vec3)(-rotMatrix * vec4(hitPosition, 1));

		//*oPosition = hitPosition + 
	}



	//printf("%f - %f\n", collDistance, oMagnitude);
	return collDistance < oMagnitude;
}