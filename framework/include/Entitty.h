#ifndef ENTITY_H
#define ENTITY_H

//project includes
#include "Component.h"

//FWD Declarations
class Shader;
class Component;

//std includes
#include <vector>
#include <map>

class Entity
{
public:
	Entity();
	virtual void Update(float a_fDeltaTime);
	virtual void Draw(Shader* a_pShader);

	const int GetEntityId() { return m_uEntityID; };

	void AddComponent(Component* a_pComponent);
	Component* FindComponentOfType(COMPONENT_TYPE a_eComponentType) const;

	static std::map<const unsigned int, Entity*>& GetEntityMap() { return s_xEntityList; }



private:
	unsigned m_uEntityID;
	std::vector<Component*> m_apComponentArray;

	static unsigned int s_EntityCount;
	static std::map<const unsigned int, Entity*> s_xEntityList;

};

#endif //!ENTITY_H