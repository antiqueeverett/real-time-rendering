#pragma once

#include <GL/glew.h>
#include <iostream>
#include <rtr/camera/Camera.h>
#include <rtr/animation/animate/model.h>
//#include <rtr/animation/camera/camera.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Loader
{
public:
	Loader();
	~Loader();

	GLfloat speed = 0.02f;
	
	//for camera
	GLfloat delta_time = 0.0f;
	GLfloat last_frame = 0.0f;
	GLfloat last_x;
	GLfloat last_y;

	GLboolean mouse_first_in;
	//animation_Camera camera;

	//animated model shader
	GLuint shade_model;

	//animation model instances
	Model model;

	//perspective
	Camera* camera = nullptr;
	glm::mat4 MVP;
	glm::mat4 view_matrix;
	glm::mat4 model_matrix;
	glm::mat4 projection_matrix;


	void init();
	void render();
	void update(int32_t width, int32_t height, Camera camera_,  glm::mat4 model, glm::mat4 view, glm::mat4 projection);
		
	static GLuint loadImageToTexture(const char* image_path);
	static GLuint loadDDS(const char* image_path, int* w = nullptr, int* h = nullptr);

};