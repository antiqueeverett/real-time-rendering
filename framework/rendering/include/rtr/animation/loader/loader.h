#pragma once

#include <GL/glew.h>
#include <iostream>
#include <rtr/camera/Camera.h>
#include <rtr/animation/animate/model.h>
#include <rtr/animation/camera/camera.h>

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
	// Camera camera; 
	animation_Camera camera;

	GLfloat last_x;
	GLfloat last_y;
	GLfloat last_frame = 0.0f;
	GLfloat delta_time = 0.0f;

	GLboolean mouse_first_in;

	//animated model shader
	GLuint shade_model;

	//animation model instances
	Model eagle;
	Model astroboy;

	glm::mat4 MVP;
	glm::mat4 perspective_view;
	glm::mat4 perspective_projection;
	glm::mat4 matr_model_1;
	glm::mat4 matr_model_2;

	glm::mat4 model_;

	void init();
	void render();
	void update(int32_t width, int32_t height);
	void _MVP(Camera _camera, glm::mat4 Model_, glm::mat4 View_, glm::mat4 Projection_);
	
	static GLuint loadImageToTexture(const char* image_path);
	static GLuint loadDDS(const char* image_path, int* w = nullptr, int* h = nullptr);

};