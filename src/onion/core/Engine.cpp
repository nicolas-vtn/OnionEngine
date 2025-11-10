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
}
