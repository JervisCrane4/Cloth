#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "cloth.h"
#include "shader.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

static float const FPS = 30.0;
static float const dt = 1.0f / FPS;

bool isWireframe = false;

void GLAPIENTRY MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	std::string error = "init error";
	switch (type)
	{
	case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
	case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
	case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
	case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
	case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
	case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
	case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
	};

	if (type == GL_DEBUG_TYPE_ERROR)
		std::cerr << "GL CALLBACK:  ** GL ERROR ** "
		<< " type = " << error
		<< " id = " << id
		<< " severity = " << severity
		<< " message = " << message
		<< std::endl;
	else
		std::cout << "GL CALLBACK:"
		<< " type = " << type
		<< " id = " << id
		<< " severity = " << severity
		<< " message = " << message
		<< std::endl;
}


void checkWireframe()
{
	if(isWireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

}


int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	int width = 800; int height = 800;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(width, height, "Cloth Simulation", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK) return -1;

	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(MessageCallback, 0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330 core");
	ImGui::StyleColorsDark();

	Shader program("code/res/basic.shader");
	program.Bind();

	int Nu = 20; int Nv = 20;
	Cloth cloth(Nu, Nv, program);

	glm::vec3 eye = glm::vec3(0.5f, 0.25f, -0.5f);
	glm::vec3 center = glm::vec3(0.0f, -0.5f, 0.5f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::mat4 view = glm::lookAt(eye, center, up);
	glm::mat4 proj = glm::perspective(glm::radians(90.0f), (float)width / (float)height, 0.1f, 20.0f);

	program.setUniformMatrix("view", view);
	program.setUniformMatrix("proj", proj);

	program.setUniform3f("cameraPos", eye.x, eye.y, eye.z);

	float lightPos[3] = { 0.0f, 5.0f, 0.0f };
	float lightColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	float ambient = 0.5f;
	float diffuse = 0.5f;
	float specular = 0.5f;
	
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClearColor(150.0f / 256.0f, 12.0f / 256.0f, 120.0f / 256.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//view = glm::rotate(view, 0.01f,glm::vec3(0.0f, 1.0f, 0.0f));
		//program.setUniformMatrix("view", view);

		checkWireframe();
		cloth.updateForces();
		cloth.updatePosition(dt);

		program.setUniform3f("lightPos", lightPos[0], lightPos[1], lightPos[2]);
		program.setUniform4f("f_lightColor", lightColor[0], lightColor[1], lightColor[2], lightColor[3]);
		program.setUniform1i("wireframe", (int)isWireframe);

		program.setUniform1f("a_strength", ambient);
		program.setUniform1f("d_strength", diffuse);
		program.setUniform1f("s_strength", specular);

		cloth.render();
		
		{
			ImGui::Begin("Parameters");

			ImGui::SliderFloat3("Light position", lightPos, -5.0f, 5.0f);
			ImGui::SliderFloat4("Light color", lightColor, 0.0f, 1.0f);

			ImGui::SliderFloat("ambient", &ambient, 0.0f, 1.0f);
			ImGui::SliderFloat("diffuse", &diffuse, 0.0f, 1.0f);
			ImGui::SliderFloat("specular", &specular, 0.0f, 1.0f);

			ImGui::Checkbox("Enable wind", &(cloth.is_wind));
			ImGui::SliderFloat("Wind strength", &(cloth.wind_strength), 0.0f, 0.0025f);
			ImGui::SliderFloat("Wind direction x", &(cloth.wind_direction.x), -1.0f, 1.0f);
			ImGui::SliderFloat("Wind direction y", &(cloth.wind_direction.y), -1.0f, 1.0f);
			ImGui::SliderFloat("Wind direction z", &(cloth.wind_direction.z), -1.0f, 1.0f);

			ImGui::Checkbox("Wireframe", &isWireframe);

			ImGui::End();
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
	return 0;
}