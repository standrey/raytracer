#include "minimal.h"

class StDrawLogic {
private:
	int				m_openGL_texture_id;
	SDL_Rect		m_screenSize;
	SDL_GLContext	m_context;
	GLuint			m_backTexture;
	GLuint			m_tilesetTexture;
	SDL_Rect		m_worldSizePixels;
	void			InitVideoOutput();
public:
					StDrawLogic();
					~StDrawLogic();
	SDL_Window *	m_window;
};