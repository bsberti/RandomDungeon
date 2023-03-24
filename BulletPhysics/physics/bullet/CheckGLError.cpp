#include "CheckGLError.h"

#include <GL/glew.h>

#include <string>
#include <sstream>
#include <iostream>

void _CheckGLError(const char* file, int line)
{
    GLenum err(glGetError());

    while (err != GL_NO_ERROR)
    {
        std::string error;
        switch (err)
        {
        case GL_INVALID_OPERATION:				error = "INVALID_OPERATION";				break;
        case GL_INVALID_ENUM:					error = "INVALID_ENUM";					    break;
        case GL_INVALID_VALUE:					error = "INVALID_VALUE";					break;
        case GL_OUT_OF_MEMORY:					error = "OUT_OF_MEMORY";					break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:  error = "INVALID_FRAMEBUFFER_OPERATION";    break;
        }
        std::stringstream ss;
        ss << "[" << file << ":" << line << "] " << error;

        printf("OpenGL Error: %s\n", ss.str().c_str());

        err = glGetError();
    }

    return;
}