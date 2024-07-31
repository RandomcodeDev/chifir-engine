// Engine implementation class

#pragma once

#include "iapplication.h"
#include "engine/engine.h"

class IVideoSystem;

class CEngine : public IEngine
{
  public:
	CEngine() DEFAULT;
	~CEngine() DEFAULT;

	void GetRequiredSystems(CVector<SystemDependency_t>& dependencies);
	s32 Run(const CVector<ISystem*>& systems /* TODO: , CCommandLine cmdLine */);

	EngineState_t GetState()
	{
		return m_state;
	}

	void Quit()
	{
		m_state = EngineStateShutdown;
	}

  private:
	EngineState_t m_state;

	IVideoSystem* m_videoSystem;
};
