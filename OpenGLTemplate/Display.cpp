#include "Display.h"
#include <iostream>
#include <GL/glew.h>

Display::Display(int width, int height, const std::string title)
{
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	gWindow = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
	gContext = SDL_GL_CreateContext(gWindow);

	glewExperimental = GL_TRUE;
	GLenum status = glewInit();

	if (status != GLEW_OK)
	{
		std::cerr << "**Glew failed to initialize**" << std::endl;
	}

	close = false;

	//glEnable(GL_CULL_FACE);

	//glCullFace(GL_BACK);
}

Display::~Display()
{
	SDL_GL_DeleteContext(gContext);
	SDL_DestroyWindow(gWindow);
	gWindow = 0;
	SDL_Quit();
}

void Display::updateBuffer()
{
	SDL_GL_SwapWindow(gWindow);
}

void Display::clear(float red, float green, float blue, float alpha)
{
	glClearColor(red, green, blue, alpha);
	glClear(GL_COLOR_BUFFER_BIT);
}

Display::Display()
{
	gWindow = 0;
	gContext = 0;
}