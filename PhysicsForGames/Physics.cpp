#include "Physics.h"

#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include "Gizmos.h"

#include "glm/ext.hpp"
#include "glm/gtc/quaternion.hpp"

#define Assert(val) if (val){}else{ *((char*)0) = 0;}
#define ArrayCount(val) (sizeof(val)/sizeof(val[0]))

void SetShapeAsTrigger(PxRigidActor* actorIn)
{
	PxRigidStatic* staticActor = actorIn->is<PxRigidStatic>();
	assert(staticActor);
	const PxU32 numShapes = staticActor->getNbShapes();
	PxShape** shapes = (PxShape**)_aligned_malloc(sizeof(PxShape*)*numShapes, 16);
	staticActor->getShapes(shapes, numShapes);
	for (PxU32 i = 0; i < numShapes; i++)
	{
		shapes[i]->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
		shapes[i]->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
	}
}

void SetupFiltering(PxRigidActor* actor, PxU32 filterGroup, PxU32 filterMask)
{
	PxFilterData filterData;
	filterData.word0 = filterGroup; // word0 = own ID
	filterData.word1 = filterMask; // word1 = ID mask to filter pairs that trigger a contact callback;
	const PxU32 numShapes = actor->getNbShapes();
	PxShape** shapes = (PxShape**)_aligned_malloc(sizeof(PxShape*)*numShapes, 16);
	actor->getShapes(shapes, numShapes);
	for (PxU32 i = 0; i < numShapes; i++)
	{
		PxShape* shape = shapes[i];
		shape->setSimulationFilterData(filterData);
	}
	_aligned_free(shapes);
}PxFilterFlags FilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	// let triggers through
	if (PxFilterObjectIsTrigger(attributes0) ||
		PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}
	// generate contacts for all that were not filtered above
	pairFlags = PxPairFlag::eCONTACT_DEFAULT;
	// trigger the contact callback for pairs (A,B) where
	// the filtermask of A contains the ID of B and vice versa.
	if ((filterData0.word0 & filterData1.word1) &&
		(filterData1.word0 & filterData0.word1))
		pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND |
		PxPairFlag::eNOTIFY_TOUCH_LOST;
	return PxFilterFlag::eDEFAULT;
}

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
	//Physics::setUpVisualDebugger();
	Physics::addPlane();
	Physics::addBox();
	Physics::addCapsule();


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

	Ragdoll* _ragdoll = new Ragdoll();
	m_Ragdoll = _ragdoll->MakeRagdoll(g_Physics, _ragdoll->ragdollData, PxTransform(PxVec3(0, 20, 0)), .1f, g_PhysicsMaterial);
	g_PhysicsScene->addArticulation(*m_Ragdoll);
	m_Ragdoll->putToSleep();

	MakeCharController();


	m_MyCloth = new PhysXCloth();
	m_Cloth = m_MyCloth->SetupCloth(g_Physics);
	g_PhysicsScene->addActor(*m_Cloth);


	m_Callback = new CollisionCallbacks();
	g_PhysicsScene->setSimulationEventCallback(m_Callback);

	//m_Cloth = CreateCloth(glm::vec3(0, 20, 0), 10, indicies);
	//g_PhysicsScene->addActor(*m_Cloth);
	//delete[] clothTexCoords;
	//delete[] indicies;
}

