#include "stHardwareOutput.h"

StDrawLogic::StDrawLogic()  {
	m_openGL_texture_id = 1;
	//создаём окно
	InitVideoOutput();
}

void StDrawLogic::InitVideoOutput() {

	m_screenSize.w = 800;
	m_screenSize.h = 800;

	if (SDL_Init(SDL_INIT_VIDEO) != 0){
		std::cout << "Error: Could not init SDL library! Error: " << SDL_GetError() << std::endl;
		exit(1);
	}
	
	/* Request opengl 3.2 context.
	* SDL doesn't have the ability to choose which profile at this time of writing,
	* but it should default to the core profile */
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	/* Turn on double buffering with a 24bit Z buffer.
	* You may need to change this to 16 or 32 for your system */
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    m_window = SDL_CreateWindow("raytracer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_screenSize.w, m_screenSize.h, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	if (m_window == nullptr){
		std::cout << "Error: Could not create window! Error: " << SDL_GetError() << std::endl;
		exit(1);
	}

	m_context = SDL_GL_CreateContext(m_window);
	int res = SDL_GL_MakeCurrent(m_window, m_context);
	if (res != 0) {
		SDL_DestroyWindow(m_window);
		exit(1);
	}
	
	if (SDL_GL_SetSwapInterval(1) < 0) {
		std::cout << "Warning: Unable to set vsync! SDL error: " << SDL_GetError() << "\n";
	}

	 //Initialize PNG loading 
	int imgFlags = IMG_INIT_PNG; 
	if( !( IMG_Init( imgFlags ) & imgFlags ) )
		std::cout << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError();
	
	glViewport( 0, 0, m_screenSize.w, m_screenSize.h );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( 0, m_screenSize.w, 0, m_screenSize.h, -1, 1 );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	
	//init background texture
	/*
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
	*/
}
/*
GLuint StDrawLogic::LoadTilesetTexture(const std::string * tileset_path) {
	SDL_Surface *loadedSurface = IMG_Load(tileset_path->c_str());
	if (loadedSurface == nullptr){
		std::cout << "Warning: IMG_Load Error: " << SDL_GetError() << std::endl;
		return 0;
	}

	SDL_Surface *gScreenSurface = SDL_GetWindowSurface(m_window);
	SDL_Surface *optimizedSurface = SDL_ConvertSurface( loadedSurface, gScreenSurface->format, 0 ); 
	if( optimizedSurface == NULL ) { 
		std::cout << "Warning: Unable to optimize image " << tileset_path << " SDL Error: " <<  SDL_GetError() << "\n"; 
		optimizedSurface = loadedSurface;
	}

	GLuint resTexture;
	glGenTextures(m_openGL_texture_id++,&resTexture);
	glBindTexture(GL_TEXTURE_2D,resTexture);

	int mode = GL_RGBA;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	gluBuild2DMipmaps(GL_TEXTURE_2D, mode, loadedSurface->w, loadedSurface->h, mode, GL_UNSIGNED_BYTE, loadedSurface->pixels);
	//glTexImage2D(GL_TEXTURE_2D, 0, mode, optimizedSurface->w, optimizedSurface->h, 0, mode, GL_UNSIGNED_BYTE, optimizedSurface->pixels);
	int error = glGetError();

	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, optimizedSurface->w, optimizedSurface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, optimizedSurface->pixels);
    SDL_FreeSurface(loadedSurface);
	SDL_FreeSurface(optimizedSurface);
	return resTexture;
}
*/


StDrawLogic::~StDrawLogic() {
	glDeleteTextures(1, &m_tilesetTexture);
	glDeleteTextures(1, &m_backTexture);
	SDL_GL_DeleteContext(m_context);
	SDL_DestroyWindow(m_window);
}