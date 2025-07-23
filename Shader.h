#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glad/glad.h>
#include <glm.hpp>

using namespace glm;

class Shader
{
public:
	unsigned int ID;
	Shader(const char* vertexShaderPath, const char* fragmentShaderPath);
	void use();
	void deleteShader();
	void setUniformFloat(const char* uniformName, float uniformValue);
	void setUniformVec3(const char* uniformName, vec3 uniformVec3);
	void setUniformInt(const char* uniformName, int integer);
};

