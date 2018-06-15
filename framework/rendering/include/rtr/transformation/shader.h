#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <GL/glew.h>
#include "transform.h"
#include "camera.h"

class Shader
{
public:
	Shader(const std::string& fileName);

	void Bind();
	void UpDate(const Transform& transform, const Camera& camera);

	virtual ~Shader();

private:
	//"NUM_SHADERS" specifies the number of composite shaders (= 3 with a geometry shader)
	static const unsigned int NUM_SHADERS = 2;
	Shader(const Shader& other){}
	void operator=(const Shader& other){}

	enum
	{
		TRANSFORM_U,

		NUM_UNIFORMS
	};

	//a handle for openGL to evaluate where the program is
	//   this is how we refer to our shader program with openGL "GLuint" 
	GLuint m_program;


	GLuint shaders[NUM_SHADERS];
	GLuint uniforms[NUM_UNIFORMS];

protected:


};

#endif //SHADER_H


/**
 *
 * Constructor takes as part of its argument a string which specifies the location of the shader.
 * The shader is in it of itself a program that is written in separate files & and they are loaded in the 
 *   shader class. These files are then built for the GPU and sent to the GPU
 *
 * @function void Bind(): shader function to instruct the GPU to utilize the .vs or .fs 
 *
 */