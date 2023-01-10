#include "stdafx.h"
#include "Plugin.h"
#include "IExamInterface.h"
#include "EBehaviorTree.h"
#include "Behaviors.h"

using namespace std;
using namespace Elite;

//Called only once, during initialization
void Plugin::Initialize(IBaseInterface* pInterface, PluginInfo& info)
{
	//Retrieving the interface
	//This interface gives you access to certain actions the AI_Framework can perform for you
	m_pInterface = static_cast<IExamInterface*>(pInterface);

	//Bit information about the plugin
	//Please fill this in!!
	info.BotName = "TheGrimlin";
	info.Student_FirstName = "Brian";
	info.Student_LastName = "Hoet";
	info.Student_Class = "2DAE07";

	m_pSteeringOutputData = new SteeringPlugin_Output();
	m_pEntitiesInFOV = new std::vector<EntityInfo>();
	m_pHousesInFOV = new std::vector<HouseInfo>();
	m_pMyGlobals = new GlobalsStruct();
	m_pSteeringBehavior = new SteeringBehavior(m_pInterface, m_pSteeringOutputData);

	m_pBlackboard = new Elite::Blackboard();
	m_pBlackboard->AddData("InterFace", m_pInterface);
	m_pBlackboard->AddData("SteeringBehavior", m_pSteeringBehavior);
	m_pBlackboard->AddData("EntityInFov", m_pEntitiesInFOV);
	m_pBlackboard->AddData("HouseInFov", m_pHousesInFOV);
	m_pBlackboard->AddData("SteeringOutput", m_pSteeringOutputData);
	m_pBlackboard->AddData("Currenthouse", m_pCurrentHouse);
	m_pBlackboard->AddData("Globals", m_pMyGlobals);

	m_pBehaviorTree = new Elite::BehaviorTree(m_pBlackboard,
		new BehaviorSelector
		(
			{
				 new BehaviorSequence
				 (
				   {
						 // Is PurgeZone in Fov													
						 new BehaviorConditional(&BT_Conditions::IsPurgeZoneInFOV),
						 // Run from Purgezone													
						 new BehaviorAction(&BT_Actions::ChangeToFlee)
				   }
				 ),
				 new BehaviorSequence
				 (
				   {
						 //Is Enemy in Fov														
						 new BehaviorConditional(&BT_Conditions::IsEnemyInFOV),

						 new BehaviorSelector
						 (
							{
							 new BehaviorConditional(&BT_Conditions::HaveGun),
							 new BehaviorAction(&BT_Actions::ShootZombieOrRun)
							}
						 ),
				   }
				 ),
				new BehaviorSequence(
					{
						 new BehaviorSequence
						 	 (
						 		{
						 		 new BehaviorConditional(&BT_Conditions::IsHouseInFOV),
								// new BehaviorInvertConditional( &BT_Conditions::AgentInHouse),
						 		 new BehaviorAction(&BT_Actions::GoInsideHouse)
						 		}
						 	 ),
						 new BehaviorSequence
					 		 (
					 		 	{
					 		 		new BehaviorConditional(&BT_Conditions::AgentInHouse),
					 		 		new BehaviorAction(&BT_Actions::CheckHouse)
					 		 	}
					 		 ),
					}
				),
			new BehaviorSequence(
				{
					new BehaviorInvertConditional(&BT_Conditions::IsHouseInFOV),
					new BehaviorAction(&BT_Actions::Explore)
				}
				),
			 new BehaviorSequence
							 (
							 	{
							 		new BehaviorConditional(&BT_Conditions::LootInFOV),
							 		new BehaviorAction(&BT_Actions::LootFOV)
							 	}
							 ),
					







			}
		)

		);
					
			
}


//Called only once
void Plugin::DllInit()
{
	//Called when the plugin is loaded
}

//Called only once
void Plugin::DllShutdown()
{
	//Called wheb the plugin gets unloaded
}

