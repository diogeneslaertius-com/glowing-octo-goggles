#include <algorithm>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Мои заголовки
#include "Shader.h"
#include "Camera.h"
#include "Render.h"
#include "Mechanics.h"
#include "Model.h"
#include "Terrain.h"
#include "Beam.h"
#include "EnemyPool.h"
#include "Player.h"
#include "Audio.h"

// Заголовки ImGui
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>

void InitImGui(GLFWwindow* window) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Темная киберпанк тема
	ImGui::StyleColorsDark();
	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.05f, 0.05f, 0.1f, 0.8f);
	style.Colors[ImGuiCol_Text] = ImVec4(0.0f, 1.0f, 1.0f, 1.0f); // cyan text
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.0f, 0.8f, 1.0f, 1.0f); // cyan progress

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 430");
}

void DrawWeaponHUD(const glm::mat4& projection, const glm::mat4& view, Shader& shader) {
	// Рисуем простой прицел в центре экрана
	// Это будет простая модель оружия или линии прицела

	// Матрица для HUD элементов (в пространстве экрана)
	glm::mat4 hudModel = glm::mat4(1.0f);

	// Отключаем depth test для HUD
	glDisable(GL_DEPTH_TEST);

	// Рисуем простой крест прицела
	shader.SetUniformMat4f("projection", glm::mat4(1.0f)); // ортогональная проекция
	shader.SetUniformMat4f("view", glm::mat4(1.0f));
	shader.SetUniformMat4f("model", hudModel);
	shader.SetUniform4f("u_Color", 0.0f, 1.0f, 0.0f, 0.8f); // зеленый прицел

	// Здесь бы рисовали модель оружия, но для простоты оставим как есть

	glEnable(GL_DEPTH_TEST);
}

