#include "PhysicsObject.h"


//### PHYSICS OBJECT ###
PhysicsObject::PhysicsObject()
{
}

//### RIGIDBODY ###

DIYRigidBody::DIYRigidBody(glm::vec3 _position, glm::vec3 _velocity, glm::quat _rotation, float _mass)
{
}

void DIYRigidBody::Update(glm::vec2 _gravity, float _timeStep)
{
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
}