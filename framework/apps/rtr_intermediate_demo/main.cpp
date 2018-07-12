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
#include <thread>
#include <mutex>

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

#include <rtr/gamepad/joy.h>
#include <rtr/gamepad/updates.h>

#include <rtr/gamepad/ps3_mappings.h>

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

const int stretch = 0;
// TERRAINRESOLUTION TO 512 ON NORMAL PCS/LAPTOPS, 2048 ON VR LAB PCS
const int terrainResolution = 2048;	// Size of the terrain
const int tileNumber = 20;			// No of tiles of terrain => 12 texture tiles
const int subdivide = 1;
float amplitude = 50.0f;			// Amplitude for noise function
float frequency = 0.0083f;			// frequency for noise function
const int terrainTextureRes = 512;	// Resolution of the texture images

float elapsedTime = 0.0;							// for moving the terrain, measure elapsed time
const float speed = 0.025;			// Speed of moving terrain
vec3 terrainTransl;					// vector for translating terrain in the shader
float oldTime = 0.0;
float mSpeed = 0.0;
float rotation = 0.0f;

const char* terrainFrag = "../resources/shaders/TerrainShader.frag";
const char* terrainVert = "../resources/shaders/TerrainShader.vert";
const char* skyboxFrag = "../resources/shaders/skybox.frag";
const char* skyboxVert = "../resources/shaders/skybox.vert";
std::vector<std::string> textures = { "../resources/textures/Terrain/Mud.png", "../resources/textures/Terrain/grassSeemless.png", "../resources/textures/Terrain/Mountain1.png" };
std::vector<std::string> skyboxDayTextures = {"../resources/textures/Skybox/right.png", "../resources/textures/Skybox/left.png", "../resources/textures/Skybox/top.png", "../resources/textures/Skybox/bottom.png", "../resources/textures/Skybox/back.png", "../resources/textures/Skybox/front.png" };
std::vector<std::string> skyboxNightTextures = {"../resources/textures/Skybox/nightRight.png", "../resources/textures/Skybox/nightLeft.png", "../resources/textures/Skybox/nightTop.png", "../resources/textures/Skybox/nightBottom.png", "../resources/textures/Skybox/nightBack.png", "../resources/textures/Skybox/nightFront.png" };

const vec3 terrainCenter = vec3(static_cast<float>((terrainResolution + stretch) / 2.0), 0.0f, static_cast<float>((terrainResolution + stretch) / 2.0)); //center of terrain
const vec3 cameraPosition = vec3(0.0f, 60.0f, -5.0f);

const vec4 sunDirection = normalize(vec4(-1.0f, 100.0f, 0-1.0f, 0.0f));	// Direction of sunlight in world space

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
glm::mat4 model_matrix;

//what we need for character control
glm::fvec3 move_dir_{0.0f, 0.0f, 1.0f};		//vector representing the direction in which we move

float accumulated_movement_speed_ = 0.0f;
uint64_t num_input_frames_passed = 0u;

float move_speed = 0.01f;
float rot_angle_ = 0.01f;
float tilt_angle_ = 0.0f;
float max_tilt_angle_ = 1.5f;

bool gear_button_pressed = false, gear_button_released = false;
int gear = 1;

std::mutex joypad_input_mutex;

glm::fmat4 accumulated_rotation_{};
glm::fmat4 rotation_ {};

double accumulated_left_stick_horizontal_axis_state = 0.0;
double accumulated_left_stick_vertical_axis_state = 0.0;

std::chrono::time_point<std::chrono::system_clock> input_frame_time_end, input_frame_time_start;

//rotation axis we need when using wasd
glm::vec3 rot_axis_ws_{1.0, 0.0, 0.0};
glm::vec3 rot_axis_ad_{0.0, 1.0, 0.0};
glm::vec3 rot_axis_qe_{0.0, 0.0, 1.0};


SimpleShaders* shader_;
TextureShaders* texture_shader_;
ParticleShaders* particle_shader_;
FireRing* fire_;
std::vector<FireRing*> rings_(50, new FireRing());

glm::fvec3 min_pos_rings_ {-700.0f, -70.f, -700.f},
           max_pos_rings_ {700.0f, 200.f, 700.f};

