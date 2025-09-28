
#include "Matrices.h"


Matrices::Matrices(GLFWwindow* window, float T)
{
	/**/
}


glm::mat4 createProjectionMat(GLFWwindow* window)
{
	int w = 0, h = 0;
	glfwGetWindowSize(window, (int*)& w, (int*)& h);
	float AspectRatio = static_cast<float>(w) / static_cast<float>(h);
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), AspectRatio, 0.1f, 100.0f);
	return projection;
}