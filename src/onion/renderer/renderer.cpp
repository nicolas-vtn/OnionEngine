#include "renderer.hpp"

#include <iostream>

using namespace Onion::Rendering;
using namespace Onion::Controls;

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
		// positions          // colors           // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
	};

	//Create vertex buffer and vertex array
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// --- NEW: index data ---
	unsigned int indices[] = {
		0, 1, 2,   // first triangle
		2, 3, 0    // second triangle
	};

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// --- FIX: correct strides (8 floats per vertex) ---
	GLsizei stride = 8 * sizeof(float);

	// position (loc=0): offset 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);

	// color (loc=1): offset 3 floats
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// uv (loc=2): offset 6 floats
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	//Shader shader("assets/shaders/simple.vert", "assets/shaders/simple.frag");
	Shader shader("assets/shaders/texture_triangle.vert", "assets/shaders/texture_triangle.frag");
	Texture texture("assets/textures/container.jpg");

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

		// Process Global Inputs
		ProcessInputs(m_InputsSnapshot);

		// Process Camera Movement
		ProcessCameraMovement(m_InputsSnapshot);

		// Get Camera projection, view and ProjView Matix
		m_ProjectionMatrix = m_Camera.GetProjectionMatrix();
		m_ViewMatrix = m_Camera.GetViewMatrix();
		m_ViewProjMatrix = m_ProjectionMatrix * m_ViewMatrix;

		shader.Use();
		shader.setMat4("uView", m_ViewMatrix);
		shader.setMat4("uProj", m_ProjectionMatrix);
		shader.setMat4("uViewProj", m_ViewProjMatrix);
		shader.setMat4("uModel", glm::mat4(1.0f));

		texture.Bind();
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		//std::this_thread::sleep_for(std::chrono::milliseconds(16)); // Approx ~60 FPS

		glfwSwapBuffers(m_Window);
		glfwPollEvents();
	}
}

void Onion::Rendering::Renderer::RegisterInputs()
{
	m_InputIdMoveForward = m_InputsManager.RegisterInput(Key::W, InputConfig(false));
	m_InputIdMoveBackward = m_InputsManager.RegisterInput(Key::S);
	m_InputIdMoveLeft = m_InputsManager.RegisterInput(Key::A);
	m_InputIdMoveRight = m_InputsManager.RegisterInput(Key::D);
	m_InputIdMoveUp = m_InputsManager.RegisterInput(Key::Space);
	m_InputIdMoveDown = m_InputsManager.RegisterInput(Key::LeftShift);
	m_InputIdSpeedUp = m_InputsManager.RegisterInput(Key::LeftControl);
	m_InputIdUnfocus = m_InputsManager.RegisterInput(Key::Escape);
}

void Renderer::ProcessCameraMovement(const std::shared_ptr<InputsSnapshot>& inputs)
{
	if (!inputs->Mouse.CaptureEnabled) {
		return; // If mouse capture is not enabled, skip camera movement processing
	}

	// Camera's Orientation
	if (inputs->Mouse.MovementOffsetChanged) {
		const double sensitivity = 0.1f;
		const double xoffset = inputs->Mouse.Xoffset * sensitivity;
		const double yoffset = inputs->Mouse.Yoffset * sensitivity;

		m_Camera.SetYaw(m_Camera.GetYaw() + xoffset);
		m_Camera.SetPitch(m_Camera.GetPitch() + yoffset);

		if (m_Camera.GetPitch() > 89.0f)
			m_Camera.SetPitch(89.0f);
		if (m_Camera.GetPitch() < -89.0f)
			m_Camera.SetPitch(-89.0f);
	}

	// Adjust camera Speed
	if (inputs->Mouse.ScrollOffsetChanged) {
		const double xoffset = inputs->Mouse.ScrollXoffset;
		const double yoffset = inputs->Mouse.ScrollYoffset;
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

	if (inputs->GetKeyState(m_InputIdSpeedUp).IsPressed) {
		velocity *= 2.0f; // Double speed if left control is pressed
	}

	// Flatten the front vector for XZ movement
	glm::vec3 CamFront = m_Camera.GetFront();
	glm::vec3 frontXZ = glm::normalize(glm::vec3(CamFront.x, 0.0f, CamFront.z));
	const glm::vec3 Up(0.0f, 1.0f, 0.0f); // Up vector

	if (inputs->GetKeyState(m_InputIdMoveForward).IsPressed)
		m_Camera.SetPosition(m_Camera.GetPosition() + frontXZ * velocity);
	if (inputs->GetKeyState(m_InputIdMoveBackward).IsPressed)
		m_Camera.SetPosition(m_Camera.GetPosition() - frontXZ * velocity);
	if (inputs->GetKeyState(m_InputIdMoveLeft).IsPressed)
		m_Camera.SetPosition(m_Camera.GetPosition() - glm::normalize(glm::cross(frontXZ, Up)) * velocity);
	if (inputs->GetKeyState(m_InputIdMoveRight).IsPressed)
		m_Camera.SetPosition(m_Camera.GetPosition() + glm::normalize(glm::cross(frontXZ, Up)) * velocity);
	if (inputs->GetKeyState(m_InputIdMoveUp).IsPressed) // Jump / up
		m_Camera.SetPosition(m_Camera.GetPosition() + Up * velocity);
	if (inputs->GetKeyState(m_InputIdMoveDown).IsPressed) // Down
		m_Camera.SetPosition(m_Camera.GetPosition() - Up * velocity);
}

void Renderer::ProcessInputs(const std::shared_ptr<InputsSnapshot>& inputs)
{
	if (inputs->GetKeyState(m_InputIdUnfocus).IsPressed && inputs->Mouse.CaptureEnabled) {
		m_InputsManager.SetMouseCaptureEnabled(false);
	}

	const bool leftPressed = inputs->Mouse.LeftButtonPressed;
	const bool rightPressed = inputs->Mouse.RightButtonPressed;
	if ((leftPressed || rightPressed) && !inputs->Mouse.CaptureEnabled) {
		m_InputsManager.SetMouseCaptureEnabled(true);
	}

	// Resize handling
	if (inputs->Framebuffer.Resized) {
		m_WindowWidth = inputs->Framebuffer.Width;
		m_WindowHeight = inputs->Framebuffer.Height;
		m_Camera.SetAspectRatio(static_cast<float>(m_WindowWidth) / static_cast<float>(m_WindowHeight));

		// Update the OpenGL viewport
		glViewport(0, 0, m_WindowWidth, m_WindowHeight);
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

	// Load OpenGL function pointers with glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD\n";
		throw std::runtime_error("GLAD initialization failed");
	}

	// Initialize Inputs Manager
	m_InputsManager.Init(m_Window);
	m_InputsManager.SetMouseCaptureEnabled(false);
	RegisterInputs();
}
