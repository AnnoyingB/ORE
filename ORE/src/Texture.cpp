#include "Texture.h"
#include "orepch.h"
#include "Debugging.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace ORE {
	Texture::Texture(TextureCreateInfo createInfo) {
		if (textureID != -1) glDeleteTextures(1, &textureID);

		GLCheckError();

		glGenTextures(1, &textureID);
		glBindTexture(createInfo.textureType, textureID);

		int width, height;
		int channels;
		if (!createInfo.loadWithFloat) {
			stbi_set_flip_vertically_on_load(createInfo.flip);
			unsigned char* imageData = stbi_load(createInfo.texturePath.c_str(), &width, &height, &channels, 4);
			stbi_set_flip_vertically_on_load(false);

			if (!imageData && createInfo.textureType != CubeMap) {
				std::cout << "Failed to load stbi image: " << stbi_failure_reason() << std::endl;
				assert(false);
				return;
			}
			
			if (createInfo.textureType == Texture1D || createInfo.textureType == Texture1DArray)
				glTexImage1D(createInfo.textureType, 0, createInfo.textureFormat, width, height, 0, GL_UNSIGNED_BYTE, imageData);
			else if (createInfo.textureType == Texture2D || createInfo.textureType == Texture2DArray)
				glTexImage2D(createInfo.textureType, 0, createInfo.textureFormat, width, height, 0, createInfo.internalFormat, GL_UNSIGNED_BYTE, imageData);
			else if (createInfo.textureType == Texture3D || createInfo.textureType == Texture2DArray)
				glTexImage3D(createInfo.textureType, 0, createInfo.textureFormat, width, height, createInfo.depth, 0, createInfo.internalFormat, GL_UNSIGNED_BYTE, imageData);
			stbi_image_free(imageData);
		} else {
			stbi_set_flip_vertically_on_load(createInfo.flip);
			float* imageData = stbi_loadf(createInfo.texturePath.c_str(), &width, &height, &channels, 4);
			stbi_set_flip_vertically_on_load(false);

			if (!imageData && createInfo.textureType != CubeMap) {
				std::cout << "Failed to load stbi image: " << stbi_failure_reason() << std::endl;
				assert(false);
				return;
			}

			if (createInfo.textureType == CubeMap) {
				for (unsigned int i = 0; i < 6; ++i)
				{
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA16F,
						512, 512, 0, GL_RGBA, GL_FLOAT, nullptr);
				}
			}
			else if (createInfo.textureType == Texture1D || createInfo.textureType == Texture1DArray)
				glTexImage1D(createInfo.textureType, 0, createInfo.textureFormat, width, height, 0, GL_FLOAT, imageData);
			else if (createInfo.textureType == Texture2D || createInfo.textureType == Texture2DArray)
				glTexImage2D(createInfo.textureType, 0, createInfo.textureFormat, width, height, 0, createInfo.internalFormat, GL_FLOAT, imageData);
			else if (createInfo.textureType == Texture3D || createInfo.textureType == Texture2DArray)
				glTexImage3D(createInfo.textureType, 0, createInfo.textureFormat, width, height, createInfo.depth, 0, createInfo.internalFormat, GL_FLOAT, imageData);
			stbi_image_free(imageData);
		}

		if (createInfo.mipmapsMin != NoMipMaps || createInfo.mipmapsMag != NoMipMaps)
			glGenerateMipmap(createInfo.textureType);

		GLCheckError();

		if (createInfo.repeatS != NoRepeat)
			glTexParameteri(createInfo.textureType, GL_TEXTURE_WRAP_S, createInfo.repeatS);
		GLCheckError();
		if (createInfo.repeatT != NoRepeat)
			glTexParameteri(createInfo.textureType, GL_TEXTURE_WRAP_T, createInfo.repeatT);
		GLCheckError();
		if (createInfo.repeatR != NoRepeat)
			glTexParameteri(createInfo.textureType, GL_TEXTURE_WRAP_R, createInfo.repeatR);
		GLCheckError();
		if (createInfo.filterMin != NoFilter)
			glTexParameteri(createInfo.textureType, GL_TEXTURE_MIN_FILTER, createInfo.filterMin);
		GLCheckError();
		if (createInfo.filterMag != NoFilter)
			glTexParameteri(createInfo.textureType, GL_TEXTURE_MAG_FILTER, createInfo.filterMag);
		GLCheckError();
		if (createInfo.mipmapsMin != NoMipMaps)
			glTexParameteri(createInfo.textureType, GL_TEXTURE_MIN_FILTER, createInfo.mipmapsMin);
		GLCheckError();
		if (createInfo.mipmapsMag != NoMipMaps)
			glTexParameteri(createInfo.textureType, GL_TEXTURE_MAG_FILTER, createInfo.mipmapsMag);
		GLCheckError();

		if (createInfo.repeatS == ClampToBorder || createInfo.repeatT == ClampToBorder || createInfo.repeatR == ClampToBorder)
			glTexParameterfv(createInfo.textureType, GL_TEXTURE_BORDER_COLOR, &createInfo.borderCol[0]);

		GLCheckError();

		glBindTexture(createInfo.textureType, 0);

		textureType = createInfo.textureType;
	}

	Texture::~Texture() {
		glDeleteTextures(1, &textureID);
	}

	void Texture::Bind() const {
		glBindTexture(textureType, textureID);
	}

	void Texture::Unbind() const {
		glBindTexture(textureType, 0);
	}
}
