#pragma once
#include <GL/glew.h>
#include <string>
void __glCheckError__(unsigned int error, int line, const std::string& file, const std::string& expression);
#define glCheckError(expression) __glCheckError__(glGetError(), __LINE__, __FILE__, expression)
#define glCall(x) x; glCheckError(#x)