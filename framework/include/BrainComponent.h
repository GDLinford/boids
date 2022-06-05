#ifndef BRAIN_COMPONENT_H
#define BRAIN_COMPONENT_H

#include "Component.h"

#include <glm.hpp>
#include <vector>
//fwd declares
class Entity;



class BrainComponent : public Component
{
public:
	BrainComponent(Entity* a_pEntity);

	virtual void Update(float a_fDeltaTime);
	virtual void Draw(Shader* pShader) {}

	glm::vec3 GetCurrentVelocity() const { return m_v3CurrentVelocity; }

	static float fSpeed;
	static float fNEIGHBOURHOOD_RADIUS;

	static float fJITTER;
	static float fWANDER_RADIUS;
	static float fCIRCLE_FORWARD_MULTIPLIER;

	static float SeparationWeighting;
	static float AlignmentWeighting;
	static float CohesionWeighting;
	static float WanderWeighting;


private:
	glm::vec3 CalculateSeekForce(const glm::vec3& v3Target, const glm::vec3& v3CurrentPos) const;
	glm::vec3 CalculateFleeForce(const glm::vec3& v3Target, const glm::vec3& v3CurrentPos) const;
	glm::vec3 CalculateWanderForce(const glm::vec3& v3Forward, const glm::vec3& v3CurrentPos);

	//Flocking Behaviours
	glm::vec3 CalculateSeperationForce();
	glm::vec3 CalculateAllignmentForce();
	glm::vec3 CalculateCohesionForce();

	//Variable
	glm::vec3 m_v3CurrentVelocity;
	glm::vec3 m_v3WanderPoint;

	void FindNearestNeighbours();
	std::vector<Entity*> m_neighbours;
};


#endif // !BRAIN_COMPONENT_H




