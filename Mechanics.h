#pragma once
#include "Camera.h"
#include <glm/glm.hpp>
#include <vector>
#include <GLFW/glfw3.h>

// Forward declarations для типов событий
enum EventType;
struct GameEvent;
class EnemyPool; // ДОБАВЛЕНО: forward declaration

class Mechanics
{
public:
	// Конструктор, принимающий указатель на камеру
	Mechanics(Camera* camera);

	// === ОСНОВНЫЕ МЕТОДЫ ===

	// Улучшенная версия RayIntersectsTriangle
	bool RayIntersectsTriangle(
		const glm::vec3& orig, const glm::vec3& dir,
		const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
		float& t, int targetID = -1);

	// Проверка стрельбы
	bool CheckForShot(GLFWwindow* window);

	// Статический колбэк для GLFW
	static void MouseCallback(GLFWwindow* window, double xpos, double ypos);

	// === СИСТЕМА ВРЕМЕНИ ===

	// Откат времени
	void RewindTime(float secondsBack);

	// Временной эффект (для шейдера)
	float SetTime(float Delta);

	// === СИСТЕМА СОБЫТИЙ ===

	// Добавление событий движения и пуль
	void AddPlayerMoveEvent(const glm::vec3& position);
	void AddBulletSpawnEvent(const glm::vec3& position, const glm::vec3& direction, int bulletID);

	// Для Временного Охотника - получить позицию игрока в прошлом
	glm::vec3 GetPlayerPositionAt(float timeBack) const;

	// === ВИЗУАЛЬНЫЕ ЭФФЕКТЫ ===

	// Визуализация "следа времени"
	void GetTimeTrailPoints(std::vector<glm::vec3>& points, float maxTime = 3.0f) const;

	// Глитч-эффекты для шейдеров
	void TriggerGlitchEffect(float intensity);
	float GetGlitchIntensity() const;
	const std::vector<glm::vec3>& GetRewindEffectPositions() const;

	// === СТАТИСТИКА И UI ===

	// Комбо-система (попадания без отмены времени)
	int GetHitsWithoutRewind() const;
	float GetTimeSinceLastRewind() const;
	int GetEventsInLastSeconds(float seconds) const;

	// === СИСТЕМНЫЕ МЕТОДЫ ===

	// Обновление системы времени (вызывай каждый кадр!)
	void Update(float deltaTime);

	// Очистка старых событий
	void CleanupHistory();

	// === НОВОЕ: ИНТЕГРАЦИЯ С ENEMYPOOL ===

	// Установка ссылки на пул врагов
	void SetEnemyPool(EnemyPool* pool) { m_EnemyPool = pool; }

	// Публичный метод для добавления событий
	void AddEvent(int eventType, const glm::vec3& position, int entityID, const glm::vec3& data = glm::vec3(0));

private:
	// === КАМЕРА И УПРАВЛЕНИЕ ===
	Camera* m_Camera; // указатель на объект камеры

	// Переменные для мыши
	float m_LastX = 400.0f;
	float m_LastY = 300.0f;
	bool m_FirstMouse = true;

	// === НОВОЕ: ССЫЛКА НА ENEMYPOOL ===
	EnemyPool* m_EnemyPool = nullptr; // указатель на пул врагов
};