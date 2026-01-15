#pragma once

#include <vector>
#include <glad/glad.h>

#include "../structs/vertex.hpp"
#include "../material/material.hpp"
#include "../shader/shader.hpp"

namespace Onion::Rendering {

	class Mesh
	{

	private:
		GLuint vao{}, vbo{}, ebo{};
		uint32_t indexCount{};

	public:
		Mesh() = default;
		Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);

		Material* material = nullptr;

		void Draw(const Shader& shader) const;
	};

} // namespace Onion::Rendering