#pragma once
#include <glad/glad.h>
#include "stb_image.h"
#include "Shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

const int textureIndexOffset = 33984;

class Texture
{
private:
	unsigned int ID;
	int textureIndex;
	int width;
	int height;
public:
	Texture(const char* path, int textureIndex);
	void toShader(Shader* shaderPtr);
	unsigned int getID();
};

