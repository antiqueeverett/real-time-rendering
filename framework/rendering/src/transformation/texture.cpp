#include <rtr/transformation/texture.h>
#include <rtr/transformation/stb_image.h>
#include <cassert>
#include <iostream>

/**
 *
 * "stb_image.c" is used to load the texture 
 * What the class is based on the texture loader used stb_image.c
 *  The whole class is subject to change should a different texture loader be used 
 * 
 */
Texture::Texture(const std::string& fileName)
{
	int width, height, numComponents;

	//stbi_load() is used to load the image texture data
	unsigned char* imageData = stbi_load(fileName.c_str(), &width, &height, &numComponents, 4);

	if(imageData == NULL)
		std::cerr << "Texture loading failed for texture: " << fileName << std::endl;

	//openGL stores everything in buffers
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	//OPTIONAL PARAMETERS THAT NEED TO BE SET:

	//"glTexParameteri" handles pixel wrapping allocation (x, y axis repeatedly)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//"glTexParameterf" ensures texture filtering in events of more | less pixel 
	//  generation in contrast to the set resolution
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//"glTexImage2D"  sending our texture to the GPU
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);

	//stbi_free() is used to free the image texture data
	stbi_image_free(imageData);
}

Texture::~Texture()
{ 
	//deleting the Texture
	glDeleteTextures(1, &texture);
}

void Texture::Bind(unsigned int unit)
{
	assert(unit >= 0 && unit <= 31);

	glActiveTexture(GL_TEXTURE0 + unit);
	//binding the texture
	glBindTexture(GL_TEXTURE_2D, texture);
}