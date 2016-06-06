#pragma once

#include "glm\glm.hpp"
#include <vector>
#include <assert.h>
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

	static bool Plane2Plane(PhysicsObject* obj1, PhysicsObject* obj2);
	static bool Plane2Sphere(PhysicsObject* obj1, PhysicsObject* obj2);
	static bool Plane2Box(PhysicsObject* obj1, PhysicsObject* obj2);
	static bool Plane2Joint(PhysicsObject* obj1, PhysicsObject* obj2);

	static bool Sphere2Plane(PhysicsObject* obj1, PhysicsObject* obj2);
	static bool Sphere2Sphere(PhysicsObject* obj1, PhysicsObject* obj2);
	static bool Sphere2Box(PhysicsObject* obj1, PhysicsObject* obj2);
	static bool Sphere2Joint(PhysicsObject* obj1, PhysicsObject* obj2);

	static bool Box2Plane(PhysicsObject* obj1, PhysicsObject* obj2);
	static bool Box2Sphere(PhysicsObject* obj1, PhysicsObject* obj2);
	static bool Box2Box(PhysicsObject* obj1, PhysicsObject* obj2);
	static bool Box2Joint(PhysicsObject* obj1, PhysicsObject* obj2);

	static bool Joint2Plane(PhysicsObject* obj1, PhysicsObject* obj2);
	static bool Joint2Sphere(PhysicsObject* obj1, PhysicsObject* obj2);
	static bool Joint2Box(PhysicsObject* obj1, PhysicsObject* obj2);
	static bool Joint2Joint(PhysicsObject* obj1, PhysicsObject* obj2);

	static void Response(PhysicsObject* pObject1, PhysicsObject* pObject2, float overlap, glm::vec3 normal);
	static void Separate(PhysicsObject* pObject1, PhysicsObject* pObject2, float overlap, glm::vec3 normal);

	void AddActor(PhysicsObject* _physObj);
	void RemoveActor(PhysicsObject* _physObj);
	void Update(float _dt);
	void DebugScene();
	void AddGizmos();
};


