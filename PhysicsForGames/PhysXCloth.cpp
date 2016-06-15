#include "PhysXCloth.h"


PhysXCloth::PhysXCloth()
{
}


PhysXCloth::~PhysXCloth()
{
}

PxCloth* PhysXCloth::SetupCloth(PxPhysics* g_physics)
{
	float _springSize = 0.2f;
	unsigned int _springRows = 128;
	unsigned int _springCols = 72;

	// top middle
	glm::vec3 _clothPos = glm::vec3(-10, 20, -10);

	float _halfWidth = _springRows * _springSize * 0.5f;

	m_ClothVertexCount = _springRows * _springCols;
	m_ClothPositions = new glm::vec3[m_ClothVertexCount];
	glm::vec2* _clothTexCoords = new glm::vec2[m_ClothVertexCount];

	for (unsigned int r = 0; r < _springRows; ++r)
	{
		for (unsigned int c = 0; c < _springCols; ++c)
		{
			m_ClothPositions[r * _springCols + c].x = _clothPos.x + (_springSize * c);
			m_ClothPositions[r * _springCols + c].y = _clothPos.y;
			m_ClothPositions[r * _springCols + c].z = _clothPos.z + _springSize * (r - _halfWidth);
			_clothTexCoords[r * _springCols + c].x = 1.0f - r / (_springRows - 1.0f);
			_clothTexCoords[r * _springCols + c].y = 1.0f - c / (_springCols - 1.0f);
		}
	}

	// Set up index grid
	m_ClothIndexCount = (_springRows - 1) * (_springCols - 1) * 2 * 3;
	unsigned int* indices = new unsigned int[m_ClothIndexCount];
	unsigned int* index = indices;
	for (unsigned int r = 0; r < (_springRows - 1); ++r)
	{
		for (unsigned int c = 0; c < (_springCols - 1); ++c)
		{
			// 4 corner quad verts
			unsigned int i0 = r * _springCols + c;
			unsigned int i1 = i0 + 1;
			unsigned int i2 = i0 + _springCols;
			unsigned int i3 = i2 + 1;
			// Invert every second tri
			if ((c + r) % 2)
			{
				*index++ = i0; *index++ = i2; *index++ = i1;
				*index++ = i1; *index++ = i2; *index++ = i3;
			}
			else
			{
				*index++ = i0; *index++ = i2; *index++ = i3;
				*index++ = i0; *index++ = i3; *index++ = i1;
			}
		}
	}

	unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);

	std::string vsSource = Resource::ImportShader("basicV");
	std::string fsSource = Resource::ImportShader("basicF");

	const GLchar* _vsCode = vsSource.c_str();
	const GLchar* _fsCode = fsSource.c_str();

	glShaderSource(vs, 1, &_vsCode, 0);
	glCompileShader(vs);

	glShaderSource(fs, 1, &_fsCode, 0);
	glCompileShader(fs);

	m_ShaderID = glCreateProgram();
	glAttachShader(m_ShaderID, vs);
	glAttachShader(m_ShaderID, fs);
	glBindAttribLocation(m_ShaderID, 0, "position");
	glBindAttribLocation(m_ShaderID, 1, "texCoords");
	glLinkProgram(m_ShaderID);

	int success = GL_FALSE;
	glGetProgramiv(m_ShaderID, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetShaderiv(m_ShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];

		glGetShaderInfoLog(m_ShaderID, infoLogLength, 0, infoLog);
		printf("Error: Failed to link Gizmo shader program!\n%s\n", infoLog);
		delete[] infoLog;
	}

	glDeleteShader(vs);
	glDeleteShader(fs);


	glGenBuffers(1, &m_ClothVBO);
	glGenBuffers(1, &m_ClothTextureVBO);
	glGenBuffers(1, &m_ClothIBO);

	glGenVertexArrays(1, &m_ClothVAO);
	glBindVertexArray(m_ClothVAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_ClothVBO);
	glBufferData(GL_ARRAY_BUFFER, m_ClothVertexCount * sizeof(glm::vec3), m_ClothPositions, GL_DYNAMIC_DRAW);


	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_ClothTextureVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * m_ClothVertexCount, _clothTexCoords, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ClothIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_ClothIndexCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), 0);


	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	m_TextureID = LoadGLTextureBasic("./data/stalin.jpg");

	// set up the cloth description
	PxClothMeshDesc _clothDesc;
	_clothDesc.setToDefault();
	_clothDesc.points.count = m_ClothVertexCount;
	_clothDesc.triangles.count = m_ClothIndexCount / 3;
	_clothDesc.points.stride = sizeof(glm::vec3);
	_clothDesc.triangles.stride = sizeof(unsigned int) * 3;
	_clothDesc.points.data = m_ClothPositions;
	_clothDesc.triangles.data = indices;

	PxDefaultMemoryOutputStream buf;
	bool cooked = false;
	PxClothFabricCooker cooker(_clothDesc, PxVec3(0, -1, 0), false);
	cooker.save(buf, cooked);
	if (cooked)
		return nullptr;

	PxDefaultMemoryInputData data = PxDefaultMemoryInputData(buf.getData(), buf.getSize());
	PxClothFabric* fab = g_physics->createClothFabric(data);
	PxClothParticle* particles = new PxClothParticle[m_ClothVertexCount];

	for (unsigned int i = 0; i < m_ClothVertexCount; ++i)
	{
		particles[i].pos = PxVec3(m_ClothPositions[i].x, m_ClothPositions[i].y, m_ClothPositions[i].z);
		if (m_ClothPositions[i].x == _clothPos.x)
			particles[i].invWeight = 0;
		else
			particles[i].invWeight = 1.0f;

	}

	m_cloth = g_physics->createCloth(PxTransform(PxVec3(0, 0, 0)),
		*fab, particles, PxClothFlag::eSWEPT_CONTACT);


	if (m_cloth != nullptr)
	{
		PxClothStretchConfig bendCfg;
		bendCfg.stiffness = 1;
		bendCfg.stretchLimit = 1.0f;
		m_cloth->setStretchConfig(PxClothFabricPhaseType::eBENDING, bendCfg);
		m_cloth->setStretchConfig(PxClothFabricPhaseType::eVERTICAL, bendCfg);
		m_cloth->setStretchConfig(PxClothFabricPhaseType::eSHEARING, bendCfg);
		m_cloth->setStretchConfig(PxClothFabricPhaseType::eHORIZONTAL, bendCfg);
		m_cloth->setDampingCoefficient(PxVec3(0.125f));
	}

	m_cloth->setClothFlag(PxClothFlag::eSCENE_COLLISION, true);
	delete[] particles;
	delete[] _clothTexCoords;
	delete[] indices;
	return m_cloth;
}

