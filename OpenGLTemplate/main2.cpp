#include <iostream>
#include "Display.h"
#include <GL/glew.h>
#include "Mesh.h"
#include "Transform.h"
#include "Shader.h"
#include "Texture.h"
#include "Bmp.h"
#include <chrono>
#include <vector>
#include <cstdlib>

const unsigned int SCREEN_H = 800;
const unsigned int SCREEN_W = 1200;

int main(int argc, char** argv)
{
	//initiates some flags for use later on
	bool close = false;
	bool buttonDown = false;

	glm::vec3 p1 = { 0.0f, 0.4f, 0.0f };
	glm::vec3 p2 = { 0.1f, 0.6f, 0.0f };
	glm::vec2 t1 = { 0.5f, 0.5f };

	Vertex v1(p1, t1);
	Vertex v2(p2, t1);

	Vertex* vertices = (Vertex*)malloc(sizeof(Vertex) * 2);
	*vertices = v1;
	*(vertices + 1) = v2;
	
	//initiates some objects that are necessary for openGL
	Display display(SCREEN_W, SCREEN_H, "butts");
	Mesh mesh;
	Shader shader("./res/basicShader");
	Texture texture("./res/red.jpg");
	Transform transform;

	SDL_Event e;

	mesh.load(vertices, 2, GL_POINTS, glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

	
	int x, y;


	
	while (!close)
	{
		display.clear(0.0f, 0.0f, 1.0f, 1.0f);
		shader.bind();
		texture.bind(0);
		shader.update(transform);
		mesh.draw();	
		display.updateBuffer();

		

		//takes care of events
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
			{
				close = true;
			}
			else if (e.type == SDL_MOUSEMOTION)
			{
				
			}
			else if (e.type == SDL_MOUSEBUTTONDOWN)
			{
				SDL_GetMouseState(&x, &y);

			}
			else if (e.type == SDL_KEYDOWN)
			{
				if (e.key.keysym.sym == SDLK_r)
				{
		
				}
				else if (e.key.keysym.sym == SDLK_h)
				{
								
				}
				else if (e.key.keysym.sym == SDLK_EQUALS)
				{
					
				}
				else if (e.key.keysym.sym == SDLK_MINUS)
				{
					
				}
				else if (e.key.keysym.sym == SDLK_l)
				{

				}					
			}
			else if (e.type == SDL_KEYUP)
			{
				if (e.key.keysym.sym == SDLK_r)
				{
			
				}
				else if (e.key.keysym.sym == SDLK_h)
				{
					
				}
				else if (e.key.keysym.sym == SDLK_EQUALS)
				{
					
				}
				else if (e.key.keysym.sym == SDLK_MINUS)
				{
					
				}
				else if (e.key.keysym.sym == SDLK_l)
				{
					
				}
			}
		}
	}	
	
	return 0;	
}
