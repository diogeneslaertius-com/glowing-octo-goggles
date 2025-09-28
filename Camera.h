#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

class Camera {
public:
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f));

	// Основные методы
	glm::mat4 ProcessInput(GLFWwindow* window, float deltaTime);
	glm::mat4 GetProjectionMatrix(GLFWwindow* window, float fov, float nearPlane, float farPlane);
	void ProcessMouseMovement(float xOffset, float yOffset, bool constrainPitch = true);

	// Геттеры и сеттеры для FPS контроллера
	glm::vec3 GetPosition() const { return position; }
	glm::vec3 GetFront() const { return front; }
	glm::vec3 GetUp() const { return up; }
	glm::vec3 GetRight() const { return right; }

	void SetPosition(const glm::vec3& pos);

	// Параметры камеры
	float MovementSpeed = 5.0f;
	float MouseSensitivity = 0.1f;
	float Zoom = 45.0f;

private:
	// Векторы камеры
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;

	// Углы Эйлера
	float yaw = -90.0f;   // инициализируем на -90 градусов (смотрим вперед)
	float pitch = 0.0f;

	void updateCameraVectors();
};