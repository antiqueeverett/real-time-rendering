#include <rtr/transformation/display.h> 
#include <GL/glew.h>
#include <iostream>

Display::Display(int width, int height,const std::string& title)
{
	//initializing SDL
	SDL_Init(SDL_INIT_EVERYTHING);

	/**
	 * 
	 * Because we inted to draw on an SDL window with openGL we need to
	 *    set SDL window attributes which are not a part of the "SDL_CreateWindow" method
	 */
	
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32); // Data in bits for SDL to allocate for each pixel
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16); 
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); // Allocate space for two windows (Double Buffering)

	/**
	 * 
	 * Member variable m_window of type "SDL_Window" is used with function "SDL_CreateWindow" to create new window.
	 * SDL is a C-API. As such the *title string arg needs to be converted to c c_str().
	 * "SDL_CreateWindow" then take parameters as part of its argument list that may be specified.
	 * The last parameter "SDL_WINDOW_OPENGL" lets SDL know we intend to use openGL to draw onto the window.
	 */
	
	m_window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
	
	//create openGL context to Draw on the SDL window
	m_glContext = SDL_GL_CreateContext(m_window);

	//Glew finds all openGL functions within the operating system 
	GLenum status = glewInit();

	if (status != GLEW_OK) // if the status isnt OK ... we got a problem 
	{
		std::cerr << "Glew failed to initialize" << std::endl;
	}

	isClosed = false;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

Display::~Display()
{
	//deconstruct in the reverse order of construction to avoid bug responces
	SDL_GL_DeleteContext(m_glContext);
	SDL_DestroyWindow(m_window);
	SDL_Quit();
}

void Display::Clear(float r, float g, float b, float a)
{
		glClearColor(r, g, b, a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
bool Display::IsClosed()
{
	return isClosed;
}
void Display::UpDate()
{
	//swapping the window (double buffering concept at play here)
	SDL_GL_SwapWindow(m_window);

	//the operating systems passes events ("SDL_Event") to validate link communication with SLD, as such, these events require handling
	SDL_Event e;

	while(SDL_PollEvent(&e))
	{	
		// checking if we have closed the window - via the OS
		if (e.type == SDL_QUIT)
		{
			isClosed = true;
		}
	}
}