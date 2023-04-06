#ifndef CheckGLError_HG
#define CheckGLError_HG

void _CheckGLError(const char* file, int line);

#define CheckGLError() _CheckGLError(__FILE__, __LINE__)

#endif#pragma once
