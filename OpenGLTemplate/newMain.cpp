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
#include "omp.h"

const unsigned int SCREEN_H = 800;
const unsigned int SCREEN_W = 1200;
const unsigned int BLOCK_W = 1200;
const unsigned int BLOCK_H = 800;
const unsigned int NUM_START_CELLS = 100000;
const unsigned int CELLS_START_BLOCK = 500;
const unsigned int HEAT_SQUARE_SIZE = 50;
const unsigned int THERMAL_CONDUCTIVITY = 0.5f;
float SPECIFIC_HEAT = 2000.0f;
const float BONE_DENSITY = 3880.0f;

const float TIME_STEP = .033333;
const float BONE_W = 4.9f;
const float PIXELS_PER_METER = 11032.0f;


//gPixel holds the pixel data that gets sent to openGL
typedef struct
{
	GLubyte blue, green, red;
}gPixel;

//vPixel is a virtual pixel that holds all of the data that is used to calculate gPixel values
typedef struct
{
	float u;
}vPixel;

int main1(int argc, char** argv)
{
	//initiates some flags for use later on
	bool close = false;
	bool buttonDown = false;
	bool transformFlag = false;
	bool heatFlag = false;
	bool plusFlag = false;
	bool minusFlag = false;
	bool lifeFlag = false;

	//this vector holds vertices that can be pushed into a mesh to manually add triangles to the screen -- doesn't work when drawPixels is active
	std::vector<Vertex> vertices;

	//initiates some objects that are necessary for openGL
	Display display(SCREEN_W, SCREEN_H, "Life");
	Mesh mesh;
	Shader shader("./res/basicShader");
	Texture texture("./res/gradient.jpg");
	Transform transform;

	//initiates variables for use with the game of life and heat transfer
	//I'm using a double buffer right now to make sure to hold on to old data.  I might have to switch to a triple buffer because I am calculating second derivatives
	vPixel* buffer1 = (vPixel*)malloc(sizeof(vPixel) * BLOCK_W * BLOCK_H);
	vPixel* buffer2 = (vPixel*)malloc(sizeof(vPixel) * BLOCK_W * BLOCK_H);
	vPixel* frontBuf;
	vPixel* backBuf;
	gPixel* pixels = (gPixel*)malloc(sizeof(gPixel) * BLOCK_W * BLOCK_H);

	SDL_Event e;

	for (int i = 0; i < BLOCK_W * BLOCK_H; i++)
	{
		(pixels + i)->blue = 0;
		(pixels + i)->green = 0;
		(pixels + i)->red = 0;

		(buffer1 + i)->u = 0;

		(buffer2 + i)->u = 0;

	}

	frontBuf = buffer1;
	backBuf = buffer2;

	float counter = 0.0f;
	int x, y;

	//sets the position where the pixel block will be drawn by glDrawPixels
	glRasterPos2d(-1, -1);

	while (!close)
	{
		//some of these I don't need because glDrawPixels just draws over everything that they do
		display.clear(0.0f, 0.0f, 1.0f, 1.0f);
		shader.bind();
		texture.bind(0);
		shader.update(transform);
		//mesh.draw();

		glDrawPixels(BLOCK_W, BLOCK_H, GL_BGR, GL_UNSIGNED_BYTE, pixels);

		display.updateBuffer();


		//plusFlag and minusFlag adjust the specific_heat variable by pressing the plus and minus keys respectively
		if (plusFlag)
		{
			SPECIFIC_HEAT += 0.0001f;
		}
		if (minusFlag)
		{
			SPECIFIC_HEAT -= 0.0001f;
		}

#pragma omp parallel num_threads(4)
		{
#pragma omp for
			for (int i = BLOCK_W + 1; i < (BLOCK_H - 2) * BLOCK_W; i++)
			{
				//(frontBuf + i)->u = (((midBuf + i - 1)->u - 2.0f * (midBuf + i)->u + (midBuf + i + 1)->u) + ((backBuf + i - 1)->u - 2.0f * (backBuf + i)->u + (backBuf + i + 1)->u)) / (2.0f * pow(X_STEP, 2))
								//	+ (((midBuf + i - BLOCK_W)->u - 2.0f * (midBuf + i)->u + (midBuf + i + BLOCK_W)->u) + ((backBuf + i - BLOCK_W)->u - 2.0f * (backBuf + i)->u + (backBuf + i + BLOCK_W)->u)) / (2.0f * pow(X_STEP, 2));
				int count = 0;
				//this if makes sure that it doesn't calculate anything for edge pixels
				if (i % (BLOCK_W) != 0  (i + 1) % BLOCK_W != 0)
				{
					//(frontBuf + i)->u += /*TIME_STEP * */(((midBuf + i - 1)->u - (2.0f * (midBuf + i)->u) + (midBuf + i + 1)->u + (backBuf + i - 1)->u - (2.0f * (backBuf + i)->u) + (backBuf + i + 1)->u) / (2.0f * pow(X_STEP, 2))
							//+ ((midBuf + i - BLOCK_W)->u - (2.0f * (midBuf + i)->u) + (midBuf + i + BLOCK_W)->u + (backBuf + i - BLOCK_W)->u - (2.0f * (backBuf + i)->u) + (backBuf + i + BLOCK_W)->u) / (2.0f * pow(X_STEP, 2)));
					(frontBuf + i)->u +=  THERMAL_CONDUCTIVITY * ((((backBuf + i - 1)->u - 2.0f * (backBuf + i)->u + (backBuf + i + 1)->u) / pow(PIXELS_PER_METER, -2)) + (((backBuf + i - BLOCK_W)->u - 2.0f * (backBuf + i)->u + (backBuf + i + BLOCK_W)->u) / pow(PIXELS_PER_METER, -2))) / BONE_DENSITY / SPECIFIC_HEAT;
				}
				(pixels + i)->red = (frontBuf + i)->u;
				(pixels + i)->blue = (frontBuf + i)->u;
				(pixels + i)->green = (frontBuf + i)->u;
			}
		}
		
		

		//swaps the buffers
		std::swap(frontBuf, backBuf);

		//takes care of events
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
			{
				close = true;
			}
			else if (e.type == SDL_MOUSEMOTION)
			{
				//this one is for moving the triangles around.  they wont show unless glDrawPixels is disabled
				if (transformFlag)
				{
					SDL_GetMouseState(&x, &y);
					std::cout << x << " " << y << " " << (frontBuf + ((BLOCK_H - y) * BLOCK_W) + x)->u << std::endl;
				}
			}
			else if (e.type == SDL_MOUSEBUTTONDOWN)
			{
				//this is for adding triangles to the screen.  again, it won't show unless glDrawPixels is disabled
				SDL_GetMouseState(&x, &y);
				vertices.emplace_back(glm::vec3((((float)x - (float)SCREEN_W / 2.0f) / ((float)SCREEN_W / 2.0f)), (((float)SCREEN_H / 2.0f - (float)y) / ((float)SCREEN_H / 2.0f)), 0), glm::vec2(0.0, 0.0));

				mesh.load(&vertices[0], vertices.size(), GL_TRIANGLES);

				std::cout << (((float)x - SCREEN_W / 2) / (float)SCREEN_W) << ", " << (((float)SCREEN_H / 2 - y) / (float)SCREEN_H) << std::endl;
				std::cout << x << ", " << y << std::endl;
			}
			else if (e.type == SDL_KEYDOWN)
			{
				if (e.key.keysym.sym == SDLK_t)
				{
					//triangles will rotate with the mouse if the 'r' key is held down
					transformFlag = true;
				}
				else if (e.key.keysym.sym == SDLK_h)
				{
					//a block of heated pixels will appear on screen if 'h' is held down
					if (!heatFlag)
					{
						for (int y = 0; y < HEAT_SQUARE_SIZE; y++)
						{
							for (int x = 0; x < HEAT_SQUARE_SIZE; x++)
							{
								(backBuf + ((BLOCK_H / 2 + y) * BLOCK_W) + BLOCK_W / 2 + x)->u = 255;
								
								(frontBuf + ((BLOCK_H / 2 + y) * BLOCK_W) + BLOCK_W / 2 + x)->u = 255;
								
							}
						}
					}

					heatFlag = true;
				}
				else if (e.key.keysym.sym == SDLK_EQUALS)
				{
					//the specific_heat constant will grow larger if '+' is held down
					plusFlag = true;
				}
				else if (e.key.keysym.sym == SDLK_MINUS)
				{
					//the specific_heat constant will shrink if '-' is held down
					minusFlag = true;
				}			
			}
			else if (e.type == SDL_KEYUP)
			{
				//sets all respective flags to false when the key gets unpressed
				if (e.key.keysym.sym == SDLK_t)
				{
					transformFlag = false;
				}
				else if (e.key.keysym.sym == SDLK_h)
				{
					heatFlag = false;
				}
				else if (e.key.keysym.sym == SDLK_EQUALS)
				{
					plusFlag = false;
				}
				else if (e.key.keysym.sym == SDLK_MINUS)
				{
					minusFlag = false;
				}
				else if (e.key.keysym.sym == SDLK_l)
				{
					lifeFlag = false;
				}
			}
		}
	}

	//frees all of the allocated data
	free(pixels);
	free(buffer1);
	free(buffer2);


	return 0;

}