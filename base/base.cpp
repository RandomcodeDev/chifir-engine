#include "base/base.h"
#include "base.h"
#include "base/compiler.h"

bool g_baseInitialized;
bool g_platInitialized;
bool g_allocUsable;
BaseCpuData_t g_cpuData;

#ifdef CH_X86
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
	// Processor info and feature bits
	u32 eax = 0x00000001;
	u32 ebx = 0x00000000;
	u32 ecx = 0x00000000;
	u32 edx = 0x00000000;
	CpuId(eax, ebx, ecx, edx);

	// Because of bit field shenanigans, these have to be made booleans first
#ifdef CH_IA32 // Widespread adoption of SSE2 predates AMD64
	g_cpuData.haveSimd128 = (bool)(edx & (1 << 25));
	g_cpuData.haveIntSimd128 = (bool)(edx & (1 << 26));
#else
	g_cpuData.haveSimd128 = true;
	g_cpuData.haveIntSimd128 = true;
#endif
	g_cpuData.haveSimd128Compare = (bool)(ecx & (1 << 20));
	g_cpuData.haveSimd256 = (bool)(ecx & (1 << 28));

	u32 regs[16] = {0}; // 0-3 brand, 4-15 model

	// brand, EAX = 0
	CpuId(regs[0], regs[1], regs[2], regs[3]);
	Base_MemCopy(g_cpuData.brand, regs, 16);
	g_cpuData.brand[11] = '\0';

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

		Base_MemCopy(g_cpuData.name, regs + 4, 48);
		g_cpuData.name[47] = '\0';
	}
}
#elif defined CH_XBOX360
static void Xbox360InitCpuData()
{
	g_cpuData.haveSimd128 = true;
	g_cpuData.haveIntSimd128 = true;
	g_cpuData.haveSimd128Compare = true;
}
#endif

BASEAPI void Base_Init()
{
#ifdef CH_X86
	X86InitCpuData();
#elif defined CH_XBOX360
	Xbox360InitCpuData();
#endif

	g_baseInitialized = true;
}

BASEAPI void Base_Shutdown()
{
}

template <typename T> static FORCEINLINE T Fnv1a(const u8* data, ssize size, T offsetBasis, T prime)
{
	T hash = offsetBasis;
	for (ssize i = 0; i < size; i++)
	{
		hash ^= data[i];
		hash *= prime;
	}

	return hash;
}

BASEAPI u32 Base_Fnv1a32(const void* data, ssize size)
{
	static const u32 OFFSET_BASIS = 0x811c9dc5;
	static const u32 PRIME = 0x01000193;

	return Fnv1a((u8*)data, size, OFFSET_BASIS, PRIME);
}

BASEAPI u64 Base_Fnv1a64(const void* data, ssize size)
{
	static const u64 OFFSET_BASIS = 0xcbf29ce484222325;
	static const u64 PRIME = 0x00000100000001B3;

	return Fnv1a((u8*)data, size, OFFSET_BASIS, PRIME);
}

template <typename T>
static FORCEINLINE void Copy(
	void* RESTRICT dest, const void* RESTRICT src, ssize offset, ssize& remaining, ssize alignment, bool reverse = false)
{
	ssize count = (remaining / alignment) * alignment;
	if (reverse)
	{
		for (ssize i = offset + count - alignment; i >= offset; i -= alignment)
		{
			static_cast<T*>(dest)[i / alignment] = static_cast<const T*>(src)[i / alignment];
			remaining -= alignment;
		}
	}
	else
	{
		for (ssize i = offset; i < count; i += alignment)
		{
			static_cast<T*>(dest)[i / alignment] = static_cast<const T*>(src)[i / alignment];
			remaining -= alignment;
		}
	}
}

