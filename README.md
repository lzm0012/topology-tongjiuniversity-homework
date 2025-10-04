# topology-tongjiuniversity-homework
孩子们imgui不能用中文😠
# Topology Graph Generator

一个基于 OpenGL 的交互式图可视化工具，支持多种图结构生成、力导向布局算法和 SVG 导出。

## Features

多种图类型：随机图、网格图、环形图、星形图
2D/3D 可视化模式
实时力导向布局算法
SVG 矢量图导出
交互式 3D 相机控制
可调节布局参数

## Build Requirements

### Dependencies

- **C++11** or higher
- **CMake** 3.10+ (推荐)
- **OpenGL** 3.3+
- **GLFW** 3.x
- **GLM** (OpenGL Mathematics)
- **GLAD** (OpenGL Loader)
- **Dear ImGui** (GUI)


**基本操作**:
**左键拖拽** - 旋转视角
**滚轮** - 缩放
**WASD** - 平移相机
**QE** - 上下移动
**Regenerate** - 重新生成图
**Export SVG** - 导出当前视图

## Usage Examples

### 生成随机图
1. 选择 "Random Graph"
2. 设置节点数 (20-100)
3. 调整边概率 (0.1-0.3)
4. 点击 "Regenerate"

### 调整布局
**Repulsion Strength**: 100-500 (节点分散度)
**Attraction Strength**: 0.05-0.2 (边的紧密度)
勾选 "Auto Layout" 查看实时效果

### 导出图形
调整到满意的视角
点击 "Export SVG"
文件保存在 `./exports/` 或程序目录
## Project Structure
```
topology-graph-generator/
├── src/
│   ├── main.cpp
│   ├── Graph.h/cpp
│   ├── Camera.h/cpp
│   ├── Renderer.h/cpp
│   └── GuiController.h/cpp
├── external/
│   ├── imgui/
│   ├── glad/
│   └── glm/
├── CMakeLists.txt
└── README.md
```

## Contributing

欢迎 Issues 和 Pull Requests！

## License

WTFPL License

**Version**: 1.0.0
