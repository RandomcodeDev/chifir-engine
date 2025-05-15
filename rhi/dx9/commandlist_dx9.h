/// \file DirectX 9 command lists (yikes)
/// \copyright 2025 Randomcode Developers

#pragma once

#include "base/allocator.h"
#include "base/async.h"
#include "base/compiler.h"

#include "rhi/irhicommandlist.h"

#include "dx9.h"

class CDx9RhiDevice;

class CDx9RhiCommandList: public IRhiCommandList, CDx9RhiBaseObject<void>
{
  public:
	~CDx9RhiCommandList() = default;

	virtual void Destroy()
	{
		if (m_mutex)
		{
			delete m_mutex;
			m_mutex = nullptr;
		}

		m_allocator.Reset();
	}

	virtual void BeginCommands();
	virtual void EndCommands();

	virtual void SetRenderTarget(u32 index, IRhiRenderTarget* target);
	virtual void SetDepthStencilTarget(IRhiDepthStencilTarget* target);

	virtual void Clear(u32 color, f32 depth, u8 stencil);

  private:
	friend class CDx9RhiDevice;

	IMutex* m_mutex;
	CArenaAllocator m_allocator;

	CDx9RhiCommandList(CDx9RhiDevice* device, ssize bufferSize)
		: CDx9RhiBaseObject(device), m_mutex(Async_CreateMutex()), m_allocator(bufferSize)
	{
	}

	void HandleCommand(u32 type, const u8* buffer, ssize& offset);
	bool Execute();
};
