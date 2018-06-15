#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <GL/glew.h>

class Texture
{
	public:
		Texture(const std::string& fileName);

		//set openGL to use whatever texture you are binding 
		//    unit can be used to bind multiple textures at once 
		void Bind(unsigned int unit);

		virtual ~Texture();
	private:
		Texture(const Texture& other){}
		void operator=(const Texture& other){}

		//a handle for the texture
		GLuint texture;

	protected:
};

#endif //TEXTURE_H