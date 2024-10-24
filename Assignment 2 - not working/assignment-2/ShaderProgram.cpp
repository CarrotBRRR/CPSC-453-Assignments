#include "ShaderProgram.h"

#include <iostream>
#include <stdexcept>
#include <vector>

#include "Log.h"


ShaderProgram::ShaderProgram(const std::string& vertexPath, const std::string& fragmentPath)
	: vertex(vertexPath, GL_VERTEX_SHADER)
	, fragment(fragmentPath, GL_FRAGMENT_SHADER)
{
	program_id = glCreateProgram();

	attach(*this, vertex);
	attach(*this, fragment);
	glLinkProgram(program_id);

	vertex.deleteShader();
	fragment.deleteShader();

	if (!checkAndLogLinkSuccess()) {
		glDeleteProgram(program_id);
		throw std::runtime_error("Shaders did not link.");
	}
}

bool ShaderProgram::recompile() {
	try {
		ShaderProgram newProgram("shaders/test.vert", "shaders/test.frag");
		*this = std::move(newProgram);
		return true;
	}
	catch (std::runtime_error &e) {
		Log::warn("SHADER_PROGRAM falling back to previous version of shaders");
		return false;
	}
}


void attach(ShaderProgram& sp, Shader& s) {
	glAttachShader(sp.program_id, s.shader_id);
}


bool ShaderProgram::checkAndLogLinkSuccess() const {

	GLint success;

	glGetProgramiv(program_id, GL_LINK_STATUS, &success);
	if (!success) {
		GLint logLength;
		glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &logLength);
		std::vector<char> log(logLength);
		glGetProgramInfoLog(program_id, logLength, NULL, log.data());

		Log::error("SHADER_PROGRAM linking {} + {}:\n{}", vertex.getPath(), fragment.getPath(), log.data());
		return false;
	}
	else {
		Log::info("SHADER_PROGRAM successfully compiled and linked {} + {}", vertex.getPath(), fragment.getPath());
		return true;
	}
}
