#include "Ragdoll.h"



Ragdoll::Ragdoll()
{
}


Ragdoll::~Ragdoll()
{
}

PxArticulation * Ragdoll::MakeRagdoll(PxPhysics * g_Physics, RagdollNode ** _nodeArray, PxTransform _worldPos, float _scaleFactor, PxMaterial* _ragdollMat)
{
	PxArticulation *_articulation = g_Physics->createArticulation();
	RagdollNode** _currentNode = _nodeArray;
	while (*_currentNode != NULL)
	{
		RagdollNode* _currentNodePtr = *_currentNode;
		RagdollNode* _parentNode = nullptr;

		float _radius = _currentNodePtr->s_Radius * _scaleFactor;
		float _halfLength = _currentNodePtr->s_HalfLength * _scaleFactor;
		float _childHalfLength = _radius + _halfLength;
		float _parentHalfLength = 0;

		PxArticulationLink* _parentLinkPtr = NULL;
		_currentNodePtr->s_ScaledGlobalPos = _worldPos.p;

		if (_currentNodePtr->s_ParentNodeID != NO_PARENT)
		{
			_parentNode = *(_nodeArray + _currentNodePtr->s_ParentNodeID);
			_parentLinkPtr = _parentNode->s_LinkPtr;
			_parentHalfLength = (_parentNode->s_Radius + _parentNode->s_HalfLength) * _scaleFactor;

			PxVec3 _currentRelative = _currentNodePtr->s_ChildLinkPos * _currentNodePtr->s_GlobalRotation.rotate(PxVec3(_childHalfLength, 0, 0));
			PxVec3 _parentRelative = -_currentNodePtr->s_ParentLinkPos * _parentNode->s_GlobalRotation.rotate(PxVec3(_parentHalfLength, 0, 0));
			_currentNodePtr->s_ScaledGlobalPos = _parentNode->s_ScaledGlobalPos - (_parentRelative + _currentRelative);
		}

		PxTransform _linkTransform = PxTransform(_currentNodePtr->s_ScaledGlobalPos, _currentNodePtr->s_GlobalRotation);
		PxArticulationLink* _link = _articulation->createLink(_parentLinkPtr, _linkTransform);
		_currentNodePtr->s_LinkPtr = _link;

		float _jointSpace = 1.0f;
		float _capsuleHalfLength = (_halfLength > _jointSpace ? _halfLength - _jointSpace : 0) + .01f;
		PxCapsuleGeometry _capsule(_radius, _capsuleHalfLength);
		_link->createShape(_capsule, *_ragdollMat);
		PxRigidBodyExt::updateMassAndInertia(*_link, 100.0f);

		if (_currentNodePtr->s_ParentNodeID != NO_PARENT)
		{

			PxArticulationJoint *_joint = _link->getInboundJoint();
			PxQuat _frameRot = _parentNode->s_GlobalRotation.getConjugate() * _currentNodePtr->s_GlobalRotation;
			PxTransform _parentConstraintFrame = PxTransform(PxVec3(_currentNodePtr->s_ParentLinkPos * _parentHalfLength, 0, 0));
			PxTransform _thisConstraintFrame = PxTransform(PxVec3(_currentNodePtr->s_ChildLinkPos * _childHalfLength, 0, 0));

			_joint->setParentPose(_parentConstraintFrame);
			_joint->setChildPose(_thisConstraintFrame);
			_joint->setStiffness(20);
			_joint->setDamping(20);
			_joint->setSwingLimit(0.4f, 0.4f);
			_joint->setSwingLimitEnabled(true);
			_joint->setTwistLimit(-0.1f, 0.1f);
			_joint->setTwistLimitEnabled(true);
		}

		_currentNode++;
	}
	return _articulation;
}
