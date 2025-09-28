#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "Mesh.h"   // ��������� ���� � Vertex � Mesh
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

class Model
{
public:
	// ������ �����, ������� ���������� ������
	std::vector<Mesh> meshes;

	// �����������: ����� ��������� ������
	Model(const std::string& path);

	// ��������� ���� ������
	void Draw();

private:
	// �������� ������ ����� Assimp
	void loadModel(const std::string& path);

	// ����������� ��������� ����� �����
	void processNode(aiNode* node, const aiScene* scene);

	// ��������� ������ ����
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
};
