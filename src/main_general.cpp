#include <iostream>
#include <cstdio>
#include <vector>
#include <list>
#include <cmath>

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

SDL_Texture *background = nullptr;

int sc = 40;
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
	Display_InitSDL2();
	InitOpenGL();
	InitTexture();

	// Prepare for simulation. Typically we use a time step of 1/60 of a
	// second (60Hz) and 10 iterations. This provides a high quality simulation
	// in most game scenarios.
	float32 timeStep = 1.0f / 60.0f;
	int32 velocityIterations = 6;
	int32 positionIterations = 2;

	//openGL coding
	while (do_loop) {
		glClearColor ( 0.0, 1.0, 0.0, 1.0 );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		DrawBodies();
		SDL_GL_SwapWindow(window);
		SDL_Delay(20);
		EventProcess();
	}

	CleanResources();

	return 0;
}