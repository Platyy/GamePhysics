#pragma once
#include "glm\glm.hpp"
#include "glm\gtc\quaternion.hpp"
#include "Gizmos.h"
#include <memory>

enum ShapeType
{
	PLANE,
	SPHERE,
	BOX,
	NUMBERSHAPE
};

class DIYRigidBody
{
public:
	DIYRigidBody(glm::vec3 _velocity, glm::quat _rotation, float _mass);

	glm::vec3 m_Velocity;
	glm::vec3 m_Acceleration;



	float m_Mass;

	glm::vec3 GetVelocity() const { return m_Velocity; };
	float GetMass() const { return m_Mass; };
	glm::vec3 GetMomentum() const { return GetMass() * GetVelocity(); }

	float m_Rotation2D;

	glm::vec3 CalculateDeltaPosition(glm::vec3 _gravity, float _timeStep);

	virtual void Debug();
	void ApplyForce(glm::vec2 _force);
	void ApplyForceToActor(DIYRigidBody* _actor, glm::vec3 _force);
	void AddVelocity(glm::vec3 _velocity);
	void AddMomentum(glm::vec3 _momentum);
};

class PhysicsObject
{
public:
	PhysicsObject() {};
	PhysicsObject(glm::vec3 _initialPos, ShapeType _shape, DIYRigidBody* _rigidbody = nullptr) : m_Position(_initialPos), m_Rigidbody(_rigidbody), m_ShapeID(_shape)
	{}

	ShapeType m_ShapeID;

	void virtual Update(glm::vec3 _gravity, float _timeStep) {
		if (m_Rigidbody != nullptr) {
			m_Position += m_Rigidbody->CalculateDeltaPosition(_gravity, _timeStep);
		}
	}
	void virtual Draw() {};
	void virtual MakeGizmo() {};
	void virtual ResetPosition() {};

	glm::vec3 PhysicsObject::GetVelocity() const;
	float PhysicsObject::GetMass() const;
	glm::vec3 GetMomentum() const { return GetMass() * GetVelocity(); }


	void AddVelocity(glm::vec3 _velocity) { if (m_Rigidbody != nullptr) m_Rigidbody->AddVelocity(_velocity); }
	void AddMomentum(glm::vec3 _momentum) { if (m_Rigidbody != nullptr) m_Rigidbody->AddMomentum(_momentum); }
	void Translate(glm::vec3 _positionDelta);


	glm::vec3 GetPosition() const;

	glm::vec3 m_Position;
	


	std::unique_ptr<DIYRigidBody> m_Rigidbody;
};

class SphereClass : public PhysicsObject
{
public:
	float m_Radius;

	SphereClass(glm::vec3 _position, DIYRigidBody * _rigidbody, float _radius, glm::vec4 _colour);

	virtual void MakeGizmo();
	float GetRadius();
};

class PlaneClass : public PhysicsObject
{
public:
	glm::vec3 m_Normal;
	float m_Distance;

	void virtual Update(glm::vec3 _gravity, float _deltaTime);
	void virtual MakeGizmo();
	glm::vec3 GetNormal() { return m_Normal; }
	float GetDistance() { return m_Distance; }

	PlaneClass(glm::vec3 _normal, float _distance);
	PlaneClass();
};