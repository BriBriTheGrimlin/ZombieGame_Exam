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
		if (!pBlackboard->GetData("SteeringBehavior", pSteeringBh) || pSteeringBh == nullptr)
			return Elite::BehaviorState::Failure;
		

		Elite::Vector2 target{ -100,-100 };

		pSteeringBh->Seek(target);
		return Elite::BehaviorState::Success;

	}

	Elite::BehaviorState ChangeToFlee(Elite::Blackboard* pBlackboard)
	{
		SteeringBehavior* pSteeringBh{};
		std::vector<EntityInfo>* pEntitiesInFOV{ nullptr };
		IExamInterface* pInterface{ nullptr };

		Elite::Vector2 targetPos;
		if (!pBlackboard->GetData("SteeringBehavior", pSteeringBh) || pSteeringBh == nullptr)
			return Elite::BehaviorState::Failure;
		if (!pBlackboard->GetData("EntityInFov", pEntitiesInFOV) || pEntitiesInFOV == nullptr)
			return Elite::BehaviorState::Failure;
		if (!pBlackboard->GetData("InterFace", pInterface) || pInterface == nullptr)
			return Elite::BehaviorState::Failure;

		auto agentInfo = pInterface->Agent_GetInfo();

		for (const auto& entity : *pEntitiesInFOV)
		{
			if(entity.Type == eEntityType::PURGEZONE)
			{
				PurgeZoneInfo purgeZone{};
				pInterface->PurgeZone_GetInfo(entity, purgeZone);
				pSteeringBh->Flee(purgeZone.Center, purgeZone.Radius);
				return Elite::BehaviorState::Success;
			}
		}
		return Elite::BehaviorState::Failure;
	}

	Elite::BehaviorState LootFOV(Elite::Blackboard* pBlackboard)
	{
		IExamInterface* pInterface{ nullptr };
		SteeringBehavior* pSteeringBh{};
		std::vector<EntityInfo>* pEntitiesInFOV{ nullptr };

		if (!pBlackboard->GetData("InterFace", pInterface) || pInterface == nullptr)
			return Elite::BehaviorState::Failure;

		if (!pBlackboard->GetData("SteeringBehavior", pSteeringBh) || pSteeringBh == nullptr)
			return Elite::BehaviorState::Failure;

		if (!pBlackboard->GetData("EntityInFov", pEntitiesInFOV) || pEntitiesInFOV == nullptr)
			return Elite::BehaviorState::Failure;

		if (pEntitiesInFOV->empty())
		{
			return Elite::BehaviorState::Failure;
		}

		const auto agentInfo = pInterface->Agent_GetInfo();



		EntityInfo closestItem{};
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
						return Elite::BehaviorState::Success;
					}
					if (item.Type == eItemType::PISTOL)
					{
						pInterface->Item_Grab(entity, item);
						pInterface->Inventory_AddItem(1, item);
						return Elite::BehaviorState::Success;
					}
					if (item.Type == eItemType::FOOD)
					{
						if (!pInterface->Inventory_GetItem(2, item))
						{
							pInterface->Item_Grab(entity, item);
							pInterface->Inventory_AddItem(2, item);
							return Elite::BehaviorState::Success;
						}
						else if (pInterface->Inventory_GetItem(2, item))
						{
							pInterface->Item_Grab(entity, item);
							pInterface->Inventory_AddItem(3, item);
							return Elite::BehaviorState::Success;
						}
						else
						{
							pInterface->Item_Destroy(entity);
							return Elite::BehaviorState::Success;
						}
					}
					if (item.Type == eItemType::MEDKIT)
					{
						if (!pInterface->Inventory_GetItem(4, item))
						{
							pInterface->Item_Grab(entity, item);
							pInterface->Inventory_AddItem(4, item);
							return Elite::BehaviorState::Success;
						}
						else
						{
							pInterface->Item_Destroy(entity);
							return Elite::BehaviorState::Success;
						}
					}
					if (item.Type == eItemType::GARBAGE)
					{
						pInterface->Item_Destroy(entity);
						return Elite::BehaviorState::Success;
					}
				}
			}
			auto target = closestItem.Location;


			pSteeringBh->Seek(target);
			return Elite::BehaviorState::Success;
		}
		return Elite::BehaviorState::Failure;
	}

	Elite::BehaviorState ShootZombieOrRun(Elite::Blackboard* pBlackboard)
	{
		IExamInterface* pInterface{ nullptr };
		SteeringBehavior* pSteeringBh{};
		std::vector<EntityInfo>* pEntitiesInFOV{ nullptr };
		std::vector<HouseInfo>* pHouseInFov{ nullptr };

		if (pBlackboard->GetData("InterFace", pInterface) == false || pInterface == nullptr)
			return Elite::BehaviorState::Failure;
		if (pBlackboard->GetData("SteeringBehavior", pSteeringBh) == false || pSteeringBh == nullptr)
			return Elite::BehaviorState::Failure;
		if (pBlackboard->GetData("EntityInFov", pEntitiesInFOV) == false || pEntitiesInFOV == nullptr)
			return Elite::BehaviorState::Failure;
		if (pBlackboard->GetData("HouseInFov", pHouseInFov) == false || pHouseInFov == nullptr)
			return Elite::BehaviorState::Failure;

		EntityInfo closestEnemy{};
		
		auto agentInfo = pInterface->Agent_GetInfo();
		auto angleBuffer{ 0.10 };
		Elite::Vector2 desiredDirection = (closestEnemy.Location - agentInfo.Position);

		if (std::abs(agentInfo.Orientation - std::atan2(desiredDirection.y, desiredDirection.x)) < angleBuffer)
		{
			ItemInfo item{};
			if (pInterface->Inventory_GetItem(0, item))
			{
				pInterface->Inventory_UseItem(0);
				return Elite::BehaviorState::Success;
			}
			else if( pInterface->Inventory_GetItem(1,item))
			{
				pInterface->Inventory_UseItem(1);
				return Elite::BehaviorState::Success;
			}
			return Elite::BehaviorState::Failure;
		}

		return Elite::BehaviorState::Success;
	}

	Elite::BehaviorState GoInsideHouse(Elite::Blackboard* pBlackboard)
	{
		std::vector<HouseInfo>* pHousesInPov{ nullptr };
		SteeringBehavior* pSteeringBh{ nullptr };
		SteeringPlugin_Output* pSteeringOutputData{ nullptr };
		IExamInterface* pInterface{ nullptr };

		if (!pBlackboard->GetData("SteeringOutput", pSteeringOutputData) || pSteeringOutputData == nullptr)
			return Elite::BehaviorState::Failure;
		if (!pBlackboard->GetData("SteeringBehavior", pSteeringBh) || pSteeringBh == nullptr)
			return Elite::BehaviorState::Failure;
		if (!pBlackboard->GetData("HouseInFov", pHousesInPov) || pHousesInPov == nullptr)
			return Elite::BehaviorState::Failure;
		if (!pBlackboard->GetData("InterFace", pInterface) || pInterface == nullptr)
			return Elite::BehaviorState::Failure;

		auto houseInView = pHousesInPov[0];
		auto agentInfo = pInterface->Agent_GetInfo();
		auto target = houseInView[0].Center;


		if ((Elite::Distance(agentInfo.Position, target) <= 3) == false)
		{
			pSteeringBh->Seek(target);
			return Elite::BehaviorState::Success;
		}
		else return Elite::BehaviorState::Failure;
	}

	Elite::BehaviorState CheckHouse(Elite::Blackboard* pBlackboard)
	{
		std::vector<HouseInfo>* pHousesInPov{ nullptr };
		SteeringPlugin_Output* pSteeringOutput{ nullptr };
		IExamInterface* pInterface{ nullptr };
		SteeringBehavior* pSteeringBh{ nullptr };

		if (!pBlackboard->GetData("SteeringOutput", pSteeringOutput) || pSteeringOutput == nullptr)
			return Elite::BehaviorState::Failure;

		if (!pBlackboard->GetData("HouseInFov", pHousesInPov) || pHousesInPov == nullptr)
			return Elite::BehaviorState::Failure;

		if (!pBlackboard->GetData("InterFace", pInterface) || pInterface == nullptr)
			return Elite::BehaviorState::Failure;

		if (!pBlackboard->GetData("SteeringBehavior", pSteeringBh) || pSteeringBh == nullptr)
			return Elite::BehaviorState::Failure;

		HouseInfo houseInView = *pHousesInPov->begin();
		auto agentInfo = pInterface->Agent_GetInfo();

		auto targetRightTop = Elite::Vector2{ houseInView.Center.x + houseInView.Size.x /2 - 3,  houseInView.Center.y + houseInView.Size.y / 2 - 3 };
		auto targetLeftBottom = Elite::Vector2{ houseInView.Center.x - houseInView.Size.x / 2,  houseInView.Center.y - houseInView.Size.y / 2 };

		bool wentRightTop = false;
		bool wentLeftDown = false;

		pInterface->Draw_Circle(targetRightTop, 1, { 0,0,1 });

		if ((Elite::Distance(agentInfo.Position, targetRightTop) <= 1) == false && !wentRightTop)
		{
			pSteeringBh->Seek(targetRightTop);
			wentRightTop = true;
			return Elite::BehaviorState::Running;
			
		}
		else if ((Elite::Distance(agentInfo.Position, targetRightTop) <= 3) == false && !wentLeftDown && wentRightTop)
		{
			pSteeringBh->Seek(targetLeftBottom);
			wentLeftDown = true;
			return Elite::BehaviorState::Success;
			
		}
		std::cout << wentRightTop << '\n';
		
		return Elite::BehaviorState::Failure;
	}
}
namespace BT_Conditions
{