float scale_ = 50.0f;
float eagle_rotation_ = -1.f;
glm::fvec3 drgnfly_pos{0.0f, 80.0f, 0.0f};
glm::fvec3 fire_pos = {0.0f, 0.008f, 0.01f};
glm::fvec3 fire_dir = -move_dir_;

bool render_obj = true, render_effect = true, render_terrain = true, move_obj = false;

//controls
std::map<unsigned char, float> keys_;
Joystick* joy = new Joystick("/dev/input/js1");


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

void translate_drgnfly_fire(glm::fvec3 transl){
	dragonfly->translate(transl);
	drgnfly_pos += transl;
	//glm::fvec3 new_fire_pos = glm::fvec3{dragonfly->rotation_matrix_ * glm::fvec4{fire_pos, 1.0f}};
	//fire_->setPos(new_fire_pos * scale_ + drgnfly_pos);
}

// move terrain with WASD
void keyboard()
{

	/*
	//TODO: Set mSpeed = 0 when wasd is released
	//elapsedTime = static_cast<float>(glutGet(GLUT_ELAPSED_TIME) / 1000000.0);
	elapsedTime = static_cast<float>(1.0f/ subdivide);


	if (keys_.at('w')){
    	rotate_drgnfly_fire(rot_axis_ws_, rot_angle_*keys_.at('w'));
		//terrainTransl += vec3(0.0, 0.0, simulateMovement().z);
		terrainTransl += vec3(elapsedTime, 0.0, 0.0);
	}
	if (keys_.at('a')){
    	rotate_drgnfly_fire(rot_axis_ad_, rot_angle_*keys_.at('a'));
    	//rotate_drgnfly_fire(rot_axis_qe_, -rot_angle_/3);
		//terrainTransl += vec3(simulateMovement().x, 0.0, 0.0);
		terrainTransl -= vec3(0.0, 0.0, elapsedTime);
	}
	if (keys_.at('s')){
    	rotate_drgnfly_fire(rot_axis_ws_, -rot_angle_*keys_.at('s'));
		//terrainTransl -= vec3(0.0, 0.0, simulateMovement().z);
		terrainTransl -= vec3(elapsedTime, 0.0, 0.0);
	}
	if (keys_.at('d')){
    	rotate_drgnfly_fire(rot_axis_ad_, -rot_angle_*keys_.at('d'));
    	//rotate_drgnfly_fire(rot_axis_qe_, rot_angle_/3);
		//terrainTransl -= vec3(simulateMovement().x, 0.0, 0.0);
		terrainTransl += vec3(0.0, 0.0, elapsedTime);
	}
    if (keys_.at('q')){
    	rotate_drgnfly_fire(rot_axis_qe_, -rot_angle_*keys_.at('q'));
    }
    if (keys_.at('e')){
    	rotate_drgnfly_fire(rot_axis_qe_, rot_angle_*keys_.at('e'));
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
	*/
	glutPostRedisplay();

}

float joystick_transfer(AxisId id){
	float input = joy->getAxisState(id);
	if(input){
		return pow((input / 32767.f), 5);
	} else { return 0.0f;}
}

