#pragma once
#include <PxScene.h>
#include <pvd\PxVisualDebugger.h>
#include <PxPhysicsAPI.h>

using namespace physx;

class MyControllerHitReport : public PxUserControllerHitReport
{
public:
	//overload the onShapeHit function
	virtual void onShapeHit(const PxControllerShapeHit &hit);
	//other collision functions which we must overload //these handle collision
	//with other controllers and hitting obstacles
	virtual void onControllerHit(const PxControllersHit &hit) {};
	//Called when current controller hits another controller. More...
	virtual void onObstacleHit(const PxControllerObstacleHit &hit) {};
	//Called when current controller hits a user-defined obstacl
	MyControllerHitReport() :PxUserControllerHitReport() {};
	PxVec3 getPlayerContactNormal() { return _playerContactNormal; };
	void clearPlayerContactNormal() { _playerContactNormal = PxVec3(0, 0, 0); };
	PxVec3 _playerContactNormal;

	bool m_Grounded;
};