	bool IsEnemyInFOV(Elite::Blackboard* pBlackboard)
	{
		std::vector<EntityInfo>* pEntitiesInFOV{ nullptr };

		if (!pBlackboard->GetData("EntityInFov", pEntitiesInFOV) || pEntitiesInFOV == nullptr)
			return false;


		for (UINT i{ 0 }; i < pEntitiesInFOV->size(); ++i)
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
		std::vector<HouseInfo>* pHousesInFOV{ nullptr };

		if (!pBlackboard->GetData("HouseInFov", pHousesInFOV) || pHousesInFOV == nullptr)
			return false;
		
		return pHousesInFOV->size() > 0;
	}

	bool LootInFOV(Elite::Blackboard* pBlackboard)
	{
		std::vector<EntityInfo>* pEntitiesInFOV{ nullptr };
		if (!pBlackboard->GetData("EntityInFov", pEntitiesInFOV) || pEntitiesInFOV == nullptr)
		{
			return false;
		}

		return pEntitiesInFOV->size() > 0;
	}

	bool HaveMedKit(Elite::Blackboard* pBlackboard)
	{
		IExamInterface* InterFace{ nullptr };

		if (!pBlackboard->GetData("InterFace", InterFace) || InterFace == nullptr)
		{
			return false;
		}

		ItemInfo item{};
		return InterFace->Inventory_GetItem(4, item);
	}

