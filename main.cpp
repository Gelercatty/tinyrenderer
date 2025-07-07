#include "utils/myMath.h"
#include "utils/tgaimage.h"
#include "base/Transform.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm> 
#include <random>
#include <numbers>
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


struct Face
{
    std::vector<int> v_idx;
};
std::vector<std::string> split(const std::string& str, char delimiter){
	std::vector<std::string> tokens;
	std::istringstream tokenStream(str);
	std::string token;
	while (std::getline(tokenStream, token, delimiter)) {
		tokens.push_back(token);
	}
	return tokens;
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



void triangle(int ax, int ay, int az, int bx, int by, int bz , int cx, int cy, int cz ,TGAImage& framebuffer, TGAImage& zbuffer, TGAColor color) {
	int bbminx = std::min({ ax, bx, cx });
	int bbminy = std::min({ ay, by, cy });
	int bbmaxx = std::max({ ax, bx, cx });
	int bbmaxy = std::max({ ay, by, cy });
    float area = cross({ static_cast<float>(bx - ax), static_cast<float>(by - ay) },
                        { static_cast<float>(cx - ax), static_cast<float>(cy - ay) });
    if (area <= 0) return;                      // �޳�����
    if (std::abs(area) < 1e-2f) return;       // ���̫С
#pragma omp parallel for
    for (int x = bbminx; x <= bbmaxx; x++) {
        for (int y = bbminy; y <= bbmaxy; y++) {
            vec3 bary = barycentric(ax, ay, bx, by, cx, cy, x, y);
            if (bary.x < 0 || bary.y < 0 || bary.z < 0) { continue; }
            unsigned char z = static_cast<unsigned char>(bary.x * az + bary.y * bz + bary.z * cz);
            if (z <= zbuffer.get(x, y)[0]){ 
                continue;
            }
            framebuffer.set(x, y, color);
			zbuffer.set(x, y, {z,z,z});
        }
    }
}

vec3 persp(vec3 v) {
    constexpr double c = 3.;
    return v / (1 - v.z / c);
}

class Mesh {
public:
    std::vector<vec3> vertices;
    std::vector<Face> faces;
    float size = 1;
	// max and min bounds for the mesh
    float max_x, min_x;
    float max_y, min_y;
	float max_z, min_z;  

    bool clear(){
		vertices.clear();
		faces.clear();
		return true;
    }
    bool load_from_obj(const std::string& filename) {
        std::ifstream in(filename);
        if (!in) {
            std::cerr<< "Cannot open " << filename << std::endl;
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
				
                vertices.push_back(v);
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
                faces.push_back(f);
            }
        } 
        if (!faces.empty()) {
			std::cout << "load face done, face size: " << faces.size() << std::endl;
            updateBounds();
        }
        if (!vertices.empty()) {
			std::cout << "load vertex done, vertex size: " << vertices.size() << std::endl;
        }
    
    }
    void drawPreview_line(TGAImage& buffer) {
        int width = buffer.width();
		int height = buffer.height();
        
        for (Face f : faces) {
            for (int i = 0; i < f.v_idx.size(); i++) {
                int idx1 = f.v_idx[i];
				int idx2 = f.v_idx[(i + 1) % f.v_idx.size()]; // wrap around to the first vertex

			    int	x1_x = static_cast<int>((vertices[idx1].x + 1) * width / 2);
                int x1_y = static_cast<int>((vertices[idx1].y + 1) * height / 2);
				int x2_x = static_cast<int>((vertices[idx2].x + 1) * width / 2);
				int x2_y = static_cast<int>((vertices[idx2].y + 1) * height / 2);

				if (x1_x >= 0 && x1_x < width && x1_y >= 0 && x1_y < height &&
					x2_x >= 0 && x2_x < width && x2_y >= 0 && x2_y < height) {
					line(x1_x, x1_y, x2_x, x2_y, buffer, red);
				}
            }
        }
    }
    void drawPreview_triangle(TGAImage& buffer, TGAImage &zbuffer) {
        int width = buffer.width();
        int height = buffer.height();
		// Clear the buffer  
        for (int x = 0; x < width; ++x) {
            for (int y = 0; y < height; ++y) {
                zbuffer.set(x, y, {0,0,0}); // ��Զ
            }
        }
        for (Face f : faces) {
            TGAColor colorchoice = randomPredefinedColor();

// fake objection
			vec3 v0 = persp(vertices[f.v_idx[0]]);
			vec3 v1 = persp(vertices[f.v_idx[1]]);
			vec3 v2 = persp(vertices[f.v_idx[2]]);
			//vec3 v0 = vertices[f.v_idx[0]];
			//vec3 v1 = vertices[f.v_idx[1]];
			//vec3 v2 = vertices[f.v_idx[2]];
            triangle(
				static_cast<int>((v0.x + 1) * width / 2),
				static_cast<int>((v0.y + 1) * height / 2),
				static_cast<int>((v0.z + 1) * 255 / 2), // Assuming z is in range [0, 1]
				static_cast<int>((v1.x + 1) * width / 2),
				static_cast<int>((v1.y + 1) * height / 2),
				static_cast<int>((v1.z + 1) * 255 / 2), // Assuming z is in range [0, 1]
				static_cast<int>((v2.x + 1) * width / 2),
				static_cast<int>((v2.y + 1) * height / 2),
				static_cast<int>((v2.z + 1) * 255 / 2), // Assuming z is in range [0, 1]
				buffer, zbuffer, colorchoice);


			//triangle(
			//	static_cast<int>((vertices[f.v_idx[0]].x + 1) * width / 2),
			//	static_cast<int>((vertices[f.v_idx[0]].y + 1) * height / 2),
			//	static_cast<int>((vertices[f.v_idx[0]].z + 1) * 255 / 2), // Assuming z is in range [0, 1]
			//	static_cast<int>((vertices[f.v_idx[1]].x + 1) * width / 2),
			//	static_cast<int>((vertices[f.v_idx[1]].y + 1) * height / 2),
			//	static_cast<int>((vertices[f.v_idx[1]].z + 1) * 255 / 2), // Assuming z is in range [0, 1]
			//	static_cast<int>((vertices[f.v_idx[2]].x + 1) * width / 2),
			//	static_cast<int>((vertices[f.v_idx[2]].y + 1) * height / 2),
			//	static_cast<int>((vertices[f.v_idx[2]].z + 1) * 255 / 2), // Assuming z is in range [0, 1]
			//	buffer, zbuffer, colorchoice);
        }
    }

private:
	void updateBounds() {
		auto [min_it_x, max_it_x] = std::minmax_element(vertices.begin(), vertices.end(),
			[](const vec3& a, const vec3& b) { return a.x < b.x; });
		auto [min_it_y, max_it_y] = std::minmax_element(vertices.begin(), vertices.end(),
			[](const vec3& a, const vec3& b) { return a.y < b.y; });
		auto [min_it_z, max_it_z] = std::minmax_element(vertices.begin(), vertices.end(),
			[](const vec3& a, const vec3& b) { return a.z < b.z; });
		min_x = min_it_x->x;
		max_x = max_it_x->x;
		min_y = min_it_y->y;
		max_y = max_it_y->y;
		min_z = min_it_z->z;
		max_z = max_it_z->z;
        return;
	}
};




class BaseActor {
public:
	Transform transform;
	BaseActor() : transform({ {0, 0, 0}, {0, 0, 0} }) {}
};


int main(int argc, char** argv) {

    const int width = 800;
    const int height = 600;

	TGAImage framebuffer(width, height, TGAImage::RGB);
	TGAImage zBuffer(width, height, TGAImage::RGB);

    Mesh mesh;
    mesh.load_from_obj("C:\\Users\\Y9000P\\Desktop\\WorkingSpace\\tinyrenderer\\obj\\diablo3_pose\\diablo3_pose.obj");

    for (auto& v : mesh.vertices) {
        v = rotate(v, { 70,0,0 });
    }
    mesh.drawPreview_triangle(framebuffer, zBuffer);
	framebuffer.write_tga_file("output3.tga");
	zBuffer.write_tga_file("zbuffer.tga");

    return 0;


};