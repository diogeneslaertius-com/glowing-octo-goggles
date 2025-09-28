#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Model.h"
#include <memory>

enum RobotType {
	BASIC_ROBOT = 0,
	TEMPORAL_HUNTER_ROBOT = 1,
	GLITCH_ROBOT = 2
};

class AnimatedRobot {
public:
	AnimatedRobot();
	~AnimatedRobot();

	// === нямнбмше лерндш ===
	void Reset(const glm::vec3& spawnPos, RobotType type = BASIC_ROBOT);
	void TakeDamage(float dmg);
	void Update(float deltaTime, const glm::vec3& playerPos);
	void Draw(); // ЕЯКХ УНВЕЬЭ ЯНАЯРБЕММШИ ПЕМДЕП

	// === церрепш ===
	glm::mat4 GetModelMatrix() const;
	glm::vec4 GetColor() const;
	bool IsActive() const { return active; }
	void Deactivate() { active = false; }
	float GetHP() const { return hp; }
	glm::vec3 GetPosition() const { return position; }
	RobotType GetRobotType() const { return robotType; }

	// === ярюрхвеяйхе лерндш дкъ гюцпсгйх лндекеи ===
	static bool LoadRobotModel(const std::string& path);
	static void CleanupModels();

private:
	// === хцпнбше оюпюлерпш ===
	bool active;
	float hp;
	float maxHp;
	glm::vec3 position;
	float speed;
	float shootTimer;
	RobotType robotType;

	// === юмхлюжхнммше оюпюлерпш ===
	float animationTime;
	float idleAnimSpeed;
	float walkAnimSpeed;
	glm::vec3 lastPosition;
	bool isMoving;

	// === бхгсюкэмше щттейрш ===
	float glitchAmount;
	float colorPulse;

	// === ярюрхвеяйхе лндекх (наыхе дкъ бяеу пнанрнб) ===
	static std::shared_ptr<Model> s_BasicRobotModel;
	static std::shared_ptr<Model> s_HunterRobotModel;
	static bool s_ModelsLoaded;

	// === опхбюрмше лерндш ===
	void UpdateAnimation(float deltaTime);
	void UpdateMovement(float deltaTime, const glm::vec3& playerPos);
	void UpdateVisualEffects(float deltaTime);
	Model* GetCurrentModel() const;
	glm::vec3 GetBaseColor() const;
};