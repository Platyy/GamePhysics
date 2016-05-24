#include "Physics.h"

#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include "Gizmos.h"

#include "glm/ext.hpp"
#include "glm/gtc/quaternion.hpp"

#define Assert(val) if (val){}else{ *((char*)0) = 0;}
#define ArrayCount(val) (sizeof(val)/sizeof(val[0]))

bool Physics::startup()
{
    if (Application::startup() == false)
    {
        return false;
    }
	
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    Gizmos::create();

    m_camera = FlyCamera(1280.0f / 720.0f, 10.0f);
    m_camera.setLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));
    m_camera.sensitivity = 3;

	m_renderer = new Renderer();
	
	Physics::setUpPhysX();
	Physics::setUpVisualDebugger();
	Physics::addPlane();
	Physics::addBox();

	Physics::PhysSetup();
	PlaneClass* _plane = new PlaneClass(glm::vec3(0, 1, 0), -0.1f);
	m_PhysScene->AddActor(_plane);
	_plane->MakeGizmo();

	m_LastFrameTime = (float)glfwGetTime();
    return true;
}

void Physics::PhysSetup()
{
	m_PhysScene = new PhysicScene();
	m_PhysScene->m_Gravity = glm::vec3(0, -10, 0);
	m_PhysScene->m_TimeStep = 0.001f;

	m_PhysScene->AddGizmos();
}

void Physics::shutdown()
{
	delete m_renderer;
	Physics::cleanUpPhysX();
    Gizmos::destroy();
    Application::shutdown();
}

void Physics::setUpVisualDebugger()
{
	if (g_Physics->getPvdConnectionManager() == NULL)
		return;

	const char* pvd_host_ip = "127.0.0.1";
	int port = 5425;

	unsigned int timeout = 100;

	PxVisualDebuggerConnectionFlags connectionFlags = PxVisualDebuggerExt::getAllConnectionFlags();
	auto theConnection = PxVisualDebuggerExt::createConnection(g_Physics->getPvdConnectionManager(), pvd_host_ip, port, timeout, connectionFlags);
	
}

void Physics::setUpPhysX()
{
	PxAllocatorCallback *myCallback = new MyAllocator();
	g_PhysicsFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, *myCallback, gDefaultErrorCallback);
	g_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *g_PhysicsFoundation, PxTolerancesScale());
	PxInitExtensions(*g_Physics);
	g_PhysicsMaterial = g_Physics->createMaterial(0.5f, 0.5f, 0.5f);
	PxSceneDesc sceneDesc(g_Physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0, -9.81f, 0); \
	sceneDesc.filterShader = &physx::PxDefaultSimulationFilterShader;
	sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(1);
	g_PhysicsScene = g_Physics->createScene(sceneDesc);

	
}

void Physics::updatePhysX(float _deltaTime)
{
	if (_deltaTime <= 0)
		return;

	g_PhysicsScene->simulate(_deltaTime);

	while (g_PhysicsScene->fetchResults() == false)
	{

	}
}

void Physics::cleanUpPhysX()
{
	g_PhysicsScene->release();
	g_Physics->release();
	g_PhysicsFoundation->release();
}

void Physics::addPlane()
{
	PxTransform pose = PxTransform(PxVec3(0.0f, 0, 0.0f), PxQuat(PxHalfPi * 1.0f, PxVec3(0.0f, 0.0f, 1.0f)));
	PxRigidStatic* plane = PxCreateStatic(*g_Physics, pose, PxPlaneGeometry(), *g_PhysicsMaterial);
	g_PhysicsScene->addActor(*plane);
}  // PhysX

void Physics::addBox()
{
	float density = 10.0f;
	PxBoxGeometry box(2, 2, 2);
	PxTransform transform(PxVec3(0, 10, 0));
	PxRigidDynamic* dynamicActor = PxCreateDynamic(*g_Physics, transform, box, *g_PhysicsMaterial, density);
	g_PhysicsScene->addActor(*dynamicActor);
} // PhysX

bool Physics::update()
{
    if (Application::update() == false)
    {
        return false;
    }

    Gizmos::clear();

    float currentTime = (float)glfwGetTime();
	float dt = currentTime - m_LastFrameTime;
	m_LastFrameTime = currentTime;
    //glfwSetTime(0.0);
	
	CreateSphere(10.0f);

	m_PhysScene->Update(dt);
	m_PhysScene->AddGizmos();

    vec4 white(1);
    vec4 black(0, 0, 0, 1);

    for (int i = 0; i <= 20; ++i)
    {
        Gizmos::addLine(vec3(-10 + i, -0.01, -10), vec3(-10 + i, -0.01, 10),
            i == 10 ? white : black);
        Gizmos::addLine(vec3(-10, -0.01, -10 + i), vec3(10, -0.01, -10 + i),
            i == 10 ? white : black);
    }

    m_camera.update(1.0f / 60.0f);

	Physics::updatePhysX(dt);
	if (glfwGetKey(m_window, GLFW_KEY_F) != GLFW_PRESS)
	{
		m_IsPressed = false;
	}
    return true;
}

