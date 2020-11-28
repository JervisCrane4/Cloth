#include "Shader.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <GL/glew.h>


Shader::Shader(const std::string & filepath)
	: m_Filepath(filepath), m_RendererID(0)
{
	ShaderProgramSources source = parseShader(filepath);
	m_RendererID = createShader(source.VertexSource, source.FragmentSource);
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

void Shader::setUniform1i(const std::string & name, int value)
{
	glUniform1i(getUniformLocation(name), value);
}

void Shader::setUniform1f(const std::string & name, float value)
{
	glUniform1f(getUniformLocation(name), value);
}

void Shader::setUniform1iv(const std::string & name, int nb ,int* value)
{
	glUniform1iv(getUniformLocation(name), nb, value);
}

void Shader::setUniform3f(const std::string & name, float v0, float v1, float v2)
{
	glUniform3f(getUniformLocation(name), v0, v1, v2);
}

void Shader::setUniform4f(const std::string & name, float v0, float v1, float v2, float v3)
{
	glUniform4f(getUniformLocation(name), v0, v1, v2, v3);
}

void Shader::setUniformMatrix(const std::string & name, glm::mat4& mat)
{
	glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
}

int Shader::getUniformLocation(const std::string & name) const
{
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
		return m_UniformLocationCache[name];

	int location = glGetUniformLocation(m_RendererID, name.c_str());
	if (location == -1)
		std::cerr << "Warning : uniform ' " << name << " ' doesn't exist !" << std::endl;
	
	m_UniformLocationCache[name] = location;
	return location;
}

unsigned int Shader::compileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, NULL);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") <<
			std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}

	return id;
}

unsigned int Shader::createShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	unsigned int program = glCreateProgram();
	unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

ShaderProgramSources Shader::parseShader(const std::string& filepath)
{
	std::ifstream stream(filepath);

	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;
	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
				type = ShaderType::VERTEX;
			else if (line.find("fragment") != std::string::npos)
				type = ShaderType::FRAGMENT;
		}
		else
		{
			ss[(int)type] << line << "\n";
		}
	}

	return { ss[0].str(), ss[1].str() };
}
