#include "orepch.h"
#include "Shader.h"

namespace ORE {
	inline const std::string Shader::OREShaders = std::string("include\\ORE\\Shaders\\");
	inline const std::string Shader::PBRShader = Shader::OREShaders + "PBRShader";
	inline const std::string Shader::CubeMapShader = Shader::OREShaders + "CubeMap";
	inline const std::string Shader::SkyBoxShader = Shader::OREShaders + "SkyBoxShader";
	inline const std::string Shader::BillboardShader = Shader::OREShaders + "BillboardShader";

	Shader::Shader(std::string shaderPath) {
		shaderID = -1;
		std::string vertexPath = shaderPath + ".vert";
		std::string fragmentPath = shaderPath + ".frag";
		
		std::ifstream vShaderFile, fShaderFile;
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		std::string vertexCode, fragmentCode;

		try {
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);

			std::stringstream vShaderStream, fShaderStream;
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();

			vShaderFile.close();
			fShaderFile.close();

			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch (std::ifstream::failure e) {
			std::cout << "ifstream error: \n" << e.what() << std::endl;
			return;
		}

		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();

		unsigned int vertexID, fragmentID;

		vertexID = glCreateShader(GL_VERTEX_SHADER);
		fragmentID = glCreateShader(GL_FRAGMENT_SHADER);

		glShaderSource(vertexID, 1, &vShaderCode, NULL);
		glCompileShader(vertexID);

		glShaderSource(fragmentID, 1, &fShaderCode, NULL);
		glCompileShader(fragmentID);

		int success;
		char infoLog[512];

		glGetShaderiv(vertexID, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertexID, 512, NULL, infoLog);
			std::cout << "VShader compilation failed:\n" << infoLog << std::endl;
			return;
		}

		glGetShaderiv(fragmentID, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragmentID, 512, NULL, infoLog);
			std::cout << "FShader compilation failed:\n" << infoLog << std::endl;
			return;
		}

		shaderID = glCreateProgram();
		glAttachShader(shaderID, vertexID);
		glAttachShader(shaderID, fragmentID);
		glLinkProgram(shaderID);

		glGetProgramiv(shaderID, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shaderID, 512, NULL, infoLog);
			std::cout << "Shader linking failed:\n" << infoLog << std::endl;
			return;
		}

		glDeleteShader(vertexID);
		glDeleteShader(fragmentID);

		assert(shaderID != -1);
		GLCheckError();
	}

	Shader::~Shader() {
		glDeleteProgram(shaderID);
	}

	void Shader::Bind() {
		glUseProgram(shaderID);
	}

	void Shader::SetBool(const std::string& name, bool value) {
		GLint location;
		if (shaderLocations.find(name) == shaderLocations.end()) {
			location = glGetUniformLocation(shaderID, name.c_str());
			shaderLocations[name] = location;
		}
		else {
			location = shaderLocations.find(name)->second;
		}
		glProgramUniform1i(shaderID, location, (int)value);
	}

	void Shader::SetInt(const std::string& name, int value) {
		GLint location;
		if (shaderLocations.find(name) == shaderLocations.end()) {
			location = glGetUniformLocation(shaderID, name.c_str());
			shaderLocations[name] = location;
		}
		else {
			location = shaderLocations.find(name)->second;
		}
		glProgramUniform1i(shaderID, location, value);
	}

	void Shader::SetFloat(const std::string& name, float value) {
		GLint location;
		if (shaderLocations.find(name) == shaderLocations.end()) {
			location = glGetUniformLocation(shaderID, name.c_str());
			shaderLocations[name] = location;
		}
		else {
			location = shaderLocations.find(name)->second;
		}
		glProgramUniform1f(shaderID, location, value);
	}

	void Shader::SetMat4(const std::string& name, glm::mat4 value) {
		GLint location;
		if (shaderLocations.find(name) == shaderLocations.end()) {
			location = glGetUniformLocation(shaderID, name.c_str());
			shaderLocations[name] = location;
		}
		else {
			location = shaderLocations.find(name)->second;
		}
		glProgramUniformMatrix4fv(shaderID, location, 1, GL_FALSE, &value[0][0]);
	}

	void Shader::SetMat3(const std::string& name, glm::mat3 value) {
		GLint location;
		if (shaderLocations.find(name) == shaderLocations.end()) {
			location = glGetUniformLocation(shaderID, name.c_str());
			shaderLocations[name] = location;
		}
		else {
			location = shaderLocations.find(name)->second;
		}
		glProgramUniformMatrix3fv(shaderID, location, 1, GL_FALSE, &value[0][0]);
	}

	void Shader::SetVec3(const std::string& name, glm::vec3 value) {
		GLint location;
		if (shaderLocations.find(name) == shaderLocations.end()) {
			location = glGetUniformLocation(shaderID, name.c_str());
			shaderLocations[name] = location;
		}
		else {
			location = shaderLocations.find(name)->second;
		}
		glProgramUniform3fv(shaderID, location, 1, &value[0]);
	}

	void Shader::SetTexture(const std::string& name, const Texture& value, int texID) {
		GLint location;
		if (shaderLocations.find(name) == shaderLocations.end()) {
			location = glGetUniformLocation(shaderID, name.c_str());
			shaderLocations[name] = location;
		}
		else {
			location = shaderLocations.find(name)->second;
		}
		glActiveTexture(GL_TEXTURE0 + texID);
		value.Bind();
		glProgramUniform1i(shaderID, location, texID);
		value.Unbind();
	}
}
