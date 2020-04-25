#include "ObjectLoader.h"
#include <fstream>
#include <iostream>

/*Object::Object()
{

}

Object::Object(std::string textureFile, std::string vertexFile)
{
	std::string num;
	float vertexCoords[3];
	texture.load(textureFile);

	int numVertices = 0;

	std::ifstream inFile;
	int i = 0;
	while (!inFile.eof())
	{
		char c = inFile.get();
		if (c != ' ' || c != '\n')
		{
			num.push_back(c);
			num.clear();
		}
		else
		{
			vertexCoords[i] = atof(num.c_str());
			i++;

			if (i > 2)
			{
				vertices.emplace_back(new glm::vec3(vertexCoords[0], vertexCoords[1], vertexCoords[2]), new glm::vec2(0.0, 0.0));
				i = 0;
				numVertices++;
			}
		}
	}

	mesh.load(&vertices[0], numVertices);




}*/