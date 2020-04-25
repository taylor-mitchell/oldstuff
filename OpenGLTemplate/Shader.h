#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <GL/glew.h>
#include "Transform.h"

class Shader
{
private:
	Shader(const Shader& sh) {}
	void operator=(const Shader& sh) {}

	static const unsigned int NUM_SHADERS = 2;

	std::string loadShader(const std::string& filename);
	void checkShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage);
	GLuint createShader(const std::string& text, unsigned int type);

	enum
	{
		TRANSFORM_U,
		NUM_UNIFORMS
	};

	GLuint programHandle;
	GLuint shaders[NUM_SHADERS];
	GLuint uniforms[NUM_UNIFORMS];

public:
	Shader();
	Shader(const std::string& filename);
	virtual ~Shader();

	void bind();
	void update(const Transform& transform);
};


#endif