void joystick(){
	input_frame_time_start = std::chrono::system_clock::now();
	while(true) {

    	float elapsed_microseconds_since_last_frame = std::chrono::duration_cast<std::chrono::milliseconds>
                             							 (input_frame_time_end-input_frame_time_start).count();

		input_frame_time_start = std::chrono::system_clock::now();
		joy->Update();
		
		if(joy->getAxisState(AXIS_LEFT_STICK_HORIZONTAL) == 0 && tilt_angle_ != 0.0f){
			tilt_angle_ *= 0.97f;
			if(abs(tilt_angle_) < 0.01f){tilt_angle_ = 0.0f;}
		} else if(joy->getAxisState(AXIS_LEFT_STICK_HORIZONTAL) > 0){
			tilt_angle_ += 0.02f * joystick_transfer(AXIS_LEFT_STICK_HORIZONTAL);
			tilt_angle_ = std::min(tilt_angle_, 1.0f);
		} else {
			tilt_angle_ += 0.02f * joystick_transfer(AXIS_LEFT_STICK_HORIZONTAL);
			tilt_angle_ = std::max(tilt_angle_, -1.0f);

		}

		if(joy->getButtonState(BUTTON_LEFT_BUMPER) == 1){
			gear_button_pressed = true;
		} else if (gear_button_pressed){
      gear_button_released = true;
    }

		joypad_input_mutex.lock();
		accumulated_rotation_ = glm::rotate(glm::fmat4{}, 
											tilt_angle_, 
											rot_axis_qe_);

		accumulated_movement_speed_ += ((joy->getAxisState(AXIS_RIGHT_TRIGGER) / 65534.f) + 0.5f);

		accumulated_left_stick_horizontal_axis_state += joy->getAxisState(AXIS_LEFT_STICK_HORIZONTAL);
		accumulated_left_stick_vertical_axis_state 	 += joy->getAxisState(AXIS_LEFT_STICK_VERTICAL);
		//rotation_ = glm::rotate(glm::fmat4{}, tilt_angle_, rot_axis_qe_);
	    //rotate_drgnfly_fire(rot_axis_ad_, -rot_angle_*(joy->getAxisState(AXIS_LEFT_STICK_HORIZONTAL) / 32767.f));
	    //rotate_drgnfly_fire(rot_axis_ws_, -rot_angle_*(joy->getAxisState(AXIS_LEFT_STICK_VERTICAL) / 32767.f));
	  	//move_speed = 0.9f * ((joy->getAxisState(AXIS_RIGHT_TRIGGER) / 65534.f) + 0.5f);


		++num_input_frames_passed;

	  	joypad_input_mutex.unlock();
 		std::this_thread::sleep_for(10ms);
		input_frame_time_end = std::chrono::system_clock::now();
	}

}

