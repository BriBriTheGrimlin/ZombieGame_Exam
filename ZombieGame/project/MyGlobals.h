#pragma once
#include "Exam_HelperStructs.h"
#include <map>

struct GlobalsStruct
{
	//House Logic
	HouseInfo currentHouse{ { 0,0 } };
	bool wentRighttop{ false };
	bool wentLeftBottom{ false };
	bool wentToCenter{ false };
	std::vector<HouseInfo> housesVisited;
	
	//Exploration Logic
	bool bottomLeftExplored{ false };
	bool topLeftExplored{ false };
	bool topRightExplored{ false };
	bool bottomRightExplored{ false };
	bool CenterExplored{ false };

	//Inventory
	std::map<std::string, int> inventorySlots;
};