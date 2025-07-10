#pragma once 
#include "geometry.h"

vec3 barycentric(const vec2& a, const vec2& b, const vec2& c, const vec2& p) {
	double area = (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
	if (std::abs(area) < 1e-5) return { -1, -1, -1 }; // Area is too small, return invalid barycentric coordinates
	double alpha = ((b.x - p.x) * (c.y - p.y) - (b.y - p.y) * (c.x - p.x)) / area;
	double beta = ((c.x - p.x) * (a.y - p.y) - (c.y - p.y) * (a.x - p.x)) / area;
	double gamma = 1.0 - alpha - beta;

	return { alpha, beta, gamma };

}
vec3 barycentric(int ax, int ay, int bx, int by, int cx, int cy, int px, int py) {
	float denom = (float)((bx - ax) * (cy - ay) - (cx - ax) * (by - ay));
	if (std::abs(denom) < 1e-5f) return { -1, -1, -1 };

	float alpha = (float)((bx - px) * (cy - py) - (cx - px) * (by - py)) / denom;
	float beta = (float)((cx - px) * (ay - py) - (ax - px) * (cy - py)) / denom;
	float gamma = 1.0f - alpha - beta;

	return { alpha, beta, gamma };
}
