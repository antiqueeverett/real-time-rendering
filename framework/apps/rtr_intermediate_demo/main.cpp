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
const int tileNumber = 32;			// No of tiles of terrain => 12 texture tiles
const int subdivide = 1;
float amplitude = 50.0f;			// Amplitude for noise function
float frequency = 0.0083f;			// frequency for noise function
const int terrainTextureRes = 512;	// Resolution of the texture images

float elapsedTime = 0.0;			// for moving the terrain, measure elapsed time
const float speed = 0.015;			// Speed of moving terrain
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
//what we need for character control
glm::fvec3 move_dir_{0.0f, 0.0f, 1.0f};		//vector representing the direction in which we move

float accumulated_movement_speed_ = 0.0f;
uint64_t num_input_frames_passed = 0u;

float move_speed = 0.01f;
float rot_angle_ = 0.01f;
float tilt_angle_ = 0.0f;
float max_tilt_angle_ = 1.5f;

bool gear_button_pressed = false, gear_button_released = false;
bool break_button_pressed = false, break_button_released = false;
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

const glm::fvec3 min_pos_rings_ {-700.0f, -40.f, -700.f},
           		 max_pos_rings_ {700.0f, 150.f, 700.f};

float scale_ = 50.0f;
float eagle_rotation_ = -1.f;
glm::fvec3 drgnfly_pos{0.0f, 80.0f, 0.0f};
bool render_obj = true, render_effect = true, render_terrain = true, move_obj = false;

//controls
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
	drgnfly_pos += transl;
	if(drgnfly_pos.y < 20.0f || drgnfly_pos.y > 200.f){
		drgnfly_pos -= glm::fvec3{0.0f, transl.y, 0.0f};
		transl = glm::fvec3{transl.x, 0.0f, transl.z};
	}
	dragonfly->translate(transl);
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

    if(joy->getButtonState(BUTTON_RIGHT_BUMPER) == 1){
			break_button_pressed = true;
		} else if (break_button_pressed){
      		break_button_released = true;
    }

		joypad_input_mutex.lock();
		accumulated_rotation_ = glm::rotate(glm::fmat4{}, 
											tilt_angle_, 
											rot_axis_qe_);

		accumulated_movement_speed_ += ((joy->getAxisState(AXIS_RIGHT_TRIGGER) / 65534.f) + 0.5f);

		accumulated_left_stick_horizontal_axis_state += joy->getAxisState(AXIS_LEFT_STICK_HORIZONTAL);
		accumulated_left_stick_vertical_axis_state 	 += joy->getAxisState(AXIS_LEFT_STICK_VERTICAL);

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

void checkBoundOfRings(FireRing* ring){
  if(ring->m_pos.x < min_pos_rings_.x||ring->m_pos.x > max_pos_rings_.x || ring->m_pos.z < min_pos_rings_.z||ring->m_pos.z > max_pos_rings_.z){
    float x = glm::linearRand(min_pos_rings_.x, max_pos_rings_.x);
    float y = glm::linearRand(min_pos_rings_.y, max_pos_rings_.y);
    float z = glm::linearRand(min_pos_rings_.z, max_pos_rings_.z);
    ring->reset();
    ring->move(-(ring->m_pos));
    ring->move(glm::fvec3(x,y,z));
    ring->m_pos = glm::fvec3(x,y,z);
    ring->setColor(glm::fvec3{0.9f, 0.1f, 0.01f});
  } else if(glm::distance(drgnfly_pos, ring->m_pos) < 6.0f) {
    ring->setColor(glm::fvec3{0.0f, 0.1f, 1.0f});
  }
  std::cout << glm::distance(drgnfly_pos, ring->m_pos) << " , " << ring->m_pos.x << " , " << ring->m_pos.z << std::endl;
}

