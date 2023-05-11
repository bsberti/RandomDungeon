#include "../globalOpenGL.h"
#define MAX_BONE_WEIGHTS 4

#include "cVAOManager.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <vector>
#include <sstream>
#include <iostream>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "../Animation.h"
#include "../AssimpGLMHelpers.h"
#include <assimp/Importer.hpp>

sModelDrawInfo::sModelDrawInfo()
{
	this->VAO_ID = 0;

	this->VertexBufferID = 0;
	this->VertexBuffer_Start_Index = 0;
	this->numberOfVertices = 0;

	this->IndexBufferID = 0;
	this->IndexBuffer_Start_Index = 0;
	this->numberOfIndices = 0;
	this->numberOfTriangles = 0;

	// The "local" (i.e. "CPU side" temporary array)
	this->pVertices = NULL;		// or 0 or nullptr
	this->pIndices = NULL;		// or 0 or nullptr

	this->minX = this->minY = this->minZ = 0.0f;
	this->maxX = this->maxY = this->maxZ = 0.0f;
	this->extentX = this->extentY = this->extentZ = 0.0f;
	this->maxExtent = 0.0f;

	return;
}

void sModelDrawInfo::CalculateExtents(void)
{
	// Do we even have an array?
	if ( this->pVertices )		// same as != NULL
	{
		// Assume that the 1st vertex is both the min and max
		this->minX = this->maxX = this->pVertices[0].x;
		this->minY = this->maxY = this->pVertices[0].y;
		this->minZ = this->maxZ = this->pVertices[0].z;

		for (unsigned int index = 0; index != this->numberOfVertices; index++)
		{
			// See if "this" vertex is smaller than the min
			if (this->pVertices[index].x < this->minX) { this->minX = this->pVertices[index].x;	}
			if (this->pVertices[index].y < this->minY) { this->minY = this->pVertices[index].y; }
			if (this->pVertices[index].z < this->minZ) { this->minZ = this->pVertices[index].z; }

			// See if "this" vertex is larger than the max
			if (this->pVertices[index].x > this->maxX) { this->maxX = this->pVertices[index].x;	}
			if (this->pVertices[index].y > this->maxY) { this->maxY = this->pVertices[index].y; }
			if (this->pVertices[index].z > this->maxZ) { this->maxZ = this->pVertices[index].z; }
		}//for (unsigned int index = 0...
	}//if ( this->pVertices )

	// Update the extents
	this->extentX = this->maxX - this->minX;
	this->extentY = this->maxY - this->minY;
	this->extentZ = this->maxZ - this->minZ;

	// What's the largest of the three extents
	this->maxExtent = this->extentX;
	if (this->extentY > this->maxExtent) { this->maxExtent = this->extentY; }
	if (this->extentZ > this->maxExtent) { this->maxExtent = this->extentZ; }

	return;
}

void cVAOManager::loadModel(std::string path)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}
	//directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene, path);
}

void cVAOManager::processNode(aiNode* node, const aiScene* scene, const std::string& path)
{
	// process all the node's meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {

		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		sModelDrawInfo* drawInfo = processMesh(mesh, scene);

		if (shaderID != -1)
		{
			LoadModelIntoVAO(path, *drawInfo, shaderID);
			return;
		}
	}
	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene, path);
	}
}

void SetVertexBoneDataToDefault(sVertex_RGBA_XYZ_N_UV_T_BiN_Bones& vertex)
{
	for (int i = 0; i < MAX_BONE_WEIGHTS; i++)
	{
		vertex.vBoneID[i] = -1;
		vertex.vBoneWeight[i] = 0.0f;
	}
}

void SetVertexBoneData(sVertex_RGBA_XYZ_N_UV_T_BiN_Bones& vertex, int boneID, float weight)
{
	for (int i = 0; i < MAX_BONE_WEIGHTS; ++i)
	{
		if (vertex.vBoneID[i] < 0)
		{
			vertex.vBoneWeight[i] = weight;
			vertex.vBoneID[i] = boneID;
			break;
		}
	}
}

