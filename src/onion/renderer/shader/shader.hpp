#pragma once

#include <glm/glm.hpp>
#include <string>

namespace Onion::Rendering {

	class Shader {
	public:
		unsigned int ID = 0;

		Shader() = default;
		Shader(const char* vertexPath, const char* fragmentPath);
		~Shader();

		// Delete copy constructor and assignment
		Shader(const Shader&) = delete;
		Shader& operator=(const Shader&) = delete;

		// Implement move constructor
		Shader(Shader&& other) noexcept
			: ID(other.ID) {
			other.ID = 0;
			m_HasBeenCompiled = other.m_HasBeenCompiled;
		}
		// Implement move assignment
		Shader& operator=(Shader&& other) noexcept;

		void Compile(const char* vertexPath, const char* fragmentPath);
		bool HasBeenCompiled() const {
			return m_HasBeenCompiled;
		}

		void Use() const;

		void setBool(const std::string& name, bool value) const;
		void setInt(const std::string& name, int value) const;
		void setFloat(const std::string& name, float value) const;
		void setVec2(const std::string& name, const glm::vec2& value) const;
		void setVec2(const std::string& name, float x, float y) const;
		void setVec3(const std::string& name, const glm::vec3& value) const;
		void setVec3(const std::string& name, float x, float y, float z) const;
		void setMat2(const std::string& name, const glm::mat2& mat) const;
		void setMat3(const std::string& name, const glm::mat3& mat) const;
		void setMat4(const std::string& name, const glm::mat4& mat) const;

	private:
		bool m_HasBeenCompiled = false;
	};
} // namespace Renderer_cpp
