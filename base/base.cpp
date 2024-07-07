#include "base/base.h"
#include "base.h"
#include "base/compiler.h"

bool g_baseInitialized;
BaseCpuData_t g_cpuData;

#ifdef CF_X86
static void CpuId(u32& eax, u32& ebx, u32& ecx, u32& edx)
{
#ifdef _MSC_VER
	s32 regs[4] = {0};
	__cpuid(regs, eax);
	eax = regs[0];
	ebx = regs[1];
	ecx = regs[2];
	edx = regs[3];
#else
	__get_cpuid(eax, &eax, &ebx, &ecx, &edx);
#endif
}

static void X86InitCpuData()
{
	u32 eax = 0x00000001;
	u32 ebx = 0x00000000;
	u32 ecx = 0x00000000;
	u32 edx = 0x00000000;
	CpuId(eax, ebx, ecx, edx);

	// Because of bit field shenanigans, these have to be made booleans first
#ifdef CH_I386 // Widespread adoption of SSE2 predates AMD64
	g_cpuData.haveSimd128 = (bool)(edx & (1 << 25));
	g_cpuData.haveIntSimd128 = (bool)(edx & (1 << 26));
#endif
	g_cpuData.haveSimd256 = (bool)(ecx & (1 << 28));

	u32 regs[16] = {0}; // 0-3 brand, 4-15 model

	// brand, EAX = 0
	CpuId(regs[0], regs[1], regs[2], regs[3]);
	Base_MemCpy(g_cpuData.brand, regs, 16);

	// model name available
	regs[4] = 0x80000000;
	CpuId(regs[4], regs[5], regs[6], regs[7]);
	g_cpuData.haveName = regs[4] >= 0x80000004;

	if (g_cpuData.haveName)
	{
		regs[4] = 0x80000002;
		regs[8] = 0x80000003;
		regs[12] = 0x80000004;
		CpuId(regs[4], regs[5], regs[6], regs[7]);
		CpuId(regs[8], regs[9], regs[10], regs[11]);
		CpuId(regs[12], regs[13], regs[14], regs[15]);

		Base_MemCpy(g_cpuData.name, regs + 4, 48);
	}
}
#endif

BASEAPI void Base_Init()
{
#ifdef CF_X86
	X86InitCpuData();
#endif

	g_baseInitialized = true;
}

BASEAPI void Base_Shutdown()
{
}

template <typename T> static inline T Fnv1a(const u8* data, usize size, T offsetBasis, T prime)
{
	T hash = offsetBasis;
	for (usize i = 0; i < size; i++)
	{
		hash ^= data[i];
		hash *= prime;
	}

	return hash;
}

BASEAPI u32 Base_Fnv1a32(const void* data, usize size)
{
	static const u32 OFFSET_BASIS = 0x811c9dc5;
	static const u32 PRIME = 0x01000193;

	return Fnv1a((u8*)data, size, OFFSET_BASIS, PRIME);
}

BASEAPI u64 Base_Fnv1a64(const void* data, usize size)
{
	static const u64 OFFSET_BASIS = 0xcbf29ce484222325;
	static const u64 PRIME = 0x00000100000001B3;

	return Fnv1a((u8*)data, size, OFFSET_BASIS, PRIME);
}

template <typename T>
static inline void Copy(void* RESTRICT dest, const void* RESTRICT src, usize offset, usize& remaining, usize alignment)
{
	for (usize i = offset; i < (remaining / alignment) * alignment; i += alignment)
	{
		((T*)dest)[i / alignment] = ((T*)src)[i / alignment];
		remaining -= alignment;
	}
}

BASEAPI void* Base_MemCpy(void* RESTRICT dest, const void* RESTRICT src, usize size)
{
	usize remaining = size;

	if (g_baseInitialized)
	{
		usize alignment = 1;
		if (g_cpuData.haveSimd256 && size >= 32)
		{
			alignment = 32;
		}
		else if (g_cpuData.haveSimd128 && size >= 16)
		{
			alignment = 16;
		}
		else if (size >= 8)
		{
			alignment = 8;
		}

		// Can only realign if they're misaligned the same amount (they should be, usually pointers are aligned on a reasonable
		// amount unless it's some arbitrary offset into an array of bytes)
		usize srcAlignment = ((uptr)src & alignment);
		usize destAlignment = ((uptr)dest & alignment);
		if (srcAlignment == destAlignment)
		{
			usize misalignment = alignment - srcAlignment;
			remaining -= misalignment;
			Copy<u8>(dest, src, 0, misalignment, 1);

#ifdef CH_SIMD256
			if (g_cpuData.haveSimd256 && size - remaining >= 32)
			{
				Copy<v256>(dest, src, size - remaining, remaining, alignment);
			}
			else
#endif
#ifdef CH_SIMD128
				if (g_cpuData.haveSimd128 && size - remaining >= 16)
			{
				Copy<v128>(dest, src, size - remaining, remaining, alignment);
			}
			else
#endif
				if (size - remaining >= 8)
			{
				Copy<u64>(dest, src, size - remaining, remaining, alignment);
			}
		}
	}

	Copy<u8>(dest, src, size - remaining, remaining, 1);

	return dest;
}

template <typename T> static inline void Set(void* RESTRICT dest, u8 value, usize offset, usize& remaining, usize alignment)
{
	// This is to get around how Clang deals with SIMD intrinsics (shouldn't cause problems)
	u8 fullValue[32] = {value, value, value, value, value, value, value, value, value, value, value,
						value, value, value, value, value, value, value, value, value, value, value,
						value, value, value, value, value, value, value, value, value, value};
	for (usize i = offset; i < (remaining / alignment) * alignment; i += alignment)
	{
		((T*)dest)[i / alignment] = *(T*)fullValue;
		remaining -= alignment;
	}
}

BASEAPI void* Base_MemSet(void* dest, u32 value, usize size)
{
	usize remaining = size;

	if (g_baseInitialized)
	{
		usize alignment = 1;
		if (g_cpuData.haveSimd256 && size >= 32)
		{
			alignment = 32;
		}
		else if (g_cpuData.haveSimd128 && size >= 16)
		{
			alignment = 16;
		}
		else if (size >= 8)
		{
			alignment = 8;
		}

		// Realign
		usize misalignment = alignment - ((uptr)dest & alignment);
		remaining -= misalignment;
		Set<u8>(dest, static_cast<u8>(value), 0, misalignment, 1);

#ifdef CH_SIMD256
		if (g_cpuData.haveSimd256 && size - remaining >= 32)
		{
			Set<v256>(dest, static_cast<u8>(value), size - remaining, remaining, alignment);
		}
		else
#endif
#ifdef CH_SIMD128
			if (g_cpuData.haveSimd128 && size - remaining >= 16)
		{
			Set<v128>(dest, static_cast<u8>(value), size - remaining, remaining, alignment);
		}
		else
#endif
			if (size - remaining >= 8)
		{
			Set<u64>(dest, static_cast<u8>(value), size - remaining, remaining, alignment);
		}
	}

	Set<u8>(dest, static_cast<u8>(value), size - remaining, remaining, 1);

	return dest;
}
