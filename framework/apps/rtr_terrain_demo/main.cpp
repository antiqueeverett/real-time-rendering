#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stdio.h>
#include <stdlib.h> 
#include <vector>

#include "rtr/terrain/Terrain.h"
#include "rtr/terrain/Skybox.h"
#include "rtr/shaders/SimpleShaders.h"
#include "rtr/shaders/TerrainShaders.h"
#include "rtr/shaders/SkyboxShaders.h"
#include "rtr/camera/Camera.h"

using namespace glm;

struct TerrainRegion{
	float min;
	float max;
	GLuint texture;
	const char* file;
};

// Dimensions of the windows
int width = 1280;
int height = 720;

const int terrainResolution = 512;	// Size of the terrain
const int tileNumber = 12;			// No of tiles of terrain => 12 texture tiles
const int subdivide = 1;
float amplitude = 80.0f;			// Amplitude for noise function
float frequency = 0.013f;			// frequency for noise function
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
std::vector<std::string> textures = { "../resources/textures/Terrain/Mountain1.png", "../resources/textures/Terrain/grassSeemless.png", "../resources/textures/Terrain/snow.png" };
std::vector<std::string> skyboxTextures = {"../resources/textures/Skybox/right.png", "../resources/textures/Skybox/left.png", "../resources/textures/Skybox/top.png", "../resources/textures/Skybox/bottom.png", "../resources/textures/Skybox/back.png", "../resources/textures/Skybox/front.png" };
const vec3 terrainCenter = vec3(static_cast<float>(terrainResolution / 2), 0.0f, static_cast<float>(terrainResolution / 2)); //center of terrain
const vec3 cameraPosition = vec3(0.0f, 100.0f, 0.0f);

Terrain* terrain = nullptr;
TerrainShaders* terrainShaders = nullptr;
Skybox* skybox = nullptr;
SkyboxShaders* skyboxShaders = nullptr;

float rotationXAngle = 0.0f;	// rotation angles for camera movement
float rotationYAngle = 0.0f;
Camera* camera;

//on resize window, re-create projection matrix
void glut_resize(int32_t _width, int32_t _height) {
  width = _width;
  height = _height;
  camera->updateProjection(width/(float)height);
}

// If mouse is moves in direction (x,y)
void mouseMotion(int x, int y)
{
	camera->mouseMove(x, y);
	glutPostRedisplay();
}

// if mouse button button is pressed
void mouse(int button, int state, int x, int y)
{
	camera->mouseButton(button, state, x, y);
	glutPostRedisplay();
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

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// update camera
	camera->update();
	
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
	glDepthFunc(GL_LESS);  								// set depth function back to default 
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

// move terrain with WASD
void keyboard(unsigned char key, int x, int y)
{
	//time = static_cast<float>(glutGet(GLUT_ELAPSED_TIME) / 1000000.0);
	//elapsedTime += 0.001;
	elapsedTime = static_cast<float>(1.0f/ subdivide);
	switch (key) {
	case 'w':
		terrainTransl += vec3(elapsedTime, 0.0, 0.0);
		//terrainTransl += vec3(simulateMovement().x, 0.0, 0.0);
		break;
	case 'a':
		//terrainTransl -= vec3(0.0, 0.0, simulateMovement().z);
		terrainTransl -= vec3(0.0, 0.0, elapsedTime);
		break;
	case 's':
		//terrainTransl -= vec3(simulateMovement().x, 0.0, 0.0);
		terrainTransl -= vec3(elapsedTime, 0.0, 0.0);
		break;
	case 'd':
		//terrainTransl += vec3(0.0, 0.0, simulateMovement().z);
		terrainTransl += vec3(0.0, 0.0, elapsedTime);
		break;
	case ' ':
      	camera->stop();
    	break;
	}
	glutPostRedisplay();

}



int main(int argc, char** argv)
{
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
	glutKeyboardFunc(keyboard);

	// Init glew so that the GLSL functionality will be available
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		printf("GLEW init failed!\n");
		return -1;
	}

	// Terrain  
	terrain = new Terrain(terrainResolution, tileNumber, subdivide);
	terrainShaders = new TerrainShaders(terrainTextureRes, textures, amplitude, frequency, terrainResolution, tileNumber);
	terrainShaders->loadVertexFragmentShaders(terrainVert, terrainFrag);
	terrainShaders->locateUniforms();
	
	//Moving terrain
	terrainTransl = vec3(0.0);

	//Skybox
	skybox = new Skybox(terrainResolution);
	skyboxShaders = new SkyboxShaders(terrainTextureRes, skyboxTextures);
	skyboxShaders->loadVertexFragmentShaders(skyboxVert, skyboxFrag);
	skyboxShaders->locateUniforms();
	
	// Camera 
	camera = new Camera((width/height), cameraPosition);
	glutMotionFunc(mouseMotion);
	glutMouseFunc(mouse);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_DEPTH_TEST);
	
	glutMainLoop();
	return 0;
}

