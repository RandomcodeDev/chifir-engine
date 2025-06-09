/// \file Legacy DirectX command list implementation (yikes) using arena allocators
/// \copyright 2025 Randomcode Developers

// Commands are stored as their type, followed by a custom struct. Execute goes through that and does the right command.

#include "rhi/irhiimage.h"

#include "commandlist_dx.h"
#include "dxcommon.h"
#include "dxdefs.h"

#define DECLARE_COMMAND(name, dataBody, params, ...)                                                                             \
    static constexpr u32 Dx##name##Command = __LINE__;                                                                           \
    struct Dx##name##CommandData_t dataBody;                                                                                     \
    void CDxRhiCommandList::name params                                                                                          \
    {                                                                                                                            \
        using CommandData_t = Dx##name##CommandData_t;                                                                           \
        u32* type = reinterpret_cast<u32*>(m_allocator.Alloc(sizeof(u32)));                                                      \
        *type = Dx##name##Command;                                                                                               \
        auto data = static_cast<CommandData_t*>(m_allocator.Alloc(sizeof(CommandData_t)));                                       \
        {                                                                                                                        \
            __VA_ARGS__                                                                                                          \
        }                                                                                                                        \
    }

#define HANDLE_COMMAND(name, ...)                                                                                                \
    case Dx##name##Command: {                                                                                                    \
        using CommandData_t = Dx##name##CommandData_t;                                                                           \
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
        CDxRhiRenderTarget* target;
    },
    (u32 index, IRhiRenderTarget* target),
    {
        data->target = reinterpret_cast<CDxRhiRenderTarget*>(target);
    })
DECLARE_COMMAND(SetDepthStencilTarget, { CDxRhiRenderTarget* target; }, (IRhiDepthStencilTarget * target), { UNUSED(data); })
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

void CDxRhiCommandList::BeginCommands()
{
    m_mutex->Lock();
    m_allocator.Reset();
}

void CDxRhiCommandList::EndCommands()
{
    m_mutex->Unlock();
}

// TODO: move this back out to individual backends
void CDxRhiCommandList::HandleCommand(u32 type, const u8* buffer, ssize& offset)
{
    auto device = m_parent->m_handle;

    switch (type)
    {
#ifdef IN_RHI_DIRECTX9
        HANDLE_COMMAND(SetRenderTarget, { device->SetRenderTarget(0, data->target->GetHandle()); })
#elif defined IN_RHI_DIRECTX8
        HANDLE_COMMAND(SetRenderTarget, { device->SetRenderTarget(data->target->GetHandle(), nullptr); })
#endif
        HANDLE_COMMAND(Clear, { device->Clear(0, nullptr, D3DCLEAR_TARGET, data->color, data->depth, data->stencil); })
    }
}

bool CDxRhiCommandList::Execute()
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
