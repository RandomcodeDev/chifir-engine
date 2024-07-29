#include "base/loader.h"
#include "base/platform.h"
#include "base/types.h"
#include "base/vector.h"
#include "isystem.h"
#include "launcher.h"
#include "utility/log.h"
#include "utility/utility.h"
#include "videosystem/ivideosystem.h"

// ChatGPT generated allocator test, so far has shown that it works but the size of the coalesced free list doesn't quite add up
void TestAlloc()
{
	const int NumIterations = 500000; // Number of allocation/deallocation cycles
	const int AllocationSize = 64;    // Size of each allocation in bytes

	void** allocations = new void*[NumIterations];

	// Allocate memory in a loop
	for (int i = 0; i < NumIterations; ++i)
	{
		void* ptr = Base_Alloc(AllocationSize);
		if (!ptr)
		{
			Base_Quit(1, "Allocation %d failed", i);
		}

		allocations[i] = ptr;
	}

	// Free all allocated memory
	for (int i = 0; i < NumIterations; ++i)
	{
		Base_Free(allocations[i]);
	}

	delete[] allocations; // Free the array of pointers
}

struct TestStruct
{
	int value;
	TestStruct() : value(5)
	{
	}
	TestStruct(int v) : value(v)
	{
	}
};

// ChatGPT CVector test
void TestVector()
{
	CVector<TestStruct> vec;
	ASSERT(vec.Size() == 0);

	// Test adding elements
	vec.Add(TestStruct(1));
	vec.Add(TestStruct(2));
	vec.Add(TestStruct(3));
	ASSERT(vec.Size() == 3);

	// Test accessing elements
	ASSERT(vec[0].value == 1);
	ASSERT(vec[1].value == 2);
	ASSERT(vec[2].value == 3);

	// Test resizing
	vec.Resize(5);
	ASSERT(vec.Size() == 5);

	// Test reserve
	vec.Reserve(10);
	ASSERT(vec.Size() == 5); // Size should remain the same

	// Test deleting an element
	vec.Delete(1);
	ASSERT(vec.Size() == 4);
	ASSERT(vec[1].value == 3);
}

class CDbgPrintLogWriter : public ILogWriter
{
  public:
	void Write(const LogMessage_t& message)
	{
		static const cstr LEVEL_NAMES[] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

		if (message.isAddress)
		{
			DbgPrint(
				"[%s] [0x%p@%s %s] %s\n", LEVEL_NAMES[message.level], message.location, message.file, message.function,
				message.message);
		}
		else
		{
			DbgPrint(
				"[%s] [%s:%d %s] %s\n", LEVEL_NAMES[message.level], message.file, message.location, message.function,
				message.message);
		}
	}
};

extern "C" LAUNCHERAPI s32 LauncherMain()
{
#ifdef CH_XBOX360
	__security_init_cookie();
#endif

	Base_Init();
	Plat_Init();

	Log_AddWriter(new CDbgPrintLogWriter());

#ifdef CH_STATIC

#else
#endif

	Plat_Shutdown();
	Base_Shutdown();

	return 0;
}
