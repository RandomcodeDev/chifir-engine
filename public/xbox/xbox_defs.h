#pragma once

#include "xboxkrnl/xboxkrnl.h"

typedef struct _CURDIR {
    UNICODE_STRING DosPath;
    HANDLE Handle;
} CURDIR, *PCURDIR;

#define RTL_USER_PROC_CURDIR_CLOSE      0x00000002
#define RTL_USER_PROC_CURDIR_INHERIT    0x00000003

typedef struct _RTL_DRIVE_LETTER_CURDIR {
    USHORT Flags;
    USHORT Length;
    ULONG TimeStamp;
    STRING DosPath;
} RTL_DRIVE_LETTER_CURDIR, *PRTL_DRIVE_LETTER_CURDIR;

#define RTL_MAX_DRIVE_LETTERS 32
#define RTL_DRIVE_LETTER_VALID (USHORT)0x0001

typedef struct _RTL_USER_PROCESS_PARAMETERS {
    ULONG MaximumLength;
    ULONG Length;

    ULONG Flags;
    ULONG DebugFlags;

    HANDLE ConsoleHandle;
    ULONG  ConsoleFlags;
    HANDLE StandardInput;
    HANDLE StandardOutput;
    HANDLE StandardError;

    CURDIR CurrentDirectory;        // ProcessParameters
    UNICODE_STRING DllPath;         // ProcessParameters
    UNICODE_STRING ImagePathName;   // ProcessParameters
    UNICODE_STRING CommandLine;     // ProcessParameters
    PVOID Environment;              // NtAllocateVirtualMemory

    ULONG StartingX;
    ULONG StartingY;
    ULONG CountX;
    ULONG CountY;
    ULONG CountCharsX;
    ULONG CountCharsY;
    ULONG FillAttribute;

    ULONG WindowFlags;
    ULONG ShowWindowFlags;
    UNICODE_STRING WindowTitle;     // ProcessParameters
    UNICODE_STRING DesktopInfo;     // ProcessParameters
    UNICODE_STRING ShellInfo;       // ProcessParameters
    UNICODE_STRING RuntimeData;     // ProcessParameters
    RTL_DRIVE_LETTER_CURDIR CurrentDirectores[ RTL_MAX_DRIVE_LETTERS ];
} RTL_USER_PROCESS_PARAMETERS, *PRTL_USER_PROCESS_PARAMETERS;

//
// Possible bit values for Flags field.
//

#define RTL_USER_PROC_PARAMS_NORMALIZED     0x00000001
#define RTL_USER_PROC_PROFILE_USER          0x00000002
#define RTL_USER_PROC_PROFILE_KERNEL        0x00000004
#define RTL_USER_PROC_PROFILE_SERVER        0x00000008
#define RTL_USER_PROC_RESERVE_1MB           0x00000020
#define RTL_USER_PROC_RESERVE_16MB          0x00000040
#define RTL_USER_PROC_CASE_SENSITIVE        0x00000080
#define RTL_USER_PROC_DISABLE_HEAP_DECOMMIT 0x00000100

typedef struct _PEB_LDR_DATA {
    ULONG Length;
    BOOLEAN Initialized;
    HANDLE SsHandle;
    LIST_ENTRY InLoadOrderModuleList;
    LIST_ENTRY InMemoryOrderModuleList;
    LIST_ENTRY InInitializationOrderModuleList;
} PEB_LDR_DATA, *PPEB_LDR_DATA;

#ifdef _MAC
#pragma warning( default : 4121 )
#endif
//
// Handle tag bits for Peb Stdio File Handles
//

#define PEB_STDIO_HANDLE_NATIVE     0
#define PEB_STDIO_HANDLE_SUBSYS     1
#define PEB_STDIO_HANDLE_PM         2
#define PEB_STDIO_HANDLE_RESERVED   3

#define GDI_HANDLE_BUFFER_SIZE32  34
#define GDI_HANDLE_BUFFER_SIZE64  60

#if !defined(_IA64_) && !defined(_AXP64_)
#define GDI_HANDLE_BUFFER_SIZE      GDI_HANDLE_BUFFER_SIZE32
#else
#define GDI_HANDLE_BUFFER_SIZE      GDI_HANDLE_BUFFER_SIZE64
#endif

#define FOREGROUND_BASE_PRIORITY  9
#define NORMAL_BASE_PRIORITY      8

typedef struct _PEB_FREE_BLOCK {
    struct _PEB_FREE_BLOCK *Next;
    ULONG Size;
} PEB_FREE_BLOCK, *PPEB_FREE_BLOCK;

