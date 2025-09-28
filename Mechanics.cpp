#define GLM_ENABLE_EXPERIMENTAL
#include "Mechanics.h"
#include "EnemyPool.h"  // ДОБАВЛЕНО для исправления ошибки C2027
#include "Camera.h"     // ДОБАВЛЕНО (если нужно)
#include <glm/gtx/intersect.hpp>
#include <iostream>
#include <algorithm>

// ----------------------------------------------------------------------
// Структуры для системы времени
// ----------------------------------------------------------------------
enum EventType {
	HIT,           // Попадание по врагу
	DEATH,         // Смерть врага
	PLAYER_MOVE,   // Движение игрока
	BULLET_SPAWN   // Создание пули
};

struct GameEvent {
	float time;
	EventType type;
	glm::vec3 position;
	int entityID;
	glm::vec3 additionalData; // направление, скорость и т.д.

	GameEvent(float t, EventType tp, glm::vec3 pos, int id, glm::vec3 data = glm::vec3(0))
		: time(t), type(tp), position(pos), entityID(id), additionalData(data) {}
};

// Глобальные переменные для системы времени
static std::vector<GameEvent> g_EventHistory;
static float g_LastRewindTime = -999.0f;
static int g_HitsWithoutRewind = 0;
static float g_GlitchIntensity = 0.0f;
static float g_GlitchTimer = 0.0f;
static std::vector<glm::vec3> g_RewindEffectPositions;
static float g_LastPlayerMoveEvent = -1.0f;

const float MAX_HISTORY_TIME = 10.0f; // 10 секунд истории максимум

// ----------------------------------------------------------------------
// Конструктор
// ----------------------------------------------------------------------
Mechanics::Mechanics(Camera* camera)
	: m_Camera(camera), m_EnemyPool(nullptr)  // ДОБАВЛЕНО инициализацию m_EnemyPool
{
	g_EventHistory.reserve(1000); // предзагружаем память
}

// ----------------------------------------------------------------------
// Обработка мыши (без изменений)
// ----------------------------------------------------------------------
void Mechanics::MouseCallback(GLFWwindow* window, double xposIn, double yposIn)
{
	Mechanics* handler = static_cast<Mechanics*>(glfwGetWindowUserPointer(window));
	if (!handler || !handler->m_Camera) return;

	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (handler->m_FirstMouse)
	{
		handler->m_LastX = xpos;
		handler->m_LastY = ypos;
		handler->m_FirstMouse = false;
	}

	float xoffset = xpos - handler->m_LastX;
	float yoffset = handler->m_LastY - ypos;

	handler->m_LastX = xpos;
	handler->m_LastY = ypos;

	handler->m_Camera->ProcessMouseMovement(xoffset, yoffset);
}

// ----------------------------------------------------------------------
// Проверка стрельбы (без изменений)
// ----------------------------------------------------------------------
bool Mechanics::CheckForShot(GLFWwindow* window)
{
	return glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
}

// ----------------------------------------------------------------------
// Улучшенный рейкаст с системой событий
// ----------------------------------------------------------------------
bool Mechanics::RayIntersectsTriangle(
	const glm::vec3& orig, const glm::vec3& dir,
	const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
	float& t, int targetID)
{
	const float EPSILON = 0.0000001f;
	glm::vec3 edge1 = v1 - v0;
	glm::vec3 edge2 = v2 - v0;
	glm::vec3 h = glm::cross(dir, edge2);
	float a = glm::dot(edge1, h);

	if (fabs(a) < EPSILON) return false;

	float f = 1.0f / a;
	glm::vec3 s = orig - v0;
	float u = f * glm::dot(s, h);
	if (u < 0.0f || u > 1.0f) return false;

	glm::vec3 q = glm::cross(s, edge1);
	float v = f * glm::dot(dir, q);
	if (v < 0.0f || u + v > 1.0f) return false;

	t = f * glm::dot(edge2, q);
	if (t > EPSILON)
	{
		glm::vec3 hitPoint = orig + dir * t;

		// Добавляем событие попадания в историю
		AddEvent(EventType::HIT, hitPoint, targetID, dir);
		g_HitsWithoutRewind++;

		std::cout << "Hit registered at: " << hitPoint.x << ", "
			<< hitPoint.y << ", " << hitPoint.z
			<< " (combo: " << g_HitsWithoutRewind << ")\n";

		return true;
	}
	return false;
}

