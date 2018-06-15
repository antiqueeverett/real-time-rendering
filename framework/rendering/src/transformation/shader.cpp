#include <rtr/transformation/shader.h>
#include <iostream>
#include <fstream>

static void CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage);
static std::string LoadShader(const std::string& fileName);
static GLuint CreateShader(const std::string& text, GLenum shaderType); //helper method to create shaders


Shader::Shader(const std::string& fileName)
{
	//creating the shader
	m_program = glCreateProgram();
	
	//loading | creating  the vertext shader and the fragment shader
	shaders[0] = CreateShader(LoadShader(fileName + ".vs"), GL_VERTEX_SHADER);
	shaders[1] = CreateShader(LoadShader(fileName + ".fs"), GL_FRAGMENT_SHADER);

	//for every shader (i.e .vd | .fs), load via the shader program
	for(unsigned int i = 0; i < NUM_SHADERS; i++)
		glAttachShader(m_program, shaders[i]);

	//before linking and validating the shader program
	//    specify what part of the data should be read as the shader program
	glBindAttribLocation(m_program, 0, "position");
	glBindAttribLocation(m_program, 1, "texCoord");
	glBindAttribLocation(m_program, 2, "normal");

	//at this point all the shaders should be linked: #link validation 
	glLinkProgram(m_program);
	CheckShaderError(m_program, GL_LINK_STATUS, true, "Error: Program linking failed: ");

	//shader validation 
	glValidateProgram(m_program);
	CheckShaderError(m_program, GL_VALIDATE_STATUS, true, "Error: Program is invalid: ");

	uniforms[TRANSFORM_U] = glGetUniformLocation(m_program, "transform");
}

Shader::~Shader()
{
	//once again destruct the shader in the reverse order which it was constructed 
	for(unsigned int i = 0; i < NUM_SHADERS; i++)
	{
		glDetachShader(m_program, shaders[i]);
		glDeleteShader(shaders[i]);
	}

	glDeleteProgram(m_program);
}

void Shader::Bind()
{
	//"glUseProgram"  : passing in the shader program into openGL
	glUseProgram(m_program);
}

void Shader::UpDate(const Transform& transform, const Camera& camera)
{
	glm::mat4 model = camera.GetViewProjection() * transform.GetModel();

	glUniformMatrix4fv(uniforms[TRANSFORM_U], 1, GL_FALSE, &model[0][0]);
}


static GLuint CreateShader(const std::string& text, GLenum shaderType)
{
	//creating the shader:
	//     - take the shader text, build and compile it then put it into the shader
	GLuint shader = glCreateShader(shaderType);

	// check for any haulting errors that may result from the shaders not being loaded
	if (shader == 0)
		std::cerr << "Error: Shader creation failed!" << std::endl;

	//having confirmed the validity of the shader
	//    because openGL is a C API we need convert our filename of type std::string to
	//    a C string format 
	const GLchar* shaderSourceStrings[1]; //expected openGL charecter size (which is actually a list of strings)
	GLint shaderSourceStringLengths[1];   //openGL will need to know about the size of the string 

	shaderSourceStrings[0] = text.c_str();
	shaderSourceStringLengths[0] = text.length();

	//sending our source code to openGL
	glShaderSource(shader, 1, shaderSourceStrings, shaderSourceStringLengths);

	//once we have sent the source code, we can send it to the openGL compiler
	glCompileShader(shader);

	//check any and all shader compiler errors
	CheckShaderError(shader, GL_COMPILE_STATUS, false, "Error: Shader compilaton failed: ");

	return shader;
}

/**
 * [this function is creates | loads the shader]
 * @param  fileName [textfile from the hard-drive]
 * @return          [loaded shader]
 */
static std::string LoadShader(const std::string& fileName)
{
	std::ifstream file;
	file.open((fileName).c_str());

	std::string output;
	std::string line;

	if(file.is_open())
	{
		while(file.good())
		{
			getline(file, line);
			output.append(line + "\n");
		}
	}
	else
	{
		std::cerr << "Unable to load shader: " << fileName << std::endl;
	}

	return output;
}

/**
 *
 * Reporting any error than may occur when loading the shader
 * 
 * [CheckShaderError description]
 * @param shader       [description]
 * @param flag         [description]
 * @param isProgram    [description]
 * @param errorMessage [description]
 */
static void CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage)
{
	GLint success = 0;
	GLchar error[1024] = { 0 };

	if(isProgram)
		glGetProgramiv(shader, flag, &success);
	else
		glGetShaderiv(shader,flag, &success);

	if(success == GL_FALSE)
	{
		if(isProgram)
			glGetProgramInfoLog(shader, sizeof(error), NULL, error);
		else
			glGetShaderInfoLog(shader, sizeof(error), NULL, error);

		std::cerr << errorMessage << ": '" << error << "'" << std::endl;
	}


}
