#pragma once

//#include "GL.h"
#include "globalOpenGL.h"

#include <map>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/vec3.hpp> 
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp> 
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>

class Model {

public:
	Model();
	Model(const char* filepath);
	Model(const char* filepath, bool withBones);
	Model(const std::vector<glm::vec3>& Vertices, const std::vector<int>& triangles);
	~Model();

	GLuint Vbo;
	GLuint VertexBufferId;
	GLuint IndexBufferId;

	std::vector<glm::vec3> Vertices;
	std::vector<int> triangles;	// 1,2,3

	unsigned int NumTriangles;
};