<!--
incase i forget abt the project
# ORE
OpenGL Rendering Engine

![GitHub Workflow Status](https://img.shields.io/github/actions/workflow/status/AnnoyingB/ORE/codeql.yml) ![GitHub code size in bytes](https://img.shields.io/github/languages/code-size/AnnoyingB/ORE) ![GitHub](https://img.shields.io/github/license/AnnoyingB/ORE)

## Build commands:
`--extensions=oreml` This enables OREML (OpenGL Rendering Engine Model Loader)

## ORE::Window
Creates a window using GLFW using the `Create()` function. Use `ShouldClose()` in a while loop to check if it is finished. To initialize OpenGL use `InitGlad()`. To clear the window use `ClearColor()`. If you would like to change the viewport size, use the `SetSizeCallback()` function. To handle all the events use the `Poll()` function. If you want your own cursor, call `SetCursor()`. `xhot` is the point where you can click buttons, links etc. `GetWindowSize` returns a Vector2 with the width and height of the window.

Here is a simple setup using the window (from the `main.cpp` file):
```c++
#include "orepch.h"
#include "Window.h"

void WindowSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

int main() {
	ORE::Window window;
	window.Create("ORE", { 1280, 720 });
	window.InitGlad();
	window.SetSizeCallback(WindowSizeCallback);

	while (!window.ShouldClose()) {
		window.ClearColor(1, 0, 0, 1);

		// Render

		window.Poll();
	}
	return 0;
}
```

## ORE::Buffer
The base class for all the buffer classes. All buffer classes automatically delete the buffer in the destructor.

## ORE::VertexBuffer
This is specifically for holding vertices. This is not for custom values. The constructor automatically creates the VBO. Use `ChangeData()` to change the value of the VBO. You can also add and remove vertices from the vector and call `ChangeData()` with no parameters and it will use vertices.

## ORE::IndexBuffer
Same as the VertexBuffer but for indices. These are stored as unsigned integers.

## ORE::VertexArray
Do NOT use `ChangeData()` on the VAO. This will give an assert. Instead use `SetData()`. Before you call this, bind the VBO and IBO. Only call this once for performance.

## ORE::Framebuffer
Call the constructor and it will create a framebuffer for you. If you want to resize it call `Create()` with the size you want. Use `ReadPixel` to get the pixel color on the given x and y or use `ReadPixelID` to get the id of the pixel's mesh at given coordinates.

## ORE::Shader
In the constructor you give it a path to the file without the extension. It will get the file with the `.orefs` (fragment shader ) and `.orevs` (vertex shader) prefix. You can bind it but not unbind it as you don't need to. You can set the uniforms in the `SetXXX()` function with the value and name.

## ORE::Mesh
A mesh creates all the buffers and shaders required to render onto the screen and with the `+` operator you can merge two meshes together for performance. The first mesh will be the merged mesh and the second mesh is the same. You can get any private variables with the `GetXXX()` function or `GetConstXXX()` function. Both do the same thing but the const function is read-only. To change position/rotation, modify the ModelMatrix with either `glm::rotate`, `glm::translate` or `glm::scale`.

## ORE::Renderer
Call `Init` to initialize the renderer. To create a mesh call `CreateMesh` with the mesh create info. This will add it to an array where calling `RenderMeshes` will render all the meshes and give you the amount of draw calls in the `RenderCalls` variable. Don't forget to call the `Cleanup` function to destroy all the meshes and framebuffers.

## ORE::Camera
A camera is automatically created when you initialize the Renderer. Call `GetView` or `GetProjection` to get the view or projection matrix. To set the aspect according to screen width and height, call `SetAspect` with the width and height of the screen. `Fovy` is the Fov in degrees (it gets converted to radians in `GetProjection`). Toggle `Ortho` on or off for an orthographic projection. Set the width, height and position variables or it will not work.

## ORE::Texture
Create a texture with the texture create info struct.
-->
just learning opengl