#pragma once
#include "glad/glad.h"
#include <iostream>

#ifdef _DEBUG
#define GLCheckError()                                                                                \
{ GLenum error;                                                                                             \
while ((error = glGetError()) != GL_NO_ERROR) {                                                           \
	std::cout << "GL Error: " << error << ". Line: " << __LINE__ << ", File: " << __FILE__ << std::endl;  \
} }
#else
#define GLCheckError()
#endif
