#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

std::string Shader::parseShader(const char* filepath)
{
	std::string content;
	std::ifstream file(filepath, std::ios::in);
	std::string line = "";
	if (!file.is_open()) {
		throw std::runtime_error(std::string("Не удалось открыть файл: ") + filepath);
	}
	while (std::getline(file, line))
	{
		content += line + "\n";
	}
	file.close();
	return content;
}

GLuint Shader::compileShader(GLenum type, const std::string & source)
{
	GLuint id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	// Добавьте проверку ошибок компиляции здесь!
	// ...

	return id;
}

GLuint Shader::createProgram(const std::string & vertexShader, const std::string & fragmentShader)
{
	GLuint program = glCreateProgram();
	GLuint vs = compileShader(GL_VERTEX_SHADER, vertexShader);
	GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs); // Шейдеры можно удалить после линковки
	glDeleteShader(fs);

	return program;
}

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
	std::string vSource = parseShader(vertexPath);
	std::string fSource = parseShader(fragmentPath);
	m_RendererID = createProgram(vSource, fSource);
}

Shader::~Shader()
{
	glDeleteProgram(m_RendererID);
}

void Shader::Bind() const
{
	glUseProgram(m_RendererID);
}

void Shader::Unbind() const
{
	glUseProgram(0);
}

GLint Shader::GetUniformLocation(const std::string & name)
{
	// Обычно здесь кэшируют локации uniform'ов
	return glGetUniformLocation(m_RendererID, name.c_str());
}

void Shader::SetUniformMat4f(const std::string & name, const glm::mat4 & matrix)
{
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::SetUniform4f(const std::string & name, float v0, float v1, float v2, float v3)
{
	glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}

void Shader::SetUniform2f(const std::string& name, float v0,float v1)
{
	glUniform2f(GetUniformLocation(name), v0 , v1);
}

void Shader::SetUniform1f(const std::string& name, float v0)
{
	glUniform1f(GetUniformLocation(name), v0);
}

void Shader::SetUniform1i(const std::string& name, int v0)
{
	glUniform1i(GetUniformLocation(name), v0);
}

