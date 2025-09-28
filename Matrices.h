#pragma once

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Matrices
{
	Matrices(GLFWwindow* window, float T);
private:
	
	glm::mat4 createProjectionMat(GLFWwindow* window);
	
};
