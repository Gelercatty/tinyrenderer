#include "utils/tgaimage.h"
#include "base/Transform.h"
#include "utils/geometry.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm> 
#include <random>
#include <numbers>
#include "base/Mesh.h"
#include "Camera.h"
#include "base/GameObject.h"
constexpr TGAColor white = {255,255,255,255};
constexpr TGAColor green   = {  0, 255,   0, 255};
constexpr TGAColor red     = {  0,   0, 255, 255};
constexpr TGAColor blue    = {255, 128,  64, 255};
constexpr TGAColor yellow  = {  0, 200, 255, 255};
TGAColor randomPredefinedColor() {
    static std::vector<TGAColor> colors = {
        white, green, red, blue, yellow
    };
    static std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> dist(0, colors.size() - 1);
    return colors[dist(gen)];
}



void line(int ax, int ay, int bx, int by, TGAImage &framebuffer, TGAColor coller){
    bool steep = std::abs(ax-bx) < std::abs(ay-by);
    if(steep){
        std::swap(ax, ay);
        std::swap(bx, by);
    }
    if(ax>bx){
        std::swap(ax,bx);
        std::swap(ay,by);
    }
    float y = ay;
    float ierror = 0;
    for(int x = ax; x <= bx; x++){
        if(steep){
            framebuffer.set(y, x, coller);
        } else {
            framebuffer.set(x, y, coller);
        }
        ierror += 2 * std::abs(by-ay);
        if(ierror > bx-ax){
            y += by > ay ? 1 : -1;
            ierror -= 2*(bx-ax);
        }
    }
}



//void triangle(int ax, int ay, float az, int bx, int by, float bz , int cx, int cy, float cz ,std::vector<float> &zbuffer, TGAImage& framebuffer, TGAColor color) {
//	int bbminx = std::min({ ax, bx, cx });
//	int bbminy = std::min({ ay, by, cy });
//	int bbmaxx = std::max({ ax, bx, cx });
//	int bbmaxy = std::max({ ay, by, cy });
//    float area = cross({ static_cast<double>(bx - ax), static_cast<double>(by - ay) },
//                       { static_cast<double>(cx - ax), static_cast<double>(cy - ay) });
//    if (area <= 0) return;                      // �޳�����
//    if (std::abs(area) < 1e-2f) return;       // ���̫С
//#pragma omp parallel for
//    for (int x = bbminx; x <= bbmaxx; x++) {
//        for (int y = bbminy; y <= bbmaxy; y++) {
//            vec3 bary = barycentric(ax, ay, bx, by, cx, cy, x, y);
//            if (bary.x < 0 || bary.y < 0 || bary.z < 0) { continue; }
//			float z = az * bary.x + bz * bary.y + cz * bary.z; // 计算z值
//            int idx = x + y * framebuffer.width();
//            if (z >= zbuffer[idx]) continue;
//            zbuffer[idx] = z;
//            framebuffer.set(x, y, color);
//        }
//    }
//}
//void write_zbuffer_image(const std::vector<float>& zbuffer, int width, int height, const std::string& filename) {
//    auto [zmin_it, zmax_it] = std::minmax_element(zbuffer.begin(), zbuffer.end());
//    float zmin = *zmin_it;
//    float zmax = *zmax_it;
//
//    std::cout << "[Z-Buffer Range] zmin: " << zmin << ", zmax: " << zmax << std::endl;
//
//    TGAImage zimg(width, height, TGAImage::RGB);
//
//    for (int y = 0; y < height; ++y) {
//        for (int x = 0; x < width; ++x) {
//            float z = zbuffer[x + y * width];
//            float znorm = (z - zmin) / (zmax - zmin);
//            int gray = static_cast<int>(znorm * 255);
//            gray = std::clamp(gray, 0, 255);
//			unsigned char ga = static_cast<unsigned char>(gray);
//            zimg.set(x, y, {  ga, ga, ga, 255 });
//        }
//    } 
//    zimg.write_tga_file(filename.c_str());
//}

vec3 persp(vec3 v) {
    constexpr double c = 3.;
    return v / (1 - v.z / c);
}


