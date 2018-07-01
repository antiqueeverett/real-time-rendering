#include <rtr/animation/opengl/display.h>
#include <SDL2/SDL.h>
#include <iostream>
#include <string>

#define FPS 60 					// frames per sec
#define sec 1000				// 1000 millisec = 1 second

const int LOOP_TIME = sec/FPS; 	

int main(int argc, char* argv[])
{
	animation_Display display;			
	display.init();				
	
	Uint32 frameStart, frameTime;
	while (display.isRunning())
	{
		frameStart = SDL_GetTicks();

		display.update();
		display.render();

		frameTime = SDL_GetTicks() - frameStart; 
	
		if (frameTime < LOOP_TIME){
			SDL_Delay((int)(LOOP_TIME - frameTime));
		}
		display.handleEvents();
	}
	display.clean();
	return 0;
}