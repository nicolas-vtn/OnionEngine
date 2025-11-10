#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <thread>
#include <string>

#include "shader/shader.hpp"
#include "camera/camera.hpp"
#include "inputs_manager/inputs_manager.hpp"

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
		InputsManager m_InputsManager;
		InputsSnapshot m_InputsSnapshot;

	private:
		void ProcessCameraMovement(const InputsSnapshot& inputs);
	};
}