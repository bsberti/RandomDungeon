#pragma once
#include <string>

// The vertex structure 
//	that will be passed to the GPU (Shader)
struct sVertex {
	float r, g, b, a;		//	in vec4 vColour;			// Was vec3
	float x, y, z, w;		//	in vec4 vPosition;			// Was vec3
	float nx, ny, nz, nw;	//	in vec4 vNormal;			// Vertex normal
	float u0, v0, u1, v1;	//	in vec4 vUVx2;				// 2 x Texture coords
	float tx, ty, yz, tw;	//	in vec4 vTangent;			// For bump mapping
	float bx, by, bz, bw;	//	in vec4 vBiNormal;			// For bump mapping
	float vBoneID[4];		//	in vec4 vBoneID;			// For skinned mesh (FBX)
	float vBoneWeight[4];	//	in vec4 vBoneWeight;		// For skinned mesh (FBX)
};

class cModel {
public:
	std::string meshName;

	unsigned int VAO_ID;

	// Vertices Data
	unsigned int VertexBufferID;
	unsigned int VertexBuffer_Start_Index;
	unsigned int numberOfVertices;

	// Triangle Indexes Data
	unsigned int IndexBufferID;
	unsigned int IndexBuffer_Start_Index;
	unsigned int numberOfIndices;
	unsigned int numberOfTriangles;

	// Vertices Structure
	sVertex* pVertices;
	unsigned int* pIndices;

	float min_x, max_x;
	float min_y, max_y;
	float min_z, max_z;


	cModel();
	~cModel();
};

