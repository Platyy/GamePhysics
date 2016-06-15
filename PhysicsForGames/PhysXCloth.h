#pragma once
#include "glm\glm.hpp"
#include "gl_core_4_4.h"
#include "glm\gtc\matrix_inverse.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "Gizmos.h"
#include "ShaderLoader.h"
#include "Render.h"
#include <PxPhysicsAPI.h>

using namespace physx;
class PhysXCloth
{
public:
	PhysXCloth();
	~PhysXCloth();

	PxCloth* SetupCloth(PxPhysics * g_physics);

	unsigned int m_ShaderID, m_TextureID;
	unsigned int m_ClothIndexCount;
	unsigned int m_ClothVertexCount;
	unsigned int m_ClothVAO, m_ClothVBO, m_ClothTextureVBO, m_ClothIBO;

	

	glm::vec3* m_ClothPositions;
	physx::PxCloth* m_cloth;


	void Render(glm::mat4 _camMatrix, glm::mat4 _projMatrix);
};