// begin_ntddk begin_wdm begin_nthal begin_ntifs
//
// ClientId
//

typedef struct _CLIENT_ID {
    HANDLE UniqueProcess;
    HANDLE UniqueThread;
} CLIENT_ID;
typedef CLIENT_ID *PCLIENT_ID;

// end_ntddk end_wdm end_nthal end_ntifs

#if !defined(CLIENT_ID64_DEFINED)

typedef struct _CLIENT_ID64 {
    ULONGLONG  UniqueProcess;
    ULONGLONG  UniqueThread;
} CLIENT_ID64;

typedef CLIENT_ID64 *PCLIENT_ID64;

#define CLIENT_ID64_DEFINED

#endif

#define TLS_MINIMUM_AVAILABLE 64    // winnt
#define TLS_EXPANSION_SLOTS   1024

typedef
VOID
(*PPS_POST_PROCESS_INIT_ROUTINE) (
    VOID
    );

typedef struct _PEB {
    BOOLEAN InheritedAddressSpace;      // These four fields cannot change unless the
    BOOLEAN ReadImageFileExecOptions;   //
    BOOLEAN BeingDebugged;              //
    BOOLEAN SpareBool;                  //
    HANDLE Mutant;                      // INITIAL_PEB structure is also updated.

    PVOID ImageBaseAddress;
    PPEB_LDR_DATA Ldr;
    struct _RTL_USER_PROCESS_PARAMETERS *ProcessParameters;
    PVOID SubSystemData;
    PVOID ProcessHeap;
    PVOID FastPebLock;
    PVOID FastPebLockRoutine;
    PVOID FastPebUnlockRoutine;
    ULONG EnvironmentUpdateCount;
    PVOID KernelCallbackTable;
    ULONG SystemReserved[2];
    PPEB_FREE_BLOCK FreeList;
    ULONG TlsExpansionCounter;
    PVOID TlsBitmap;
    ULONG TlsBitmapBits[2];         // relates to TLS_MINIMUM_AVAILABLE
    PVOID ReadOnlySharedMemoryBase;
    PVOID ReadOnlySharedMemoryHeap;
    PVOID *ReadOnlyStaticServerData;
    PVOID AnsiCodePageData;
    PVOID OemCodePageData;
    PVOID UnicodeCaseTableData;

    //
    // Useful information for LdrpInitialize
    ULONG NumberOfProcessors;
    ULONG NtGlobalFlag;

    //
    // Passed up from MmCreatePeb from Session Manager registry key
    //

    LARGE_INTEGER CriticalSectionTimeout;
    SIZE_T HeapSegmentReserve;
    SIZE_T HeapSegmentCommit;
    SIZE_T HeapDeCommitTotalFreeThreshold;
    SIZE_T HeapDeCommitFreeBlockThreshold;

    //
    // Where heap manager keeps track of all heaps created for a process
    // Fields initialized by MmCreatePeb.  ProcessHeaps is initialized
    // to point to the first free byte after the PEB and MaximumNumberOfHeaps
    // is computed from the page size used to hold the PEB, less the fixed
    // size of this data structure.
    //

    ULONG NumberOfHeaps;
    ULONG MaximumNumberOfHeaps;
    PVOID *ProcessHeaps;

    //
    //
    PVOID GdiSharedHandleTable;
    PVOID ProcessStarterHelper;
    ULONG GdiDCAttributeList;
    PVOID LoaderLock;

    //
    // Following fields filled in by MmCreatePeb from system values and/or
    // image header.
    //

    ULONG OSMajorVersion;
    ULONG OSMinorVersion;
    USHORT OSBuildNumber;
    USHORT OSCSDVersion;
    ULONG OSPlatformId;
    ULONG ImageSubsystem;
    ULONG ImageSubsystemMajorVersion;
    ULONG ImageSubsystemMinorVersion;
    ULONG_PTR ImageProcessAffinityMask;
    ULONG GdiHandleBuffer[GDI_HANDLE_BUFFER_SIZE];
    PPS_POST_PROCESS_INIT_ROUTINE PostProcessInitRoutine;

    PVOID TlsExpansionBitmap;
    ULONG TlsExpansionBitmapBits[32];   // relates to TLS_EXPANSION_SLOTS

    //
    // Id of the Hydra session in which this process is running
    //
    ULONG SessionId;

    //
    // Filled in by LdrQueryImageFileExecutionOptions
    //
    PVOID AppCompatInfo;

    //
    // Used by GetVersionExW as the szCSDVersion string
    //
    UNICODE_STRING CSDVersion;


} PEB, *PPEB;

