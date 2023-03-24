#include "DebugDrawer.h"

#include "Conversion.h"

#include <glm/glm.hpp>

#include "CheckGLError.h"

namespace physics
{
	DebugDrawer::DebugDrawer(GLuint shaderId)
		: m_ShaderId(shaderId)
	{
		unsigned int numVerticesInVertArray = 2;
		unsigned int numIndicesInIndexArray = 2;
		sVertex_p4c4* pTempVertArray = new sVertex_p4c4[numIndicesInIndexArray];
		GLuint* pIndexArrayLocal = new GLuint[numIndicesInIndexArray];

		pTempVertArray[0].Pos.x = 10.f;
		pTempVertArray[0].Pos.y = 0.f;
		pTempVertArray[0].Pos.z = 0.f;
		pTempVertArray[0].Pos.w = 1.f;
		pTempVertArray[0].Color.x = 0.f;
		pTempVertArray[0].Color.y = 1.f;
		pTempVertArray[0].Color.z = 0.f;
		pTempVertArray[0].Color.w = 0.f;

		pTempVertArray[1].Pos.x = 0.f;
		pTempVertArray[1].Pos.y = 0.f;
		pTempVertArray[1].Pos.z = 0.f;
		pTempVertArray[1].Pos.w = 1.f;
		pTempVertArray[1].Color.x = 0.f;
		pTempVertArray[1].Color.y = 1.f;
		pTempVertArray[1].Color.z = 0.f;
		pTempVertArray[1].Color.w = 0.f;

		glGenVertexArrays(1, &m_VBO);
		glBindVertexArray(m_VBO);
		CheckGLError();

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		CheckGLError();

		glGenBuffers(1, &m_VertexBufferId);
		glGenBuffers(1, &m_IndexBufferId);
		CheckGLError();

		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId);
		CheckGLError();

		unsigned int totalVertBufferSizeBYTES = numIndicesInIndexArray * sizeof(sVertex_p4c4);
		glBufferData(GL_ARRAY_BUFFER, totalVertBufferSizeBYTES, pTempVertArray, GL_DYNAMIC_DRAW);
		CheckGLError();

		unsigned int bytesInOneVertex = sizeof(sVertex_p4c4);
		unsigned int byteOffsetToPosition = offsetof(sVertex_p4c4, Pos);
		unsigned int byteOffsetToColor = offsetof(sVertex_p4c4, Color);

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, bytesInOneVertex, (GLvoid*)byteOffsetToPosition);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, bytesInOneVertex, (GLvoid*)byteOffsetToColor);
		CheckGLError();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferId);
		CheckGLError();

		unsigned int sizeOfIndexArrayInBytes = numIndicesInIndexArray * sizeof(GLuint);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeOfIndexArrayInBytes, pIndexArrayLocal, GL_DYNAMIC_DRAW);
		CheckGLError();

		glBindVertexArray(0);
		CheckGLError();

		delete[] pTempVertArray;
		delete[] pIndexArrayLocal;
	}

	DebugDrawer::~DebugDrawer()
	{

	}

	void DebugDrawer::drawLine(const btVector3& btFrom, const btVector3& btTo, const btVector3& btColor)
	{
		unsigned int bytesInOneVertex = sizeof(sVertex_p4c4);
		unsigned int byteOffsetToPosition = offsetof(sVertex_p4c4, Pos);
		unsigned int byteOffsetToColor = offsetof(sVertex_p4c4, Color);

		// Depending on how your model is loaded and stored
		// You may not have a list of all of your triangles in order
		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId);
		CheckGLError();
		glm::vec3 glmFrom, glmTo, glmColor;

		CastGLMVec3(btFrom, &glmFrom);
		CastGLMVec3(btTo, &glmTo);
		CastGLMVec3(btColor, &glmColor);

		glm::vec4 orgin(0.f, 0.f, 0.f, 1.f);
		glm::vec4 left(.5f, .5f, 0.f, 1.f);

		GLsizeiptr size = GLsizeiptr(sizeof(float));

		// FROM Point
		glBufferSubData(GL_ARRAY_BUFFER,
			/* offset */	GLintptr(0),
			/* size */		size * 3,
			/* data */		&glmFrom);
		CheckGLError();

		// FROM Color
		glBufferSubData(GL_ARRAY_BUFFER,
			/* offset */	GLintptr(byteOffsetToColor),
			/* size */		size * 3,
			/* data */		&glmColor);
		CheckGLError();

		glBufferSubData(GL_ARRAY_BUFFER,
			/* offset */	GLintptr(bytesInOneVertex),
			/* size */		size * 3,
			/* data */		&glmTo);
		CheckGLError();

		// FROM Color
		glBufferSubData(GL_ARRAY_BUFFER,
			/* offset */	GLintptr(bytesInOneVertex + byteOffsetToColor),
			/* size */		size * 3,
			/* data */		&glmColor);
		CheckGLError();

		glBindBuffer(GL_ARRAY_BUFFER, 0);


		glBindVertexArray(m_VBO);
		CheckGLError();
		glDrawArrays(GL_LINES, 0, 2);
		//glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, (GLvoid*)0);
		CheckGLError();
	}

	void DebugDrawer::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)	 
	{
		drawLine(PointOnB, PointOnB + normalOnB, color);
	}

	void DebugDrawer::reportErrorWarning(const char* warningString)																						 
	{
		printf("DebugDrawer:ErrorWarning: %s\n", warningString);
	}

	void DebugDrawer::draw3dText(const btVector3& location, const char* textString)																		 
	{
		// My graphics engine doesn't support text at the moment
	}

	void DebugDrawer::setDebugMode(int debugMode)																										 
	{
		m_DebugMode = debugMode;
	}

	int DebugDrawer::getDebugMode() const																												 
	{
		return m_DebugMode;
	}
}