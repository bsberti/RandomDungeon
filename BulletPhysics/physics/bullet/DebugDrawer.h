#pragma once

#include <btBulletDynamicsCommon.h>

#include <GL/glew.h>

namespace physics
{
	class DebugDrawer : public btIDebugDraw
	{
	public:
		DebugDrawer(GLuint shaderId);
		virtual ~DebugDrawer();

		virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;
		virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override;
		virtual void reportErrorWarning(const char* warningString) override;
		virtual void draw3dText(const btVector3& location, const char* textString) override;
		virtual void setDebugMode(int debugMode) override;
		virtual int getDebugMode() const override;

	private:
		typedef struct sFloat4 {
			float x, y, z, w;
		} sFloat4;

		typedef struct sVertex_p4c4 {
			sFloat4 Pos;
			sFloat4 Color;
		} sVertex_p4c4;

		typedef struct sVertex_p4 {
			sFloat4 Pos;
		} sVertex_p4;

		GLuint m_VertexBufferId;
		GLuint m_IndexBufferId;
		GLuint m_VBO;
		GLuint m_ShaderId;

		int m_DebugMode;
	};
}