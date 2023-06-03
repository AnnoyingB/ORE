#include "Objects/Font.h"
#include "Renderer.h"

namespace ORE {
    FT_Library Font::ft = NULL;
	Font::Font(const std::string& fontAtlas, int fontHeight, int characters) {
        if (FT_Init_FreeType(&ft))
        {
            assert(false && "Failed to initialize FreeType");
            return;
        }

        if (FT_New_Face(ft, fontAtlas.c_str(), 0, &face))
        {
            assert(false && "Failed to load font");
            return;
        }

        assert(face && "Face is nullptr");
        
        FT_Set_Pixel_Sizes(face, 0, fontHeight);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        for (unsigned char i = 0; i < characters; i++) {
            // load character glyph 
            if (FT_Load_Char(face, i, FT_LOAD_RENDER))
            {
                std::cout << "Failed to load glyph" << std::endl;
                continue;
            }

            // don't need to use the texture class.
            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );
            
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // now store character for later use
            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                face->glyph->advance.x
            };
            Characters.insert(std::pair<char, Character>(i, character));
        }

        vao = new VertexArray();
        vbo = new VertexBuffer();
        vao->Bind();
        vbo->Bind();
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        vbo->Unbind();
        vao->Unbind();

        textShader = new Shader(Shader::TextShader);
	}
    
    void Font::RenderText(const std::string& text, glm::vec2 pos, float scale, glm::vec3 color)
    {
        glDisable(GL_DEPTH_TEST);
        // activate corresponding render state	
        textShader->Bind();
        textShader->SetVec3("textColor", color);
        textShader->SetMat4("projection", Renderer::CurrentCamera.GetProjection());
        glActiveTexture(GL_TEXTURE0);
        vao->Bind();

        // iterate through all characters
        std::string::const_iterator c;
        for (c = text.begin(); c != text.end(); c++)
        {
            Character ch = Characters[*c];

            float xpos = pos.x + ch.Bearing.x * scale;
            float ypos = pos.y - (ch.Size.y - ch.Bearing.y) * scale;

            float w = ch.Size.x * scale;
            float h = ch.Size.y * scale;
            // update VBO for each character
            float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
            };
            // render glyph texture over quad
            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            // update content of VBO memory
            vbo->Bind();
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            vbo->Unbind();
            // render quad
            glDrawArrays(GL_TRIANGLES, 0, 6);
            // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            pos.x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
        }
        vao->Unbind();
        glBindTexture(GL_TEXTURE_2D, 0);
        glEnable(GL_DEPTH_TEST);
    }

    Font::~Font() {
        FT_Done_Face(face);
        FT_Done_FreeType(ft);
    }
}