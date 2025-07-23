#include "Texture.h"

Texture::Texture(const char* path, int index)
{
	textureIndex = index;
	glGenTextures(1, &ID);	
	glActiveTexture(textureIndexOffset + index);
	glBindTexture(GL_TEXTURE_2D,  ID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int nrChannels;

	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		std::cout << "Texture loading success" << std::endl;
	}
	else
	{
		std::cout << "ERROR: texture could not be loaded" << std::endl;
	}
	stbi_image_free(data);
}

void Texture::toShader(Shader* shaderPtr)
{
	Shader shader = *shaderPtr;
	shader.setUniformInt(("texture" + std::to_string(textureIndex)).c_str(), textureIndex);
}

unsigned int Texture::getID()
{
	return ID;
}
