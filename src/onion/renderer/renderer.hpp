#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <thread>
#include <string>

#include "shader/shader.hpp"
#include "texture/texture.hpp"
#include "camera/camera.hpp"
#include "inputs_manager/inputs_manager.hpp"
#include "model/model.hpp"
#include "asset_manager/asset_manager.hpp"

namespace Onion::Rendering
{
	class Renderer {

	public:
		Renderer();
		~Renderer();

		void Start();
		void Stop();

	private:
		void InitWindow();
		void InitOpenGlState();

		void RenderThreadFunction(std::stop_token stopToken);
		std::jthread m_ThreadRenderer;

		//GLFW
	private:
		GLFWwindow* m_Window = nullptr;
		int m_WindowWidth = 800;
		int m_WindowHeight = 600;
		std::string m_WindowTitle = "Onion Engine";

	private:
		double m_DeltaTime = 0.0f;
		double m_LastFrame = 0.0f;

	private:
		Camera m_Camera;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewProjMatrix;

		float m_CameraSpeed = 5.0f;

	private:
		Onion::Controls::InputsManager m_InputsManager;
		std::shared_ptr<Onion::Controls::InputsSnapshot> m_InputsSnapshot;
		void RegisterInputs();
		int m_InputIdMoveForward = -1;
		int m_InputIdMoveBackward = -1;
		int m_InputIdMoveLeft = -1;
		int m_InputIdMoveRight = -1;
		int m_InputIdMoveUp = -1;
		int m_InputIdMoveDown = -1;
		int m_InputIdSpeedUp = -1;
		int m_InputIdUnfocus = -1;

		// ------------ TESTS ------------
	private:
		AssetManager m_AssetManager;
		Model m_AppleModel;
		Shader m_ShaderModel;

		void InitAppleModel();
		void UpdateShaderModel();
		void DrawAppleModel();

	private:
		void ProcessCameraMovement(const std::shared_ptr<Onion::Controls::InputsSnapshot>& inputs);
		void ProcessInputs(const std::shared_ptr<Onion::Controls::InputsSnapshot>& inputs);
	};
}