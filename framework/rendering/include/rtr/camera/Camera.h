#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

class Camera
{
public:

	Camera(float ratio, glm::vec3 camPos);
	~Camera();

	void mouseMove(int x, int y);
	void mouseButton(int button, int state, int x, int y);

	void update();
	void stop() {mSpeed = 0;}

	const glm::mat4& getViewMatrix() const;
	const glm::mat4& getProjectionMatrix() const;

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