//Called only once, during initialization
void Plugin::InitGameDebugParams(GameDebugParams& params)
{
	params.AutoFollowCam = true; //Automatically follow the AI? (Default = true)
	params.RenderUI = true; //Render the IMGUI Panel? (Default = true)
	params.SpawnEnemies = true; //Do you want to spawn enemies? (Default = true)
	params.EnemyCount = 20; //How many enemies? (Default = 20)
	params.GodMode = false; //GodMode > You can't die, can be useful to inspect certain behaviors (Default = false)
	params.LevelFile = "GameLevel.gppl";
	params.AutoGrabClosestItem = true; //A call to Item_Grab(...) returns the closest item that can be grabbed. (EntityInfo argument is ignored)
	params.StartingDifficultyStage = 1;
	params.InfiniteStamina = false;
	params.SpawnDebugPistol = true;
	params.SpawnDebugShotgun = true;
	params.SpawnPurgeZonesOnMiddleClick = true;
	params.PrintDebugMessages = true;
	params.ShowDebugItemNames = true;
	params.Seed = 2;	//16
}

//Only Active in DEBUG Mode
//(=Use only for Debug Purposes)
//above only for debug
void Plugin::Update(float dt)
{
	//Demo Event Code
	//In the end your AI should be able to walk around without external input
	if (m_pInterface->Input_IsMouseButtonUp(Elite::InputMouseButton::eLeft))
	{
		//Update target based on input
		Elite::MouseData mouseData = m_pInterface->Input_GetMouseData(Elite::InputType::eMouseButton, Elite::InputMouseButton::eLeft);
		const Elite::Vector2 pos = Elite::Vector2(static_cast<float>(mouseData.X), static_cast<float>(mouseData.Y));
		m_Target = m_pInterface->Debug_ConvertScreenToWorld(pos);
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Space))
	{
		m_CanRun = true;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Left))
	{
		m_AngSpeed -= Elite::ToRadians(10);
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Right))
	{
		m_AngSpeed += Elite::ToRadians(10);
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_G))
	{
		m_GrabItem = true;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_U))
	{
		m_UseItem = true;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_R))
	{
		m_RemoveItem = true;
	}
	else if (m_pInterface->Input_IsKeyboardKeyUp(Elite::eScancode_Space))
	{
		m_CanRun = false;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Delete))
	{
		m_pInterface->RequestShutdown();
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_KP_Minus))
	{
		if (m_InventorySlot > 0)
			--m_InventorySlot;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_KP_Plus))
	{
		if (m_InventorySlot < 4)
			++m_InventorySlot;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Q))
	{
		ItemInfo info = {};
		m_pInterface->Inventory_GetItem(m_InventorySlot, info);
		std::cout << (int)info.Type << std::endl;
	}

	
}

