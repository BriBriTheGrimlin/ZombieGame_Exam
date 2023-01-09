#include "stdafx.h"
#include "SteeringBehaviors.h"
//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

SteeringBehavior::SteeringBehavior(IExamInterface* pInterface, SteeringPlugin_Output* pSteeringbh)
	: m_pInterface{ pInterface }
	, m_pSteeringbh{ pSteeringbh }
{
	
}


//SEEK
void SteeringBehavior::Seek(const Elite::Vector2 target) const
{
	auto agentInfo = m_pInterface->Agent_GetInfo();

	auto nextTargetPos = m_pInterface->NavMesh_GetClosestPathPoint(target);

	m_pSteeringbh->LinearVelocity = nextTargetPos - agentInfo.Position; //Desired Velocity
	m_pSteeringbh->LinearVelocity.Normalize();						  //Normalize Desired Velocity
	m_pSteeringbh->LinearVelocity *= agentInfo.MaxLinearSpeed;
}

//FLEE
void SteeringBehavior::Flee(const Elite::Vector2& target, float radius) const
{
	auto agentInfo = m_pInterface->Agent_GetInfo();

	Elite::Vector2 toTarget = agentInfo.Position - target;
	auto newTarget = agentInfo.Position - (toTarget.GetNormalized() * radius);

	Seek(newTarget);

}

//FACE
void SteeringBehavior::Face(const Elite::Vector2 target) const
{
	auto agentInfo = m_pInterface->Agent_GetInfo();
	m_pSteeringbh->AutoOrient = false;

	Elite::Vector2 toTarget = (target - agentInfo.Position);
	toTarget.Normalize();
	const float agentRot{ agentInfo.Orientation + 0.5f * float(M_PI) };
	Elite::Vector2 agentDirection{ std::cosf(agentRot),std::sinf(agentRot) };
	m_pSteeringbh->AngularVelocity = (toTarget.Dot(agentDirection)) * agentInfo.MaxAngularSpeed;
}