#include "asset_manager.hpp"

#include <stdexcept>

using namespace Onion::Rendering;

Texture* AssetManager::LoadTexture(const std::string& filePath) {
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

Material* AssetManager::CreateMaterial(const std::string& name) {

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

void Onion::Rendering::AssetManager::FreeAllAssets()
{
	// Delete all Textures
	for (auto& [key, texture] : m_Textures) {
		texture->Delete();
	}
	m_Textures.clear();

	// Delete all Materials
	for (auto& [key, material] : m_Materials) {

		if (material->Albedo) {
			material->Albedo->Delete();
		}

		if (material->Normal) {
			material->Normal->Delete();
		}

		if (material->Roughness) {
			material->Roughness->Delete();
		}
	}
	m_Materials.clear();

}