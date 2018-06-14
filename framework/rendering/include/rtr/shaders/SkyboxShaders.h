#ifndef SKYBOX_SHADERS_H
#define SKYBOX_SHADERS_H

#include "rtr/shaders/SimpleShaders.h"
#include "SOIL/SOIL.h"
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
class SkyboxShaders : public SimpleShaders
{
public:
	
	SkyboxShaders(int textureResolution, std::vector<std::string> texture_path);
	~SkyboxShaders();
	void locateUniforms();
	void activate() override;

	void setViewMatrix(const glm::mat4& viewMatrix);
	void setProjectionMatrix(const glm::mat4& projMatrix);

private:

	GLuint generateSkyBox(int resolution, std::vector<std::string> texture_path);

	GLint mViewLocation;
	GLint mProjectionLocation;
	GLint mCubeSamplerLocation;
	GLint mTextureID;

	std::vector<std::string> mTexturePath;

};

#endif