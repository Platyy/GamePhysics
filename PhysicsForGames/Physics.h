#ifndef SOUND_PROGRAMMING_H_
#define SOUND_PROGRAMMING_H_

#include "Application.h"
#include "Camera.h"
#include "Render.h"
#include "PhysicScene.h"

#include <PxPhysicsAPI.h>
#include <PxScene.h>
#include <pvd\PxVisualDebugger.h>

using namespace physx;
class Physics : public Application
{
public:
	PhysicScene* m_PhysScene;

	virtual bool startup();
	void PhysSetup();
	virtual void shutdown();
    virtual bool update();
    virtual void draw();

	void setUpPhysX();
	void updatePhysX(float _deltaTime);
	void cleanUpPhysX();

	void addPlane();

	void addBox();

	void setUpVisualDebugger();

	void renderGizmos(PxScene* physics_scene);

	void CreateSphere(float _launchSpeed);

	bool m_IsPressed = false;

    Renderer* m_renderer;
    FlyCamera m_camera;
    float m_delta_time;
	float m_LastFrameTime;


	PxFoundation* g_PhysicsFoundation;
	PxPhysics* g_Physics;
	PxScene* g_PhysicsScene;
	PxDefaultErrorCallback gDefaultErrorCallback;
	PxDefaultAllocator gDefaultAllocatorCallback;
	PxSimulationFilterShader gDefaultFilterShader = PxDefaultSimulationFilterShader;
	PxMaterial* g_PhysicsMaterial;
	PxMaterial* g_boxMaterial;
	PxCooking* g_PhysicsCooker;
};

class MyAllocator : public PxAllocatorCallback
{
public:
	MyAllocator() {}
	virtual ~MyAllocator() {}
	virtual void* allocate(size_t size, const char* typeName, const char* filename, int line)
	{
		void* pointer = _aligned_malloc(size, 16);
		return pointer;
	}
	virtual void deallocate(void* ptr)
	{
		_aligned_free(ptr);
	}
};

#endif //CAM_PROJ_H_
