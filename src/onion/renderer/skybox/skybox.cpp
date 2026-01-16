#include "skybox.hpp"

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <stb_image.h>

#include <iostream>
#include <vector>

using namespace Onion::Rendering;

Skybox::Skybox() {
}

Skybox::~Skybox() {
}

void Skybox::Render(const glm::mat4& View, const glm::mat4& Projection)
{
	if (!HasBeenInitialized())
		InitSkybox();

	// ------------------------------------------------------------
	// Save OpenGL state
	// ------------------------------------------------------------
	GLint oldProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &oldProgram);

	GLint oldDepthFunc;
	glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFunc);

	GLboolean oldDepthMask;
	glGetBooleanv(GL_DEPTH_WRITEMASK, &oldDepthMask);

	GLint oldActiveTexture;
	glGetIntegerv(GL_ACTIVE_TEXTURE, &oldActiveTexture);

	GLint oldCubemap;
	glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &oldCubemap);

	GLboolean cullEnabled = glIsEnabled(GL_CULL_FACE);
	GLint oldCullFace;
	glGetIntegerv(GL_CULL_FACE_MODE, &oldCullFace);

	// ------------------------------------------------------------
	// Skybox render state
	// ------------------------------------------------------------
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);

	glDisable(GL_CULL_FACE); // or glCullFace(GL_FRONT);

	m_ShaderSkybox.Use();

	glm::mat4 viewNoTranslation = glm::mat4(glm::mat3(View));
	m_ShaderSkybox.setMat4("view", viewNoTranslation);
	m_ShaderSkybox.setMat4("projection", Projection);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID);

	glBindVertexArray(m_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	// ------------------------------------------------------------
	// Restore OpenGL state
	// ------------------------------------------------------------
	glBindTexture(GL_TEXTURE_CUBE_MAP, oldCubemap);
	glActiveTexture(oldActiveTexture);

	if (cullEnabled)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);

	glCullFace(oldCullFace);

	glDepthMask(oldDepthMask);
	glDepthFunc(oldDepthFunc);

	glUseProgram(oldProgram);
}

void Skybox::Delete() {
	m_ShaderSkybox.Delete();

	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
	glDeleteTextures(1, &m_TextureID);
}


void Skybox::InitSkybox() {

	if (HasBeenInitialized()) {
		return; // Already initialized
	}

	std::lock_guard<std::mutex> lock(m_MutexInit);

	m_ShaderSkybox = Shader("assets/shaders/skybox.vert", "assets/shaders/skybox.frag");

	LoadTextures();

	const float skyboxVertices[] = { -1.0f, 1.0f,  -1.0f, // front face
									-1.0f, -1.0f, -1.0f, 1.0f,	-1.0f, -1.0f, 1.0f,	 -1.0f, -1.0f, 1.0f,  1.0f,	 -1.0f, -1.0f, 1.0f,  -1.0f,

									-1.0f, -1.0f, 1.0f, // back face
									-1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,	-1.0f, -1.0f, 1.0f,	 1.0f,	-1.0f, -1.0f, 1.0f,

									1.0f,  -1.0f, -1.0f, // right face
									1.0f,  -1.0f, 1.0f,	 1.0f,	1.0f,  1.0f,  1.0f,	 1.0f,	1.0f,  1.0f,  1.0f,	 -1.0f, 1.0f,  -1.0f, -1.0f,

									-1.0f, -1.0f, 1.0f, // front face
									-1.0f, 1.0f,  1.0f,	 1.0f,	1.0f,  1.0f,  1.0f,	 1.0f,	1.0f,  1.0f,  -1.0f, 1.0f,	-1.0f, -1.0f, 1.0f,

									-1.0f, 1.0f,  -1.0f, // top face
									1.0f,  1.0f,  -1.0f, 1.0f,	1.0f,  1.0f,  1.0f,	 1.0f,	1.0f,  -1.0f, 1.0f,	 1.0f,	-1.0f, 1.0f,  -1.0f,

									-1.0f, -1.0f, -1.0f, // bottom face
									-1.0f, -1.0f, 1.0f,	 1.0f,	-1.0f, -1.0f, 1.0f,	 -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,	1.0f,  -1.0f, 1.0f };

	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	m_HasBeenInitialized = true;
}

bool Skybox::HasBeenInitialized() const {
	std::lock_guard<std::mutex> lock(m_MutexInit);
	return m_HasBeenInitialized;
}

void Skybox::LoadTextures() {

	const std::vector<std::string> faces = {
		"assets/textures/skybox/right.bmp",  // +X
		"assets/textures/skybox/left.bmp",   // -X
		"assets/textures/skybox/top.bmp",	   // +Y
		"assets/textures/skybox/bottom.bmp", // -Y
		"assets/textures/skybox/front.bmp",  // +Z
		"assets/textures/skybox/back.bmp"	   // -Z
	};

	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;

	stbi_set_flip_vertically_on_load(false);

	for (unsigned int i = 0; i < faces.size(); i++) {
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else {
			std::cerr << "Failed to load cubemap texture at " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	m_TextureID = textureID; // Store the texture ID for later use
}
