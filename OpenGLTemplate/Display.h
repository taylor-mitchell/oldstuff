#ifndef DISPLAY_H
#define DISPLAY_H

#include <string>
#include "SDL2/SDL.h"

class Display
{
private:
	Display(const Display& d) {}
	Display& operator=(const Display& d) {}

	SDL_Window* gWindow;
	SDL_GLContext gContext;

	bool close;
public:
	Display();
	Display(int width, int height, const std::string title);
	virtual ~Display();

	bool isClosed() { return close; }

	void updateBuffer();

	void clear(float, float, float, float);
};
#endif