#include <iostream>
#include <GL/glew.h>


#include <rtr/transformation/display.h>
#include <rtr/transformation/shader.h>
#include <rtr/transformation/mesh.h>
#include <rtr/transformation/texture.h>
#include <rtr/transformation/transform.h>
#include <rtr/transformation/camera.h>

#define WIDTH 1280
#define HEIGHT 720

int main (){

	Display display(WIDTH, HEIGHT, "openGL");

	Vertex vertices[] = {

		Vertex(glm::vec3(-0.5, -0.5, 0), glm::vec2(  0,   0)),
		Vertex(glm::vec3(   0,  0.5, 0), glm::vec2(0.5, 1.0)),
		Vertex(glm::vec3( 0.5, -0.5, 0), glm::vec2(1.0, 0.0)),
	};

	float counter = 0.0f;
	unsigned int indices[] = {0, 1, 2 };

	//Mesh, Shader, Texture, Camera & Transformation ~  
	Transform transform;
	Mesh mesh("../resources/objects/sphere.obj");
	Shader shader("../resources/shaders/basicShader");
	Texture texture("../resources/textures/bricks.jpg");
	Camera camera(glm::vec3(0,0,-2), 70.0f, (float)WIDTH/(float)HEIGHT, 0.001f, 1000.0f);

	//while our display is open 
	while(!display.IsClosed())
	{	
		display.Clear(0.0f, 0.0f, 0.0f, 0.5f);

		float sinCounter = sinf(counter);
		float cosCounter = cosf(counter);

		transform.GetPos().x = sinCounter;
		transform.GetPos().z = cosCounter;
		transform.GetRot().x = counter*2.5;
		transform.GetRot().y = counter*2.5;
		transform.GetRot().z = counter*2.5;
		//transform.SetScale(glm::vec3(cosCounter, cosCounter,cosCounter));
		//transform.GetScale() = cosf(counter);
		counter += 0.0001f;

		//Shader Binding and UpDating, Mesh Drawing & Display UpDating
		shader.Bind();
		texture.Bind(0);
		shader.UpDate(transform, camera);
		mesh.Draw();
		display.UpDate();
	}
	return	 0;
}
