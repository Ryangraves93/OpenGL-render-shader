#pragma once
#include <string>
#include <unordered_map>
#include "glm/glm.hpp"

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};

class Shader
{
private:
	unsigned int m_RenderID;
	std::string m_Filepath;
	std::unordered_map<std::string, int> m_UniformLocationCache;
public:
	Shader(const std::string& filepath);
	~Shader();

	void Bind() const;
	void Unbind() const;

	
	//Set uniforms
	void SetUniforms4f(const std::string& Name, float v0, float v1, float v2, float v3);
	void SetUniforms1f(const std::string& Name, float value);
	void SetUniforms1i(const std::string& Name, int value);
	void SetUniformMat4f(const std::string& Name, const glm::mat4& matrix);
	
private:
	int GetUniformLocation(const std::string& name);

	ShaderProgramSource PraseShader(const std::string& filepath);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
	unsigned int compileShader(unsigned int type, const std::string& source);
};