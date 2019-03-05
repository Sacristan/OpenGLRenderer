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

const int ResolutionX = 800;
const int ResolutionY = 600;

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

const glm::vec3 axis_x(1, 0, 0);
const glm::vec3 axis_y(0, 1, 0);
const glm::vec3 axis_z(0, 0, 1);

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
		ImGui::StyleColorsDark();

		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 130");

		bool show_demo_window = true;
		bool show_another_window = false;
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

		glm::vec3 cameraPos = glm::vec3(0.0, 2.0, 0.0);

		float fov = 100.0f;
		float nearClippingPlane = 0.1f;
		float farClippingPlane = 50.0f;

		float matrixCameraPos[3] = { 0.0, 2.0, 0.0 };
		float matrixTranslation[3] = { 0, 0, -4.0 };
		float matrixRotation[3] = { 0, 0, 0 };
		float matrixScale[3] = { 1.0, 1.0, 1.0 };

		while (!glfwWindowShouldClose(window))
		{
			float deltaTime = clock() - lastFrameTime;
			renderer.Clear();

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			{
				ImGui::Begin("Controls");

				ImGui::BeginGroup();

				ImGui::Text("Cube Transform");
				ImGui::InputFloat3("Position", matrixTranslation, 3);
				ImGui::InputFloat3("Rotation", matrixRotation, 3);
				ImGui::InputFloat3("Scale", matrixScale, 3);
				ImGui::EndGroup();

				ImGui::Separator();

				ImGui::Text("Camera");
				ImGui::SliderFloat("FOV", &fov, 45.0f, 120.0f);
				ImGui::InputFloat3("Camera Pos", matrixCameraPos, 3);
				ImGui::Text("Clipping Planes: ");
				ImGui::InputFloat("Near", &nearClippingPlane);
				ImGui::InputFloat("Far", &farClippingPlane);

				ImGui::End();
			}

			angle = matrixRotation[1];
			angle += deltaTime * rotationSpeed;
			matrixRotation[1] = angle;

			glm::vec3 pos = glm::vec3(matrixTranslation[0], matrixTranslation[1], matrixTranslation[2]);
			glm::vec3 rot = glm::vec3(matrixRotation[0], matrixRotation[1], matrixRotation[2]);
			glm::vec3 scale = glm::vec3(matrixScale[0], matrixScale[1], matrixScale[2]);

			glm::vec3 cameraPos = glm::vec3(matrixCameraPos[0], matrixCameraPos[1], matrixCameraPos[2]);

			glm::mat4 projection = glm::perspective(glm::radians(fov), 1.0f * (float)ResolutionX / (float)ResolutionY, nearClippingPlane, farClippingPlane); //world2screen matrix
			glm::mat4 view = glm::lookAt(cameraPos, pos, axis_y); //camera matrix 

			glm::mat4 model = glm::mat4(1.0f); //object matrix
			model = glm::translate(model, pos);
			model = glm::scale(model, scale);

			model = glm::rotate(model, glm::radians(rot.x), axis_x);
			model = glm::rotate(model, glm::radians(rot.y), axis_y);
			model = glm::rotate(model, glm::radians(rot.z), axis_z);

			glm::mat4 mvp = projection * view * model;

			shader.Bind();
			shader.SetUniform4f("_Color", r, 0.25f, 1.0f, 1.0f);
			shader.SetUniformMatrix4f("_MVP", mvp);

			va.Bind();
			ib.Bind();

			renderer.Draw(va, ib, shader);

			if (r > 1.0f) increment *= -1;
			else if (r < 0.05f) increment = abs(increment);

			r += increment;

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