#ifndef RENDER_ENGINE_H
#define RENDER_ENGINE_H

#include "gl_core_4_4.h"
#include "GLFW\glfw3.h"
#include "glm\glm.hpp"
#include "glm\gtc\type_ptr.hpp"
#include <math.h>
#include <iostream>



#pragma once
class RenderEngine
{
public:

	struct m_Verts
	{
		glm::vec4 position;
		glm::vec4 colour;
	};

	GLuint m_VBO, m_VAO, m_IBO;

	GLuint vertexShader;
	GLuint fragmentShader;

	GLuint m_ProgramID;

	GLuint indexCount;

	GLint success;
	GLchar infoLog[512];

	void Init(const GLchar* vsSource, const GLchar* fsSource);

	void Render(glm::mat4 projectionViewMatrix, glm::mat4 world);

	void GenGrid(unsigned int _rows, unsigned int _cols);

	void Close();

	RenderEngine();
	~RenderEngine();

private:
	unsigned int projViewUniform;
	unsigned int worldUniform;
};

#endif // !RENDER_ENGINE_H
