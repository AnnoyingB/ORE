#pragma once
#include <fstream>
#include <sstream>
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glad/glad.h"

namespace ORE {
	class Shader
	{
	private:
		unsigned int shaderID;
	public:
		/** 
		 * @param
		 * shaderPath - Should not have an extension. Keep the
		 * file name for the vertex and fragment shader the same.
		 */
		Shader(std::string_view shaderPath);
		virtual ~Shader();

		void Bind();

		void SetBool(const std::string& name, bool value) const;
		void SetInt(const std::string& name, int value) const;
		void SetFloat(const std::string& name, float value) const;
		void SetMat4(const std::string& name, glm::mat4 value) const;
	};
}
