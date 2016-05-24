#include "PhysicsObject.h"


//### PHYSICS OBJECT ###
PhysicsObject::PhysicsObject()
{
}

//### RIGIDBODY ###

DIYRigidBody::DIYRigidBody(glm::vec3 _position, glm::vec3 _velocity, glm::quat _rotation, float _mass)
{
	m_Position = _position;
	m_Velocity = _velocity;
	m_Mass = _mass;
}

void DIYRigidBody::Update(glm::vec3 _gravity, float _timeStep)
{
	m_Acceleration += _gravity;
	m_Velocity += m_Acceleration * _timeStep;

	m_Position += m_Velocity * _timeStep;
	m_Acceleration = glm::vec3(0, 0, 0);
}

void DIYRigidBody::Debug()
{
}

void DIYRigidBody::ApplyForce(glm::vec2 _force)
{
}

void DIYRigidBody::ApplyForceToActor(DIYRigidBody * _actor, glm::vec3 _force)
{
}

//### SPHERE ###

SphereClass::SphereClass(glm::vec3 _position, glm::vec3 _velocity, float _mass, float _radius, glm::vec4 _colour) : DIYRigidBody(_position, _velocity, glm::quat(), _mass)
{
}

void SphereClass::MakeGizmo()
{
	Gizmos::addSphere(this->m_Position, this->m_Radius, 10, 10, glm::vec4(1, 0, 0, 1));
}

void PlaneClass::Update(glm::vec3 _gravity, float _deltaTime)
{
}

void PlaneClass::MakeGizmo()
{
	float _segmentLength = 300.0f;
	glm::vec3 _center = m_Normal * m_Distance;
	glm::vec3 _parallel = glm::vec3(m_Normal.y, -m_Normal.x, m_Normal.y);
	glm::vec4 _colour(0.5,0.5,0.5,1);
	glm::vec3 _start = _center + (_parallel * _segmentLength);
	glm::vec3 _end = _center - (_parallel * _segmentLength);
	//Gizmos::addLine(_start, _end, _colour); 
	Gizmos::addAABBFilled(_center, _start, _colour);
}

PlaneClass::PlaneClass(glm::vec3 _normal, float _distance)
{
	m_Normal = _normal;
	m_Distance = _distance;
}

PlaneClass::PlaneClass()
{
}
