#ifndef TEXTURE_SHADERS_H
#define TEXTURE_SHADERS_H

#include <SOIL/SOIL.h>
#include <rtr/ren/SimpleShaders.h>


class TextureShaders : public SimpleShaders
{
public:
	explicit TextureShaders(int textureResolution, const char* path, int dim);
	virtual ~TextureShaders() = default;

	void locateUniforms();
	void activate() override;

	void setModelMatrix(const glm::mat4& transformMatrix);
	void setCameraMatrix(const glm::mat4& cameraMatrix);
	void setProjectionMatrix(const glm::mat4& projectionMatrix);
	void setAtlasDim();
	void setMinTextureFilter(int filter);
	void setMagTextureFilter(int filter);

private:

	void generateTexture(int resolution);

	GLint mModelLocation;
	GLint mCameraLocation;
	GLint mProjectionLocation;
	GLint mTextureSamplerLocation;
	GLint mDimLocation;
	GLuint mTextureID;

	const char* path;
	const int atlas_dim;
};

#endif
