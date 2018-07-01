#include <rtr/animation/opengl/display.h>
#include <rtr/animation/controls/input.h>

#include <iostream>
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

animation_Display::animation_Display()
{
	running 	  = true;
	screen_width  = 1024;
	screen_height = 768;
}

animation_Display::~animation_Display()
{

}

void animation_Display::init()
{	
	if (SDL_Init(SDL_INIT_EVERYTHING) >= 0)
	{	
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1); 
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4); 

		window = SDL_CreateWindow("Skeleton Animation rtr_animation", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			screen_width, screen_height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | 0);

		if(window != 0){
			glcontext = SDL_GL_CreateContext(window);
			std::cout << "SDL initialized successfuly ! " << std::endl;
		}
	}
	
	status = glewInit();
	if(status != GLEW_OK){
		std::cerr << "Glew failed to initialize" << std::endl;
	}

	ilInit(); 
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);
	ilEnable(IL_ORIGIN_SET); 
	ilOriginFunc(IL_ORIGIN_UPPER_LEFT); 

	glViewport(0, 0, screen_width, screen_height); 
	glEnable(GL_DEPTH_TEST); 
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &buffer);
	loader.init();
}

void animation_Display::handleEvents()
{
	InputHandler::Instance()->updateEvent();
}

void animation_Display::update()
{
	loader.update();
}

void animation_Display::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	loader.render();
	glFlush(); 
	SDL_GL_SwapWindow(window);	 
}

void animation_Display::clean()
{
	SDL_GL_DeleteContext(glcontext);
	SDL_DestroyWindow(window);
	SDL_Quit();
	
	running = false;
}