#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GL/freeglut.h>
#include <glm/gtc/matrix_transform.hpp>

#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h> 

#include <cmath>
#include <string>
#include <vector>
#include <chrono>
#include <cstring>
#include <stdio.h>
#include <stdlib.h> 
#include <iostream>

#include <rtr/animation/loader/loader.h>
#include <rtr/animation/display/display.h>

#include "rtr/terrain/Terrain.h"
#include "rtr/terrain/Skybox.h"
#include "rtr/shaders/SimpleShaders.h"
#include "rtr/shaders/TerrainShaders.h"
#include "rtr/shaders/SkyboxShaders.h"
#include "rtr/camera/Camera.h"

#include <rtr/ren/headers.hpp>
#include <rtr/ren/particleHeaders.hpp>
#include <rtr/shaders/TextureShaders.h>
#include <rtr/shaders/ParticleShaders.h>
#include <rtr/object/object.hpp>

Loader loader;

//high resolution clock used to establish time points reletive to each frame
using namespace std::chrono;
typedef high_resolution_clock Clock;
std::chrono::duration<double> last_frame;

uint64_t elapsed_ms_{0};

using namespace glm;
// Dimensions of the windows
 int width = 1280;
 int height = 720;

const int stretch = 200;
// TERRAINRESOLUTION TO 512 ON NORMAL PCS/LAPTOPS, 2048 ON VR LAB PCS
const int terrainResolution = 512;	// Size of the terrain
const int tileNumber = 20;			// No of tiles of terrain => 12 texture tiles
const int subdivide = 1;
float amplitude = 50.0f;			// Amplitude for noise function
float frequency = 0.0083f;			// frequency for noise function
const int terrainTextureRes = 512;	// Resolution of the texture images

float elapsedTime = 0.0;							// for moving the terrain, measure elapsed time
const float speed = 2.0;			// Speed of moving terrain
vec3 terrainTransl;					// vector for translating terrain in the shader
float oldTime = 0.0;
float mSpeed = 0.0;

const char* terrainFrag = "../resources/shaders/TerrainShader.frag";
const char* terrainVert = "../resources/shaders/TerrainShader.vert";
const char* skyboxFrag = "../resources/shaders/skybox.frag";
const char* skyboxVert = "../resources/shaders/skybox.vert";
std::vector<std::string> textures = { "../resources/textures/Terrain/Mud.png", "../resources/textures/Terrain/grassSeemless.png", "../resources/textures/Terrain/Mountain1.png" };
std::vector<std::string> skyboxTextures = {"../resources/textures/Skybox/right.png", "../resources/textures/Skybox/left.png", "../resources/textures/Skybox/top.png", "../resources/textures/Skybox/bottom.png", "../resources/textures/Skybox/back.png", "../resources/textures/Skybox/front.png" };

const vec3 terrainCenter = vec3(static_cast<float>((terrainResolution + stretch) / 2.0), 0.0f, static_cast<float>((terrainResolution + stretch) / 2.0)); //center of terrain
const vec3 cameraPosition = vec3(0.0f, 60.0f, -5.0f);

const vec3 sunDirection = normalize(vec3(0.5f, 100.0f, 0.5f));	// Direction of sunlight in world space

Terrain* terrain = nullptr;
TerrainShaders* terrainShaders = nullptr;
Skybox* skybox = nullptr;
SkyboxShaders* skyboxShaders = nullptr;

float rotationXAngle = 0.0f;	// rotation angles for camera movement
float rotationYAngle = 0.0f;
Camera* camera;

std::string obj_file_ = "../resources/objects/dragon_fly.obj";
//std::string obj_file_ = "../resources/objects/DRAGNFLY.OBJ";

Object* dragonfly;

//what we need for character control
glm::fvec3 move_dir_{0.0f, 0.0f, 1.0f};		//vector representing the direction in which we move
float move_speed = 0.01f;
float rot_angle_ = 0.01f;
//rotation axis we need when using wasd
glm::vec3 rot_axis_ws_{1.0, 0.0, 0.0};
glm::vec3 rot_axis_ad_{0.0, 1.0, 0.0};
glm::vec3 rot_axis_qe_{0.0, 0.0, 1.0};


SimpleShaders* shader_;
TextureShaders* texture_shader_;
FlameThrowerEffect* fire_;