typedef struct _NT_TIB {
    struct _EXCEPTION_REGISTRATION_RECORD *ExceptionList;
    PVOID StackBase;
    PVOID StackLimit;
    PVOID SubSystemTib;
    union {
        PVOID FiberData;
        ULONG Version;
    };
    PVOID ArbitraryUserPointer;
    struct _NT_TIB *Self;
} NT_TIB;
typedef NT_TIB *PNT_TIB;
// end_nthal end_ntddk end_ntifs end_winnt

//
// Gdi command batching
//

#define GDI_BATCH_BUFFER_SIZE 310

typedef struct _GDI_TEB_BATCH {
    ULONG    Offset;
    ULONG_PTR HDC;
    ULONG    Buffer[GDI_BATCH_BUFFER_SIZE];
} GDI_TEB_BATCH,*PGDI_TEB_BATCH;


//
// Wx86 thread state information
//

typedef struct _Wx86ThreadState {
    PULONG  CallBx86Eip;
    PVOID   DeallocationCpu;
    BOOLEAN UseKnownWx86Dll;
    char    OleStubInvoked;
} WX86THREAD, *PWX86THREAD;


//
//  TEB - The thread environment block
//

#define STATIC_UNICODE_BUFFER_LENGTH 261
#define WIN32_CLIENT_INFO_LENGTH 62

#define WIN32_CLIENT_INFO_SPIN_COUNT 1

typedef struct _TEB {
    NT_TIB NtTib;
    PVOID  EnvironmentPointer;
    CLIENT_ID ClientId;
    PVOID ActiveRpcHandle;
    PVOID ThreadLocalStoragePointer;
    PPEB ProcessEnvironmentBlock;
    ULONG LastErrorValue;
    ULONG CountOfOwnedCriticalSections;
    PVOID CsrClientThread;
    PVOID Win32ThreadInfo;          // PtiCurrent
    ULONG User32Reserved[26];       // user32.dll items
    ULONG UserReserved[5];          // Winsrv SwitchStack
    PVOID WOW32Reserved;            // used by WOW
    ULONG CurrentLocale;
    ULONG FpSoftwareStatusRegister; // offset known by outsiders!
    PVOID SystemReserved1[54];      // Used by FP emulator
    NTSTATUS ExceptionCode;         // for RaiseUserException
    UCHAR SpareBytes1[44];
    GDI_TEB_BATCH GdiTebBatch;      // Gdi batching
    CLIENT_ID RealClientId;
    HANDLE GdiCachedProcessHandle;
    ULONG GdiClientPID;
    ULONG GdiClientTID;
    PVOID GdiThreadLocalInfo;
    ULONG_PTR Win32ClientInfo[WIN32_CLIENT_INFO_LENGTH]; // User32 Client Info
    PVOID glDispatchTable[233];     // OpenGL
    ULONG_PTR glReserved1[29];      // OpenGL
    PVOID glReserved2;              // OpenGL
    PVOID glSectionInfo;            // OpenGL
    PVOID glSection;                // OpenGL
    PVOID glTable;                  // OpenGL
    PVOID glCurrentRC;              // OpenGL
    PVOID glContext;                // OpenGL
    ULONG LastStatusValue;
    UNICODE_STRING StaticUnicodeString;
    WCHAR StaticUnicodeBuffer[STATIC_UNICODE_BUFFER_LENGTH];
    PVOID DeallocationStack;
    PVOID TlsSlots[TLS_MINIMUM_AVAILABLE];
    LIST_ENTRY TlsLinks;
    PVOID Vdm;
    PVOID ReservedForNtRpc;
    PVOID DbgSsReserved[2];
    ULONG HardErrorsAreDisabled;
    PVOID Instrumentation[16];
    PVOID WinSockData;              // WinSock
    ULONG GdiBatchCount;
    BOOLEAN InDbgPrint;
    BOOLEAN SpareB1;
    BOOLEAN SpareB2;
    BOOLEAN SpareB3;
    ULONG Spare3;
    PVOID ReservedForPerf;
    PVOID ReservedForOle;
    ULONG WaitingOnLoaderLock;
    WX86THREAD Wx86Thread;
    PVOID *TlsExpansionSlots;
#ifdef  _IA64_
    PVOID DeallocationBStore;
    PVOID BStoreLimit;
#endif
    ULONG ImpersonationLocale;      // Current locale of impersonated user
    ULONG IsImpersonating;         // Thread impersonation status
    PVOID NlsCache;                // NLS thread cache
} TEB;
typedef TEB *PTEB;
