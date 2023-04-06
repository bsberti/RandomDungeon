#pragma once

#include "cMeshObject.h"
#include <vector>
#include "Model.h"
#include "physics.h"
#include "Model.h"

struct DebugRendererObject
{
    unsigned int MaterialId;
    unsigned int MeshId;
};

// Assimp Loading
void GDP_LoadFBXFile(unsigned int& id, std::string& animationName, const std::string& filename);
cMeshObject* GDP_CreateAnimatedCharacter(
    const char* filename, const std::vector<std::string>& animations);

// Animation
void GDP_LoadAnimation(const char* name, AnimationData animation);

void GDP_CreateModel(unsigned int& id, const std::vector<glm::vec3>& Vertices, const std::vector<int>& triangles);
void GDP_GetModelData(unsigned int id, std::vector<glm::vec3>& Vertices, std::vector<int>& triangles,
    unsigned int& numTriangles, unsigned int& vbo);

Model* GDP_GetModel(unsigned int id);
void GDP_CreateMaterial(unsigned int& id, unsigned int textureId, const glm::vec3& color);
void GDP_CreateShaderProgramFromFiles(unsigned int& id, unsigned int& shaderProgramId,
    const char* vertexShader, const char* fragmentShader);

// Game World
cMeshObject* GDP_CreateGameObject();