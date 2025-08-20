# CatsRender
大猫猫的渲染引擎

基于`GLFW`  `Dear ImGui` `vulkan` 的实时渲染引擎。

大猫猫的图形学学习之旅。

# BluePrint
```
 your_project/
├─ CMakeLists.txt
├─ external/               # 第三方（glfw、imgui、vulkan-headers/VMA等）
├─ app/
│  └─ main.cpp             # 入口，解析参数，创建窗口&选择渲染后端
├─ engine/
│  ├─ core/                # Math/Transform/Timer/Log
│  ├─ platform/
│  │  ├─ IWindow.hpp
│  │  └─ GlfwWindow.cpp/.hpp
│  ├─ ui/
│  │  └─ ImGuiLayer.cpp/.hpp  # 统一 ImGui 生命周期（NewFrame/Render等）
│  ├─ renderer/
│  │  ├─ IRenderer.hpp
│  │  ├─ software/         # 纯CPU渲染器
│  │  │  ├─ SoftwareRenderer.cpp/.hpp
│  │  │  └─ GlTexturePresenter.cpp/.hpp  # 用OpenGL纹理显示CPU帧
│  │  └─ vulkan/           # Vulkan后端
│  │     ├─ VulkanRenderer.cpp/.hpp
│  │     └─ VulkanContext.cpp/.hpp       # instance/device/swapchain等
│  └─ scene/               # GameObject/Mesh/Material/Camera/ShaderDesc
└─ assets/
```