void display(void){

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
    terrainShaders->setDayTime(timeinMS*0.33);
    terrainShaders->setSunRotation(rotate(mat4(1.0), rotation, vec3(0.0f, 1.0f, 0.0f)));
    terrainShaders->activate();
    terrain->draw();
  
    glDepthFunc(GL_LEQUAL);               // change depth function so depth test passes when values are equal to depth buffer's content
    mat4 viewMat = mat4(mat3(camera->getViewMatrix())); // remove translation from the view matrix
    mat4 rotateView = rotate(mat4(1.0f), rotation, vec3(0.0f, 1.0f, 0.0f));
    skyboxShaders->setViewMatrix(viewMat, rotateView);
    skyboxShaders->setProjectionMatrix(camera->getProjectionMatrix());
    skyboxShaders->setTime(timeinMS*0.33);
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

  if(break_button_pressed && break_button_released){
      --gear;
      gear = gear % 6;
      if(gear == 0)
        gear = 1;
    break_button_pressed = false;
    break_button_released = false;
  }


  //update dragonfly speed (average)
  if(0 != num_input_frames_passed) {
    move_speed = gear* (accumulated_movement_speed_ / num_input_frames_passed);
    accumulated_left_stick_horizontal_axis_state /= num_input_frames_passed;
    accumulated_left_stick_vertical_axis_state /= num_input_frames_passed;
  } else {
    move_speed = 0;
  }

  dragonfly->rotate(rot_axis_ad_, -rot_angle_* 2.0f * accumulated_left_stick_horizontal_axis_state / 32767.f);
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
          checkBoundOfRings(i);
        }
    }
  
    dragonfly->rotation_matrix_ *= rotation_;
    dragonfly->rotate(glm::fvec3(1.0f, 0.0f, 0.0f), eagle_rotation_);

    dragonfly->get_model_matrix();

    loader.update(width, height, *camera, dragonfly->model_matrix_, camera->getViewMatrix(), camera->getProjectionMatrix());
    loader.render();

    dragonfly->rotate(glm::fvec3(1.0f, 0.0f, 0.0f), -eagle_rotation_);
    dragonfly->rotation_matrix_ *= glm::inverse(rotation_);
  
  }

	if(render_effect){

		glEnable(GL_BLEND);
	 glBlendFunc (GL_SRC_ALPHA, GL_ONE);
		particle_shader_->activate();
		particle_shader_->setCameraMatrix(camera->getViewMatrix());
		particle_shader_->setProjectionMatrix(camera->getProjectionMatrix());
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

void init_scene(){
  joypad_input_mutex.lock();
  
  drgnfly_pos = glm::fvec3{0.0f, 80.0f, 0.0f};
  elapsedTime = 0.0;
  oldTime = 0.0;
  mSpeed = 0.0;
  rotation = 0.0f;
  terrainTransl = glm::fvec3{0.0f};

  delete camera;
  camera = new Camera((width/(float)height), cameraPosition);

  move_dir_ = glm::fvec3{0.0f, 0.0f, 1.0f}; 
  accumulated_movement_speed_ = 0.0f;
  num_input_frames_passed = 0u;

  move_speed = 0.01f;
  tilt_angle_ = 0.0f;
  gear_button_pressed = false;
  gear_button_released = false;
  gear = 1;
  accumulated_rotation_ = glm::fmat4{};
  rotation_ = glm::fmat4{};

  accumulated_left_stick_horizontal_axis_state = 0.0;
  accumulated_left_stick_vertical_axis_state = 0.0;

  delete dragonfly;
  dragonfly = new Object(obj_file_, (model::POSITION | model::TEXCOORD | model::NORMAL));
  dragonfly->translate(drgnfly_pos);
  dragonfly->scale(0.7f);

  for (auto i : rings_){
    i->setColor(glm::fvec3{0.9f, 0.1f, 0.01f});
  }

  joypad_input_mutex.unlock();

}

// move terrain with WASD
void keyboard(uint8_t _key, int32_t _x, int32_t _y) {
  switch (_key) {
    //escape key
    case 27:
      exit(0);
      break;
    case '.':
      glutFullScreenToggle();
      break;
    case ' ':
      init_scene();
      break;
    default:
      break;

  }

  glutPostRedisplay();
}

int main(int argc, char** argv)
{

	input_frame_time_end = std::chrono::system_clock::now();
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
	glutKeyboardFunc(keyboard);

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

  particle_shader_ = new ParticleShaders();
  particle_shader_->loadVertGeomFragShaders("../resources/shaders/particleQuad.vert", "../resources/shaders/particleQuad.geom", "../resources/shaders/particlePoint.frag"); 
  particle_shader_->locateUniforms();
    
  dragonfly = new Object(obj_file_, (model::POSITION | model::TEXCOORD | model::NORMAL));
  dragonfly->translate(drgnfly_pos);
  dragonfly->scale(0.7f);

  for(auto& i : rings_){
    i = new FireRing();
    i->init(1000, camera);
    i->initRenderer();
    float x = glm::linearRand(min_pos_rings_.x, max_pos_rings_.x);
    float y = glm::linearRand(min_pos_rings_.y, max_pos_rings_.y);
    float z = glm::linearRand(min_pos_rings_.z, max_pos_rings_.z);

    i->move(glm::fvec3(x,y,z));
  }
  
  // Camera 
  glutMotionFunc(mouseMotion);
  glutMouseFunc(mouse);
  glutReshapeFunc(glut_resize);

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glEnable(GL_DEPTH_TEST);
  
  glutMainLoop();
	return 0;
}