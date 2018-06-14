	#include <rtr/shaders/TextureShaders.h>

	#include <glm/gtc/matrix_transform.hpp>


	using namespace glm;

	TextureShaders::TextureShaders(int textureResolution, const char* texture_path, int dim):
		mModelLocation(-1),
		mCameraLocation(-1),
		mProjectionLocation(-1),
		mTextureSamplerLocation(-1),
		mDimLocation(-1),
		mTextureID(0),
		path(texture_path),
		atlas_dim(dim)
	{
		generateTexture(textureResolution);
	}

	void TextureShaders::activate()
{
	// activate texture unit 0 and bind the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	SimpleShaders::activate(); // calls activate of inheritated class
}

void TextureShaders::locateUniforms()
{
	
	mCameraLocation = glGetUniformLocation(mShaderProgram, "camera_matrix");
	if (mCameraLocation == -1)
		printf("Camera location not found\n");
	
	mProjectionLocation = glGetUniformLocation(mShaderProgram, "projection_matrix");
	if (mProjectionLocation == -1)
		printf("Projection location not found\n");

	mat4 identityMatrix;

	mModelLocation = glGetUniformLocation(mShaderProgram, "model_matrix");
	if (mModelLocation == -1)
		printf("Model location not found\n");
	glUniformMatrix4fv(mModelLocation, 1, GL_FALSE, &identityMatrix[0][0]);

	mTextureSamplerLocation = glGetUniformLocation(mShaderProgram, "sampler");
	if (mTextureSamplerLocation == -1)
		printf("Texture sampler not found\n");

	glUniform1i(mTextureSamplerLocation, 0);

	mDimLocation = glGetUniformLocation(mShaderProgram, "atlas_dim");
	if (mDimLocation == -1)
		printf("Dimension location not found\n");

	
}


// Model Matrix
void TextureShaders::setModelMatrix(const glm::mat4& modelMatrix)
{
	if (mModelLocation < 0)
		printf("[TextureShaders] uniform location for 'model' not known\n");

	glUseProgram(mShaderProgram);												
	glUniformMatrix4fv(mModelLocation, 1, GL_FALSE, &modelMatrix[0][0]);
}

// View Matrix
void TextureShaders::setCameraMatrix(const glm::mat4& cameraMatrix)
{
	if (mCameraLocation < 0)
		printf("[TextureShaders] uniform location for 'camera' not known\n");

	glUseProgram(mShaderProgram);
	glUniformMatrix4fv(mCameraLocation, 1, GL_FALSE, &cameraMatrix[0][0]);
}

// Projection Matrix
void TextureShaders::setProjectionMatrix(const glm::mat4& projMatrix)
{
	if (mProjectionLocation < 0)
		printf("[TextureShaders] uniform location for 'projection' not known\n");

	glUseProgram(mShaderProgram);
	glUniformMatrix4fv(mProjectionLocation, 1, GL_FALSE, &projMatrix[0][0]);
}

void TextureShaders::setAtlasDim()
{
	if (mDimLocation < 0)
		printf("[TextureShaders] uniform location for 'atlas_dim' not known\n");

	glUseProgram(mShaderProgram);
	glUniform1i(mDimLocation, atlas_dim);
}

void TextureShaders::setMinTextureFilter(int filter)
{
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void TextureShaders::setMagTextureFilter(int filter)
{
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	glBindTexture(GL_TEXTURE_2D, 0);
}

// the important part
void TextureShaders::generateTexture(int imageResolution)
{
	unsigned char* texture_data = SOIL_load_image(path,&imageResolution, &imageResolution, 0, SOIL_LOAD_RGBA);

	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageResolution, imageResolution, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_data);
	glEnable (GL_BLEND); 
    glBlendFunc (GL_SRC_ALPHA, GL_ONE);
	glGenerateMipmap(GL_TEXTURE_2D);
		std::cout << "hi" << std::endl;
}