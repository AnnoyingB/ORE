#pragma once
#include "Texture.h"
#include "Mesh.h"

namespace ORE {
	struct Sprite2DCreateInfo {
		MeshCreateInfo mci; // Don't need to set shader path.
		TextureCreateInfo tci; // Don't need to set texture type.

		Sprite2DCreateInfo() {
			mci.shaderPath = "include\\ORE\\Shaders\\Sprite2DShader";
			tci.textureType = Texture2D;
		}
	};

	class Sprite2D : Mesh{
	private:
		Texture* m_Texture;
	public:
		Sprite2D() = default;
		Sprite2D(TextureCreateInfo tci, MeshCreateInfo mci)
			: Mesh(mci) {
			m_Texture = new Texture(tci);
		}
		virtual ~Sprite2D();

		void Draw(Camera cam) override;
	};
}