// ----------------------------------------------------------------------
// Система событий
// ----------------------------------------------------------------------
void Mechanics::AddEvent(int eventType, const glm::vec3& position, int entityID, const glm::vec3& data)
{
	EventType type = static_cast<EventType>(eventType);
	float currentTime = static_cast<float>(glfwGetTime());
	g_EventHistory.emplace_back(currentTime, type, position, entityID, data);
}

void Mechanics::AddPlayerMoveEvent(const glm::vec3& position)
{
	float currentTime = static_cast<float>(glfwGetTime());

	// Добавляем только каждые 0.1 секунды для оптимизации
	if (currentTime - g_LastPlayerMoveEvent > 0.1f) {
		AddEvent(EventType::PLAYER_MOVE, position, -1);
		g_LastPlayerMoveEvent = currentTime;
	}
}

void Mechanics::AddBulletSpawnEvent(const glm::vec3 & position, const glm::vec3 & direction, int bulletID)
{
	AddEvent(EventType::BULLET_SPAWN, position, bulletID, direction);
}

// ----------------------------------------------------------------------
// Улучшенный откат времени
// ----------------------------------------------------------------------
void Mechanics::RewindTime(float secondsBack)
{
	float currentTime = static_cast<float>(glfwGetTime());
	float cutoff = currentTime - secondsBack;

	g_RewindEffectPositions.clear();
	int rewindedEvents = 0;
	std::vector<int> revivedEnemies;

	std::cout << "=== REWINDING TIME ===\n";
	std::cout << "Current time: " << currentTime << ", Cutoff: " << cutoff << "\n";

	// ДОБАВЛЕНА ПРОВЕРКА НА NULLPTR для безопасности
	if (!m_EnemyPool) {
		std::cerr << "ERROR: EnemyPool not set in Mechanics! Call SetEnemyPool() first.\n";
		// Можно продолжить работу без восстановления врагов
	}

	// Проходим события в обратном порядке
	for (auto it = g_EventHistory.rbegin(); it != g_EventHistory.rend(); ++it) {
		if (it->time < cutoff) break;

		switch (it->type) {
		case EventType::HIT: {  // ДОБАВЛЕНЫ ФИГУРНЫЕ СКОБКИ для исправления потенциальных проблем
			std::cout << "Rewinding HIT at (" << it->position.x << ", "
				<< it->position.y << ", " << it->position.z << ")\n";

			// РЕАЛЬНОЕ ВОССТАНОВЛЕНИЕ ВРАГА
			if (m_EnemyPool && it->entityID >= 0) {
				auto& enemies = m_EnemyPool->GetEnemies();
				if (it->entityID < static_cast<int>(enemies.size())) {
					enemies[it->entityID].Reset(it->position);
					revivedEnemies.push_back(it->entityID);
					std::cout << "Revived enemy " << it->entityID << "\n";
				}
			}

			g_RewindEffectPositions.push_back(it->position);
			rewindedEvents++;
			break;
		}

		case EventType::DEATH: {  // ДОБАВЛЕНЫ ФИГУРНЫЕ СКОБКИ
			std::cout << "Rewinding DEATH of entity " << it->entityID << "\n";
			// Аналогично для смерти
			if (m_EnemyPool && it->entityID >= 0) {
				auto& enemies = m_EnemyPool->GetEnemies();
				if (it->entityID < static_cast<int>(enemies.size())) {
					enemies[it->entityID].Reset(it->position);
					revivedEnemies.push_back(it->entityID);
				}
			}
			break;
		}

		case EventType::BULLET_SPAWN: {  // ДОБАВЛЕНЫ ФИГУРНЫЕ СКОБКИ
			std::cout << "Rewinding BULLET spawn\n";
			// removeBullet(it->entityID); // можешь добавить позже
			break;
		}

		default:
			break;
		}
	}

	// Визуальные эффекты
	if (rewindedEvents > 0) {
		TriggerGlitchEffect(rewindedEvents * 0.3f);
		std::cout << "Triggered glitch effect. Revived " << revivedEnemies.size() << " enemies.\n";
	}

	// Сброс комбо системы
	g_LastRewindTime = currentTime;
	g_HitsWithoutRewind = 0;

	// Удаляем отмененные события из истории
	g_EventHistory.erase(
		std::remove_if(g_EventHistory.begin(), g_EventHistory.end(),
			[cutoff](const GameEvent & e) { return e.time >= cutoff; }),
		g_EventHistory.end()
	);

	std::cout << "Events remaining in history: " << g_EventHistory.size() << "\n";
	std::cout << "=== REWIND COMPLETE ===\n";
}

