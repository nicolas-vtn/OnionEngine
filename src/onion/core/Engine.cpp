#include "Engine.hpp"

#include <iostream>

using namespace Onion;

Engine::Engine()
{
	std::cout << "Engine initialized." << std::endl;
}

Engine::~Engine()
{
	std::cout << "Engine destroyed." << std::endl;
}

void Engine::Run()
{
	std::cout << "Engine running..." << std::endl;

	m_Renderer.Start();

	// Wait for user input to stop the engine
	std::cout << "Press Enter to stop the engine..." << std::endl;
	std::cin.get();
}
