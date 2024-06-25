#include "base/basicwstr.h"
#include "base/base.h"
#include "base/compiler.h"
#include "base/dll.h"
#include "base/types.h"

BASEAPI wchar_t Base_WToUpper(wchar_t c)
{
	return 0;
}

BASEAPI s32 Base_WStrCmp(cwstr RESTRICT a, cwstr RESTRICT b, bool caseSensitive, usize maxCount)
{
	for (usize i = 0; i < maxCount && a[i] != 0 && b[i] != 0; i++)
	{
		wchar_t ac = caseSensitive ? a[i] : Base_WToUpper(a[i]);
		wchar_t bc = caseSensitive ? b[i] : Base_WToUpper(b[i]);

		if (ac != bc)
		{
			return (s32)ac - (s32)bc;
		}
	}

	return 0;
}