inline vec3 ndcToScreen(const vec3& ndc, int width, int height) {
    return {
        (ndc.x + 1.0f) * 0.5f * width,
        (ndc.y + 1.0f) * 0.5f * height, // Y轴反向
        ndc.z
    };
}
/////////////////////
void triangle(int ax, int ay, float az, int bx, int by, float bz, int cx, int cy, float cz, std::vector<float>& zbuffer, TGAImage& framebuffer, TGAColor color) {
    int bbminx = std::min({ ax, bx, cx });
    int bbminy = std::min({ ay, by, cy });
    int bbmaxx = std::max({ ax, bx, cx });
    int bbmaxy = std::max({ ay, by, cy });
    float area = cross({ bx - ax, by - ay }, { cx - ax, cy - ay });
    if (area <= 0 || std::abs(area) < 1e-2f) return;

#pragma omp parallel for
    for (int x = bbminx; x <= bbmaxx; x++) {
        for (int y = bbminy; y <= bbmaxy; y++) {
            vec3 bary = barycentric(ax, ay, bx, by, cx, cy, x, y);
            if (bary.x < 0 || bary.y < 0 || bary.z < 0) continue;
            float z = az * bary.x + bz * bary.y + cz * bary.z;
            int idx = x + y * framebuffer.width();
            if (z >= zbuffer[idx]) continue;
            zbuffer[idx] = z;
            framebuffer.set(x, y, color);
        }
    }
}


void write_zbuffer_image(const std::vector<float>& zbuffer, int width, int height, const std::string& filename) {
    auto [zmin_it, zmax_it] = std::minmax_element(zbuffer.begin(), zbuffer.end());
    float zmin = *zmin_it;
    float zmax = *zmax_it;

    float range = zmax - zmin;
    if (range < 1e-5f) range = 1.0f;

    TGAImage zimg(width, height, TGAImage::RGB);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float z = zbuffer[x + y * width];
            float znorm = (z - zmin) / range;
            int gray = static_cast<int>(znorm * 255);
            gray = std::clamp(gray, 0, 255);
            unsigned char g = static_cast<unsigned char>(gray);
            zimg.set(x, y, { g, g, g, 255 });
        }
    }
    zimg.write_tga_file(filename.c_str());
}


/////////////////////
//int main(int argc, char** argv) {
//
//    int width = 1080;
//    int height = 720;
//    TGAImage framebuffer(width, height, TGAImage::RGB);
//    TGAImage zbuffer_tga(width, height, TGAImage::RGB);
//    std::vector<float> zbuffer(width * height, 1.0);
//
//    Camera camera;
//	camera.transform.position = { 0,0.5,1 }; // 相机位置
//	camera.aspect_ratio = static_cast<float>(width) / height;
//
//    GameObject testObj;
//    std::string path = R"(C:\Users\Y9000P\Desktop\WorkingSpace\tinyrenderer\obj\diablo3_pose\diablo3_pose.obj)";
//    testObj.mesh.load_from_obj(path);
//	testObj.transform.position = { 0, 0, 0 }; // 世界空间位置
//	camera.transform.lookAt({ 0, 0, 0 }); // 面向原点 
//    //float deg2rad = std::numbers::pi / 180.f;
//    int num = 0;
//
//	double maxzz = std::numeric_limits<float>::lowest();
//	double minzz = std::numeric_limits<float>::max();
//
//    for (auto &f : testObj.mesh.faces()) {
//
//		vec3 v0 = testObj.mesh.vertices()[f.v_idx[0]];
//		vec3 v1 = testObj.mesh.vertices()[f.v_idx[1]];
//		vec3 v2 = testObj.mesh.vertices()[f.v_idx[2]];
//
//        // MVP
//		mat<4, 4> MVP = camera.ProjectMatrix() * camera.ViewMatrix() * testObj.transform.ModelMatrix();
//		vec4 p0 = MVP * vec4{ v0.x, v0.y, v0.z, 1.0f };
//		vec4 p1 = MVP * vec4{ v1.x, v1.y, v1.z, 1.0f };
//		vec4 p2 = MVP * vec4{ v2.x, v2.y, v2.z, 1.0f };
//
//		vec3 ndc0 = vec3{ p0.x, p0.y, p0.z } / p0.w; // NDC
//		vec3 ndc1 = vec3{ p1.x, p1.y, p1.z } / p1.w; // NDC
//		vec3 ndc2 = vec3{ p2.x, p2.y, p2.z } / p2.w; // NDC
//
//  //      // 剔除ndcbox 之外的顶点
//		if (ndc0.x < -1 || ndc0.x > 1 || ndc0.y < -1 || ndc0.y > 1 ||
//			ndc1.x < -1 || ndc1.x > 1 || ndc1.y < -1 || ndc1.y > 1 ||
//			ndc2.x < -1 || ndc2.x > 1 || ndc2.y < -1 || ndc2.y > 1) 
//			continue; 
//        
//		vec3 sp0 = ndcToScreen(ndc0, width, height);
//		vec3 sp1 = ndcToScreen(ndc1, width, height);
//		vec3 sp2 = ndcToScreen(ndc2, width, height);
//		maxzz = std::max(maxzz, std::max({ sp0.z, sp1.z, sp2.z }));
//		minzz = std::min(minzz, std::min({ sp0.z, sp1.z, sp2.z }));
//	    triangle(
//			static_cast<int>(sp0.x), static_cast<int>(sp0.y), sp0.z,
//			static_cast<int>(sp1.x), static_cast<int>(sp1.y), sp1.z,
//			static_cast<int>(sp2.x), static_cast<int>(sp2.y), sp2.z,
//            zbuffer,
//			framebuffer,  
//            randomPredefinedColor()),
//		framebuffer.set(static_cast<int>(sp0.x), static_cast<int>(sp0.y), white);
//		framebuffer.set(static_cast<int>(sp1.x), static_cast<int>(sp1.y), white);
//		framebuffer.set(static_cast<int>(sp2.x), static_cast<int>(sp2.y), white);
//    }
//    //auto max_it = std::max_element(zbuffer.begin(), zbuffer.end());
//    //auto min_it = std::min_element(zbuffer.begin(), zbuffer.end());
//
//    //std::cout << "max: " << *max_it << std::endl;
//    //std::cout << "min: " << *min_it << std::endl;
//	std::cout << "max z: " << maxzz << std::endl;
//	std::cout << "min z: " << minzz << std::endl;
//    framebuffer.write_tga_file("AAframeBuffer.tga");
//	write_zbuffer_image(zbuffer, width, height, "zbuffer.tga");
//};



