#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

// Default camera values
/*const GLfloat YAW = -90.0f;
const GLfloat PITCH = 0.0f;
const GLfloat SENSITIVTY = 0.1f;
const GLfloat FOV = 45.0f;*/

class Camera
{
public:

	Camera(float ratio, glm::vec3 camPos);
	~Camera();

	void mouseMove(int x, int y);
	void mouseRotate(int x, int y);

	void mouseButton(int button, int state, int x, int y);

	void update();
	void stop() {mSpeed = 0;}
	void setViewDir(glm::fvec3 dir);
	void setPosition(glm::fvec3 pos);
	void updateProjection(float ratio);

	const glm::mat4& getViewMatrix() const;
	const glm::mat4& getProjectionMatrix() const;
	const glm::vec3& getPosition() const { return mPosition;}

/*	GLfloat yaw;
	GLfloat fov;
	GLfloat pitch;
	GLfloat sensitivity;
	
	glm::vec3 world_up;
	glm::vec3 camera_up;
	glm::vec3 camera_pos;
	glm::vec3 camera_right;
	glm::vec3 camera_front;*/

private:
	int mOldX;             
	int mOldY;
	int mMotionState;       
	float mTheta;           
	float mPhi;            
	float mSpeed;           
	float mMotionStep;      
	float mThetaStep;
	float mPhiStep;
	glm::vec3 mPosition;    
	glm::vec3 mDirection; 

	glm::mat4 mViewMatrix;
	glm::mat4 mProjectionMatrix;

	bool mMove = true;
};

#endif
