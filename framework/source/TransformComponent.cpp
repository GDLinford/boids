//this file header
#include "TransformComponent.h"

//typedefs
typedef Component PARENT;

TransformComponent::TransformComponent(Entity* pOwner)
	: PARENT(pOwner)
	, m_m4EntityMatrix(glm::mat4(1.0f))
{
	m_eComponentType = TRANSFORM;
}

void TransformComponent::SetEntityMatrixRow(MATRIX_ROW a_eRow, glm::vec3 a_v3Vec) 
{
	m_m4EntityMatrix[a_eRow] = glm::vec4(a_v3Vec, (a_eRow == POSITION_VECTOR ? 1.f : 0.0f));
}
glm::vec3 TransformComponent::GetEntityMatrixRow(MATRIX_ROW a_eRow) 
{
	return m_m4EntityMatrix[a_eRow];
}