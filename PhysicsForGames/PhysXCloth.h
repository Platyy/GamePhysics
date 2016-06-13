#pragma once
#include "glm\glm.hpp"
#include "gl_core_4_4.h"
#include "glm\gtc\matrix_inverse.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "Gizmos.h"
#include <PxPhysicsAPI.h>

using namespace physx;
class PhysXCloth
{
public:
	PhysXCloth();
	~PhysXCloth();

	unsigned int m_ShaderID, m_TextureID;
	unsigned int m_ClothIndexCount;
	unsigned int m_ClothVertexCount;
	unsigned int m_ClothVAO, m_ClothVBO, m_ClothTextureVBO, m_ClothIBO;


	void SetupCloth();
	void Render(glm::mat4 _camMatrix, glm::mat4 _projMatrix);


	PxCloth* _createCloth(const glm::vec3& _pos, unsigned int& _vertCount,
		unsigned int& _indexCount, const glm::vec3* _verts, unsigned int* _indices);
	PxCloth* m_Cloth;
};