void ExtractBoneWeightForVertices(sModelDrawInfo* drawInfo, aiMesh* mesh, const aiScene* scene)
{
	for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
	{
		int boneID = -1;
		std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
		if (drawInfo->m_BoneInfoMap.find(boneName) == drawInfo->m_BoneInfoMap.end())
		{
			CharacterBoneInfo newBoneInfo;
			newBoneInfo.id = drawInfo->m_BoneCounter;
			newBoneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(
				mesh->mBones[boneIndex]->mOffsetMatrix);
			drawInfo->m_BoneInfoMap[boneName] = newBoneInfo;
			boneID = drawInfo->m_BoneCounter;
			drawInfo->m_BoneCounter++;
		}
		else
		{
			boneID = drawInfo->m_BoneInfoMap[boneName].id;
		}
		assert(boneID != -1);
		auto weights = mesh->mBones[boneIndex]->mWeights;
		int numWeights = mesh->mBones[boneIndex]->mNumWeights;

		for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
		{
			int vertexId = weights[weightIndex].mVertexId;
			float weight = weights[weightIndex].mWeight;
			assert(vertexId <= drawInfo->numberOfVertices);
			SetVertexBoneData(drawInfo->pVertices[vertexId], boneID, weight);
		}
	}
}

sModelDrawInfo* cVAOManager::processMesh(aiMesh* mesh, const aiScene* scene)
{
	sModelDrawInfo* drawInfo = new sModelDrawInfo();
	drawInfo->numberOfIndices = mesh->mNumFaces * 3;
	drawInfo->numberOfVertices = mesh->mNumVertices;
	drawInfo->numberOfTriangles = mesh->mNumFaces;
	drawInfo->pVertices = new sVertex_RGBA_XYZ_N_UV_T_BiN_Bones[mesh->mNumVertices];

	
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		sVertex_RGBA_XYZ_N_UV_T_BiN_Bones vertex;
		SetVertexBoneDataToDefault(vertex);
		// process vertex positions, normals and texture coordinates
		vertex.x = mesh->mVertices[i].x;
		vertex.y = mesh->mVertices[i].y;
		vertex.z = mesh->mVertices[i].z;
		vertex.w = 1.f;

		if (mesh->mColors[0]) {
			vertex.r = mesh->mColors[0][i].r;
			vertex.g = mesh->mColors[0][i].g;
			vertex.b = mesh->mColors[0][i].b;
		}

		vertex.nx = mesh->mNormals[i].x;
		vertex.ny = mesh->mNormals[i].y;
		vertex.nz = mesh->mNormals[i].z;
		vertex.nw = 1.f;

		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			vertex.u0 = mesh->mTextureCoords[0][i].x;
			vertex.v0 = mesh->mTextureCoords[0][i].y;
		}
		else
		{
			vertex.u0 = 0.f;
			vertex.v0 = 0.f;
		}

		drawInfo->pVertices[i] = vertex;
	}
	
	drawInfo->pIndices = new unsigned int[mesh->mNumFaces * 3]; 
	unsigned int indicesCount = 0;
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			drawInfo->pIndices[indicesCount] = face.mIndices[j];
			indicesCount++;
		}
	}

	ExtractBoneWeightForVertices(drawInfo, mesh, scene);

	return drawInfo;
}

bool cVAOManager::LoadModelIntoVAO(
		std::string fileName, 
		sModelDrawInfo &drawInfo,
	    unsigned int shaderProgramID)

