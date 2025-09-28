#include "EnemyPool.h"
#include <glm/gtc/constants.hpp>
#include <random>
#include <algorithm>
#include <iostream>

Enemy* EnemyPool::Spawn(const glm::vec3& pos) {
	for (auto& e : enemies) {
		if (!e.IsActive()) {
			e.Reset(pos);
			return &e;
		}
	}
	return nullptr;
}

void EnemyPool::UpdateAll(float deltaTime, const glm::vec3& playerPos) {
	for (auto& e : enemies) {
		if (!e.IsActive()) continue;
		e.Update(deltaTime, playerPos);
	}
}

void EnemyPool::SpawnWaveCircle(int count, const glm::vec3& center, float radius, float jitter) {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_real_distribution<float> jitterDis(-jitter, jitter);

	for (int i = 0; i < count; i++) {
		float angle = (float)i / count * 2.0f * glm::pi<float>();
		float actualRadius = radius + jitterDis(gen);
		angle += jitterDis(gen) * 0.5f;

		glm::vec3 spawnPos = center + glm::vec3(
			cos(angle) * actualRadius,
			0.0f,
			sin(angle) * actualRadius
		);

		Spawn(spawnPos);
	}
}

void EnemyPool::SaveCurrentState(float timestamp) {
	EnemyPoolSnapshot snapshot = CreateSnapshot(timestamp);
	stateHistory.push_back(snapshot);

	// Ограничиваем размер истории
	if (stateHistory.size() > 300) {
		stateHistory.erase(stateHistory.begin());
	}
}

bool EnemyPool::RestoreStateAt(float targetTime) {
	if (stateHistory.empty()) return false;

	// Ищем ближайший снимок к целевому времени
	EnemyPoolSnapshot* bestSnapshot = nullptr;
	float bestTimeDiff = 999999.0f;

	for (auto& snapshot : stateHistory) {
		float timeDiff = abs(snapshot.timestamp - targetTime);
		if (timeDiff < bestTimeDiff) {
			bestTimeDiff = timeDiff;
			bestSnapshot = &snapshot;
		}
	}

	if (bestSnapshot && bestTimeDiff < 1.0f) { // максимум 1 секунда разницы
		ApplySnapshot(*bestSnapshot);

		std::cout << "Restored enemy state to time: " << bestSnapshot->timestamp
			<< " (diff: " << bestTimeDiff << "s)" << std::endl;

		return true;
	}

	return false;
}

void EnemyPool::CleanOldStates(float maxHistoryTime) {
	if (stateHistory.empty()) return;

	float currentTime = stateHistory.back().timestamp;
	float cutoffTime = currentTime - maxHistoryTime;

	auto removeIt = std::remove_if(stateHistory.begin(), stateHistory.end(),
		[cutoffTime](const EnemyPoolSnapshot & snapshot) {
			return snapshot.timestamp < cutoffTime;
		});

	size_t removed = std::distance(removeIt, stateHistory.end());
	if (removed > 0) {
		stateHistory.erase(removeIt, stateHistory.end());
		std::cout << "Cleaned " << removed << " old enemy states" << std::endl;
	}
}

int EnemyPool::GetActiveCount() const {
	int count = 0;
	for (const auto& e : enemies) {
		if (e.IsActive()) count++;
	}
	return count;
}

EnemyPoolSnapshot EnemyPool::CreateSnapshot(float timestamp) const {
	EnemyPoolSnapshot snapshot;
	snapshot.timestamp = timestamp;

	for (size_t i = 0; i < enemies.size(); ++i) {
		snapshot.enemyStates.emplace_back(enemies[i], static_cast<int>(i));
	}

	return snapshot;
}

void EnemyPool::ApplySnapshot(const EnemyPoolSnapshot& snapshot) {
	// Применяем состояние к каждому врагу
	for (const auto& state : snapshot.enemyStates) {
		if (state.enemyIndex >= 0 && state.enemyIndex < static_cast<int>(enemies.size())) {
			Enemy& enemy = enemies[state.enemyIndex];

			if (state.active) {
				// Восстанавливаем активного врага
				if (!enemy.IsActive()) {
					enemy.Reset(state.position); // активируем
				}
				// TODO: Установить HP (нужно добавить метод SetHP в Enemy)
				// enemy.SetHP(state.hp);
			}
			else {
				// Деактивируем врага если он должен быть неактивным
				if (enemy.IsActive()) {
					enemy.Deactivate();
				}
			}
		}
	}
}

void EnemyPool::DebugPrintHistory() const {
	std::cout << "Enemy state history (" << stateHistory.size() << " snapshots):" << std::endl;
	for (const auto& snapshot : stateHistory) {
		int activeCount = 0;
		for (const auto& state : snapshot.enemyStates) {
			if (state.active) activeCount++;
		}
		std::cout << "  Time: " << snapshot.timestamp << ", Active enemies: " << activeCount << std::endl;
	}
}