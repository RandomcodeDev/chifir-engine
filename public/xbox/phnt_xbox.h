/*
 * Xbox 360 Win32 definition support
 */

#ifndef _PHNT_WINDOWS_H
#define _PHNT_WINDOWS_H

// This header file provides access to Win32, plus NTSTATUS values and some access mask values.

#ifndef __cplusplus
#ifndef CINTERFACE
#define CINTERFACE
#endif

#ifndef COBJMACROS
#define COBJMACROS
#endif
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef INT_ERROR
#define INT_ERROR (-1)
#endif

#ifndef ULONG64_MAX
#define ULONG64_MAX 0xffffffffffffffffui64
#endif

#ifndef SIZE_T_MAX
#ifdef _WIN64
#define SIZE_T_MAX 0xffffffffffffffffui64
#else
#define SIZE_T_MAX 0xffffffffUL
#endif
#endif

#ifndef ENABLE_RTL_NUMBER_OF_V2
#define ENABLE_RTL_NUMBER_OF_V2
#endif

#ifndef INITGUID
#define INITGUID
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef WIN32_NO_STATUS
#define WIN32_NO_STATUS
#endif

#ifndef COM_NO_WINDOWS_H
#define COM_NO_WINDOWS_H
#endif

#ifndef __cplusplus
// This is needed to workaround C17 preprocessor errors when using legacy versions of the Windows SDK. (dmex)
#ifndef MICROSOFT_WINDOWS_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS
#define MICROSOFT_WINDOWS_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS 0
#endif
#endif

#include "salieri.h"

#include <xtl.h>

#include <xbox.h>

#undef WIN32_NO_STATUS
#include "xntstatus.h"
// #include <winioctl.h>
// #include <evntrace.h>

// #ifdef COM_NO_WINDOWS_H
// #include <ole2.h>
// #endif

