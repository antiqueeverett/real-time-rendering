#ifndef PARTICLE_SHADERS_H
#define PARTICLE_SHADERS_H

#include <SOIL/SOIL.h>
#include <rtr/shaders/SimpleShaders.h>


class ParticleShaders : public SimpleShaders
{
public:
	explicit ParticleShaders();
	virtual ~ParticleShaders() = default;

	void locateUniforms();
	void activate() override;

	void setCameraMatrix(const glm::mat4& cameraMatrix);
	void setProjectionMatrix(const glm::mat4& projectionMatrix);

	GLint mDrawMode = GL_FILL;

private:

	GLint mCameraLocation;
	GLint mProjectionLocation;

};

#endif
