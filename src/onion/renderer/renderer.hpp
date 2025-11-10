#pragma once

#include <GLFW/glfw3.h>

#include <thread>
#include <string>

namespace Onion
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
	};
}