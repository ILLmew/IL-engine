#pragma once

#include "IL/Renderer/Shader.h"
#include "glm/glm.hpp"

// TODO: remove 
typedef unsigned int GLenum;

namespace IL
{

	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc);
		OpenGLShader(const std::string& path);
		~OpenGLShader();

		void Bind() const override;
		void Unbind() const override;

		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);

		void UploadUniformInt(const std::string& name, const int& values);

		void UploadUniformFloat(const std::string& name, const float& values);
		void UploadUniformFloat2(const std::string& name, const glm::vec2& values);
		void UploadUniformFloat3(const std::string& name, const glm::vec3& values);
		void UploadUniformFloat4(const std::string& name, const glm::vec4& values);
	private:
		std::string ReadFile(const std::string& filepath);
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
		void Compile(const std::unordered_map<GLenum, std::string>& sourceMap);
	private:
		uint32_t m_Program;
	};

}