	bool HaveFood(Elite::Blackboard* pBlackboard)
	{
		IExamInterface* InterFace{ nullptr };

		if (!pBlackboard->GetData("InterFace", InterFace) || InterFace == nullptr)
		{
			return false;
		}

		ItemInfo item{};
		return InterFace->Inventory_GetItem(3, item);
	}

	bool HaveGun(Elite::Blackboard* pBlackboard)
	{
		IExamInterface* InterFace{ nullptr };

		if (!pBlackboard->GetData("InterFace", InterFace) || InterFace == nullptr)
		{
			return false;
		}

		ItemInfo item{};
		if (!InterFace->Inventory_GetItem(0, item))
		{
			return InterFace->Inventory_GetItem(1, item);
		}
		else if (!InterFace->Inventory_GetItem(1, item))
		{
			return InterFace->Inventory_GetItem(1, item);
		}
		else return false;
	}

	bool IsPurgeZoneInFOV(Elite::Blackboard* pBlackboard)
	{
		std::vector<EntityInfo>* pEntitiesInFOV{ nullptr };
		
		if (!pBlackboard->GetData("EntityInFov", pEntitiesInFOV) || pEntitiesInFOV == nullptr)
			return false;
		
		for (const auto& entity : *pEntitiesInFOV)
		{
			if (entity.Type == eEntityType::PURGEZONE)
			{
				return true;
			}
		}
		return false;
	}

	bool returnTrue(Elite::Blackboard* pBlackboard)
	{
		return true;
	}

	bool AgentInHouse(Elite::Blackboard* pBlackboard)
	{
		IExamInterface* interFace{ nullptr };

		if (!pBlackboard->GetData("InterFace", interFace) || interFace == nullptr)
			return false;

		if(interFace->Agent_GetInfo().IsInHouse)
		{
			
			return true;
;		}

		return false;
	}
}

#endif
