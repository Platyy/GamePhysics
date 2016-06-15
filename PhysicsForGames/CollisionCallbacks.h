#pragma once
#include <PxPhysicsAPI.h>
using namespace physx;

class CollisionCallbacks : public physx::PxSimulationEventCallback
{
public:
	CollisionCallbacks();
	~CollisionCallbacks();

	virtual void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs);
	virtual void onTrigger(PxTriggerPair* pairs, PxU32 nbPairs);
	virtual void onConstraintBreak(PxConstraintInfo*, PxU32) {};
	virtual void onWake(PxActor**, PxU32) {};
	virtual void onSleep(PxActor**, PxU32) {};
	const bool IsTriggered() { return m_Triggered; }
	PxRigidActor* GetTriggerActor() { return m_TriggerBody; }

	bool m_Triggered = false;
	PxRigidActor* m_TriggerBody;
};

