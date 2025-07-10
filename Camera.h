#pragma once
#include "base/Transform.h"
#include "utils/geometry.h"
#include <cmath>
#include <numbers>

enum class ProjectionType {
	ORTHOGRAPHIC, // Orthographic projection 
	PERSPECTIVE   // 
};
class Camera
{
public:
	Transform transform; // 相机位置
	ProjectionType projection_type = ProjectionType::PERSPECTIVE; // 投影类型
	
	
	float fov = 60.f;
	float near_plane = 0.1f; // Near plane distance
	float far_plane = 100.f; // Far plane distance
	float view_width = 2.0f; // 视口宽度
	float aspect_ratio = 1.0f; // 屏幕宽高比
	
	mat<4, 4> ViewMatrix() const; // 视图矩阵
	mat<4, 4> ProjectMatrix() const; // 投影矩阵，只是为了将可视范围的空间缩放到[-1, 1]^3  (所谓的NDC立方体)，用于之后的裁切
private: 
	// 正交投影矩阵
	mat<4, 4> OthographicProjectionMatrix() const; // 正交投影矩阵

	// 透视投影矩阵
	mat<4, 4> PerspectiveMatrix() const;
};

