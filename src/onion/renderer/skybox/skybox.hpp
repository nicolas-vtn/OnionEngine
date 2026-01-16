#pragma once

// #define STB_IMAGE_IMPLEMENTATION
// #include <stb_image.h>

#include "../shader/shader.hpp"

#include <mutex>

namespace Onion::Rendering {
	class Skybox {
	public:
		Skybox();
		~Skybox();

		void Render(const glm::mat4& ViewProjMatrix, const glm::mat4& Projection);
		void Delete();

	private:
		unsigned int m_TextureID = 0;

		unsigned int m_VAO = 0, m_VBO = 0;

		Shader m_ShaderSkybox;

		void InitSkybox();
		bool HasBeenInitialized() const;

		void LoadTextures();

		mutable std::mutex m_MutexInit;
		bool m_HasBeenInitialized = false;
	};
} // namespace Renderer_cpp