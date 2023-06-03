#pragma once
#include <fstream>
#include <sstream>
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <filesystem>
#include <unordered_map>
#include "Debugging.h"

#include "Texture.h"

namespace ORE {
	class Shader
	{
	private:
		std::unordered_map<std::string, GLint> shaderLocations;

		unsigned int shaderID;
	public:
		static const std::string OREShaders;
		static const std::string PBRShader;
		static const std::string CubeMapShader;
		static const std::string SkyBoxShader;
		static const std::string BillboardShader;
		static const std::string TextShader;

		// please do not actually use this.
		Shader() = default;
		/** 
		 * @param
		 * shaderPath - Should not have an extension. Keep the
		 * file name for the vertex and fragment shader the same.
		 */
		Shader(std::string shaderPath);
		virtual ~Shader();

		void Bind();

		void SetBool(const std::string& name, bool value);
		void SetInt(const std::string& name, int value);
		void SetFloat(const std::string& name, float value);
		void SetMat4(const std::string& name, glm::mat4 value);
		void SetMat3(const std::string& name, glm::mat3 value);
		void SetVec3(const std::string& name, glm::vec3 value);
		void SetTexture(const std::string& name, const Texture& texture, int texID);
	};
}
