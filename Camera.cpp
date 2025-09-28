// Camera.cpp (основные методы)
#include "Camera.h"

Camera::Camera(glm::vec3 pos)
	: position(pos), worldUp(glm::vec3(0.0f, 1.0f, 0.0f)), yaw(-90.0f), pitch(0.0f) {
	updateCameraVectors();
}

void Camera::SetPosition(const glm::vec3& pos) {
	position = pos;
}

glm::mat4 Camera::ProcessInput(GLFWwindow* window, float deltaTime) {
	// Обрабатываем клавиатурный ввод только для поворота камеры
	// Движение теперь контролируется через Player класс

	return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::GetProjectionMatrix(GLFWwindow* window, float fov, float nearPlane, float farPlane) {
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	if (height == 0) height = 1;

	float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
	return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
}

void Camera::ProcessMouseMovement(float xOffset, float yOffset, bool constrainPitch) {
	xOffset *= MouseSensitivity;
	yOffset *= MouseSensitivity;

	yaw += xOffset;
	pitch += yOffset;

	if (constrainPitch) {
		if (pitch > 89.0f) pitch = 89.0f;
		if (pitch < -89.0f) pitch = -89.0f;
	}

	updateCameraVectors();
}

void Camera::updateCameraVectors() {
	glm::vec3 frontTemp;
	frontTemp.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	frontTemp.y = sin(glm::radians(pitch));
	frontTemp.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	front = glm::normalize(frontTemp);
	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));
}