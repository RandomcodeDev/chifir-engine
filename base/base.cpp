/// \file Miscellaneous functions (Mem*, Quit) and initialization/shutdown
/// \copyright 2025 Randomcode Developers

#include "base.h"
#include "base/base.h"
#include "base/basicstr.h"
#include "base/compiler.h"
#include "base/log.h"
#include "base/platform.h"

bool g_baseInitialized;
bool g_platInitialized;
bool g_allocUsable;
BaseCpuData g_cpuData;
bool g_quitSignalled;

#if defined CH_ORBIS || CH_SCARLETT
static void InitCpuData()
{
	g_cpuData.haveSimd128 = true;
	g_cpuData.haveIntSimd128 = true;
	g_cpuData.haveSimd128Compare = true;
	g_cpuData.haveSimd256 = true;
}
#elif defined CH_XBOX
static void InitCpuData()
{
	g_cpuData.haveSimd128 = true;
}
#elif defined CH_X86
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

static void InitCpuData()
{
	// Processor info and feature bits
	u32 eax = 0x00000001;
	u32 ebx = 0x00000000;
	u32 ecx = 0x00000000;
	u32 edx = 0x00000000;
	CpuId(eax, ebx, ecx, edx);

	// Because of bit field shenanigans, these have to be made booleans first
#ifdef CH_IA32 // Widespread adoption of SSE2 predates AMD64, and it's part of the base spec
	g_cpuData.haveSimd128 = (bool)(edx & (1 << 25));
	g_cpuData.haveIntSimd128 = (bool)(edx & (1 << 26));
#else
	g_cpuData.haveSimd128 = true;
	g_cpuData.haveIntSimd128 = true;
#endif
	g_cpuData.haveSimd128Compare = (bool)(ecx & (1 << 20));

	// Windows XP doesn't support AVX, even if the CPU does (i.e. it doesn't enable it in CR0/CR4 or include it in contexts)
#ifdef CH_WIN32
	if (AT_LEAST_WINDOWS_VISTA())
	{
#endif
		g_cpuData.haveSimd256 = (bool)(ecx & (1 << 28));
#ifdef CH_WIN32
	}
#endif

	u32 regs[16] = {0}; // 0-3 brand, 4-15 model

	// brand, EAX = 0
	CpuId(regs[0], regs[1], regs[2], regs[3]);
	Base_MemCopy(g_cpuData.brand, regs + 1, 4);
	Base_MemCopy(g_cpuData.brand + 4, regs + 3, 4);
	Base_MemCopy(g_cpuData.brand + 8, regs + 2, 4);
	g_cpuData.brand[12] = '\0';
	Base_StrTrim(g_cpuData.brand);

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
		g_cpuData.name[48] = '\0';
		Base_StrTrim(g_cpuData.name);
	}
}
#elif defined CH_XENON
static void InitCpuData()
{
	g_cpuData.haveSimd128 = true;
	g_cpuData.haveIntSimd128 = true;
	g_cpuData.haveSimd128Compare = true;
}
#else
static void InitCpuData()
{
}
#endif

BASEAPI void Base_Init()
{
	if (!g_baseInitialized)
	{
		InitCpuData();
		Plat_Init();
		Log_Init();
#ifdef CH_WIN32
		Log_AddWriter(new CDbgPrintLogWriter());
#endif
		Log_AddWriter(new CConsoleLogWriter());

		g_baseInitialized = true;
	}
}

BASEAPI void Base_Shutdown()
{
	if (g_baseInitialized)
	{
		Log_Shutdown();
		Plat_Shutdown();
	}
}

BASEAPI NORETURN void Base_Quit(cstr message, ...)
{
	va_list args;

	va_start(args, message);
	cstr formatted = Base_VStrFormat(message, args);
	va_end(args);
	if (!formatted)
	{
		formatted = message;
	}

	if (g_baseInitialized)
	{
		Log_FatalError("%s", formatted);
		// this is a guess, since this function has no knowledge of the engine's paths, just where it probably initialized it to
		Log_FatalError("Check the log in (probably) %s", Plat_GetDataLocation());
	}
	Base_AbortSafe(ABORT_RELEVANT_ERROR, formatted);
}

BASEAPI bool Plat_QuitSignalled()
{
	return g_quitSignalled;
}

template <typename T> static T Fnv1a(const u8* data, ssize size, T offsetBasis, T prime)
{
	T hash = offsetBasis;
	for (ssize i = 0; i < size; i++)
	{
		hash ^= data[i];
		hash *= prime;
	}

	return hash;
}

// yay pointless metaprogramming
#define DEFINE_FNV1A_IMPL(length, offsetBasis, prime)                                                                            \
	BASEAPI u##length Base_Fnv1a##length(const void* data, ssize size)                                                           \
	{                                                                                                                            \
		return Fnv1a<u##length>(reinterpret_cast<const u8*>(data), size, (offsetBasis), (prime));                                \
	}

