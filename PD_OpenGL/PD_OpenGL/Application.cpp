/* TODO:
	* Create Cube Factory
	* Add IMGUI
	* Support TEXCOORD
*/

#include "imgui/imgui.h";
#include "imgui/imgui_impl_glfw.h";
#include "imgui/imgui_impl_opengl3.h";

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

template<typename T> struct PingPongValue {
	T value, min, max, speed, dir{ 1 };

	PingPongValue(T mMin, T mMax, T mSpeed)
		: value(mMin), min(mMin), max(mMax), speed(mSpeed) { }

	void step()
	{
		// allowing over-/underflow
		value += speed * dir;

		// clamp
		value += (min - value)*(value < min) + (max - value)*(value > max);

		// set dir
		dir += 2 * ((value == min) - (value == max));
	}
};

//3 pos 2 texcoord
GLfloat cube_vertex_data[] = {
	// front
	-1.0, -1.0,  1.0, 0.0, 0.0,
	 1.0, -1.0,  1.0, 1.0, 0.0,
	 1.0,  1.0,  1.0, 1.0, 1.0,
	-1.0,  1.0,  1.0, 0.0, 1.0,
	// back
	-1.0, -1.0, -1.0, 0.0, 0.0,
	 1.0, -1.0, -1.0, 1.0, 0.0,
	 1.0,  1.0, -1.0, 1.0, 1.0,
	-1.0,  1.0, -1.0, 0.0, 1.0,
};

GLuint cube_indices[] = {
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
		//GLCall(glEnable(GL_BLEND));
		//GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
		GLCall(glEnable(GL_DEPTH_TEST));

		unsigned int vao;
		GLCall(glGenVertexArrays(1, &vao));
		GLCall(glBindVertexArray(vao));

		VertexArray va;
		VertexBuffer vb(cube_vertex_data, 24 * 3 * sizeof(float));
		VertexBufferLayout layout;
		layout.Push<float>(3);
		layout.Push<float>(2);
		va.AddBuffer(vb, layout);

		IndexBuffer ib(cube_indices, 36);

		glm::vec3 pos = glm::vec3(0.0, 0.0, -4.0);

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
		double lastFrameTime = clock();
		float angle = 0;
		float rotationSpeed = 0.1f;

		PingPongValue<double> fovPingPong = PingPongValue<double>(45.0, 120.0, 0.25f);

		Renderer renderer;

		IMGUI_CHECKVERSION();
		
		ImGui::CreateContext();
		//// Setup Dear ImGui style
		ImGui::StyleColorsDark();

		//// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 130");

		bool show_demo_window = true;
		bool show_another_window = false;
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

		while (!glfwWindowShouldClose(window))
		{
			float deltaTime = clock() - lastFrameTime;
			renderer.Clear();

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			{
				static float f = 0.0f;
				static int counter = 0;

				ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

				ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
				ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
				ImGui::Checkbox("Another Window", &show_another_window);

				ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
				ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

				if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
					counter++;
				ImGui::SameLine();
				ImGui::Text("counter = %d", counter);

				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
				ImGui::End();
			}
			
			//float fov = PingPongValue(45.0f, 120.0f, deltaTime());
			fovPingPong.step();
			float fov = (float)fovPingPong.value;
			glm::mat4 projection = glm::perspective(glm::radians(fov), 1.0f * (float)ResolutionX / (float)ResolutionY, 0.1f, 50.0f); //world2screen matrix
			glm::mat4 view = glm::lookAt(glm::vec3(0.0, 2.0, 0.0), pos, glm::vec3(0.0, 1.0, 0.0)); //camera matrix
			glm::mat4 model = glm::translate(glm::mat4(1.0f), pos); //object matrix

			angle += deltaTime* rotationSpeed;
			glm::vec3 axis_y(0, 1, 0);

			glm::mat4 modelRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis_y);
			//glm::mat4 rotationMatrix = glm::mat4(1.0f);

			glm::mat4 mvp = projection * view * model * modelRotationMatrix;

			shader.Bind();
			shader.SetUniform4f("_Color", r, 0.25f, 1.0f, 1.0f);
			shader.SetUniformMatrix4f("_MVP", mvp);

			va.Bind();
			ib.Bind();

			renderer.Draw(va, ib, shader);

			if (r > 1.0f) increment *= -1;
			else if (r < 0.05f) increment = abs(increment);

			r += increment;
			//r = sin(clock());

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


			lastFrameTime = clock();
			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}