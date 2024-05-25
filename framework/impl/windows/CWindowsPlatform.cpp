#include "framework/Log.h"

#include "CWindowsPlatform.h"
#include "CWindowsSharedLibrary.h"

void CWindowsPlatform::Initialize()
{
#ifdef KR_PLATFORM_GDK
	KR_LOG_INFO("Initializing game runtime");
	HRESULT result = XGameRuntimeInitialize();
	if (!SUCCEEDED(result))
	{
		KR_QUIT("Failed to initialize game runtime: HRESULT 0x{:08x}", result);
	}
#endif

#ifndef KR_PLATFORM_XBOX
	KR_LOG_INFO("Attempting to load symbols");
	SymSetOptions(SYMOPT_DEBUG | SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
	if (!SymInitialize(GetCurrentProcess(), nullptr, FALSE))
	{
		DWORD error = GetLastError();
		KR_LOG_ERROR("Failed to initialize DbgHelp: HRESULT 0x{:08x}", HRESULT_FROM_WIN32(error));
	}
#endif
}

void CWindowsPlatform::Shutdown()
{
}

const std::string& CWindowsPlatform::DescribeOs()
{
	static std::string description;

	// This shouldn't ever change between runs
	if (description.length() > 0)
	{
		return description;
	}

	// All versions
	HKEY currentVersionKey;
	CHAR edition[32] = {};
	CHAR product[32] = {};
	DWORD size = 0;

	// Windows 10 and above
	CHAR installationType[32] = {};
	DWORD majorVersion = 0;
	DWORD minorVersion = 0;
	CHAR buildNumber[8] = {};
	DWORD ubr = 0;
	CHAR displayVersion[8] = {};
	CHAR buildLabExtended[64] = {};

	// Windows 8 and below
	CHAR version[4] = {};
	CHAR csdVersion[8] = {};
	CHAR buildLab[64] = {};

	PCSTR name = nullptr;

	BOOL isWow64 = FALSE;
#ifdef _M_IX86
	IsWow64Process(INVALID_HANDLE_VALUE, &isWow64);
#endif

	RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_QUERY_VALUE, &currentVersionKey);

	size = sizeof(edition);
	RegQueryValueExA(currentVersionKey, "EditionID", nullptr, nullptr, (LPBYTE)edition, &size);

	size = sizeof(product);
	RegQueryValueExA(currentVersionKey, "ProductName", nullptr, nullptr, (LPBYTE)product, &size);

	size = sizeof(INT);
	if (RegQueryValueExA(currentVersionKey, "CurrentMajorVersionNumber", nullptr, nullptr, (LPBYTE)&majorVersion, &size) ==
		ERROR_SUCCESS)
	{
		size = sizeof(INT);
		RegQueryValueExA(currentVersionKey, "CurrentMinorVersionNumber", nullptr, nullptr, (LPBYTE)&minorVersion, &size);

		size = sizeof(installationType);
		RegQueryValueExA(currentVersionKey, "InstallationType", nullptr, nullptr, (LPBYTE)installationType, &size);

		size = sizeof(buildNumber);
		RegQueryValueExA(currentVersionKey, "CurrentBuildNumber", nullptr, nullptr, (LPBYTE)buildNumber, &size);

		size = sizeof(INT);
		RegQueryValueExA(currentVersionKey, "UBR", nullptr, nullptr, (LPBYTE)&ubr, &size);

		size = sizeof(buildLabExtended);
		RegQueryValueExA(currentVersionKey, "BuildLabEx", nullptr, nullptr, (LPBYTE)&buildLabExtended, &size);

		size = sizeof(displayVersion);
		RegQueryValueExA(currentVersionKey, "DisplayVersion", nullptr, nullptr, (LPBYTE)displayVersion, &size);

		std::string edition(edition, std::min(strlen(edition), KR_ARRAYSIZE(edition)));
		description = fmt::format(
#ifdef KR_DEBUG
			"{} {} {}.{}.{} {}{}",
#else
			"{} {} {}.{}.{}.{} {}{}",
#endif
			edition == "SystemOS" ? "Xbox System Software" : "Windows",
			(strncmp(installationType, "Client", KR_ARRAYSIZE(installationType)) == 0) ? "Desktop" : installationType, majorVersion,
			minorVersion,
#ifdef KR_DEBUG
			buildLabExtended, edition, isWow64 ? " (WoW64)" : ""
#else
			currentBuild, ubr, edition, isWow64 ? " (WoW64)" : ""
#endif
		);
	}
	else
	{
		size = sizeof(csdVersion);
		RegQueryValueExA(currentVersionKey, "CSDVersion", nullptr, nullptr, (LPBYTE)buildLab, &size);
		size = sizeof(buildLab);
		RegQueryValueExA(currentVersionKey, "BuildLab", nullptr, nullptr, (LPBYTE)buildLab, &size);

		description = fmt::format("Windows {} {} {} (build lab {}{})", product, edition, csdVersion, buildLab, isWow64 ? ", WoW64" : "");
	}
	return description;
}

const std::string& CWindowsPlatform::DescribeHardware()
{
	return std::string();
}

[[noreturn]] void CWindowsPlatform::Quit(
	const std::string& message, bool useLastError, const std::string& file, const std::string& function, int line)
{
	std::string fullMessage = fmt::format("Fatal error in {} at {}:{}: {}", function, file, line, message);
	KR_LOG_FATAL(fullMessage);
	MessageBoxA(nullptr, fullMessage.c_str(), "FATAL ERROR", MB_ICONERROR | MB_ABORTRETRYIGNORE);
	TerminateProcess(GetCurrentProcess(), HRESULT_FROM_WIN32(useLastError ? GetLastError() : ERROR_FATAL_APP_EXIT));
}

ISharedLibrary* CWindowsPlatform::LoadLibrary(const std::string& name, const std::vector<std::string>& paths)
{
	std::string fullName = name + ".dll";

	for (const auto& searchPath : paths)
	{
		HMODULE handle = LoadLibraryA((fullName + searchPath).c_str());
		if (handle)
		{
			return new CWindowsSharedLibrary(name, handle);
		}
	}

	return nullptr;
}

bool CWindowsPlatform::CreateDirectory(const std::string& name)
{
	size_t offset = 0;
	while ((offset = name.find('/', offset)))
	{
		std::string current = name.substr(0, offset);
		if (!CreateDirectoryA(current.c_str(), NULL))
		{
			if (GetLastError() != ERROR_FILE_EXISTS)
			{
				return false;
			}
		}
	}

	return true;
}

static CWindowsPlatform s_platform;
FRAMEWORK_API IPlatform* GetPlatform()
{
	return &s_platform;
}