void glut_timer(int32_t _e)
{
  	glutPostRedisplay();
  	glutTimerFunc(1, glut_timer, 1);
  	elapsed_ms_ += 1;
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



void display(void){
  //joystick();

  glViewport(0, 0, (GLsizei)width, (GLsizei)height);

  glClearColor(0.22f, 0.28f, 0.31f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_BLEND);

	float timeinMS = glutGet(GLUT_ELAPSED_TIME);
	float timeSinceStart = static_cast<float>(glutGet(GLUT_ELAPSED_TIME) / 1000.0f); // time since glutInit in s
	float mDeltaTime = timeSinceStart - oldTime;
	oldTime = timeSinceStart;
	rotation += speed* mDeltaTime;


	// update camera
	//camera->update();
	camera->setPosition(drgnfly_pos - 10.0f * glm::fvec3{dragonfly->rotation_matrix_ * glm::fvec4{move_dir_, 1.0f}} + glm::fvec3{0.0f, 7.0f, 0.0f});
	camera->setViewDir((drgnfly_pos + 2.f*glm::fvec3{dragonfly->rotation_matrix_ * glm::fvec4{rot_axis_qe_, 1.0f}}) - camera->getPosition());

	if(render_terrain){
		
		mat4 worldMatrix = calcTerrainTransformation(rotationXAngle, rotationYAngle);
	
		// render terrain
		terrainShaders->setModelMatrix(worldMatrix * mat4(1.0));	
		terrainShaders->setViewMatrix(camera->getViewMatrix());
		terrainShaders->setProjectionMatrix(camera->getProjectionMatrix());
	
		terrainShaders->setTime(terrainTransl);
		terrainShaders->setDayTime(timeinMS*0.5);
		terrainShaders->setSunRotation(rotate(mat4(1.0), rotation, vec3(0.0f, 1.0f, 0.0f)));
		terrainShaders->activate();
		terrain->draw();
	
		glDepthFunc(GL_LEQUAL);								// change depth function so depth test passes when values are equal to depth buffer's content
		mat4 viewMat = mat4(mat3(camera->getViewMatrix())); // remove translation from the view matrix
		mat4 rotateView = rotate(viewMat, rotation, vec3(0.0f, 1.0f, 0.0f));
		skyboxShaders->setViewMatrix(rotateView, mDeltaTime);
		skyboxShaders->setProjectionMatrix(camera->getProjectionMatrix());
		skyboxShaders->setTime(timeinMS*0.5);
		skyboxShaders->activate();
		skybox->draw();
		glDepthFunc(GL_LESS);
	}  
									// set depth function back to default 
	

	joypad_input_mutex.lock();
	//update dragonfly roll
	rotation_ = accumulated_rotation_;
	accumulated_rotation_ = glm::fmat4{};

	if(gear_button_pressed && gear_button_released){
			++gear;
			gear = gear % 6;
			if(gear == 0)
				gear = 1;
		gear_button_pressed = false;
    gear_button_released = false;
	}


	//update dragonfly speed (average)
	if(0 != num_input_frames_passed) {
		move_speed = gear* (accumulated_movement_speed_ / num_input_frames_passed);
		accumulated_left_stick_horizontal_axis_state /= num_input_frames_passed;
		accumulated_left_stick_vertical_axis_state /= num_input_frames_passed;
	} else {
		move_speed = 0;
	}

	dragonfly->rotate(rot_axis_ad_, -rot_angle_* accumulated_left_stick_horizontal_axis_state / 32767.f);
	dragonfly->rotate(rot_axis_ws_, -rot_angle_* accumulated_left_stick_vertical_axis_state / 32767.f);

	accumulated_left_stick_horizontal_axis_state = 0.0;
	accumulated_left_stick_vertical_axis_state = 0.0;
	accumulated_movement_speed_ = 0.0f;
	
	num_input_frames_passed = 0;
	
	joypad_input_mutex.unlock();

	if(render_obj){

		if(move_speed > 0.001f){
			glm::fvec3 dir_ = glm::fvec3{dragonfly->rotation_matrix_ * glm::fvec4{move_dir_, 1.0f}} ;
	    	translate_drgnfly_fire(glm::fvec3{0.0f, dir_.y * move_speed, 0.0f});
	    	terrainTransl += vec3(dir_.x * move_speed, 0.0, dir_.z * move_speed);
        for(auto i : rings_){ 
          i->move(-glm::fvec3(dir_.x * move_speed, 0.0, dir_.z * move_speed));
        }
		}
		shader_->activate();
	
		dragonfly->rotation_matrix_ *= rotation_;
		dragonfly->rotate(glm::fvec3(1.0f, 0.0f, 0.0f), eagle_rotation_);

		//upload model, camera and projection matrices to GPU (1 matrix, transposed, address beginnings of data block)
		//glUniformMatrix4fv(shader_->getUniform("model_matrix"), 1, GL_FALSE, dragonfly->get_model_matrix()); 
		//glUniformMatrix4fv(shader_->getUniform("camera_matrix"), 1, GL_FALSE, glm::value_ptr(camera->getViewMatrix())); 
		//glUniformMatrix4fv(shader_->getUniform("projection_matrix"), 1, GL_FALSE, glm::value_ptr(camera->getProjectionMatrix()));
		
		//loader.MVP(camera, dragonfly->get_model_matrix(), camera->getViewMatrix(), camera->getProjectionMatrix());
		// --- depricating .....loader.update(width, height);

		dragonfly->get_model_matrix();

		loader.update(width, height, *camera, dragonfly->model_matrix_, camera->getViewMatrix(), camera->getProjectionMatrix());
		loader.render();

		dragonfly->rotate(glm::fvec3(1.0f, 0.0f, 0.0f), -eagle_rotation_);
		dragonfly->rotation_matrix_ *= glm::inverse(rotation_);
		//bind the VBO of the model such that the next draw call will render with these vertices
		//dragonfly->activate();
	
		//draw triangles from the currently bound buffer
		//dragonfly->draw();
		//dragonfly->deactivate();

	}

	if(render_effect){
		//fire_dir = -glm::fvec3{dragonfly->rotation_matrix_ * glm::fvec4{move_dir_, 1.0f}};
		//fire_dir.y = -fire_dir.y;
		//fire_->setDir(fire_dir);
		glEnable(GL_BLEND);
	
		particle_shader_->activate();
		particle_shader_->setCameraMatrix(camera->getViewMatrix());
		particle_shader_->setProjectionMatrix(camera->getProjectionMatrix());
		//texture_shader_->setAtlasDim();
	  for(auto i : rings_){
  		i->cpuUpdate(16.f/1000.f);
  		i->gpuUpdate();
  		i->render();
	  }
		glDisable(GL_BLEND);
	}
	
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
	keys_['w'] = 0.0f;
	keys_['a'] = 0.0f;
	keys_['s'] = 0.0f;
	keys_['d'] = 0.0f;
	keys_['e'] = 0.0f;
	keys_['q'] = 0.0f;
	keys_['c'] = 0.0f;
	keys_['r'] = 0.0f;
	keys_['f'] = 0.0f;
	keys_['v'] = 0.0f;
	keys_[' '] = 0.0f;
	keys_['x'] = 0.0f;
	keys_['y'] = 0.0f;
	keys_['t'] = 0.0f;
}



// move terrain with WASD
void keyboarddown(unsigned char key, int x, int y)
{
	//TODO: Set mSpeed = 0 when wasd is released
	//elapsedTime = static_cast<float>(1.0f/ subdivide);
	keys_[key] = 1.0f;
	keyboard();
}

// move terrain with WASD
void keyboardup(unsigned char key, int x, int y)
{
	//TODO: Set mSpeed = 0 when wasd is released
	//elapsedTime = static_cast<float>(1.0f/ subdivide);
	keys_[key] = 0.0f;
	keyboard();
}



int main(int argc, char** argv)
{

	input_frame_time_end = std::chrono::system_clock::now();

	init_keys();

	joy->Update();

	std::thread input_thread(joystick);


	// Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(width, height);

	if (glutCreateWindow("rtr_intermediate_demo") == 0)
	{
		printf("Glut init failed\n");
		return -1;
	}

	glutDisplayFunc(display);
	//glutSpecialFunc(special);
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

	loader.init(); //initialize animation
	camera = new Camera((width/(float)height), cameraPosition);
	 

	// Terrain  
	terrain = new Terrain(terrainResolution, tileNumber, subdivide, stretch);
	terrainShaders = new TerrainShaders(terrainTextureRes, textures, sunDirection, amplitude, frequency, terrainResolution, tileNumber, subdivide);
	terrainShaders->loadVertexFragmentShaders(terrainVert, terrainFrag);
	terrainShaders->locateUniforms();
	
	//Moving terrain
	terrainTransl = vec3(0.0);

	//Skybox
	skybox = new Skybox(terrainResolution, stretch);
	skyboxShaders = new SkyboxShaders(terrainTextureRes, skyboxDayTextures, skyboxNightTextures);
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

  particle_shader_ = new ParticleShaders();
  particle_shader_->loadVertGeomFragShaders("../resources/shaders/particleQuad.vert", "../resources/shaders/particleQuad.geom", "../resources/shaders/particlePoint.frag"); 
  particle_shader_->locateUniforms();
  	
  	dragonfly = new Object(obj_file_, (model::POSITION | model::TEXCOORD | model::NORMAL));
	
	dragonfly->translate(drgnfly_pos);
	//move_dir_ = glm::fvec3{glm::rotate(glm::fmat4{}, -eagle_rotation_, glm::fvec3(1.0f, 0.0f, 0.0f)) * glm::fvec4{move_dir_, 1.0f}};
	dragonfly->scale(0.7f);

	fire_ = new FireRing();
	fire_->init(100000, camera);
	fire_->initRenderer();

  for(auto& i : rings_){
    i = new FireRing();
    i->init(1000, camera);
    i->initRenderer();
    float x = glm::linearRand(min_pos_rings_.x, max_pos_rings_.x);
    float y = glm::linearRand(min_pos_rings_.y, max_pos_rings_.y);
    float z = glm::linearRand(min_pos_rings_.z, max_pos_rings_.z);

    i->move(glm::fvec3(x,y,z));
  }

	//fire_->setPos(fire_pos * scale_ + drgnfly_pos);
	//fire_->setDir(fire_dir);
	
	// Camera 
	glutMotionFunc(mouseMotion);
	glutMouseFunc(mouse);
  	glutReshapeFunc(glut_resize);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_DEPTH_TEST);
	
	glutMainLoop();
	return 0;
}