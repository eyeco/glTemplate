# cvglTemplate
template for c++ OpenGL applications for quick technical 3D-visualizations (and beyond)

NOTE: wip, description will follow

## Prerequisites
best use vcpkg to install dependencies
- glm
- glew
- freeglut
- imgui
- cxxopts

```
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
bootstrap-vcpkg.bat
vcpkg integrate install
vcpkg install glm:x86-windows glm:x64-windows glew:x86-windows glew:x64-windows freglut:x86-windows freeglut:x64-windows imgui:x86-windows imgui:x64-windows imgui[opengl2-binding]:x86-windows imgui[opengl2-binding]:x64-windows imgui[glut-binding]:x86-windows imgui[glut-binding]:x64-windows cxxopts:x86-windows cxxopts:x64-windows
```

## How to build

## TODO
- get rid of freeGLUT, replace by glfw