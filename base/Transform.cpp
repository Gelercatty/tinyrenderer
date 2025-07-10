#include "Transform.h"

mat<4, 4> Transform::ModelMatrix() const {
	mat<4, 4> S = { {
		{ scale.x, 0, 0, 0 },
		{ 0, scale.y, 0, 0 },
		{ 0, 0, scale.z, 0 },
		{ 0, 0, 0, 1 }
	} }; // 缩放矩阵

	mat<4, 4> R = rot2Eular_xyz(rotation); // 旋转矩阵

	mat<4, 4> T = { {
		{1, 0, 0, position.x },
		{ 0, 1, 0, position.y },
		{ 0, 0, 1, position.z },
		{ 0, 0, 0, 1 }
	    }}; // 平移矩阵

	return T * R * S; // 模型矩阵 = 平移矩阵 * 旋转矩阵 * 缩放矩阵
}

vec3 Transform::forward() const
{
	mat<4, 4> R = rot2Eular_xyz(rotation);
	return vec3{ -R[0][2], -R[1][2], -R[2][2] };
}

vec3 Transform::up() const
{
	mat<4, 4> R = rot2Eular_xyz(rotation);
	return vec3{ R[0][1], R[1][1], R[2][1] }; // 第1列
}

vec3 Transform::right() const
{
    mat<4, 4> R = rot2Eular_xyz(rotation);
	return vec3{ R[0][0], R[1][0], R[2][0] }; // 第0列
}

void Transform::lookAt(const vec3& target, const vec3& worldup)
{
	vec3 forwar = normalized(target - position);
	vec3 right = normalized(cross(forwar, worldup));
	vec3 up = normalized(cross(right, forwar));
	mat<4, 4> R = { {
		{ right.x, up.x, -forwar.x, 0 },
		{ right.y, up.y, -forwar.y, 0 },
		{ right.z, up.z, -forwar.z, 0 },
		{ 0, 0, 0, 1 }
	} };
	rotation = Eular_xyz_from_rot(R); // 将旋转矩阵转换为欧拉角
}
