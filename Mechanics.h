#pragma once
#include "Camera.h"
#include <glm/glm.hpp>
#include <vector>
#include <GLFW/glfw3.h>

// Forward declarations ��� ����� �������
enum EventType;
struct GameEvent;
class EnemyPool; // ���������: forward declaration

class Mechanics
{
public:
	// �����������, ����������� ��������� �� ������
	Mechanics(Camera* camera);

	// === �������� ������ ===

	// ���������� ������ RayIntersectsTriangle
	bool RayIntersectsTriangle(
		const glm::vec3& orig, const glm::vec3& dir,
		const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
		float& t, int targetID = -1);

	// �������� ��������
	bool CheckForShot(GLFWwindow* window);

	// ����������� ������ ��� GLFW
	static void MouseCallback(GLFWwindow* window, double xpos, double ypos);

	// === ������� ������� ===

	// ����� �������
	void RewindTime(float secondsBack);

	// ��������� ������ (��� �������)
	float SetTime(float Delta);

	// === ������� ������� ===

	// ���������� ������� �������� � ����
	void AddPlayerMoveEvent(const glm::vec3& position);
	void AddBulletSpawnEvent(const glm::vec3& position, const glm::vec3& direction, int bulletID);

	// ��� ���������� �������� - �������� ������� ������ � �������
	glm::vec3 GetPlayerPositionAt(float timeBack) const;

	// === ���������� ������� ===

	// ������������ "����� �������"
	void GetTimeTrailPoints(std::vector<glm::vec3>& points, float maxTime = 3.0f) const;

	// �����-������� ��� ��������
	void TriggerGlitchEffect(float intensity);
	float GetGlitchIntensity() const;
	const std::vector<glm::vec3>& GetRewindEffectPositions() const;

	// === ���������� � UI ===

	// �����-������� (��������� ��� ������ �������)
	int GetHitsWithoutRewind() const;
	float GetTimeSinceLastRewind() const;
	int GetEventsInLastSeconds(float seconds) const;

	// === ��������� ������ ===

	// ���������� ������� ������� (������� ������ ����!)
	void Update(float deltaTime);

	// ������� ������ �������
	void CleanupHistory();

	// === �����: ���������� � ENEMYPOOL ===

	// ��������� ������ �� ��� ������
	void SetEnemyPool(EnemyPool* pool) { m_EnemyPool = pool; }

	// ��������� ����� ��� ���������� �������
	void AddEvent(int eventType, const glm::vec3& position, int entityID, const glm::vec3& data = glm::vec3(0));

private:
	// === ������ � ���������� ===
	Camera* m_Camera; // ��������� �� ������ ������

	// ���������� ��� ����
	float m_LastX = 400.0f;
	float m_LastY = 300.0f;
	bool m_FirstMouse = true;

	// === �����: ������ �� ENEMYPOOL ===
	EnemyPool* m_EnemyPool = nullptr; // ��������� �� ��� ������
};