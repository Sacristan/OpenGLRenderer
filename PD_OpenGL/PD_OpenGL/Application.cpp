#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#define ASSERT(x) if(!(x)) __debugbreak(); //__debugbreak() VS exclusive
#define GLCall(x) GLClearErrors();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

static void GLClearErrors() 
{
	while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError())
	{
		std::cout <<"OpenGL Error: ("<< error << ")" 
			<< function << " "
			<< file << ":"
			<< line << ":"
			<< std::endl;
		return false;
	}

	return true;
}

struct ShaderProgramSource
{
	std::string VertexShaderSource;
	std::string FragmentShaderSource;
};

static ShaderProgramSource ParseShader(const std::string& filepath)
{
	std::ifstream stream(filepath);

	enum class ShaderType {
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;

	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
				type = ShaderType::VERTEX;
			else if (line.find("fragment") != std::string::npos)
				type = ShaderType::FRAGMENT;
		}
		else
		{
			ss[int(type)] << line << '\n';
		}
	}

	return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);

	if (result == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));

		glGetShaderInfoLog(id, length, &length, message);

		std::cout << "ERROR: Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader" << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}

	return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

int main(void)
{
	GLFWwindow* window;

	if (!glfwInit())
		return -1;

	window = glfwCreateWindow(640, 480, "...OpenGL Magick...", NULL, NULL);
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

	float positions[] = {
		-0.5f, -0.5,
		0.5f, -0.5f,
		0.5f, 0.5f,
		-0.5f, 0.5f,
	};

	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	unsigned int bufferId;
	GLCall(glGenBuffers(1, &bufferId));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, bufferId));
	GLCall(glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), positions, GL_STATIC_DRAW));

	GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));
	GLCall(glEnableVertexAttribArray(0));

	unsigned int ibo;
	GLCall(glGenBuffers(1, &ibo));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW));

	ShaderProgramSource shaderSource = ParseShader("res/shaders/Base.shader");
	unsigned int shader = CreateShader(shaderSource.VertexShaderSource, shaderSource.FragmentShaderSource);
	GLCall(glUseProgram(shader));

	GLCall(int uniformLocation = glGetUniformLocation(shader, "_Color"));
	ASSERT(uniformLocation != -1);
	//GLCall(glUniform4f(uniformLocation, 1.0f, 1.0f, 1.0f, 1.0f));

	float r = 0.0f;
	float increment = 0.05f;

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);
	
		GLCall(glUniform4f(uniformLocation, r, 0.25f, 1.0f, 1.0f));
		GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

		if (r > 1.0f) increment *= -1;
		else if (r < 0.05f) increment = abs(increment);

		r += increment;

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	GLCall(glDeleteProgram(shader));

	glfwTerminate();
	return 0;
}