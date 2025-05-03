/// \file Implements UWP in pure COM, so that I don't have to use the STL or anything
/// \copyright 2025 Randomcode Developers

#include "base/async.h"
#include "base/base.h"

#include "platform_win32.h"

#ifndef CH_XBOX360
#include "base/basicstr.h"

using namespace winrt_min;

template <typename T> static HRESULT GetClass(PCWSTR name, T** instance)
{
	HSTRING string = nullptr;
	ssize length = Base_StrLength(name);
	ASSERT(SUCCEEDED(WindowsCreateString(name, static_cast<u32>(length), &string)));

	IActivationFactory* factory = nullptr;
	HRESULT result = RoGetActivationFactory(string, IID_PPV_ARGS(&factory));
	if (FAILED(result))
	{
		DbgPrint("Failed to get activation factory for class %ls: HRESULT 0x%08X\n", name, result);
		goto Done;
	}

	result = factory->QueryInterface(instance);
	if (FAILED(result))
	{
		DbgPrint("Failed to get instance of class %ls: HRESULT 0x%08X\n", name, result);
		goto Done;
	}

Done:
	if (factory)
	{
		factory->Release();
	}
	WindowsDeleteString(string);
	return result;
}

static ICoreApplication* CoreApplication;
//static IApplicationDataStatics* ApplicationDataStatics;
//static IApplicationData* ApplicationData;
static IAppDataPathsStatics* AppDataPathsStatics;
static IAppDataPaths* AppDataPaths;

DECLARE_AVAILABLE(RoInitialize);
DECLARE_AVAILABLE(RoGetActivationFactory);
DECLARE_AVAILABLE(IsImmersiveProcess);

bool Base_InitWinRt()
{
	if (!RoInitialize_Available() || !RoGetActivationFactory_Available())
	{
		return false;
	}

	if (IsImmersiveProcess_Available() && !IsImmersiveProcess(NtCurrentProcess()))
	{
		DbgPrint("Not attempting Windows Runtime initialization, not a UWP process\n");
		return false;
	}

	HRESULT result = RoInitialize(RO_INIT_MULTITHREADED);
	if (FAILED(result))
	{
		DbgPrint("Failed to initialize Windows Runtime: 0x%08X\n", result);
		return false;
	}

	result = GetClass(RuntimeClass_CoreApplication, &CoreApplication);
	if (FAILED(result))
	{
		return false;
	}

	result = GetClass(RuntimeClass_AppDataPaths, &AppDataPathsStatics);
	if (FAILED(result))
	{
		return false;
	}

	result = AppDataPathsStatics->GetDefault(&AppDataPaths);
	if (FAILED(result))
	{
		DbgPrint("Failed to get IAppDataPaths: HRESULT 0x%08X\n", result);
		return false;
	}

	return true;
}

#define COMPARE_IID(a, b) (Base_MemCompare(&(a), &(b), SIZEOF(IID)) == 0)

struct UnknownBase: public IUnknown
{
  public:
	UnknownBase() : m_refCount(1)
	{
	}
	virtual ~UnknownBase() = default;

	virtual ULONG STDMETHODCALLTYPE AddRef(void) override
	{
		m_refCount++;
		return m_refCount;
	}

	virtual ULONG STDMETHODCALLTYPE Release(void) override
	{
		if (m_refCount == 1)
		{
			delete this;
			return 0;
		}

		m_refCount--;
		return m_refCount;
	}

  private:
	Atomic<ULONG> m_refCount;
};

struct InspectableBase: public IInspectable, public UnknownBase
{
  public:
	InspectableBase(PCWSTR className, TrustLevel trustLevel, const IID* iids, ULONG iidCount)
		: UnknownBase(), m_className(className), m_classNameLength(static_cast<ULONG>(Base_StrLength(m_className))),
		  m_trustLevel(trustLevel), m_iids(iids), m_iidCount(iidCount)
	{
	}
	virtual ~InspectableBase() = default;

	virtual ULONG STDMETHODCALLTYPE AddRef(void) override
	{
		return UnknownBase::AddRef();
	}

	virtual ULONG STDMETHODCALLTYPE Release(void) override
	{
		return UnknownBase::Release();
	}

