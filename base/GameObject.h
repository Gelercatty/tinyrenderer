#pragma once
#include "Transform.h"
#include "../utils/geometry.h"
#include "../base/Mesh.h"


class GameObject
{
public:
	Transform transform; // transform in world space
	Mesh mesh; // mesh of the object
	
};