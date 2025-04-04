/// \file Declares WinRT interfaces in a way that doesn't drag in a ton of dynamic symbols
/// \copyright Randomcode Developers

#pragma once

#ifndef CH_WIN32
#error "This file is Windows only"
#endif

#include "base/compiler.h"
#include "base/platform.h"
#include "base/types.h"

// i honestly can't tell if i hate this file or it's beautiful

#include <roapi.h>
#include <winstring.h>

namespace winrt_min
{

struct EventRegistrationToken
{
	s64 value;
};

template <typename T> MIDL_INTERFACE("9DE1C535-6AE1-11E0-84E1-18A905BCC53F") EventHandler: IUnknown
{
	virtual s32 __stdcall Invoke(void*, T arg) = 0;
};

#define MAKE_EVENTHANDLER(T)                                                                                                     \
	const PCWSTR RuntimeClass_EventHandler##T = L"EventHandler`1<" L#T L">";                                                     \
	const ATTRIBUTE(selectany) IID& IID_EventHandler##T = __uuidof(EventHandler<T>) = 0;

template <typename S, typename R> MIDL_INTERFACE("5A648006-843A-4DA9-865B-9D26E5DFAD7B") TypedEventHandler: IUnknown
{
	virtual s32 __stdcall Invoke(S, R) = 0;
};

#define MAKE_TYPEDEVENTHANDLER(S, R)                                                                                             \
	const PCWSTR RuntimeClass_TypedEventHandler##S##_##R = L"TypedEventHandler`2<" L#S L", " L#R L">";                           \
	const ATTRIBUTE(selectany) IID& IID_TypedEventHandler##S##_##R = __uuidof(TypedEventHandler<S, R>) = 0;

#define MAKE_INTERFACE(name, uuid, body)                                                                                   \
	MIDL_INTERFACE(uuid) name body;                                                                                              \
	const ATTRIBUTE(selectany) IID& IID_##name = __uuidof(name);

MAKE_INTERFACE(IPropertySet, "8A43ED9F-F4E6-4421-ACF9-1DAB2986820C", : public IInspectable{})

MAKE_INTERFACE(
	ICoreApplicationView, "638BB2DB-451D-4661-B099-414F34FFB9F1", : public IInspectable {
		virtual HRESULT __stdcall CoreWindow(void** value) = 0;
		virtual HRESULT __stdcall Activated(
			TypedEventHandler<ICoreApplicationView*, void*>* handler, EventRegistrationToken* token) = 0;
		virtual HRESULT __stdcall Activated(EventRegistrationToken token) = 0;
		virtual HRESULT __stdcall IsMain(bool* value) = 0;
		virtual HRESULT __stdcall IsHosted(bool* value) = 0;
	})

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

enum CoreWindowFlowDirection
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
	ICoreCursor, "96893ACF-111D-442C-8A77-B87992F8E2D6", : public IInspectable {
		virtual HRESULT __stdcall Id(u32 value) = 0;
		virtual HRESULT __stdcall Type(CoreCursorType * value) = 0;
	})

