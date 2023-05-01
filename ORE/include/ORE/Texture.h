#pragma once
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "stb_image.h"
#include <string>

namespace ORE {
	enum Filter {
		NoFilter = GL_NONE,
		Nearest = GL_NEAREST,
		Linear = GL_LINEAR,
	};

	enum Repeating {
		NoRepeat = GL_NONE,
		Repeat = GL_REPLACE,
		MirroredRepeat = GL_MIRRORED_REPEAT,
		ClampToEdge = GL_CLAMP_TO_EDGE,
		ClampToBorder = GL_CLAMP_TO_BORDER,
	};

	enum MipMaps {
		NoMipMaps = GL_NONE,
		NearestMipMapNearest = GL_NEAREST_MIPMAP_NEAREST,
		LinearMipMapNearest = GL_LINEAR_MIPMAP_NEAREST,
		NearestMipMapLinear = GL_NEAREST_MIPMAP_LINEAR,
		LinearMipMapLinear = GL_LINEAR_MIPMAP_LINEAR,
	};

	enum TextureType {
		Texture1D = GL_TEXTURE_1D,
		Texture1DArray = GL_TEXTURE_1D_ARRAY,
		Texture2D = GL_TEXTURE_2D,
		Texture2DArray = GL_TEXTURE_2D_ARRAY,
		Texture3D = GL_TEXTURE_3D,
		Texture3DArray = GL_TEXTURE_2D_ARRAY,
	};

	struct TextureCreateInfo {
		std::string texturePath;

		Filter filterMin, filterMag;
		Repeating repeatS, repeatT;
		MipMaps mipmapsMin, mipmapsMag;
		TextureType textureType;
		glm::vec4 borderCol = glm::vec4(0, 0, 0, 0); // If you are using ClampToBorder, set this.
		int depth; // If you are not using a 3d texture don't worry about this.
	};

	class Texture {
	private:
		GLenum textureType;
		unsigned int textureID = -1;
	public:
		Texture() = default;
		Texture(TextureCreateInfo);

		virtual ~Texture();

		void Bind();
		void Unbind();
	};
}