float scale_ = 50.0f;
glm::fvec3 drgnfly_pos{0.0f, 80.0f, 0.0f};
glm::fvec3 fire_pos = {0.0f, 0.008f, 0.01f};
glm::fvec3 fire_dir{0.0f, -0.5f, 1.0f};

bool render_obj = true, render_effect = true, render_terrain = true, move_obj = false;

std::map<unsigned char, bool> keys_;


// If mouse is moves in direction (x,y)
void mouseMotion(int x, int y)
{
	camera->mouseRotate(x, y);
	glutPostRedisplay();
}

// if mouse button button is pressed
void mouse(int button, int state, int x, int y)
{
	camera->mouseButton(button, state, x, y);
	glutPostRedisplay();
}

void glut_timer(int32_t _e)
{
  	glutPostRedisplay();
  	glutTimerFunc(16, glut_timer, 1);
  	elapsed_ms_ += 16;
}

//on resize window, re-create projection matrix
void glut_resize(int32_t _width, int32_t _height) {
  width = _width;
  height = _height;
  camera->updateProjection(width/(float)height);
}

// Transformation of terrain with angles for x and y axis
mat4 calcTerrainTransformation(float rotationXAngle, float rotationYAngle)
{
	mat4 transformMatrix = mat4(1.0f);
	transformMatrix = rotate(transformMatrix, rotationYAngle, vec3(0.0f, 1.0f, 0.0f));
	transformMatrix = rotate(transformMatrix, rotationXAngle, vec3(1.0f, 0.0f, 0.0f));
	transformMatrix = translate(transformMatrix, -terrainCenter);
	return transformMatrix;
}

void rotate_drgnfly_fire(glm::fvec3 rot_axis, float angle){
	dragonfly->rotate(rot_axis, angle);
	glm::fvec3 new_fire_pos = glm::fvec3{dragonfly->rotation_matrix_ * glm::fvec4{fire_pos, 1.0f}};
	glm::fvec3 new_fire_dir = glm::fvec3{dragonfly->rotation_matrix_ * glm::fvec4{fire_dir, 1.0f}};
	fire_->setPos(new_fire_pos * scale_ + drgnfly_pos);
	fire_->setDir(new_fire_dir);
}

void translate_drgnfly_fire(glm::fvec3 transl){
	dragonfly->translate(transl);
	drgnfly_pos += transl;
	glm::fvec3 new_fire_pos = glm::fvec3{dragonfly->rotation_matrix_ * glm::fvec4{fire_pos, 1.0f}};
	fire_->setPos(new_fire_pos * scale_ + drgnfly_pos);
}

void display(void)
{
  glViewport(0, 0, (GLsizei)width, (GLsizei)height);

  glClearColor(0.22f, 0.28f, 0.31f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_BLEND);

	// update camera
	//camera->update();
	camera->setViewDir(drgnfly_pos - cameraPosition);
	if(render_terrain){
		
		mat4 worldMatrix = calcTerrainTransformation(rotationXAngle, rotationYAngle);
	
		// render terrain
		terrainShaders->setModelMatrix(worldMatrix * mat4(1.0));	
		terrainShaders->setViewMatrix(camera->getViewMatrix());
		terrainShaders->setProjectionMatrix(camera->getProjectionMatrix());
		terrainShaders->setAmplitude(amplitude);
		terrainShaders->setFrequency(frequency);
		
		terrainShaders->setTime(terrainTransl);
		terrainShaders->activate();
		terrain->draw();
	
		glDepthFunc(GL_LEQUAL);								// change depth function so depth test passes when values are equal to depth buffer's content
		mat4 viewMat = mat4(mat3(camera->getViewMatrix())); // remove translation from the view matrix
		skyboxShaders->setViewMatrix(viewMat);
		skyboxShaders->setProjectionMatrix(camera->getProjectionMatrix());
		skyboxShaders->activate();
		skybox->draw();
		glDepthFunc(GL_LESS);
	}  
									// set depth function back to default 
	
	if(render_obj){

		if(move_obj){
			glm::fvec3 dir_ = glm::fvec3{dragonfly->rotation_matrix_ * glm::fvec4{move_dir_, 1.0f}} ;
	    	translate_drgnfly_fire(glm::fvec3{0.0f, dir_.y * move_speed, 0.0f});
	    	terrainTransl += vec3(dir_.x * move_speed, 0.0, dir_.z * move_speed);
		}
		shader_->activate();
	
		//upload model, camera and projection matrices to GPU (1 matrix, transposed, address beginnings of data block)
		glUniformMatrix4fv(shader_->getUniform("model_matrix"), 1, GL_FALSE, dragonfly->get_model_matrix()); 
		glUniformMatrix4fv(shader_->getUniform("camera_matrix"), 1, GL_FALSE, glm::value_ptr(camera->getViewMatrix())); 
		glUniformMatrix4fv(shader_->getUniform("projection_matrix"), 1, GL_FALSE, glm::value_ptr(camera->getProjectionMatrix()));
		
		//loader.MVP(camera, dragonfly->get_model_matrix(), camera->getViewMatrix(), camera->getProjectionMatrix());
		loader.update(width, height);

		//bind the VBO of the model such that the next draw call will render with these vertices
		dragonfly->activate();
	
		//draw triangles from the currently bound buffer
		dragonfly->draw();
		dragonfly->deactivate();

	}

	if(render_effect){
		glEnable(GL_BLEND);
	
		texture_shader_->activate();
		texture_shader_->setCameraMatrix(camera->getViewMatrix());
		texture_shader_->setProjectionMatrix(camera->getProjectionMatrix());
		texture_shader_->setAtlasDim();
	
		fire_->cpuUpdate(16.f/1000.f);
		fire_->gpuUpdate();
		fire_->render();
	
		glDisable(GL_BLEND);
	}
	
	loader.render();
	glutSwapBuffers();
}