void Physics::updatePhysX(float _deltaTime)
{
	if (_deltaTime <= 0)
		return;

	g_PhysicsScene->simulate(_deltaTime);

	if (glfwGetKey(m_window, GLFW_KEY_V) == GLFW_PRESS)
	{
		PxSphereGeometry _sphere(1.5f);
		float _density = 0.5;
		PxReal _mass = 10.0f;
		PxTransform _trans (PxVec3(m_camera.GetPosition().x, m_camera.GetPosition().y, m_camera.GetPosition().z));

		PxRigidDynamic* _actor = PxCreateDynamic(*g_Physics, _trans, _sphere, *g_PhysicsMaterial, _density);
		_actor->setMass(_mass);
		glm::vec3 _dir(-m_camera.world[2]);
		PxVec3 _vel = PxVec3(_dir.x, _dir.y, _dir.z) * 2;
		_actor->setLinearVelocity(_vel, true);
		g_PhysicsScene->addActor(*_actor);
	}

	if (m_Callback->IsTriggered() && m_Ragdoll->isSleeping())
		m_Ragdoll->wakeUp();
	else if(!m_Callback->IsTriggered() && !m_Ragdoll->isSleeping())
		m_Ragdoll->putToSleep();


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
	PxTransform transform(PxVec3(10, 0, 10));
	PxRigidDynamic* dynamicActor = PxCreateDynamic(*g_Physics, transform, box, *g_PhysicsMaterial, density);
	g_PhysicsScene->addActor(*dynamicActor);

	PxRigidStatic* _staticActor = PxCreateStatic(*g_Physics, PxTransform(PxVec3(0, 2, 0)), box, *g_PhysicsMaterial);
	g_PhysicsScene->addActor(*_staticActor);

	PxRigidStatic* _staticActor1 = PxCreateStatic(*g_Physics, PxTransform(PxVec3(-10, 2, -10)), box, *g_PhysicsMaterial);
	SetShapeAsTrigger(_staticActor1);
	SetupFiltering(_staticActor1, FilterGroup::eGROUND, FilterGroup::ePLAYER);
	g_PhysicsScene->addActor(*_staticActor1);
} // PhysX

void Physics::addCapsule()
{
	PxCapsuleGeometry _caps(0.5f, 2.0f);

	PxTransform _trans(PxVec3(10, 10, 0));

	PxMat44 _m(_trans);
	glm::mat4* _M = (glm::mat4*)(&_m);

	glm::vec3 _position = glm::vec3(_trans.p.x, _trans.p.y, _trans.p.z);
	glm::vec4 _axis(2.5f, 0, 0, 0);
	_axis = (*_M) * _axis;

	Gizmos::addSphere(_position + _axis.xyz(), 1.5f, 10, 10, glm::vec4(0, 1, 0, 1));
	Gizmos::addSphere(_position - _axis.xyz(), 1.5f, 10, 10, glm::vec4(0, 1, 0, 1));

	glm::mat4 m2 = glm::rotate(*_M, 11 / 7.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	Gizmos::addCylinderFilled(_position, 1.5f, 2.5f, 10, glm::vec4(0, 1, 0, 1), &m2);


	float _density = 10;

	PxRigidDynamic* _actor = PxCreateDynamic(*g_Physics, _trans, _caps, *g_PhysicsMaterial, _density);
	glm::vec3 _dir(-m_camera.world[2]);
	PxVec3 _vel = PxVec3(_dir.x, _dir.y, _dir.z) * 2;
	g_PhysicsScene->addActor(*_actor);
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
	CreateBox(10.0f);

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
	UpdateChar(dt);
	renderGizmos(g_PhysicsScene);
	if (glfwGetKey(m_window, GLFW_KEY_F) != GLFW_PRESS && glfwGetKey(m_window, GLFW_KEY_G) != GLFW_PRESS)
	{
		m_IsPressed = false;
	}
    return true;
}

void Physics::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	m_MyCloth->Render(m_camera.view, m_camera.proj);
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
    
    vec4 geo_color(0, 1, 0, 1);
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
		m_Thrown++;
		DIYRigidBody* _rigidbody = new DIYRigidBody(m_camera.GetForward() * _launchSpeed, glm::quat(), 1.0f);
		SphereClass *_ball = new SphereClass(m_camera.GetPosition() + m_camera.GetForward(),
			_rigidbody, 1.0f, glm::vec4(1, 0, 0, 1));

		_ball->m_Radius = 1.0f;
		m_PhysScene->AddActor(_ball);

		if(m_Thrown == 1)
			m_Spheres[0] = _ball;
		if (m_Thrown == 2)
		{
			m_Spheres[1] = _ball;
			CreateJoint(m_Spheres[0], m_Spheres[1]);
		}
		if (m_Thrown == 3)
		{
			m_Spheres[2] = _ball;
			CreateJoint(m_Spheres[1], m_Spheres[2]);
		}
		if (m_Thrown == 4)
		{
			m_Spheres[3] = _ball;
			CreateJoint(m_Spheres[2], m_Spheres[3]);
		}
		if (m_Thrown == 5)
		{
			m_Spheres[4] = _ball;
			CreateJoint(m_Spheres[3], m_Spheres[4]);
			CreateJoint(m_Spheres[0], m_Spheres[4]);
			m_Thrown = 0;
		}


		m_IsPressed = true;
	}
}

