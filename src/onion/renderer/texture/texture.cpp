#include "texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glad/glad.h>

#include <iostream>

using namespace Onion::Rendering;

Texture::Texture(const std::string& filePath) {
	if (!LoadFromFile(filePath)) {
		std::cout << "[TEXTURE] [ERROR] : Failed to load texture from file: " << filePath << std::endl;
	}
}

Texture::~Texture() {
	if (m_TextureID != 0) {
		std::cout << "[TEXTURE] [ERROR] : Texture '" << m_FilePath << "' not deleted before destruction. There is a memory leak."
			<< std::endl;
	}
}

bool Texture::LoadFromFile(const std::string& filePath) {
	m_FilePath = filePath;
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // For OpenGL coordinate system
	unsigned char* data = stbi_load(m_FilePath.c_str(), &width, &height, &nrChannels, 0);
	if (!data) {
		// handle error
		std::cout << "[TEXTURE] [ERROR] : Failed to load texture: " << m_FilePath << std::endl;
		return false;
	}

	// Store texture info
	m_Width = width;
	m_Height = height;
	m_NrChannels = nrChannels;

	// Saves the raw data, it will be freed after uploading to GPU
	m_Data = data;

	return true;
}

void Texture::UploadToGPU() const {

	if (m_Data == nullptr) {
		std::cout << "[TEXTURE] [ERROR] : No data to upload for texture: " << m_FilePath << std::endl;
		return;
	}

	glGenTextures(1, &m_TextureID);
	glBindTexture(GL_TEXTURE_2D, m_TextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, m_NrChannels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, m_Data);
	// glGenerateMipmap(GL_TEXTURE_2D);
	//  Texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // No mipmap
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	stbi_image_free(m_Data);

	m_Data = nullptr; // Data is freed after uploading to GPU

	m_HasBeenUploadedToGPU = true;
}

void Texture::Bind() const {

	if (!m_HasBeenUploadedToGPU) {
		UploadToGPU();
	}

	glActiveTexture(GL_TEXTURE0);			   // Activate texture slot 0
	glBindTexture(GL_TEXTURE_2D, m_TextureID); // Bind our atlas to slot 0
}

void Texture::Unbind() const {
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Delete() {
	glDeleteTextures(1, &m_TextureID);
	m_TextureID = 0;
}

unsigned int Texture::GetTextureID() const {
	return m_TextureID;
}

bool Texture::HasBeenLoaded() const {
	return m_TextureID != 0;
}

int Texture::GetWidth() const {
	return m_Width;
}
int Texture::GetHeight() const {
	return m_Height;
}
int Texture::GetNrChannels() const {
	return m_NrChannels;
}
