//files header
#include "modelComponent.h"

//other project includes
#include "TransformComponent.h"
#include "Entitty.h"

//learnOpenGL includes
#include <learnopengl/shader.h>
#include <learnopengl/model.h>

//glm Includes
#include <gtc/matrix_transform.hpp>

//typedefs
typedef Component PARENT;

modelComponent::modelComponent(Entity* a_pOwnerEntity) 
	: PARENT(a_pOwnerEntity)
{
	m_pModelData = nullptr;
	m_fModelScale = 0.0f;
}

void modelComponent::Draw(Shader* a_pShader) 
{

    //null checks
    if (!a_pShader) 
    {
        return; //early out
    }

    if (!m_pModelData) 
    {
        return;
    }

    //get the transform
    TransformComponent* pTransformComponent = static_cast<TransformComponent*>(m_pOwnerEntity->FindComponentOfType(TRANSFORM));

    if (!pTransformComponent) 
    {
        return; //Early out
    }

    // render the loaded model
    glm::mat4 m4ModelMatrix = pTransformComponent->GetEntityMatrix();
    m4ModelMatrix = glm::scale(m4ModelMatrix, glm::vec3(m_fModelScale, m_fModelScale, m_fModelScale));
    a_pShader->setMat4("model", m4ModelMatrix);
    m_pModelData->Draw(*a_pShader);
}