#pragma once
#include "glm\glm.hpp"
#include "glm\gtc\quaternion.hpp"
#include "Gizmos.h"

enum ShapeType
{
	PLANE,
	SPHERE,
	BOX
};

class PhysicsObject
{
public:
	PhysicsObject();
	ShapeType m_ShapeID;
	void virtual Update(glm::vec3 _gravity, float _timeStep) = 0;
	void virtual Draw() {};
	void Debug();
	void virtual MakeGizmo() = 0;
	void virtual ResetPosition() {};
};

class DIYRigidBody : public PhysicsObject
{
public:
	DIYRigidBody(glm::vec3 _position, glm::vec3 _velocity, glm::quat _rotation, float _mass);

	glm::vec3 m_Position;
	glm::vec3 m_Velocity;
	glm::vec3 m_Acceleration;

	float m_Mass;
	float m_Rotation2D;

	virtual void Update(glm::vec3 _gravity, float _timeStep) override;
	virtual void Debug();
	void ApplyForce(glm::vec2 _force);
	void ApplyForceToActor(DIYRigidBody* _actor, glm::vec3 _force);
};

class SphereClass : public DIYRigidBody
{
public:
	float m_Radius;

	SphereClass(glm::vec3 _position, glm::vec3 _velocity, 
		float _mass, float _radius, 
		glm::vec4 _colour);

	virtual void MakeGizmo();

};

class PlaneClass : public PhysicsObject
{
public:
	glm::vec3 m_Normal;
	float m_Distance;

	void virtual Update(glm::vec3 _gravity, float _deltaTime);
	void virtual MakeGizmo();

	PlaneClass(glm::vec3 _normal, float _distance);
	PlaneClass();
};