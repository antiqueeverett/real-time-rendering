#pragma once

#include <GL/glew.h>
#include <iostream>
#include "opengl/camera.h"
#include "collada_parser/model.h"

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
	Vector2D mouse_position;
	GLboolean mouse_first_in;
	animation_Camera camera;

	GLuint shaders_animated_model;
	Model model_man;
	Model model_astroboy;

	glm::mat4 MVP;
	glm::mat4 perspective_view;
	glm::mat4 perspective_projection;
	glm::mat4 matr_model_1;
	glm::mat4 matr_model_2;

	void init();
	void update();
	void render();
	void playSound();
	
	static GLuint loadImageToTexture(const char* image_path);
	static GLuint loadDDS(const char* image_path, int* w = nullptr, int* h = nullptr);

};