// Rotate terrain with arrow keys
void special(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_UP:
		rotationXAngle -= 3.0f;
		break;
	case GLUT_KEY_DOWN:
		rotationXAngle += 3.0f;
		break;
	case GLUT_KEY_LEFT:
		rotationYAngle -= 3.0f;
		break;
	case GLUT_KEY_RIGHT:
		rotationYAngle += 3.0f;
		break;
	}
	calcTerrainTransformation(rotationXAngle, rotationYAngle);
	glutPostRedisplay();
}

vec3 simulateMovement() {
	float maxSpeed = -0.5;
	float deltaTime = elapsedTime - oldTime;
	mSpeed -= speed * deltaTime;
	//printf("%f, ", mSpeed);
	if (mSpeed < maxSpeed)
		mSpeed = maxSpeed;
	vec3 mDirection = vec3(sin(2.0) * cos(4.0), cos(2.0), sin(2.0) * sin(4.0));
	oldTime = elapsedTime;
	return mSpeed * mDirection;
}

void init_keys(){
	keys_['w'] = false;
	keys_['a'] = false;
	keys_['s'] = false;
	keys_['d'] = false;
	keys_['e'] = false;
	keys_['q'] = false;
	keys_['c'] = false;
	keys_['r'] = false;
	keys_['f'] = false;
	keys_['v'] = false;
	keys_[' '] = false;
	keys_['x'] = false;
	keys_['y'] = false;
	keys_['t'] = false;
}

// move terrain with WASD
void keyboard()
{
	//TODO: Set mSpeed = 0 when wasd is released
	//time = static_cast<float>(glutGet(GLUT_ELAPSED_TIME) / 1000000.0);
	elapsedTime += 0.001;
	
	if (keys_.at('w')){
    	rotate_drgnfly_fire(rot_axis_ws_, rot_angle_);
		//terrainTransl += vec3(0.0, 0.0, simulateMovement().z);
	}
	if (keys_.at('a')){
    	rotate_drgnfly_fire(rot_axis_ad_, rot_angle_);
    	rotate_drgnfly_fire(rot_axis_qe_, -rot_angle_/3);
		//terrainTransl += vec3(simulateMovement().x, 0.0, 0.0);
	}
	if (keys_.at('s')){
    	rotate_drgnfly_fire(rot_axis_ws_, -rot_angle_);
		//terrainTransl -= vec3(0.0, 0.0, simulateMovement().z);
	}
	if (keys_.at('d')){
    	rotate_drgnfly_fire(rot_axis_ad_, -rot_angle_);
    	rotate_drgnfly_fire(rot_axis_qe_, rot_angle_/3);
		//terrainTransl -= vec3(simulateMovement().x, 0.0, 0.0);
	}
    if (keys_.at('q')){
    	rotate_drgnfly_fire(rot_axis_qe_, -rot_angle_);
    }
    if (keys_.at('e')){
    	rotate_drgnfly_fire(rot_axis_qe_, rot_angle_);
    }
	if (keys_.at('c')){
      	camera->stop();
    }
    if (keys_.at('r')){
    	render_obj = !render_obj;
    }
    if (keys_.at('f')){
    	render_terrain = !render_terrain;
    }
    if (keys_.at('v')){
    	render_effect = !render_effect;
    }
    if (keys_.at(' ')){
    	move_obj = !move_obj;
    }
    if (keys_.at('x')){
    	move_speed += 0.05f;
    }
    if (keys_.at('y')){
    	move_speed -= 0.05f;
    }	
    if (keys_.at('t')){
		terrainShaders->loadVertexFragmentShaders(terrainVert, terrainFrag);
		terrainShaders->locateUniforms();
	}		
	
	glutPostRedisplay();

}

