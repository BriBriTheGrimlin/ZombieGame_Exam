#pragma once
#include "Exam_HelperStructs.h"
#include "SteeringBehaviors.h"
#include "IExamInterface.h"

	class SteeringBehavior final
	{
	public:
		SteeringBehavior(IExamInterface* pInterface, SteeringPlugin_Output* pSteeringbh);
		~SteeringBehavior() = default;

		

		void Seek(const Elite::Vector2 target) const;
		void Flee(const Elite::Vector2& target, float radius) const;
		void Face(const Elite::Vector2 target) const;

	private:
		IExamInterface* m_pInterface = nullptr;
		SteeringPlugin_Output* m_pSteeringbh = nullptr;
	};


#pragma endregion