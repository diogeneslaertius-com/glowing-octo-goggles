#include "Render.h"

void Renderer::Clear(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

void Renderer::Draw(GLenum mode, int count)
{
	glDrawArrays(mode, 0, count);
}

bool Renderer::CheckError()
{
	bool foundError = false;
	GLenum glerr = glGetError();
	while (glerr != GL_NO_ERROR)
	{
		foundError = true;
		std::cerr << std::endl << "[GL ERROR] FOUNDED ERROR -->" << glerr << std::endl;
		glerr = glGetError();
	}
	return foundError;
}

void Renderer::InitGLSettings()
{
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

void Renderer::UpdateViewport(GLFWwindow * window)
{
	int w = 0, h = 0;
	glfwGetWindowSize(window, &w, &h);
	if (h == 0) h = 1;

	glViewport(0, 0, w, h);
}