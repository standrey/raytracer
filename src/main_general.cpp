#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>
#include <list>
#include <cmath>

//extern "C"
//{
	#include <SDL2/SDL.h>
	#include <SDL2/SDL_opengl.h>
//}
#include "stHardwareOutput.h"
#include "stScene.h"
#include "init.h"


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
	unsigned int window_width = 800, window_height = 800;

	auto mainscene = std::make_unique<stScene>(window_width, window_height);
	StDrawLogic drawInterface;
	//openGL coding
	while (do_loop) {
		//glClear(GL_COLOR_BUFFER_BIT);
		auto pixels = mainscene->calcPixels(window_width, window_height);
		glDrawPixels(window_width, window_height, GL_RGB, GL_FLOAT, pixels);
		SDL_GL_SwapWindow(drawInterface.m_window);
		EventProcess();
	}

	return 0;
}