void Physics::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
    Gizmos::draw(m_camera.proj, m_camera.view);
	
    m_renderer->RenderAndClear(m_camera.view_proj);

    glfwSwapBuffers(m_window);
    glfwPollEvents();
}

void AddWidget(PxShape* shape, PxRigidActor* actor, vec4 geo_color)
{
    PxTransform full_transform = PxShapeExt::getGlobalPose(*shape, *actor);
    vec3 actor_position(full_transform.p.x, full_transform.p.y, full_transform.p.z);
    glm::quat actor_rotation(full_transform.q.w,
        full_transform.q.x,
        full_transform.q.y,
        full_transform.q.z);
    glm::mat4 rot(actor_rotation);

    mat4 rotate_matrix = glm::rotate(10.f, glm::vec3(7, 7, 7));

    PxGeometryType::Enum geo_type = shape->getGeometryType();

    switch (geo_type)
    {
    case (PxGeometryType::eBOX) :
    {
        PxBoxGeometry geo;
        shape->getBoxGeometry(geo);
        vec3 extents(geo.halfExtents.x, geo.halfExtents.y, geo.halfExtents.z);
        Gizmos::addAABBFilled(actor_position, extents, geo_color, &rot);
    } break;
    case (PxGeometryType::eCAPSULE) :
    {
        PxCapsuleGeometry geo;
        shape->getCapsuleGeometry(geo);
        Gizmos::addCapsule(actor_position, geo.halfHeight * 2, geo.radius, 16, 16, geo_color, &rot);
    } break;
    case (PxGeometryType::eSPHERE) :
    {
        PxSphereGeometry geo;
        shape->getSphereGeometry(geo);
        Gizmos::addSphereFilled(actor_position, geo.radius, 16, 16, geo_color, &rot);
    } break;
    case (PxGeometryType::ePLANE) :
    {

    } break;
    }
}

void Physics::renderGizmos(PxScene* physics_scene)
{
    PxActorTypeFlags desiredTypes = PxActorTypeFlag::eRIGID_STATIC | PxActorTypeFlag::eRIGID_DYNAMIC;
    PxU32 actor_count = physics_scene->getNbActors(desiredTypes);
    PxActor** actor_list = new PxActor*[actor_count];
	physics_scene->getActors(desiredTypes, actor_list, actor_count);
    
    vec4 geo_color(1, 0, 0, 1);
    for (int actor_index = 0;
        actor_index < (int)actor_count;
        ++actor_index)
    {
        PxActor* curr_actor = actor_list[actor_index];
        if (curr_actor->isRigidActor())
        {
            PxRigidActor* rigid_actor = (PxRigidActor*)curr_actor;
            PxU32 shape_count = rigid_actor->getNbShapes();
            PxShape** shapes = new PxShape*[shape_count];
            rigid_actor->getShapes(shapes, shape_count);

            for (int shape_index = 0;
                shape_index < (int)shape_count;
                ++shape_index)
            {
                PxShape* curr_shape = shapes[shape_index];
                AddWidget(curr_shape, rigid_actor, geo_color);
            }

            delete[]shapes;
        }
    }

    delete[] actor_list;

    int articulation_count = physics_scene->getNbArticulations();

    for (int a = 0; a < articulation_count; ++a)
    {
        PxArticulation* articulation;
		physics_scene->getArticulations(&articulation, 1, a);

        int link_count = articulation->getNbLinks();

        PxArticulationLink** links = new PxArticulationLink*[link_count];
        articulation->getLinks(links, link_count);

        for (int l = 0; l < link_count; ++l)
        {
            PxArticulationLink* link = links[l];
            int shape_count = link->getNbShapes();

            for (int s = 0; s < shape_count; ++s)
            {
                PxShape* shape;
                link->getShapes(&shape, 1, s);
                AddWidget(shape, link, geo_color);
            }
        }
        delete[] links;
    }
}

void Physics::CreateSphere(float _launchSpeed)
{
	if (glfwGetKey(m_window, GLFW_KEY_F) == GLFW_PRESS && !m_IsPressed)
	{
		SphereClass *_ball = new SphereClass(m_camera.GetPosition() + m_camera.GetForward(), m_camera.GetForward() * _launchSpeed, 3.0f, 1.0f, glm::vec4(1, 0, 0, 1));
		_ball->m_Radius = 1.0f;
		m_PhysScene->AddActor(_ball);
		m_IsPressed = true;
	}
}