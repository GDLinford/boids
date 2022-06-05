#ifndef MODEL_COMPONENT_H
#define MODEL_COMPONENT_H

//parent class
#include "Component.h"

//fwd Declares
class Model;

class modelComponent : public Component
{
public:
	modelComponent(Entity* a_pOwner);

	virtual void Update(float a_fDeltaTime) {};
	virtual void Draw(Shader* a_pShader);

	void SetModel(Model* pNewModel) { m_pModelData = pNewModel; }
	void SetScale(float a_fNewScale) { m_fModelScale = a_fNewScale; }

private:
	Model* m_pModelData;
	float m_fModelScale;
};



#endif // !MODEL_COMPONENT_H
