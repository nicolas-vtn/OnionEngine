#pragma once

#include <cstdio>
#include <thread>

#include "../renderer/renderer.hpp"

using namespace Onion::Rendering;

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