#pragma once

#include <GL/glew.h>
#include <GL/glew.h>
#include <string>
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <glm/glm.hpp>

class Shader
{
private:
	GLuint m_RendererID;

	std::string parseShader(const char* filepath);
	GLuint compileShader(GLenum type, const std::string& source);
	GLuint createProgram(const std::string& vertexShader, const std::string& fragmentShader);

public:
	Shader(const char* vertexPath, const char* fragmentPath);
	~Shader();

	void Bind() const;
	void Unbind() const;

	void SetUniform1f(const std::string& name, float v0);
	void SetUniform1i(const std::string& name, int v0);

	void SetUniform2f(const std::string& name, float v0, float v1);
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);
private:
	GLint GetUniformLocation(const std::string& name);
};