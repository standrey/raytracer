#ifndef INIT_SDL2
#define INIT_SDL2

SDL_Window		* window;
SDL_Renderer		* renderer;
SDL_GLContext	context;
GLuint texture;
SDL_Window	*	Display_InitSDL2();
void			InitOpenGL();
void			CleanResources();
void			InitTexture();

#endif