{
	// Load the model from file
	// (We do this here, since if we can't load it, there's 
	//	no point in doing anything else, right?)
	drawInfo.meshName = fileName;

	// Calculate the extents of this model
	drawInfo.CalculateExtents();

	// TODO: Load the model from file

	// 
	// Model is loaded and the vertices and indices are in the drawInfo struct
	// 

	// Create a VAO (Vertex Array Object), which will 
	//	keep track of all the 'state' needed to draw 
	//	from this buffer...

	// Ask OpenGL for a new buffer ID...
	glGenVertexArrays( 1, &(drawInfo.VAO_ID) );
	// "Bind" this buffer:
	// - aka "make this the 'current' VAO buffer
	glBindVertexArray(drawInfo.VAO_ID);

	// Now ANY state that is related to vertex or index buffer
	//	and vertex attribute layout, is stored in the 'state' 
	//	of the VAO... 


	// NOTE: OpenGL error checks have been omitted for brevity
//	glGenBuffers(1, &vertex_buffer);
	glGenBuffers(1, &(drawInfo.VertexBufferID) );

//	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, drawInfo.VertexBufferID);
	// sVert vertices[3]
	glBufferData( GL_ARRAY_BUFFER, 
				  sizeof(sVertex_RGBA_XYZ_N_UV_T_BiN_Bones) * drawInfo.numberOfVertices,	// ::g_NumberOfVertsToDraw,	// sizeof(vertices), 
				  (GLvoid*) drawInfo.pVertices,							// pVertices,			//vertices, 
				  GL_STATIC_DRAW );


	// Copy the index buffer into the video card, too
	// Create an index buffer.
	glGenBuffers( 1, &(drawInfo.IndexBufferID) );

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawInfo.IndexBufferID);

	glBufferData( GL_ELEMENT_ARRAY_BUFFER,			// Type: Index element array
	              sizeof( unsigned int ) * drawInfo.numberOfIndices, 
	              (GLvoid*) drawInfo.pIndices,
                  GL_STATIC_DRAW );


