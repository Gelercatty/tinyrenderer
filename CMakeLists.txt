cmake_minimum_required(VERSION 3.16)
project(tinyrenderer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 选择渲染路径：vulkan / soft（都链接 Vulkan，soft 用它来显示自制 framebuffer）
set(RENDER_BACKEND "vulkan" CACHE STRING "Render backend: vulkan or soft")
set_property(CACHE RENDER_BACKEND PROPERTY STRINGS vulkan soft)

# 编译告警
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU|Intel")
  add_compile_options(-Wall)
endif()

find_package(OpenMP COMPONENTS CXX)

# -----------------------------
# ImGui 源码 + Vulkan/GLFW 后端
# -----------------------------
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui)
set(IMGUI_BACKENDS_DIR ${IMGUI_DIR}/backends)

set(IMGUI_SOURCES
  ${IMGUI_DIR}/imgui.cpp
  ${IMGUI_DIR}/imgui_draw.cpp
  ${IMGUI_DIR}/imgui_tables.cpp
  ${IMGUI_DIR}/imgui_widgets.cpp
  ${IMGUI_BACKENDS_DIR}/imgui_impl_glfw.cpp
  ${IMGUI_BACKENDS_DIR}/imgui_impl_vulkan.cpp
)



# 应用源文件：同时抓取 .cpp 和 .mm
file(GLOB APP_CPP "${CMAKE_CURRENT_SOURCE_DIR}/app/*.cpp")
file(GLOB APP_MM  "${CMAKE_CURRENT_SOURCE_DIR}/app/*.mm")
set(APP_FILES ${APP_CPP} ${APP_MM})

# 如果包含 .mm，需要启用 OBJCXX
if(APPLE AND APP_MM)
  enable_language(OBJCXX)
endif()

# 调试：打印哪些源文件被加入（确认 main.* 在里面）
message(STATUS "App sources: ${APP_FILES}")
# 你的应用源码（纯 C++）
# file(GLOB APP_FILES "${CMAKE_CURRENT_SOURCE_DIR}/app/*.cpp")

add_executable(${PROJECT_NAME}
  ${APP_FILES}
  ${IMGUI_SOURCES}
)

target_include_directories(${PROJECT_NAME} PRIVATE
  ${IMGUI_DIR}
  ${IMGUI_BACKENDS_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  $<$<BOOL:${OpenMP_CXX_FOUND}>:OpenMP::OpenMP_CXX>
)

# -----------------------------
# GLFW
# -----------------------------
# 如果你把 GLFW 当作子模块放在 external/glfw
add_subdirectory(external/glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

# -----------------------------
# Vulkan（跨平台）
# -----------------------------
# 优先用 find_package(Vulkan)；找不到时回退到 VULKAN_SDK 环境变量
find_package(Vulkan QUIET)

if(NOT Vulkan_FOUND)
  if(DEFINED ENV{VULKAN_SDK})
    message(STATUS "Vulkan SDK from ENV: $ENV{VULKAN_SDK}")
    target_include_directories(${PROJECT_NAME} PRIVATE "$ENV{VULKAN_SDK}/include")
    if(WIN32)
      target_link_directories(${PROJECT_NAME} PRIVATE "$ENV{VULKAN_SDK}/Lib")
      target_link_libraries(${PROJECT_NAME} PRIVATE vulkan-1)
    else()
      target_link_directories(${PROJECT_NAME} PRIVATE "$ENV{VULKAN_SDK}/lib")
      target_link_libraries(${PROJECT_NAME} PRIVATE vulkan)
    endif()
  else()
    message(FATAL_ERROR "Vulkan not found. Install LunarG Vulkan SDK and/or set VULKAN_SDK env.")
  endif()
else()
  target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan)
endif()

# -----------------------------
# 平台特定设置
# -----------------------------
if(APPLE)
  # 使用 MoltenVK（Vulkan -> Metal），运行时需配置 VK_ICD_FILENAMES 指向 MoltenVK_icd.json
  target_link_libraries(${PROJECT_NAME} PRIVATE
    "-framework Cocoa"
    "-framework IOKit"
    "-framework CoreVideo"
  )
  # brew 安装的库常在此处
  set(CMAKE_BUILD_RPATH "/opt/homebrew/lib")
elseif(WIN32)
  # Windows 一般无需额外系统库；Vulkan::Vulkan -> vulkan-1.lib
elseif(UNIX AND NOT APPLE)
  # Linux 上常见的 X11/Wayland 依赖由 GLFW 自己处理；如需手动可在此添加
endif()

# -----------------------------
# 宏/定义
# -----------------------------
# 如果你在渲染路径里用 glfwGetRequiredInstanceExtensions/glfwCreateWindowSurface，
# 不需要显式定义平台宏（VK_USE_PLATFORM_*）。GLFW 会处理。
target_compile_definitions(${PROJECT_NAME} PRIVATE
  $<$<STREQUAL:${RENDER_BACKEND},soft>:TR_BACKEND_SOFT=1>
  $<$<STREQUAL:${RENDER_BACKEND},vulkan>:TR_BACKEND_VULKAN=1>
)

# 生成 .gitignore
file(GENERATE OUTPUT .gitignore CONTENT "*")
