#include "renderer.hpp"

#include <iostream>

using namespace Onion::Rendering;

static void error_callback(int code, const char* desc) {
	std::fprintf(stderr, "GLFW error %d: %s\n", code, desc);
}

Renderer::Renderer() : m_Camera(glm::vec3(0.0f, 0.0f, -3.0f), m_WindowWidth, m_WindowHeight)
{
	m_Camera.SetFront(glm::vec3(0.0f, 0.0f, 1.0f)); // Look towards positive Z

	m_ViewMatrix = m_Camera.GetViewMatrix();
	m_ProjectionMatrix = m_Camera.GetProjectionMatrix();
	m_ViewProjMatrix = m_ProjectionMatrix * m_ViewMatrix;
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

	m_InputsManager.SetMouseCaptureEnabled(true);

	while (!stopToken.stop_requested() && !glfwWindowShouldClose(m_Window)) {
		// No GL calls needed; just clear to black if you like:
		glClearColor(0.1f, 0.1f, 0.12f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Calculate Delta Time
		double currentFrame = glfwGetTime();
		m_DeltaTime = currentFrame - m_LastFrame;
		m_LastFrame = currentFrame;

		// Pool inputs
		m_InputsManager.PoolInputs();
		m_InputsSnapshot = m_InputsManager.GetInputsSnapshot();

		// Process Camera Movement
		ProcessCameraMovement(m_InputsSnapshot);

		// Get Camera projection, view and ProjView Matix
		m_ProjectionMatrix = m_Camera.GetProjectionMatrix();
		m_ViewMatrix = m_Camera.GetViewMatrix();
		m_ViewProjMatrix = m_ProjectionMatrix * m_ViewMatrix;

		shader.setMat4("uView", m_ViewMatrix);
		shader.setMat4("uProj", m_ProjectionMatrix);
		shader.setMat4("uViewProj", m_ViewProjMatrix);
		shader.setMat4("uModel", glm::mat4(1.0f));

		shader.Use();
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		std::this_thread::sleep_for(std::chrono::milliseconds(16)); // Approx ~60 FPS

		glfwSwapBuffers(m_Window);
		glfwPollEvents();
	}
}

void Renderer::ProcessCameraMovement(const InputsSnapshot& inputs)
{
	if (!inputs.MouseCaptureEnabled) {
		return; // If mouse capture is not enabled, skip camera movement processing
	}

	// Camera's Orientation
	if (inputs.MouseXoffset != 0.f || inputs.MouseYoffset != 0.f) {
		float sensitivity = 0.1f;
		float xoffset = inputs.MouseXoffset * sensitivity;
		float yoffset = inputs.MouseYoffset * sensitivity;

		m_Camera.SetYaw(m_Camera.GetYaw() + xoffset);
		m_Camera.SetPitch(m_Camera.GetPitch() + yoffset);

		if (m_Camera.GetPitch() > 89.0f)
			m_Camera.SetPitch(89.0f);
		if (m_Camera.GetPitch() < -89.0f)
			m_Camera.SetPitch(-89.0f);

		// Updates the Camera's facing direction based on yaw and pitch
		const float CamYaw = m_Camera.GetYaw();
		const float CamPitch = m_Camera.GetPitch();
		glm::vec3 front(0.f);
		front.x = cos(glm::radians(CamYaw)) * cos(glm::radians(CamPitch));
		front.y = sin(glm::radians(CamPitch));
		front.z = sin(glm::radians(CamYaw)) * cos(glm::radians(CamPitch));
		m_Camera.SetFront(glm::normalize(front));
	}

	// Camera's speed
	// Adjust camera Speed
	if (inputs.MouseScrollOffsetChanged) {
		float xoffset, yoffset;
		m_InputsManager.GetMouseScrollOffset(xoffset, yoffset);
		if (yoffset != 0.f) {
			float coeefIncrease = 1.3f;
			float coeefDecrease = 0.7f;
			if (yoffset > 0) {
				m_CameraSpeed *= coeefIncrease; // Speed up
			}
			else if (yoffset < 0) {
				m_CameraSpeed *= coeefDecrease; // Slow down
			}
		}
	}

	// Camera's Position
	float velocity = m_CameraSpeed * m_DeltaTime;

	if (inputs.KeyLCtrl) {
		velocity *= 2.0f; // Double speed if left control is pressed
	}

	// Flatten the front vector for XZ movement
	glm::vec3 CamFront = m_Camera.GetFront();
	glm::vec3 frontXZ = glm::normalize(glm::vec3(CamFront.x, 0.0f, CamFront.z));
	const glm::vec3 Up(0.0f, 1.0f, 0.0f); // Up vector

	if (inputs.KeyW)
		m_Camera.SetPosition(m_Camera.GetPosition() + frontXZ * velocity);
	if (inputs.KeyS)
		m_Camera.SetPosition(m_Camera.GetPosition() - frontXZ * velocity);
	if (inputs.KeyA)
		m_Camera.SetPosition(m_Camera.GetPosition() - glm::normalize(glm::cross(frontXZ, Up)) * velocity);
	if (inputs.KeyD)
		m_Camera.SetPosition(m_Camera.GetPosition() + glm::normalize(glm::cross(frontXZ, Up)) * velocity);
	if (inputs.KeySpace) // Jump / up
		m_Camera.SetPosition(m_Camera.GetPosition() + Up * velocity);
	if (inputs.KeyLShift) // Down
		m_Camera.SetPosition(m_Camera.GetPosition() - Up * velocity);
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

	// Initialize Inputs Manager
	m_InputsManager.Init(m_Window);
}