//   __     __        _              _                            _   
//   \ \   / /__ _ __| |_ _____  __ | |    __ _ _   _  ___  _   _| |_ 
//    \ \ / / _ \ '__| __/ _ \ \/ / | |   / _` | | | |/ _ \| | | | __|
//     \ V /  __/ |  | ||  __/>  <  | |__| (_| | |_| | (_) | |_| | |_ 
//      \_/ \___|_|   \__\___/_/\_\ |_____\__,_|\__, |\___/ \__,_|\__|
//                                              |___/                 

	// in vec4 vColour;
	GLint vColour_location = glGetAttribLocation(shaderProgramID, "vColour");	
	glEnableVertexAttribArray(vColour_location);	
	glVertexAttribPointer(vColour_location, 
						  4, GL_FLOAT, 
						  GL_FALSE,
						  sizeof(sVertex_RGBA_XYZ_N_UV_T_BiN_Bones),						// Stride	(number of bytes)
						  ( void* ) offsetof(sVertex_RGBA_XYZ_N_UV_T_BiN_Bones, r) );		// Offset the member variable

	//in vec4 vPosition;			
	GLint vPosition_location = glGetAttribLocation(shaderProgramID, "vPosition");
	glEnableVertexAttribArray(vPosition_location);
	glVertexAttribPointer(vPosition_location,
						  4, GL_FLOAT, 
						  GL_FALSE,
						  sizeof(sVertex_RGBA_XYZ_N_UV_T_BiN_Bones),						// Stride	(number of bytes)
						  ( void* ) offsetof(sVertex_RGBA_XYZ_N_UV_T_BiN_Bones, x) );		// Offset the member variable

	//in vec4 vNormal;			
	GLint vNormal_location = glGetAttribLocation(shaderProgramID, "vNormal");
	glEnableVertexAttribArray(vNormal_location);
	glVertexAttribPointer(vNormal_location,
						  4, GL_FLOAT, 
						  GL_FALSE,
						  sizeof(sVertex_RGBA_XYZ_N_UV_T_BiN_Bones),						// Stride	(number of bytes)
						  ( void* ) offsetof(sVertex_RGBA_XYZ_N_UV_T_BiN_Bones, nx) );		// Offset the member variable

	//in vec4 vUVx2;			
	GLint vUVx2_location = glGetAttribLocation(shaderProgramID, "vUVx2");
	glEnableVertexAttribArray(vUVx2_location);
	glVertexAttribPointer(vUVx2_location,
						  4, GL_FLOAT, 
						  GL_FALSE,
						  sizeof(sVertex_RGBA_XYZ_N_UV_T_BiN_Bones),						// Stride	(number of bytes)
						  ( void* ) offsetof(sVertex_RGBA_XYZ_N_UV_T_BiN_Bones, u0) );		// Offset the member variable

	//in vec4 vTangent;			
	GLint vTangent_location = glGetAttribLocation(shaderProgramID, "vTangent");
	glEnableVertexAttribArray(vTangent_location);
	glVertexAttribPointer(vTangent_location,
						  4, GL_FLOAT, 
						  GL_FALSE,
						  sizeof(sVertex_RGBA_XYZ_N_UV_T_BiN_Bones),						// Stride	(number of bytes)
						  ( void* ) offsetof(sVertex_RGBA_XYZ_N_UV_T_BiN_Bones, tx) );		// Offset the member variable

	//in vec4 vBiNormal;		
	GLint vBiNormal_location = glGetAttribLocation(shaderProgramID, "vBiNormal");
	glEnableVertexAttribArray(vBiNormal_location);
	glVertexAttribPointer(vBiNormal_location,
						  4, GL_FLOAT, 
						  GL_FALSE,
						  sizeof(sVertex_RGBA_XYZ_N_UV_T_BiN_Bones),						// Stride	(number of bytes)
						  ( void* ) offsetof(sVertex_RGBA_XYZ_N_UV_T_BiN_Bones, bx) );		// Offset the member variable

	//in vec4 vBoneID;			
	GLint vBoneID_location = glGetAttribLocation(shaderProgramID, "vBoneID");
	glEnableVertexAttribArray(vBoneID_location);
	glVertexAttribPointer(vBoneID_location,
						  4, GL_FLOAT, 
						  GL_FALSE,
						  sizeof(sVertex_RGBA_XYZ_N_UV_T_BiN_Bones),						// Stride	(number of bytes)
						  ( void* ) offsetof(sVertex_RGBA_XYZ_N_UV_T_BiN_Bones, vBoneID[0]) );		// Offset the member variable

	//in vec4 vBoneWeight;		
	GLint vBoneWeight_location = glGetAttribLocation(shaderProgramID, "vBoneWeight");
	glEnableVertexAttribArray(vBoneWeight_location);
	glVertexAttribPointer(vBoneWeight_location,
						  4, GL_FLOAT, 
						  GL_FALSE,
						  sizeof(sVertex_RGBA_XYZ_N_UV_T_BiN_Bones),						// Stride	(number of bytes)
						  ( void* ) offsetof(sVertex_RGBA_XYZ_N_UV_T_BiN_Bones, vBoneWeight[0]) );		// Offset the member variable

	// Now that all the parts are set up, set the VAO to zero
	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDisableVertexAttribArray(vColour_location);
	glDisableVertexAttribArray(vPosition_location);
	glDisableVertexAttribArray(vNormal_location);
	glDisableVertexAttribArray(vUVx2_location);
	glDisableVertexAttribArray(vTangent_location);
	glDisableVertexAttribArray(vBiNormal_location);
	glDisableVertexAttribArray(vBoneID_location);
	glDisableVertexAttribArray(vBoneWeight_location);

	// Store the draw information into the map
	sModelDrawInfo existingDrawInfo;
	int meshCount = 1;
	std::string newMeshName = drawInfo.meshName + std::to_string(meshCount);
	if (FindDrawInfoByModelName(drawInfo.meshName, existingDrawInfo)) {
		while (FindDrawInfoByModelName(newMeshName, existingDrawInfo)) {
			meshCount++;
			newMeshName = drawInfo.meshName + std::to_string(meshCount);
		}
		this->m_map_ModelName_to_VAOID[newMeshName] = drawInfo;
	}
	else {
		this->m_map_ModelName_to_VAOID[drawInfo.meshName] = drawInfo;
	}

	return true;
}

void cVAOManager::AddDrawInfoToMap(std::string meshName, sModelDrawInfo drawInfo) {
	this->m_map_ModelName_to_VAOID[meshName] = drawInfo;
}

// We don't want to return an int, likely
bool cVAOManager::FindDrawInfoByModelName(
		std::string filename,
		sModelDrawInfo &drawInfo) 
{
	std::map< std::string /*model name*/,
			sModelDrawInfo /* info needed to draw*/ >::iterator 
		itDrawInfo = this->m_map_ModelName_to_VAOID.find( filename );

	// Find it? 
	if ( itDrawInfo == this->m_map_ModelName_to_VAOID.end() )
	{
		// Nope
		return false;
	}

	// Else we found the thing to draw
	// ...so 'return' that information
	drawInfo = itDrawInfo->second;
	return true;
}

