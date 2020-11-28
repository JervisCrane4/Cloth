#pragma once

#include <string>
#include <unordered_map>

#include <glm.hpp>
#include <ext.hpp>

struct ShaderProgramSources
{
	std::string VertexSource;
	std::string FragmentSource;
};

class Shader 
{
private:
	std::string m_Filepath;
	unsigned int m_RendererID;
	mutable std::unordered_map<std::string, int> m_UniformLocationCache;
public:
	Shader(const std::string& filepath);
	~Shader();

	void Bind() const;
	void Unbind() const;

	// Set uniforms
	void setUniform1i(const std::string& name, int value);
	void setUniform1f(const std::string& name, float value);
	void setUniform1iv(const std::string& name, int nb, int* value);
	void setUniform3f(const std::string& name, float v0, float v1, float v2);
	void setUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void setUniformMatrix(const std::string& name, glm::mat4& mat);

private:
	unsigned int compileShader(unsigned int type, const std::string& source);
	ShaderProgramSources parseShader(const std::string & filepath);
	unsigned int createShader(const std::string & vertexShader, const std::string & fragmentShader);
	int getUniformLocation(const std::string& name) const;
};