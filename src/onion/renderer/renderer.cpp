#include "renderer.hpp"

#include <iostream>

using namespace Onion;

static void error_callback(int code, const char* desc) {
	std::fprintf(stderr, "GLFW error %d: %s\n", code, desc);
}

Renderer::Renderer()
{

}

Renderer::~Renderer()
{
	Stop();
}

void Renderer::Start()
{
	m_ThreadRenderer = std::jthread([this](std::stop_token st) {
		RenderThreadFunction(st);
		});
}

void Renderer::Stop()
{
	if (m_ThreadRenderer.joinable()) {
		m_ThreadRenderer.request_stop();
		m_ThreadRenderer.join();
	}

	glfwDestroyWindow(m_Window);
	glfwTerminate();
}

void Renderer::RenderThreadFunction(std::stop_token stopToken)
{
	InitWindow();
	while (!stopToken.stop_requested() && !glfwWindowShouldClose(m_Window)) {
		// No GL calls needed; just clear to black if you like:
		glClearColor(0.1f, 0.1f, 0.12f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(m_Window);
		glfwPollEvents();
	}
}

void Onion::Renderer::InitWindow()
{
	glfwSetErrorCallback(error_callback);

	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		throw std::runtime_error("GLFW initialization failed");
	}

	// Request an OpenGL context
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	m_Window = glfwCreateWindow(m_WindowWidth, m_WindowHeight, m_WindowTitle.c_str(), nullptr, nullptr);
	if (!m_Window) {
		glfwTerminate();
		throw std::runtime_error("Failed to create GLFW window");
	}

	glfwMakeContextCurrent(m_Window);

	glfwSwapInterval(1); // vsync

	// ESC to close
	glfwSetKeyCallback(m_Window, [](GLFWwindow* w, int key, int, int action, int) {
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(w, 1);
		});

}
