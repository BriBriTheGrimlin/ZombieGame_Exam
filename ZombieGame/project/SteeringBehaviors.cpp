#include "stdafx.h"
#include "SteeringBehaviors.h"
//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//Includes
#include "SteeringBehaviors.h"

//SEEK
//****
SteeringPlugin_Output Seek::CalculateSteering(float deltaT, AgentInfo* pAgent)
{
	SteeringPlugin_Output steering = {};

	steering.LinearVelocity = m_Target - pAgent->Position;
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= pAgent->MaxLinearSpeed;

	return steering;
}



//FLEE
//****
SteeringPlugin_Output Flee::CalculateSteering(float deltaT, AgentInfo* pAgent)
{
	Elite::Vector2 toTarget = pAgent->Position - m_Target;
	float distanceSqrd = toTarget.MagnitudeSquared();

	SteeringPlugin_Output steering = {};
	if (distanceSqrd > m_FleeRadius * m_FleeRadius)
	{
		return steering;
	}

	steering.LinearVelocity = m_Target - pAgent->Position;
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= -pAgent->MaxLinearSpeed;

	return steering;
}


//FACE
//****
//SteeringPlugin_Output Face::CalculateSteering(float deltaT, AgentInfo* pAgent)
//{
//	pAgent->SetAutoOrient(false);
//
//	//SteeringOutput steering{};
//	//Elite::Vector2 directionVector{ m_Target.Position - pAgent->GetPosition() };
//	//directionVector.Normalize();
//	//const float orientation{ Elite::GetOrientationFromVelocity(directionVector)};
//	//steering.AngularVelocity = orientation - pAgent->GetRotation() - M_PI / 2.f;
//
//	const Elite::Vector2 agentDirection{ pAgent->Orientation};
//	const Elite::Vector2 targetDirection{ (m_Target.Position - pAgent->GetPosition()).GetNormalized() };
//
//	const float dotProduct{ agentDirection.Dot(targetDirection) };            // - Projection of normalized vectors to check "overlap" (through projection)
//
//	float turnDirection{};
//	constexpr float epsilon{ 0.01f };                                        // - Chosen by iterative testing
//	if (dotProduct > 1 + epsilon || dotProduct < 1 - epsilon)                // - If agent & target vector overlap, result of dot will be 1
//	{
//		const float crossResult{ agentDirection.Cross(targetDirection) };    // - Determines turning direction
//		turnDirection = crossResult / abs(crossResult);                      // - "Unitize" turnDirection so speed of turning is not 
//	}                                                                        //	  determined by angular distance to target
//
//	SteeringPlugin_Output steering = {};
//	steering.AngularVelocity = turnDirection * pAgent->GetMaxAngularSpeed();
//
//	return steering;
//}

//EVADE
//****
SteeringPlugin_Output Evade::CalculateSteering(float deltaT, AgentInfo* pAgent)
{
	SteeringPlugin_Output steering{};
	Elite::Vector2 direction{};
	const Elite::Vector2 evadeMagnitude = m_Target - pAgent->Position;

	if (evadeMagnitude.MagnitudeSquared() <= 15 * 15)
	{
		//			'-' because we want to get away and not towards the target
		direction = -(evadeMagnitude)+m_Target;
	}

	steering.LinearVelocity = direction;
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= pAgent->MaxLinearSpeed;
	return steering;
}