DEFINE_FNV1A_IMPL(32, 0x811c9dc5, 0x01000193)
DEFINE_FNV1A_IMPL(64, 0xcbf29ce484222325, 0x00000100000001B3)

BASEAPI ssize Base_Search(
	const void* RESTRICT value, const void* RESTRICT data, ssize count, ssize elemSize,
	s32 (*compare)(const void* RESTRICT a, const void* RESTRICT b, void* userData), void* userData)
{
	ssize min = 0;
	ssize max = count - 1;

	while (min <= max)
	{
		ssize mid = min + (max - min) / 2;
		const auto cur = reinterpret_cast<void*>(reinterpret_cast<uptr>(data) + mid * elemSize);
		s32 cmp = compare(cur, value, userData);
		if (cmp < 0) // cur < value
		{
			min = mid + 1;
		}
		else if (cmp > 0) // cur > value
		{
			max = mid - 1;
		}
		else // cur == value
		{
			return mid;
		}
	}

	return -1;
}

OPTIMIZE_OFF

template <typename T>
static void Copy(
	void* RESTRICT dest, const void* RESTRICT src, ssize offset, ssize& remaining, ssize alignment, bool reverse = false)
{
	ssize count = (remaining / alignment) * alignment;
	const void* realSrc = static_cast<const u8 * RESTRICT>(src) + offset;
	void* realDest = static_cast<u8 * RESTRICT>(dest) + offset;
	if (reverse)
	{
		for (ssize i = count - alignment; i >= 0; i -= alignment)
		{
			static_cast<T*>(realDest)[i / alignment] = static_cast<const T*>(realSrc)[i / alignment];
			remaining -= alignment;
		}
	}
	else
	{
		for (ssize i = 0; i < count; i += alignment)
		{
			static_cast<T*>(realDest)[i / alignment] = static_cast<const T*>(realSrc)[i / alignment];
			remaining -= alignment;
		}
	}
}

#ifdef CH_X86
static void X86RepMovsb(void* RESTRICT dest, const void* RESTRICT src, ssize size)
{
#ifdef __clang__
	__asm__("rep movsb" : "+D"(dest), "+S"(src), "+c"(size) : : "memory");
#else
#ifdef CH_XBOX
	__asm {
		push edi
		push esi

		mov edi, dword ptr [ebp + 8]
		mov esi, dword ptr [ebp + 12]
		mov ecx, dword ptr [ebp + 16]
		rep movsb

		pop esi
		pop edi
	}
#else
	__movsb(static_cast<u8*>(dest), static_cast<const u8*>(src), static_cast<usize>(size));
#endif
#endif
}
#endif

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

    PREFETCH(PREFETCH_HIGH, src);
    PREFETCH(PREFETCH_HIGH, dest);

	// Check if data should be copied in reverse in case of overlap
	bool reverse = reinterpret_cast<uptr>(dest) > reinterpret_cast<uptr>(src);

#ifdef CH_X86
	if (!reverse && size >= 1024 && size <= 32 * 1024)
	{
		X86RepMovsb(dest, src, size);
		return dest;
	}
#endif

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
	ssize srcMisalignment = AlignUp(reinterpret_cast<uptr>(src), alignment) - reinterpret_cast<uptr>(src);
	ssize destMisalignment = AlignUp(reinterpret_cast<uptr>(dest), alignment) - reinterpret_cast<uptr>(dest);
	if (srcMisalignment == destMisalignment)
	{
		if (srcMisalignment > remaining)
		{
			srcMisalignment = remaining;
			remaining = 0;
		}
		else
		{
			remaining -= srcMisalignment;
		}

		// Do this afterward
		if (!reverse)
		{
			Copy<u8>(dest, src, 0, srcMisalignment, 1, reverse);
		}

		// In the event of reverse, this copies from the end of the buffers to either the start or the first aligned point
#ifdef CH_SIMD256
		if (alignment == 32 && remaining >= alignment)
		{
			Copy<v256>(dest, src, size - remaining, remaining, alignment, reverse);
		}
		else
#endif
#ifdef CH_SIMD128
			if (alignment == 16 && remaining >= alignment)
		{
			Copy<v128>(dest, src, size - remaining, remaining, alignment, reverse);
		}
		else
#endif
			if (alignment == 8 && remaining >= alignment)
		{
			Copy<u64>(dest, src, size - remaining, remaining, alignment, reverse);
		}

		// Copy unaligned part now if going backward
		if (reverse)
		{
			Copy<u8>(dest, src, 0, srcMisalignment, 1, reverse);
		}
	}

	// This won't do anything if the above ran
	Copy<u8>(dest, src, size - remaining, remaining, 1, reverse);

	return dest;
}

