#include "glcall.h"
#include <sstream>
#include <exception>
#ifdef _WIN32
#include <Windows.h>
#else
#include <iostream>
#endif
void __glCheckError__(unsigned int err, int line, const std::string& file, const std::string& expression) {
	std::string error;
	switch (err) {
	case GL_INVALID_ENUM:
		error = "invalid enum";
		break;
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		error = "invalid framebuffer operation";
		break;
	case GL_INVALID_INDEX:
		error = "invalid index";
		break;
	case GL_INVALID_OPERATION:
		error = "invalid operation";
		break;
	case GL_INVALID_VALUE:
		error = "invalid value";
		break;
	default:
		error = "other";
		break;
	}
	std::stringstream output;
	output << "error: " << error << " on line " << line << " in file " << file << " executing expression " << expression << "\n";
	std::string output_str = output.str();
	throw std::exception(output_str.c_str());
	exit(9);
}