#pragma once
#pragma once
#include "AnimatedRobot.h"
#include <vector>

class RobotPool {
public:
	RobotPool(size_t size) { robots.resize(size); }

	// ����� ������
	AnimatedRobot* Spawn(const glm::vec3& pos, RobotType type = BASIC_ROBOT);

	// ���������� ���� �������
	void UpdateAll(float deltaTime, const glm::vec3& playerPos);

	// ��������� ���� ������� (�����������)
	void DrawAll();

	// ����� ����� ������� �� �����
	void SpawnWaveCircle(int count, const glm::vec3& center, float radius,
		float hunterChance = 0.2f, float jitter = 0.5f);

	// ��������� ���������� �������� �������
	int GetActiveCount() const;

	// ������ � �������
	std::vector<AnimatedRobot>& GetRobots() { return robots; }
	const std::vector<AnimatedRobot>& GetRobots() const { return robots; }

	// ������������� � �������
	static bool Initialize(const std::string& robotModelPath);
	static void Cleanup();

private:
	std::vector<AnimatedRobot> robots;
};
