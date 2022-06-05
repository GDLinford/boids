#include "Entitty.h"

//typeDefs
typedef std::pair<const unsigned int, Entity*> EntityPair;

//Statics
unsigned int Entity::s_EntityCount = 0;
std::map<const unsigned int, Entity*> Entity::s_xEntityList;

Entity::Entity() 
{
	//Increment entity ID
	m_uEntityID = s_EntityCount++;

	//add this entity to our list
	s_xEntityList.insert(EntityPair(m_uEntityID, this));
}

void Entity::Update(float a_fDeltaTime) 
{
	std::vector<Component*>::iterator xIter;
	for (xIter = m_apComponentArray.begin(); xIter < m_apComponentArray.end(); ++xIter) 
	{
		Component* pComponent = *xIter;
		if (pComponent) 
		{
			pComponent->Update(a_fDeltaTime);
		}
	}
}

void Entity::Draw(Shader* a_pShader)
{
	std::vector<Component*>::iterator xIter;
	for (xIter = m_apComponentArray.begin(); xIter < m_apComponentArray.end(); ++xIter)
	{
		Component* pComponent = *xIter;
		if (pComponent)
		{
			pComponent->Draw(a_pShader);
		}
	}
}

void Entity::AddComponent(Component* a_pComponent)
{ 
	m_apComponentArray.push_back(a_pComponent);
}

Component* Entity::FindComponentOfType(COMPONENT_TYPE a_eComponentType) const 
{
	std::vector<Component*>::const_iterator xIter;
	for (xIter = m_apComponentArray.begin(); xIter < m_apComponentArray.end(); ++xIter)
	{
		Component* pComponent = *xIter;
		if (pComponent && pComponent->GetComponentType() == a_eComponentType)
		{
			return pComponent;
		}
	}

	return nullptr;
}
