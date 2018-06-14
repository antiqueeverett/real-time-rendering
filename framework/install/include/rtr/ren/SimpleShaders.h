#ifndef SIMPLE_SHADERS_H
#define SIMPLE_SHADERS_H

#include "headers.hpp"

class SimpleShaders
{
public:
	explicit SimpleShaders();
	virtual ~SimpleShaders();

	bool loadVertexFragmentShaders(const char* vertexShaderFilename, const char* fragmentShaderFilename);
	bool loadVertGeomFragShaders(const char* vertexShaderFilename, const char* geometryShaderFilename, const char* fragmentShaderFilename);

	virtual void activate();
	virtual void deactivate();

protected:
	std::string readFile(std::string fileName);

	void printShaderInfoLog(GLuint shader);
	void printProgramInfoLog();

	GLuint mVertexShader;	
	GLuint mFragmentShader;
	GLuint mGeometryShader;
	GLuint mShaderProgram;
};

#endif
