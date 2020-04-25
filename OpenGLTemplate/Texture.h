#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <GL/glew.h>

class Texture
{
private:
	Texture(const Texture& texture) {}
	void operator= (const Texture& texture) {}

	GLuint texture;
public:
	Texture();
	Texture(const std::string& filename);

	bool load(const std::string& filename);

	void bind(unsigned int unit);

	virtual ~Texture();
};

/*
class FrameBuffer
{
private:
	FrameBuffer(const FrameBuffer& fBuffer) {}
	void operator= (const FrameBuffer& fBuffer) {}

	GLuint frameBuffer;
	GLuint texture;
	int width, height;
public:
	FrameBuffer();

	bool init(int w, int h);

	void render();

	void bind();

	virtual ~FrameBuffer();
};
*/


#endif
