#pragma once
#include "Texture.h"
#include "Mesh.h"

#include "glm/gtx/matrix_decompose.hpp"
#include "glm/glm.hpp"
#include <array>

namespace ORE {
	extern std::array<glm::vec2, 4> SquareVertices;

	class Billboard : public Mesh {
	private:
		Texture* billboardTexture;
	public:
		Billboard(const std::string& texPath, MeshCreateInfo mci)
			: Mesh(mci) {
			TextureCreateInfo tci;
			tci.texturePath = texPath;
			tci.textureType = Texture2D;
			tci.filterMin = Linear;
			tci.filterMag = Linear;
			tci.repeatS = ClampToEdge;
			tci.repeatT = ClampToEdge;

			billboardTexture = new Texture(tci);
			//glActiveTexture(GL_TEXTURE1);
			//billboardTexture->Bind();
			//Material->GetShader().SetTexture("billboardTex", *billboardTexture, 1);
		}

		void Scale(bool); // if false(default) it will not scale to be the same size on screen
		void Update();
		void Render();
	};
}