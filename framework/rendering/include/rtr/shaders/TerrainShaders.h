#ifndef TERRAIN_SHADERS_H
#define TERRAIN_SHADERS_H

#include "rtr/shaders/SimpleShaders.h"
#include "SOIL/SOIL.h"
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

using namespace glm;

class TerrainShaders : public SimpleShaders
{
public:
	explicit TerrainShaders(int textureResolution, std::vector<std::string> texture_path, vec4 sunDirection, float amplitude, float frequency, int terrainResolution, int tileNumber, int subdivide);
	virtual ~TerrainShaders() = default;

	void locateUniforms();
	void activate() override;

	void setModelMatrix(const mat4& transformMatrix);
	void setViewMatrix(const mat4& viewMatrix);
	void setProjectionMatrix(const mat4& projMatrix);
	void setAmplitude(const float amplitude);
	void setFrequency(const float frequency);
	void setTime(const vec3& timeTranslate);
	void setDayTime(float time);
	void setSunRotation(const mat4& rotationMatrix); 
private:
	GLuint generateTexture(int resolution, const char* path);

	GLint mModelLocation = -1;
	GLint mModelInvTLocation = -1;
	GLint mViewLocation = -1;
	GLint mProjectionLocation = -1;
	GLint mWorldSunDirectionLocation = -1;
	GLint mTextureSamplerLocation0 = -1;
	GLint mTextureSamplerLocation1 = -1;
	GLint mTextureSamplerLocation2 = -1;
	GLint mAmplitudeLocation = -1;
	GLint mFrequencyLocation = -1;
	GLint mTimeLocation = -1;
	GLint mTerrainResLocation = -1;
	GLint mTileNoLocation = -1;
	GLint mSubdivideLocation = -1;
	GLint mTimeDayLocation = -1;
	GLint mRotationLocation = -1;
	GLuint mTextureID0;
	GLuint mTextureID1;
	GLuint mTextureID2;

	std::vector<std::string> mTexturePath;
	vec4 mSunDirection;
	float const mFrequency;
	float const mAmplitude;
	float const mTerrainResolution;
	float const mTileNumber;
	int const mSubdivide;
};

#endif
