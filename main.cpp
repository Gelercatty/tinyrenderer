#include "tgaimage.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm> 
#include <random>
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
enum class ProjectionType {
	ORTHOGRAPHIC, // Orthographic projection 正交投影
	PERSPECTIVE   // Perspective projection 透视投影
};
struct Vec3f
{
    float x, y, z;
    Vec3f normalize() {
        float length = std::sqrt(x * x + y * y + z * z);
        if (length == 0) return { 0, 0, 0 }; // Avoid division by zero
        return { x / length, y / length, z / length };
    }
};
struct Vec2f
{
    float x, y;
	Vec2f normalize() {
		float length = std::sqrt(x * x + y * y);
		if (length == 0) return { 0, 0 }; // Avoid division by zero
		return { x / length, y / length };
	}
};
struct Face
{
    std::vector<int> v_idx;
};
struct Transform
{
    Vec3f position;
    Vec3f rotation; // Euler angles in radians
};

class Camera: Transform{
public:
	ProjectionType projection_type = ProjectionType::ORTHOGRAPHIC;     
    
    

};
// math
float cross(const Vec2f& a, const Vec2f& b) {
    return a.x * b.y - a.y * b.x;
}
// tool 
std::vector<std::string> split(const std::string& str, char delimiter){
	std::vector<std::string> tokens;
	std::istringstream tokenStream(str);
	std::string token;
	while (std::getline(tokenStream, token, delimiter)) {
		tokens.push_back(token);
	}
	return tokens;
}
/// <summary>
/// drawa line between two points (ax, ay) and (bx, by) in the framebuffer.
/// </summary>
/// <param name="ax"></param>
/// <param name="ay"></param>
/// <param name="bx"></param>
/// <param name="by"></param>
/// <param name="framebuffer"></param>
/// <param name="coller"></param>

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

//std::vector<std::pair<int,int>> line_(int ax, int ay, int bx, int by, TGAImage& framebuffer, TGAColor coller) {
//    bool steep = std::abs(ax - bx) < std::abs(ay - by);
//    if (steep) {
//        std::swap(ax, ay);
//        std::swap(bx, by);
//    }
//    if (ax > bx) {
//        std::swap(ax, bx);
//        std::swap(ay, by);
//    }
//    float y = ay;
//    float ierror = 0;
//	std::vector<std::pair<int, int>> points;
//    for (int x = ax; x <= bx; x++) {
//        if (steep) {
//            //framebuffer.set(y, x, coller);
//			points.emplace_back(y, x);
//        }
//        else {
//            //framebuffer.set(x, y, coller);
//			points.emplace_back(x, y);
//        }
//        ierror += 2 * std::abs(by - ay);
//        if (ierror > bx - ax) {
//            y += by > ay ? 1 : -1;
//            ierror -= 2 * (bx - ax);
//        }
//    }
//	return points;
//}
//
//std::vector<std::pair<int, int>> getTriangleBoundle(int ax, int ay, int bx, int by, int cx, int cy) {
//    // Step 1: 找到 y 的范围
//    int min_y = std::min({ ay, by, cy });
//    int max_y = std::max({ ay, by, cy });
//
//    // 初始化表格，先用极端值填充
//    int height = max_y - min_y + 1;
//    std::vector<std::pair<int, int>> bounds(height, { INT_MAX, INT_MIN });
//
//    // Step 2: 定义一个 lambda，用于填充边界信息
//    auto scan_edge = [&](int x0, int y0, int x1, int y1) {
//        auto pts = line_(x0, y0, x1, y1, *(TGAImage*)nullptr, white); // framebuffer 为空，只取坐标
//
//        for (auto& [x, y] : pts) {
//            if (y < min_y || y > max_y) continue;
//            int idx = y - min_y;
//            bounds[idx].first = std::min(bounds[idx].first, x);
//            bounds[idx].second = std::max(bounds[idx].second, x);
//        }
//        };
//
//    // Step 3: 扫描三条边
//    scan_edge(ax, ay, bx, by);
//    scan_edge(bx, by, cx, cy);
//    scan_edge(cx, cy, ax, ay);
//
//    return bounds;
//}
//
//
//void triangle(int ax, int ay, int bx, int by, int cx, int cy, TGAImage& framebuffer, TGAColor color) {
//    int top = std::max(std::max(ay, by), cy);
//    int down = std::min(std::min(ay, by), cy);
//
//    std::vector<std::pair<int, int>> bounds = getTriangleBoundle(ax, ay, bx, by, cx, cy);
//    for (int y = down; y <= top; y++) {
//        if (y < 0 || y >= framebuffer.height()) continue; // 确保 y 在有效范围内
//        int x_start = bounds[y - down].first;
//        int x_end = bounds[y - down].second;
//        if (x_start < 0) x_start = 0;
//        if (x_end >= framebuffer.width()) x_end = framebuffer.width() - 1;
//        for (int x = x_start; x <= x_end; x++) {
//            framebuffer.set(x, y, color);
//        }
//    }
//}
// 
bool isPtsinTriangle(int ax, int ay, int bx, int by, int cx, int cy, Vec2f p) {
	Vec2f ab = { bx - ax, by - ay };
	Vec2f bc = { cx - bx, cy - by };
    Vec2f ca = { ax - cx, ay - cy };

	Vec2f ap = { p.x - ax, p.y - ay };
	Vec2f bp = { p.x - bx, p.y - by };
	Vec2f cp = { p.x - cx, p.y - cy };

	float cross_ab_ap = cross(ab, ap);
	float cross_bc_ap = cross(bc, bp);
	float cross_ca_cp = cross(ca, cp);

	return (cross_ab_ap >= 0 && cross_bc_ap >= 0 && cross_ca_cp >= 0) ||
		(cross_ab_ap <= 0 && cross_bc_ap <= 0 && cross_ca_cp <= 0);
}
void triangle(int ax, int ay, int bx, int by, int cx, int cy, TGAImage& framebuffer, TGAColor color) { 
	int bbminx = std::min({ ax, bx, cx });
	int bbminy = std::min({ ay, by, cy });
	int bbmaxx = std::max({ ax, bx, cx });
	int bbmaxy = std::max({ ay, by, cy });
    float area = cross({ static_cast<float>(bx - ax), static_cast<float>(by - ay) },
                        { static_cast<float>(cx - ax), static_cast<float>(cy - ay) });
    if (area <= 0) return;                      // 剔除背面
    if (std::abs(area) < 1e-2f) return;       // 面积太小
#pragma omp parallel for
    for (int x = bbminx; x <= bbmaxx; x++) {
        for (int y = bbminy; y <= bbmaxy; y++) {
            if (isPtsinTriangle(ax, ay, bx, by, cx, cy, { static_cast<float>(x), static_cast<float>(y) })) {
                framebuffer.set(x, y, color);
            }
        }
    }
}

