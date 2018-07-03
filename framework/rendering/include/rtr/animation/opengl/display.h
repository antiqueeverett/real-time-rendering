#pragma once

#include "../loader.h"

#include <vector>
#include <SDL2/SDL.h>

class animation_Display{

public:
	animation_Display();
	virtual ~animation_Display();

	void init();
	void clean();
	void render();
	void update();
	void handleEvents();

	inline bool isRunning(){return running;}
	inline SDL_Window* getWindow()const{return window;}

	static animation_Display* Instance(){
		static animation_Display inst;
		return &inst;
	}

	int screen_width;
	int screen_height;

private:
	int buffer;
	bool running;

	GLenum status;
	Loader loader;
	SDL_Window* window;
	SDL_GLContext glcontext;
	SDL_DisplayMode display_mode;
};