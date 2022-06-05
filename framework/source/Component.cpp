//the header
#include "Component.h"

//Project Include
#include "Entitty.h"

Component::Component(Entity* a_pOwner)
{
	m_pOwnerEntity = a_pOwner;
	m_eComponentType = NONE;
}

