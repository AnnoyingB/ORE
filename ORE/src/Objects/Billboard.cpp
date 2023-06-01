#include "Objects/Billboard.h"
#include "Renderer.h"

std::array<glm::vec2, 4> ORE::SquareVertices = {
	// top
	glm::vec2 {-.5f, .5f}, {.5f, .5f},
	// bottom
	{.5f, -.5f}, {-.5f, -.5f}
};
namespace ORE {
	void Billboard::Scale(bool scale) {
		Material->GetShader().SetBool("scale", scale);
	}

	void Billboard::Update() {
		Material->SetMatrices(Renderer::CurrentCamera, ModelMatrix);
		Material->GetShader().SetVec3("billboardPos", ModelMatrix[3]);
	}

	void Billboard::Render() {
		billboardTexture->Bind();
		Update();
		Draw(Renderer::CurrentCamera);
	}
}