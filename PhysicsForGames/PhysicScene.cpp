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
	PhysicScene::Plane2Plane, PhysicScene::Plane2Sphere, PhysicScene::Plane2Box, PhysicScene::Plane2Joint,
	PhysicScene::Sphere2Plane, PhysicScene::Sphere2Sphere, PhysicScene::Sphere2Box, PhysicScene::Sphere2Joint,
	PhysicScene::Box2Plane, PhysicScene::Box2Sphere, PhysicScene::Box2Box, PhysicScene::Box2Joint,
	PhysicScene::Joint2Plane, PhysicScene::Joint2Sphere, PhysicScene::Joint2Box, PhysicScene::Joint2Joint
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
	const float _coeffRestitution = 0.4f;
	//float _impulse1 = -(1 + _coeffRestitution) * glm::dot(pObject1->GetMomentum(), normal);
	//float _impulse2 = -(1 + _coeffRestitution) * glm::dot(pObject2->GetMomentum(), normal);
	//
	//pObject1->AddMomentum(_impulse1 * normal);
	//pObject2->AddMomentum(_impulse2 * normal);

	glm::vec3 _relativeVel = pObject2->GetVelocity() - pObject1->GetVelocity();
	float _velocityAlongNormal = glm::dot(_relativeVel, normal);
	float _impulseAmount = -(1 + _coeffRestitution) * _velocityAlongNormal;
	_impulseAmount /= 1 / pObject1->GetMass() + 1 / pObject2->GetMass();
	
	glm::vec3 impulse = _impulseAmount * normal;
	pObject1->AddVelocity(1 / pObject1->GetMass() * -impulse);
	pObject2->AddVelocity(1 / pObject2->GetMass() * +impulse);

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

bool PhysicScene::Plane2Box(PhysicsObject * _planeObj, PhysicsObject * _boxObj)
{
	PlaneClass *plane = dynamic_cast<PlaneClass*>(_planeObj);
	BoxClass *box = dynamic_cast<BoxClass*>(_boxObj);

	glm::vec3 _boxPos = _boxObj->GetPosition();
	glm::vec3 _minPos = _boxPos - box->GetExtents();
	glm::vec3 _maxPos = _boxPos + box->GetExtents();

	float _minPointDistAlongPlaneNorm = glm::dot(_minPos, plane->GetNormal());
	float _maxPointDistAlongPlaneNorm = glm::dot(_maxPos, plane->GetNormal());

	float _overlap = glm::min(_minPointDistAlongPlaneNorm, _maxPointDistAlongPlaneNorm);

	if (_overlap < 0)
	{
		Response(_planeObj, _boxObj, -_overlap, plane->GetNormal());
		return true;
	}


	return false;
}

bool PhysicScene::Plane2Joint(PhysicsObject * obj1, PhysicsObject * obj2)
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

bool PhysicScene::Sphere2Box(PhysicsObject * _sphereObj, PhysicsObject * _boxObj)
{
	SphereClass *sphere = dynamic_cast<SphereClass*>(_sphereObj);
	BoxClass *box = dynamic_cast<BoxClass*>(_boxObj);

	glm::vec3 _minPos = -box->GetExtents();
	glm::vec3 _maxPos = box->GetExtents();

	glm::vec3 _dist = _sphereObj->GetPosition() - _boxObj->GetPosition();
	glm::vec3 _clampedPoint = _dist;

	if (_dist.x < _minPos.x)
		_clampedPoint.x = _minPos.x;
	else if (_dist.x > _maxPos.x)
		_clampedPoint.x = _maxPos.x;

	if (_dist.y < _minPos.y)
		_clampedPoint.y = _minPos.y;
	else if (_dist.y > _maxPos.y)
		_clampedPoint.y = _maxPos.y;

	if (_dist.z < _minPos.z)
		_clampedPoint.z = _minPos.z;
	else if (_dist.z > _maxPos.z)
		_clampedPoint.z = _maxPos.z;

	glm::vec3 _clampedDist = _dist - _clampedPoint;
	float _overlap = glm::length(_clampedDist) - sphere->GetRadius();
	if (_overlap < 0)
	{
		Response(_boxObj, _sphereObj, -_overlap, glm::normalize(_dist)); // 
		return true;
	}

	return false;
}

bool PhysicScene::Sphere2Joint(PhysicsObject * obj1, PhysicsObject * obj2)
{
	return false;
}

bool PhysicScene::Box2Plane(PhysicsObject * _boxObj, PhysicsObject * _planeObj)
{
	return Plane2Box(_planeObj, _boxObj);
}

bool PhysicScene::Box2Sphere(PhysicsObject * _boxObj, PhysicsObject * _sphereObj)
{
	return Sphere2Box(_sphereObj, _boxObj);
}

bool PhysicScene::Box2Box(PhysicsObject * _boxObj1, PhysicsObject * _boxObj2)
{
	BoxClass *box1 = dynamic_cast<BoxClass*>(_boxObj1);
	BoxClass *box2 = dynamic_cast<BoxClass*>(_boxObj2);

	if (box1 != nullptr && box2 != nullptr)
	{
		glm::vec3 _box1Pos = _boxObj1->GetPosition();
		glm::vec3 _box1Extents = box1->GetExtents();

		glm::vec3 _box2Pos = _boxObj2->GetPosition();
		glm::vec3 _box2Extents = box2->GetExtents();

		glm::vec3 _boxDelta = _box2Pos - _box1Pos;
		glm::vec3 _extentsCombined = _box1Extents + _box2Extents;

		float xOverlap = std::abs(_boxDelta.x) - _extentsCombined.x;
		float yOverlap = std::abs(_boxDelta.y) - _extentsCombined.y;
		float zOverlap = std::abs(_boxDelta.z) - _extentsCombined.z;

		if (xOverlap <= 0 && yOverlap <= 0 && zOverlap <= 0)
		{
			float minOverlap = xOverlap;
			minOverlap = yOverlap < 0 ? glm::max(minOverlap, yOverlap) : minOverlap;
			minOverlap = zOverlap < 0 ? glm::max(minOverlap, zOverlap) : minOverlap;

			glm::vec3 separationNormal(0);

			if		(xOverlap == minOverlap) separationNormal.x = std::signbit(_boxDelta.x) ? -1.0f : 1.0f;
			else if (yOverlap == minOverlap) separationNormal.y = std::signbit(_boxDelta.y) ? -1.0f : 1.0f;
			else if (zOverlap == minOverlap) separationNormal.z = std::signbit(_boxDelta.z) ? -1.0f : 1.0f;

			Response(_boxObj1, _boxObj2, -minOverlap, separationNormal);

			return true;
		}
	}


	return false;
}

bool PhysicScene::Box2Joint(PhysicsObject * obj1, PhysicsObject * obj2)
{
	return false;
}

bool PhysicScene::Joint2Plane(PhysicsObject * obj1, PhysicsObject * obj2)
{
	return false;
}

bool PhysicScene::Joint2Sphere(PhysicsObject * obj1, PhysicsObject * obj2)
{
	return false;
}

bool PhysicScene::Joint2Box(PhysicsObject * obj1, PhysicsObject * obj2)
{
	return false;
}

bool PhysicScene::Joint2Joint(PhysicsObject * obj1, PhysicsObject * obj2)
{
	return false;
}
