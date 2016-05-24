#pragma once

#include "glm\glm.hpp"
#include <vector>
#include "PhysicsObject.h"

class PhysicScene
{
public:

	PhysicScene();
	~PhysicScene();

	PhysicScene* m_PhysScene;

	glm::vec3 m_Gravity = glm::vec3(0.f, -9.81f, 0.f);
	float m_TimeStep = .001f;

	std::vector<PhysicsObject* > m_Actors;
	
	void CheckForCollisions();

	void AddActor(PhysicsObject* _physObj);
	void RemoveActor(PhysicsObject* _physObj);
	void Update(float _dt);
	void DebugScene();
	void AddGizmos();
};


