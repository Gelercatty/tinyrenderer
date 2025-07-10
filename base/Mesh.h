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


struct Face
{
    std::vector<int> v_idx;
};
std::vector<std::string> split(const std::string& str, char delimiter);

class Mesh {
private:
    std::vector<vec3> _vertices;
    std::vector<Face> _faces;
    void updateBounds();
public:
    float size = 1;
    // max and min bounds for the mesh
    float max_x, min_x;
    float max_y, min_y;
    float max_z, min_z;

    bool clear() {
        _vertices.clear();
        _faces.clear();
        return true;
    }
    bool load_from_obj(const std::string& filename);

    const std::vector<vec3>& vertices() const { 
        return _vertices; 
    }
    const std::vector<Face>& faces() const { 
        return _faces;
    }
    //void drawPreview_line(TGAImage& buffer) {
    //    int width = buffer.width();
    //    int height = buffer.height();

    //    for (Face f : faces) {
    //        for (int i = 0; i < f.v_idx.size(); i++) {
    //            int idx1 = f.v_idx[i];
    //            int idx2 = f.v_idx[(i + 1) % f.v_idx.size()]; // wrap around to the first vertex

    //            int	x1_x = static_cast<int>((vertices[idx1].x + 1) * width / 2);
    //            int x1_y = static_cast<int>((vertices[idx1].y + 1) * height / 2);
    //            int x2_x = static_cast<int>((vertices[idx2].x + 1) * width / 2);
    //            int x2_y = static_cast<int>((vertices[idx2].y + 1) * height / 2);

    //            if (x1_x >= 0 && x1_x < width && x1_y >= 0 && x1_y < height &&
    //                x2_x >= 0 && x2_x < width && x2_y >= 0 && x2_y < height) {
    //                line(x1_x, x1_y, x2_x, x2_y, buffer, red);
    //            }
    //        }
    //    }
    //}
    //void drawPreview_triangle(TGAImage& buffer, TGAImage& zbuffer) {
    //    int width = buffer.width();
    //    int height = buffer.height();
    //    // Clear the buffer  
    //    for (int x = 0; x < width; ++x) {
    //        for (int y = 0; y < height; ++y) {
    //            zbuffer.set(x, y, { 0,0,0 }); // ��Զ
    //        }
    //    }
    //    for (Face f : faces) {
    //        TGAColor colorchoice = randomPredefinedColor();

    //        // fake objection
    //        vec3 v0 = persp(vertices[f.v_idx[0]]);
    //        vec3 v1 = persp(vertices[f.v_idx[1]]);
    //        vec3 v2 = persp(vertices[f.v_idx[2]]);
    //        //vec3 v0 = vertices[f.v_idx[0]];
    //        //vec3 v1 = vertices[f.v_idx[1]];
    //        //vec3 v2 = vertices[f.v_idx[2]];
    //        triangle(
    //            static_cast<int>((v0.x + 1) * width / 2),
    //            static_cast<int>((v0.y + 1) * height / 2),
    //            static_cast<int>((v0.z + 1) * 255 / 2), // Assuming z is in range [0, 1]
    //            static_cast<int>((v1.x + 1) * width / 2),
    //            static_cast<int>((v1.y + 1) * height / 2),
    //            static_cast<int>((v1.z + 1) * 255 / 2), // Assuming z is in range [0, 1]
    //            static_cast<int>((v2.x + 1) * width / 2),
    //            static_cast<int>((v2.y + 1) * height / 2),
    //            static_cast<int>((v2.z + 1) * 255 / 2), // Assuming z is in range [0, 1]
    //            buffer, zbuffer, colorchoice);


    //        //triangle(
    //        //	static_cast<int>((vertices[f.v_idx[0]].x + 1) * width / 2),
    //        //	static_cast<int>((vertices[f.v_idx[0]].y + 1) * height / 2),
    //        //	static_cast<int>((vertices[f.v_idx[0]].z + 1) * 255 / 2), // Assuming z is in range [0, 1]
    //        //	static_cast<int>((vertices[f.v_idx[1]].x + 1) * width / 2),
    //        //	static_cast<int>((vertices[f.v_idx[1]].y + 1) * height / 2),
    //        //	static_cast<int>((vertices[f.v_idx[1]].z + 1) * 255 / 2), // Assuming z is in range [0, 1]
    //        //	static_cast<int>((vertices[f.v_idx[2]].x + 1) * width / 2),
    //        //	static_cast<int>((vertices[f.v_idx[2]].y + 1) * height / 2),
    //        //	static_cast<int>((vertices[f.v_idx[2]].z + 1) * 255 / 2), // Assuming z is in range [0, 1]
    //        //	buffer, zbuffer, colorchoice);
    //    }
    //}


};
