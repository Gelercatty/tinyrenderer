#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm> 
#include <random>
#include <numbers>
#include "../utils/geometry.h"
#include "../utils//tgaimage.h"

struct Face
{
    std::vector<int> v_idx;
};
std::vector<std::string> split(const std::string& str, char delimiter);

class Mesh {
private:
    std::vector<vec3> verts = {};
    std::vector<vec2> tex = {};  // 每个顶点的纹理坐标
	std::vector<vec3> norms = {}; // 每个顶点的法线

    std::vector<int> facet_vrt = {};
    std::vector<int> facet_nrm = {};
    std::vector<int> facet_tex = {};
    TGAImage diffusemap = {};
    TGAImage normalmap = {};
    TGAImage specularmap = {};

public:
    bool load_from_obj(const std::string& filename);

    int nverts() const;
	int nfaces() const;

	vec3 vert(const int i) const; // return the i-th vertex
    vec3 vert(const int iface, const int nthvert) const; // return the n-th  vert of the face
	vec3 normal(const int iface, const int nthvert) const; // return the n-th normal of the face
	vec3 normal(const vec2& uvf) const; // return the normal at the given texture coordinate
	vec2 uv(const int iface, const int nthvert) const; // return the n-th texture coordinate of the face
    
    const TGAImage& diffuse() const;
    const TGAImage& specular() const;



	bool load_diffuse(std::string filename) {
		return diffusemap.read_tga_file(filename.c_str());
	};
	bool load_normal(std::string filename) {
		return normalmap.read_tga_file(filename.c_str());
	};
	bool load_specular(std::string filename) {
		return specularmap.read_tga_file(filename.c_str());
	};
};
