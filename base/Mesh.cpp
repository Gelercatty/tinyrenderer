#pragma once
#include "Mesh.h"

bool Mesh::load_from_obj(const std::string& filename) {
    std::ifstream in(filename);
    if (!in) {
        std::cerr << "Cannot open " << filename << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(in, line)) {
        std::istringstream iss(line);
        std::string type;
        iss >> type;
        if (type == "v") {
            vec3 v;
            iss >> v.x >> v.y >> v.z;
            verts.push_back(v);
        }
        if (type == "vt") {
            vec2 vt;
			iss >> vt.x >> vt.y;
        }
        if (type == "vn") {
			vec3 vn;
			iss >> vn.x >> vn.y >> vn.z;
        }
        else if (type == "f") {
            std::string temp;
            while (iss >> temp) {
                std::vector<std::string> parts = split(temp, '/');
                if (!parts.empty()) {
					facet_vrt.push_back(std::stoi(parts[0]) - 1); // OBJ indices are 1-based
					facet_tex.push_back(std::stoi(parts[1]) - 1); // OBJ indices are 1-based
					facet_nrm.push_back(std::stoi(parts[2]) - 1); // OBJ indices are 1-based
                }
            }
        }
    }
    std::cout << "load vertex done, vertex size: " << nverts() << std::endl;
    auto load_texture = [&filename](const std::string suffix, TGAImage& img) {
		size_t dot = filename.find_last_of('.');
        if (dot == std::string::npos) return;
		std::string texfile = filename.substr(0, dot) + suffix;
		std::cerr << "load texture: " << texfile << std::endl;
    };

	load_texture("_diffuse.tga", diffusemap);
	load_texture("_nm_tangent.tga", normalmap);
	load_texture("_spec.tga", specularmap);

}




std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream tokenStream(str);
    std::string token;
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

int Mesh::nverts() const
{
    return verts.size();
}

int Mesh::nfaces() const
{
	return facet_vrt.size() / 3; // 每个面有3个顶点
}

vec3 Mesh::vert(const int i) const
{
	return verts[i];
}

vec3 Mesh::vert(const int iface, const int nthvert) const
{
    return verts[facet_vrt[iface * 3 + nthvert]];
}

vec3 Mesh::normal(const int iface, const int nthvert) const
{
	return norms[facet_nrm[iface * 3 + nthvert]];
}

vec3 Mesh::normal(const vec2& uvf) const
{
	TGAColor c = normalmap.get(uvf[0] * normalmap.width(), uvf[1] * normalmap.height());
    return vec3{ (double)c[2], (double)c[1], (double)c[0] } *2. / 255. - vec3{1,1,1};
}/// ????

vec2 Mesh::uv(const int iface, const int nthvert) const
{
	return tex[facet_tex[iface * 3 + nthvert]];
}

const TGAImage& Mesh::diffuse() const
{
    // TODO: insert return statement here
	return diffusemap;
}

const TGAImage& Mesh::specular() const
{
    // TODO: insert return statement here
	return specularmap;
}
