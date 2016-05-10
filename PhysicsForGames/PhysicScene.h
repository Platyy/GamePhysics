#pragma once

#include "glm\glm.hpp"
#include <vector>
#include "PhysicsObject.h"

class PhysicScene
{
public:
	glm::vec3 m_Gravity = glm::vec3(0.f, -9.81f, 0.f);
	float m_TimeStep = .001f;

	std::vector<PhysicsObject* > m_Actors;
	
	void AddActor(PhysicsObject* );
	void RemoveActor(PhysicsObject* );
	void Update();
	void UpdateGizmos();
	void DebugScene();
	void AddGizmos();
};

