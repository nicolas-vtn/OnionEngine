#pragma once

#include "../mesh/mesh.hpp"
#include "../shader/shader.hpp"

#include <stdexcept>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Onion::Rendering
{
	class Model
	{
	public:
		Model() = default;
		explicit Model(const std::string& path);

		void Draw(const Shader& shader) const;

		void SetMaterial(Material* material);
		Material* GetMaterial() const;

	private:
		std::vector<Mesh> m_Meshes;
		Material* m_Material = nullptr;

		void Load(const std::string& path);

		void ProcessNode(aiNode* node, const aiScene* scene);

		Mesh ProcessMesh(aiMesh* mesh);
	};

} // namespace Onion::Rendering