template <typename T> void Set(void* dest, u8 value, ssize offset, ssize& remaining, ssize alignment)
{
	// This is to get around how Clang deals with SIMD intrinsics (shouldn't cause problems)
	u64 partValue = 0x0101010101010101 * value; // fast way to fill a u64 with one byte
	u64 fullValue[4] = {partValue, partValue, partValue, partValue}; // 32 bytes for max SIMD register size

	ssize count = (remaining / alignment) * alignment;
	void* realDest = static_cast<u8*>(dest) + offset;
	for (ssize i = 0; i < count; i += alignment)
	{
		static_cast<T*>(realDest)[i / alignment] = *reinterpret_cast<T*>(fullValue);
		remaining -= alignment;
	}
}

#ifdef CH_X86
// rep stosb is faster for big blocks
static void X86RepStosb(void* dest, u8 value, ssize size)
{
#ifdef __clang__
	__asm__("rep stosb" : "+D"(dest), "+c"(size) : "a"(value) : "memory");
#else
#ifdef CH_XBOX
	__asm {
		push edi

		mov edi, dword ptr [ebp + 8]
		mov ecx, dword ptr [ebp + 16]
		mov al, byte ptr [ebp + 12]
		rep stosb

		pop edi
	}
#else
	__stosb(static_cast<u8*>(dest), value, static_cast<usize>(size));
#endif
#endif
}
#endif

CLANG_ONLY(__attribute__((target("mmx"))))
BASEAPI void* Base_MemSet(void* dest, u32 value, ssize size)
{
	ssize remaining = size;

	if (!dest || size == 0)
	{
		return dest;
	}

    PREFETCH(PREFETCH_HIGH, dest);

#ifdef CH_X86
	// https://msrc.microsoft.com/blog/2021/01/building-faster-amd64-memset-routines/
	if (size >= 1024 && size <= 32 * 1024)
	{
		X86RepStosb(dest, static_cast<u8>(value), size);
		return dest;
	}
#endif

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
	ssize misalignment = AlignUp(reinterpret_cast<uptr>(dest), alignment) - reinterpret_cast<uptr>(dest);
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
	if (alignment == 32 && remaining >= alignment)
	{
		Set<v256>(dest, static_cast<u8>(value), size - remaining, remaining, alignment);
	}
	else
#endif
#ifdef CH_SIMD128
		if (alignment == 16 && remaining >= alignment)
	{
		Set<v128>(dest, static_cast<u8>(value), size - remaining, remaining, alignment);
	}
	else
#endif
		if (alignment == 8 && remaining >= alignment)
	{
		Set<u64>(dest, static_cast<u8>(value), size - remaining, remaining, alignment);
	}

	// This won't do anything if the above ran
	Set<u8>(dest, static_cast<u8>(value), size - remaining, remaining, 1);

	return dest;
}

template <typename T>
static s32 Compare(const void* RESTRICT a, const void* RESTRICT b, ssize offset, ssize& remaining, ssize alignment)
{
	ssize count = (remaining / alignment) * alignment;
	ssize i = 0;
	const void* RESTRICT realA = static_cast<const u8 * RESTRICT>(a) + offset;
	const void* RESTRICT realB = static_cast<const u8 * RESTRICT>(b) + offset;
	for (; i < count &&
		   static_cast<const T * RESTRICT>(realA)[i / alignment] == static_cast<const T * RESTRICT>(realB)[i / alignment];
		 i += alignment)
	{
		remaining -= alignment;
	}

	if (remaining > 0)
	{
		// Compare the bytes of the larger thing
		for (usize j = 0; j < SIZEOF(T); j++)
		{
			s8 ab = static_cast<const s8 * RESTRICT>(realA)[i / alignment + j];
			s8 bb = static_cast<const s8 * RESTRICT>(realB)[i / alignment + j];
			if (ab != bb)
			{
				return bb - ab;
			}
		}
	}

	return 0;
}