void PhysXCloth::Render(glm::mat4 _camMatrix, glm::mat4 _projMatrix)
{
	// clear the backbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// get the view matrix from the world-space camera matrix
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// bind shader and transforms, along with texture
	glUseProgram(m_ShaderID);
	int location = glGetUniformLocation(m_ShaderID, "projectionView");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(_projMatrix * _camMatrix));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_TextureID);

	PxClothParticleData* data = m_cloth->lockParticleData(PxDataAccessFlag::eREADABLE);
	for (PxU32 i = 0, n = m_cloth->getNbParticles(); i < n; ++i)
	{
		PxVec3 pos = data->particles[i].pos;
		m_ClothPositions[i] = glm::vec3(pos.x, pos.y, pos.z);

	}
	data->unlock();

	// update vertex positions from the cloth
	glBindBuffer(GL_ARRAY_BUFFER, m_ClothVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_ClothVertexCount * sizeof(glm::vec3), m_ClothPositions);
	// disable face culling so that we can draw it double-sided
	glDisable(GL_CULL_FACE);
	// bind and draw the cloth
	glBindVertexArray(m_ClothVAO);
	glDrawElements(GL_TRIANGLES, m_ClothIndexCount, GL_UNSIGNED_INT, 0);
	glEnable(GL_CULL_FACE);
	glUseProgram(0);
}