	virtual HRESULT STDMETHODCALLTYPE GetIids(ULONG* iidCount, IID** iids) override
	{
		if (!iidCount || !iids)
		{
			return E_POINTER;
		}

		*iidCount = m_iidCount;
		*iids = reinterpret_cast<IID*>(CoTaskMemAlloc(*iidCount * SIZEOF(IID)));
		if (!iids)
		{
			return E_OUTOFMEMORY;
		}

		Base_MemCopy(*iids, m_iids, *iidCount * SIZEOF(IID));
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE GetRuntimeClassName(HSTRING* className) override
	{
		if (!className)
		{
			return E_POINTER;
		}

		return WindowsCreateString(m_className, m_classNameLength, className);
	}

	virtual HRESULT STDMETHODCALLTYPE GetTrustLevel(TrustLevel* trustLevel) override
	{
		if (!trustLevel)
		{
			return E_POINTER;
		}

		*trustLevel = m_trustLevel;
		return S_OK;
	}

  private:
	PCWSTR m_className;
	ULONG m_classNameLength;
	TrustLevel m_trustLevel;
	const IID* m_iids;
	ULONG m_iidCount;
};

#define MAKE_HANDLER_IMPL(name, base, category, invoke, invokeArgs)                                                              \
	struct name##Handler: public UnknownBase, public base                                                                        \
	{                                                                                                                            \
	  public:                                                                                                                    \
		const UwpVideoCallbacks* callbacks;                                                                                      \
                                                                                                                                 \
		name##Handler(const Uwp##category##Callbacks* callbacks) : UnknownBase(), callbacks(callbacks)                           \
		{                                                                                                                        \
		}                                                                                                                        \
		virtual ~name##Handler() = default;                                                                                      \
                                                                                                                                 \
		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override                                 \
		{                                                                                                                        \
			if (!ppvObject)                                                                                                      \
			{                                                                                                                    \
				return E_POINTER;                                                                                                \
			}                                                                                                                    \
                                                                                                                                 \
			if (COMPARE_IID(riid, IID_AnyEventHandler) || COMPARE_IID(riid, __uuidof(IUnknown)))                                 \
			{                                                                                                                    \
				*ppvObject = reinterpret_cast<void*>(static_cast<base*>(this));                                                  \
			}                                                                                                                    \
			else                                                                                                                 \
			{                                                                                                                    \
				*ppvObject = nullptr;                                                                                            \
			}                                                                                                                    \
                                                                                                                                 \
			if (*ppvObject)                                                                                                      \
			{                                                                                                                    \
				reinterpret_cast<IUnknown*>(*ppvObject)->AddRef();                                                               \
				return S_OK;                                                                                                     \
			}                                                                                                                    \
			else                                                                                                                 \
			{                                                                                                                    \
				return E_NOINTERFACE;                                                                                            \
			}                                                                                                                    \
		}                                                                                                                        \
                                                                                                                                 \
		virtual ULONG STDMETHODCALLTYPE AddRef(void) override                                                                    \
		{                                                                                                                        \
			return UnknownBase::AddRef();                                                                                        \
		}                                                                                                                        \
                                                                                                                                 \
		virtual ULONG STDMETHODCALLTYPE Release(void) override                                                                   \
		{                                                                                                                        \
			return UnknownBase::Release();                                                                                       \
		}                                                                                                                        \
                                                                                                                                 \
		virtual s32 STDMETHODCALLTYPE Invoke invokeArgs override                                                                 \
		{                                                                                                                        \
			invoke;                                                                                                              \
			return 0;                                                                                                            \
		}                                                                                                                        \
	};

#define MAKE_HANDLER(name, T, category, invoke) MAKE_HANDLER_IMPL(name, EventHandler<T>, category, invoke, (void*, T arg))
#define MAKE_TYPED_HANDLER(name, S, R, category, invoke)                                                                         \
	MAKE_HANDLER_IMPL(name, TypedEventHandler<S COMMA R>, category, invoke, (S sender, R args))

MAKE_TYPED_HANDLER(SizeChanged, ICoreWindow*, IWindowSizeChangedEventArgs*, Video, {
	Size size = {};
	args->Size(&size);
	callbacks->OnSizeChanged(size.Width, size.Height, callbacks->user);
})
MAKE_TYPED_HANDLER(Closed, ICoreWindow*, void*, Video, callbacks->OnClosed(callbacks->user))
MAKE_HANDLER(Exiting, void*, Video, callbacks->OnClosed(callbacks->user))

#define ADD_HANDLER(window, handler, token) (window)->handler((handler), &(token));

struct App: public InspectableBase, public IFrameworkView, public IFrameworkViewSource
{
  public:
	int (*main)();
	int result;
	ICoreWindow* window;
	ICoreApplicationView* appView;
	ICoreApplicationView2* appView2;
	ICoreDispatcher* dispatcher;

	UwpVideoCallbacks videoCallbacks;

	SizeChangedHandler* SizeChanged;
	winrt_min::EventRegistrationToken sizeChangedToken;
	ClosedHandler* Closed;
	winrt_min::EventRegistrationToken closedToken;

