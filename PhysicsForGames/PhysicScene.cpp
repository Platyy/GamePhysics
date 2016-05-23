#include "PhysicScene.h"

PhysicScene::PhysicScene()
{
}

PhysicScene::~PhysicScene()
{
}

void PhysicScene::AddActor(PhysicsObject* _physObj)
{
	m_Actors.push_back(_physObj);
}

void PhysicScene::RemoveActor(PhysicsObject* _physObj)
{
	for (auto iter = m_Actors.begin(); iter != m_Actors.end(); iter++)
	{
		if ((*iter) == _physObj)
			iter = m_Actors.erase(iter);
	}
}

void PhysicScene::Update(float _dt)
{
	for (auto i = m_Actors.begin(); i < m_Actors.end(); i++)
	{
		(*i)->Update(m_Gravity, _dt);
	}
}

void PhysicScene::DebugScene()
{
}

void PhysicScene::AddGizmos()
{
	for (auto i = m_Actors.begin(); i < m_Actors.end(); i++)
	{
		(*i)->MakeGizmo();
	}
}
