#include <iostream>
#include <cstdio>
#include <vector>
#include <list>
#include <cmath>
#include "stScene.h"

extern "C"
{
	#include <SDL2/SDL.h>
	#include <SDL2/SDL_opengl.h>
}
#include "init.h"

extern SDL_Window		* window;
extern SDL_Renderer		* renderer;
extern SDL_GLContext	context;
extern GLuint			texture;


bool do_loop = true;

void EventProcess() {
	int event_pending = 0;
	SDL_Event event;
	event_pending = SDL_PollEvent(&event);
	if (event_pending == 0) return;
	
	switch(event.type)
	{
		case SDL_QUIT:
			do_loop=false;
			break;
		case SDL_KEYDOWN:
			switch(event.key.keysym.sym) {
				case SDLK_ESCAPE:
					do_loop=false;
					break;
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
			break;
	}
}

int main(int argc, char** argv)
{
	unsigned int window_width = 1024, window_height = 1024;

	auto mainscene = std::make_unique<stScene>(window_width, window_height);
	//openGL coding
	while (do_loop) {
		glClearColor ( 0.0, 1.0, 0.0, 1.0 );
		glClear( GL_COLOR_BUFFER_BIT);
		//DRAW
		auto pixels = mainscene->calcPixels(window_width, window_height);
		glDrawPixels(window_width,window_height,GL_RGB,GL_FLOAT,pixels);
		SDL_GL_SwapWindow(window);
		EventProcess();
	}

	return 0;
}