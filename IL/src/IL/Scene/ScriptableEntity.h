#pragma once

#include "Entity.h"

namespace IL
{

	class SciptableEntity
	{
	public:
		virtual ~SciptableEntity() = default;

		template <typename T>
		inline T& GetComponent()
		{
			return m_Entity.GetComponent<T>();
		}
		virtual std::string FeedbackScriptName() { return "No name"; }
	protected:
		virtual void OnUpdate(TimeStep dt) {}
		virtual void OnEvent(Event& e) {}
		virtual void OnCreate() {}
		virtual void OnDestroy() {}
	private:
		Entity m_Entity;

		friend class Scene;
	};

}