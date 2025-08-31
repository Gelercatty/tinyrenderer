#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <OpenGL/gl3.h>
#include <CoreText/CoreText.h>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <iostream>

const int width = 1280;
const int height = 720;
static uint32_t framebuffer[width * height]; // 颜色缓冲

// 顶点着色器源码
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;    // 顶点位置
layout (location = 1) in vec3 aColor;  // 顶点颜色

out vec3 ourColor; // 传给片段着色器

void main()
{
    gl_Position = vec4(aPos, 1.0);
    ourColor = aColor;
}
)";

// 片段着色器源码
const char* fragmentShaderSource = R"(
#version 330 core
in vec3 ourColor;
out vec4 FragColor;

void main()
{
    FragColor = vec4(ourColor, 1.0);
}
)";

// 🔹 工具函数：根据字体名找到系统里对应的字体文件路径
std::string GetFontPath(const char* fontName) {
    // 创建一个 CTFont 对象
    CTFontRef fontRef = CTFontCreateWithName(
        CFStringCreateWithCString(NULL, fontName, kCFStringEncodingUTF8),
        0.0,
        NULL
    );
    if (!fontRef) return "";

    // 获取字体文件 URL
    CFURLRef url = (CFURLRef)CTFontCopyAttribute(fontRef, kCTFontURLAttribute);
    CFRelease(fontRef);

    if (!url) return "";

    // 转成文件系统路径
    char path[PATH_MAX];
    CFURLGetFileSystemRepresentation(url, true, (UInt8*)path, PATH_MAX);
    CFRelease(url);

    return std::string(path);
}



int main(){
    // 初始化 GLFW
    if(!glfwInit()){
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Mac 必须要这句
#endif

    // 创建窗口
    GLFWwindow* window = glfwCreateWindow(width, height, "TinyRenderer", nullptr, nullptr);
    if(!window){
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // 开启 VSync

    // 初始化 ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark(); // UI 风格
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    
    std::string chalkPath = GetFontPath("Chalkboard");
    ImFont* chalk = io.Fonts->AddFontFromFileTTF(chalkPath.c_str(), 20.0f);





    // ====== 创建 Shader 程序 ======
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // 删除临时 shader 对象
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // ====== 设置顶点数据 VAO/VBO ======
    float vertices[] = {
        // positions        // colors
         0.0f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // 上 (红色)
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // 左 (绿色)
         0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // 右 (蓝色)
    };

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO); // 创建 VAO
    glGenBuffers(1, &VBO);      // 创建 VBO

    glBindVertexArray(VAO);

    // 把顶点数据送进 VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 顶点属性指针 (位置)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 顶点属性指针 (颜色)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    // 创建纹理， 将软光栅画出的内容设置为纹理，进行渲染
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // ====== 主循环 ======
    while(!glfwWindowShouldClose(window)){
        // 1. 处理事件
        glfwPollEvents();
        
        // 2. 清屏
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 3. 画三角形
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glBindTexture(GL_TEXTURE_2D, tex);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height,
                GL_RGBA, GL_UNSIGNED_BYTE, framebuffer);
        // 4. 画 ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Hello, world!");
        ImGui::Text("This is a simple ImGui example.");
        static float f = 0.0f;
        ImGui::SliderFloat("Float", &f, 0.0f, 1.0f);
        if(ImGui::Button("click here"))
            std::cout << "Button clicked!" << std::endl;
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // 5. 交换缓冲区
        glfwSwapBuffers(window);
    }
    
    // ====== 资源清理 ======
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
