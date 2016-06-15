#include "CollisionCallbacks.h"

CollisionCallbacks::CollisionCallbacks()
{
	m_Triggered = false;
}

CollisionCallbacks::~CollisionCallbacks()
{
}

void CollisionCallbacks::onContact(const PxContactPairHeader & pairHeader, const PxContactPair * pairs, PxU32 nbPairs)
{
	for (PxU32 i = 0; i < nbPairs; i++)
	{
		const PxContactPair& cp = pairs[i];
		if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			m_Triggered = true;
		}
	}
}

void CollisionCallbacks::onTrigger(PxTriggerPair * pairs, PxU32 nbPairs)
{
	for (PxU32 i = 0; i < nbPairs; ++i)
	{
		if (!m_Triggered)
		{
			m_Triggered = true;
			pairs->triggerActor = m_TriggerBody;

		}
		else
		{
			m_Triggered = false;
		}
	}
}
