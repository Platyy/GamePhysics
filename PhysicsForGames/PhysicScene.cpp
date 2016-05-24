#include "PhysicScene.h"

PhysicScene::PhysicScene()
{
}

PhysicScene::~PhysicScene()
{
}

void PhysicScene::CheckForCollisions()
{
	int _actorCount = m_Actors.size();
	for (int _outer = 0; _outer < _actorCount - 1; _outer++)
	{
		for (int _inner = _outer + 1; _inner < _actorCount; _inner++)
		{
			PhysicsObject* object1 = m_Actors[_outer];
			PhysicsObject* object2 = m_Actors[_inner];
			ShapeType _shapeID1 = object1->m_ShapeID;
			ShapeType _shapeID2 = object2->m_ShapeID;

			switch (_shapeID1)
			{
			case PLANE:
				switch (_shapeID2)
				{
				case PLANE:
					//Plane2Plane
					break;
				case SPHERE:
					//Plane2Sphere
					break;
				}
				break;
			case SPHERE:
				switch (_shapeID2)
				{
				case PLANE:
					//Sphere2Plane
					break;
				case SPHERE:
					//Sphere2Sphere
					break;
				}
				break;
			}
		}
	}
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