//Update
//This function calculates the new SteeringOutput, called once per frame
SteeringPlugin_Output Plugin::UpdateSteering(float dt)
{
	auto steering = SteeringPlugin_Output();
	
	//Use the Interface (IAssignmentInterface) to 'interface' with the AI_Framework
	auto agentInfo = m_pInterface->Agent_GetInfo();

	*m_pHousesInFOV = GetHousesInFOV();//uses m_pInterface->Fov_GetHouseByIndex(...)
	*m_pEntitiesInFOV = GetEntitiesInFOV(); //uses m_pInterface->Fov_GetEntityByIndex(...)

	m_pBehaviorTree->Update(dt);

	//Use the navmesh to calculate the next navmesh point
	Elite::Vector2 checkpointLocation{};
	m_nextTargetPos = m_pInterface->NavMesh_GetClosestPathPoint(checkpointLocation);

	//auto nextTargetPos = m_pInterface->NavMesh_GetClosestPathPoint(m_Target); //Uncomment this to use mouse position as guidance

	
	/*
	bool seenItem{ false };
	bool seenEnemy{ false };

	steering.AutoOrient = true;

	m_pInterface->Draw_Point(m_nextTargetPos, 50, { 1,0,0 });

	//UseItems
	ItemInfo item;
	if (m_pInterface->Inventory_GetItem(4, item))
	{
		if (agentInfo.Health <= agentInfo.Health - 4)
		{
			m_pInterface->Inventory_UseItem(4);
		}
	}
	
	if (m_pInterface->Inventory_GetItem(2, item))
	{
		if (agentInfo.Energy <= agentInfo.Energy - 6)
		{
			m_pInterface->Inventory_UseItem(2);
		}
	}
	if(agentInfo.WasBitten)
	{
		m_CanRun = true;
		
	}
	if (agentInfo.Stamina <= 0)
	{
		m_CanRun = false;
	}
	

	for (auto& e : vEntitiesInFOV)
	{
		if (e.Type == eEntityType::PURGEZONE)
		{
			PurgeZoneInfo zoneInfo;
			m_pInterface->PurgeZone_GetInfo(e, zoneInfo);
			//std::cout << "Purge Zone in FOV:" << e.Location.x << ", "<< e.Location.y << "---Radius: "<< zoneInfo.Radius << std::endl;
		}

		if (e.Type == eEntityType::ENEMY)
		{
			auto EnemyInFOV{ GetEntitiesInFOV()[0] };
			checkpointLocation = agentInfo.Position - EnemyInFOV.Location;
			//nextTargetPos = m_pInterface->NavMesh_GetClosestPathPoint(checkpointLocation);
			seenEnemy = true;
			EnemyInfo enemyInfo{};
			m_pInterface->Enemy_GetInfo(e, enemyInfo);

			if (enemyInfo.Type == eEnemyType::ZOMBIE_NORMAL)
			{
				steering.AutoOrient = false;
				m_nextTargetPos = m_pInterface->NavMesh_GetClosestPathPoint(checkpointLocation);

				

				auto xdifference = enemyInfo.Location.x - agentInfo.Position.x;
				auto ydifference = enemyInfo.Location.y - agentInfo.Position.y;
				steering.AngularVelocity = atan(ydifference / xdifference) ;
				auto angleBetweenAgentAndEnemy = atan(ydifference / xdifference);

				std::cout << agentInfo.Orientation + angleBetweenAgentAndEnemy << '\n';

				auto distance{ sqrtf(powf(enemyInfo.Location.x - agentInfo.Position.x, 2) + powf(enemyInfo.Location.y - agentInfo.Position.y, 2)) };

				if(agentInfo.Orientation > angleBetweenAgentAndEnemy && agentInfo.Orientation <= angleBetweenAgentAndEnemy)
				{
					ItemInfo item;
					if (distance <= agentInfo.FOV_Range/3 && m_pInterface->Inventory_GetItem(0,item ))
					{
						m_pInterface->Inventory_UseItem(0);
					}
					else
					{
						m_pInterface->Inventory_UseItem(1);
					}
					
					
				}
				
			}
			if (enemyInfo.Type == eEnemyType::DEFAULT)
			{
				
				std::cout << "Yeet2\n";
			}
			if (enemyInfo.Type == eEnemyType::RANDOM_ENEMY)
			{
				std::cout << "Yeet3\n";
			}
			if (enemyInfo.Type == eEnemyType::ZOMBIE_HEAVY)
			{
				std::cout << "Yeet4\n";
			}
			if (enemyInfo.Type == eEnemyType::ZOMBIE_RUNNER)
			{
				std::cout << "Yeet5\n";
			}
		}

		if(e.Type == eEntityType::ITEM)
		{
			seenItem = true;
			if(!seenEnemy)
			{
			auto itemInFOV{ GetEntitiesInFOV()[0]};
			checkpointLocation = itemInFOV.Location;
			m_nextTargetPos = m_pInterface->NavMesh_GetClosestPathPoint(checkpointLocation);
				if (agentInfo.Position.Distance(e.Location) <= agentInfo.GrabRange)
				{
					ItemInfo item{};
					m_pInterface->Item_GetInfo(e, item);

					if (item.Type == eItemType::SHOTGUN)
					{

						m_pInterface->Item_Grab(e, item);
						m_pInterface->Inventory_AddItem(0, item);
					}
					if (item.Type == eItemType::PISTOL)
					{
						m_pInterface->Item_Grab(e, item);
						m_pInterface->Inventory_AddItem(1, item);
					}
					if (item.Type == eItemType::FOOD)
					{
						if (!m_pInterface->Inventory_GetItem(2, item))
						{
							m_pInterface->Item_Grab(e, item);
							m_pInterface->Inventory_AddItem(2, item);
						}
						if (m_pInterface->Inventory_GetItem(2, item))
						{
							m_pInterface->Item_Grab(e, item);
							m_pInterface->Inventory_AddItem(3, item);
						}
					}
					if (item.Type == eItemType::MEDKIT)
					{
						if (!m_pInterface->Inventory_GetItem(4, item))
						{
							m_pInterface->Item_Grab(e, item);
							m_pInterface->Inventory_AddItem(4, item);
						}
						else
						{
							m_pInterface->Item_Destroy(e);
						}
					}
					if (item.Type == eItemType::GARBAGE)
					{
						m_pInterface->Item_Destroy(e);
					}
					seenItem = false;

				}
			}
		}
	}
		
		

	for (auto& h : vHousesInFOV)
	{
		auto houseInView = GetHousesInFOV()[0];
	
		checkpointLocation = houseInView.Center;
		if (!seenItem)
		{
			m_nextTargetPos = m_pInterface->NavMesh_GetClosestPathPoint(checkpointLocation);
			steering.LinearVelocity = m_nextTargetPos - agentInfo.Position; //Desired Velocity
			steering.LinearVelocity.Normalize(); //Normalize Desired Velocity
			steering.LinearVelocity *= agentInfo.MaxLinearSpeed;
			if (Elite::Distance(agentInfo.Position, m_nextTargetPos) <= 2)
			{
				steering.AutoOrient = false;
				steering.AngularVelocity = 1.f;
			}
		}
	}

	if (GetHousesInFOV().size() == 0)
	{
		m_nextTargetPos = m_pInterface->NavMesh_GetClosestPathPoint(agentInfo.Position - m_pInterface->World_GetInfo().Dimensions / 2);
	}
	*/

	//INVENTORY USAGE DEMO
	//********************

	if (m_GrabItem)
	{
		ItemInfo item;
		//Item_Grab > When DebugParams.AutoGrabClosestItem is TRUE, the Item_Grab function returns the closest item in range
		//Keep in mind that DebugParams are only used for debugging purposes, by default this flag is FALSE
		//Otherwise, use GetEntitiesInFOV() to retrieve a vector of all entities in the FOV (EntityInfo)
		//Item_Grab gives you the ItemInfo back, based on the passed EntityHash (retrieved by GetEntitiesInFOV)
		if (m_pInterface->Item_Grab({}, item))
		{
			//Once grabbed, you can add it to a specific inventory slot
			//Slot must be empty
			m_pInterface->Inventory_AddItem(m_InventorySlot, item);
		}
	}

	if (m_UseItem)
	{
		//Use an item (make sure there is an item at the given inventory slot)
		m_pInterface->Inventory_UseItem(m_InventorySlot);
	}

	if (m_RemoveItem)
	{
		//Remove an item from a inventory slot
		m_pInterface->Inventory_RemoveItem(m_InventorySlot);
	}

	//Simple Seek Behaviour (towards Target)
	steering.LinearVelocity = m_nextTargetPos - agentInfo.Position; //Desired Velocity
	steering.LinearVelocity.Normalize(); //Normalize Desired Velocity
	steering.LinearVelocity *= agentInfo.MaxLinearSpeed; //Rescale to Max Speed

	if (Distance(m_nextTargetPos, agentInfo.Position) < 2.f)
	{
		steering.LinearVelocity = Elite::ZeroVector2;
	}

	//steering.AngularVelocity = m_AngSpeed; //Rotate your character to inspect the world while walking
	//steering.AutoOrient = true; //Setting AutoOrient to TRue overrides the AngularVelocity

	steering.RunMode = m_CanRun; //If RunMode is True > MaxLinSpd is increased for a limited time (till your stamina runs out)

	//SteeringPlugin_Output is works the exact same way a SteeringBehaviour output

//@End (Demo Purposes)
	m_GrabItem = false; //Reset State
	m_UseItem = false;
	m_RemoveItem = false;

	return *m_pSteeringOutputData;
}

//This function should only be used for rendering debug elements
void Plugin::Render(float dt) const
{
	//This Render function should only contain calls to Interface->Draw_... functions
	//m_pInterface->Draw_SolidCircle(m_Target, .7f, { 0,0 }, { 1, 0, 0 });

	//m_pInterface->Draw_Circle(m_nextTargetPos, 3, { 1,0,0 });
	
}

vector<HouseInfo> Plugin::GetHousesInFOV() const
{
	vector<HouseInfo> vHousesInFOV = {};

	HouseInfo hi = {};
	for (int i = 0;; ++i)
	{
		if (m_pInterface->Fov_GetHouseByIndex(i, hi))
		{
			vHousesInFOV.push_back(hi);
			continue;
		}

		break;
	}

	return vHousesInFOV;
}

vector<EntityInfo> Plugin::GetEntitiesInFOV() const
{
	vector<EntityInfo> vEntitiesInFOV = {};

	EntityInfo ei = {};
	for (int i = 0;; ++i)
	{
		if (m_pInterface->Fov_GetEntityByIndex(i, ei))
		{
			vEntitiesInFOV.push_back(ei);
			continue;
		}

		break;
	}

	return vEntitiesInFOV;
}
