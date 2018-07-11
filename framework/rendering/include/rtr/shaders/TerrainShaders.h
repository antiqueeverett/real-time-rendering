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
	GLint mTerrainResLocation;
	GLint mTileNoLocation;
	GLint mSubdivideLocation;
	GLint mTimeDayLocation;
	GLint mRotationLocation;
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
