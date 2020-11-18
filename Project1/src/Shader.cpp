#include "Shader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "Renderer.h"




Shader::Shader(const std::string& filepath)
	: m_Filepath(filepath), m_RenderID(0)
{
	ShaderProgramSource source = PraseShader(filepath);
	m_RenderID = CreateShader(source.VertexSource, source.FragmentSource);	
}

Shader::~Shader()
{
	glDeleteProgram(m_RenderID);
}

ShaderProgramSource Shader::PraseShader(const std::string& filepath)
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
			{
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos)
			{
				type = ShaderType::FRAGMENT;
			}
		}
		else
		{
			ss[(int)type] << line << "\n";
		}
	}

	return { ss[0].str(), ss[1].str() };
}

unsigned int Shader::compileShader(unsigned int type, const std::string& source)
{
	//Create shader object - Maintains the source code strings that define a shader
	//Type indicates type of shader - Vertex shader, geometry shader or fragment shader
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	//Sets the source in id to the source code in the array of strings, i.e src
	glShaderSource(id, 1, &src, nullptr);
	//Compiles the shader, returns gl_true if fine ! is false
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile shader" << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}

	return id;
}
unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	//Creates an empty program object. Program objects are objects to which shader objects can be attached.
	unsigned int program = glCreateProgram();
	unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

	//Attaches to the program, the shader object 
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	//Links program object into an executable
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

void Shader::Bind() const
{
	glUseProgram(m_RenderID);
}

void Shader::Unbind() const
{
	glUseProgram(0);
}

void Shader::SetUniforms4f(const std ::string& Name, float v0, float v1, float v2, float v3)
{
	glUniform4f(GetUniformLocation(Name), v0, v1, v2, v3);
}

unsigned int Shader::GetUniformLocation(const std::string& name)
{
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
		return m_UniformLocationCache[name];
	int location = glGetUniformLocation(m_RenderID, name.c_str());
	if (location == -1)
		std::cout << "Waring : uniform" << name << "Doesn't exist" << std::endl;

	m_UniformLocationCache[name] = location;
	return location;

}
