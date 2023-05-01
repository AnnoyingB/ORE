#include "Texture.h"
#include "orepch.h"

namespace ORE {
	Texture::Texture(TextureCreateInfo createInfo) {
		if (textureID != -1) glDeleteTextures(1, &textureID);

		glGenTextures(1, &textureID);
		glBindTexture(createInfo.textureType, textureID);

		if (createInfo.repeatS != NoRepeat)
			glTexParameteri(createInfo.textureType, GL_TEXTURE_WRAP_S, createInfo.repeatS);
		if (createInfo.repeatT != NoRepeat)
			glTexParameteri(createInfo.textureType, GL_TEXTURE_WRAP_T, createInfo.repeatT);
		if (createInfo.filterMin != NoFilter)
			glTexParameteri(createInfo.textureType, GL_TEXTURE_MIN_FILTER, createInfo.filterMin);
		if (createInfo.filterMag != NoFilter)
			glTexParameteri(createInfo.textureType, GL_TEXTURE_MAG_FILTER, createInfo.filterMag);
		if (createInfo.mipmapsMin != NoMipMaps)
			glTexParameteri(createInfo.textureType, GL_TEXTURE_MIN_FILTER, createInfo.mipmapsMin);
		if (createInfo.mipmapsMag != NoMipMaps)
			glTexParameteri(createInfo.textureType, GL_TEXTURE_MAG_FILTER, createInfo.mipmapsMag);

		if (createInfo.repeatS == Repeating::ClampToBorder || createInfo.repeatT == Repeating::ClampToBorder)
			glTexParameterfv(createInfo.textureType, GL_TEXTURE_BORDER_COLOR, &createInfo.borderCol[0]);

		int width, height;
		int channels;
		unsigned char* imageData = stbi_load(createInfo.texturePath.c_str(), &width, &height, &channels, 4);

		assert(imageData);

		if (createInfo.textureType == Texture1D || createInfo.textureType == Texture1DArray)
			glTexImage1D(createInfo.textureType, 0, GL_RGBA, width, height, 0, GL_UNSIGNED_BYTE, imageData);
		else if (createInfo.textureType == Texture2D || createInfo.textureType == Texture2DArray)
			glTexImage2D(createInfo.textureType, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
		else if (createInfo.textureType == Texture3D || createInfo.textureType == Texture2DArray)
			glTexImage3D(createInfo.textureType, 0, GL_RGBA, width, height,	createInfo.depth, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);

		if (createInfo.mipmapsMin != NoMipMaps || createInfo.mipmapsMag != NoMipMaps)
			glGenerateMipmap(createInfo.textureType);

		glBindTexture(createInfo.textureType, 0);

		stbi_image_free(imageData);

		textureType = createInfo.textureType;
	}

	Texture::~Texture() {
		glDeleteTextures(1, &textureID);
	}

	void Texture::Bind() {
		glBindTexture(textureType, textureID);
	}

	void Texture::Unbind() {
		glBindTexture(textureType, 0);
	}
}
