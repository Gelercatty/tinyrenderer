#pragma once
#include "base/Transform.h"
#include "utils/myMath.h"

enum class ProjectionType {
	ORTHOGRAPHIC, // Orthographic projection ����ͶӰ
	PERSPECTIVE   // Perspective projection ͸��ͶӰ
};
class Camera
{
public:
	int SensorWidth;
	int SensorHeight;
	Transform transform; // Camera's position and rotation
	ProjectionType projection_type = ProjectionType::ORTHOGRAPHIC; // Projection type
	
	void renderOneFrame();

private:
	std::tuple<int, int, int> project(vec3 v);
};

