#ifndef COMPONENT_H
#define COMPONENT_H

//FWD Declarations
class Entity;
class Shader;

enum COMPONENT_TYPE 
{
	NONE,
	TRANSFORM,
	MODEL,
	BRAIN,
};

class Component
{
public:
	Component(Entity* a_pOwner);
	
	virtual void Update(float a_fDeltaTime) = 0;
	virtual void Draw(Shader* pShader) = 0;

	inline Entity* GetOwnerEntity() { return m_pOwnerEntity; }
	inline COMPONENT_TYPE GetComponentType() { return m_eComponentType; }

protected:
	Entity* m_pOwnerEntity;
	COMPONENT_TYPE m_eComponentType;
};

#endif //!COMPONENT_H

