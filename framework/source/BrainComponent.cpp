//files header
#include "BrainComponent.h"

//project includes
#include "Entitty.h"
#include "TransformComponent.h"

//imgui include
#include <imgui/imgui.h>
#include <imgui/impl/imgui_impl_glfw.h>
#include <imgui/impl/imgui_impl_opengl3.h>
#include <queue>

//constants
float BrainComponent::fSpeed = 2.0f;
float BrainComponent::fNEIGHBOURHOOD_RADIUS = 50.0f;

//wander constants
float BrainComponent::fJITTER = 0.5f;
float BrainComponent::fWANDER_RADIUS = 4.0f;
float BrainComponent::fCIRCLE_FORWARD_MULTIPLIER = 10.0f;

//weighted forces
float BrainComponent::SeparationWeighting = 0.5f;
float BrainComponent::AlignmentWeighting = 2.f;
float BrainComponent::CohesionWeighting = 3.f;
float BrainComponent::WanderWeighting = 0.6f;

BrainComponent::BrainComponent(Entity* a_pOwner)
	: Component(a_pOwner)
	, m_v3CurrentVelocity(0.0f)
	, m_v3WanderPoint(0.0f)
	
{
	m_eComponentType = BRAIN;
	m_neighbours.clear();
}

void BrainComponent::Update(float a_fDeltaTime) 
{
	//Get Entity owner
	Entity* pEntity = GetOwnerEntity();
	if (!pEntity) 
	{
		return; //early out
	}

	//Get Transform component
	TransformComponent* pTransComp = static_cast<TransformComponent*>(pEntity->FindComponentOfType(TRANSFORM));
	if (!pTransComp) 
	{
		return; // early out
	}


	//Get the vector we need for calculations
	glm::vec3 v3Forward = pTransComp->GetEntityMatrixRow(FORWARD_VECTOR);
	glm::vec3 v3CurrentPos = pTransComp->GetEntityMatrixRow(POSITION_VECTOR);

	//Find Neighbours
	m_neighbours.clear();
	FindNearestNeighbours();
	//if you want to print out the size of m_neighbours
	//calculate force
	glm::vec3 v3FinalForce(0.0f);

	glm::vec3 v3SeperationForce = CalculateSeperationForce() * SeparationWeighting;
	glm::vec3 v3AllignmentForce = CalculateAllignmentForce() * AlignmentWeighting;
	glm::vec3 v3CohesionForce = CalculateCohesionForce() * CohesionWeighting;

	
	glm::vec3 v3WanderForce = CalculateWanderForce(v3Forward, v3CurrentPos) * WanderWeighting;

	//v3FinalForce = v3AllignmentForce;
	v3FinalForce = v3WanderForce + v3CohesionForce + v3AllignmentForce + v3SeperationForce;

	//Apply force
	m_v3CurrentVelocity += v3FinalForce;

	//clamp velocity
	m_v3CurrentVelocity = glm::clamp(m_v3CurrentVelocity, glm::vec3(-10.0f, -10.0f, -10.0f), glm::vec3(10.0f, 10.0f, 10.0f));

	//Apply our velocity to our position
	v3CurrentPos += m_v3CurrentVelocity * a_fDeltaTime;

	if (glm::length(v3CurrentPos) > fNEIGHBOURHOOD_RADIUS) 
	{
		v3CurrentPos = glm::normalize(v3CurrentPos);
		
	}

	//get new forward and normalise
	v3Forward = m_v3CurrentVelocity;
	if (glm::length(v3Forward) > 0.0f) 
	{
		v3Forward = glm::normalize(v3Forward);
	}

	//up and right
	glm::vec3 v3Up = pTransComp->GetEntityMatrixRow(UP_VECTOR);
	glm::vec3 v3Right = glm::cross(v3Up, v3Forward);

	//update our matrix
	pTransComp->SetEntityMatrixRow(RIGHT_VECTOR, v3Right);
	pTransComp->SetEntityMatrixRow(FORWARD_VECTOR, v3Forward);
	pTransComp->SetEntityMatrixRow(POSITION_VECTOR, v3CurrentPos);

}

