#include <iostream>
#include <exception>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

extern SDL_Window		* window;
extern SDL_Renderer		* renderer;
extern SDL_GLContext	context;
extern GLuint			texture;

SDL_Window * Display_InitSDL2() {
	SDL_Init(SDL_INIT_VIDEO);
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	window = SDL_CreateWindow( 
      "An SDL2 window",         //    const char* title
      20, //SDL_WINDOWPOS_UNDEFINED,  //    int x: initial x position
      20, //SDL_WINDOWPOS_UNDEFINED,  //    int y: initial y position
      1024,                      //    int w: width, in pixels
      768,                      //    int h: height, in pixels
      SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL          //    Uint32 flags: window options, see docs
      );

	// Check that the window was successfully made
	if(window==nullptr){   
		// In the event that the window could not be made...
		std::cout << "Could not create window: " << SDL_GetError() << '\n';
		SDL_Quit(); 
		return nullptr;
	}
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetSwapInterval(1);
    context = SDL_GL_CreateContext(window);
	int res = SDL_GL_MakeCurrent(window, context);
	if (res != 0) {
		SDL_DestroyWindow(window);
		return nullptr;
	}
	return window;
	
	try {
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
	} catch (std::exception &ex) {
		SDL_DestroyWindow(window);
		return nullptr;
	}
	SDL_RendererInfo displayRendererInfo;
	SDL_GetRendererInfo(renderer, &displayRendererInfo);
	
	
	/*TODO: Check that we have OpenGL */
	if ((displayRendererInfo.flags & SDL_RENDERER_ACCELERATED) == 0 ||
			(displayRendererInfo.flags & SDL_RENDERER_TARGETTEXTURE) == 0) {
		/*TODO: Handle this. We have no render surface and not accelerated. */
		SDL_SetRenderDrawColor(renderer, 100, 100, 250, 255);
	}

	return window;
}

void InitOpenGL() {
	glClearColor( 0, 0, 0, 0 );
	glEnable( GL_TEXTURE_2D ); 
	glViewport( 0, 0, 1024, 768 );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( 0, 1024, 0, 768, -1, 1 );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
}

void InitTexture() {
	SDL_Surface *surface = SDL_LoadBMP("..\\res\\background.bmp");
	if (surface == nullptr){
		std::cout << "SDL_LoadBMP Error: " << SDL_GetError() << std::endl;
		return;
	}
	glGenTextures(1,&texture);
	glBindTexture(GL_TEXTURE_2D,texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w,surface->h, 0, GL_RGB,GL_UNSIGNED_BYTE,surface->pixels);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    SDL_FreeSurface(surface);
}

void CleanResources() {
	glDeleteTextures(1,&texture);
	SDL_DestroyWindow(window); 
	SDL_GL_DeleteContext(context);
	SDL_Quit();
}