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
//****
void SteeringBehavior::Seek(const Elite::Vector2 target) const
{
	auto agentInfo = m_pInterface->Agent_GetInfo();

	auto nextTargetPos = m_pInterface->NavMesh_GetClosestPathPoint(target);

	m_pSteeringbh->LinearVelocity = nextTargetPos - agentInfo.Position; //Desired Velocity
	m_pSteeringbh->LinearVelocity.Normalize();						  //Normalize Desired Velocity
	m_pSteeringbh->LinearVelocity *= agentInfo.MaxLinearSpeed;
}

SteeringPlugin_Output SteeringBehavior::Flee(const Elite::Vector2& target, float radius) const
{
	auto agentInfo = m_pInterface->Agent_GetInfo();

	Elite::Vector2 toTarget = agentInfo.Position - target;
	float distanceSqrd = toTarget.MagnitudeSquared();

	SteeringPlugin_Output steering ={};
	if (distanceSqrd > radius * radius)
	{
		return steering;
	}

	steering.LinearVelocity = target - agentInfo.Position;
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= -agentInfo.MaxLinearSpeed;

	return steering;
}
SteeringPlugin_Output SteeringBehavior::Face(const Elite::Vector2 target) const
{
	auto AgentInfo = m_pInterface->Agent_GetInfo();

	const Elite::Vector2 agentDirection{ cosf(AgentInfo.Orientation), sinf(AgentInfo.Orientation) };
	const Elite::Vector2 targetDirection{ (target - AgentInfo.Position).GetNormalized() };

	const float dotProduct{ agentDirection.Dot(targetDirection) };            // - Projection of normalized vectors to check "overlap" (through projection)

	float turnDirection{};
	constexpr float epsilon{ 0.01f };                                        // - Chosen by iterative testing
	if (dotProduct > 1 + epsilon || dotProduct < 1 - epsilon)                // - If agent & target vector overlap, result of dot will be 1
	{
		const float crossResult{ agentDirection.Cross(targetDirection) };    // - Determines turning direction
		turnDirection = crossResult / abs(crossResult);                      // - "Unitize" turnDirection so speed of turning is not 
	}                                                                        //	  determined by angular distance to target

	SteeringPlugin_Output steering = {};
	steering.AngularVelocity = turnDirection * AgentInfo.MaxAngularSpeed;

	return steering;
}

SteeringPlugin_Output SteeringBehavior::Evade(const Elite::Vector2 target) const
{
	SteeringPlugin_Output steering{};
	Elite::Vector2 direction{};
	auto agentInfo = m_pInterface->Agent_GetInfo();
	const Elite::Vector2 evadeMagnitude = target - agentInfo.Position;

	if (evadeMagnitude.MagnitudeSquared() <= 15 * 15)
	{
		//			'-' because we want to get away and not towards the target
		direction = -(evadeMagnitude) + target;
	}

	steering.LinearVelocity = direction;
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= agentInfo.MaxLinearSpeed;
	return steering;
}