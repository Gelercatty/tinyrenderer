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

            _vertices.push_back(v);
        }
        else if (type == "f") {
            Face f;
            std::string temp;
            while (iss >> temp) {
                std::vector<std::string> parts = split(temp, '/');
                if (!parts.empty()) {
                    f.v_idx.push_back(std::stoi(parts[0]) - 1); // OBJ indices are 1-based
                }
            }
            _faces.push_back(f);
        }
    }
    if (!_faces.empty()) {
        std::cout << "load face done, face size: " << _faces.size() << std::endl;
        updateBounds();
    }
    if (!_vertices.empty()) {
        std::cout << "load vertex done, vertex size: " << _vertices.size() << std::endl;
    }
}



void Mesh::updateBounds() {
    auto [min_it_x, max_it_x] = std::minmax_element(_vertices.begin(), _vertices.end(),
        [](const vec3& a, const vec3& b) { return a.x < b.x; });
    auto [min_it_y, max_it_y] = std::minmax_element(_vertices.begin(), _vertices.end(),
        [](const vec3& a, const vec3& b) { return a.y < b.y; });
    auto [min_it_z, max_it_z] = std::minmax_element(_vertices.begin(), _vertices.end(),
        [](const vec3& a, const vec3& b) { return a.z < b.z; });
    min_x = min_it_x->x;
    max_x = max_it_x->x;
    min_y = min_it_y->y;
    max_y = max_it_y->y;
    min_z = min_it_z->z;
    max_z = max_it_z->z;
    return;
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
