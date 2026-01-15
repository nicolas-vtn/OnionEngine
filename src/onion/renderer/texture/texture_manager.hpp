#pragma once

#include <vector>
#include <unordered_map>
#include <stdexcept>

#include "texture.hpp"

namespace Onion::Rendering {

	class TextureManager {

	public:
		TextureManager() = default;
		~TextureManager() = default;

		Texture* LoadTexture(const std::string& filePath) {

			// Check if texture is already loaded
			auto it = m_Textures.find(filePath);
			if (it != m_Textures.end()) {
				return &(it->second);
			}

			// Load new texture
			Texture texture(filePath);
			if (!texture.HasBeenLoaded()) {
				throw std::runtime_error("Failed to load texture: " + filePath);
			}

			// Store and return the texture
			m_Textures[filePath] = texture;
			return &m_Textures[filePath];
		}
	};

private:
	std::unordered_map<std::string, Texture> m_Textures;

} // namespace Onion::Rendering