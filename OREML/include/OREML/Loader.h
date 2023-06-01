#pragma once
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include <vector>

#include "Mesh.h"
#include "glm/glm.hpp"

namespace OREML {
	class Loader {
	private:
		static Assimp::Importer* importer;
	public:
		static std::vector<ORE::MeshCreateInfo> Load(std::string_view path, unsigned int flags = -1);
		static ORE::MeshCreateInfo ParseMesh(aiMesh* mesh);
	};
}
