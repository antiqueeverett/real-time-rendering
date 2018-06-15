#ifndef DISPLAY_H 
#define DISPLAY_H

#include <SDL2/SDL.h>
#include <string>

class Display
{
public:
	Display(int width, int height, const std::string& title);
	virtual ~Display();

	void UpDate();
	bool IsClosed();
	void Clear(float r, float g, float b, float a);
	
private:
	Display(const Display& other){}
	void operator=(const Display& other){}

	SDL_Window* m_window;
	SDL_GLContext m_glContext;	
	bool isClosed;

protected:
	
};

#endif // DISPLAY_H

/**
 * @parameter SDL_Window* m_window: a private variable for creating a m_window
 * @parameter SDL_GLContext m_glContext: a private variable for declaring the openGL context
 * @function UpDate(): a function used to swap buffers
 */