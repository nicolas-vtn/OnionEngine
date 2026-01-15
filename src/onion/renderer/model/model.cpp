#include "model.hpp"

using namespace Onion::Rendering;

Model::Model(const std::string& path)
{
	Load(path);
}

void Model::Draw(const Shader& shader) const
{
	for (const auto& mesh : m_Meshes) {
		if (mesh.material)
			mesh.material->Albedo->Bind();

		mesh.Draw(shader);
	}
}

void Model::SetMaterial(Material* material)
{
	m_Material = material;

	for (auto& mesh : m_Meshes)
		mesh.material = material;
}

Material* Onion::Rendering::Model::GetMaterial() const
{
	return m_Material;
}

void Model::Load(const std::string& path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(
		path,
		aiProcess_Triangulate |
		aiProcess_GenNormals |       // safety
		aiProcess_FlipWindingOrder |
		aiProcess_CalcTangentSpace   // REQUIRED for normal maps
	);

	if (!scene || !scene->mRootNode)
		throw std::runtime_error(importer.GetErrorString());

	ProcessNode(scene->mRootNode, scene);
}

void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
	for (uint32_t i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* ai_mesh = scene->mMeshes[node->mMeshes[i]];
		m_Meshes.push_back(ProcessMesh(ai_mesh));
	}

	for (uint32_t i = 0; i < node->mNumChildren; i++)
		ProcessNode(node->mChildren[i], scene);
}

Mesh Model::ProcessMesh(aiMesh* mesh) {
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	for (uint32_t i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex v{};
		v.Position = {
			mesh->mVertices[i].x,
			mesh->mVertices[i].y,
			mesh->mVertices[i].z
		};

		v.Normal = {
			mesh->mNormals[i].x,
			mesh->mNormals[i].y,
			mesh->mNormals[i].z
		};

		if (mesh->HasTextureCoords(0))
		{
			v.UV = {
				mesh->mTextureCoords[0][i].x,
				mesh->mTextureCoords[0][i].y
			};
		}
		else
		{
			v.UV = { 0.0f, 0.0f };
		}

		vertices.push_back(v);
	}

	for (uint32_t i = 0; i < mesh->mNumFaces; i++)
	{
		const aiFace& face = mesh->mFaces[i];
		for (uint32_t j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	return Mesh(vertices, indices);
}

