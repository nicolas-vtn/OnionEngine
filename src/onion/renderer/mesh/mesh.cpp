#include "mesh.hpp"

using namespace Onion::Rendering;

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
{
	indexCount = static_cast<uint32_t>(indices.size());

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER,
		vertices.size() * sizeof(Vertex),
		vertices.data(),
		GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		indices.size() * sizeof(uint32_t),
		indices.data(),
		GL_STATIC_DRAW);

	// Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);

	// Normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
		sizeof(Vertex),
		(void*)offsetof(Vertex, Normal));
	glEnableVertexAttribArray(1);

	// UV
	glVertexAttribPointer(
		2, 2, GL_FLOAT, GL_FALSE,
		sizeof(Vertex),
		(void*)offsetof(Vertex, UV)
	);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}

void Mesh::Draw(const Shader& shader) const
{
	shader.Use();

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}
