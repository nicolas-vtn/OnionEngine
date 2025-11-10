#include "renderer.hpp"

#include <iostream>

using namespace Onion::Rendering;

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

	// Define triangle vertices
	float vertices[] = {
		 0.0f,  0.5f, 0.0f,  // top
		-0.5f, -0.5f, 0.0f,  // bottom left
		 0.5f, -0.5f, 0.0f   // bottom right
	};

	//Create vertex buffer and vertex array
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Vertex attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	Shader shader("assets/shaders/simple.vert", "assets/shaders/simple.frag");

	while (!stopToken.stop_requested() && !glfwWindowShouldClose(m_Window)) {
		// No GL calls needed; just clear to black if you like:
		glClearColor(0.1f, 0.1f, 0.12f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		shader.Use();
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(m_Window);
		glfwPollEvents();
	}
}

void Renderer::InitWindow()
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


	// Load OpenGL function pointers with glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD\n";
		throw std::runtime_error("GLAD initialization failed");
	}

}
