#include <rtr/shaders/ParticleShaders.h>

#include <glm/gtc/matrix_transform.hpp>


using namespace glm;

ParticleShaders::ParticleShaders():
	mCameraLocation(-1),
	mProjectionLocation(-1)
{}

void ParticleShaders::activate(){

glPolygonMode(GL_FRONT_AND_BACK, mDrawMode);
SimpleShaders::activate(); // calls activate of inheritated class
}

void ParticleShaders::locateUniforms()
{
	
	mCameraLocation = glGetUniformLocation(mShaderProgram, "camera_matrix");
	if (mCameraLocation == -1)
		printf("Camera location not found\n");
	
	mProjectionLocation = glGetUniformLocation(mShaderProgram, "projection_matrix");
	if (mProjectionLocation == -1)
		printf("Projection location not found\n");
}

// View Matrix
void ParticleShaders::setCameraMatrix(const glm::mat4& cameraMatrix)
{
	if (mCameraLocation < 0)
		printf("[ParticleShaders] uniform location for 'camera' not known\n");

	glUseProgram(mShaderProgram);
	glUniformMatrix4fv(mCameraLocation, 1, GL_FALSE, &cameraMatrix[0][0]);
}

// Projection Matrix
void ParticleShaders::setProjectionMatrix(const glm::mat4& projMatrix)
{
	if (mProjectionLocation < 0)
		printf("[ParticleShaders] uniform location for 'projection' not known\n");

	glUseProgram(mShaderProgram);
	glUniformMatrix4fv(mProjectionLocation, 1, GL_FALSE, &projMatrix[0][0]);
}