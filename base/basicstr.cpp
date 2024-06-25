#include "base/base.h"
#include "base/basicstr.h"
#include "base/compiler.h"
#include "base/dll.h"
#include "base/types.h"

BASEAPI usize Base_StrLen(cstr str, usize maxSize)
{
	usize size;
	for (size = 0; size < maxSize && str[size] != '\0'; size++)
		;

	return size;
}
