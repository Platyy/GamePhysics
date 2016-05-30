#include "PhysicScene.h"

PhysicScene::PhysicScene()
{
}

PhysicScene::~PhysicScene()
{
}

typedef bool(*fn) (PhysicsObject*, PhysicsObject*);
static fn CollisionFunctionArray[] =
{
	PhysicScene::Plane2Plane, PhysicScene::Plane2Sphere, PhysicScene::Plane2Box,
	PhysicScene::Sphere2Plane, PhysicScene::Sphere2Sphere, PhysicScene::Sphere2Box,
	PhysicScene::Box2Plane, PhysicScene::Box2Sphere, PhysicScene::Box2Box
};


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

			int _functionIndex = (_shapeID1 * NUMBERSHAPE) + _shapeID2;
			fn _collisionFunctionPtr = CollisionFunctionArray[_functionIndex];
			if (_collisionFunctionPtr != NULL)
			{
				_collisionFunctionPtr(object1, object2);
			}


			//switch (_shapeID1)
			//{
			//case PLANE:
			//	switch (_shapeID2)
			//	{
			//	case PLANE:
			//		//Plane2Plane
			//		break;
			//	case SPHERE:
			//		//Plane2Sphere
			//		break;
			//	}
			//	break;
			//case SPHERE:
			//	switch (_shapeID2)
			//	{
			//	case PLANE:
			//		//Sphere2Plane
			//		break;
			//	case SPHERE:
			//		//Sphere2Sphere
			//		break;
			//	}
			//	break;
			//}
		}
	}
}

void PhysicScene::Response(PhysicsObject* pObject1, PhysicsObject* pObject2, float overlap, glm::vec3 normal)
{
	Separate(pObject1, pObject2, overlap, normal);
	const float _coeffRestitution = 0.5f;
	float _impulse1 = -(1 + _coeffRestitution) * glm::dot(pObject1->GetMomentum(), normal);
	float _impulse2 = -(1 + _coeffRestitution) * glm::dot(pObject2->GetMomentum(), normal);

	pObject1->AddMomentum(_impulse1 * normal);
	pObject2->AddMomentum(_impulse2 * normal);
}

void PhysicScene::Separate(PhysicsObject * pObject1, PhysicsObject * pObject2, float overlap, glm::vec3 normal)
{
	float _totalMass = pObject1->GetMass() + pObject2->GetMass();
	float _massRatio1 = pObject1->GetMass() / _totalMass;
	float _massRatio2 = pObject2->GetMass() / _totalMass;

	glm::vec3 _separationVec = normal * overlap;
	pObject1->Translate(-_separationVec * _massRatio2);
	pObject2->Translate(_separationVec * _massRatio1);
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
		CheckForCollisions();
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

bool PhysicScene::Plane2Plane(PhysicsObject * obj1, PhysicsObject * obj2)
{
	return false;
}

bool PhysicScene::Plane2Sphere(PhysicsObject * _planeObj, PhysicsObject * _sphereObj)
{
	SphereClass *sphere = dynamic_cast<SphereClass*>(_sphereObj);
	PlaneClass *plane = dynamic_cast<PlaneClass*>(_planeObj);

	if (sphere == nullptr || plane == nullptr)
		return false;

	const glm::vec3 _sphereVec = _sphereObj->GetPosition();
	const glm::vec3 _planeNorm = plane->GetNormal();

	float _sphereDistAlongNorm = glm::dot(_sphereVec, _planeNorm);
	float _overlap = _sphereDistAlongNorm - (plane->GetDistance() + sphere->GetRadius());

	if (_overlap < 0)
	{
		Response(_planeObj, _sphereObj, -_overlap, _planeNorm);
	}

	return false;
}

bool PhysicScene::Plane2Box(PhysicsObject * obj1, PhysicsObject * obj2)
{
	return false;
}

bool PhysicScene::Sphere2Plane(PhysicsObject * _sphereObj, PhysicsObject * _planeObj)
{
	return Plane2Sphere(_planeObj, _sphereObj);
}

bool PhysicScene::Sphere2Sphere(PhysicsObject* obj1, PhysicsObject* obj2)
{
	SphereClass *sphere1 = dynamic_cast<SphereClass*>(obj1);
	SphereClass *sphere2 = dynamic_cast<SphereClass*>(obj2);

	if (sphere1 == nullptr || sphere2 == nullptr)
		return false;

	glm::vec3 _dirVec = obj2->GetPosition() - obj1->GetPosition();
	float _centerDist = glm::length(_dirVec);
	float _radiusDist = sphere1->GetRadius() + sphere2->GetRadius();

	float _overlap = _centerDist - _radiusDist;
	if (_overlap < 0)
	{
		Response(obj1, obj2, -_overlap, glm::normalize(_dirVec));
		return true;
	}

	return false;
}

bool PhysicScene::Sphere2Box(PhysicsObject * obj1, PhysicsObject * obj2)
{
	return false;
}

bool PhysicScene::Box2Plane(PhysicsObject * obj1, PhysicsObject * obj2)
{
	return false;
}

bool PhysicScene::Box2Sphere(PhysicsObject * obj1, PhysicsObject * obj2)
{
	return false;
}

bool PhysicScene::Box2Box(PhysicsObject * obj1, PhysicsObject * obj2)
{
	return false;
}
