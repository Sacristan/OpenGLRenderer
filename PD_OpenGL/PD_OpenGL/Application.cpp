#include <GL/glew.h>
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
#include <ctime>

const int ResolutionX = 640;
const int ResolutionY = 480;

//static const float cube_vertex_buffer_data[] = {
//
//
//
//};


GLfloat cube_vertices[] = {
	// front
	-1.0, -1.0,  1.0,
	 1.0, -1.0,  1.0,
	 1.0,  1.0,  1.0,
	-1.0,  1.0,  1.0,
	// back
	-1.0, -1.0, -1.0,
	 1.0, -1.0, -1.0,
	 1.0,  1.0, -1.0,
	-1.0,  1.0, -1.0
};

GLfloat cube_colors[] = {
	// front colors
	1.0, 0.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 0.0, 1.0,
	1.0, 1.0, 1.0,
	// back colors
	1.0, 0.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 0.0, 1.0,
	1.0, 1.0, 1.0
};

GLuint cube_elements[] = {
	// front
	0, 1, 2,
	2, 3, 0,
	// right
	1, 5, 6,
	6, 2, 1,
	// back
	7, 6, 5,
	5, 4, 7,
	// left
	4, 0, 3,
	3, 7, 4,
	// bottom
	4, 5, 1,
	1, 0, 4,
	// top
	3, 2, 6,
	6, 7, 3
};

//static const float cube_vertex_buffer[] = {
//	//front
//	-1.0, -1.0, 1.0, 0.0f, 0.0f,
//	 1.0, -1.0, 1.0, 0.0f, 0.0f,
//	 1.0, 1.0, 1.0, 0.0f, 0.0f,
//	-1.0, 1.0, 1.0, 0.0f, 0.0f,
//	//right
//	1.0, 1.0, 1.0, 0.0f, 0.0f,
//	1.0, 1.0, -1.0, 0.0f, 0.0f,
//	1.0, -1.0, -1.0, 0.0f, 0.0f,
//	1.0, -1.0, 1.0, 0.0f, 0.0f,
//	//back
//	-1.0, -1.0, -1.0, 0.0f, 0.0f,
//	 1.0, -1.0, -1.0, 0.0f, 0.0f,
//	 1.0, 1.0, -1.0, 0.0f, 0.0f,
//	-1.0, 1.0, -1.0, 0.0f, 0.0f,
//	//left
//	-1.0, -1.0, -1.0, 0.0f, 0.0f,
//	-1.0, -1.0, 1.0, 0.0f, 0.0f,
//	-1.0, 1.0, 1.0, 0.0f, 0.0f,
//	-1.0, 1.0, -1.0, 0.0f, 0.0f,
//	//upper
//	1.0, 1.0, 1.0, 0.0f, 0.0f,
//	-1.0, 1.0, 1.0, 0.0f, 0.0f,
//	-1.0, 1.0, -1.0, 0.0f, 0.0f,
//	 1.0, 1.0, -1.0, 0.0f, 0.0f,
//	 //bottom
//	-1.0, -1.0, -1.0, 0.0f, 0.0f,
//	 1.0, -1.0, -1.0, 0.0f, 0.0f,
//	 1.0, -1.0, 1.0, 0.0f, 0.0f,
//	-1.0, -1.0, 1.0, 0.0f, 0.0f,
//};
//
//static const unsigned int cube_indices[] = {
//	0, 1, 2, 0, 2, 3, //front
//	4, 5, 6, 4, 6, 7, //right
//	8, 9, 10, 8, 10, 11, //back
//	12, 13, 14, 12, 14, 15, //left
//	16, 17, 18, 16, 18, 19, //upper
//	20, 21, 22, 20, 22, 23 //bottom
//};

void DrawCube(GLfloat centerPosX, GLfloat centerPosY, GLfloat centerPosZ, GLfloat edgeLength);

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
		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
		GLCall(glEnable(GL_DEPTH_TEST));

		unsigned int vao;
		GLCall(glGenVertexArrays(1, &vao));
		GLCall(glBindVertexArray(vao));

		VertexArray va;
		VertexBuffer vb(cube_vertices, 24 * 3 * sizeof(float));
		VertexBufferLayout layout;
		layout.Push<float>(3);
		//layout.Push<float>(2);
		va.AddBuffer(vb, layout);

		IndexBuffer ib(cube_elements, 36);

		//glm::mat4 proj = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -100.0f, 100.0f);
		//glm::mat4 proj = glm::ortho(0.0f, (float)ResolutionX, 0.0f, (float)ResolutionY, 0.0f, 0.0f); //per pixel projection matrix
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1.0f * (float)ResolutionX / (float)ResolutionY, 0.1f, 50.0f);
		//glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 view = glm::lookAt(glm::vec3(0.0, 2.0, 0.0), glm::vec3(0.0, 0.0, -4.0), glm::vec3(0.0, 1.0, 0.0));
		//view = glm::rotate(view, 45 * 0.017453292f, glm::vec3(0, 1, 0));
		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -4.0));

		glm::mat4 mvp = projection * view * model;
		
		//glm::vec3(sin(std::time(nullptr)) * 5

		Shader shader("res/shaders/Base.shader");
		shader.Bind();
		shader.SetUniform4f("_Color", 1.0f, 1.0f, 1.0f, 1.0f);

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
			shader.SetUniformMatrix4f("_MVP", mvp);

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