
#include "Beam.h"
#include <cmath>
#include <algorithm>

Beam::Beam(const glm::vec3& origin, const glm::vec3& direction,
	float maxLength, float duration, const glm::vec3& color)
	: startPos(origin), baseColor(color), maxLifetime(duration),
	lifetime(duration), intensity(1.0f), width(0.1f) {

	// Вычисляем конечную точку луча
	endPos = origin + glm::normalize(direction) * maxLength;

	setupMesh();
}

Beam::~Beam() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void Beam::Update(float deltaTime) {
	lifetime -= deltaTime;

	// Обновляем интенсивность (затухание)
	float lifePercent = lifetime / maxLifetime;
	intensity = lifePercent;

	// Эффект пульсации
	float pulse = sin(((maxLifetime - lifetime) * 20.0f)) * 0.3f + 0.7f;
	intensity *= pulse;

	// Обновляем ширину луча (расширяется при затухании)
	width = 0.05f + (1.0f - lifePercent) * 0.15f;

	updateMesh();
}

void Beam::Draw() {
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6); // два треугольника = прямоугольник
	glBindVertexArray(0);
}

glm::mat4 Beam::GetModelMatrix() const {
	return glm::mat4(1.0f); // луч позиционируется через vertices
}

glm::vec3 Beam::GetColor() const {
	return baseColor * intensity;
}

void Beam::setupMesh() {
	// Создаем прямоугольник для луча
	vertices.resize(18); // 6 вершин * 3 координаты

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Настраиваем атрибуты вершин
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	updateMesh();
}

void Beam::updateMesh() {
	// Вычисляем направление и перпендикуляр для ширины луча
	glm::vec3 direction = glm::normalize(endPos - startPos);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	// Если луч направлен вертикально, используем другой вектор
	if (abs(glm::dot(direction, up)) > 0.99f) {
		up = glm::vec3(1.0f, 0.0f, 0.0f);
	}

	glm::vec3 right = glm::normalize(glm::cross(direction, up));
	glm::vec3 widthVector = right * width;

	// Создаем 6 вершин для прямоугольника (2 треугольника)
	glm::vec3 v1 = startPos - widthVector;  // левый старт
	glm::vec3 v2 = startPos + widthVector;  // правый старт
	glm::vec3 v3 = endPos + widthVector;    // правый конец
	glm::vec3 v4 = endPos - widthVector;    // левый конец

	// Треугольник 1: v1, v2, v3
	vertices[0] = v1.x; vertices[1] = v1.y; vertices[2] = v1.z;
	vertices[3] = v2.x; vertices[4] = v2.y; vertices[5] = v2.z;
	vertices[6] = v3.x; vertices[7] = v3.y; vertices[8] = v3.z;

	// Треугольник 2: v1, v3, v4
	vertices[9] = v1.x; vertices[10] = v1.y; vertices[11] = v1.z;
	vertices[12] = v3.x; vertices[13] = v3.y; vertices[14] = v3.z;
	vertices[15] = v4.x; vertices[16] = v4.y; vertices[17] = v4.z;

	// Обновляем буфер
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
		vertices.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}