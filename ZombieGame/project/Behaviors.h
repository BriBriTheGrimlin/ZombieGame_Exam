#pragma once
#ifndef ELITE_APPLICATION_BEHAVIOR_TREE_BEHAVIORS
#define ELITE_APPLICATION_BEHAVIOR_TREE_BEHAVIORS
//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "Exam_HelperStructs.h"
#include "EBehaviorTree.h"
#include "SteeringBehaviors.h"
#include "MyGlobals.h"

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
		

		Elite::Vector2 target{ 0,-100 };

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
				pSteeringBh->Flee(purgeZone.Center, purgeZone.Radius + 10);
				return Elite::BehaviorState::Success;
			}
		}
		
		agentInfo.RunMode = false;
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


		for (const auto& entity : *pEntitiesInFOV)
		{
				pSteeringBh->Seek(pEntitiesInFOV->begin()->Location);
					ItemInfo item{};
					pInterface->Item_GetInfo(entity, item);

					//pSteeringBh->Seek(item.Location);

					if (agentInfo.Position.Distance(item.Location) <= agentInfo.GrabRange)
					{
						if (item.Type == eItemType::SHOTGUN)
						{
							if (!pInterface->Inventory_GetItem(0, item))
							{
								pInterface->Item_Grab(entity, item);
								pInterface->Inventory_AddItem(0, item);
								return Elite::BehaviorState::Success;
							}
							else
							{
								pInterface->Item_Destroy(entity);
							}
							
						}
						if (item.Type == eItemType::PISTOL)
						{
							if (!pInterface->Inventory_GetItem(1, item))
							{
								pInterface->Item_Grab(entity, item);
								pInterface->Inventory_AddItem(1, item);
								return Elite::BehaviorState::Success;
							}
							else
							{
								pInterface->Item_Destroy(entity);
							}
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
								return Elite::BehaviorState::Running;
							}
							else
							{
								pInterface->Item_Destroy(entity);
								return Elite::BehaviorState::Running;
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
			if(pEntitiesInFOV->empty())
			{
				return Elite::BehaviorState::Failure;
			}
			return Elite::BehaviorState::Failure;
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
		closestEnemy = pEntitiesInFOV->begin()[0];
		auto agentInfo = pInterface->Agent_GetInfo();
		auto angleBuffer{ 0.10 };
		Elite::Vector2 desiredDirection = (closestEnemy.Location - agentInfo.Position);

		if (std::abs(agentInfo.Orientation - std::atan2(desiredDirection.y, desiredDirection.x)) < angleBuffer)
		{
			ItemInfo item{};
			if (pInterface->Inventory_GetItem(0, item))
			{
				pInterface->Inventory_UseItem(0);
				if (pInterface->Weapon_GetAmmo(item) <= 0)
				{
					pInterface->Inventory_RemoveItem(0);
				}
				return Elite::BehaviorState::Success;
			}
			if( pInterface->Inventory_GetItem(1,item))
			{
				pInterface->Inventory_UseItem(1);
				if (pInterface->Weapon_GetAmmo(item) <= 0)
				{
					pInterface->Inventory_RemoveItem(1);
				}
				return Elite::BehaviorState::Success;
			}
			
		}
		else
		{
			pSteeringBh->Flee(closestEnemy.Location, 15);
			return Elite::BehaviorState::Success;
		}

		return Elite::BehaviorState::Failure;
	}

	Elite::BehaviorState GoInsideHouse(Elite::Blackboard* pBlackboard)
	{
		std::vector<HouseInfo>* pHousesInPov{ nullptr };
		SteeringBehavior* pSteeringBh{ nullptr };
		SteeringPlugin_Output* pSteeringOutputData{ nullptr };
		IExamInterface* pInterface{ nullptr };
		GlobalsStruct* pGlobals{ nullptr };

		if (!pBlackboard->GetData("SteeringOutput", pSteeringOutputData) || pSteeringOutputData == nullptr)
			return Elite::BehaviorState::Failure;
		if (!pBlackboard->GetData("SteeringBehavior", pSteeringBh) || pSteeringBh == nullptr)
			return Elite::BehaviorState::Failure;
		if (!pBlackboard->GetData("HouseInFov", pHousesInPov) || pHousesInPov == nullptr)
			return Elite::BehaviorState::Failure;
		if (!pBlackboard->GetData("InterFace", pInterface) || pInterface == nullptr)
			return Elite::BehaviorState::Failure;
		if (!pBlackboard->GetData("Globals", pGlobals) || pGlobals == nullptr)
			return Elite::BehaviorState::Failure;

		if (pHousesInPov->empty())
			return Elite::BehaviorState::Failure;

		Elite::Vector2 target = pGlobals->currentHouse.Center;

		AgentInfo agentInfo = pInterface->Agent_GetInfo();

		pInterface->Draw_SolidCircle(target, 3, { 0,0 }, { 1,1,0 });
		if(pGlobals->wentToCenter)
			return Elite::BehaviorState::Success;

		if ((Elite::Distance(agentInfo.Position, target) >= 3) && !pGlobals->wentToCenter)
		{
			pSteeringBh->Seek(target);
		}
		if ((Elite::Distance(agentInfo.Position, target) <= 3) && !pGlobals->wentToCenter)
		{
			pSteeringOutputData->LinearVelocity = Elite::Vector2{ 0,0 };
			pGlobals->wentToCenter = true;
			return Elite::BehaviorState::Success;
		}
		
		return Elite::BehaviorState::Failure;
	}

	Elite::BehaviorState CheckHouse(Elite::Blackboard* pBlackboard)
	{
		std::vector<HouseInfo>* pHousesInPov{ nullptr };
		SteeringPlugin_Output* pSteeringOutput{ nullptr };
		IExamInterface* pInterface{ nullptr };
		SteeringBehavior* pSteeringBh{ nullptr };
		GlobalsStruct* pGlobals{ nullptr };

		if (!pBlackboard->GetData("SteeringOutput", pSteeringOutput) || pSteeringOutput == nullptr)
			return Elite::BehaviorState::Failure;

		if (!pBlackboard->GetData("HouseInFov", pHousesInPov) || pHousesInPov == nullptr)
			return Elite::BehaviorState::Failure;

		if (!pBlackboard->GetData("InterFace", pInterface) || pInterface == nullptr)
			return Elite::BehaviorState::Failure;

		if (!pBlackboard->GetData("SteeringBehavior", pSteeringBh) || pSteeringBh == nullptr)
			return Elite::BehaviorState::Failure;

		if (!pBlackboard->GetData("Globals", pGlobals) || pGlobals == nullptr)
			return Elite::BehaviorState::Failure;
		

		HouseInfo houseInView = *pHousesInPov->begin();
		auto agentInfo = pInterface->Agent_GetInfo();

		auto targetRightTop = Elite::Vector2{ houseInView.Center.x + houseInView.Size.x /2-3,  houseInView.Center.y + houseInView.Size.y / 2 -3 };
		auto targetLeftBottom = Elite::Vector2{ houseInView.Center.x - houseInView.Size.x / 2 + 3,  houseInView.Center.y - houseInView.Size.y / 2 + 3 };
		
		pInterface->Draw_Circle(targetRightTop, 3, { 1,0,0 });

		//check if went rightTop
		if (Elite::Distance(agentInfo.Position, targetRightTop) <= 3)
		{
			pGlobals->wentRighttop = true;
		}
		else if ((Elite::Distance(agentInfo.Position, targetRightTop) >= 3) && !pGlobals->wentRighttop)
		{
			pSteeringBh->Seek(targetRightTop);
		}
		
		if (Elite::Distance(agentInfo.Position, targetLeftBottom) <= 3)
		{
			pGlobals->wentLeftBottom = true;
		}
		else if ((Elite::Distance(agentInfo.Position, targetLeftBottom) >= 3) && !pGlobals->wentLeftBottom && pGlobals->wentRighttop)
		{
			pSteeringBh->Seek(targetLeftBottom);
			pGlobals->wentLeftBottom = true;
			
			return Elite::BehaviorState::Success;
		}
		
		
		return Elite::BehaviorState::Failure;
	}

	Elite::BehaviorState Explore(Elite::Blackboard* pBlackboard)
	{
		IExamInterface* pInterFace{ nullptr };
		SteeringBehavior* pSteeringBh{ nullptr };

		if (!pBlackboard->GetData("InterFace", pInterFace) || pInterFace == nullptr)
			return Elite::BehaviorState::Failure;
		if (pBlackboard->GetData("SteeringBehavior", pSteeringBh) == false || pSteeringBh == nullptr)
			return Elite::BehaviorState::Failure;

		WorldInfo worldInfo = pInterFace->World_GetInfo();

		Elite::Vector2 bottomLeft{ worldInfo.Center.x - worldInfo.Dimensions.x / 4, worldInfo.Center.y - worldInfo.Dimensions.y / 3 };

		pInterFace->Draw_Circle(bottomLeft, 5, { 1,0,0 });

		pSteeringBh->Seek(bottomLeft);
		
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
		GlobalsStruct* pGlobals{ nullptr };

		if (!pBlackboard->GetData("HouseInFov", pHousesInFOV) || pHousesInFOV == nullptr)
			return false;
		if (!pBlackboard->GetData("Globals", pGlobals) || pGlobals == nullptr)
			return false;
		
		if ((pGlobals->currentHouse.Center == Elite::Vector2{ 0,0 } && pHousesInFOV->size() > 0))
		{
			pGlobals->currentHouse = (*pHousesInFOV)[0];
			return true;
		}
		else if (pGlobals->currentHouse.Center != Elite::Vector2{ 0,0 })
		{
			return true;
		}
		return false;
	}

	bool LootInFOV(Elite::Blackboard* pBlackboard)
	{
		std::vector<EntityInfo>* pEntitiesInFOV{ nullptr };
		
		if (!pBlackboard->GetData("EntityInFov", pEntitiesInFOV) || pEntitiesInFOV == nullptr)
			return false;
		
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
		if (InterFace->Inventory_GetItem(0, item))
		{
			return InterFace->Inventory_GetItem(0, item);
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

	bool CheckedHouse(Elite::Blackboard* pBlackboard)
	{
		GlobalsStruct* pGlobals{ nullptr };

		if (!pBlackboard->GetData("Globals", pGlobals) || pGlobals == nullptr)
			return false;

		if (pGlobals->wentRighttop && pGlobals->wentLeftBottom)
		{
			return true;
		}
		return false;
	}
}

	

#endif
