#ifndef TEXTURE_SHADERS_H
#define TEXTURE_SHADERS_H

#include <SOIL/SOIL.h>
#include <rtr/shaders/SimpleShaders.h>


class TextureShaders : public SimpleShaders
{
public:
	explicit TextureShaders(int textureResolution, const char* path, int dim);
	virtual ~TextureShaders() = default;

	void locateUniforms();
	void activate() override;
    void deactivate() override;

	void setModelMatrix(const glm::mat4& transformMatrix);
	void setCameraMatrix(const glm::mat4& cameraMatrix);
	void setProjectionMatrix(const glm::mat4& projectionMatrix);
	void setAtlasDim();
	void setMinTextureFilter(int filter);
	void setMagTextureFilter(int filter);
	void addTexture(int imageX, int imageY, const char* sampler, const char* location);


private:

	//for single textures
	void generateTexture(int resolution);
	//for multiple textures


	GLint mModelLocation;
	GLint mCameraLocation;
	GLint mProjectionLocation;
	GLint mTextureSamplerLocation;
	GLint mDimLocation;
	GLuint mTextureID = -1;

	std::vector<GLuint> mTextures;


	const char* path;
	const int atlas_dim;
};

#endif