void BrainComponent::FindNearestNeighbours()
{
	//Entities transform
	TransformComponent* pLocalTransform = static_cast<TransformComponent*>(GetOwnerEntity()->FindComponentOfType(TRANSFORM));

	//this entities position
	glm::vec3 v3LocalPosition = pLocalTransform->GetEntityMatrixRow(POSITION_VECTOR);
	//create an iterator
	const std::map<const unsigned int, Entity*>& xEntityMap = Entity::GetEntityMap();
	std::map<const unsigned int, Entity*>::const_iterator xConstIter;

	//loop all over entities in scene
	for (xConstIter = xEntityMap.begin(); xConstIter != xEntityMap.end(); ++xConstIter)
	{
		Entity* pTarget = xConstIter->second;
		if (pTarget->GetEntityId() != GetOwnerEntity()->GetEntityId())
		{
			TransformComponent* pTargetTransform = static_cast<TransformComponent*>(pTarget->FindComponentOfType(TRANSFORM));

			//find distance
			glm::vec3 v3TargetPos = pTargetTransform->GetEntityMatrixRow(POSITION_VECTOR);
			float fDistanceBetween = glm::length(v3TargetPos - v3LocalPosition);

			//check distance between is within our neighbourhood
			if (fDistanceBetween < fNEIGHBOURHOOD_RADIUS)
			{
				m_neighbours.push_back(pTarget);
			}
		}
	}
}

glm::vec3 BrainComponent::CalculateSeekForce(const glm::vec3& v3Target, const glm::vec3& v3CurrentPos) const 
{
	//calculate target direction
	glm::vec3 v3TargetDir(v3Target - v3CurrentPos);
	if (glm::length(v3TargetDir) > 0.0f) 
	{
		v3TargetDir = glm::normalize(v3TargetDir);
	}

	//calculate new velocity
	glm::vec3 v3Newelocity = v3TargetDir * fSpeed;

	return (v3Newelocity - m_v3CurrentVelocity);
}

glm::vec3 BrainComponent::CalculateFleeForce(const glm::vec3& v3Target, const glm::vec3& v3CurrentPos) const
{
	//calculate target direction
	glm::vec3 v3TargetDir(v3CurrentPos - v3Target);
	if (glm::length(v3TargetDir) > 0.0f)
	{
		v3TargetDir = glm::normalize(v3TargetDir);
	}

	//calculate new velocity
	glm::vec3 v3Newelocity = v3TargetDir * fSpeed;

	return (v3Newelocity - m_v3CurrentVelocity);
}

glm::vec3 BrainComponent::CalculateWanderForce(const glm::vec3& v3Forward, const glm::vec3& v3CurrentPos)
{
	//project a point in front us for the centre of our sphere
	glm::vec3 v3SphereOrigin = v3CurrentPos + (v3Forward * fCIRCLE_FORWARD_MULTIPLIER);

	if (glm::length(m_v3WanderPoint) == 0.0f)
	{
		//Find a random point on sphere
		//fCircle_Forward is basically where the wander sphere spawns
		//fWander_Radius is the size of that sphere
		glm::vec3 v3RandomPointOnSphere = glm::sphericalRand(fWANDER_RADIUS);

		//Add the random point to our sphere origin
		m_v3WanderPoint = v3SphereOrigin + v3RandomPointOnSphere;
	}

	//calculate direction to walk too
	glm::vec3 v3DirectionToTarget = glm::normalize(m_v3WanderPoint - v3SphereOrigin) * fWANDER_RADIUS;
	
	//glm::vec3 v3DirectionToTarget = glm::normalize(m_v3WanderPoint - v3CurrentPos) * fWANDER_RADIUS;

	//find out final target point
	m_v3WanderPoint = v3SphereOrigin + v3DirectionToTarget;

	//Add jitter vector
	m_v3WanderPoint += glm::sphericalRand(fJITTER);

	return CalculateSeekForce(m_v3WanderPoint, v3CurrentPos);
}

