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
	unsigned int _springRows = 40;
	unsigned int _springCols = 40;

	// top middle
	glm::vec3 _clothPos = glm::vec3(0, 12, 0);

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
	PxClothFabricCooker(_clothDesc, PxVec3(0, -9.81, 0), false).save(buf, cooked);
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

	PxCloth* cloth = g_physics->createCloth(PxTransform(PxVec3(0, 0, 0)),
		*fab, particles, PxClothFlag::eSWEPT_CONTACT);

	cloth->addCollisionPlane(PxClothCollisionPlane());

	if (cloth != nullptr)
	{
		PxClothStretchConfig bendCfg;
		bendCfg.stiffness = 1;
		bendCfg.stretchLimit = 1.0f;
		cloth->setStretchConfig(PxClothFabricPhaseType::eBENDING, bendCfg);
		cloth->setStretchConfig(PxClothFabricPhaseType::eVERTICAL, bendCfg);
		cloth->setStretchConfig(PxClothFabricPhaseType::eSHEARING, bendCfg);
		cloth->setStretchConfig(PxClothFabricPhaseType::eHORIZONTAL, bendCfg);
		cloth->setDampingCoefficient(PxVec3(0.125f));
	}

	delete[] particles;

	delete[] m_ClothPositions;
	delete[] indices;
	return cloth;







	//std::string vs = Resource::ImportShader("basicV");
	//const char* vsSource = vs.c_str();
	//std::string fs = Resource::ImportShader("basicF");
	//const char* fsSource = fs.c_str();
	//m_ShaderID = CreateProgram(vs, 0, 0, 0, fs);
	//
	//glDeleteShader(vs);
	//glDeleteShader(fs);
	//
	//m_TextureID = LoadTexture(".. / .. / Build / Textures / cloth.png");

}

void PhysXCloth::Render(glm::mat4 _camMatrix, glm::mat4 _projMatrix)
{
	// clear the backbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// get the view matrix from the world-space camera matrix
	glm::mat4 viewMatrix = glm::inverse(_camMatrix);
	// bind shader and transforms, along with texture
	glUseProgram(m_ShaderID);
	int location = glGetUniformLocation(m_ShaderID, "projectionView");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(_projMatrix * viewMatrix));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_TextureID);
	// update vertex positions from the cloth
	glBindBuffer(GL_ARRAY_BUFFER, m_ClothVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_ClothVertexCount * sizeof(glm::vec3),
		m_ClothPositions);
	// disable face culling so that we can draw it double-sided
	glDisable(GL_CULL_FACE);
	// bind and draw the cloth
	glBindVertexArray(m_ClothVAO);
	glDrawElements(GL_TRIANGLES, m_ClothIndexCount, GL_UNSIGNED_INT, 0);
	glEnable(GL_CULL_FACE);
	// draw the gizmos from this frame
	Gizmos::draw(viewMatrix, _projMatrix);
}
//
//bool LoadShaderType(char* filename, GLenum shader_type,	unsigned int* output)
//{
//	//we want to be able to return if we succeded
//	bool succeeded = true;
//
//	//open the shader file
//	FILE* shader_file = fopen(filename, "r");
//
//	//did it open successfully 
//	if (shader_file == 0)
//	{
//		succeeded = false;
//	}
//	else
//	{
//		//find out how long the file is
//		fseek(shader_file, 0, SEEK_END);
//		int shader_file_length = ftell(shader_file);
//		fseek(shader_file, 0, SEEK_SET);
//		//allocate enough space for the file
//		char *shader_source = new char[shader_file_length];
//		//read the file and update the length to be accurate
//		shader_file_length = fread(shader_source, 1, shader_file_length, shader_file);
//
//		//create the shader based on the type that got passed in
//		unsigned int shader_handle = glCreateShader(shader_type);
//		//compile the shader
//		glShaderSource(shader_handle, 1, &shader_source, &shader_file_length);
//		glCompileShader(shader_handle);
//
//		//chech the shader for errors
//		int success = GL_FALSE;
//		glGetShaderiv(shader_handle, GL_COMPILE_STATUS, &success);
//		if (success == GL_FALSE)
//		{
//			int log_length = 0;
//			glGetShaderiv(shader_handle, GL_INFO_LOG_LENGTH, &log_length);
//			char* log = new char[log_length];
//			glGetShaderInfoLog(shader_handle, log_length, NULL, log);
//			printf("%s\n", log);
//			delete[] log;
//			succeeded = false;
//		}
//		//only give the result to the caller if we succeeded
//		if (succeeded)
//		{
//			*output = shader_handle;
//		}
//
//		//clean up the stuff we allocated
//		delete[] shader_source;
//		fclose(shader_file);
//	}
//
//	return succeeded;
//}
//
//bool LoadShader(char* vertex_filename, char* geometry_filename,	char* fragment_filename, GLuint* result)
//{
//	bool succeeded = true;
//
//	*result = glCreateProgram();
//
//	unsigned int vertex_shader;
//
//	if (LoadShaderType(vertex_filename, GL_VERTEX_SHADER, &vertex_shader))
//	{
//		glAttachShader(*result, vertex_shader);
//		glDeleteShader(vertex_shader);
//	}
//	else
//	{
//		printf("FAILED TO LOAD VERTEX SHADER\n");
//	}
//
//	if (geometry_filename != nullptr)
//	{
//		unsigned int geometry_shader;
//		if (LoadShaderType(geometry_filename, GL_GEOMETRY_SHADER, &geometry_shader))
//		{
//			glAttachShader(*result, geometry_shader);
//			glDeleteShader(geometry_shader);
//		}
//		else
//		{
//			printf("FAILED TO LOAD GEOMETRY SHADER\n");
//		}
//	}
//	if (fragment_filename != nullptr)
//	{
//		unsigned int fragment_shader;
//		if (LoadShaderType(fragment_filename, GL_FRAGMENT_SHADER, &fragment_shader))
//		{
//			glAttachShader(*result, fragment_shader);
//			glDeleteShader(fragment_shader);
//		}
//		else
//		{
//			printf("FAILED TO LOAD FRAGMENT SHADER\n");
//		}
//	}
//
//	glLinkProgram(*result);
//
//	GLint success;
//	glGetProgramiv(*result, GL_LINK_STATUS, &success);
//	if (success == GL_FALSE)
//	{
//		GLint log_length;
//		glGetProgramiv(*result, GL_INFO_LOG_LENGTH, &log_length);
//		char* log = new char[log_length];
//		glGetProgramInfoLog(*result, log_length, 0, log);
//
//		printf("ERROR: STUFF DONE SCREWED UP IN UR SHADER BUDDY!\n\n");
//		printf("%s", log);
//
//		delete[] log;
//		succeeded = false;
//	}
//
//	return succeeded;
//}