cmake_minimum_required(VERSION 3.12)
get_property(is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(NOT is_multi_config AND NOT (CMAKE_BUILD_TYPE OR DEFINED ENV{CMAKE_BUILD_TYPE}))
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Release default")
endif()

project(tinyrenderer LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 20)

option(iwyu "Run include-what-you-use")
if(iwyu)
  find_program(IWYU_EXE NAMES include-what-you-use REQUIRED)
  set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE ${IWYU_EXE})
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU|Intel")
  add_compile_options(-Wall)
endif()

find_package(OpenMP COMPONENTS CXX)


file(GLOB APP_FILES    "${CMAKE_CURRENT_SOURCE_DIR}/app/*.cpp")

set(IMGUI_SOURCE
    external/imgui/imgui.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/backends/imgui_impl_glfw.cpp
    external/imgui/backends/imgui_impl_opengl3.cpp)

set(SOURCES ${APP_FILES}
            ${IMGUI_SOURCE})

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/backends
)

target_link_libraries(${PROJECT_NAME} PRIVATE $<$<BOOL:${OpenMP_CXX_FOUND}>:OpenMP::OpenMP_CXX>)
# 添加 GLFW 子目录
add_subdirectory(external/glfw)
# 链接 GLFW
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

file(GENERATE OUTPUT .gitignore CONTENT "*")
