#ifndef SKYBOX_SHADERS_H
#define SKYBOX_SHADERS_H

#include "rtr/shaders/SimpleShaders.h"
#include "SOIL/SOIL.h"
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
class SkyboxShaders : public SimpleShaders
{
public:
	
	SkyboxShaders(int textureResolution,std::vector<std::string> texture_path_day, std::vector<std::string> texture_path_night);
	~SkyboxShaders();
	void locateUniforms();
	void activate() override;

	void setViewMatrix(const glm::mat4& viewMatrix, const glm::mat4& rotateViewMat);
	void setProjectionMatrix(const glm::mat4& projMatrix);
	void setTime(float time);

private:

	GLuint generateSkyBox(int resolution, std::vector<std::string> texture_path);
	
	GLint mViewLocation;
	GLint mRotateViewLocation;
	GLint mProjectionLocation;
	GLint mCubeDaySamplerLocation;
	GLint mCubeNightSamplerLocation;
	GLint mTimeLocation;
	GLint mTextureID1;
	GLint mTextureID2;

	std::vector<std::string> mTexturePathDay;
	std::vector<std::string> mTexturePathNight;
	const float mRotationSpeed = 0.05f;
	float mRotation;


};

#endif