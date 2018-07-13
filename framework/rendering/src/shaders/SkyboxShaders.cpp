#include "rtr/shaders/SkyboxShaders.h"
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;


SkyboxShaders::SkyboxShaders(int textureResolution, std::vector<std::string> texture_path_day, std::vector<std::string> texture_path_night) :
	mViewLocation(-1),
	mRotateViewLocation(-1),
	mProjectionLocation(-1),
	mCubeDaySamplerLocation(-1),
	mTimeLocation(-1),
	mTexturePathDay(texture_path_day),
	mTexturePathNight(texture_path_night),
	mRotation(0.0f)
	{
		mTextureID1 = generateSkyBox(textureResolution, mTexturePathDay);
		mTextureID2 = generateSkyBox(textureResolution, mTexturePathNight);
	}


SkyboxShaders::~SkyboxShaders()
{
}

void SkyboxShaders::activate()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, mTextureID1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, mTextureID2);
	SimpleShaders::activate();
}

void SkyboxShaders::locateUniforms()
{
	glUseProgram(mShaderProgram);


	mViewLocation = glGetUniformLocation(mShaderProgram, "view");
	if (mViewLocation == -1){
		printf("[Skybox Shader] View location not found\n");
		exit(0);
	}

	mRotateViewLocation = glGetUniformLocation(mShaderProgram, "rotateView");
	if(mRotateViewLocation == -1)
		printf("[Skybox Shader] Rotate view location not found\n");

	mProjectionLocation = glGetUniformLocation(mShaderProgram, "projection");
	if (mProjectionLocation == -1)
		printf("[Skybox Shader] Projection location not found\n");
	
	mCubeDaySamplerLocation = glGetUniformLocation(mShaderProgram, "skyboxDaySampler");
	if (mCubeDaySamplerLocation == -1)
		printf("[Skybox Shader] Skybox Day Sampler location not found\n");
	glUniform1i(mCubeDaySamplerLocation, 0);

	mCubeNightSamplerLocation = glGetUniformLocation(mShaderProgram, "skyboxNightSampler");
	if (mCubeNightSamplerLocation == -1)
		printf("[Skybox Shader] Skybox Night Sampler location not found\n");
	glUniform1i(mCubeNightSamplerLocation, 1);

	mTimeLocation = glGetUniformLocation(mShaderProgram, "time");
	if (mTimeLocation == -1)
		printf("[Skybox Shader] time location not found\n");

}

// View Matrix
void SkyboxShaders::setViewMatrix(const glm::mat4& viewMatrix, const glm::mat4& rotateViewMat)
{
	if (mViewLocation < 0)
		printf("[Skybox Shader] uniform location for 'view' not known\n");

	glUseProgram(mShaderProgram);
	glUniformMatrix4fv(mViewLocation, 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(mRotateViewLocation, 1, GL_FALSE, &rotateViewMat[0][0]);

}

// Projection Matrix
void SkyboxShaders::setProjectionMatrix(const glm::mat4& projMatrix)
{
	if (mProjectionLocation < 0)
		printf("[TerrainShaders] uniform location for 'projection' not known\n");

	glUseProgram(mShaderProgram);
	glUniformMatrix4fv(mProjectionLocation, 1, GL_FALSE, &projMatrix[0][0]);
}

void SkyboxShaders::setTime(float time)
{
	int timeInt = static_cast<int>(time) % 24000;
	glUseProgram(mShaderProgram);
	glUniform1f(mTimeLocation, static_cast<float> (timeInt));
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