glm::vec3 BrainComponent::CalculateSeperationForce() 
{
	//force that keeps agents apart
	//final seperation vector

	glm::vec3 v3SeperationForce(0.0f);
	unsigned int uNeighbourCount = 0;

	//Entities transform
	TransformComponent* pLocalTransform = static_cast<TransformComponent*>(GetOwnerEntity()->FindComponentOfType(TRANSFORM));

	//this entities position
	glm::vec3 v3LocalPosition = pLocalTransform->GetEntityMatrixRow(POSITION_VECTOR);

	//loop all over neighbours
	for (auto iter = m_neighbours.begin(); iter != m_neighbours.end();  ++iter)
	{
		Entity* pTarget = (*iter);
		
		TransformComponent* pTargetTransform = static_cast<TransformComponent*>(pTarget->FindComponentOfType(TRANSFORM));

		//find distance
		glm::vec3 v3TargetPos = pTargetTransform->GetEntityMatrixRow(POSITION_VECTOR);
		v3SeperationForce += (v3LocalPosition - v3TargetPos);
	}

	if (glm::length(v3SeperationForce) > 0.0f) 
	{
		v3SeperationForce /= m_neighbours.size();
		v3SeperationForce = glm::normalize(v3SeperationForce);
	}
	return v3SeperationForce;
}

glm::vec3 BrainComponent::CalculateAllignmentForce() 
{
	//final allignment force
	glm::vec3 v3AllignmentForce(0.0f);
	unsigned int uNeighbourCount = 0;

	//Entities transform
	TransformComponent* pLocalTransform = static_cast<TransformComponent*>(GetOwnerEntity()->FindComponentOfType(TRANSFORM));

	//this entities position
	glm::vec3 v3LocalPosition = pLocalTransform->GetEntityMatrixRow(POSITION_VECTOR);

	//loop all over neighbours
	for (auto iter = m_neighbours.begin(); iter != m_neighbours.end(); ++iter)
	{
		Entity* pTarget = (*iter);
		BrainComponent* pTargetBrain = static_cast<BrainComponent*>(pTarget->FindComponentOfType(BRAIN));
		v3AllignmentForce += pTargetBrain->GetCurrentVelocity();		
	}

	if (glm::length(v3AllignmentForce) > 0.0f) 
	{
		v3AllignmentForce /= m_neighbours.size();
		v3AllignmentForce = glm::normalize(v3AllignmentForce);
	}


	return v3AllignmentForce;
}

glm::vec3 BrainComponent::CalculateCohesionForce() 
{
	
	glm::vec3 v3CohesionForce(0.0f);
	unsigned int uNeighbourCount = 0;

	//Entities transform
	TransformComponent* pLocalTransform = static_cast<TransformComponent*>(GetOwnerEntity()->FindComponentOfType(TRANSFORM));

	//this entities position
	glm::vec3 v3LocalPosition = pLocalTransform->GetEntityMatrixRow(POSITION_VECTOR);

	//loop all over neighbours
	for (auto iter = m_neighbours.begin(); iter != m_neighbours.end(); ++iter)
	{
		Entity* pTarget = (*iter);

		TransformComponent* pTargetTransform = static_cast<TransformComponent*>(pTarget->FindComponentOfType(TRANSFORM));

		//find distance
		glm::vec3 v3TargetPos = pTargetTransform->GetEntityMatrixRow(POSITION_VECTOR);
		v3CohesionForce += v3TargetPos;
	}

	//Check length is zero before normalize

	if (glm::length(v3CohesionForce) > 0.0f) 
	{
		v3CohesionForce /= m_neighbours.size();
		v3CohesionForce = glm::normalize(v3CohesionForce - v3LocalPosition);
	}
	//v3CohesionForce /= uNeighbourCount;
	//v3CohesionForce = glm::normalize(v3CohesionForce - v3LocalPosition);


	return v3CohesionForce;
}