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

		Texture* LoadTexture(const std::string& filePath);

		Material* CreateMaterial(const std::string& name);

		void FreeAllAssets();

	private:
		std::unordered_map<std::string, std::unique_ptr<Texture>> m_Textures;
		std::unordered_map<std::string, std::unique_ptr<Material>> m_Materials;
	};

} // namespace Onion::Rendering