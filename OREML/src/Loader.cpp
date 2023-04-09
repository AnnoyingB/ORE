#include "Loader.h"

namespace OREML {
	std::vector<ORE::MeshCreateInfo> Loader::Load(std::string_view path, unsigned int flags) {
		aiScene* scene;
		if (flags == -1)
			scene = const_cast<aiScene*>(importer.ReadFile(path.data(), aiProcess_Triangulate | aiProcess_OptimizeMeshes));
		else
			scene = const_cast<aiScene*>(importer.ReadFile(path.data(), aiProcess_Triangulate | aiProcess_OptimizeMeshes | flags));
		
		std::vector<ORE::MeshCreateInfo> meshInfo;
		for (int i = 0; i < scene->mNumMeshes; i++)
			meshInfo.push_back(ParseMesh(scene->mMeshes[i]));

		return meshInfo;
	}

	ORE::MeshCreateInfo Loader::ParseMesh(aiMesh* mesh) {
		ORE::MeshCreateInfo meshInfo;
		std::vector<unsigned int> indices;

		for (int i = 0; i < mesh->mNumFaces; i++)
			for (int j = 0; j < mesh->mFaces[i].mNumIndices; j++)
				indices.push_back(*mesh->mFaces[i].mIndices);

		for (int i = 0; i < mesh->mNumVertices; i++) {
			aiVector3D pos = mesh->mVertices[i];
			aiColor4D* col = mesh->mColors[i];
			aiVector3D normal = mesh->mNormals[i];
			
			meshInfo.vertices.push_back({ glm::vec3(pos.x, pos.y, pos.z), glm::vec4(col->r, col->g, col->b, col->a), glm::vec3(normal.x, normal.y, normal.z) });
			meshInfo.indices = indices;
		}

		return meshInfo;
	}
}
