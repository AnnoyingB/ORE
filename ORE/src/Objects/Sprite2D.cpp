#include "Objects/Sprite2D.h"
#include "orepch.h"

namespace ORE {
	Sprite2D::~Sprite2D() {
		delete m_Texture;
	}

	void Sprite2D::Draw(Camera cam) {
		assert(cam.Ortho && "You need an orthographic projection for a Sprite2D");

		m_vao->Bind();
		m_vbo->Bind();
		m_ibo->Bind();
		glActiveTexture(GL_TEXTURE0);
		m_Texture->Bind();
		
		glDrawElements(GL_TRIANGLES, m_vbo->GetConstVertices().size(), GL_UNSIGNED_INT, m_ibo->GetConstIndices().data());

		m_Texture->Unbind();
		m_ibo->Unbind();
		m_vbo->Unbind();
		m_vao->Unbind();
	}
}