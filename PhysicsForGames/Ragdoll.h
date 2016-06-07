#pragma once
#include <PxPhysicsAPI.h>

#include <PxScene.h>

using namespace physx;

enum RagDollParts
{
	NO_PARENT = -1,
	LOWER_SPINE,
	LEFT_PELVIS,
	RIGHT_PELVIS,
	LEFT_UPPER_LEG,
	RIGHT_UPPER_LEG,
	LEFT_LOWER_LEG,
	RIGHT_LOWER_LEG,
	UPPER_SPINE,
	LEFT_CLAVICLE,
	RIGHT_CLAVICLE,
	NECK,
	HEAD,
	LEFT_UPPER_ARM,
	RIGHT_UPPER_ARM,
	LEFT_LOWER_ARM,
	RIGHT_LOWER_ARM
};


struct RagdollNode
{
	PxQuat s_GlobalRotation;
	PxVec3 s_ScaledGlobalPos;

	int s_ParentNodeID;
	float s_HalfLength, s_Radius, s_ParentLinkPos, s_ChildLinkPos;
	char* s_Name;

	PxArticulationLink* s_LinkPtr;

	RagdollNode(PxQuat _globalRot, int _parentNodeID, float _halfLength, float _radius,
		float _parentLinkPos, float _childLinkPos, char* _name)
	{
		s_GlobalRotation = _globalRot; s_ParentNodeID = _parentNodeID; s_HalfLength = _halfLength; s_Radius = _radius;
		s_ParentLinkPos = _parentLinkPos; s_ChildLinkPos = _childLinkPos; s_Name = _name;
	};
};


class Ragdoll
{
public:
	Ragdoll();
	~Ragdoll();

	const PxVec3 X_AXIS = PxVec3(1, 0, 0);
	const PxVec3 Y_AXIS = PxVec3(0, 1, 0);
	const PxVec3 Z_AXIS = PxVec3(0, 0, 1);
	RagdollNode* ragdollData[17] =
	{
		new RagdollNode(PxQuat(PxPi / 2.0f, Z_AXIS),		NO_PARENT,		 1, 3,	   1,  1,	"lower spine"),
		new RagdollNode(PxQuat(PxPi, Z_AXIS),				LOWER_SPINE,	 1, 1,	  -1,  1,	"left pelvis"),
		new RagdollNode(PxQuat(0, Z_AXIS),					LOWER_SPINE,	 1, 1,	  -1,  1,	"right pelvis"),
		new RagdollNode(PxQuat(PxPi / 2.0f + 0.2f, Z_AXIS),	LEFT_PELVIS,	 5, 2,	  -1,  1,	"L upper leg"),
		new RagdollNode(PxQuat(PxPi / 2.0f - 0.2f, Z_AXIS),	RIGHT_PELVIS,	 5, 2,	  -1,  1,	"R upper leg"),
		new RagdollNode(PxQuat(PxPi / 2.0f + 0.2f, Z_AXIS),	LEFT_UPPER_LEG,	 5, 1.75f, -1,  1,	"L lower leg"),
		new RagdollNode(PxQuat(PxPi / 2.0f - 0.2f, Z_AXIS),	RIGHT_UPPER_LEG, 5, 1.75f, -1,  1,	"R lower leg"),
		new RagdollNode(PxQuat(PxPi / 2.0f, Z_AXIS),		LOWER_SPINE,	 1, 3,	   1, -1,	"upper spine"),
		new RagdollNode(PxQuat(PxPi, Z_AXIS),				UPPER_SPINE,	 1, 1.5f,   1,  1,	"left clavicle"),
		new RagdollNode(PxQuat(0, Z_AXIS),					UPPER_SPINE,	 1, 1.5f,   1,  1,	"right clavicle"),
		new RagdollNode(PxQuat(PxPi / 2.0f, Z_AXIS),		UPPER_SPINE,	 1, 1,	   1, -1,	"neck"),
		new RagdollNode(PxQuat(PxPi / 2.0f, Z_AXIS),		NECK,			 1, 3,	   1, -1,	"HEAD"),
		new RagdollNode(PxQuat(PxPi - .3f, Z_AXIS),			LEFT_CLAVICLE,	 3, 1.5f,  -1,  1,	"left upper arm"),
		new RagdollNode(PxQuat(0.3f, Z_AXIS),				RIGHT_CLAVICLE,  3, 1.5f,  -1,  1,	"right upper arm"),
		new RagdollNode(PxQuat(PxPi - .3f, Z_AXIS),			LEFT_UPPER_ARM,  3, 1,	  -1,  1,	"left lower arm"),
		new RagdollNode(PxQuat(0.3f, Z_AXIS),				RIGHT_UPPER_ARM, 3, 1,	  -1,  1,	"right lower arm"),
		NULL
	};

	PxArticulation* MakeRagdoll(PxPhysics* g_Physics, RagdollNode** _nodeArray, 
		PxTransform _worldPos, float _scaleFactor, PxMaterial* _ragdollMat);

};

