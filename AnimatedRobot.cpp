#include "AnimatedRobot.h"
#include <iostream>
#include <cmath>

// Статические переменные
std::shared_ptr<Model> AnimatedRobot::s_BasicRobotModel = nullptr;
std::shared_ptr<Model> AnimatedRobot::s_HunterRobotModel = nullptr;
bool AnimatedRobot::s_ModelsLoaded = false;

// ----------------------------------------------------------------------
// Конструктор
// ----------------------------------------------------------------------
AnimatedRobot::AnimatedRobot()
	: active(false), hp(100.0f), maxHp(100.0f), position(0.0f),
	speed(2.0f), shootTimer(0.0f), robotType(BASIC_ROBOT),
	animationTime(0.0f), idleAnimSpeed(1.0f), walkAnimSpeed(2.0f),
	lastPosition(0.0f), isMoving(false), glitchAmount(0.0f), colorPulse(0.0f)
{
}

AnimatedRobot::~AnimatedRobot() = default;

// ----------------------------------------------------------------------
// Загрузка моделей (вызывай один раз в начале программы)
// ----------------------------------------------------------------------
bool AnimatedRobot::LoadRobotModel(const std::string& path) {
	if (s_ModelsLoaded) return true;

	try {
		// Пытаемся загрузить основную модель робота
		s_BasicRobotModel = std::make_shared<Model>(path);

		// Для разнообразия можешь загрузить разные модели
		// s_HunterRobotModel = std::make_shared<Model>("assets/hunter_robot.fbx");
		// Но для хакатона можно использовать одну модель с разными цветами
		s_HunterRobotModel = s_BasicRobotModel;

		s_ModelsLoaded = true;
		std::cout << "Robot models loaded successfully from: " << path << std::endl;
		return true;

	}
	catch (const std::exception & e) {
		std::cerr << "Failed to load robot model: " << e.what() << std::endl;
		// Fallback - если не удалось загрузить, используем nullptr
		// Тогда в рендере будем использовать запасной куб
		return false;
	}
}

void AnimatedRobot::CleanupModels() {
	s_BasicRobotModel.reset();
	s_HunterRobotModel.reset();
	s_ModelsLoaded = false;
}

// ----------------------------------------------------------------------
// Основные методы
// ----------------------------------------------------------------------
void AnimatedRobot::Reset(const glm::vec3& spawnPos, RobotType type) {
	active = true;
	robotType = type;

	// Разные параметры для разных типов роботов
	switch (type) {
	case BASIC_ROBOT:
		hp = maxHp = 100.0f;
		speed = 2.0f;
		idleAnimSpeed = 1.0f;
		walkAnimSpeed = 2.0f;
		break;

	case TEMPORAL_HUNTER_ROBOT:
		hp = maxHp = 150.0f; // больше HP
		speed = 1.5f; // медленнее, но опаснее
		idleAnimSpeed = 0.8f;
		walkAnimSpeed = 1.8f;
		glitchAmount = 0.3f; // постоянный глитч-эффект
		break;

	case GLITCH_ROBOT:
		hp = maxHp = 75.0f; // меньше HP
		speed = 3.0f; // быстрее
		idleAnimSpeed = 2.0f;
		walkAnimSpeed = 3.0f;
		break;
	}

	position = spawnPos;
	lastPosition = position;
	shootTimer = 2.0f;
	animationTime = 0.0f;
	isMoving = false;
	colorPulse = 0.0f;
}

void AnimatedRobot::TakeDamage(float dmg) {
	if (!active) return;

	hp -= dmg;

	// Визуальный эффект при получении урона
	colorPulse = 1.0f;
	glitchAmount += 0.2f; // временный глитч при уроне

	if (hp <= 0.0f) {
		hp = 0.0f;
		active = false;
		std::cout << "Robot destroyed!" << std::endl;
	}
}

// ----------------------------------------------------------------------
// Обновление робота
// ----------------------------------------------------------------------
void AnimatedRobot::Update(float deltaTime, const glm::vec3& playerPos) {
	if (!active) return;

	UpdateMovement(deltaTime, playerPos);
	UpdateAnimation(deltaTime);
	UpdateVisualEffects(deltaTime);

	// Система стрельбы
	shootTimer -= deltaTime;
	if (shootTimer <= 0.0f) {
		// Здесь можно добавить логику стрельбы
		// ShootAt(playerPos);
		shootTimer = (robotType == TEMPORAL_HUNTER_ROBOT) ? 1.5f : 2.0f;
	}
}

void AnimatedRobot::UpdateMovement(float deltaTime, const glm::vec3 & playerPos) {
	lastPosition = position;

	// Разное поведение для разных типов
	glm::vec3 targetPos = playerPos;

	if (robotType == TEMPORAL_HUNTER_ROBOT) {
		// Temporal Hunter должен получать позицию игрока из прошлого
		// targetPos = mechanics.GetPlayerPositionAt(1.5f);
		// Пока используем обычную логику
	}

	// Движение к цели
	glm::vec3 direction = targetPos - position;
	float distance = glm::length(direction);

	if (distance > 0.1f) {
		direction = glm::normalize(direction);
		position += direction * speed * deltaTime;
		isMoving = true;
	}
	else {
		isMoving = false;
	}
}

