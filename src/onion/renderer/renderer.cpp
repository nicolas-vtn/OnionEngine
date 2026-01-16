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

	InitOpenGlState();

	InitAppleModel();

	while (!stopToken.stop_requested() && !glfwWindowShouldClose(m_Window)) {
		// No GL calls needed; just clear to black if you like:
		glClearColor(0.1f, 0.1f, 0.12f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

		// ------ TESTS MODELS ------
		UpdateShaderModel();
		DrawAppleModel();


		glfwSwapBuffers(m_Window);
		glfwPollEvents();
	}

	// Cleanup
	CleanupOpenGL();
}

void Onion::Rendering::Renderer::RegisterInputs()
{
	m_InputIdMoveForward = m_InputsManager.RegisterInput(Key::W);
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
		const float sensitivity = 0.1f;
		const float xoffset = static_cast<float>(inputs->Mouse.Xoffset * sensitivity);
		const float yoffset = static_cast<float>(inputs->Mouse.Yoffset * sensitivity);

		m_Camera.SetYaw(m_Camera.GetYaw() + xoffset);
		m_Camera.SetPitch(m_Camera.GetPitch() + yoffset);

		if (m_Camera.GetPitch() > 89.0f)
			m_Camera.SetPitch(89.0f);
		if (m_Camera.GetPitch() < -89.0f)
			m_Camera.SetPitch(-89.0f);
	}

	// Adjust camera Speed
	if (inputs->Mouse.ScrollOffsetChanged) {
		const float yoffset = static_cast<float>(inputs->Mouse.ScrollYoffset);
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
	float velocity = static_cast<float>(m_CameraSpeed * m_DeltaTime);

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

void Onion::Rendering::Renderer::InitOpenGlState()
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CCW);

	glDisable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Onion::Rendering::Renderer::InitAppleModel()
{
	// Create Model Shader
	m_ShaderModel = Shader("assets/shaders/model.vert", "assets/shaders/model.frag");
	m_ShaderModel.setInt("uAlbedo", 0);
	m_ShaderModel.setInt("uRoughness", 1);

	// Create Model
	m_AppleModel = Model("assets/models/food_apple_01_4k/food_apple_01_4k.gltf");

	// Create Material
	Material* appleMaterial = m_AssetManager.CreateMaterial("Apple");
	appleMaterial->Albedo = m_AssetManager.LoadTexture("assets/models/food_apple_01_4k/textures/food_apple_01_diff_4k.jpg");
	appleMaterial->Normal = m_AssetManager.LoadTexture("assets/models/food_apple_01_4k/textures/food_apple_01_nor_gl_4k.jpg");
	appleMaterial->Roughness = m_AssetManager.LoadTexture("assets/models/food_apple_01_4k/textures/food_apple_01_rough_4k.jpg");

	// Assign Material to Model
	m_AppleModel.SetMaterial(appleMaterial);
}

void Onion::Rendering::Renderer::UpdateShaderModel()
{
	m_ShaderModel.Use();

	// Matrices
	m_ShaderModel.setMat4("uView", m_ViewMatrix);
	m_ShaderModel.setMat4("uProj", m_ProjectionMatrix);
	m_ShaderModel.setMat4("uViewProj", m_ViewProjMatrix);
	m_ShaderModel.setMat4("uModel", glm::mat4(1.0f));

	// Lighting
	m_ShaderModel.setVec3("uLightDir", glm::normalize(glm::vec3(-1.0f, -1.0f, -0.5f)));
	m_ShaderModel.setVec3("uLightColor", glm::vec3(1.0f));
	m_ShaderModel.setVec3("uAmbient", glm::vec3(0.5f));

	// Camera
	m_ShaderModel.setVec3("uCameraPos", m_Camera.GetPosition());

	// Specular control
	m_ShaderModel.setFloat("uSpecularStrength", 0.5f);

}

void Onion::Rendering::Renderer::DrawAppleModel()
{
	Material* appleMaterial = m_AppleModel.GetMaterial();

	glActiveTexture(GL_TEXTURE0);
	appleMaterial->Albedo->Bind();

	glActiveTexture(GL_TEXTURE1);
	appleMaterial->Roughness->Bind();

	m_AppleModel.Draw(m_ShaderModel);
}

void Onion::Rendering::Renderer::CleanupOpenGL()
{
	m_AssetManager.FreeAllAssets();
	m_ShaderModel.Delete();
}