#include "internal.h"
#include <GL/glew.h>
#include <iostream>
#include "glcall.h"
unsigned int create_shader_from_source(const std::string& source, unsigned int type) {
	const char* src = source.c_str();
	unsigned int shader = glCall(glCreateShader(type));
	glShaderSource(shader, 1, &src, NULL);
	glCompileShader(shader);
	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char info_log[512];
		glGetShaderInfoLog(shader, 512, NULL, info_log);
		std::cout << "error compiling " << src << ": " << info_log << std::endl;
	}
	return shader;
}
