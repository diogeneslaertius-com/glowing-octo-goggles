#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "Mesh.h"   // отдельный файл с Vertex и Mesh
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

class Model
{
public:
	// список мешей, которые составляют модель
	std::vector<Mesh> meshes;

	// конструктор: сразу загружает модель
	Model(const std::string& path);

	// отрисовка всей модели
	void Draw();

private:
	// загрузка модели через Assimp
	void loadModel(const std::string& path);

	// рекурсивная обработка узлов сцены
	void processNode(aiNode* node, const aiScene* scene);

	// обработка одного меша
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
};
