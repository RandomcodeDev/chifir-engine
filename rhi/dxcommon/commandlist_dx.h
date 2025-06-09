/// \file Legacy DirectX command lists (yikes) using arena allocators
/// \copyright 2025 Randomcode Developers

#pragma once

#include "base/allocator.h"
#include "base/async.h"
#include "base/compiler.h"

#include "rhi/irhicommandlist.h"

#include "dxcommon.h"

class CDxRhiCommandList: public IRhiCommandList, public CDxRhiBaseObject<void>
{
  public:
	~CDxRhiCommandList() = default;

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
	friend CDxRhiDevice;

	IMutex* m_mutex;
	CArenaAllocator m_allocator;

	CDxRhiCommandList(CDxRhiDevice* device, ssize bufferSize)
		: CDxRhiBaseObject(device), m_mutex(Async_CreateMutex()), m_allocator(bufferSize)
	{
	}

	void HandleCommand(u32 type, const u8* buffer, ssize& offset);
	bool Execute();
};
