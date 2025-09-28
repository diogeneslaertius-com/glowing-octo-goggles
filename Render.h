#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

class Renderer
{
public:
	static void Clear(float r, float g, float b, float a);
	static void Draw(GLenum mode, int count);
	static bool CheckError();
	static void InitGLSettings();

	// Ваша функция ProjectionMatrix (Viewport)
	static void UpdateViewport(GLFWwindow* window);
};