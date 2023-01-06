#pragma once
#include "Exam_HelperStructs.h"
#include "SteeringBehaviors.h"

#pragma region **ISTEERINGBEHAVIOR** (BASE)
	class ISteeringBehavior
	{
	public:
		ISteeringBehavior() = default;
		virtual ~ISteeringBehavior() = default;

		virtual SteeringPlugin_Output CalculateSteering(float deltaT, AgentInfo* pAgent) = 0;

		//Seek Functions
		void SetTarget(const Elite::Vector2& target) { m_Target = target; }

		template<class T, typename std::enable_if<std::is_base_of<ISteeringBehavior, T>::value>::type* = nullptr>
		T* As()
		{
			return static_cast<T*>(this);
		}

	protected:
		Elite::Vector2 m_Target;
	};
#pragma endregion

	///////////////////////////////////////
	//SEEK
	//****
	class Seek : public ISteeringBehavior
	{
	public:
		Seek() = default;
		virtual ~Seek() = default;

		//Seek Behaviour
		SteeringPlugin_Output CalculateSteering(float deltaT, AgentInfo* pAgent) override;
	};

	
	///////////////////////////////////////
	//FLEE
	//****
	class Flee : public ISteeringBehavior
	{
	public:
		Flee() = default;
		virtual ~Flee() = default;

		//Seek behavior
		SteeringPlugin_Output CalculateSteering(float deltaT, AgentInfo* pAgent) override;
		void SetFleeRadius(float fleeRadius) { m_FleeRadius = fleeRadius; }
	private:
		float m_FleeRadius = 20.f;
	};

	///////////////////////////////////////
	//ARRIVE
	//****
	class Arrive : public ISteeringBehavior
	{
	public:
		Arrive() = default;
		virtual ~Arrive() = default;

		SteeringPlugin_Output CalculateSteering(float deltaT, AgentInfo* pAgent) override;
	};

	///////////////////////////////////////
	//FACE
	//****
	class Face : public ISteeringBehavior
	{
	public:
		Face() = default;
		virtual ~Face() = default;

		SteeringPlugin_Output CalculateSteering(float deltaT, AgentInfo* pAgent) override;
	};

	///////////////////////////////////////
	//EVADE
	//****
	class Evade : public ISteeringBehavior
	{
	public:
		Evade() = default;
		virtual ~Evade() = default;

		SteeringPlugin_Output CalculateSteering(float deltaT, AgentInfo* pAgent) override;
	};