#ifdef __cplusplus
#define EXTERN_C_START                                                                                                           \
	extern "C"                                                                                                                   \
	{
#define EXTERN_C_END }
#else
#define EXTERN_C_START
#define EXTERN_C_END
#endif

#undef C_ASSERT
#define C_ASSERT(x)

// Definitions from modern winnt.h. Many of these most likely have no use whatsoever on Xbox, like much of phnt, but this is the
// easiest way.

#define DUMMYUNIONNAME
#define DUMMYUNIONNAME1
#define DUMMYUNIONNAME2
#define DUMMYUNIONNAME3
#define DUMMYUNIONNAME4
#define DUMMYSTRUCTNAME
#define DUMMYSTRUCTNAME1
#define DUMMYSTRUCTNAME2
#define DUMMYSTRUCTNAME3
#define DUMMYSTRUCTNAME4

#define DECLSPEC_ALIGN(x) __declspec(align(x))
#define DECLSPEC_ALLOCATOR
#define DECLSPEC_IMPORT             __declspec(dllimport)
#define MEMORY_ALLOCATION_ALIGNMENT 16

typedef struct _GROUP_AFFINITY
{
	KAFFINITY Mask;
	WORD Group;
	WORD Reserved[3];
} GROUP_AFFINITY, *PGROUP_AFFINITY;

typedef enum _FIRMWARE_TYPE
{
	FirmwareTypeUnknown,
	FirmwareTypeBios,
	FirmwareTypeUefi,
	FirmwareTypeMax
} FIRMWARE_TYPE, *PFIRMWARE_TYPE;

#define XSTATE_LEGACY_FLOATING_POINT (0)
#define XSTATE_LEGACY_SSE            (1)
#define XSTATE_GSSE                  (2)
#define XSTATE_AVX                   (XSTATE_GSSE)
#define XSTATE_MPX_BNDREGS           (3)
#define XSTATE_MPX_BNDCSR            (4)
#define XSTATE_AVX512_KMASK          (5)
#define XSTATE_AVX512_ZMM_H          (6)
#define XSTATE_AVX512_ZMM            (7)
#define XSTATE_IPT                   (8)
#define XSTATE_PASID                 (10)
#define XSTATE_CET_U                 (11)
#define XSTATE_CET_S                 (12)
#define XSTATE_AMX_TILE_CONFIG       (17)
#define XSTATE_AMX_TILE_DATA         (18)
#define XSTATE_LWP                   (62)
#define MAXIMUM_XSTATE_FEATURES      (64)

typedef struct _XSTATE_FEATURE
{
	DWORD Offset;
	DWORD Size;
} XSTATE_FEATURE, *PXSTATE_FEATURE;

typedef struct _XSTATE_CONFIGURATION
{
	// Mask of all enabled features
	DWORD64 EnabledFeatures;

	// Mask of volatile enabled features
	DWORD64 EnabledVolatileFeatures;

	// Total size of the save area for user states
	DWORD Size;

	// Control Flags
	union {
		DWORD ControlFlags;
		struct
		{
			DWORD OptimizedSave : 1;
			DWORD CompactionEnabled : 1;
			DWORD ExtendedFeatureDisable : 1;
		} DUMMYSTRUCTNAME;
	} DUMMYUNIONNAME;

	// List of features
	XSTATE_FEATURE Features[MAXIMUM_XSTATE_FEATURES];

	// Mask of all supervisor features
	DWORD64 EnabledSupervisorFeatures;

	// Mask of features that require start address to be 64 byte aligned
	DWORD64 AlignedFeatures;

	// Total size of the save area for user and supervisor states
	DWORD AllFeatureSize;

	// List which holds size of each user and supervisor state supported by CPU
	DWORD AllFeatures[MAXIMUM_XSTATE_FEATURES];

	// Mask of all supervisor features that are exposed to user-mode
	DWORD64 EnabledUserVisibleSupervisorFeatures;

	// Mask of features that can be disabled via XFD
	DWORD64 ExtendedFeatureDisableFeatures;

	// Total size of the save area for non-large user and supervisor states
	DWORD AllNonLargeFeatureSize;

	DWORD Spare;

} XSTATE_CONFIGURATION, *PXSTATE_CONFIGURATION;

typedef struct _IMAGE_DELAYLOAD_DESCRIPTOR
{
	union {
		DWORD AllAttributes;
		struct
		{
			DWORD RvaBased : 1; // Delay load version 2
			DWORD ReservedAttributes : 31;
		} DUMMYSTRUCTNAME;
	} Attributes;

	DWORD DllNameRVA;                 // RVA to the name of the target library (NULL-terminate ASCII string)
	DWORD ModuleHandleRVA;            // RVA to the HMODULE caching location (PHMODULE)
	DWORD ImportAddressTableRVA;      // RVA to the start of the IAT (PIMAGE_THUNK_DATA)
	DWORD ImportNameTableRVA;         // RVA to the start of the name table (PIMAGE_THUNK_DATA::AddressOfData)
	DWORD BoundImportAddressTableRVA; // RVA to an optional bound IAT
	DWORD UnloadInformationTableRVA;  // RVA to an optional unload info table
	DWORD TimeDateStamp;              // 0 if not bound,
									  // Otherwise, date/time of the target DLL

} IMAGE_DELAYLOAD_DESCRIPTOR, *PIMAGE_DELAYLOAD_DESCRIPTOR;
typedef const IMAGE_DELAYLOAD_DESCRIPTOR* PCIMAGE_DELAYLOAD_DESCRIPTOR;

//
// Define flags for setting process CFG valid call target entries.
//

//
// Call target should be made valid.  If not set, the call target is made
// invalid.  Input flag.
//

#define CFG_CALL_TARGET_VALID (0x00000001)

//
// Call target has been successfully processed.  Used to report to the caller
// how much progress has been made.  Output flag.
//

#define CFG_CALL_TARGET_PROCESSED (0x00000002)

//
// Call target should be made valid only if it is suppressed export.
// What this flag means is that it can *only* be used on a cell which is
// currently in the CFG export suppressed state (only considered for export
// suppressed processes and not legacy CFG processes!), and it is also
// allowed to be used even if the process is a restricted (i.e. no ACG) process.
//

#define CFG_CALL_TARGET_CONVERT_EXPORT_SUPPRESSED_TO_VALID (0x00000004)

//
// Call target should be made into an XFG call target.
//

#define CFG_CALL_TARGET_VALID_XFG (0x00000008)

//
// Call target should be made valid only if it is already an XFG target
// in a process which has XFG audit mode enabled.
//

#define CFG_CALL_TARGET_CONVERT_XFG_TO_CFG (0x00000010)

typedef struct _CFG_CALL_TARGET_INFO
{
	ULONG_PTR Offset;
	ULONG_PTR Flags;
} CFG_CALL_TARGET_INFO, *PCFG_CALL_TARGET_INFO;

typedef enum
{
	ACTCTX_RUN_LEVEL_UNSPECIFIED = 0,
	ACTCTX_RUN_LEVEL_AS_INVOKER,
	ACTCTX_RUN_LEVEL_HIGHEST_AVAILABLE,
	ACTCTX_RUN_LEVEL_REQUIRE_ADMIN,
	ACTCTX_RUN_LEVEL_NUMBERS
} ACTCTX_REQUESTED_RUN_LEVEL;

typedef enum
{
	ACTCTX_COMPATIBILITY_ELEMENT_TYPE_UNKNOWN = 0,
	ACTCTX_COMPATIBILITY_ELEMENT_TYPE_OS,
	ACTCTX_COMPATIBILITY_ELEMENT_TYPE_MITIGATION,
	ACTCTX_COMPATIBILITY_ELEMENT_TYPE_MAXVERSIONTESTED
} ACTCTX_COMPATIBILITY_ELEMENT_TYPE;

typedef struct _PROCESSOR_NUMBER
{
	WORD Group;
	BYTE Number;
	BYTE Reserved;
} PROCESSOR_NUMBER, *PPROCESSOR_NUMBER;

typedef struct _LDT_ENTRY
{
	WORD LimitLow;
	WORD BaseLow;
	union {
		struct
		{
			BYTE BaseMid;
			BYTE Flags1; // Declare as bytes to avoid alignment
			BYTE Flags2; // Problems.
			BYTE BaseHi;
		} Bytes;
		struct
		{
			DWORD BaseMid : 8;
			DWORD Type : 5;
			DWORD Dpl : 2;
			DWORD Pres : 1;
			DWORD LimitHi : 4;
			DWORD Sys : 1;
			DWORD Reserved_0 : 1;
			DWORD Default_Big : 1;
			DWORD Granularity : 1;
			DWORD BaseHi : 8;
		} Bits;
	} HighWord;
} LDT_ENTRY, *PLDT_ENTRY;

typedef enum _PROCESS_MITIGATION_POLICY
{
	ProcessDEPPolicy,
	ProcessASLRPolicy,
	ProcessDynamicCodePolicy,
	ProcessStrictHandleCheckPolicy,
	ProcessSystemCallDisablePolicy,
	ProcessMitigationOptionsMask,
	ProcessExtensionPointDisablePolicy,
	ProcessControlFlowGuardPolicy,
	ProcessSignaturePolicy,
	ProcessFontDisablePolicy,
	ProcessImageLoadPolicy,
	ProcessSystemCallFilterPolicy,
	ProcessPayloadRestrictionPolicy,
	ProcessChildProcessPolicy,
	ProcessSideChannelIsolationPolicy,
	ProcessUserShadowStackPolicy,
	ProcessRedirectionTrustPolicy,
	ProcessUserPointerAuthPolicy,
	ProcessSEHOPPolicy,
	ProcessActivationContextTrustPolicy,
	MaxProcessMitigationPolicy
} PROCESS_MITIGATION_POLICY, *PPROCESS_MITIGATION_POLICY;

typedef struct _PROCESS_MITIGATION_ASLR_POLICY
{
	union {
		DWORD Flags;
		struct
		{
			DWORD EnableBottomUpRandomization : 1;
			DWORD EnableForceRelocateImages : 1;
			DWORD EnableHighEntropy : 1;
			DWORD DisallowStrippedImages : 1;
			DWORD ReservedFlags : 28;
		} DUMMYSTRUCTNAME;
	} DUMMYUNIONNAME;
} PROCESS_MITIGATION_ASLR_POLICY, *PPROCESS_MITIGATION_ASLR_POLICY;

typedef struct _PROCESS_MITIGATION_DEP_POLICY
{
	union {
		DWORD Flags;
		struct
		{
			DWORD Enable : 1;
			DWORD DisableAtlThunkEmulation : 1;
			DWORD ReservedFlags : 30;
		} DUMMYSTRUCTNAME;
	} DUMMYUNIONNAME;
	BOOLEAN Permanent;
} PROCESS_MITIGATION_DEP_POLICY, *PPROCESS_MITIGATION_DEP_POLICY;

typedef struct _PROCESS_MITIGATION_STRICT_HANDLE_CHECK_POLICY
{
	union {
		DWORD Flags;
		struct
		{
			DWORD RaiseExceptionOnInvalidHandleReference : 1;
			DWORD HandleExceptionsPermanentlyEnabled : 1;
			DWORD ReservedFlags : 30;
		} DUMMYSTRUCTNAME;
	} DUMMYUNIONNAME;
} PROCESS_MITIGATION_STRICT_HANDLE_CHECK_POLICY, *PPROCESS_MITIGATION_STRICT_HANDLE_CHECK_POLICY;

typedef struct _PROCESS_MITIGATION_SYSTEM_CALL_DISABLE_POLICY
{
	union {
		DWORD Flags;
		struct
		{
			DWORD DisallowWin32kSystemCalls : 1;
			DWORD AuditDisallowWin32kSystemCalls : 1;
			DWORD DisallowFsctlSystemCalls : 1;
			DWORD AuditDisallowFsctlSystemCalls : 1;
			DWORD ReservedFlags : 28;
		} DUMMYSTRUCTNAME;
	} DUMMYUNIONNAME;
} PROCESS_MITIGATION_SYSTEM_CALL_DISABLE_POLICY, *PPROCESS_MITIGATION_SYSTEM_CALL_DISABLE_POLICY;

typedef struct _PROCESS_MITIGATION_EXTENSION_POINT_DISABLE_POLICY
{
	union {
		DWORD Flags;
		struct
		{
			DWORD DisableExtensionPoints : 1;
			DWORD ReservedFlags : 31;
		} DUMMYSTRUCTNAME;
	} DUMMYUNIONNAME;
} PROCESS_MITIGATION_EXTENSION_POINT_DISABLE_POLICY, *PPROCESS_MITIGATION_EXTENSION_POINT_DISABLE_POLICY;

typedef struct _PROCESS_MITIGATION_DYNAMIC_CODE_POLICY
{
	union {
		DWORD Flags;
		struct
		{
			DWORD ProhibitDynamicCode : 1;
			DWORD AllowThreadOptOut : 1;
			DWORD AllowRemoteDowngrade : 1;
			DWORD AuditProhibitDynamicCode : 1;
			DWORD ReservedFlags : 28;
		} DUMMYSTRUCTNAME;
	} DUMMYUNIONNAME;
} PROCESS_MITIGATION_DYNAMIC_CODE_POLICY, *PPROCESS_MITIGATION_DYNAMIC_CODE_POLICY;

typedef struct _PROCESS_MITIGATION_CONTROL_FLOW_GUARD_POLICY
{
	union {
		DWORD Flags;
		struct
		{
			DWORD EnableControlFlowGuard : 1;
			DWORD EnableExportSuppression : 1;
			DWORD StrictMode : 1;
			DWORD EnableXfg : 1;
			DWORD EnableXfgAuditMode : 1;
			DWORD ReservedFlags : 27;
		} DUMMYSTRUCTNAME;
	} DUMMYUNIONNAME;
} PROCESS_MITIGATION_CONTROL_FLOW_GUARD_POLICY, *PPROCESS_MITIGATION_CONTROL_FLOW_GUARD_POLICY;

typedef struct _PROCESS_MITIGATION_BINARY_SIGNATURE_POLICY
{
	union {
		DWORD Flags;
		struct
		{
			DWORD MicrosoftSignedOnly : 1;
			DWORD StoreSignedOnly : 1;
			DWORD MitigationOptIn : 1;
			DWORD AuditMicrosoftSignedOnly : 1;
			DWORD AuditStoreSignedOnly : 1;
			DWORD ReservedFlags : 27;
		} DUMMYSTRUCTNAME;
	} DUMMYUNIONNAME;
} PROCESS_MITIGATION_BINARY_SIGNATURE_POLICY, *PPROCESS_MITIGATION_BINARY_SIGNATURE_POLICY;

typedef struct _PROCESS_MITIGATION_FONT_DISABLE_POLICY
{
	union {
		DWORD Flags;
		struct
		{
			DWORD DisableNonSystemFonts : 1;
			DWORD AuditNonSystemFontLoading : 1;
			DWORD ReservedFlags : 30;
		} DUMMYSTRUCTNAME;
	} DUMMYUNIONNAME;
} PROCESS_MITIGATION_FONT_DISABLE_POLICY, *PPROCESS_MITIGATION_FONT_DISABLE_POLICY;

typedef struct _PROCESS_MITIGATION_IMAGE_LOAD_POLICY
{
	union {
		DWORD Flags;
		struct
		{
			DWORD NoRemoteImages : 1;
			DWORD NoLowMandatoryLabelImages : 1;
			DWORD PreferSystem32Images : 1;
			DWORD AuditNoRemoteImages : 1;
			DWORD AuditNoLowMandatoryLabelImages : 1;
			DWORD ReservedFlags : 27;
		} DUMMYSTRUCTNAME;
	} DUMMYUNIONNAME;
} PROCESS_MITIGATION_IMAGE_LOAD_POLICY, *PPROCESS_MITIGATION_IMAGE_LOAD_POLICY;

typedef struct _PROCESS_MITIGATION_SYSTEM_CALL_FILTER_POLICY
{
	union {
		DWORD Flags;
		struct
		{
			DWORD FilterId : 4;
			DWORD ReservedFlags : 28;
		} DUMMYSTRUCTNAME;
	} DUMMYUNIONNAME;
} PROCESS_MITIGATION_SYSTEM_CALL_FILTER_POLICY, *PPROCESS_MITIGATION_SYSTEM_CALL_FILTER_POLICY;

typedef struct _PROCESS_MITIGATION_PAYLOAD_RESTRICTION_POLICY
{
	union {
		DWORD Flags;
		struct
		{
			DWORD EnableExportAddressFilter : 1;
			DWORD AuditExportAddressFilter : 1;

			DWORD EnableExportAddressFilterPlus : 1;
			DWORD AuditExportAddressFilterPlus : 1;

			DWORD EnableImportAddressFilter : 1;
			DWORD AuditImportAddressFilter : 1;

			DWORD EnableRopStackPivot : 1;
			DWORD AuditRopStackPivot : 1;

			DWORD EnableRopCallerCheck : 1;
			DWORD AuditRopCallerCheck : 1;

			DWORD EnableRopSimExec : 1;
			DWORD AuditRopSimExec : 1;

			DWORD ReservedFlags : 20;
		} DUMMYSTRUCTNAME;
	} DUMMYUNIONNAME;
} PROCESS_MITIGATION_PAYLOAD_RESTRICTION_POLICY, *PPROCESS_MITIGATION_PAYLOAD_RESTRICTION_POLICY;

typedef struct _PROCESS_MITIGATION_CHILD_PROCESS_POLICY
{
	union {
		DWORD Flags;
		struct
		{
			DWORD NoChildProcessCreation : 1;
			DWORD AuditNoChildProcessCreation : 1;
			DWORD AllowSecureProcessCreation : 1;
			DWORD ReservedFlags : 29;
		} DUMMYSTRUCTNAME;
	} DUMMYUNIONNAME;
} PROCESS_MITIGATION_CHILD_PROCESS_POLICY, *PPROCESS_MITIGATION_CHILD_PROCESS_POLICY;

typedef struct _PROCESS_MITIGATION_SIDE_CHANNEL_ISOLATION_POLICY
{
	union {
		DWORD Flags;
		struct
		{

			//
			// Prevent branch target pollution cross-SMT-thread in user mode.
			//

			DWORD SmtBranchTargetIsolation : 1;

			//
			// Isolate this process into a distinct security domain, even from
			// other processes running as the same security context.  This
			// prevents branch target injection cross-process (normally such
			// branch target injection is only inhibited across different
			// security contexts).
			//
			// Page combining is limited to processes within the same security
			// domain.  This flag thus also effectively limits the process to
			// only being able to combine internally to the process itself,
			// except for common pages (unless further restricted by the
			// DisablePageCombine policy).
			//

			DWORD IsolateSecurityDomain : 1;

			//
			// Disable all page combining for this process, even internally to
			// the process itself, except for common pages (zeroes or ones).
			//

			DWORD DisablePageCombine : 1;

			//
			// Memory Disambiguation Disable.
			//

			DWORD SpeculativeStoreBypassDisable : 1;

			//
			// Prevent this process' threads from being scheduled on the same
			// core as threads outside its security domain.
			//

			DWORD RestrictCoreSharing : 1;

			DWORD ReservedFlags : 27;

		} DUMMYSTRUCTNAME;
	} DUMMYUNIONNAME;
} PROCESS_MITIGATION_SIDE_CHANNEL_ISOLATION_POLICY, *PPROCESS_MITIGATION_SIDE_CHANNEL_ISOLATION_POLICY;

typedef struct _PROCESS_MITIGATION_USER_SHADOW_STACK_POLICY
{
	union {
		DWORD Flags;
		struct
		{
			DWORD EnableUserShadowStack : 1;
			DWORD AuditUserShadowStack : 1;
			DWORD SetContextIpValidation : 1;
			DWORD AuditSetContextIpValidation : 1;
			DWORD EnableUserShadowStackStrictMode : 1;
			DWORD BlockNonCetBinaries : 1;
			DWORD BlockNonCetBinariesNonEhcont : 1;
			DWORD AuditBlockNonCetBinaries : 1;
			DWORD CetDynamicApisOutOfProcOnly : 1;
			DWORD SetContextIpValidationRelaxedMode : 1;
			DWORD ReservedFlags : 22;

		} DUMMYSTRUCTNAME;
	} DUMMYUNIONNAME;
} PROCESS_MITIGATION_USER_SHADOW_STACK_POLICY, *PPROCESS_MITIGATION_USER_SHADOW_STACK_POLICY;

typedef struct _PROCESS_MITIGATION_ACTIVATION_CONTEXT_TRUST_POLICY
{
	union {
		DWORD Flags;
		struct
		{
			DWORD AssemblyManifestRedirectionTrust : 1;
			DWORD ReservedFlags : 31;
		} DUMMYSTRUCTNAME;
	} DUMMYUNIONNAME;
} PROCESS_MITIGATION_ACTIVATION_CONTEXT_TRUST_POLICY, *PPROCESS_MITIGATION_ACTIVATION_CONTEXT_TRUST_POLICY;

typedef enum _HARDWARE_COUNTER_TYPE
{
	PMCCounter,
	MaxHardwareCounterType
} HARDWARE_COUNTER_TYPE, *PHARDWARE_COUNTER_TYPE;

#define MAX_HW_COUNTERS 16

typedef struct _JOBOBJECT_BASIC_ACCOUNTING_INFORMATION
{
	LARGE_INTEGER TotalUserTime;
	LARGE_INTEGER TotalKernelTime;
	LARGE_INTEGER ThisPeriodTotalUserTime;
	LARGE_INTEGER ThisPeriodTotalKernelTime;
	DWORD TotalPageFaultCount;
	DWORD TotalProcesses;
	DWORD ActiveProcesses;
	DWORD TotalTerminatedProcesses;
} JOBOBJECT_BASIC_ACCOUNTING_INFORMATION, *PJOBOBJECT_BASIC_ACCOUNTING_INFORMATION;

//@[comment("MVI_tracked")]
typedef struct _JOBOBJECT_BASIC_LIMIT_INFORMATION
{
	LARGE_INTEGER PerProcessUserTimeLimit;
	LARGE_INTEGER PerJobUserTimeLimit;
	DWORD LimitFlags;
	SIZE_T MinimumWorkingSetSize;
	SIZE_T MaximumWorkingSetSize;
	DWORD ActiveProcessLimit;
	ULONG_PTR Affinity;
	DWORD PriorityClass;
	DWORD SchedulingClass;
} JOBOBJECT_BASIC_LIMIT_INFORMATION, *PJOBOBJECT_BASIC_LIMIT_INFORMATION;

typedef enum _JOBOBJECTINFOCLASS
{
	JobObjectBasicAccountingInformation = 1,
	JobObjectBasicLimitInformation,
	JobObjectBasicProcessIdList,
	JobObjectBasicUIRestrictions,
	JobObjectSecurityLimitInformation, // deprecated
	JobObjectEndOfJobTimeInformation,
	JobObjectAssociateCompletionPortInformation,
	JobObjectBasicAndIoAccountingInformation,
	JobObjectExtendedLimitInformation,
	JobObjectJobSetInformation,
	JobObjectGroupInformation,
	JobObjectNotificationLimitInformation,
	JobObjectLimitViolationInformation,
	JobObjectGroupInformationEx,
	JobObjectCpuRateControlInformation,
	JobObjectCompletionFilter,
	JobObjectCompletionCounter,

	//
	//

	JobObjectReserved1Information = 18,
	JobObjectReserved2Information,
	JobObjectReserved3Information,
	JobObjectReserved4Information,
	JobObjectReserved5Information,
	JobObjectReserved6Information,
	JobObjectReserved7Information,
	JobObjectReserved8Information,
	JobObjectReserved9Information,
	JobObjectReserved10Information,
	JobObjectReserved11Information,
	JobObjectReserved12Information,
	JobObjectReserved13Information,
	JobObjectReserved14Information = 31,
	JobObjectNetRateControlInformation,
	JobObjectNotificationLimitInformation2,
	JobObjectLimitViolationInformation2,
	JobObjectCreateSilo,
	JobObjectSiloBasicInformation,
	JobObjectReserved15Information = 37,
	JobObjectReserved16Information = 38,
	JobObjectReserved17Information = 39,
	JobObjectReserved18Information = 40,
	JobObjectReserved19Information = 41,
	JobObjectReserved20Information = 42,
	JobObjectReserved21Information = 43,
	JobObjectReserved22Information = 44,
	JobObjectReserved23Information = 45,
	JobObjectReserved24Information = 46,
	JobObjectReserved25Information = 47,
	JobObjectReserved26Information = 48,
	JobObjectReserved27Information = 49,
	MaxJobObjectInfoClass
} JOBOBJECTINFOCLASS;

typedef struct _JOB_SET_ARRAY
{
	HANDLE JobHandle;  // Handle to job object to insert
	DWORD MemberLevel; // Level of this job in the set. Must be > 0. Can be sparse.
	DWORD Flags;       // Unused. Must be zero
} JOB_SET_ARRAY, *PJOB_SET_ARRAY;

typedef struct _DEBUG_EVENT
{
	DWORD dwDebugEventCode;
	DWORD dwProcessId;
	DWORD dwThreadId;
	DWORD Padding;
} DEBUG_EVENT, *LPDEBUG_EVENT;

typedef struct _FILE_ID_128
{
	BYTE Identifier[16];
} FILE_ID_128, *PFILE_ID_128;

typedef enum _STORAGE_RESERVE_ID
{

	StorageReserveIdNone = 0,
	StorageReserveIdHard,
	StorageReserveIdSoft,
	StorageReserveIdUpdateScratch,

	StorageReserveIdMax

} STORAGE_RESERVE_ID, *PSTORAGE_RESERVE_ID;

#define DEVICE_TYPE DWORD

#ifndef FORCEINLINE
#define FORCEINLINE __forceinline
#endif
#define CFORCEINLINE FORCEINLINE
#define NTAPI_INLINE __stdcall
typedef LUID* PLUID;

static inline struct _TEB* NtCurrentTeb(void)
{
	__asm
	{
		// This is based on PowerPC code from OpenNT (and PowerPC documentation says r.13 is used for this general purpose)
		// However, I don't think the TEB actually exists on Xbox
        mr r3, r13
	}
}

typedef VOID(NTAPI* WAITORTIMERCALLBACKFUNC)(PVOID, BOOLEAN);
typedef VOID(NTAPI* WORKERCALLBACKFUNC)(PVOID);
typedef VOID(NTAPI* APC_CALLBACK_FUNCTION)(DWORD, PVOID, PVOID);
typedef WAITORTIMERCALLBACKFUNC WAITORTIMERCALLBACK;
typedef VOID(NTAPI* PFLS_CALLBACK_FUNCTION)(_In_ PVOID lpFlsData);

typedef struct _TP_CALLBACK_INSTANCE TP_CALLBACK_INSTANCE, *PTP_CALLBACK_INSTANCE;

typedef struct _OSVERSIONINFOEXW
{
	DWORD dwOSVersionInfoSize;
	DWORD dwMajorVersion;
	DWORD dwMinorVersion;
	DWORD dwBuildNumber;
	DWORD dwPlatformId;
	WCHAR szCSDVersion[128]; // Maintenance string for PSS usage
	WORD wServicePackMajor;
	WORD wServicePackMinor;
	WORD wSuiteMask;
	BYTE wProductType;
	BYTE wReserved;
} OSVERSIONINFOEXW, *POSVERSIONINFOEXW, *LPOSVERSIONINFOEXW, RTL_OSVERSIONINFOEXW, *PRTL_OSVERSIONINFOEXW;

//
// Trace header for all legacy events.
//

typedef struct _EVENT_TRACE_HEADER
{                // overlays WNODE_HEADER
	USHORT Size; // Size of entire record
	union {
		USHORT FieldTypeFlags; // Indicates valid fields
		struct
		{
			UCHAR HeaderType;  // Header type - internal use only
			UCHAR MarkerFlags; // Marker - internal use only
		} DUMMYSTRUCTNAME;
	} DUMMYUNIONNAME;
	union {
		ULONG Version;
		struct
		{
			UCHAR Type;     // event type
			UCHAR Level;    // trace instrumentation level
			USHORT Version; // version of trace record
		} Class;
	} DUMMYUNIONNAME2;
	ULONG ThreadId;          // Thread Id
	ULONG ProcessId;         // Process Id
	LARGE_INTEGER TimeStamp; // time when event happens
	union {
		GUID Guid;         // Guid that identifies event
		ULONGLONG GuidPtr; // use with WNODE_FLAG_USE_GUID_PTR
	} DUMMYUNIONNAME3;
	union {
		struct
		{
			ULONG KernelTime; // Kernel Mode CPU ticks
			ULONG UserTime;   // User mode CPU ticks
		} DUMMYSTRUCTNAME;
		ULONG64 ProcessorTime; // Processor Clock
		struct
		{
			ULONG ClientContext; // Reserved
			ULONG Flags;         // Event Flags
		} DUMMYSTRUCTNAME2;
	} DUMMYUNIONNAME4;
} EVENT_TRACE_HEADER, *PEVENT_TRACE_HEADER;

typedef struct _NT_TIB32
{
	DWORD ExceptionList;
	DWORD StackBase;
	DWORD StackLimit;
	DWORD SubSystemTib;

#if defined(_MSC_EXTENSIONS)
	union {
		DWORD FiberData;
		DWORD Version;
	};
#else
	DWORD FiberData;
#endif

	DWORD ArbitraryUserPointer;
	DWORD Self;
} NT_TIB32, *PNT_TIB32;

typedef struct _RTL_SRWLOCK
{
	PVOID Ptr;
} RTL_SRWLOCK, *PRTL_SRWLOCK;

typedef enum _SYSTEM_POWER_STATE
{
	PowerSystemUnspecified = 0,
	PowerSystemWorking = 1,
	PowerSystemSleeping1 = 2,
	PowerSystemSleeping2 = 3,
	PowerSystemSleeping3 = 4,
	PowerSystemHibernate = 5,
	PowerSystemShutdown = 6,
	PowerSystemMaximum = 7
} SYSTEM_POWER_STATE, *PSYSTEM_POWER_STATE;

typedef enum _DEVICE_POWER_STATE
{
	PowerDeviceUnspecified,
	PowerDeviceD0,
	PowerDeviceD1,
	PowerDeviceD2,
	PowerDeviceD3,
	PowerDeviceMaximum
} DEVICE_POWER_STATE, *PDEVICE_POWER_STATE;

//
// Monitor on/off reasons
//
// N.B. Update power-event mapping when adding new events.
//
typedef enum
{
	MonitorRequestReasonUnknown,
	MonitorRequestReasonPowerButton,
	MonitorRequestReasonRemoteConnection,
	MonitorRequestReasonScMonitorpower,
	MonitorRequestReasonUserInput,
	MonitorRequestReasonAcDcDisplayBurst,
	MonitorRequestReasonUserDisplayBurst,
	MonitorRequestReasonPoSetSystemState,
	MonitorRequestReasonSetThreadExecutionState,
	MonitorRequestReasonFullWake,
	MonitorRequestReasonSessionUnlock,
	MonitorRequestReasonScreenOffRequest,
	MonitorRequestReasonIdleTimeout,
	MonitorRequestReasonPolicyChange,
	MonitorRequestReasonSleepButton,
	MonitorRequestReasonLid,
	MonitorRequestReasonBatteryCountChange,
	MonitorRequestReasonGracePeriod,
	MonitorRequestReasonPnP,
	MonitorRequestReasonDP,
	MonitorRequestReasonSxTransition,
	MonitorRequestReasonSystemIdle,
	MonitorRequestReasonNearProximity,
	MonitorRequestReasonThermalStandby,
	MonitorRequestReasonResumePdc,
	MonitorRequestReasonResumeS4,
	MonitorRequestReasonTerminal,
	MonitorRequestReasonPdcSignal,
	MonitorRequestReasonAcDcDisplayBurstSuppressed,
	MonitorRequestReasonSystemStateEntered, // When CS exit happens because system
											// transition to S4/S5, please note this
											// reason is different than ReasonSxTransition.
	MonitorRequestReasonWinrt,
	MonitorRequestReasonUserInputKeyboard,
	MonitorRequestReasonUserInputMouse,
	MonitorRequestReasonUserInputTouchpad,
	MonitorRequestReasonUserInputPen,
	MonitorRequestReasonUserInputAccelerometer,
	MonitorRequestReasonUserInputHid,
	MonitorRequestReasonUserInputPoUserPresent,
	MonitorRequestReasonUserInputSessionSwitch,
	MonitorRequestReasonUserInputInitialization,
	MonitorRequestReasonPdcSignalWindowsMobilePwrNotif, // PDC_SIGNAL_PROVIDER_PWRNOTIF_SVC
	MonitorRequestReasonPdcSignalWindowsMobileShell,    // PDC_SIGNAL_PROVIDER_UM_CS_CONTROL
	MonitorRequestReasonPdcSignalHeyCortana,            // PDC_SIGNAL_PROVIDER_HEY_CORTANA
	MonitorRequestReasonPdcSignalHolographicShell,      // PDC_SIGNAL_PROVIDER_HOLOSI_CRITICAL_BATTERY_WAKE
	MonitorRequestReasonPdcSignalFingerprint,           // PDC_SIGNAL_PROVIDER_WINBIO
	MonitorRequestReasonDirectedDrips,
	MonitorRequestReasonDim,
	MonitorRequestReasonBuiltinPanel,
	MonitorRequestReasonDisplayRequiredUnDim,
	MonitorRequestReasonBatteryCountChangeSuppressed,
	MonitorRequestReasonResumeModernStandby,
	MonitorRequestReasonTerminalInit,
	MonitorRequestReasonPdcSignalSensorsHumanPresence, // PDC_SIGNAL_PROVIDER_SENSORS_HUMAN_PRESENCE_MONITOR
	MonitorRequestReasonBatteryPreCritical,
	MonitorRequestReasonUserInputTouch,
	MonitorRequestReasonMax
} POWER_MONITOR_REQUEST_REASON;

typedef enum
{
	SystemPowerPolicyAc,
	SystemPowerPolicyDc,
	VerifySystemPolicyAc,
	VerifySystemPolicyDc,
	SystemPowerCapabilities,
	SystemBatteryState,
	SystemPowerStateHandler,
	ProcessorStateHandler,
	SystemPowerPolicyCurrent,
	AdministratorPowerPolicy,
	SystemReserveHiberFile,
	ProcessorInformation,
	SystemPowerInformation,
	ProcessorStateHandler2,
	LastWakeTime,  // Compare with KeQueryInterruptTime()
	LastSleepTime, // Compare with KeQueryInterruptTime()
	SystemExecutionState,
	SystemPowerStateNotifyHandler,
	ProcessorPowerPolicyAc,
	ProcessorPowerPolicyDc,
	VerifyProcessorPowerPolicyAc,
	VerifyProcessorPowerPolicyDc,
	ProcessorPowerPolicyCurrent,
	SystemPowerStateLogging,
	SystemPowerLoggingEntry,
	SetPowerSettingValue,
	NotifyUserPowerSetting,
	PowerInformationLevelUnused0,
	SystemMonitorHiberBootPowerOff,
	SystemVideoState,
	TraceApplicationPowerMessage,
	TraceApplicationPowerMessageEnd,
	ProcessorPerfStates,
	ProcessorIdleStates,
	ProcessorCap,
	SystemWakeSource,
	SystemHiberFileInformation,
	TraceServicePowerMessage,
	ProcessorLoad,
	PowerShutdownNotification,
	MonitorCapabilities,
	SessionPowerInit,
	SessionDisplayState,
	PowerRequestCreate,
	PowerRequestAction,
	GetPowerRequestList,
	ProcessorInformationEx,
	NotifyUserModeLegacyPowerEvent,
	GroupPark,
	ProcessorIdleDomains,
	WakeTimerList,
	SystemHiberFileSize,
	ProcessorIdleStatesHv,
	ProcessorPerfStatesHv,
	ProcessorPerfCapHv,
	ProcessorSetIdle,
	LogicalProcessorIdling,
	UserPresence,
	PowerSettingNotificationName,
	GetPowerSettingValue,
	IdleResiliency,
	SessionRITState,
	SessionConnectNotification,
	SessionPowerCleanup,
	SessionLockState,
	SystemHiberbootState,
	PlatformInformation,
	PdcInvocation,
	MonitorInvocation,
	FirmwareTableInformationRegistered,
	SetShutdownSelectedTime,
	SuspendResumeInvocation, // Deprecated
	PlmPowerRequestCreate,
	ScreenOff,
	CsDeviceNotification,
	PlatformRole,
	LastResumePerformance,
	DisplayBurst,
	ExitLatencySamplingPercentage,
	RegisterSpmPowerSettings,
	PlatformIdleStates,
	ProcessorIdleVeto, // Deprecated.
	PlatformIdleVeto,  // Deprecated.
	SystemBatteryStatePrecise,
	ThermalEvent,
	PowerRequestActionInternal,
	BatteryDeviceState,
	PowerInformationInternal,
	ThermalStandby,
	SystemHiberFileType,
	PhysicalPowerButtonPress,
	QueryPotentialDripsConstraint,
	EnergyTrackerCreate,
	EnergyTrackerQuery,
	UpdateBlackBoxRecorder,
	SessionAllowExternalDmaDevices,
	SendSuspendResumeNotification,
	BlackBoxRecorderDirectAccessBuffer,
	PowerInformationLevelMaximum
} POWER_INFORMATION_LEVEL;

#define ES_SYSTEM_REQUIRED   ((DWORD)0x00000001)
#define ES_DISPLAY_REQUIRED  ((DWORD)0x00000002)
#define ES_USER_PRESENT      ((DWORD)0x00000004)
#define ES_AWAYMODE_REQUIRED ((DWORD)0x00000040)
#define ES_CONTINUOUS        ((DWORD)0x80000000)

typedef DWORD EXECUTION_STATE, *PEXECUTION_STATE;

typedef enum
{
	LT_DONT_CARE,
	LT_LOWEST_LATENCY
} LATENCY_TIME;

typedef enum
{
	PowerActionNone = 0,
	PowerActionReserved,
	PowerActionSleep,
	PowerActionHibernate,
	PowerActionShutdown,
	PowerActionShutdownReset,
	PowerActionShutdownOff,
	PowerActionWarmEject,
	PowerActionDisplayOff
} POWER_ACTION, *PPOWER_ACTION;

typedef struct _RTL_CRITICAL_SECTION_DEBUG
{
	WORD Type;
	WORD CreatorBackTraceIndex;
	struct _RTL_CRITICAL_SECTION* CriticalSection;
	LIST_ENTRY ProcessLocksList;
	DWORD EntryCount;
	DWORD ContentionCount;
	DWORD Flags;
	WORD CreatorBackTraceIndexHigh;
	WORD Identifier;
} RTL_CRITICAL_SECTION_DEBUG, *PRTL_CRITICAL_SECTION_DEBUG, RTL_RESOURCE_DEBUG, *PRTL_RESOURCE_DEBUG;

#define MEM_EXTENDED_PARAMETER_TYPE_BITS 8

typedef struct DECLSPEC_ALIGN(8) MEM_EXTENDED_PARAMETER
{

	struct
	{
		DWORD64 Type : MEM_EXTENDED_PARAMETER_TYPE_BITS;
		DWORD64 Reserved : 64 - MEM_EXTENDED_PARAMETER_TYPE_BITS;
	} DUMMYSTRUCTNAME;

	union {
		DWORD64 ULong64;
		PVOID Pointer;
		SIZE_T Size;
		HANDLE Handle;
		DWORD ULong;
	} DUMMYUNIONNAME;

} MEM_EXTENDED_PARAMETER, *PMEM_EXTENDED_PARAMETER;

typedef LONG(NTAPI* PVECTORED_EXCEPTION_HANDLER)(struct _EXCEPTION_POINTERS* ExceptionInfo);

#define UNICODE_STRING_MAX_BYTES ((WORD)65534)
#define UNICODE_STRING_MAX_CHARS (32767)

typedef struct tagACTCTX_SECTION_KEYED_DATA_ASSEMBLY_METADATA
{
	PVOID lpInformation;
	PVOID lpSectionBase;
	ULONG ulSectionLength;
	PVOID lpSectionGlobalDataBase;
	ULONG ulSectionGlobalDataLength;
} ACTCTX_SECTION_KEYED_DATA_ASSEMBLY_METADATA, *PACTCTX_SECTION_KEYED_DATA_ASSEMBLY_METADATA;
typedef const ACTCTX_SECTION_KEYED_DATA_ASSEMBLY_METADATA* PCACTCTX_SECTION_KEYED_DATA_ASSEMBLY_METADATA;
typedef enum _ACTIVATION_CONTEXT_INFO_CLASS
{
	ActivationContextBasicInformation = 1,
	ActivationContextDetailedInformation = 2,
	AssemblyDetailedInformationInActivationContext = 3,
	FileInformationInAssemblyOfAssemblyInActivationContext = 4,
	RunlevelInformationInActivationContext = 5,
	CompatibilityInformationInActivationContext = 6,
	ActivationContextManifestResourceName = 7,
	MaxActivationContextInfoClass,

	//
	// compatibility with old names
	//
	AssemblyDetailedInformationInActivationContxt = 3,
	FileInformationInAssemblyOfAssemblyInActivationContxt = 4
} ACTIVATION_CONTEXT_INFO_CLASS;

#define ACTIVATIONCONTEXTINFOCLASS ACTIVATION_CONTEXT_INFO_CLASS

typedef struct _ACTIVATION_CONTEXT_QUERY_INDEX
{
	DWORD ulAssemblyIndex;
	DWORD ulFileIndexInAssembly;
} ACTIVATION_CONTEXT_QUERY_INDEX, *PACTIVATION_CONTEXT_QUERY_INDEX;

typedef struct tagACTCTX_SECTION_KEYED_DATA
{
	ULONG cbSize;
	ULONG ulDataFormatVersion;
	PVOID lpData;
	ULONG ulLength;
	PVOID lpSectionGlobalData;
	ULONG ulSectionGlobalDataLength;
	PVOID lpSectionBase;
	ULONG ulSectionTotalLength;
	HANDLE hActCtx;
	ULONG ulAssemblyRosterIndex;
	// 2600 stops here
	ULONG ulFlags;
	ACTCTX_SECTION_KEYED_DATA_ASSEMBLY_METADATA AssemblyMetadata;
} ACTCTX_SECTION_KEYED_DATA, *PACTCTX_SECTION_KEYED_DATA;
typedef const ACTCTX_SECTION_KEYED_DATA* PCACTCTX_SECTION_KEYED_DATA;

typedef CONST WCHAR *LPCWCHAR, *PCWCHAR;
typedef CONST WCHAR UNALIGNED *LPCUWCHAR, *PCUWCHAR;

#ifdef __cplusplus
extern "C"
{
#endif

	FORCEINLINE
	CHAR ReadAcquire8(_In_ _Interlocked_operand_ CHAR const volatile* Source)

	{

		CHAR Value;

		Value = *Source;
		return Value;
	}

	FORCEINLINE
	CHAR ReadNoFence8(_In_ _Interlocked_operand_ CHAR const volatile* Source)

	{

		CHAR Value;

		Value = *Source;
		return Value;
	}

	FORCEINLINE
	VOID WriteRelease8(_Out_ _Interlocked_operand_ CHAR volatile* Destination, _In_ CHAR Value)

	{

		*Destination = Value;
		return;
	}

	FORCEINLINE
	VOID WriteNoFence8(_Out_ _Interlocked_operand_ CHAR volatile* Destination, _In_ CHAR Value)

	{

		*Destination = Value;
		return;
	}

	FORCEINLINE
	SHORT
	ReadAcquire16(_In_ _Interlocked_operand_ SHORT const volatile* Source)

	{

		SHORT Value;

		Value = *Source;
		return Value;
	}

	FORCEINLINE
	SHORT
	ReadNoFence16(_In_ _Interlocked_operand_ SHORT const volatile* Source)

	{

		SHORT Value;

		Value = *Source;
		return Value;
	}

	FORCEINLINE
	VOID WriteRelease16(_Out_ _Interlocked_operand_ SHORT volatile* Destination, _In_ SHORT Value)

	{

		*Destination = Value;
		return;
	}

	FORCEINLINE
	VOID WriteNoFence16(_Out_ _Interlocked_operand_ SHORT volatile* Destination, _In_ SHORT Value)

	{

		*Destination = Value;
		return;
	}

	FORCEINLINE
	LONG ReadAcquire(_In_ _Interlocked_operand_ LONG const volatile* Source)

	{

		LONG Value;

		Value = *Source;
		return Value;
	}

	CFORCEINLINE
	LONG ReadNoFence(_In_ _Interlocked_operand_ LONG const volatile* Source)

	{

		LONG Value;

		Value = *Source;
		return Value;
	}

	CFORCEINLINE
	VOID WriteRelease(_Out_ _Interlocked_operand_ LONG volatile* Destination, _In_ LONG Value)

	{

		*Destination = Value;
		return;
	}

	FORCEINLINE
	VOID WriteNoFence(_Out_ _Interlocked_operand_ LONG volatile* Destination, _In_ LONG Value)

	{

		*Destination = Value;
		return;
	}

	FORCEINLINE
	LONG64
	ReadAcquire64(_In_ _Interlocked_operand_ LONG64 const volatile* Source)

	{

		LONG64 Value;

		Value = *Source;
		return Value;
	}

	CFORCEINLINE
	LONG64
	ReadNoFence64(_In_ _Interlocked_operand_ LONG64 const volatile* Source)

	{

		LONG64 Value;

		Value = *Source;
		return Value;
	}

	CFORCEINLINE
	VOID WriteRelease64(_Out_ _Interlocked_operand_ LONG64 volatile* Destination, _In_ LONG64 Value)

	{

		*Destination = Value;
		return;
	}

	FORCEINLINE
	VOID WriteNoFence64(_Out_ _Interlocked_operand_ LONG64 volatile* Destination, _In_ LONG64 Value)

	{

		*Destination = Value;
		return;
	}

#if !defined(_M_CEE_PURE)

	FORCEINLINE
	VOID BarrierAfterRead(VOID)

	{
		__lwsync();
		return;
	}

#endif

	FORCEINLINE
	PVOID
	ReadPointerAcquire(_In_ _Interlocked_operand_ PVOID const volatile* Source)

	{

		return (PVOID)ReadAcquire((PLONG)Source);
	}

	CFORCEINLINE
	PVOID
	ReadPointerNoFence(_In_ _Interlocked_operand_ PVOID const volatile* Source)

	{

		return (PVOID)ReadNoFence((PLONG)Source);
	}

	FORCEINLINE
	LONG ReadRaw(_In_ _Interlocked_operand_ LONG const volatile* Source)

	{

		LONG Value;

		Value = *(LONG*)Source;
		return Value;
	}

	CFORCEINLINE
	VOID WriteRaw(_Out_ _Interlocked_operand_ LONG volatile* Destination, _In_ LONG Value)

	{

		*(LONG*)Destination = Value;
		return;
	}

	FORCEINLINE
	PVOID
	ReadPointerRaw(_In_ _Interlocked_operand_ PVOID const volatile* Source)

	{

		return (PVOID)ReadRaw((PLONG)Source);
	}

	CFORCEINLINE
	VOID WritePointerRelease(_Out_ _Interlocked_operand_ PVOID volatile* Destination, _In_ PVOID Value)

	{

		WriteRelease((PLONG)Destination, (LONG)Value);
		return;
	}

	FORCEINLINE
	VOID WritePointerNoFence(_Out_ _Interlocked_operand_ PVOID volatile* Destination, _In_opt_ PVOID Value)

	{

		WriteNoFence((PLONG)Destination, (LONG)Value);
		return;
	}

	FORCEINLINE
	VOID WritePointerRaw(_Out_ _Interlocked_operand_ PVOID volatile* Destination, _In_opt_ PVOID Value)

	{

		WriteRaw((PLONG)Destination, (LONG)Value);
		return;
	}

#define ReadLongPtrAcquire ReadAcquire

#define ReadLongPtrNoFence ReadNoFence

#define ReadLongPtrRaw ReadRaw

#define WriteLongPtrRelease WriteRelease

#define WriteLongPtrNoFence WriteNoFence

#define WriteLongPtrRaw WriteRaw

#define ReadULongPtrAcquire ReadULongAcquire

#define ReadULongPtrNoFence ReadULongNoFence

#define ReadULongPtrRaw ReadULongRaw

#define WriteULongPtrRelease WriteULongRelease

#define WriteULongPtrNoFence WriteULongNoFence

#define WriteULongPtrRaw WriteULongRaw

#ifdef __cplusplus
}
#endif

typedef enum _HEAP_INFORMATION_CLASS
{

	HeapCompatibilityInformation = 0,
	HeapEnableTerminationOnCorruption = 1

#if ((NTDDI_VERSION > NTDDI_WINBLUE) || (NTDDI_VERSION == NTDDI_WINBLUE && defined(WINBLUE_KBSPRING14)))
		,

	HeapOptimizeResources = 3

#endif

		,

	HeapTag = 7

} HEAP_INFORMATION_CLASS;

typedef struct _OBJECT_BOUNDARY_DESCRIPTOR
{
	ULONG Version;
	ULONG Items;
	ULONG TotalSize;
	union {
		ULONG Flags;
		struct
		{
			ULONG AddAppContainerSid : 1;
			ULONG Reserved : 31;
		};
	};
} OBJECT_BOUNDARY_DESCRIPTOR, *POBJECT_BOUNDARY_DESCRIPTOR;

#define _TRACEHANDLE_DEFINED
typedef __int64 TRACEHANDLE, *PTRACEHANDLE;

typedef struct _ETW_BUFFER_CONTEXT
{
	union {
		struct
		{
			UCHAR ProcessorNumber;
			UCHAR Alignment;
		} DUMMYSTRUCTNAME;
		USHORT ProcessorIndex;
	} DUMMYUNIONNAME;
	USHORT LoggerId;
} ETW_BUFFER_CONTEXT, *PETW_BUFFER_CONTEXT;

typedef LPVOID(WINAPI* PENCLAVE_ROUTINE)(LPVOID lpThreadParameter);
typedef PENCLAVE_ROUTINE LPENCLAVE_ROUTINE;

//
// WNODE definition
typedef struct _WNODE_HEADER
{
	ULONG BufferSize; // Size of entire buffer inclusive of this ULONG
	ULONG ProviderId; // Provider Id of driver returning this buffer
	union {
		ULONG64 HistoricalContext; // Logger use
		struct
		{
			ULONG Version; // Reserved
			ULONG Linkage; // Linkage field reserved for WMI
		} DUMMYSTRUCTNAME;
	} DUMMYUNIONNAME;

	union {
		ULONG CountLost;         // Reserved
		HANDLE KernelHandle;     // Kernel handle for data block
		LARGE_INTEGER TimeStamp; // Timestamp as returned in units of 100ns
								 // since 1/1/1601
	} DUMMYUNIONNAME2;
	GUID Guid; // Guid for data block returned with results
	ULONG ClientContext;
	ULONG Flags; // Flags, see below
} WNODE_HEADER, *PWNODE_HEADER;

//
// Base signing levels.
//

typedef BYTE SE_SIGNING_LEVEL, *PSE_SIGNING_LEVEL;

#define SE_SIGNING_LEVEL_UNCHECKED       0x00000000
#define SE_SIGNING_LEVEL_UNSIGNED        0x00000001
#define SE_SIGNING_LEVEL_ENTERPRISE      0x00000002
#define SE_SIGNING_LEVEL_CUSTOM_1        0x00000003
#define SE_SIGNING_LEVEL_DEVELOPER       SE_SIGNING_LEVEL_CUSTOM_1
#define SE_SIGNING_LEVEL_AUTHENTICODE    0x00000004
#define SE_SIGNING_LEVEL_CUSTOM_2        0x00000005
#define SE_SIGNING_LEVEL_STORE           0x00000006
#define SE_SIGNING_LEVEL_CUSTOM_3        0x00000007
#define SE_SIGNING_LEVEL_ANTIMALWARE     SE_SIGNING_LEVEL_CUSTOM_3
#define SE_SIGNING_LEVEL_MICROSOFT       0x00000008
#define SE_SIGNING_LEVEL_CUSTOM_4        0x00000009
#define SE_SIGNING_LEVEL_CUSTOM_5        0x0000000A
#define SE_SIGNING_LEVEL_DYNAMIC_CODEGEN 0x0000000B
#define SE_SIGNING_LEVEL_WINDOWS         0x0000000C
#define SE_SIGNING_LEVEL_CUSTOM_7        0x0000000D
#define SE_SIGNING_LEVEL_WINDOWS_TCB     0x0000000E
#define SE_SIGNING_LEVEL_CUSTOM_6        0x0000000F

typedef ULONG NOTIFICATION_MASK;
#define TRANSACTION_NOTIFY_MASK                0x3FFFFFFF
#define TRANSACTION_NOTIFY_PREPREPARE          0x00000001
#define TRANSACTION_NOTIFY_PREPARE             0x00000002
#define TRANSACTION_NOTIFY_COMMIT              0x00000004
#define TRANSACTION_NOTIFY_ROLLBACK            0x00000008
#define TRANSACTION_NOTIFY_PREPREPARE_COMPLETE 0x00000010
#define TRANSACTION_NOTIFY_PREPARE_COMPLETE    0x00000020
#define TRANSACTION_NOTIFY_COMMIT_COMPLETE     0x00000040
#define TRANSACTION_NOTIFY_ROLLBACK_COMPLETE   0x00000080
#define TRANSACTION_NOTIFY_RECOVER             0x00000100
#define TRANSACTION_NOTIFY_SINGLE_PHASE_COMMIT 0x00000200
#define TRANSACTION_NOTIFY_DELEGATE_COMMIT     0x00000400
#define TRANSACTION_NOTIFY_RECOVER_QUERY       0x00000800
#define TRANSACTION_NOTIFY_ENLIST_PREPREPARE   0x00001000
#define TRANSACTION_NOTIFY_LAST_RECOVER        0x00002000
#define TRANSACTION_NOTIFY_INDOUBT             0x00004000
#define TRANSACTION_NOTIFY_PROPAGATE_PULL      0x00008000
#define TRANSACTION_NOTIFY_PROPAGATE_PUSH      0x00010000
#define TRANSACTION_NOTIFY_MARSHAL             0x00020000
#define TRANSACTION_NOTIFY_ENLIST_MASK         0x00040000
#define TRANSACTION_NOTIFY_RM_DISCONNECTED     0x01000000
#define TRANSACTION_NOTIFY_TM_ONLINE           0x02000000
#define TRANSACTION_NOTIFY_COMMIT_REQUEST      0x04000000
#define TRANSACTION_NOTIFY_PROMOTE             0x08000000
#define TRANSACTION_NOTIFY_PROMOTE_NEW         0x10000000
#define TRANSACTION_NOTIFY_REQUEST_OUTCOME     0x20000000

typedef struct _TOKEN_MANDATORY_POLICY
{
	DWORD Policy;
} TOKEN_MANDATORY_POLICY, *PTOKEN_MANDATORY_POLICY;

//
// Types of objects known to the kernel transaction manager.
//

typedef enum _KTMOBJECT_TYPE
{

	KTMOBJECT_TRANSACTION,
	KTMOBJECT_TRANSACTION_MANAGER,
	KTMOBJECT_RESOURCE_MANAGER,
	KTMOBJECT_ENLISTMENT,
	KTMOBJECT_INVALID

} KTMOBJECT_TYPE, *PKTMOBJECT_TYPE;

//
// KTMOBJECT_CURSOR
//
// Used by NtEnumerateTransactionObject to enumerate a transaction
// object namespace (e.g. enlistments in a resource manager).
//

typedef struct _KTMOBJECT_CURSOR
{

	//
	// The last GUID enumerated; zero if beginning enumeration.
	//

	GUID LastQuery;

	//
	// A count of GUIDs filled in by this last enumeration.
	//

	DWORD ObjectIdCount;

	//
	// ObjectIdCount GUIDs from the namespace specified.
	//

	GUID ObjectIds[1];

} KTMOBJECT_CURSOR, *PKTMOBJECT_CURSOR;

// TODO: warning, duplicated def in tm.h.
typedef struct _TRANSACTION_NOTIFICATION
{
	PVOID TransactionKey;
	ULONG TransactionNotification;
	LARGE_INTEGER TmVirtualClock;
	ULONG ArgumentLength;
} TRANSACTION_NOTIFICATION, *PTRANSACTION_NOTIFICATION;

// begin_wdm
typedef enum _ENLISTMENT_INFORMATION_CLASS
{
	EnlistmentBasicInformation,
	EnlistmentRecoveryInformation,
	EnlistmentCrmInformation
} ENLISTMENT_INFORMATION_CLASS;

// begin_wdm
typedef enum _RESOURCEMANAGER_INFORMATION_CLASS
{
	ResourceManagerBasicInformation,
	ResourceManagerCompletionInformation,
} RESOURCEMANAGER_INFORMATION_CLASS;

// begin_wdm
typedef enum _TRANSACTION_INFORMATION_CLASS
{
	TransactionBasicInformation,
	TransactionPropertiesInformation,
	TransactionEnlistmentInformation,
	TransactionSuperiorEnlistmentInformation
		// end_wdm
		,
	// The following info-classes are intended for DTC's use only; it will be
	// deprecated, and no one else should take a dependency on it.
	TransactionBindInformation,      // private and deprecated
	TransactionDTCPrivateInformation // private and deprecated
		,
	// begin_wdm
} TRANSACTION_INFORMATION_CLASS;

// begin_wdm
typedef enum _TRANSACTIONMANAGER_INFORMATION_CLASS
{
	TransactionManagerBasicInformation,
	TransactionManagerLogInformation,
	TransactionManagerLogPathInformation,
	TransactionManagerRecoveryInformation = 4
		// end_wdm
		,
	// The following info-classes are intended for internal use only; they
	// are considered deprecated, and no one else should take a dependency
	// on them.
	TransactionManagerOnlineProbeInformation = 3,
	TransactionManagerOldestTransactionInformation = 5
	// end_wdm

	// begin_wdm
} TRANSACTIONMANAGER_INFORMATION_CLASS;

typedef GUID CRM_PROTOCOL_ID, *PCRM_PROTOCOL_ID;
typedef struct _PORT_MESSAGE* PPORT_MESSAGE;

typedef struct _WORKER_FACTORY_DEFERRED_WORK
{
	PPORT_MESSAGE AlpcSendMessage;
	PVOID AlpcSendMessagePort;
	ULONG AlpcSendMessageFlags;
	ULONG Flags;
} WORKER_FACTORY_DEFERRED_WORK, *PWORKER_FACTORY_DEFERRED_WORK;

typedef double DOUBLE;
typedef GUID* PGUID;

// Desktop access rights
#define DESKTOP_ALL_ACCESS                                                                                                       \
	(DESKTOP_CREATEMENU | DESKTOP_CREATEWINDOW | DESKTOP_ENUMERATE | DESKTOP_HOOKCONTROL | DESKTOP_JOURNALPLAYBACK |             \
	 DESKTOP_JOURNALRECORD | DESKTOP_READOBJECTS | DESKTOP_SWITCHDESKTOP | DESKTOP_WRITEOBJECTS | STANDARD_RIGHTS_REQUIRED)
#define DESKTOP_GENERIC_READ (DESKTOP_ENUMERATE | DESKTOP_READOBJECTS | STANDARD_RIGHTS_READ)
#define DESKTOP_GENERIC_WRITE                                                                                                    \
	(DESKTOP_CREATEMENU | DESKTOP_CREATEWINDOW | DESKTOP_HOOKCONTROL | DESKTOP_JOURNALPLAYBACK | DESKTOP_JOURNALRECORD |         \
	 DESKTOP_WRITEOBJECTS | STANDARD_RIGHTS_WRITE)
#define DESKTOP_GENERIC_EXECUTE (DESKTOP_SWITCHDESKTOP | STANDARD_RIGHTS_EXECUTE)

// Window station access rights
#define WINSTA_GENERIC_READ                                                                                                      \
	(WINSTA_ENUMDESKTOPS | WINSTA_ENUMERATE | WINSTA_READATTRIBUTES | WINSTA_READSCREEN | STANDARD_RIGHTS_READ)
#define WINSTA_GENERIC_WRITE   (WINSTA_ACCESSCLIPBOARD | WINSTA_CREATEDESKTOP | WINSTA_WRITEATTRIBUTES | STANDARD_RIGHTS_WRITE)
#define WINSTA_GENERIC_EXECUTE (WINSTA_ACCESSGLOBALATOMS | WINSTA_EXITWINDOWS | STANDARD_RIGHTS_EXECUTE)

// WMI access rights
#define WMIGUID_GENERIC_READ  (WMIGUID_QUERY | WMIGUID_NOTIFICATION | WMIGUID_READ_DESCRIPTION | STANDARD_RIGHTS_READ)
#define WMIGUID_GENERIC_WRITE (WMIGUID_SET | TRACELOG_CREATE_REALTIME | TRACELOG_CREATE_ONDISK | STANDARD_RIGHTS_WRITE)
#define WMIGUID_GENERIC_EXECUTE                                                                                                  \
	(WMIGUID_EXECUTE | TRACELOG_GUID_ENABLE | TRACELOG_LOG_EVENT | TRACELOG_ACCESS_REALTIME | TRACELOG_REGISTER_GUIDS |          \
	 STANDARD_RIGHTS_EXECUTE)

// Note: Some parts of the Windows Runtime, COM or third party hooks are returning
// S_FALSE and null pointers on errors when S_FALSE is a success code. (dmex)
#define HR_SUCCESS(hr) (((HRESULT)(hr)) == S_OK)
#define HR_FAILED(hr)  (((HRESULT)(hr)) != S_OK)

// Note: The CONTAINING_RECORD macro doesn't support UBSan and generates false positives,
// we redefine the macro with FIELD_OFFSET as a workaround until the WinSDK is fixed (dmex)
//#undef CONTAINING_RECORD
//#define CONTAINING_RECORD(address, type, field) ((type*)((ULONG_PTR)(address)-UFIELD_OFFSET(type, field)))

#ifndef __PCGUID_DEFINED__
#define __PCGUID_DEFINED__
typedef const GUID* PCGUID;
#endif

#endif
