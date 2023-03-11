#include "cModel.h"

#ifdef _DEBUG
#define DEBUG_LOG_ENABLED
#endif
#ifdef DEBUG_LOG_ENABLED
#define DEBUG_PRINT(x, ...) printf(x, __VA_ARGS__)
#else
#define DEBUG_PRINT(x)
#endif

cModel::cModel() {
	DEBUG_PRINT("cModel::cModel()\n");
	this->VAO_ID = 0;

	this->VertexBufferID = 0;
	this->VertexBuffer_Start_Index = 0;
	this->numberOfVertices = 0;

	this->IndexBufferID = 0;
	this->IndexBuffer_Start_Index = 0;
	this->numberOfIndices = 0;
	this->numberOfTriangles = 0;

	// The "local" (i.e. "CPU side" temporary array)
	this->pVertices = 0;
	this->pIndices = 0;

	//glm::vec3 maxValues;
	//glm::vec3 minValues;
	////	scale = 5.0/maxExtent;		-> 5x5x5
	//float maxExtent;
}

cModel::~cModel() {
	DEBUG_PRINT("cModel::~cModel()\n");
	delete[] pVertices;
}
