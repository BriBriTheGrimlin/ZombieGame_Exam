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
		SteeringPlugin_Output* pSteeringOutputData{ nullptr };

		if (!pBlackboard->GetData("InterFace", pInterface) || pInterface == nullptr)
			return Elite::BehaviorState::Failure;

		if (!pBlackboard->GetData("SteeringBehavior", pSteeringBh) || pSteeringBh == nullptr)
			return Elite::BehaviorState::Failure;

		if (!pBlackboard->GetData("EntityInFov", pEntitiesInFOV) || pEntitiesInFOV == nullptr)
			return Elite::BehaviorState::Failure;

		if (!pBlackboard->GetData("SteeringOutput", pSteeringOutputData) || pSteeringOutputData == nullptr)
			return Elite::BehaviorState::Failure;

		if (pEntitiesInFOV->empty())
		{
			return Elite::BehaviorState::Failure;
		}

		const auto agentInfo = pInterface->Agent_GetInfo();


		for (const auto& entity : *pEntitiesInFOV)
		{
			pSteeringOutputData->AutoOrient = true;
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
		GlobalsStruct* pGlobals{ nullptr };

		if (pBlackboard->GetData("InterFace", pInterface) == false || pInterface == nullptr)
			return Elite::BehaviorState::Failure;
		if (pBlackboard->GetData("SteeringBehavior", pSteeringBh) == false || pSteeringBh == nullptr)
			return Elite::BehaviorState::Failure;
		if (pBlackboard->GetData("EntityInFov", pEntitiesInFOV) == false || pEntitiesInFOV == nullptr)
			return Elite::BehaviorState::Failure;
		if (pBlackboard->GetData("HouseInFov", pHouseInFov) == false || pHouseInFov == nullptr)
			return Elite::BehaviorState::Failure;
		if (!pBlackboard->GetData("Globals", pGlobals) || pGlobals == nullptr)
			return Elite::BehaviorState::Failure;


		EntityInfo closestEnemy{};
		closestEnemy = pEntitiesInFOV->begin()[0];
		auto agentInfo = pInterface->Agent_GetInfo();
		auto angleBufferShotgun{ 0.3 };
		auto angleBufferGun{ 0.1 };
		Elite::Vector2 desiredDirection = (closestEnemy.Location - agentInfo.Position);
		ItemInfo item{};

		pSteeringBh->Face(closestEnemy.Location);

		if (std::abs(agentInfo.Orientation - std::atan2(desiredDirection.y, desiredDirection.x)) < angleBufferGun)
		{
			if (pInterface->Inventory_GetItem(pGlobals->inventorySlots["Pistol"], item)) //|| pInterface->Inventory_GetItem(pGlobals->inventorySlots["Shotgun"], item))
			{

				pInterface->Inventory_UseItem(pGlobals->inventorySlots["Pistol"]);
				if (pInterface->Weapon_GetAmmo(item) <= 0)
				{
					pInterface->Inventory_RemoveItem(pGlobals->inventorySlots["Pistol"]);
				}
				return Elite::BehaviorState::Success;
			}
			else if (pInterface->Inventory_GetItem(pGlobals->inventorySlots["Shotgun"], item))
			{
				pInterface->Inventory_UseItem(pGlobals->inventorySlots["Shotgun"]);
				if (pInterface->Weapon_GetAmmo(item) <= 0)
				{
					pInterface->Inventory_RemoveItem(pGlobals->inventorySlots["Shotgun"]);
				}
				return Elite::BehaviorState::Success;
			}
			else
			{
				pSteeringBh->FaceAndFlee(closestEnemy.Location);
				return Elite::BehaviorState::Failure;
			}
		}
		else
		{
			pSteeringBh->Face(closestEnemy.Location);
			return Elite::BehaviorState::Success;
		}
		//else if (std::abs(agentInfo.Orientation - std::atan2(desiredDirection.y, desiredDirection.x)) < angleBufferShotgun)
		//{
		//	if (agentInfo.Position.Distance(closestEnemy.Location) <= agentInfo.FOV_Range / 3)
		//	{
		//		if (pInterface->Inventory_GetItem(pGlobals->inventorySlots["Shotgun"], item))
		//		{
		//			pInterface->Inventory_UseItem(pGlobals->inventorySlots["Shotgun"]);
		//			if (pInterface->Weapon_GetAmmo(item) <= 0)
		//			{
		//				pInterface->Inventory_RemoveItem(pGlobals->inventorySlots["Shotgun"]);
		//			}
		//			return Elite::BehaviorState::Success;
		//		}
		//	}
		//}
		

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

		if (pHousesInPov->empty() && pGlobals->currentHouse.Center == Elite::Vector2{0,0})
			return Elite::BehaviorState::Failure;

		Elite::Vector2 target = pGlobals->currentHouse.Center;

		AgentInfo agentInfo = pInterface->Agent_GetInfo();

		if(pGlobals->wentToCenter)
			return Elite::BehaviorState::Success;

		if ((Elite::Distance(agentInfo.Position, target) >= 3) && !pGlobals->wentToCenter)
		{
			pSteeringBh->Seek(target);
		}
		if ((Elite::Distance(agentInfo.Position, target) <= 3) && !pGlobals->wentToCenter)
		{
			//pSteeringOutputData->LinearVelocity = Elite::Vector2{ 0,0 };
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
		
		if (pGlobals->wentLeftBottom)
			return Elite::BehaviorState::Success;


		HouseInfo houseInView = pGlobals->currentHouse;
		auto agentInfo = pInterface->Agent_GetInfo();

		auto targetRightTop = Elite::Vector2{ houseInView.Center.x + houseInView.Size.x /2-3,  houseInView.Center.y + houseInView.Size.y / 2 -3 };
		auto targetLeftBottom = Elite::Vector2{ houseInView.Center.x - houseInView.Size.x / 2 + 3,  houseInView.Center.y - houseInView.Size.y / 2 + 3 };


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
			return Elite::BehaviorState::Success;
		}
		else if ((Elite::Distance(agentInfo.Position, targetLeftBottom) >= 3) && !pGlobals->wentLeftBottom && pGlobals->wentRighttop)
		{
			pSteeringBh->Seek(targetLeftBottom);
			
		}
	
		return Elite::BehaviorState::Failure;
	}

	Elite::BehaviorState LeaveHouse(Elite::Blackboard* pBlackboard)
	{
		SteeringBehavior* pSteeringBh{ nullptr };
		IExamInterface* pInterface{ nullptr };
		GlobalsStruct* pGlobals{ nullptr };

		if (!pBlackboard->GetData("SteeringBehavior", pSteeringBh) || pSteeringBh == nullptr)
			return Elite::BehaviorState::Failure;
		if (!pBlackboard->GetData("InterFace", pInterface) || pInterface == nullptr)
			return Elite::BehaviorState::Failure;
		if (!pBlackboard->GetData("Globals", pGlobals) || pGlobals == nullptr)
			return Elite::BehaviorState::Failure;

		

		auto agentInfo = pInterface->Agent_GetInfo();
		pSteeringBh->Seek(Elite::Vector2{ agentInfo.Position.x - 100, agentInfo.Position.y - 100 });

		if (agentInfo.IsInHouse == false)
		{
			pGlobals->wentRighttop = false;
			pGlobals->wentLeftBottom = false;
			pGlobals->wentToCenter = false;
			pGlobals->housesVisited.push_back(pGlobals->currentHouse);
			pGlobals->currentHouse.Center = { 0,0 };
			return Elite::BehaviorState::Success;
		}

		return Elite::BehaviorState::Failure;
	}

	Elite::BehaviorState Explore(Elite::Blackboard* pBlackboard)
	{
		IExamInterface* pInterface{ nullptr };
		SteeringBehavior* pSteeringBh{ nullptr };
		SteeringPlugin_Output* pSteeringOutput{ nullptr };
		GlobalsStruct* pGlobals{ nullptr };

		if (!pBlackboard->GetData("InterFace", pInterface) || pInterface == nullptr)
			return Elite::BehaviorState::Failure;
		if (pBlackboard->GetData("SteeringBehavior", pSteeringBh) == false || pSteeringBh == nullptr)
			return Elite::BehaviorState::Failure;
		if (pBlackboard->GetData("SteeringOutput", pSteeringOutput) == false || pSteeringOutput == nullptr)
			return Elite::BehaviorState::Failure;
		if (!pBlackboard->GetData("Globals", pGlobals) || pGlobals == nullptr)
			return Elite::BehaviorState::Failure;

		WorldInfo worldInfo = pInterface->World_GetInfo();
		AgentInfo agentInfo = pInterface->Agent_GetInfo();

		const float offset{ 4 };
		Elite::Vector2 bottomLeft{ worldInfo.Center.x - worldInfo.Dimensions.x / offset, worldInfo.Center.y - worldInfo.Dimensions.y / offset };
		Elite::Vector2 topLeft{ worldInfo.Center.x - worldInfo.Dimensions.x / offset, worldInfo.Center.y + worldInfo.Dimensions.y / offset };
		Elite::Vector2 bottomRight{ worldInfo.Center.x + worldInfo.Dimensions.x / offset, worldInfo.Center.y - worldInfo.Dimensions.y / offset };
		Elite::Vector2 topRight{ worldInfo.Center.x + worldInfo.Dimensions.x / offset, worldInfo.Center.y + worldInfo.Dimensions.y / offset };

		Elite::Vector2 target{};

		

		if (pGlobals->bottomLeftExplored && pGlobals->topLeftExplored &&
			pGlobals->topRightExplored && pGlobals->bottomRightExplored)
		{
			std::cout << "HAS SEARCHED ALL CORNERS, reset them\n";
			pGlobals->bottomLeftExplored = false;
			pGlobals->topLeftExplored = false;
			pGlobals->topRightExplored = false;
			pGlobals->bottomRightExplored = false;
			pGlobals->housesVisited.clear();
		}
		else if (pGlobals->bottomLeftExplored && pGlobals->topLeftExplored &&
			pGlobals->topRightExplored)
		{
			target = bottomRight;
		}
		else if (pGlobals->bottomLeftExplored && pGlobals->topLeftExplored)
		{
			target = topRight;
		}
		else if (pGlobals->bottomLeftExplored)
		{
			target = topLeft;
		}
		else
		{
			target = bottomLeft;
		}

		

		const float distance{ 3 };
		if (!pGlobals->bottomLeftExplored && Elite::Distance(agentInfo.Position, bottomLeft) <= distance)
		{

			pGlobals->bottomLeftExplored = true;
		}

		else if (!pGlobals->topLeftExplored && Elite::Distance(agentInfo.Position, topLeft) <= distance)
		{
			pGlobals->topLeftExplored = true;
		}

		else if (!pGlobals->bottomRightExplored && Elite::Distance(agentInfo.Position, topRight) <= distance)
		{
			pGlobals->topRightExplored = true;
		}

		else if (!pGlobals->topRightExplored && Elite::Distance(agentInfo.Position, bottomRight) <= distance)
		{
			pGlobals->bottomRightExplored = true;
		}

		pSteeringBh->SeekWhileSpinning(target);

		pInterface->Draw_Circle(target, 5, { 1,0,0 });
		
		return Elite::BehaviorState::Failure;
	}

	Elite::BehaviorState Eat(Elite::Blackboard* pBlackboard)
	{
		IExamInterface* pInterface{ nullptr };
		if (!pBlackboard->GetData("InterFace", pInterface) || pInterface == nullptr)
		{
			return Elite::BehaviorState::Failure;
		}
		GlobalsStruct* pGlobals{ nullptr };
		if (!pBlackboard->GetData("Globals", pGlobals) || pGlobals == nullptr)
			return Elite::BehaviorState::Failure;


		if (pInterface->Inventory_UseItem(pGlobals->inventorySlots["Food"])) // OR Food2
		{
			pInterface->Inventory_RemoveItem(pGlobals->inventorySlots["Food"]);
			return Elite::BehaviorState::Success;
		}
		else
		{
			return Elite::BehaviorState::Failure;
		}
		
	}

	Elite::BehaviorState Heal(Elite::Blackboard* pBlackboard)
	{
		IExamInterface* pInterface{ nullptr };
		if (!pBlackboard->GetData("InterFace", pInterface) || pInterface == nullptr)
		{
			return Elite::BehaviorState::Failure;
		}

		GlobalsStruct* pGlobals{ nullptr };
		if (!pBlackboard->GetData("Globals", pGlobals) || pGlobals == nullptr)
			return Elite::BehaviorState::Failure;

		if (pInterface->Inventory_UseItem(pGlobals->inventorySlots["Medkit"]))
		{
			pInterface->Inventory_RemoveItem(pGlobals->inventorySlots["Medkit"]);
			return Elite::BehaviorState::Success;
		}
		else
		{
			return Elite::BehaviorState::Failure;
		}

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

	bool LootInFOV(Elite::Blackboard* pBlackboard)
	{
		std::vector<EntityInfo>* pEntitiesInFOV{ nullptr };
		
		if (!pBlackboard->GetData("EntityInFov", pEntitiesInFOV) || pEntitiesInFOV == nullptr)
			return false;
		
		return pEntitiesInFOV->size() > 0;
	}

	bool HaveMedKit(Elite::Blackboard* pBlackboard)
	{
		IExamInterface* pInterface{ nullptr };
		if (!pBlackboard->GetData("InterFace", pInterface) || pInterface == nullptr)
		{
			return false;
		}

		GlobalsStruct* pGlobals{ nullptr };
		if (!pBlackboard->GetData("Globals", pGlobals) || pGlobals == nullptr)
			return false;

		if (pInterface->Agent_GetInfo().Health > 8)
			return false;
		
		ItemInfo item{};
		if (!pInterface->Inventory_GetItem(pGlobals->inventorySlots["Medkit"], item))
			return false;

		return true;
	}

	bool HaveFood(Elite::Blackboard* pBlackboard)
	{
		IExamInterface* pInterface{ nullptr };
		if (!pBlackboard->GetData("InterFace", pInterface) || pInterface == nullptr)
		{
			return false;
		}

		GlobalsStruct* pGlobals{ nullptr };
		if (!pBlackboard->GetData("Globals", pGlobals) || pGlobals == nullptr)
			return false;

		if (pInterface->Agent_GetInfo().Energy > 5)
			return false;

		ItemInfo item{};
		if (!pInterface->Inventory_GetItem(pGlobals->inventorySlots["Food"], item))
			return false;

		return true;
	}

	bool HaveGun(Elite::Blackboard* pBlackboard)
	{
		IExamInterface* pInterface{ nullptr };
		if (!pBlackboard->GetData("InterFace", pInterface) || pInterface == nullptr)
			return false;

		GlobalsStruct* pGlobals{ nullptr };
		if (!pBlackboard->GetData("Globals", pGlobals) || pGlobals == nullptr)
			return false;

		ItemInfo item{};
		if (pInterface->Inventory_GetItem(pGlobals->inventorySlots["Pistol"], item) || (pInterface->Inventory_GetItem(pGlobals->inventorySlots["Shotgun"], item))) // OR shotgun
		{
			//if (!pInterface->Inventory_GetItem(pGlobals->inventorySlots["Pistol"], item))
			//{
			//	return pInterface->Inventory_GetItem(pGlobals->inventorySlots["Shotgun"], item);
			//}
			//if (!pInterface->Inventory_GetItem(pGlobals->inventorySlots["Shotgun"], item))
			//{
			//	return pInterface->Inventory_GetItem(pGlobals->inventorySlots["Pistol"], item);
			//}
			return true;
		}
		else 
			return false;
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

	bool NoHouseInFOV(Elite::Blackboard* pBlackboard)
	{
		GlobalsStruct* pGlobals{ nullptr };
		if (!pBlackboard->GetData("Globals", pGlobals) || pGlobals == nullptr)
			return false;


		if (pGlobals->currentHouse.Center == Elite::Vector2{0,0})
			return true;

		return false;
	}

	bool IsNewHouse(Elite::Blackboard* pBlackboard)
	{
		std::vector<HouseInfo>* pHousesInFOV{ nullptr };
		GlobalsStruct* pGlobals{ nullptr };

		if (!pBlackboard->GetData("HouseInFov", pHousesInFOV) || pHousesInFOV == nullptr)
			return false;
		if (!pBlackboard->GetData("Globals", pGlobals) || pGlobals == nullptr)
			return false;
		
		if (pGlobals->currentHouse.Center != Elite::Vector2{ 0,0 })
		{
			return true; //we already have a current house
		}

		if (pHousesInFOV->size() > 0)
		{
			//check if house has been visited
			for (const auto &h : pGlobals->housesVisited)
			{
				if ((*pHousesInFOV)[0].Center == h.Center)
					return false; //we already visited house
			}
		}

		if ((pGlobals->currentHouse.Center == Elite::Vector2{ 0,0 } && pHousesInFOV->size() > 0))
		{
			pGlobals->currentHouse = (*pHousesInFOV)[0];
			return true; //we see a house but have no current house so we make it current house
		}
		return false;
	}

	bool InHouse(Elite::Blackboard* pBlackboard)
	{
		IExamInterface* pInterface{ nullptr };
		GlobalsStruct* pGlobals{ nullptr };

		if (!pBlackboard->GetData("InterFace", pInterface) || pInterface == nullptr)
			return false;

		if (!pBlackboard->GetData("Globals", pGlobals) || pGlobals == nullptr)
			return false;

		if(pInterface->Agent_GetInfo().IsInHouse || pGlobals->currentHouse.Center != Elite::Vector2{ 0,0 })
		{
			return true;
;		}

		return false;
	}

	bool IsHouseChecked(Elite::Blackboard* pBlackboard)
	{
		IExamInterface* pInterface{ nullptr };
		GlobalsStruct* pGlobals{ nullptr };

		if (!pBlackboard->GetData("InterFace", pInterface) || pInterface == nullptr)
			return false;

		if (!pBlackboard->GetData("Globals", pGlobals) || pGlobals == nullptr)
			return false;

		if (pGlobals->wentLeftBottom)
		{
			return true;
		}
		return false;
	}
}

	

#endif
