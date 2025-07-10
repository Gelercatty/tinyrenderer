#pragma once
#include "../utils/geometry.h"
class Transform
{
public:
	vec3 position;
	vec3 rotation;
	vec3 scale = { 1, 1, 1 };

	mat<4, 4> ModelMatrix() const;

	vec3 forward() const;
	vec3 up()      const;
	vec3 right()   const;
	void lookAt(const vec3& target, const vec3& worldup = {0,1,0});
};