MAKE_INTERFACE(
	ICoreWindow, "79B9D5F2-879E-4B89-B798-79E47598030C", : public IInspectable {
		virtual HRESULT __stdcall AutomationHostProvider(IInspectable * *value) = 0;
		virtual HRESULT __stdcall Bounds(Rect * value) = 0;
		virtual HRESULT __stdcall CustomProperties(IPropertySet * *value) = 0;
		virtual HRESULT __stdcall Dispatcher(void** value) = 0;
		virtual HRESULT __stdcall FlowDirection(void* value) = 0;
		virtual HRESULT __stdcall FlowDirection(CoreWindowFlowDirection value) = 0;
		virtual HRESULT __stdcall IsInputEnabled(bool* value) = 0;
		virtual HRESULT __stdcall IsInputEnabled(bool value) = 0;
		virtual HRESULT __stdcall PointerCursor(ICoreCursor * *value) = 0;
		virtual HRESULT __stdcall PointerCursor(ICoreCursor * value) = 0;
		virtual HRESULT __stdcall PointerPosition(Point * value) = 0;
		virtual HRESULT __stdcall Visible(bool* value) = 0;
		virtual HRESULT __stdcall Activate() = 0;
		virtual HRESULT __stdcall Close() = 0;
		virtual HRESULT __stdcall GetAsyncKeyState(VirtualKey virtualKey, CoreVirtualKeyStates * KeyState) = 0;
		virtual HRESULT __stdcall GetKeyState(VirtualKey virtualKey, CoreVirtualKeyStates * KeyState) = 0;
		virtual HRESULT __stdcall ReleasePointerCapture() = 0;
		virtual HRESULT __stdcall SetPointerCapture() = 0;
		virtual HRESULT __stdcall Activated(TypedEventHandler<ICoreWindow*, void*> * handler, EventRegistrationToken * pCookie) = 0;
		virtual HRESULT __stdcall Activated(EventRegistrationToken cookie) = 0;
		virtual HRESULT __stdcall AutomationProviderRequested(
			TypedEventHandler<ICoreWindow*, void*> * handler, EventRegistrationToken * cookie) = 0;
		virtual HRESULT __stdcall AutomationProviderRequested(EventRegistrationToken cookie) = 0;
		virtual HRESULT __stdcall CharacterReceived(
			TypedEventHandler<ICoreWindow*, void*> * handler, EventRegistrationToken * pCookie) = 0;
		virtual HRESULT __stdcall CharacterReceived(EventRegistrationToken cookie) = 0;
		virtual HRESULT __stdcall Closed(TypedEventHandler<ICoreWindow*, void*> * handler, EventRegistrationToken * pCookie) = 0;
		virtual HRESULT __stdcall Closed(EventRegistrationToken cookie) = 0;
		virtual HRESULT __stdcall InputEnabled(
			TypedEventHandler<ICoreWindow*, void*> * handler, EventRegistrationToken * pCookie) = 0;
		virtual HRESULT __stdcall InputEnabled(EventRegistrationToken cookie) = 0;
		virtual HRESULT __stdcall KeyDown(TypedEventHandler<ICoreWindow*, void*> * handler, EventRegistrationToken * pCookie) = 0;
		virtual HRESULT __stdcall KeyDown(EventRegistrationToken cookie) = 0;
		virtual HRESULT __stdcall KeyUp(TypedEventHandler<ICoreWindow*, void*> * handler, EventRegistrationToken * pCookie) = 0;
		virtual HRESULT __stdcall KeyUp(EventRegistrationToken cookie) = 0;
		virtual HRESULT __stdcall PointerCaptureLost(
			TypedEventHandler<ICoreWindow*, void*> * handler, EventRegistrationToken * cookie) = 0;
		virtual HRESULT __stdcall PointerCaptureLost(EventRegistrationToken cookie) = 0;
		virtual HRESULT __stdcall PointerEntered(
			TypedEventHandler<ICoreWindow*, void*> * handler, EventRegistrationToken * cookie) = 0;
		virtual HRESULT __stdcall PointerEntered(EventRegistrationToken cookie) = 0;
		virtual HRESULT __stdcall PointerExited(
			TypedEventHandler<ICoreWindow*, void*> * handler, EventRegistrationToken * cookie) = 0;
		virtual HRESULT __stdcall PointerExited(EventRegistrationToken cookie) = 0;
		virtual HRESULT __stdcall PointerMoved(TypedEventHandler<ICoreWindow*, void*> * handler, EventRegistrationToken * cookie) = 0;
		virtual HRESULT __stdcall PointerMoved(EventRegistrationToken cookie) = 0;
		virtual HRESULT __stdcall PointerPressed(
			TypedEventHandler<ICoreWindow*, void*> * handler, EventRegistrationToken * cookie) = 0;
		virtual HRESULT __stdcall PointerPressed(EventRegistrationToken cookie) = 0;
		virtual HRESULT __stdcall PointerReleased(
			TypedEventHandler<ICoreWindow*, void*> * handler, EventRegistrationToken * cookie) = 0;
		virtual HRESULT __stdcall PointerReleased(EventRegistrationToken cookie) = 0;
		virtual HRESULT __stdcall TouchHitTesting(
			TypedEventHandler<ICoreWindow*, void*> * handler, EventRegistrationToken * pCookie) = 0;
		virtual HRESULT __stdcall TouchHitTesting(EventRegistrationToken cookie) = 0;
		virtual HRESULT __stdcall PointerWheelChanged(
			TypedEventHandler<ICoreWindow*, void*> * handler, EventRegistrationToken * cookie) = 0;
		virtual HRESULT __stdcall PointerWheelChanged(EventRegistrationToken cookie) = 0;
		virtual HRESULT __stdcall SizeChanged(TypedEventHandler<ICoreWindow*, void*> * handler, EventRegistrationToken * pCookie) = 0;
		virtual HRESULT __stdcall SizeChanged(EventRegistrationToken cookie) = 0;
		virtual HRESULT __stdcall VisibilityChanged(
			TypedEventHandler<ICoreWindow*, void*> * handler, EventRegistrationToken * pCookie) = 0;
		virtual HRESULT __stdcall VisibilityChanged(EventRegistrationToken cookie) = 0;
	})

const PCWSTR RuntimeClass_CoreWindow = L"Windows.UI.Core.CoreWindow";

MAKE_INTERFACE(
	IFrameworkView, "FAAB5CD0-8924-45AC-AD0F-A08FAE5D0324", : public IInspectable {
		virtual HRESULT __stdcall Initialize(ICoreApplicationView * applicationView) = 0;
		virtual HRESULT __stdcall SetWindow(ICoreWindow * window) = 0;
		virtual HRESULT __stdcall Load(HSTRING entryPoint) = 0;
		virtual HRESULT __stdcall Run() = 0;
		virtual HRESULT __stdcall Uninitialize() = 0;
	})

MAKE_INTERFACE(
	IFrameworkViewSource, "CD770614-65C4-426C-9494-34FC43554862", : public IInspectable {
		virtual HRESULT __stdcall CreateView(IFrameworkView * *viewProvider) = 0;
	})

MAKE_INTERFACE(
	ICoreApplication, "0AACF7A4-5E1D-49DF-8034-FB6A68BC5ED1", : public IInspectable {
		virtual HRESULT __stdcall Id(HSTRING * value) = 0;
		virtual HRESULT __stdcall Suspending(EventHandler<void*> * handler, EventRegistrationToken* token) = 0;
		virtual HRESULT __stdcall Suspending(EventRegistrationToken token) = 0;
		virtual HRESULT __stdcall Resuming(EventHandler<IInspectable*> * handler, EventRegistrationToken* token) = 0;
		virtual HRESULT __stdcall Resuming(EventRegistrationToken token) = 0;
		virtual HRESULT __stdcall Properties(IPropertySet * *value) = 0;
		virtual HRESULT __stdcall GetCurrentView(ICoreApplicationView * *value) = 0;
		virtual HRESULT __stdcall Run(IFrameworkViewSource * viewSource) = 0;
		virtual HRESULT __stdcall RunWithActivationFactories(void* activationFactoryCallback) = 0;
	})

const PCWSTR RuntimeClass_CoreApplication = L"Windows.ApplicationModel.Core.CoreApplication";

}