// move terrain with WASD
void keyboarddown(unsigned char key, int x, int y)
{
	//TODO: Set mSpeed = 0 when wasd is released
	//time = static_cast<float>(glutGet(GLUT_ELAPSED_TIME) / 1000000.0);
	keys_[key] = true;
	keyboard();
}

// move terrain with WASD
void keyboardup(unsigned char key, int x, int y)
{
	//TODO: Set mSpeed = 0 when wasd is released
	//time = static_cast<float>(glutGet(GLUT_ELAPSED_TIME) / 1000000.0);
	keys_[key] = false;
	keyboard();
}

int main(int argc, char** argv)
{
	init_keys();

	// Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(width, height);

	if (glutCreateWindow("rtr_terrain") == 0)
	{
		printf("Glut init failed\n");
		return -1;
	}

	glutDisplayFunc(display);
	glutSpecialFunc(special);
	glutIdleFunc(display);
	glutKeyboardFunc(keyboarddown);
	glutKeyboardUpFunc(keyboardup);

	// Init glew so that the GLSL functionality will be available
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		printf("GLEW init failed!\n");
		return -1;
	}

	//initialize DeviL -- utilized for loading animation diffuse textures
	ilInit(); 
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);
	ilEnable(IL_ORIGIN_SET); 
	ilOriginFunc(IL_ORIGIN_UPPER_LEFT);

	loader.init();
	
	camera = new Camera((width/(float)height), cameraPosition);
	//initialize animation loading


	// Terrain  
	terrain = new Terrain(terrainResolution, tileNumber, subdivide, stretch);
	terrainShaders = new TerrainShaders(terrainTextureRes, textures, sunDirection, amplitude, frequency, terrainResolution, tileNumber, subdivide);
	terrainShaders->loadVertexFragmentShaders(terrainVert, terrainFrag);
	terrainShaders->locateUniforms();
	
	//Moving terrain
	terrainTransl = vec3(0.0);

	//Skybox
	skybox = new Skybox(terrainResolution);
	skyboxShaders = new SkyboxShaders(terrainTextureRes, skyboxTextures);
	skyboxShaders->loadVertexFragmentShaders(skyboxVert, skyboxFrag);
	skyboxShaders->locateUniforms();

	shader_ = new SimpleShaders();


	shader_->loadVertexFragmentShaders("../resources/shaders/tutorial.vert", "../resources/shaders/tutorial.frag"); 
	shader_->addUniform("model_matrix");
	shader_->addUniform("camera_matrix");
	shader_->addUniform("projection_matrix");

  	texture_shader_ = new TextureShaders(400, "../resources/textures/explosion.png", 8);
	texture_shader_->loadVertGeomFragShaders("../resources/shaders/particleQuad.vert", "../resources/shaders/particleFire.geom", "../resources/shaders/particleFire.frag");
  	texture_shader_->locateUniforms();
  	
  	

  	dragonfly = new Object(obj_file_, (model::POSITION | model::TEXCOORD | model::NORMAL));
	dragonfly->scale(scale_);
	dragonfly->translate(drgnfly_pos);

	

	fire_ = new FlameThrowerEffect();
	fire_->init(10000, camera);
	fire_->initRenderer();

	fire_->setPos(fire_pos * scale_ + drgnfly_pos);
	fire_->setDir(fire_dir);

	
	// Camera 
	glutMotionFunc(mouseMotion);
	glutMouseFunc(mouse);
  	glutReshapeFunc(glut_resize);


	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_DEPTH_TEST);
	
	glutMainLoop();
	return 0;
}

