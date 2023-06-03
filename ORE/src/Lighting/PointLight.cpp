#include "Lighting/PointLight.h"
#include "Renderer.h"

namespace ORE {
	void PointLight::RenderBillboard() {
		if (!billboard) {
			billboard = Renderer::CreateBillboard("include\\ORE\\Assets\\LightBulb.png", true, false);
			billboard->Position = position;
		}

		billboard->Draw(Renderer::CurrentCamera);
	}
}