int main(void) {
	GLFWwindow* window;

	if (!glfwInit())
		return -1;

	// Настраиваем контекст OpenGL 4.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(1200, 800, "Time Crisis Shooter - Hackathon Demo", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) {
		std::cerr << "GLEW initialization failed!\n";
		glfwTerminate();
		return -1;
	}

	std::cout << "OpenGL ver " << glGetString(GL_VERSION) << std::endl;
	glfwSwapInterval(1);

	// Инициализация систем
	Shader shader("vertex.glsl", "fragment.glsl");
	Renderer::InitGLSettings();

	// === АУДИО СИСТЕМА ===
	Audio& audio = Audio::GetInstance();
	if (audio.Initialize()) {
		// Загружаем звуки
		audio.LoadSound("shoot", "assets/shoot.wav");
		audio.LoadSound("rewind", "assets/rewind.wav");
		audio.LoadSound("hit", "assets/hit.wav");
		audio.LoadSound("music", "assets/music.wav");

		// Включаем фоновую музыку
		audio.PlaySound("music", true); // зацикленное воспроизведение
		std::cout << "Audio system initialized with background music" << std::endl;
	}

	const float FOV = 45.0f;
	const float NEAR_PLANE = 0.1f;
	const float FAR_PLANE = 100.0f;

	// Константы событий
	const int HIT = 0;
	const int DEATH = 1;
	const int PLAYER_MOVE = 2;
	const int BULLET_SPAWN = 3;

	// Игровые переменные
	float timeEnergy = 100.0f;
	float lastSaveTime = 0.0f;
	const float SAVE_INTERVAL = 0.05f; // сохраняем состояние каждые 50ms

	// Инициализация игровых объектов
	glm::vec3 startPos(0.0f, 2.0f, 3.0f);
	Camera camera(startPos);
	Player player;
	player.SetPosition(startPos);

	Mechanics mechanicsHandler(&camera);
	EnemyPool enemyPool(30); // увеличили пул до 30 врагов
	Terrain terrain(50.0f, 0.0f);

	// Система волн
	float waveTimer = 0.0f;
	int currentWave = 1;
	bool waveActive = false;

	// Настройка ввода
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetWindowUserPointer(window, &mechanicsHandler);
	glfwSetCursorPosCallback(window, Mechanics::MouseCallback);

	// === ИНИЦИАЛИЗАЦИЯ IMGUI ===
	InitImGui(window);

	// Модели и эффекты
	std::vector<Beam> beams;
	Model enemyModel("assets/cube.obj"); // fallback куб для врагов
	Model weaponModel("assets/weapon.obj"); // модель оружия (опционально)

	float lastTime = 0.0f;
	bool lastShotState = false;

	std::cout << "=== TIME CRISIS SHOOTER INITIALIZED ===\n";
	std::cout << "Controls: WASD - Move, Mouse - Look, LMB - Shoot, R - Rewind Time\n";

	while (!glfwWindowShouldClose(window)) {
		float currentTime = (float)glfwGetTime();
		float deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		// === ОБНОВЛЕНИЕ СИСТЕМ ===
		mechanicsHandler.Update(deltaTime);
		player.Update(deltaTime, terrain);

		// Синхронизируем камеру с игроком
		glm::vec3 playerPos = player.GetPosition();
		camera.SetPosition(playerPos + glm::vec3(0.0f, 1.8f, 0.0f)); // высота глаз

		// Обновляем позицию слушателя для 3D звука
		audio.SetListenerPosition(playerPos.x, playerPos.y, playerPos.z);

		mechanicsHandler.AddPlayerMoveEvent(playerPos);

		// === УПРАВЛЕНИЕ ИГРОКОМ ===
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) player.MoveForward(1.0f);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) player.MoveForward(-1.0f);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) player.MoveRight(-1.0f);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) player.MoveRight(1.0f);
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) player.Jump();

		// === СОХРАНЕНИЕ СОСТОЯНИЯ ВРАГОВ ===
		if (currentTime - lastSaveTime > SAVE_INTERVAL) {
			enemyPool.SaveCurrentState(currentTime);
			lastSaveTime = currentTime;
		}

		// === СИСТЕМА ВОЛН ===
		waveTimer += deltaTime;
		if (!waveActive && waveTimer > 4.0f) { // новая волна каждые 4 секунды
			glm::vec3 spawnCenter = playerPos;
			int enemyCount = std::min(currentWave + 3, 15); // максимум 15 врагов за волну

			enemyPool.SpawnWaveCircle(enemyCount, spawnCenter, 10.0f + currentWave, 2.0f);

			std::cout << "=== WAVE " << currentWave << " SPAWNED (" << enemyCount << " enemies) ===\n";
			currentWave++;
			waveActive = true;
			waveTimer = 0.0f;
		}

		// Обновляем врагов
		enemyPool.UpdateAll(deltaTime, playerPos);

		// Проверяем завершение волны
		if (waveActive && enemyPool.GetActiveCount() == 0) {
			waveActive = false;
			timeEnergy = std::min(timeEnergy + 20.0f, 100.0f); // бонус энергии за волну
			std::cout << "Wave " << (currentWave - 1) << " cleared! +20 Time Energy\n";
		}

		// === ЛОГИКА СТРЕЛЬБЫ ===
		bool currentShotState = mechanicsHandler.CheckForShot(window);
		if (currentShotState && !lastShotState) { // только при нажатии, не удерживании
			glm::vec3 rayOrigin = camera.GetPosition();
			glm::vec3 rayDir = camera.GetFront();

			// Звук выстрела
			audio.PlaySound("shoot");

			// Проверяем попадания по врагам
			bool hitTarget = false;
			auto& enemies = enemyPool.GetEnemies();
			for (size_t i = 0; i < enemies.size(); ++i) {
				auto& enemy = enemies[i];
				if (!enemy.IsActive()) continue;

				// Простая проверка пересечения луча с врагом
				glm::vec3 enemyPos = enemy.GetPosition();
				glm::vec3 toEnemy = enemyPos - rayOrigin;
				float distance = glm::length(toEnemy);

				if (distance > 50.0f) continue; // максимальная дистанция

				float projection = glm::dot(toEnemy, rayDir);
				if (projection <= 0) continue; // враг позади

				glm::vec3 closestPoint = rayOrigin + rayDir * projection;
				float distanceToRay = glm::length(enemyPos - closestPoint);

				if (distanceToRay < 1.2f) { // радиус попадания
					enemy.TakeDamage(75.0f); // больше урона для баланса
					mechanicsHandler.AddEvent(HIT, enemyPos, static_cast<int>(i));

					// Звук попадания
					audio.PlaySound("hit");
					audio.SetSoundPosition("hit", enemyPos.x, enemyPos.y, enemyPos.z);

					std::cout << "HIT! Enemy " << i << " HP: " << enemy.GetHP() << "\n";
					hitTarget = true;
					break; // один выстрел = одна цель
				}
			}

			// Создаем визуальный луч
			float beamLength = hitTarget ? 15.0f : 25.0f;
			glm::vec3 beamColor = hitTarget ? glm::vec3(0.2f, 1.0f, 0.2f) : glm::vec3(1.0f, 0.2f, 1.0f);
			beams.emplace_back(rayOrigin, rayDir, beamLength, 0.3f, beamColor);
		}
		lastShotState = currentShotState;

		// === СИСТЕМА ОТМЕНЫ ВРЕМЕНИ ===
		static bool lastRewindState = false;
		bool currentRewindState = glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS;

		if (currentRewindState && timeEnergy > 0.0f) {
			float energyCost = 60.0f * deltaTime; // 60 единиц в секунду

			if (timeEnergy >= energyCost) {
				// Отматываем время
				float rewindAmount = deltaTime * 2.0f; // отматываем в 2x скорости
				mechanicsHandler.RewindTime(rewindAmount);

				// Восстанавливаем состояние врагов
				float targetTime = currentTime - rewindAmount * 3.0f; // берем состояние из более далекого прошлого
				if (enemyPool.RestoreStateAt(targetTime)) {
					// Успешное восстановление
				}

				timeEnergy -= energyCost;

				// Звук отмены времени (проигрываем только в начале)
				if (!lastRewindState) {
					audio.PlaySound("rewind");
				}
			}
			else {
				timeEnergy = 0.0f;
			}
		}
		else if (!currentRewindState && timeEnergy < 100.0f) {
			// Восстановление энергии
			timeEnergy = std::min(timeEnergy + (15.0f * deltaTime), 100.0f);
		}
		lastRewindState = currentRewindState;

		// === ОБНОВЛЕНИЕ КАМЕРЫ И ПРОЕКЦИИ ===
		Renderer::UpdateViewport(window);
		glm::mat4 view = camera.ProcessInput(window, deltaTime);
		glm::mat4 projection = camera.GetProjectionMatrix(window, FOV, NEAR_PLANE, FAR_PLANE);

		// === IMGUI HUD ===
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Главный HUD
		ImGui::Begin("TACTICAL HUD", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);
		ImGui::SetWindowPos(ImVec2(20, 20));

		// Информация о волне
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "WAVE: %d", currentWave);
		ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "ENEMIES: %d", enemyPool.GetActiveCount());
		ImGui::TextColored(ImVec4(0.8f, 0.2f, 1.0f, 1.0f), "COMBO: %dx", mechanicsHandler.GetHitsWithoutRewind());

		ImGui::Separator();

		// Энергия времени
		ImGui::Text("TIME ENERGY");
		float energyPercent = timeEnergy / 100.0f;
		ImVec4 energyColor = energyPercent > 0.3f ? ImVec4(0.0f, 0.8f, 1.0f, 1.0f) : ImVec4(1.0f, 0.3f, 0.0f, 1.0f);
		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, energyColor);
		ImGui::ProgressBar(energyPercent, ImVec2(250.0f, 25.0f), "");
		ImGui::PopStyleColor();

		// Индикатор глитча
		float glitchIntensity = mechanicsHandler.GetGlitchIntensity();
		if (glitchIntensity > 0.01f) {
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "<<< TIME FLUX ACTIVE >>>");
		}

		ImGui::End();

		// Прицел в центре
		ImVec2 center = ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
		ImGui::GetForegroundDrawList()->AddLine(
			ImVec2(center.x - 10, center.y), ImVec2(center.x + 10, center.y),
			IM_COL32(0, 255, 0, 200), 2.0f);
		ImGui::GetForegroundDrawList()->AddLine(
			ImVec2(center.x, center.y - 10), ImVec2(center.x, center.y + 10),
			IM_COL32(0, 255, 0, 200), 2.0f);

		// === РЕНДЕРИНГ 3D СЦЕНЫ ===
		Renderer::Clear(0.02f, 0.02f, 0.05f, 1.0f); // темно-синий космический фон

		shader.Bind();
		shader.SetUniformMat4f("projection", projection);
		shader.SetUniformMat4f("view", view);
		shader.SetUniform1f("time", currentTime);
		shader.SetUniform1f("glitchIntensity", glitchIntensity);

		// 1. Рендерим террейн
		glm::mat4 terrainModel = glm::mat4(1.0f);
		shader.SetUniformMat4f("model", terrainModel);
		shader.SetUniform4f("u_Color", 0.1f, 0.2f, 0.3f, 1.0f); // темно-синий пол
		terrain.Draw();

		// 2. Рендерим врагов с анимированными цветами
		for (const auto& enemy : enemyPool.GetEnemies()) {
			if (!enemy.IsActive()) continue;

			shader.SetUniformMat4f("model", enemy.GetModelMatrix());
			glm::vec4 color = enemy.GetColor();

			// Добавляем пульсацию для живости
			float pulse = sin(currentTime * 5.0f + enemy.GetPosition().x) * 0.1f + 0.9f;
			color *= pulse;

			shader.SetUniform4f("u_Color", color.r, color.g, color.b, color.a);
			enemyModel.Draw();
		}

		// 3. Рендерим энергетические лучи
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE); // аддитивное смешивание для свечения

		for (auto it = beams.begin(); it != beams.end();) {
			it->Update(deltaTime);
			if (!it->IsAlive()) {
				it = beams.erase(it);
			}
			else {
				shader.SetUniformMat4f("model", it->GetModelMatrix());
				glm::vec3 beamColor = it->GetColor();
				shader.SetUniform4f("u_Color", beamColor.r, beamColor.g, beamColor.b, 0.8f);
				it->Draw();
				++it;
			}
		}

		glDisable(GL_BLEND);

		// === ФИНАЛИЗАЦИЯ КАДРА ===
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Очистка истории каждые 5 секунд
		static float cleanupTimer = 0.0f;
		cleanupTimer += deltaTime;
		if (cleanupTimer > 5.0f) {
			enemyPool.CleanOldStates(8.0f); // храним 8 секунд истории
			cleanupTimer = 0.0f;
		}

		Renderer::CheckError();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// === ОЧИСТКА РЕСУРСОВ ===
	std::cout << "Shutting down systems...\n";

	audio.Shutdown();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();

	std::cout << "=== TIME CRISIS SHOOTER - SESSION COMPLETE ===\n";
	return 0;
}