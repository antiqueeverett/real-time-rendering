#ifndef TERRAIN_SHADERS_H
#define TERRAIN_SHADERS_H

#include "rtr/shaders/SimpleShaders.h"
#include "SOIL/SOIL.h"
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class TerrainShaders : public SimpleShaders
{
public:
	explicit TerrainShaders(int textureResolution, std::vector<std::string> texture_path, float amplitude, float frequency);
	virtual ~TerrainShaders() = default;

	void locateUniforms();
	void activate() override;

	void setModelMatrix(const glm::mat4& transformMatrix);
	void setViewMatrix(const glm::mat4& viewMatrix);
	void setProjectionMatrix(const glm::mat4& projMatrix);
	void setAmplitude(const float amplitude);
	void setFrequency(const float frequency);
	void setTime(const glm::vec3& timeTranslate);

	
private:
	GLuint generateTexture(int resolution, const char* path);

	GLint mModelLocation;
	GLint mModelInvTLocation;
	GLint mViewLocation;
	GLint mProjectionLocation;
	GLint mWorldSunDirectionLocation;
	GLint mTextureSamplerLocation0;
	GLint mTextureSamplerLocation1;
	GLint mTextureSamplerLocation2;
	GLint mAmplitudeLocation;
	GLint mFrequencyLocation;
	GLint mTimeLocation;
	GLuint mTextureID0;
	GLuint mTextureID1;
	GLuint mTextureID2;

	std::vector<std::string> mTexturePath;
	float const frequency;
	float const amplitude;

};

#endif