#if defined CH_X86 && defined CH_SIMD128_COMPARE
// https://github.com/WojciechMula/simd-string/blob/master/memcmp.cpp
CLANG_ONLY(__attribute__((target("sse4"))))
static bool
V128ByteEqual(v128 a, v128 b, s32& inequalIdx)
{
	static const u8 mode = _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_NEGATIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT;
	inequalIdx = _mm_cmpestri(*reinterpret_cast<const __m128i*>(&a), 16, *reinterpret_cast<const __m128i*>(&b), 16, mode);
	return !_mm_cmpestrc(*reinterpret_cast<const __m128i*>(&a), 16, *reinterpret_cast<const __m128i*>(&b), 16, mode);
}
#elif defined CH_XENON
static bool V128ByteEqual(v128 a, v128 b, s32& inequalIdx)
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
#elif defined CH_ARM64
static FORCEINLINE bool V128ByteEqual(v128 a, v128 b, s32& inequalIdx)
{
	uint8x16_t cmp = vceqq_u8(*reinterpret_cast<uint8x16_t*>(&a), *reinterpret_cast<uint8x16_t*>(&b));
	uint64x2_t cmp64 = vreinterpretq_u64_u8(cmp);
	uint32x2_t cmp32 = vreinterpret_u32_u64(vadd_u64(vget_low_u64(cmp64), vget_high_u64(cmp64)));
	u64 cmp64_res = vget_lane_u32(cmp32, 0);

	if (cmp64_res == 0xFFFFFFFFFFFFFFFF)
	{
		return true;
	}

	u8* cmpBytes = reinterpret_cast<u8*>(&cmp);
	for (int i = 0; i < SIZEOF(v128); ++i)
	{
		if (cmpBytes[i] == 0)
		{
			inequalIdx = i;
			return false;
		}
	}

	return true;
}
#endif

#if CH_SIMD128_COMPARE
template <> s32 Compare<v128>(const void* RESTRICT a, const void* RESTRICT b, ssize offset, ssize& remaining, ssize alignment)
{
	ssize count = (remaining / alignment) * alignment;
	ssize i = 0;
	const v128* va = reinterpret_cast<const v128*>(reinterpret_cast<uptr>(a) + offset);
	const v128* vb = reinterpret_cast<const v128*>(reinterpret_cast<uptr>(b) + offset);
	s32 inequalIdx = 0;
	for (; i < count && V128ByteEqual(va[i / alignment], vb[i / alignment], inequalIdx); i += alignment)
	{
		remaining -= alignment;
	}

	return static_cast<const s8*>(a)[offset + i + inequalIdx] - static_cast<const s8*>(a)[offset + i + inequalIdx];
}
#endif

BASEAPI s32 Base_MemCompare(const void* RESTRICT a, const void* RESTRICT b, ssize size){
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

    PREFETCH(PREFETCH_HIGH, a);
    PREFETCH(PREFETCH_HIGH, b);

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
	ssize aMisalignment = AlignUp(reinterpret_cast<uptr>(a), alignment) - reinterpret_cast<uptr>(a);
	ssize bMisalignment = AlignUp(reinterpret_cast<uptr>(b), alignment) - reinterpret_cast<uptr>(b);
	s32 comparison;
	if (aMisalignment == bMisalignment)
	{
		if (aMisalignment > remaining)
		{
			aMisalignment = remaining;
			remaining = 0;
		}
		else
		{
			remaining -= aMisalignment;
		}

		comparison = Compare<u8>(a, b, 0, aMisalignment, 1);
		if (comparison != 0)
		{
			return comparison;
		}

#ifdef CH_SIMD128_COMPARE
		if (alignment == 16 && remaining >= alignment)
		{
			comparison = Compare<v128>(a, b, size - remaining, remaining, alignment);
		}
		else
#endif
			if (alignment == 8 && remaining >= alignment)
		{
			comparison = Compare<u64>(a, b, size - remaining, remaining, alignment);
		}
	}

	comparison = Compare<u8>(a, b, size - remaining, remaining, 1);
	return comparison;
}

// TODO: https://stackoverflow.com/questions/14523621/how-to-break-from-a-loop-when-using-sse-intrinsics/14524319#14524319
BASEAPI ssize Base_MemFind(const void* data, ssize size, u8 value, bool reverse)
{
	return Base_MemFind(data, size, &value, 1, reverse);
}

static bool IsSequence(ssize i, const u8* RESTRICT bytes, ssize size, const u8* RESTRICT sequence, ssize sequenceSize)
{
	// if the first byte is equal, and there's enough room for the sequence, is it here?
	return bytes[i] == sequence[0] && size - i > sequenceSize && Base_MemCompare(bytes + i, sequence, sequenceSize) == 0;
}

BASEAPI ssize Base_MemFind(const void* RESTRICT data, ssize size, const void* RESTRICT sequence, ssize sequenceSize, bool reverse)
{
	const u8* RESTRICT bytes = static_cast<const u8 * RESTRICT>(data);
	const u8* RESTRICT sequenceBytes = static_cast<const u8 * RESTRICT>(sequence);
	if (reverse)
	{
		for (ssize i = size - 1; i >= 0; i--)
		{
			if (IsSequence(i, bytes, size, sequenceBytes, sequenceSize))
			{
				return i;
			}
		}
	}
	else
	{
		for (ssize i = 0; i < size; i++)
		{
			if (IsSequence(i, bytes, size, sequenceBytes, sequenceSize))
			{
				return i;
			}
		}
	}

	return -1;
}

OPTIMIZE_ON