BASEAPI void* Base_MemCopy(void* RESTRICT dest, const void* RESTRICT src, ssize size)
{
	ssize remaining = size;

	if (!dest || !src || size == 0)
	{
		return dest;
	}

	// No point copying
	if (src == dest)
	{
		return dest;
	}

	// Check if data should be copied in reverse in case of overlap
	bool reverse = reinterpret_cast<uptr>(dest) > reinterpret_cast<uptr>(src);

	ssize alignment = 1;
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
	ssize srcAlignment = ((uptr)src & alignment);
	ssize destAlignment = ((uptr)dest & alignment);
	if (srcAlignment == destAlignment)
	{
		ssize misalignment = alignment - srcAlignment;
		if (misalignment > remaining)
		{
			misalignment = remaining;
			remaining = 0;
		}
		else
		{
			remaining -= misalignment;
		}

		// Do this afterward
		if (!reverse)
		{
			Copy<u8>(dest, src, 0, misalignment, 1, reverse);
		}

		// In the event of reverse, this copies from the end of the buffers to either the start or the first aligned point
#ifdef CH_SIMD256
		if (g_cpuData.haveSimd256 && size - remaining >= 32)
		{
			Copy<v256>(dest, src, size - remaining, remaining, alignment, reverse);
		}
		else
#endif
#ifdef CH_SIMD128
			if (g_cpuData.haveSimd128 && size - remaining >= 16)
		{
			Copy<v128>(dest, src, size - remaining, remaining, alignment, reverse);
		}
		else
#endif
			if (size - remaining >= 8)
		{
			Copy<u64>(dest, src, size - remaining, remaining, alignment, reverse);
		}

		// Copy unaligned part now if going backward
		if (reverse)
		{
			Copy<u8>(dest, src, 0, misalignment, 1, reverse);
		}
	}

	// This won't do anything if the above ran
	Copy<u8>(dest, src, size - remaining, remaining, 1, reverse);

	return dest;
}

template <typename T> FORCEINLINE void Set(void* dest, u8 value, ssize offset, ssize& remaining, ssize alignment)
{
	// This is to get around how Clang deals with SIMD intrinsics (shouldn't cause problems)
	u8 fullValue[32] = {value, value, value, value, value, value, value, value, value, value, value,
						value, value, value, value, value, value, value, value, value, value, value,
						value, value, value, value, value, value, value, value, value, value};
	ssize count = (remaining / alignment) * alignment;
	for (ssize i = offset; i < count; i += alignment)
	{
		static_cast<T*>(dest)[i / alignment] = *reinterpret_cast<T*>(fullValue);
		remaining -= alignment;
	}
}

BASEAPI void* Base_MemSet(void* dest, u32 value, ssize size)
{
	ssize remaining = size;

	if (!dest || size == 0)
	{
		return dest;
	}

	ssize alignment = 1;
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
	ssize misalignment = alignment - ((uptr)dest & alignment);
	if (misalignment > remaining)
	{
		misalignment = remaining;
		remaining = 0;
	}
	else
	{
		remaining -= misalignment;
	}
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

	// This won't do anything if the above ran
	Set<u8>(dest, static_cast<u8>(value), size - remaining, remaining, 1);

	return dest;
}

template <typename T>
static FORCEINLINE s32 Compare(const void* RESTRICT a, const void* RESTRICT b, ssize offset, ssize& remaining, ssize alignment)
{
	ssize count = (remaining / alignment) * alignment;
	ssize i = offset;
	for (; i < count && static_cast<const T * RESTRICT>(a)[i / alignment] == static_cast<const T * RESTRICT>(b)[i / alignment];
		 i += alignment)
	{
		remaining -= alignment;
	}

	if (remaining > 0)
	{
		// Compare the bytes of the larger thing
		for (usize j = 0; j < SIZEOF(T); j++)
		{
			s8 ab = static_cast<const s8 * RESTRICT>(a)[i / alignment + j];
			s8 bb = static_cast<const s8 * RESTRICT>(b)[i / alignment + j];
			if (ab != bb)
			{
				return bb - ab;
			}
		}
	}

	return 0;
}

