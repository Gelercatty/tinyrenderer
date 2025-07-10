#include "Camera.h"
#include <numbers>
mat<4, 4> Camera::ViewMatrix() const
{
    return transform.ModelMatrix().invert();
}

mat<4, 4> Camera::ProjectMatrix() const
{
	if (projection_type == ProjectionType::ORTHOGRAPHIC) {
		return OthographicProjectionMatrix();
	}
	else {
		return PerspectiveMatrix();
	}
}

mat<4, 4> Camera::OthographicProjectionMatrix() const
{
	float vie_height = view_width / aspect_ratio;

	float l = -view_width / 2.0f;
	float r = view_width / 2.0f;
	float t = vie_height / 2.0f;
	float b = -vie_height / 2.0f;
	float n = near_plane;
	float f = far_plane;
	return { {
	{2.0f / (r - l),           0.0f,                 0.0f,              -(r + l) / (r - l)},
	{0.0f,           2.0f / (t - b),                 0.0f,              -(t + b) / (t - b)},
	{0.0f,                    0.0f,        -2.0f / (f - n),              -(f + n) / (f - n)},
	{0.0f,                    0.0f,                 0.0f,                            1.0f}
} };
}

mat<4, 4> Camera::PerspectiveMatrix() const
{
	float fov_rad = fov * std::numbers::pi_v<float> / 180.f; // ½Ç¶È×ª»¡¶È
	float tan_half_fov = std::tan(fov_rad / 2.0f);

	float n = near_plane;
	float f = far_plane;

	float a = aspect_ratio;

	return { {
		{1.0f / (a * tan_half_fov), 0, 0, 0},
		{0, 1.0f / tan_half_fov, 0, 0},
		{0, 0, -(f + n) / (f - n), -2.0f * f * n / (f - n)},
		{0, 0, -1.0f, 0}
	} };
}