void AnimatedRobot::UpdateAnimation(float deltaTime) {
	// Простая система анимации без скелета
	float animSpeed = isMoving ? walkAnimSpeed : idleAnimSpeed;
	animationTime += deltaTime * animSpeed;

	// Для глитч-робота добавляем хаотичность
	if (robotType == GLITCH_ROBOT) {
		animationTime += sin(animationTime * 10.0f) * 0.1f;
	}
}

void AnimatedRobot::UpdateVisualEffects(float deltaTime) {
	// Затухание эффектов урона
	if (colorPulse > 0.0f) {
		colorPulse -= deltaTime * 3.0f; // затухает за ~0.33 секунды
		if (colorPulse < 0.0f) colorPulse = 0.0f;
	}

	if (glitchAmount > 0.3f && robotType != TEMPORAL_HUNTER_ROBOT) {
		glitchAmount -= deltaTime * 2.0f; // затухающий глитч от урона
	}
}

// ----------------------------------------------------------------------
// Рендеринг и визуализация
// ----------------------------------------------------------------------
glm::mat4 AnimatedRobot::GetModelMatrix() const {
	glm::mat4 model = glm::mat4(1.0f);

	// Базовая позиция
	model = glm::translate(model, position);

	// Простая "анимация" - разные эффекты для разных роботов
	switch (robotType) {
	case BASIC_ROBOT:
		// Простое покачивание при движении
		if (isMoving) {
			model = glm::rotate(model, sin(animationTime * 8.0f) * 0.05f, glm::vec3(0, 0, 1));
		}
		break;

	case TEMPORAL_HUNTER_ROBOT:
		// Медленное вращение + глитч-дрожание
		model = glm::rotate(model, animationTime * 0.3f, glm::vec3(0, 1, 0));
		if (glitchAmount > 0.0f) {
			glm::vec3 glitchOffset = glm::vec3(
				sin(animationTime * 50.0f) * glitchAmount * 0.1f,
				sin(animationTime * 47.0f) * glitchAmount * 0.1f,
				sin(animationTime * 53.0f) * glitchAmount * 0.1f
			);
			model = glm::translate(model, glitchOffset);
		}
		break;

	case GLITCH_ROBOT:
		// Хаотичное движение
		model = glm::rotate(model, sin(animationTime * 15.0f) * 0.2f, glm::vec3(1, 0, 0));
		model = glm::rotate(model, cos(animationTime * 12.0f) * 0.1f, glm::vec3(0, 0, 1));
		break;
	}

	// Размер (можно варьировать для разных типов)
	float scale = (robotType == TEMPORAL_HUNTER_ROBOT) ? 0.7f : 0.5f;
	model = glm::scale(model, glm::vec3(scale));

	return model;
}

glm::vec4 AnimatedRobot::GetColor() const {
	if (!active) return glm::vec4(0.3f, 0.3f, 0.3f, 0.5f);

	glm::vec3 baseColor = GetBaseColor();

	// Эффект урона (красная вспышка)
	if (colorPulse > 0.0f) {
		baseColor = glm::mix(baseColor, glm::vec3(1.0f, 0.0f, 0.0f), colorPulse * 0.7f);
	}

	// Пульсация здоровья
	float healthPercent = hp / maxHp;
	float alpha = 0.7f + healthPercent * 0.3f;

	return glm::vec4(baseColor, alpha);
}

glm::vec3 AnimatedRobot::GetBaseColor() const {
	switch (robotType) {
	case BASIC_ROBOT:
		return glm::vec3(0.8f, 0.3f, 0.3f); // красный

	case TEMPORAL_HUNTER_ROBOT:
		// Мерцающий фиолетовый с глитч-эффектом
		float glitchPulse = sin(animationTime * 10.0f) * 0.3f + 0.7f;
		return glm::vec3(0.8f * glitchPulse, 0.2f, 0.8f * glitchPulse);

	case GLITCH_ROBOT:
		// Хаотично меняющийся цвет
		return glm::vec3(
			0.5f + sin(animationTime * 7.0f) * 0.3f,
			0.5f + sin(animationTime * 11.0f) * 0.3f,
			0.5f + sin(animationTime * 13.0f) * 0.3f
		);

	default:
		return glm::vec3(0.5f, 0.5f, 0.5f);
	}
}

Model* AnimatedRobot::GetCurrentModel() const {
	switch (robotType) {
	case TEMPORAL_HUNTER_ROBOT:
		return s_HunterRobotModel.get();
	default:
		return s_BasicRobotModel.get();
	}
}