/// \file DirectX 8 command list implementation (yikes)
/// \copyright 2025 Randomcode Developers

// Commands are stored as their type, followed by a custom struct. Execute goes through that and does the right command.

#include "dx8.h"
#include "rhi/irhiimage.h"

#include "commandlist_dx8.h"
#include "device_dx8.h"
#include "image_dx8.h"

#define DECLARE_COMMAND(name, dataBody, params, ...)                                                                             \
	static constexpr u32 Dx8##name##Command = __LINE__;                                                                          \
	struct Dx8##name##CommandData_t dataBody;                                                                                    \
	void CDx8RhiCommandList::name params                                                                                         \
	{                                                                                                                            \
		using CommandData_t = Dx8##name##CommandData_t;                                                                          \
		u32* type = reinterpret_cast<u32*>(m_allocator.Alloc(sizeof(u32)));                                                      \
		*type = Dx8##name##Command;                                                                                              \
		auto data = static_cast<CommandData_t*>(m_allocator.Alloc(sizeof(CommandData_t)));                                       \
		{                                                                                                                        \
			__VA_ARGS__                                                                                                          \
		}                                                                                                                        \
	}

#define HANDLE_COMMAND(name, ...)                                                                                                \
	case Dx8##name##Command: {                                                                                                   \
		using CommandData_t = Dx8##name##CommandData_t;                                                                          \
		__VA_OPT__(auto data = reinterpret_cast<const CommandData_t*>(buffer + offset);)                                         \
		offset += sizeof(CommandData_t);                                                                                         \
		{                                                                                                                        \
			__VA_ARGS__                                                                                                          \
		}                                                                                                                        \
		break;                                                                                                                   \
	}

DECLARE_COMMAND(
	SetRenderTarget,
	{
		u32 index;
		CDx8RhiRenderTarget* target;
	},
	(u32 index, IRhiRenderTarget* target),
	{
		data->index = index;
		data->target = reinterpret_cast<CDx8RhiRenderTarget*>(target);
	})
DECLARE_COMMAND(SetDepthStencilTarget, { CDx8RhiRenderTarget* target; }, (IRhiDepthStencilTarget * target), { UNUSED(data); })
DECLARE_COMMAND(
	Clear,
	{
		u32 color;
		f32 depth;
		u8 stencil;
	},
	(u32 color, f32 depth, u8 stencil),
	{
		data->color = RgbaToArgb(color);
		data->depth = depth;
		data->stencil = stencil;
	})

void CDx8RhiCommandList::BeginCommands()
{
	m_mutex->Lock();
	m_allocator.Reset();
}

void CDx8RhiCommandList::EndCommands()
{
	m_mutex->Unlock();
}

void CDx8RhiCommandList::HandleCommand(u32 type, const u8* buffer, ssize& offset)
{
	auto device = m_parent->m_handle;

	switch (type)
	{
		HANDLE_COMMAND(SetRenderTarget, { device->SetRenderTarget(data->target->GetHandle(), nullptr); })
		HANDLE_COMMAND(Clear, { device->Clear(0, nullptr, D3DCLEAR_TARGET, data->color, data->depth, data->stencil); })
	}
}

bool CDx8RhiCommandList::Execute()
{
	if (!m_mutex->TryLock())
	{
		return false;
	}

	auto buffer = static_cast<u8*>(m_allocator.GetBase());
	ssize offset = 0;

	while (offset < m_allocator.GetSize())
	{
		u32 type = *reinterpret_cast<u32*>(buffer + offset);
		offset += sizeof(u32);
		HandleCommand(type, buffer, offset);
	}

	m_mutex->Unlock();

	return true;
}
