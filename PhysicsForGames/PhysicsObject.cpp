#include "PhysicsObject.h"


//### PHYSICS OBJECT ###


void PhysicsObject::Translate(glm::vec3 _positionDelta)
{
	m_Position += _positionDelta;
}

glm::vec3 PhysicsObject::GetPosition() const
{
	return m_Position;
}

glm::vec3 PhysicsObject::GetVelocity() const
{
	return m_Rigidbody == nullptr ? glm::vec3(0) : m_Rigidbody->GetVelocity();
}

float PhysicsObject::GetMass() const
{
	if (m_Rigidbody == nullptr)
		return std::numeric_limits<float>::max();
	else
		return m_Rigidbody->GetMass();


}

//### RIGIDBODY ###

DIYRigidBody::DIYRigidBody(glm::vec3 _velocity, glm::quat _rotation, float _mass)
{
	m_Velocity = _velocity;
	m_Mass = _mass;
}

glm::vec3 DIYRigidBody::CalculateDeltaPosition(glm::vec3 _gravity, float _timeStep)
{
	m_Acceleration += _gravity;
	m_Velocity += m_Acceleration * _timeStep;
	m_Acceleration = glm::vec3(0, 0, 0);

	return m_Velocity * _timeStep;
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

void DIYRigidBody::AddVelocity(glm::vec3 _velocity)
{
	m_Velocity += _velocity;
}

void DIYRigidBody::AddMomentum(glm::vec3 _momentum)
{
	AddVelocity(_momentum / m_Mass);
}

//### SPHERE ###

SphereClass::SphereClass(glm::vec3 _position, DIYRigidBody* _rigidbody, float _radius, glm::vec4 _colour) : PhysicsObject(_position, ShapeType::SPHERE, PhysicsType::DYNAMIC, _rigidbody)
{
}

void SphereClass::MakeGizmo()
{
	Gizmos::addSphere(m_Position, m_Radius, 10, 10, glm::vec4(1, 0, 0, 1));
}

//### PLANE ###

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
	m_ShapeID = ShapeType::PLANE;
	m_PhysType = PhysicsType::STATIC;
}

PlaneClass::PlaneClass()
{
}

//### BOX ###

BoxClass::BoxClass(glm::vec3 _position, glm::vec3 _extents, DIYRigidBody * _rigidbody) 
	: PhysicsObject(_position, ShapeType::BOX, PhysicsType::DYNAMIC, _rigidbody),
	m_Extents(_extents)
{
}

void BoxClass::MakeGizmo()
{
	Gizmos::addAABBFilled(m_Position, m_Extents, glm::vec4(1, 0, 0, 1));
}

//### JOINT ###

SpringJoint::SpringJoint(PhysicsObject * _connection1, PhysicsObject * _connection2, float _springCoefficient, float _restLength, float _damping, 
	glm::vec3 _position, DIYRigidBody* _rigidbody)
	: PhysicsObject(_position, JOINT, PhysicsType::DYNAMIC, _rigidbody)
{
	m_Connections[0] = _connection1;
	m_Connections[1] = _connection2;
	m_SpringCoefficient = _springCoefficient;
	m_Damping = _damping;
	m_RestLength = _restLength;
	m_ShapeID = JOINT;
}

void SpringJoint::Update(glm::vec3 _gravity, float _deltaTime)
{
	// F = -kX - (bv)
	// k = spring constant (stiffness)
	// X = displacement (dist of end of spring from resting pos)
	// F = force spring exerts to return to rest pos
	// b = damping, higher will result in faster returns to rest
	// v = relative velocity of the particle

	float k = m_SpringCoefficient;
	float X = glm::length(m_Connections[0]->GetPosition() - m_Connections[1]->GetPosition()) - m_RestLength;
	float b = m_Damping;
	float v = glm::length(m_Connections[0]->GetVelocity() - m_Connections[1]->GetVelocity());
	float F = (-k * X) - (b * v);

	glm::vec3 _forceDir = glm::normalize(m_Connections[0]->GetPosition() - m_Connections[1]->GetPosition());
	//if (m_Connections[0]->m_PhysType == DYNAMIC)
		m_Connections[0]->AddVelocity(F * _forceDir);
	//if (m_Connections[1]->m_PhysType == DYNAMIC)
		m_Connections[1]->AddVelocity(-F * _forceDir);

	Gizmos::addLine(m_Connections[0]->GetPosition(), m_Connections[1]->GetPosition(), glm::vec4(0, 0, 1, 1));
}