void Physics::CreateBox(float _launchSpeed)
{
	if (glfwGetKey(m_window, GLFW_KEY_G) == GLFW_PRESS && !m_IsPressed)
	{
		DIYRigidBody* _rigidbody = new DIYRigidBody(m_camera.GetForward() * _launchSpeed, glm::quat(), 1.0f);
		BoxClass *_box = new BoxClass(m_camera.GetPosition() + m_camera.GetForward(), glm::vec3(1), _rigidbody);

		//_box->m_Extents = glm::vec3(1);
		m_PhysScene->AddActor(_box);
		m_IsPressed = true;
	}
}

void Physics::CreateJoint(PhysicsObject* _obj1, PhysicsObject* _obj2)
{
	DIYRigidBody* _rb = new DIYRigidBody(_obj1->GetVelocity(), glm::quat(), 1.0f);
	SpringJoint *_joint = new SpringJoint(_obj1, _obj2, 20.0f, 5.0f, 0.25f, _obj1->GetPosition(), _rb);
	m_PhysScene->AddActor(_joint);
}

void Physics::MakeCharController()
{
	m_HitReport = new MyControllerHitReport();

	g_CharManager = PxCreateControllerManager(*g_PhysicsScene);

	PxCapsuleControllerDesc _desc;
	_desc.height = 1.6f;
	_desc.radius = 0.4f;
	_desc.position.set(0, 0, 0);
	_desc.material = g_PhysicsMaterial;
	_desc.reportCallback = m_HitReport; //connect it to our collision detection routine
	_desc.density = 10;

	g_PlayerController = g_CharManager->createController(_desc);
	g_PlayerController->setPosition(PxExtendedVec3(5,2,5));
	m_YVel = 0;
	m_Rotation = 0;
	m_Grav = -0.5f;
	m_HitReport->clearPlayerContactNormal();
	SetupFiltering(g_PlayerController->getActor(), FilterGroup::ePLAYER, FilterGroup::eGROUND);
	g_PhysicsScene->addActor(*g_PlayerController->getActor());
}

void Physics::UpdateChar(float _deltaTime)
{
	if (m_HitReport->getPlayerContactNormal().y > 0.3f)
	{
		m_YVel = -0.1f;
		m_HitReport->m_Grounded = true;
	}
	else
	{
		m_YVel += -0.5f * _deltaTime;
		m_HitReport->m_Grounded = false;
	}

	m_HitReport->clearPlayerContactNormal();
	const PxVec3 up(0, 1, 0);
	//scan the keys and set up our intended velocity based on a global transform
	PxVec3 velocity(0, m_YVel, 0);
	if (glfwGetKey(m_window, GLFW_KEY_UP) == GLFW_PRESS)
		velocity.x -= m_MovSpeed * _deltaTime;
	if (glfwGetKey(m_window, GLFW_KEY_DOWN) == GLFW_PRESS)
		velocity.x += m_MovSpeed * _deltaTime;
	if (glfwGetKey(m_window, GLFW_KEY_LEFT) == GLFW_PRESS)
		velocity.z += m_MovSpeed * _deltaTime;
	if (glfwGetKey(m_window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		velocity.z -= m_MovSpeed * _deltaTime;
	if (glfwGetKey(m_window, GLFW_KEY_SPACE))
		velocity.y += (m_MovSpeed * 2) * _deltaTime;

	float minDistance = 0.001f;
	PxControllerFilters filter;
	//make controls relative to player facing
	PxQuat rotation(m_Rotation, PxVec3(0, 1, 0));
	//move the controller
	g_PlayerController->move(rotation.rotate(velocity), minDistance, _deltaTime, filter);
}