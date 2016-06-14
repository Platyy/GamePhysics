#ifndef SOUND_PROGRAMMING_H_
#define SOUND_PROGRAMMING_H_

#include "Application.h"
#include "Camera.h"
#include "Render.h"
#include "PhysicScene.h"
#include "Ragdoll.h"
#include "MyControllerHitReport.h"

#include <streambuf>
#include <PxPhysicsAPI.h>
#include <PxScene.h>
#include <pvd\PxVisualDebugger.h>
#include <iostream>

#include "PhysXCloth.h"
using namespace physx;
class Physics : public Application
{
public:
	
	PhysicScene* m_PhysScene;
	PxCloth* m_Cloth;
	PhysXCloth* m_MyCloth;
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

	void addCapsule();

	void setUpVisualDebugger();

	void renderGizmos(PxScene* physics_scene);

	void CreateSphere(float _launchSpeed);
	void CreateBox(float _launchSpeed);

	void CreateJoint(PhysicsObject * _obj1, PhysicsObject * _obj2);

	void MakeCharController();

	bool m_IsPressed = false;

    Renderer* m_renderer;
    FlyCamera m_camera;
    float m_delta_time;
	float m_LastFrameTime;

	SphereClass* m_Spheres[5];
	int m_Thrown = 0;


	PxFoundation* g_PhysicsFoundation;
	PxPhysics* g_Physics;
	PxScene* g_PhysicsScene;
	PxDefaultErrorCallback gDefaultErrorCallback;
	PxDefaultAllocator gDefaultAllocatorCallback;
	PxSimulationFilterShader gDefaultFilterShader = PxDefaultSimulationFilterShader;
	PxMaterial* g_PhysicsMaterial;
	PxMaterial* g_boxMaterial;
	PxCooking* g_PhysicsCooker;

	PxControllerManager* g_CharManager;
	PxController* g_PlayerController;

	PxCloth* CreateCloth(const glm::vec3 & _pos, unsigned int & _vertCount, unsigned int & _indexCount, const glm::vec3 * _verts, unsigned int * _indices);

	/* Character Controller */
	MyControllerHitReport* m_HitReport;
	float m_YVel, m_Rotation, m_Grav;
	bool m_Grounded;
	float m_MovSpeed = 10.0f;
	float m_RotSpeed = 1.0f;

	void UpdateChar(float _deltaTime);
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