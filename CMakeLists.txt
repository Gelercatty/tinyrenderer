cmake_minimum_required(VERSION 3.16)
project(tinyrenderer LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- 源码 ---
add_executable(${PROJECT_NAME}
  app/main_debug.cpp
  external/imgui/imgui.cpp
  external/imgui/imgui_draw.cpp
  external/imgui/imgui_tables.cpp
  external/imgui/imgui_widgets.cpp
  external/imgui/imgui_demo.cpp        # 不需要可删
  external/imgui/backends/imgui_impl_glfw.cpp
  external/imgui/backends/imgui_impl_opengl3.cpp
  $<$<PLATFORM_ID:Windows>:external/glad/src/gl.c>  # Windows: glad v2 源码
)

target_include_directories(${PROJECT_NAME} PRIVATE
  external/imgui
  $<$<PLATFORM_ID:Windows>:external/glad/include>
)

# --- 只链接：GLFW + OpenGL ---
add_subdirectory(external/glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

if(APPLE)
  target_link_libraries(${PROJECT_NAME} PRIVATE
    "-framework OpenGL" "-framework Cocoa" "-framework IOKit" "-framework CoreVideo"
  )
  target_compile_definitions(${PROJECT_NAME} PRIVATE
    # IMGUI_IMPL_OPENGL_LOADER_CUSTOM
    GL_SILENCE_DEPRECATION=1
  )
elseif(WIN32)
  target_link_libraries(${PROJECT_NAME} PRIVATE opengl32)
  target_compile_definitions(${PROJECT_NAME} PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLAD2)
else() # Linux 等
  find_package(OpenGL REQUIRED)
  target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)
  target_compile_definitions(${PROJECT_NAME} PRIVATE IMGUI_IMPL_OPENGL_LOADER_CUSTOM)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