int main(int argc, char** argv) {

    const int width = 1080;
    const int height = 720;
    TGAImage framebuffer(width, height, TGAImage::RGB);
    std::vector<float> zbuffer(width * height, std::numeric_limits<float>::max());

    Camera camera;
    camera.transform.position = { 0, 0.5, 1 };
    camera.aspect_ratio = static_cast<float>(width) / height;

    GameObject testObj;
    std::string path = R"(C:\Users\Y9000P\Desktop\WorkingSpace\tinyrenderer\obj\diablo3_pose\diablo3_pose.obj)";
    testObj.mesh.load_from_obj(path);
    testObj.transform.position = { 0, 0, 0 };
    camera.transform.lookAt({ 0, 0, 0 });

    mat<4, 4> M = testObj.transform.ModelMatrix();
    mat<4, 4> V = camera.ViewMatrix();
    mat<4, 4> P = camera.ProjectMatrix();
    mat<4, 4> MVP = P * V * M;

    for (auto& f : testObj.mesh.faces()) {
        vec3 v0 = testObj.mesh.vertices()[f.v_idx[0]];
        vec3 v1 = testObj.mesh.vertices()[f.v_idx[1]];
        vec3 v2 = testObj.mesh.vertices()[f.v_idx[2]];

        vec4 p0_clip = MVP * vec4{ v0.x, v0.y, v0.z, 1.0f };
        vec4 p1_clip = MVP * vec4{ v1.x, v1.y, v1.z, 1.0f };
        vec4 p2_clip = MVP * vec4{ v2.x, v2.y, v2.z, 1.0f };

        vec3 ndc0 = vec3{ p0_clip.x, p0_clip.y, p0_clip.z } / p0_clip.w;
        vec3 ndc1 = vec3{ p1_clip.x, p1_clip.y, p1_clip.z } / p1_clip.w;
        vec3 ndc2 = vec3{ p2_clip.x, p2_clip.y, p2_clip.z } / p2_clip.w;

        if (std::abs(ndc0.x) > 1 || std::abs(ndc0.y) > 1 ||
            std::abs(ndc1.x) > 1 || std::abs(ndc1.y) > 1 ||
            std::abs(ndc2.x) > 1 || std::abs(ndc2.y) > 1)
            continue;

        vec3 sp0 = ndcToScreen(ndc0, width, height);
        vec3 sp1 = ndcToScreen(ndc1, width, height);
        vec3 sp2 = ndcToScreen(ndc2, width, height);

        vec4 p0_view = V * M * vec4{ v0.x, v0.y, v0.z, 1.0f };
        vec4 p1_view = V * M * vec4{ v1.x, v1.y, v1.z, 1.0f };
        vec4 p2_view = V * M * vec4{ v2.x, v2.y, v2.z, 1.0f };

        float z0_eye = -p0_view.z;
        float z1_eye = -p1_view.z;
        float z2_eye = -p2_view.z;

        triangle(
            static_cast<int>(sp0.x), static_cast<int>(sp0.y), z0_eye,
            static_cast<int>(sp1.x), static_cast<int>(sp1.y), z1_eye,
            static_cast<int>(sp2.x), static_cast<int>(sp2.y), z2_eye,
            zbuffer,
            framebuffer,
            randomPredefinedColor()
        );
    }

    framebuffer.write_tga_file("AAframeBuffer.tga");
    write_zbuffer_image(zbuffer, width, height, "zbuffer.tga");
}
