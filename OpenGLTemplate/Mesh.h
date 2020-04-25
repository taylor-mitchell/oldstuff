#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <GL/glew.h>

class Vertex
{
private:

	glm::vec3 point;
	glm::vec2 texCoord;

public:
	Vertex(const glm::vec3& p, const glm::vec2 &t) 
	{ 
		point = p;
		texCoord = t;
	}

	inline glm::vec3* position()
	{
		return &point;
	}

	inline glm::vec2* textureCoord()
	{
		return &texCoord;
	}

	Vertex& operator= (const Vertex& v)
	{
		point = v.point;
		texCoord = v.texCoord;
		return *this;
	}

};

class Mesh
{
private:
	Mesh(const Mesh& m);
	Mesh& operator=(const Mesh& m);

	enum
	{
		POSITION_VB,
		NUM_BUFFERS,
		TEXCOORD_VB
	};

	GLuint vrtxArrOb;
	GLuint vrtxArrBuf[NUM_BUFFERS];
	GLenum type;
	glm::vec4 color;

	unsigned int drawCount;

public:
	Mesh();
	Mesh(Vertex*, unsigned int, GLenum, glm::vec4);
	void load(Vertex*, unsigned int, GLenum);
	virtual ~Mesh();

	void draw();
};

#endif
