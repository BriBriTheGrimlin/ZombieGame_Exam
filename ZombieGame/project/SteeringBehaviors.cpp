#include "stdafx.h"
#include "SteeringBehaviors.h"
//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

SteeringBehavior::SteeringBehavior(IExamInterface* pInterface, SteeringPlugin_Output* pSteeringbh)
	: m_pInterface{ pInterface }
	, m_pSteeringBh{ pSteeringbh }
{

}


//SEEK
void SteeringBehavior::Seek(const Elite::Vector2 target) const
{
	auto agentInfo = m_pInterface->Agent_GetInfo();

	auto nextTargetPos = m_pInterface->NavMesh_GetClosestPathPoint(target);

	m_pSteeringBh->LinearVelocity = nextTargetPos - agentInfo.Position;
	m_pSteeringBh->LinearVelocity.Normalize();
	m_pSteeringBh->LinearVelocity *= agentInfo.MaxLinearSpeed;
}

//FLEE
void SteeringBehavior::Flee(const Elite::Vector2& target, float radius) const
{
	auto agentInfo = m_pInterface->Agent_GetInfo();

	Elite::Vector2 toTarget = target - agentInfo.Position;
	auto newTarget = agentInfo.Position - (toTarget.GetNormalized() * radius);


	Seek(newTarget);
}

//FACE
void SteeringBehavior::Face(const Elite::Vector2 target) const
{
	auto agentInfo = m_pInterface->Agent_GetInfo();
	m_pSteeringBh->AutoOrient = false;

	Elite::Vector2 toTarget = (target - agentInfo.Position);
	toTarget.Normalize();
	const float agentRot{ agentInfo.Orientation + 0.9f * float(M_PI) };
	Elite::Vector2 agentDirection{ std::cosf(agentRot),std::sinf(agentRot) };
	m_pSteeringBh->AngularVelocity = (toTarget.Dot(agentDirection)) * agentInfo.MaxAngularSpeed;
}

void SteeringBehavior::SeekWhileSpinning(const Elite::Vector2 target) const
{

	m_pSteeringBh->AutoOrient = false;
	m_pSteeringBh->AngularVelocity = m_pInterface->Agent_GetInfo().MaxAngularSpeed;

	Seek(target);
}

void SteeringBehavior::FaceAndFlee(const Elite::Vector2 target) const
{

	//FACE
	m_pSteeringBh->AutoOrient = false;
	Elite::Vector2 toTarget = (target - m_pInterface->Agent_GetInfo().Position);
	toTarget.Normalize();
	const float agentRot{ m_pInterface->Agent_GetInfo().Orientation + 0.5f * float(M_PI)};
	Elite::Vector2 agentDirection{ std::cosf(agentRot),std::sinf(agentRot) };
	m_pSteeringBh->AngularVelocity = (toTarget.Dot(agentDirection)) * m_pInterface->Agent_GetInfo().MaxAngularSpeed;


	//FLEE
	m_pSteeringBh->LinearVelocity = target - m_pInterface->Agent_GetInfo().Position;
	m_pSteeringBh->LinearVelocity.Normalize();
	m_pSteeringBh->LinearVelocity *= -m_pInterface->Agent_GetInfo().MaxLinearSpeed / 2; //only use half speed so we dont lose the enemy

}