	App()
		: InspectableBase(NAME, TRUST, IIDS, ArraySize<ULONG>(IIDS)), main(nullptr), result(INT32_MIN), window(nullptr),
		  appView(nullptr), dispatcher(nullptr), videoCallbacks{}, SizeChanged(nullptr), sizeChangedToken{0}, Closed(nullptr),
		  closedToken{0}
	{
		SizeChanged = new SizeChangedHandler(&videoCallbacks);
		Closed = new ClosedHandler(&videoCallbacks);
	}
	virtual ~App() = default;

	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override
	{
		if (!ppvObject)
		{
			return E_POINTER;
		}

		// https://groups.google.com/g/microsoft.public.win32.programmer.ole/c/5Q-2AC9yk7Q?pli=1
		if (COMPARE_IID(riid, IID_IFrameworkView) || COMPARE_IID(riid, __uuidof(IUnknown)))
		{
			*ppvObject = reinterpret_cast<void*>(static_cast<IFrameworkView*>(this));
		}
		else if (COMPARE_IID(riid, IID_IFrameworkViewSource))
		{
			*ppvObject = reinterpret_cast<void*>(static_cast<IFrameworkViewSource*>(this));
		}
		else
		{
			*ppvObject = nullptr;
		}

		if (*ppvObject)
		{
			reinterpret_cast<IUnknown*>(*ppvObject)->AddRef();
			return S_OK;
		}
		else
		{
			return E_NOINTERFACE;
		}
	}

	virtual ULONG STDMETHODCALLTYPE AddRef(void) override
	{
		return InspectableBase::AddRef();
	}

	virtual ULONG STDMETHODCALLTYPE Release(void) override
	{
		return InspectableBase::Release();
	}

	virtual HRESULT STDMETHODCALLTYPE GetIids(ULONG* iidCount, IID** iids) override
	{
		return InspectableBase::GetIids(iidCount, iids);
	}

	virtual HRESULT STDMETHODCALLTYPE GetRuntimeClassName(HSTRING* className) override
	{
		return InspectableBase::GetRuntimeClassName(className);
	}

	virtual HRESULT STDMETHODCALLTYPE GetTrustLevel(TrustLevel* trustLevel) override
	{
		return InspectableBase::GetTrustLevel(trustLevel);
	}

	virtual HRESULT STDMETHODCALLTYPE Initialize(ICoreApplicationView* appViewPtr) override
	{
		Plat_GetTlsData()->isMainThread = true;

		appView = appViewPtr;
		appView->AddRef();
		appView->QueryInterface(&appView2);
		appView2->Dispatcher(&dispatcher);

		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE Load(HSTRING entryPoint) override
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE Uninitialize() override
	{
		window->Release();
		SizeChanged->Release();
		Closed->Release();
		dispatcher->Release();
		appView2->Release();
		appView->Release();
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE Run() override
	{
		appView->CoreWindow(reinterpret_cast<void**>(&window));
		window->Activate();

		result = main();

		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE SetWindow(ICoreWindow* window) override
	{
		ADD_HANDLER(window, SizeChanged, sizeChangedToken);
		ADD_HANDLER(window, Closed, closedToken);
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE CreateView(IFrameworkView** viewProvider) override
	{
		*viewProvider = this;
		return S_OK;
	}

  private:
	static constexpr wchar_t NAME[] = L"Windows.ApplicationModel.Core.IFrameworkView";
	static constexpr TrustLevel TRUST = FullTrust;
	static const IID IIDS[4];
};

const IID App::IIDS[] = {IID_IFrameworkView, IID_IFrameworkViewSource, __uuidof(IInspectable), __uuidof(IUnknown)};

static App* s_app;

int Base_RunMainWinRt(int (*main)())
{
	s_app = new App();
	s_app->main = main;

	// this is the "main" thread, but really the one the app runs on is the main one
	Plat_GetTlsData()->isMainThread = false;
	CoreApplication->Run(s_app);

	s32 result = s_app->result;
	// could probably use delete, but might as well do it right so the runtime doesn't explode
	reinterpret_cast<IUnknown*>(s_app)->Release();
	return result;
}

void Base_ShutdownWinRt()
{
}

cstr Base_GetWinRtAppData()
{
	static char s_path[MAX_PATH + 1];

	if (!Base_StrLength(s_path))
	{
		HSTRING path = nullptr;
		HRESULT result = AppDataPaths->LocalAppData(&path);
		if (FAILED(result))
		{
			Base_Quit("Failed to get AppData folder: HRESULT 0x%08X", result);
		}

		UNICODE_STRING pathWStr = {};
		u32 length = 0;
		// NUL terminated, but length doesn't include that
		pathWStr.Buffer = (dwstr)WindowsGetStringRawBuffer(path, &length);
		pathWStr.Length = static_cast<u16>(length * SIZEOF(wchar_t));
		pathWStr.MaximumLength = static_cast<u16>((length + 1) * SIZEOF(wchar_t));

		ANSI_STRING pathStr = {};
		pathStr.Buffer = s_path;
		pathStr.MaximumLength = SIZEOF(s_path);
		RtlUnicodeStringToAnsiString(&pathStr, &pathWStr, FALSE);
	}

	return s_path;
}

BASEAPI void Plat_BindUwpVideo(UwpVideoInfo& info, const UwpVideoCallbacks& callbacks)
{
	info.window = s_app->window;
	info.dispatcher = s_app->dispatcher;

	s_app->videoCallbacks = callbacks;
}

#endif
