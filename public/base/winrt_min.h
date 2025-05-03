/// \file Declares WinRT interfaces in a way that doesn't drag in a ton of STL stuff
/// \copyright 2025 Randomcode Developers

/// This file was made by copying stuff from IDL files and manually turning it into valid C++.
/// It mostly works, but when something is wrong it's excruciating to fix.

#pragma once

#ifndef CH_WIN32
#error "This file is Windows only"
#endif

#include "base/compiler.h"
#include "base/platform.h"
#include "base/types.h"

#include <roapi.h>
#include <winstring.h>

namespace winrt_min
{

struct EventRegistrationToken
{
	s64 value;
};

template <typename T> struct ATTRIBUTE(novtable) EventHandler: public IUnknown
{
	virtual s32 STDMETHODCALLTYPE Invoke(void*, T arg) = 0;
};

template <typename S, typename R> struct ATTRIBUTE(novtable) TypedEventHandler: public IUnknown
{
	virtual s32 STDMETHODCALLTYPE Invoke(S, R) = 0;
};

const ATTRIBUTE(selectany) IID& IID_AnyEventHandler = {
	0x9DE1C535, 0x6AE1, 0x11E0, {0x84, 0xE1, 0x18, 0xA9, 0x05, 0xBC, 0xC5, 0x3F}
};

#define MAKE_INTERFACE(name, uuid, body)                                                                                         \
	MIDL_INTERFACE(uuid) name body;                                                                                              \
	const ATTRIBUTE(selectany) IID& IID_##name = __uuidof(name);

MAKE_INTERFACE(IPropertySet, "8A43ED9F-F4E6-4421-ACF9-1DAB2986820C", : public IInspectable{})

struct Point
{
	f32 X;
	f32 Y;
};

struct Rect
{
	f32 X;
	f32 Y;
	f32 Width;
	f32 Height;
};

struct Size
{
	f32 Width;
	f32 Height;
};

enum class CoreWindowFlowDirection
{
	LeftToRight = 0,
	RightToLeft = 1
};

enum class CoreCursorType
{
	Arrow = 0,
	Cross = 1,
	Custom = 2,
	Hand = 3,
	Help = 4,
	IBeam = 5,
	SizeAll = 6,
	SizeNortheastSouthwest = 7,
	SizeNorthSouth = 8,
	SizeNorthwestSoutheast = 9,
	SizeWestEast = 10,
	UniversalNo = 11,
	UpArrow = 12,
	Wait = 13,
	Pin = 14,
	Person = 15
};

enum class VirtualKey
{
	None = 0,
	LeftButton = 1,
	RightButton = 2,
	Cancel = 3,
	MiddleButton = 4,
	XButton1 = 5,
	XButton2 = 6,
	Back = 8,
	Tab = 9,
	Clear = 12,
	Enter = 13,
	Shift = 16,
	Control = 17,
	Menu = 18,
	Pause = 19,
	CapitalLock = 20,
	Kana = 21,
	Hangul = 21,
	ImeOn = 22,
	Junja = 23,
	Final = 24,
	Hanja = 25,
	Kanji = 25,
	ImeOff = 26,
	Escape = 27,
	Convert = 28,
	NonConvert = 29,
	Accept = 30,
	ModeChange = 31,
	Space = 32,
	PageUp = 33,
	PageDown = 34,
	End = 35,
	Home = 36,
	Left = 37,
	Up = 38,
	Right = 39,
	Down = 40,
	Select = 41,
	Print = 42,
	Execute = 43,
	Snapshot = 44,
	Insert = 45,
	Delete = 46,
	Help = 47,
	Number0 = 48,
	Number1 = 49,
	Number2 = 50,
	Number3 = 51,
	Number4 = 52,
	Number5 = 53,
	Number6 = 54,
	Number7 = 55,
	Number8 = 56,
	Number9 = 57,
	A = 65,
	B = 66,
	C = 67,
	D = 68,
	E = 69,
	F = 70,
	G = 71,
	H = 72,
	I = 73,
	J = 74,
	K = 75,
	L = 76,
	M = 77,
	N = 78,
	O = 79,
	P = 80,
	Q = 81,
	R = 82,
	S = 83,
	T = 84,
	U = 85,
	V = 86,
	W = 87,
	X = 88,
	Y = 89,
	Z = 90,
	LeftWindows = 91,
	RightWindows = 92,
	Application = 93,
	Sleep = 95,
	NumberPad0 = 96,
	NumberPad1 = 97,
	NumberPad2 = 98,
	NumberPad3 = 99,
	NumberPad4 = 100,
	NumberPad5 = 101,
	NumberPad6 = 102,
	NumberPad7 = 103,
	NumberPad8 = 104,
	NumberPad9 = 105,
	Multiply = 106,
	Add = 107,
	Separator = 108,
	Subtract = 109,
	Decimal = 110,
	Divide = 111,
	F1 = 112,
	F2 = 113,
	F3 = 114,
	F4 = 115,
	F5 = 116,
	F6 = 117,
	F7 = 118,
	F8 = 119,
	F9 = 120,
	F10 = 121,
	F11 = 122,
	F12 = 123,
	F13 = 124,
	F14 = 125,
	F15 = 126,
	F16 = 127,
	F17 = 128,
	F18 = 129,
	F19 = 130,
	F20 = 131,
	F21 = 132,
	F22 = 133,
	F23 = 134,
	F24 = 135,
	NavigationView = 136,
	NavigationMenu = 137,
	NavigationUp = 138,
	NavigationDown = 139,
	NavigationLeft = 140,
	NavigationRight = 141,
	NavigationAccept = 142,
	NavigationCancel = 143,
	NumberKeyLock = 144,
	Scroll = 145,
	LeftShift = 160,
	RightShift = 161,
	LeftControl = 162,
	RightControl = 163,
	LeftMenu = 164,
	RightMenu = 165,
	GoBack = 166,
	GoForward = 167,
	Refresh = 168,
	Stop = 169,
	Search = 170,
	Favorites = 171,
	GoHome = 172,
	GamepadA = 195,
	GamepadB = 196,
	GamepadX = 197,
	GamepadY = 198,
	GamepadRightShoulder = 199,
	GamepadLeftShoulder = 200,
	GamepadLeftTrigger = 201,
	GamepadRightTrigger = 202,
	GamepadDPadUp = 203,
	GamepadDPadDown = 204,
	GamepadDPadLeft = 205,
	GamepadDPadRight = 206,
	GamepadMenu = 207,
	GamepadView = 208,
	GamepadLeftThumbstickButton = 209,
	GamepadRightThumbstickButton = 210,
	GamepadLeftThumbstickUp = 211,
	GamepadLeftThumbstickDown = 212,
	GamepadLeftThumbstickRight = 213,
	GamepadLeftThumbstickLeft = 214,
	GamepadRightThumbstickUp = 215,
	GamepadRightThumbstickDown = 216,
	GamepadRightThumbstickRight = 217,
	GamepadRightThumbstickLeft = 218
};

enum CoreVirtualKeyStates
{
	None = 0x0,
	Down = 0x1,
	Locked = 0x2
};

MAKE_INTERFACE(
	ICoreWindowEventArgs, "272B1EF3-C633-4DA5-A26C-C6D0F56B29DA", : public IInspectable {
		virtual HRESULT STDMETHODCALLTYPE Handled(bool value) = 0;
		virtual HRESULT STDMETHODCALLTYPE Handled(bool* value) = 0;
	})

MAKE_INTERFACE(
	IWindowSizeChangedEventArgs,
	"5A200EC7-0426-47DC-B86C-6F475915E451", : public IInspectable { virtual HRESULT STDMETHODCALLTYPE Size(Size * value) = 0; })

MAKE_INTERFACE(
	ICoreCursor, "96893ACF-111D-442C-8A77-B87992F8E2D6", : public IInspectable {
		virtual HRESULT STDMETHODCALLTYPE Id(u32 value) = 0;
		virtual HRESULT STDMETHODCALLTYPE Type(CoreCursorType * value) = 0;
	})

MAKE_INTERFACE(
	ICoreWindow, "79B9D5F2-879E-4B89-B798-79E47598030C", : public IInspectable {
		virtual HRESULT STDMETHODCALLTYPE AutomationHostProvider(IInspectable * *value) = 0;
		virtual HRESULT STDMETHODCALLTYPE Bounds(Rect * value) = 0;
		virtual HRESULT STDMETHODCALLTYPE CustomProperties(IPropertySet * *value) = 0;
		virtual HRESULT STDMETHODCALLTYPE Dispatcher(void** value) = 0;
		virtual HRESULT STDMETHODCALLTYPE FlowDirection(void* value) = 0;
		virtual HRESULT STDMETHODCALLTYPE FlowDirection(CoreWindowFlowDirection value) = 0;
		virtual HRESULT STDMETHODCALLTYPE IsInputEnabled(bool* value) = 0;
		virtual HRESULT STDMETHODCALLTYPE IsInputEnabled(bool value) = 0;
		virtual HRESULT STDMETHODCALLTYPE PointerCursor(ICoreCursor * *value) = 0;
		virtual HRESULT STDMETHODCALLTYPE PointerCursor(ICoreCursor * value) = 0;
		virtual HRESULT STDMETHODCALLTYPE PointerPosition(Point * value) = 0;
		virtual HRESULT STDMETHODCALLTYPE Visible(bool* value) = 0;
		virtual HRESULT STDMETHODCALLTYPE Activate() = 0;
		virtual HRESULT STDMETHODCALLTYPE Close() = 0;
		virtual HRESULT STDMETHODCALLTYPE GetAsyncKeyState(VirtualKey virtualKey, CoreVirtualKeyStates * KeyState) = 0;
		virtual HRESULT STDMETHODCALLTYPE GetKeyState(VirtualKey virtualKey, CoreVirtualKeyStates * KeyState) = 0;
		virtual HRESULT STDMETHODCALLTYPE ReleasePointerCapture() = 0;
		virtual HRESULT STDMETHODCALLTYPE SetPointerCapture() = 0;
		virtual HRESULT STDMETHODCALLTYPE Activated(EventRegistrationToken cookie) = 0;
		virtual HRESULT STDMETHODCALLTYPE Activated(
			TypedEventHandler<ICoreWindow*, void*> * handler, EventRegistrationToken * pCookie) = 0;
		virtual HRESULT STDMETHODCALLTYPE AutomationProviderRequested(EventRegistrationToken cookie) = 0;
		virtual HRESULT STDMETHODCALLTYPE AutomationProviderRequested(
			TypedEventHandler<ICoreWindow*, void*> * handler, EventRegistrationToken * cookie) = 0;
		virtual HRESULT STDMETHODCALLTYPE CharacterReceived(EventRegistrationToken cookie) = 0;
		virtual HRESULT STDMETHODCALLTYPE CharacterReceived(
			TypedEventHandler<ICoreWindow*, void*> * handler, EventRegistrationToken * pCookie) = 0;
		virtual HRESULT STDMETHODCALLTYPE Closed(EventRegistrationToken cookie) = 0;
		virtual HRESULT STDMETHODCALLTYPE Closed(
			TypedEventHandler<ICoreWindow*, void*> * handler, EventRegistrationToken * pCookie) = 0;
		virtual HRESULT STDMETHODCALLTYPE InputEnabled(EventRegistrationToken cookie) = 0;
		virtual HRESULT STDMETHODCALLTYPE InputEnabled(
			TypedEventHandler<ICoreWindow*, void*> * handler, EventRegistrationToken * pCookie) = 0;
		virtual HRESULT STDMETHODCALLTYPE KeyDown(EventRegistrationToken cookie) = 0;
		virtual HRESULT STDMETHODCALLTYPE KeyDown(
			TypedEventHandler<ICoreWindow*, void*> * handler, EventRegistrationToken * pCookie) = 0;
		virtual HRESULT STDMETHODCALLTYPE KeyUp(EventRegistrationToken cookie) = 0;
		virtual HRESULT STDMETHODCALLTYPE KeyUp(
			TypedEventHandler<ICoreWindow*, void*> * handler, EventRegistrationToken * pCookie) = 0;
		virtual HRESULT STDMETHODCALLTYPE PointerCaptureLost(EventRegistrationToken cookie) = 0;
		virtual HRESULT STDMETHODCALLTYPE PointerCaptureLost(
			TypedEventHandler<ICoreWindow*, void*> * handler, EventRegistrationToken * cookie) = 0;
		virtual HRESULT STDMETHODCALLTYPE PointerEntered(EventRegistrationToken cookie) = 0;
		virtual HRESULT STDMETHODCALLTYPE PointerEntered(
			TypedEventHandler<ICoreWindow*, void*> * handler, EventRegistrationToken * cookie) = 0;
		virtual HRESULT STDMETHODCALLTYPE PointerExited(EventRegistrationToken cookie) = 0;
		virtual HRESULT STDMETHODCALLTYPE PointerExited(
			TypedEventHandler<ICoreWindow*, void*> * handler, EventRegistrationToken * cookie) = 0;
		virtual HRESULT STDMETHODCALLTYPE PointerMoved(EventRegistrationToken cookie) = 0;
		virtual HRESULT STDMETHODCALLTYPE PointerMoved(
			TypedEventHandler<ICoreWindow*, void*> * handler, EventRegistrationToken * cookie) = 0;
		virtual HRESULT STDMETHODCALLTYPE PointerPressed(EventRegistrationToken cookie) = 0;
		virtual HRESULT STDMETHODCALLTYPE PointerPressed(
			TypedEventHandler<ICoreWindow*, void*> * handler, EventRegistrationToken * cookie) = 0;
		virtual HRESULT STDMETHODCALLTYPE PointerReleased(EventRegistrationToken cookie) = 0;
		virtual HRESULT STDMETHODCALLTYPE PointerReleased(
			TypedEventHandler<ICoreWindow*, void*> * handler, EventRegistrationToken * cookie) = 0;
		virtual HRESULT STDMETHODCALLTYPE TouchHitTesting(EventRegistrationToken cookie) = 0;
		virtual HRESULT STDMETHODCALLTYPE TouchHitTesting(
			TypedEventHandler<ICoreWindow*, void*> * handler, EventRegistrationToken * pCookie) = 0;
		virtual HRESULT STDMETHODCALLTYPE PointerWheelChanged(EventRegistrationToken cookie) = 0;
		virtual HRESULT STDMETHODCALLTYPE PointerWheelChanged(
			TypedEventHandler<ICoreWindow*, void*> * handler, EventRegistrationToken * cookie) = 0;
		virtual HRESULT STDMETHODCALLTYPE SizeChanged(EventRegistrationToken cookie) = 0;
		virtual HRESULT STDMETHODCALLTYPE SizeChanged(
			TypedEventHandler<ICoreWindow*, IWindowSizeChangedEventArgs*> * handler, EventRegistrationToken * pCookie) = 0;
		virtual HRESULT STDMETHODCALLTYPE VisibilityChanged(EventRegistrationToken cookie) = 0;
		virtual HRESULT STDMETHODCALLTYPE VisibilityChanged(
			TypedEventHandler<ICoreWindow*, void*> * handler, EventRegistrationToken * pCookie) = 0;
	})

const PCWSTR RuntimeClass_CoreWindow = L"Windows.UI.Core.CoreWindow";

enum class CoreProcessEventsOption
{
	ProcessOneAndAllPending = 0,
	ProcessOneIfPresent = 1,
	ProcessUntilQuit = 2,
	ProcessAllIfPresent = 3
};

MAKE_INTERFACE(
	ICoreDispatcher, "60DB2FA8-B705-4FDE-A7D6-EBBB1891D39E", : public IInspectable {
		virtual HRESULT STDMETHODCALLTYPE HasThreadAccess(bool* value) = 0;
		virtual HRESULT STDMETHODCALLTYPE ProcessEvents(CoreProcessEventsOption options) = 0;
		virtual HRESULT STDMETHODCALLTYPE RunAsync(u32 priority, void* agileCallback, void** asyncAction) = 0;
		virtual HRESULT STDMETHODCALLTYPE RunIdleAsync(void* agileCallback, void** asyncAction) = 0;
	})

MAKE_INTERFACE(
	ICoreApplicationView, "638BB2DB-451D-4661-B099-414F34FFB9F1", : public IInspectable {
		virtual HRESULT STDMETHODCALLTYPE CoreWindow(void** value) = 0;
		virtual HRESULT STDMETHODCALLTYPE Activated(
			TypedEventHandler<ICoreApplicationView*, void*> * handler, EventRegistrationToken * token) = 0;
		virtual HRESULT STDMETHODCALLTYPE Activated(EventRegistrationToken token) = 0;
		virtual HRESULT STDMETHODCALLTYPE IsMain(bool* value) = 0;
		virtual HRESULT STDMETHODCALLTYPE IsHosted(bool* value) = 0;
	})

// NOT INHERITED FROM ICoreApplicationView
MAKE_INTERFACE(
	ICoreApplicationView2, "68EB7ADF-917F-48EB-9AEB-7DE53E086AB1", : public IInspectable {
		virtual HRESULT STDMETHODCALLTYPE Dispatcher(ICoreDispatcher * *value) = 0;
	})

MAKE_INTERFACE(
	IFrameworkView, "FAAB5CD0-8924-45AC-AD0F-A08FAE5D0324", : public IInspectable {
		virtual HRESULT STDMETHODCALLTYPE Initialize(ICoreApplicationView * applicationView) = 0;
		virtual HRESULT STDMETHODCALLTYPE SetWindow(ICoreWindow * window) = 0;
		virtual HRESULT STDMETHODCALLTYPE Load(HSTRING entryPoint) = 0;
		virtual HRESULT STDMETHODCALLTYPE Run() = 0;
		virtual HRESULT STDMETHODCALLTYPE Uninitialize() = 0;
	})

MAKE_INTERFACE(
	IFrameworkViewSource, "CD770614-65C4-426C-9494-34FC43554862", : public IInspectable {
		virtual HRESULT STDMETHODCALLTYPE CreateView(IFrameworkView * *viewProvider) = 0;
	})

MAKE_INTERFACE(
	ICoreApplication, "0AACF7A4-5E1D-49DF-8034-FB6A68BC5ED1", : public IInspectable {
		virtual HRESULT STDMETHODCALLTYPE Id(HSTRING * value) = 0;
		virtual HRESULT STDMETHODCALLTYPE Suspending(EventRegistrationToken token) = 0;
		virtual HRESULT STDMETHODCALLTYPE Suspending(EventHandler<void*> * handler, EventRegistrationToken * token) = 0;
		virtual HRESULT STDMETHODCALLTYPE Resuming(EventRegistrationToken token) = 0;
		virtual HRESULT STDMETHODCALLTYPE Resuming(EventHandler<IInspectable*> * handler, EventRegistrationToken * token) = 0;
		virtual HRESULT STDMETHODCALLTYPE Properties(IPropertySet * *value) = 0;
		virtual HRESULT STDMETHODCALLTYPE GetCurrentView(ICoreApplicationView * *value) = 0;
		virtual HRESULT STDMETHODCALLTYPE Run(IFrameworkViewSource * viewSource) = 0;
		virtual HRESULT STDMETHODCALLTYPE RunWithActivationFactories(void* activationFactoryCallback) = 0;
	})

const PCWSTR RuntimeClass_CoreApplication = L"Windows.ApplicationModel.Core.CoreApplication";

MAKE_INTERFACE(
	IStorageItem, "4207A996-CA2F-42F7-BDE8-8B10457A7F30", : public IInspectable {
		virtual HRESULT STDMETHODCALLTYPE RenameAsync(HSTRING desiredName, void** operation) = 0;
		virtual HRESULT STDMETHODCALLTYPE RenameAsync(HSTRING desiredName, UINT32 option, void** operation) = 0;
		virtual HRESULT STDMETHODCALLTYPE DeleteAsync(void** operation) = 0;
		virtual HRESULT STDMETHODCALLTYPE DeleteAsync(UINT32 option, void** operation) = 0;
		virtual HRESULT STDMETHODCALLTYPE GetBasicPropertiesAsync(void** operation) = 0;
		virtual HRESULT STDMETHODCALLTYPE Name(HSTRING * value) = 0;
		virtual HRESULT STDMETHODCALLTYPE Path(HSTRING * value) = 0;
		virtual HRESULT STDMETHODCALLTYPE Attributes(void* value) = 0;
		virtual HRESULT STDMETHODCALLTYPE DateCreated(void* value) = 0;
		virtual HRESULT STDMETHODCALLTYPE IsOfType(UINT32 type, BOOLEAN * value) = 0;
	})

MAKE_INTERFACE(
	IStorageFolder, "72D1CB78-B3EF-4F75-A80B-6FD9DAE2944B", : public IStorageItem {
		virtual HRESULT STDMETHODCALLTYPE RenameAsync(HSTRING desiredName, void** operation) = 0;
		virtual HRESULT STDMETHODCALLTYPE RenameAsync(HSTRING desiredName, UINT32 option, void** operation) = 0;
		virtual HRESULT STDMETHODCALLTYPE DeleteAsync(void** operation) = 0;
		virtual HRESULT STDMETHODCALLTYPE DeleteAsync(UINT32 option, void** operation) = 0;
		virtual HRESULT STDMETHODCALLTYPE GetBasicPropertiesAsync(void** operation) = 0;
		virtual HRESULT STDMETHODCALLTYPE Name(HSTRING * value) = 0;
		virtual HRESULT STDMETHODCALLTYPE Path(HSTRING * value) = 0;
		virtual HRESULT STDMETHODCALLTYPE Attributes(void* value) = 0;
		virtual HRESULT STDMETHODCALLTYPE DateCreated(void* value) = 0;
		virtual HRESULT STDMETHODCALLTYPE IsOfType(UINT32 type, BOOLEAN * value) = 0;

		virtual HRESULT STDMETHODCALLTYPE CreateFileAsync(HSTRING desiredName, void** operation) = 0;
		virtual HRESULT STDMETHODCALLTYPE CreateFileAsync(HSTRING desiredName, UINT32 options, void** operation) = 0;
		virtual HRESULT STDMETHODCALLTYPE CreateFolderAsync(HSTRING desiredName, void** operation) = 0;
		virtual HRESULT STDMETHODCALLTYPE CreateFolderAsync(HSTRING desiredName, UINT32 options, void** operation) = 0;
		virtual HRESULT STDMETHODCALLTYPE GetFileAsync(HSTRING name, void** operation) = 0;
		virtual HRESULT STDMETHODCALLTYPE GetFolderAsync(HSTRING name, void** operation) = 0;
		virtual HRESULT STDMETHODCALLTYPE GetItemAsync(HSTRING name, void** operation) = 0;
		virtual HRESULT STDMETHODCALLTYPE GetFilesAsyncStartAndCount(void** operation) = 0;
		virtual HRESULT STDMETHODCALLTYPE GetFoldersAsyncStartAndCount(void** operation) = 0;
		virtual HRESULT STDMETHODCALLTYPE GetItemsAsync(void** operation) = 0;
	})

MAKE_INTERFACE(
	IApplicationData, "C3DA6FB7-B744-4B45-B0B8-223A0938D0DC", : public IInspectable {
		virtual HRESULT STDMETHODCALLTYPE Version(UINT32 * value) = 0;
		virtual HRESULT STDMETHODCALLTYPE SetVersionAsync(UINT32 desiredVersion, void* handler, void** setVersionOperation) = 0;
		virtual HRESULT STDMETHODCALLTYPE ClearAllAsync(void** clearOperation) = 0;
		virtual HRESULT STDMETHODCALLTYPE ClearAsync(UINT32 locality, void** clearOperation) = 0;
		virtual HRESULT STDMETHODCALLTYPE LocalSettings(void** value) = 0;
		virtual HRESULT STDMETHODCALLTYPE RoamingSettings(void** value) = 0;
		virtual HRESULT STDMETHODCALLTYPE LocalFolder(IStorageFolder * *value) = 0;
		virtual HRESULT STDMETHODCALLTYPE RoamingFolder(IStorageFolder * *value) = 0;
		virtual HRESULT STDMETHODCALLTYPE TemporaryFolder(IStorageFolder * *value) = 0;
		virtual HRESULT STDMETHODCALLTYPE DataChanged(
			TypedEventHandler<IApplicationData*, IInspectable*> * handler, EventRegistrationToken * token) = 0;
		virtual HRESULT STDMETHODCALLTYPE DataChanged(EventRegistrationToken token) = 0;
		virtual HRESULT STDMETHODCALLTYPE SignalDataChanged() = 0;
		virtual HRESULT STDMETHODCALLTYPE RoamingStorageQuota(UINT64 * value) = 0;
	})

MAKE_INTERFACE(
	IApplicationDataStatics, "5612147B-E843-45E3-94D8-06169E3C8E17", : public IInspectable {
		virtual HRESULT STDMETHODCALLTYPE Current(IApplicationData * *value) = 0;
	})

const PCWSTR RuntimeClass_ApplicationData = L"Windows.Storage.ApplicationData";

MAKE_INTERFACE(
	IAppDataPaths, "7301D60A-79A2-48C9-9EC0-3FDA092F79E1", : public IInspectable {
		virtual HRESULT STDMETHODCALLTYPE Cookies(HSTRING * value) = 0;
		virtual HRESULT STDMETHODCALLTYPE Desktop(HSTRING * value) = 0;
		virtual HRESULT STDMETHODCALLTYPE Documents(HSTRING * value) = 0;
		virtual HRESULT STDMETHODCALLTYPE Favorites(HSTRING * value) = 0;
		virtual HRESULT STDMETHODCALLTYPE History(HSTRING * value) = 0;
		virtual HRESULT STDMETHODCALLTYPE InternetCache(HSTRING * value) = 0;
		virtual HRESULT STDMETHODCALLTYPE LocalAppData(HSTRING * value) = 0;
		virtual HRESULT STDMETHODCALLTYPE ProgramData(HSTRING * value) = 0;
		virtual HRESULT STDMETHODCALLTYPE RoamingAppData(HSTRING * value) = 0;
	})

MAKE_INTERFACE(
	IAppDataPathsStatics, "D8EB2AFE-A9D9-4B14-B999-E3921379D903", : public IInspectable {
		virtual HRESULT STDMETHODCALLTYPE GetForUser(void* user, IAppDataPaths** result) = 0;
		virtual HRESULT STDMETHODCALLTYPE GetDefault(IAppDataPaths** result) = 0;
	})

const PCWSTR RuntimeClass_AppDataPaths = L"Windows.Storage.AppDataPaths";

} // namespace winrt_min