// Для Временного Охотника - получить позицию игрока в прошлом
glm::vec3 Mechanics::GetPlayerPositionAt(float timeBack) const
{
	float currentTime = static_cast<float>(glfwGetTime());
	float targetTime = currentTime - timeBack;

	glm::vec3 closestPosition = glm::vec3(0);
	float closestTimeDiff = 999.0f;

	for (const auto& event : g_EventHistory) {
		if (event.type == EventType::PLAYER_MOVE) {
			float timeDiff = fabs(event.time - targetTime);
			if (timeDiff < closestTimeDiff) {
				closestTimeDiff = timeDiff;
				closestPosition = event.position;
			}
		}
	}

	return closestPosition;
}

// ----------------------------------------------------------------------
// Визуализация "следа времени"
// ----------------------------------------------------------------------
void Mechanics::GetTimeTrailPoints(std::vector<glm::vec3> & points, float maxTime) const
{
	points.clear();
	float currentTime = static_cast<float>(glfwGetTime());
	float cutoff = currentTime - maxTime;

	for (const auto& event : g_EventHistory) {
		if (event.time < cutoff) continue;
		if (event.type == EventType::HIT) {
			points.push_back(event.position);
		}
	}
}

// ----------------------------------------------------------------------
// Система глитч-эффектов
// ----------------------------------------------------------------------
void Mechanics::TriggerGlitchEffect(float intensity)
{
	g_GlitchIntensity = std::min(intensity, 1.0f);
	g_GlitchTimer = 0.3f; // эффект длится 0.3 секунды
}

float Mechanics::GetGlitchIntensity() const
{
	return g_GlitchIntensity;
}

const std::vector<glm::vec3>& Mechanics::GetRewindEffectPositions() const
{
	return g_RewindEffectPositions;
}

// ----------------------------------------------------------------------
// Статистика и геттеры
// ----------------------------------------------------------------------
int Mechanics::GetHitsWithoutRewind() const
{
	return g_HitsWithoutRewind;
}

float Mechanics::GetTimeSinceLastRewind() const
{
	float currentTime = static_cast<float>(glfwGetTime());
	return currentTime - g_LastRewindTime;
}

int Mechanics::GetEventsInLastSeconds(float seconds) const
{
	float currentTime = static_cast<float>(glfwGetTime());
	float cutoff = currentTime - seconds;
	return std::count_if(g_EventHistory.begin(), g_EventHistory.end(),
		[cutoff](const GameEvent & e) { return e.time >= cutoff; });
}

// Обновление системы (вызывай в основном цикле!)
void Mechanics::Update(float deltaTime)
{
	// Обновляем glitch эффект
	if (g_GlitchTimer > 0) {
		g_GlitchTimer -= deltaTime;
		g_GlitchIntensity = g_GlitchTimer / 0.3f; // затухание за 0.3 сек
	}
	else {
		g_GlitchIntensity = 0.0f;
	}

	// Очистка старых эффектов
	static float effectTimer = 0.0f;
	if (!g_RewindEffectPositions.empty()) {
		effectTimer += deltaTime;
		if (effectTimer > 1.0f) { // эффекты видны 1 секунду
			g_RewindEffectPositions.clear();
			effectTimer = 0.0f;
		}
	}

	// Периодическая очистка истории
	static float cleanupTimer = 0.0f;
	cleanupTimer += deltaTime;
	if (cleanupTimer > 5.0f) { // каждые 5 секунд
		CleanupHistory();
		cleanupTimer = 0.0f;
	}
}

// ----------------------------------------------------------------------
// Очистка старых событий
// ----------------------------------------------------------------------
void Mechanics::CleanupHistory()
{
	float currentTime = static_cast<float>(glfwGetTime());
	float cutoff = currentTime - MAX_HISTORY_TIME;

	size_t oldSize = g_EventHistory.size();
	g_EventHistory.erase(
		std::remove_if(g_EventHistory.begin(), g_EventHistory.end(),
			[cutoff](const GameEvent & e) { return e.time < cutoff; }),
		g_EventHistory.end()
	);

	size_t removed = oldSize - g_EventHistory.size();
	if (removed > 0) {
		std::cout << "Cleaned up " << removed << " old events. Remaining: " << g_EventHistory.size() << "\n";
	}
}

// Временной эффект
float Mechanics::SetTime(float Delta)
{
	float period = fmod(Delta, 5.0f);
	if (period < 0.5f)
	{
		return 1.0f - (period / 0.5f);
	}
	return 0.0f;
}