﻿#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

const int ResolutionX = 640;
const int ResolutionY = 480;

int main(void)
{
	GLFWwindow* window;

	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(ResolutionX, ResolutionY, "...OpenGL Magick...", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK) {
		std::cout << "GLEW Init Error" << std::endl;
	}

	std::cout << "OpenGL v" << glGetString(GL_VERSION) << std::endl;
	{
		float positions[] = {
			100.0f, 100.0f, 0.0f, 0.0f,
			200.f, 100.0f, 1.0f, 0.0f,
			200.0f, 200.0f, 1.0f, 1.0f,
			100.0f, 200.0f, .0f, 1.0f
		};

		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0
		};

		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		unsigned int vao;
		GLCall(glGenVertexArrays(1, &vao));
		GLCall(glBindVertexArray(vao));

		VertexArray va;
		VertexBuffer vb(positions, 4 * 4 * sizeof(float));
		VertexBufferLayout layout;
		layout.Push<float>(2);
		layout.Push<float>(2);
		va.AddBuffer(vb, layout);

 		IndexBuffer ib(indices, 6);
		
		//glm::mat4 proj = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);
		glm::mat4 proj = glm::ortho(0.0f, (float)ResolutionX, 0.0f, (float)ResolutionY, -1.0f, 1.0f); //per pixel projection matrix

		Shader shader("res/shaders/Base.shader");
		shader.Bind();
		shader.SetUniform4f("_Color", 1.0f, 1.0f, 1.0f, 1.0f);
		shader.SetUniformMatrix4f("_MVP", proj);

		Texture texture("res/textures/shotty.png");
		texture.Bind();
		shader.SetUniform1i("_Texture", 0);

		va.Unbind();
		shader.Unbind();
		vb.Unbind();
		ib.Unbind();

		float r = 0.0f;
		float increment = 0.05f;

		Renderer renderer;

		while (!glfwWindowShouldClose(window))
		{
			renderer.Clear();

			shader.Bind();
			shader.SetUniform4f("_Color", r, 0.25f, 1.0f, 1.0f);

			va.Bind();
			ib.Bind();

			renderer.Draw(va, ib, shader);

			if (r > 1.0f) increment *= -1;
			else if (r < 0.05f) increment = abs(increment);

			r += increment;

			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}

	glfwTerminate();
	return 0;
}