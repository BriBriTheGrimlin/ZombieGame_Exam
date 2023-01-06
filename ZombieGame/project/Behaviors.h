#pragma once
#ifndef ELITE_APPLICATION_BEHAVIOR_TREE_BEHAVIORS
#define ELITE_APPLICATION_BEHAVIOR_TREE_BEHAVIORS
//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "Exam_HelperStructs.h"
#include "EBehaviorTree.h"
#include "SteeringBehaviors.h"

//-----------------------------------------------------------------
// Behaviors
//-----------------------------------------------------------------


namespace  BT_Actions
{
	Elite::BehaviorState ChangeToSeek(Elite::Blackboard* pBlackboard)
	{
		SteeringPlugin_Output* pSteering;
		Elite::Vector2 targetPos{};
		if (!pBlackboard->GetData("Steering", pSteering) || pSteering == nullptr)
			return Elite::BehaviorState::Failure;
		if (!pBlackboard->GetData("Target", targetPos))
			return Elite::BehaviorState::Failure;


		return Elite::BehaviorState::Success;
	}
	
	//Elite::BehaviorState ChangeToFlee(Elite::Blackboard* pBlackboard)
	//{
	//	SteeringPlugin_Output* pAgent;
	//
	//	Elite::Vector2 targetPos;
	//	if (!pBlackboard->GetData("Agent", pAgent) || pAgent == nullptr)
	//		return Elite::BehaviorState::Failure;
	//	if (!pBlackboard->GetData("Target", targetPos))
	//		return Elite::BehaviorState::Failure;
	//
	//	
	//	return Elite::BehaviorState::Success;
	//}
}

namespace BT_Conditions
{
	//bool IsFoodNearby(Elite::Blackboard* pBlackboard)
	//{
	//	AgarioAgent* pAgent;
	//	std::vector<AgarioFood*>* pFoodVec;
	//	if (!pBlackboard->GetData("Agent", pAgent) || pAgent == nullptr)
	//		return false;
	//	if (!pBlackboard->GetData("FoodVec", pFoodVec) || pFoodVec == nullptr)
	//		return false;
	//
	//	const float searchRadius = { 30.f + pAgent->GetRadius() };
	//	//DEBUGRENDERER2D->DrawCircle(pAgent->GetPosition(), searchRadius, { 0, 0, 1 }, 0.f);
	//
	//	AgarioFood* pClosestFood = nullptr;
	//	float closestDistSqrt = { searchRadius * searchRadius };
	//
	//	const Elite::Vector2 agentPos = pAgent->GetPosition();
	//	//Todo: Debug rendering!!!
	//
	//	for (auto& pFood : *pFoodVec)
	//	{
	//		float distSqrt = pFood->GetPosition().DistanceSquared(agentPos);
	//		if (distSqrt < closestDistSqrt)
	//		{
	//			pClosestFood = pFood;
	//			closestDistSqrt = distSqrt;
	//		}
	//	}
	//	if (pClosestFood != nullptr)
	//	{
	//		DEBUGRENDERER2D->DrawSegment(pClosestFood->GetPosition(), pAgent->GetPosition(), { 0, 0, 1 });
	//		pBlackboard->ChangeData("Target", pClosestFood->GetPosition());
	//		return true;
	//	}
	//	return false;
}
#endif
