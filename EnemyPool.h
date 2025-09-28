// EnemyPool.h
#pragma once
#include "Enemy.h"
#include <vector>

struct EnemyState {
	bool active;
	float hp;
	glm::vec3 position;
	int enemyIndex;

	EnemyState() : active(false), hp(0.0f), position(0.0f), enemyIndex(-1) {}
	EnemyState(const Enemy& enemy, int index)
		: active(enemy.IsActive()), hp(enemy.GetHP()),
		position(enemy.GetPosition()), enemyIndex(index) {}
};

struct EnemyPoolSnapshot {
	float timestamp;
	std::vector<EnemyState> enemyStates;

	EnemyPoolSnapshot() : timestamp(0.0f) {}
};

class EnemyPool {
public:
	EnemyPool(size_t size) {
		enemies.resize(size);
		stateHistory.reserve(300); // ~10 секунд при 30 FPS
	}

	Enemy* Spawn(const glm::vec3& pos);
	void UpdateAll(float deltaTime, const glm::vec3& playerPos);
	void SpawnWaveCircle(int count, const glm::vec3& center, float radius, float jitter = 0.5f);

	// === НОВЫЕ МЕТОДЫ ДЛЯ СИСТЕМЫ ВРЕМЕНИ ===
	void SaveCurrentState(float timestamp);
	bool RestoreStateAt(float targetTime);
	void CleanOldStates(float maxHistoryTime = 10.0f);

	// Геттеры
	std::vector<Enemy>& GetEnemies() { return enemies; }
	const std::vector<Enemy>& GetEnemies() const { return enemies; }

	int GetActiveCount() const;
	void DebugPrintHistory() const;

private:
	std::vector<Enemy> enemies;
	std::vector<EnemyPoolSnapshot> stateHistory;

	EnemyPoolSnapshot CreateSnapshot(float timestamp) const;
	void ApplySnapshot(const EnemyPoolSnapshot& snapshot);
};

