#include "PhysXCloth.h"



PhysXCloth::PhysXCloth()
{
}


PhysXCloth::~PhysXCloth()
{
}

void PhysXCloth::SetupCloth()
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

	//unsigned int vs = Utility::LoadShader("../../Build/shaders/basic.vert", GL_VERTEX_SHADER);
	//unsigned int fs = Utility::LoadShader("../../Build/shaders/basic.frag", GL_FRAGMENT_SHADER);
	//m_ShaderID = Utility::CreateProgram(vs, 0, 0, 0, fs);
	//glDeleteShader(vs);
	//glDeleteShader(fs);
	//m_TextureID = Utiity::LoadTexture(".. / .. / Build / Textures / cloth.png");

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

