#pragma once
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include <vector>

// who cares about compiler includes these days
#include "../../../ORE/include/ORE/Mesh.h"
#include "../../../ORE/vendor/glm/glm.hpp"

namespace OREML {
	class Loader {
	private:
		Assimp::Importer importer;
	public:
		std::vector<ORE::MeshCreateInfo> Load(std::string_view path, unsigned int flags = -1);
		ORE::MeshCreateInfo ParseMesh(aiMesh* mesh);
	};
}
