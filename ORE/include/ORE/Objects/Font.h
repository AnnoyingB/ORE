#pragma once
#include "glm/glm.hpp"
#include "ft2build.h"
#include "Buffers/VertexArray.h"
#include "Shader.h"
#include "glad/glad.h"
#include FT_FREETYPE_H

#include <string>
#include <assert.h>
#include <unordered_map>
#include <iostream>

namespace ORE {
	struct Character {
		unsigned int TextureID;  // ID handle of the glyph texture
		glm::ivec2   Size;       // Size of glyph
		glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
		unsigned int Advance;    // Offset to advance to next glyph
	};

	class Font {
	private:
		VertexArray* vao;
		VertexBuffer* vbo;
		Shader* textShader;
		float fontHeight;
	public:
		static FT_Library ft;
		std::unordered_map<char, Character> Characters = std::unordered_map<char, Character>();
		FT_Face face = NULL;

		Font(const std::string& fontAtlas, int fontHeight, int characters = 128);
		virtual ~Font();

		void RenderText(const std::string& text, glm::vec2 pos, float fontSize, glm::vec3 color = glm::vec3(1.f, 1.f, 1.f));
	};
}