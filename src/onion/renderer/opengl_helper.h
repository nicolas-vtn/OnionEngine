#pragma once

#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include <glad/glad.h>

#include <GLFW/glfw3.h>

namespace Onion::Debug {

#ifdef NDEBUG
#define GL_CHECK(stmt) stmt
#define GL_ASSERT(stmt) stmt
#else
#define GL_CHECK(stmt)                                                                                                                     \
	do {                                                                                                                                   \
		stmt;                                                                                                                              \
		CheckGLError(#stmt, __FILE__, __LINE__);                                                                                           \
	} while (0)

#define GL_ASSERT(stmt)                                                                                                                    \
	do {                                                                                                                                   \
		stmt;                                                                                                                              \
		AssertGLError(#stmt, __FILE__, __LINE__);                                                                                          \
	} while (0)
#endif

	static inline const std::unordered_map<GLenum, std::string> GL_ERROR_STRINGS = {
		{GL_NO_ERROR, "No error has been recorded"},
		{GL_INVALID_ENUM, "Invalid enum: unacceptable value for an enumerated argument"},
		{GL_INVALID_VALUE, "Invalid value: numeric argument out of range"},
		{GL_INVALID_OPERATION, "Invalid operation: not allowed in the current state"},
		{GL_INVALID_FRAMEBUFFER_OPERATION, "Invalid framebuffer operation: framebuffer is incomplete"},
		{GL_OUT_OF_MEMORY, "Out of memory: insufficient memory to execute command"},
	#ifdef GL_STACK_UNDERFLOW
		{GL_STACK_UNDERFLOW, "Stack underflow: operation caused stack underflow"},
	#endif
	#ifdef GL_STACK_OVERFLOW
		{GL_STACK_OVERFLOW, "Stack overflow: operation caused stack overflow"},
	#endif
	};

	inline const char* GetGLErrorString(GLenum err) {
		auto it = GL_ERROR_STRINGS.find(err);
		return (it != GL_ERROR_STRINGS.end()) ? it->second.c_str() : "Unknown OpenGL error";
	}

	// Actual check function
	inline void CheckGLError(const char* stmt, const char* fname, int line) {
		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR) {
			std::fprintf(stderr, "OpenGL error %d (%s), at %s:%d - for call '%s'\n", err, GetGLErrorString(err), fname, line, stmt);
		}
	}

	// Throws on errors
	inline void AssertGLError(const char* stmt, const char* fname, int line) {
		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR) {
			char buffer[512];
			std::snprintf(buffer, sizeof(buffer), "OpenGL error %d (%s), at %s:%d - for call '%s'", err, GetGLErrorString(err), fname, line,
				stmt);
			throw std::runtime_error(buffer);
		}
	}
} // namespace Onion::Rendering
