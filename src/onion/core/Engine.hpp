#pragma once

#include <GLFW/glfw3.h>
#include <cstdio>

#include <thread>

#include "../renderer/renderer.hpp"

namespace Onion
{
	class Engine {
	public:
		Engine();
		~Engine();

		void Run();

	private:
		Renderer m_Renderer;
	};
}