#ifdef CH_X86
// https://github.com/WojciechMula/simd-string/blob/master/memcmp.cpp
static FORCEINLINE bool V128ByteEqual(v128 a, v128 b, s32& inequalIdx)
{
	static const u8 mode = _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_NEGATIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT;
	inequalIdx = _mm_cmpestri(*reinterpret_cast<const __m128i*>(&a), 16, *reinterpret_cast<const __m128i*>(&b), 16, mode);
	return _mm_cmpestrc(*reinterpret_cast<const __m128i*>(&a), 16, *reinterpret_cast<const __m128i*>(&b), 16, mode);
}
#elif defined CH_XBOX360
static FORCEINLINE bool V128ByteEqual(v128 a, v128 b, s32& inequalIdx)
{
	u32 cr = 0;
	__vcmpequbR(a, b, &cr);
	// If completely equal, do nothing and return true
	if (cr & (1 << 7))
	{
		return true;
	}
	// If completely inequal, do nothing and return false
	else if (cr & (1 << 5))
	{
		return false;
	}
	// If only partially inequal, find the bad index
	else
	{
		// Compare the bytes of the larger thing
		for (ssize i = 0; i < SIZEOF(v128); i++)
		{
			s8 ab = reinterpret_cast<const s8*>(&a)[i];
			s8 bb = reinterpret_cast<const s8*>(&b)[i];
			if (ab != bb)
			{
				inequalIdx = i;
			}
		}

		return false;
	}
}
#endif

#if CH_SIMD128
template <>
FORCEINLINE s32 Compare<v128>(const void* RESTRICT a, const void* RESTRICT b, ssize offset, ssize& remaining, ssize alignment)
{
	ssize count = (remaining / alignment) * alignment;
	ssize i = offset;
	const v128* va = static_cast<const v128*>(a);
	const v128* vb = static_cast<const v128*>(b);
	s32 inequalIdx = 0;
	for (; i < count && V128ByteEqual(va[i / alignment], vb[i / alignment], inequalIdx); i += alignment)
	{
		remaining -= alignment;
	}

	return static_cast<const s8*>(a)[i + inequalIdx] - static_cast<const s8*>(a)[i + inequalIdx];
}
#endif

BASEAPI s32 Base_MemCompare(const void* RESTRICT a, const void* RESTRICT b, ssize size)
{
	ssize remaining = size;

	if (!a || !b || size == 0)
	{
		return -1;
	}

	// No point comparing
	if (a == b)
	{
		return 0;
	}

	ssize alignment = 1;
	if (g_cpuData.haveSimd256 && size >= 32)
	{
		alignment = 32;
	}
	else if (g_cpuData.haveIntSimd128 && size >= 16)
	{
		alignment = 16;
	}
	else if (size >= 8)
	{
		alignment = 8;
	}

	// Can only realign if they're misaligned the same amount (they should be, usually pointers are aligned on a reasonable
	// amount unless it's some arbitrary offset into an array of bytes)
	ssize aAlignment = ((uptr)a & alignment);
	ssize bAlignment = ((uptr)b & alignment);
	s32 comparison;
	if (aAlignment == bAlignment)
	{
		ssize misalignment = alignment - aAlignment;
		if (misalignment > remaining)
		{
			misalignment = remaining;
			remaining = 0;
		}
		else
		{
			remaining -= misalignment;
		}

		comparison = Compare<u8>(a, b, 0, misalignment, 1);
		if (comparison != 0)
		{
			return comparison;
		}

		// _mm_cmpestr* aren't builtins in Clang, and I don't know how to deal with that on Windows
#if defined CH_SIMD128 && !defined __clang__
		if (g_cpuData.haveSimd128Compare && size - remaining >= 16)
		{
			comparison = Compare<v128>(a, b, size - remaining, remaining, alignment);
		}
		else
#endif
			if (size - remaining >= 8)
		{
			comparison = Compare<u64>(a, b, size - remaining, remaining, alignment);
		}
	}

	comparison = Compare<u8>(a, b, size - remaining, remaining, 1);
	return comparison;
}
