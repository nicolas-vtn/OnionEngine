#pragma once

#include <string>
#include <unordered_map>
#include <memory>

#include "../texture/texture.hpp"
#include "../material/material.hpp"

namespace Onion::Rendering {

	class AssetManager {

	public:
		AssetManager() = default;
		~AssetManager() = default;

		Texture* LoadTexture(const std::string& filePath) {
			// Check if texture is already loaded
			auto it = m_Textures.find(filePath);
			if (it != m_Textures.end()) {
				return it->second.get();
			}
			// Load new texture
			auto texture = std::make_unique<Texture>(filePath);
			texture->Bind();
			if (!texture->HasBeenLoaded()) {
				throw std::runtime_error("Failed to load texture: " + filePath);
			}
			// Store and return the texture
			Texture* texturePtr = texture.get();
			m_Textures[filePath] = std::move(texture);
			return texturePtr;
		}

		Material* CreateMaterial(const std::string& name) {

			// Check if material already exists
			auto it = m_Materials.find(name);
			if (it != m_Materials.end()) {
				return it->second.get();
			}

			// Create new material
			auto material = std::make_unique<Material>();

			// Store and return the material
			Material* materialPtr = material.get();
			m_Materials[name] = std::move(material);
			return materialPtr;
		}

	private:
		std::unordered_map<std::string, std::unique_ptr<Texture>> m_Textures;
		std::unordered_map<std::string, std::unique_ptr<Material>> m_Materials;
	};

} // namespace Onion::Rendering