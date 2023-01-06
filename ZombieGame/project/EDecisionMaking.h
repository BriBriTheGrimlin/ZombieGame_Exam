#pragma once
#ifndef ELITE_DECISION_MAKING
#define ELITE_DECISION_MAKING

/* --- Data --- */
//General Interface
namespace Elite
{
	class IDecisionMaking
	{
	public:
		IDecisionMaking() = default;
		virtual ~IDecisionMaking() = default;

		virtual void Update(float deltaT) = 0;

	};
}

/* --- Data --- */
//Blackboard
#include "EBlackboard.h"

/* --- Decision Making Structures --- */
#include "EBehaviorTree.h"


#endif
