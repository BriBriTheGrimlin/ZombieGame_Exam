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
		SteeringBehavior* pSteeringBh;
		if (!pBlackboard->GetData("Steering", pSteeringBh) || pSteeringBh == nullptr)
			return Elite::BehaviorState::Failure;

		Elite::Vector2 target{ 0,-100 };

		pSteeringBh->Seek(target);
	}

	Elite::BehaviorState ChangeToFlee(Elite::Blackboard* pBlackboard)
	{
		SteeringBehavior* pSteeringBh;

		Elite::Vector2 targetPos;
		if (!pBlackboard->GetData("Steering", pSteeringBh) || pSteeringBh == nullptr)
			return Elite::BehaviorState::Failure;


		return Elite::BehaviorState::Success;
	}

	Elite::BehaviorState LootFOV(Elite::Blackboard* pBlackboard)
	{
		IExamInterface* pInterface{ nullptr };
		SteeringBehavior* pSteeringBh{};
		std::vector<EntityInfo>* pEntitiesInFOV{ nullptr };

		if (pBlackboard->GetData("interface", pInterface) == false || pInterface == nullptr)
			return Elite::BehaviorState::Failure;

		if (pBlackboard->GetData("SteeringBehavior", pSteeringBh) == false || pSteeringBh == nullptr)
			return Elite::BehaviorState::Failure;

		if (pBlackboard->GetData("InFov", pEntitiesInFOV) == false || pEntitiesInFOV == nullptr)
			return Elite::BehaviorState::Failure;

		if (pEntitiesInFOV->empty())
		{
			return Elite::BehaviorState::Failure;
		}

		auto agentInfo = pInterface->Agent_GetInfo();



		EntityInfo closestItem{};
		closestItem.Location.x = 1000.f;
		for (const auto& entity : *pEntitiesInFOV)
		{
			if ((closestItem.Location - pInterface->Agent_GetInfo().Position).MagnitudeSquared() < agentInfo.GrabRange * agentInfo.GrabRange)
			{
				if (entity.Type == eEntityType::ITEM)
				{
					ItemInfo item{};
					pInterface->Item_GetInfo(entity, item);

					if (item.Type == eItemType::SHOTGUN)
					{

						pInterface->Item_Grab(entity, item);
						pInterface->Inventory_AddItem(0, item);
						return Elite::BehaviorState::Running;
					}
					if (item.Type == eItemType::PISTOL)
					{
						pInterface->Item_Grab(entity, item);
						pInterface->Inventory_AddItem(1, item);
						return Elite::BehaviorState::Running;
					}
					if (item.Type == eItemType::FOOD)
					{
						if (!pInterface->Inventory_GetItem(2, item))
						{
							pInterface->Item_Grab(entity, item);
							pInterface->Inventory_AddItem(2, item);
							return Elite::BehaviorState::Running;
						}
						if (pInterface->Inventory_GetItem(2, item))
						{
							pInterface->Item_Grab(entity, item);
							pInterface->Inventory_AddItem(3, item);
							return Elite::BehaviorState::Running;
						}
					}
					if (item.Type == eItemType::MEDKIT)
					{
						if (!pInterface->Inventory_GetItem(4, item))
						{
							pInterface->Item_Grab(entity, item);
							pInterface->Inventory_AddItem(4, item);
							return Elite::BehaviorState::Running;
						}
						else
						{
							pInterface->Item_Destroy(entity);
							return Elite::BehaviorState::Running;
						}
					}
					if (item.Type == eItemType::GARBAGE)
					{
						pInterface->Item_Destroy(entity);
						return Elite::BehaviorState::Running;
					}
				}
			}
			auto target = closestItem.Location;


			pSteeringBh->Seek(target);
			return Elite::BehaviorState::Success;
		}

	}

	namespace BT_Conditions
	{
		bool IsEnemyInFOV(Elite::Blackboard* pBlackboard)
		{
			std::vector<EntityInfo>* pEntitiesInFOV{ nullptr };

			if (pBlackboard->GetData("InFov", pEntitiesInFOV) == false || pEntitiesInFOV == nullptr)
				return false;


			for (int i{ 0 }; i < pEntitiesInFOV->size(); ++i)
			{
				if (pEntitiesInFOV->at(i).Type == eEntityType::ENEMY)
				{
					return true;
				}
			}

			return false;
		}

		bool IsHouseInFOV(Elite::Blackboard* pBlackboard)
		{
			IExamInterface* pInterface{ nullptr };
			std::vector<HouseInfo>* pHousesInFOV{ nullptr };

			if (pBlackboard->GetData("InterFace", pInterface) == false || pInterface == nullptr)
				return false;

			if (pBlackboard->GetData("HouseInFov", pHousesInFOV) == false || pHousesInFOV == nullptr)
				return false;


			return pHousesInFOV->size() > 0;
		}

		bool LootInFOV(Elite::Blackboard* pBlackboard)
		{
			IExamInterface* pInterface{ nullptr };
			std::vector<EntityInfo>* pEntitiesInFOV{ nullptr };

			if (pBlackboard->GetData("InterFace", pInterface) == false || pInterface == nullptr)
			{
				return false;
			}
			if (pBlackboard->GetData("InFov", pEntitiesInFOV) == false || pEntitiesInFOV == nullptr)
			{
				return false;
			}

			return pEntitiesInFOV->size() > 0;
		}

		bool DoIHaveMedKit(Elite::Blackboard* pBlackboard)
		{
			IExamInterface* InterFace{ nullptr };

			if (pBlackboard->GetData("InterFace", InterFace) == false || InterFace == nullptr)
			{
				return false;
			}

			ItemInfo item{};

			return InterFace->Inventory_GetItem(4, item);
		}

		bool DoIHaveFood(Elite::Blackboard* pBlackboard)
		{
			IExamInterface* InterFace{ nullptr };

			if (pBlackboard->GetData("InterFace", InterFace) == false || InterFace == nullptr)
			{
				return false;
			}

			ItemInfo item{};

			return InterFace->Inventory_GetItem(3, item);
		}
	}
}
#endif
