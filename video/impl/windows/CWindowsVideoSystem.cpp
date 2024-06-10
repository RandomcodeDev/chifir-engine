#include "CWindowsVideoSystem.h"
#include "framework/IPlatform.h"
#include "framework/Log.h"

#ifdef _MSC_VER
#pragma comment(                                                                                                                                     \
	linker,                                                                                                                                          \
	"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

bool CWindowsVideoSystem::Initialize()
{
	KR_LOG_INFO("Initializing Windows video system");

	m_hinstance = GetModuleHandleA(nullptr);

	if (!RegisterWindowClass() || !InitializeMainWindow())
	{
		return false;
	}

	ShowWindow(m_window, SW_NORMAL);

	KR_LOG_INFO("Windows video system initialized");

	return true;
}

bool CWindowsVideoSystem::Update()
{
	m_resized = false;

	MSG msg = {};
	while (PeekMessageA(&msg, m_window, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}

	return !m_closed;
}

void CWindowsVideoSystem::Shutdown()
{
	KR_LOG_INFO("Shutting down Windows video system");

	DestroyWindow(m_window);
	UnregisterClassA(WINDOW_CLASS, m_hinstance);

	KR_LOG_INFO("Windows video system shut down");
}

void CWindowsVideoSystem::SetTitle(const std::string& title)
{
	m_title = title;
	if (m_window)
	{
		SetWindowTextA(m_window, m_title.c_str());
	}
}

bool CWindowsVideoSystem::RegisterWindowClass()
{
	KR_LOG_DEBUG("Registering window class {}", WINDOW_CLASS);

	WNDCLASSEXA wndClass = {};
	wndClass.cbSize = sizeof(wndClass);
	wndClass.lpszClassName = WINDOW_CLASS;
	wndClass.hInstance = m_hinstance;
	wndClass.lpfnWndProc = WindowProc;
	wndClass.hCursor = LoadCursorA(nullptr, IDC_ARROW);
	if (!RegisterClassExA(&wndClass))
	{
		KR_LOG_ERROR("Failed to register window class: HRESULT 0x{08x}", HRESULT_FROM_WIN32(GetLastError()));
		return false;
	}

	KR_LOG_DEBUG("Window class registered");

	return true;
}

bool CWindowsVideoSystem::InitializeMainWindow()
{
	RECT clientArea = {};

	// make the window 2/3 of the main monitor's size by default
	clientArea.left = 0;
	clientArea.right = (u32)(GetSystemMetrics(SM_CXSCREEN) * 2 / 3);
	clientArea.top = 0;
	clientArea.bottom = (u32)(GetSystemMetrics(SM_CYSCREEN) * 2 / 3);

	// get the size the window needs to be created at for the client area to be the desired size
	RECT adjustedArea = clientArea;
	AdjustWindowRect(&adjustedArea, WINDOW_STYLE, FALSE);
	m_width = adjustedArea.right - adjustedArea.left;
	m_height = adjustedArea.bottom - adjustedArea.top;

	m_extraWidth = m_width - clientArea.right;
	m_extraHeight = m_height - clientArea.bottom;

	m_title = fmt::format("Красний Engine");

	KR_LOG_DEBUG(
		"Creating {}x{} (for internal size {}x{}) window titled {}", m_width, m_height, clientArea.right - clientArea.left,
		clientArea.bottom - clientArea.top, m_title);

	// center the window
	u32 x = GetSystemMetrics(SM_CXSCREEN) / 2 - m_width / 2;
	u32 y = GetSystemMetrics(SM_CYSCREEN) / 2 - m_height / 2;
	m_window = CreateWindowExA(0, WINDOW_CLASS, "", WINDOW_STYLE, x, y, m_width, m_height, nullptr, nullptr, m_hinstance, this);
	if (!m_window)
	{
		KR_LOG_ERROR("Failed to create window: HRESULT 0x{:08x}", HRESULT_FROM_WIN32(GetLastError()));
		return false;
	}

	// Update DPI, because size and position get messages
	UpdateDpi();

	return true;
}

void CWindowsVideoSystem::UpdateSize()
{
	RECT clientArea = {};
	GetClientRect(m_window, &clientArea);
	m_width = clientArea.right - clientArea.left;
	m_height = clientArea.bottom - clientArea.top;
}

void CWindowsVideoSystem::UpdatePosition()
{
	RECT clientArea = {};
	GetClientRect(m_window, &clientArea);
	POINT clientPos = {clientArea.left, clientArea.top};
	ClientToScreen(m_window, &clientPos);
	m_x = clientPos.x;
	m_y = clientPos.y;
}

void CWindowsVideoSystem::UpdateDpi()
{
	m_dpi = (GetDpiForWindow(m_window) / USER_DEFAULT_SCREEN_DPI);
}

LRESULT CWindowsVideoSystem::WindowProc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam)
{
	// there's only one instance of a given system at a time
	static CWindowsVideoSystem* s_videoSystem;

	switch (msg)
	{
	case WM_CREATE: {
		CREATESTRUCTA* createInfo = (CREATESTRUCTA*)lparam;
		s_videoSystem = (CWindowsVideoSystem*)createInfo->lpCreateParams;
		return 0;
	}
	case WM_DPICHANGED: {
		s_videoSystem->UpdateDpi();
		KR_LOG_INFO("Window DPI changed to {}", s_videoSystem->GetDpi());
		return 0;
	}
	case WM_MOVE: {
		s_videoSystem->UpdatePosition();
		return 0;
	}
	case WM_SIZE: {
		s_videoSystem->UpdateSize();
		KR_LOG_INFO("Window resized to {}x{}", s_videoSystem->GetWidth(), s_videoSystem->GetHeight());
		return 0;
	}
	case WM_CLOSE: {
		s_videoSystem->m_closed = true;
		KR_LOG_INFO("Window closed");
		return 0;
	}
	}

	return DefWindowProcA(window, msg, wparam, lparam);
}
