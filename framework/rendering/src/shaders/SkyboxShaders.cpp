#include "rtr/shaders/SkyboxShaders.h"
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;


SkyboxShaders::SkyboxShaders(int textureResolution, std::vector<std::string> texture_path) :
	mViewLocation(-1),
	mProjectionLocation(-1),
	mCubeSamplerLocation(-1),
	mTexturePath(texture_path)
	{
		mTextureID = generateSkyBox(textureResolution, mTexturePath);
	}


SkyboxShaders::~SkyboxShaders()
{
}

void SkyboxShaders::activate()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, mTextureID);
	SimpleShaders::activate();
}

void SkyboxShaders::locateUniforms()
{
	glUseProgram(mShaderProgram);


	mViewLocation = glGetUniformLocation(mShaderProgram, "view");
	if (mViewLocation == -1)
		printf("[TerrainShaders] View location not found\n");


	mProjectionLocation = glGetUniformLocation(mShaderProgram, "projection");
	if (mProjectionLocation == -1)
		printf("[TerrainShaders] Projection location not found\n");
	
	mCubeSamplerLocation = glGetUniformLocation(mShaderProgram, "skyboxSampler");
	if (mCubeSamplerLocation == -1)
		printf("[TerrainShaders] Skybox Sampler location not found\n");
	glUniform1i(mCubeSamplerLocation, 0);
}

// View Matrix
void SkyboxShaders::setViewMatrix(const glm::mat4& viewMatrix)
{
	if (mViewLocation < 0)
		printf("[TerrainShaders] uniform location for 'view' not known\n");

	glUseProgram(mShaderProgram);
	glUniformMatrix4fv(mViewLocation, 1, GL_FALSE, &viewMatrix[0][0]);

}

// Projection Matrix
void SkyboxShaders::setProjectionMatrix(const glm::mat4& projMatrix)
{
	if (mProjectionLocation < 0)
		printf("[TerrainShaders] uniform location for 'projection' not known\n");

	glUseProgram(mShaderProgram);
	glUniformMatrix4fv(mProjectionLocation, 1, GL_FALSE, &projMatrix[0][0]);
}

GLuint SkyboxShaders::generateSkyBox(int resolution, std::vector<std::string> texture_path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	for (int i = 0; i < texture_path.size(); i++)
	{
		unsigned char* texture_data = SOIL_load_image(texture_path[i].c_str(), &resolution, &resolution, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, resolution, resolution, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}