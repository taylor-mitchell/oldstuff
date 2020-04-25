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
#include "SDL2/SDL.h"


const unsigned int SCREEN_H = 800;
const unsigned int SCREEN_W = 1200;
const unsigned int BLOCK_W = 1200;
const unsigned int BLOCK_H = 800;
const unsigned int NUM_START_CELLS = 100000;
const unsigned int CELLS_START_BLOCK = 500;
const unsigned int HEAT_SQUARE_SIZE = 50;
float SPECIFIC_HEAT = 0.49f;


//gPixel holds the pixel data that gets sent to openGL
typedef struct
{
	GLubyte blue, green, red;
}gPixel;

//vPixel is a virtual pixel that holds all of the data that is used to calculate gPixel values
typedef struct
{
	float u;
	float u_dx, u_dx2, u_dy, u_dy2;
	bool alive;
}vPixel;

int main(int argc, char** argv)
{
	//initiates some flags for use later on
	bool close = false;
	bool buttonDown = false;
	bool transformFlag = false;
	bool heatFlag = false;
	bool plusFlag = false;
	bool minusFlag = false;
	bool lifeFlag = false;

	double average = 0;
	double total = 0;
	double cnt = 0;
	
	//this vector holds vertices that can be pushed into a mesh to manually add triangles to the screen -- doesn't work when drawPixels is active
	std::vector<Vertex> vertices;

	std::chrono::high_resolution_clock::time_point t1, t2;
	
	
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
		(buffer1 + i)->u_dx = 0.0f;
		(buffer1 + i)->u_dx2 = 0.0f;
		(buffer1 + i)->u_dy = 0.0f;
		(buffer1 + i)->u_dy2 = 0.0f;
		(buffer1 + i)->alive = false;

		(buffer2 + i)->u = 0;
		(buffer2 + i)->u_dx = 0.0f;
		(buffer2 + i)->u_dx2 = 0.0f;
		(buffer2 + i)->u_dy = 0.0f;
		(buffer2 + i)->u_dy2 = 0.0f;
		(buffer2 + i)->alive = false;		
	}

	frontBuf = buffer1;
	backBuf = buffer2;

	float counter = 0.0f;
	int x, y;

	//sets the position where the pixel block will be drawn by glDrawPixels
	glRasterPos2d(-1, -1);
	
	while (!close)
	{
		cnt++;
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
		
		t1 = std::chrono::high_resolution_clock::now();
		//this for is for both heat and the game of life. 
#pragma omp parallel num_threads(4)
		{		
#pragma omp for
			for (int i = BLOCK_W + 1; i < (BLOCK_H - 2) * BLOCK_W; i++)
			{
				int count = 0;
				//this if makes sure that it doesn't calculate anything for edge pixels
				//if (i % (BLOCK_W) != 0  (i + 1) % BLOCK_W != 0)
				//{
					//counts alive neighbors
					count = (backBuf + i + 1)->alive + (backBuf + i - 1)->alive + (backBuf + i - BLOCK_W)->alive + (backBuf + i - BLOCK_W - 1)->alive
						+ (backBuf + i - BLOCK_W + 1)->alive + (backBuf + i + BLOCK_W)->alive + (backBuf + i + BLOCK_W - 1)->alive + (backBuf + i + BLOCK_W + 1)->alive;
				
					//keeps the old alive value if it has 2 alive neighbors
					if (count == 2)
					{
						(frontBuf + i)->alive = (backBuf + i)->alive;
					}
					//sets the cell to be alive if it has three alive neighbors
					else if (count == 3)
					{
						(frontBuf + i)->alive = true;
						(frontBuf + i)->u_dx = ((backBuf + i - 1)->u + (backBuf + i)->u + (backBuf + i + 1)->u) / 3.0f;
						(frontBuf + i)->u_dx2 = ((backBuf + i - 1)->u_dx + (backBuf + i)->u_dx + (backBuf + i + 1)->u_dx) / 3.0f;

						(frontBuf + i)->u_dy = ((backBuf + i - BLOCK_W)->u + (backBuf + i)->u + (backBuf + i + BLOCK_W)->u) / 3.0f;
						(frontBuf + i)->u_dy2 = ((backBuf + i - BLOCK_W)->u_dy + (backBuf + i)->u_dy + (backBuf + i + BLOCK_W)->u_dy) / 3.0f;

						(frontBuf + i)->u = 255;

						(pixels + i)->blue = 255;
						(pixels + i)->green = 255;
						(pixels + i)->red = 255;
					}
					else 
					{
						(frontBuf + i)->alive = false;

						(frontBuf + i)->u_dx = ((backBuf + i - 1)->u + (backBuf + i)->u + (backBuf + i + 1)->u) / 3.0f;
						(frontBuf + i)->u_dx2 = ((backBuf + i - 1)->u_dx + (backBuf + i)->u_dx + (backBuf + i + 1)->u_dx) / 3.0f;

						//calculates the second derivative with respect to y
						(frontBuf + i)->u_dy = ((backBuf + i - BLOCK_W)->u + (backBuf + i)->u + (backBuf + i + BLOCK_W)->u) / 3.0f;
						(frontBuf + i)->u_dy2 = ((backBuf + i - BLOCK_W)->u_dy + (backBuf + i)->u_dy + (backBuf + i + BLOCK_W)->u_dy) / 3.0f;

						//calculates the heat of the pixel by using the second derivatives of x and y and a specific_heat constant
						//also it ensures that the value cannot be over 255
						(frontBuf + i)->u = (SPECIFIC_HEAT * ((backBuf + i)->u_dx2 + (backBuf + i)->u_dy2)); //> 255 ? 255 : (SPECIFIC_HEAT * ((backBuf + i)->u_dx2 + (backBuf + i)->u_dy2));

						//assigns color values to the pixels
						(pixels + i)->blue = (frontBuf + i)->u;
						(pixels + i)->green = (frontBuf + i)->u;
						(pixels + i)->red = (frontBuf + i)->u;
					}				
				//}
			}
		}
		
		t2 = std::chrono::high_resolution_clock::now();

		std::chrono::duration<double> span = t2 - t1;

		total += span.count();

		std::cout << total / cnt << std::endl;

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
					transform.getRot().y = (x - (SCREEN_W / 2)) * .01;
					transform.getRot().x = (y - (SCREEN_H / 2)) * .01;
					std::cout << transform.getPos().y << " " << transform.getPos().x << " " << transform.getPos().z << std::endl;
				}
			}
			else if (e.type == SDL_MOUSEBUTTONDOWN)
			{
				//this is for adding triangles to the screen.  again, it won't show unless glDrawPixels is disabled
				SDL_GetMouseState(&x, &y);
				vertices.emplace_back(glm::vec3((((float)x - (float)SCREEN_W / 2.0f) / ((float)SCREEN_W / 2.0f)), (((float)SCREEN_H / 2.0f  - (float)y) / ((float)SCREEN_H / 2.0f)), 0), glm::vec2(0.0, 0.0));

				mesh.load(&vertices[0], vertices.size(), GL_TRIANGLES);

				std::cout << (((float)x - SCREEN_W / 2) / (float)SCREEN_W) << ", " << (((float)SCREEN_H / 2 - y) / (float)SCREEN_H) << std::endl;
				std::cout << x << ", " << y << std::endl;
			}
			else if (e.type == SDL_KEYDOWN)
			{
				if (e.key.keysym.sym == SDLK_r)
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
								(backBuf + ((BLOCK_H / 2 + y) * BLOCK_W) + BLOCK_W / 2 + x)->u_dx = 255;
								(backBuf + ((BLOCK_H / 2 + y) * BLOCK_W) + BLOCK_W / 2 + x)->u_dx2 = 255;
								(backBuf + ((BLOCK_H / 2 + y) * BLOCK_W) + BLOCK_W / 2 + x)->u_dy = 255;
								(backBuf + ((BLOCK_H / 2 + y) * BLOCK_W) + BLOCK_W / 2 + x)->u_dy2 = 255;
								(frontBuf + ((BLOCK_H / 2 + y) * BLOCK_W) + BLOCK_W / 2 + x)->u = 255;
								(frontBuf + ((BLOCK_H / 2 + y) * BLOCK_W) + BLOCK_W / 2 + x)->u_dx = 255;
								(frontBuf + ((BLOCK_H / 2 + y) * BLOCK_W) + BLOCK_W / 2 + x)->u_dx2 = 255;
								(frontBuf + ((BLOCK_H / 2 + y) * BLOCK_W) + BLOCK_W / 2 + x)->u_dy = 255;
								(frontBuf + ((BLOCK_H / 2 + y) * BLOCK_W) + BLOCK_W / 2 + x)->u_dy2 = 255;
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
				else if (e.key.keysym.sym == SDLK_l)
				{
					//randomly brings cells to life within a square with sides of CELLS_START_BLOCK length
					if (!lifeFlag)
					{
						int tempX, tempY;
						
						for (int i = 0; i < NUM_START_CELLS; i++)
						{
							
							int tempX = (BLOCK_W / 2 - CELLS_START_BLOCK / 2 + rand() % CELLS_START_BLOCK);
							int tempY = ((BLOCK_H / 2 - CELLS_START_BLOCK / 2 + rand() % CELLS_START_BLOCK) * BLOCK_W);

							(backBuf + tempY + tempX)->alive = true;
					
							(backBuf + tempY + tempX)->u = 255;
							(backBuf + tempY + tempX)->u_dx = ((backBuf + tempY + tempX - 1)->u + (backBuf + tempY + tempX)->u + (backBuf + tempY + tempX + 1)->u) / 3.0f;
							(backBuf + tempY + tempX)->u_dx2 = ((backBuf + tempY + tempX - 1)->u_dx + (backBuf + tempY + tempX)->u_dx + (backBuf + tempY + tempX + 1)->u_dx) / 3.0f;

							(backBuf + tempY + tempX)->u_dy = ((backBuf + tempY + tempX - BLOCK_W)->u + (backBuf + tempY + tempX)->u + (backBuf + tempY + tempX + BLOCK_W)->u) / 3.0f;
							(backBuf + tempY + tempX)->u_dy2 = ((backBuf + tempY + tempX - BLOCK_W)->u_dy + (backBuf + tempY + tempX)->u_dy + (backBuf + tempY + tempX + BLOCK_W)->u_dy) / 3.0f;
						}

					}

					lifeFlag = true;
				}
				else if (e.key.keysym.sym == SDLK_t)
				{
					std::cout << total / cnt << std::endl;
				}
			}
			else if (e.type == SDL_KEYUP)
			{
				//sets all respective flags to false when the key gets unpressed
				if (e.key.keysym.sym == SDLK_r)
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