class Mesh {
public:
    std::vector<Vec3f> vertices;
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
                Vec3f v;
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
    void drawPreview_triangle(TGAImage& buffer) {
        int width = buffer.width();
        int height = buffer.height();
		// Clear the buffer  
		
        for (Face f : faces) {
            TGAColor colorchoice = randomPredefinedColor();
			triangle(
				static_cast<int>((vertices[f.v_idx[0]].x + 1) * width / 2),
				static_cast<int>((vertices[f.v_idx[0]].y + 1) * height / 2),
				static_cast<int>((vertices[f.v_idx[1]].x + 1) * width / 2),
				static_cast<int>((vertices[f.v_idx[1]].y + 1) * height / 2),
				static_cast<int>((vertices[f.v_idx[2]].x + 1) * width / 2),
				static_cast<int>((vertices[f.v_idx[2]].y + 1) * height / 2),
				buffer, colorchoice);
        }
    }

private:
	void updateBounds() {
		auto [min_it_x, max_it_x] = std::minmax_element(vertices.begin(), vertices.end(),
			[](const Vec3f& a, const Vec3f& b) { return a.x < b.x; });
		auto [min_it_y, max_it_y] = std::minmax_element(vertices.begin(), vertices.end(),
			[](const Vec3f& a, const Vec3f& b) { return a.y < b.y; });
		auto [min_it_z, max_it_z] = std::minmax_element(vertices.begin(), vertices.end(),
			[](const Vec3f& a, const Vec3f& b) { return a.z < b.z; });
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

class Cube : public BaseActor, public Mesh {
public:
    Cube() {
        // Initialize the cube with a default size and position
        transform.position = { 0, 0, 0 };
        transform.rotation = { 0, 0, 0 };
        vertices = {
        {-0.5f, -0.5f, -0.5f}, // 0
        { 0.5f, -0.5f, -0.5f}, // 1
        { 0.5f,  0.5f, -0.5f}, // 2
        {-0.5f,  0.5f, -0.5f}, // 3
        {-0.5f, -0.5f,  0.5f}, // 4
        { 0.5f, -0.5f,  0.5f}, // 5
        { 0.5f,  0.5f,  0.5f}, // 6
        {-0.5f,  0.5f,  0.5f}  // 7
        };
		faces = {
			{{0, 1, 2, 3}}, // front face
			{{4, 5, 6, 7}}, // back face
			{{0, 1, 5, 4}}, // bottom face
			{{2, 3, 7, 6}}, // top face
			{{0, 3, 7, 4}}, // left face
			{{1, 2, 6, 5}}  // right face
		};
    }

};

int main(int argc, char** argv) {

    const int width = 800;
    const int height = 600;

	TGAImage framebuffer(width, height, TGAImage::RGB);
    Mesh mesh;
    mesh.load_from_obj("C:\\Users\\Y9000P\\Desktop\\WorkingSpace\\tinyrenderer\\obj\\diablo3_pose\\diablo3_pose.obj");
	mesh.drawPreview_triangle(framebuffer);
	framebuffer.write_tga_file("output3.tga");
   
    return 0;


};