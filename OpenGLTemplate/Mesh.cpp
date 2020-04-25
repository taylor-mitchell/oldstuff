#include "Mesh.h"
#include <iostream>
#include <vector>


Mesh::Mesh()
{

}

void Mesh::load(Vertex* vertices, unsigned int numVertices, GLenum t)
{
	
	type = t;

	switch (t)
	{
	case GL_POINTS:
		drawCount = numVertices;
		break;
	case GL_TRIANGLES:
		drawCount = numVertices - (numVertices % 3);
		break;
	}
	
	glGenVertexArrays(1, &vrtxArrOb);
	glBindVertexArray(vrtxArrOb);

	std::vector<glm::vec3> positions;
	std::vector<glm::vec2> texCoords;

	positions.reserve(numVertices + 50);
	texCoords.reserve(numVertices + 50);

	for (unsigned int i = 0; i < numVertices; i++)
	{
		positions.push_back(*vertices[i].position());
		texCoords.push_back(*vertices[i].textureCoord());
	}

	glGenBuffers(NUM_BUFFERS, vrtxArrBuf);
	glBindBuffer(GL_ARRAY_BUFFER, vrtxArrBuf[POSITION_VB]);

	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(positions[0]), &positions[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vrtxArrBuf[TEXCOORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(texCoords[0]), &positions[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray(0);

}

Mesh::Mesh(Vertex* vertices, unsigned int numVertices, GLenum t, glm::vec4 color)
{
	type = t;

	switch (t)
	{
	case GL_POINTS:
		drawCount = numVertices;
		break;
	case GL_TRIANGLES:
		drawCount = numVertices - (numVertices % 3);
		break;
	}

	glGenVertexArrays(1, &vrtxArrOb);
	glBindVertexArray(vrtxArrOb);

	std::vector<glm::vec3> positions;
	std::vector<glm::vec2> texCoords;

	positions.reserve(numVertices + 50);
	texCoords.reserve(numVertices + 50);

	for (unsigned int i = 0; i < numVertices; i++)
	{
		positions.push_back(*vertices[i].position());
		texCoords.push_back(*vertices[i].textureCoord());
	}

	glGenBuffers(NUM_BUFFERS, vrtxArrBuf);
	glBindBuffer(GL_ARRAY_BUFFER, vrtxArrBuf[POSITION_VB]);

	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(positions[0]), &positions[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vrtxArrBuf[TEXCOORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(texCoords[0]), &positions[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray(0);
}

Mesh::Mesh(const Mesh& m)
{

}

Mesh& Mesh::operator=(const Mesh& m)
{
	return *this;
}


Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &vrtxArrOb);
}

void Mesh::draw()
{
	glBindVertexArray(vrtxArrOb);

	glDrawArrays(type, 0, drawCount);

	glBindVertexArray(0);
}

