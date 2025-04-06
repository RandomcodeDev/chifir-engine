#include "base/base.h"

#include "platform_win32.h"

#ifndef CH_XBOX360
#include "base/basicstr.h"

template <typename T>
static HRESULT GetClass(PCWSTR name, T** instance)
{
	HSTRING string = nullptr;
	ssize length = Base_StrLength(name);
	ASSERT(SUCCEEDED(WindowsCreateString(name, static_cast<u32>(length), &string)));

	IActivationFactory* factory = nullptr;
	HRESULT result = RoGetActivationFactory(string, IID_PPV_ARGS(&factory));
	if (!SUCCEEDED(result))
	{
		DbgPrint("Failed to get activation factory for class %ls: HRESULT 0x%08X\n", name, result);
		goto Done;
	}

	result = factory->QueryInterface(instance);
	if (!SUCCEEDED(result))
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

static winrt_min::ICoreApplication* CoreApplication;

DECLARE_AVAILABLE(RoInitialize);
DECLARE_AVAILABLE(RoGetActivationFactory);
DECLARE_AVAILABLE(IsImmersiveProcess);

bool Base_InitWinRt()
{
	if (!RoInitialize_Available() || !RoGetActivationFactory_Available() || !IsImmersiveProcess_Available())
	{
		return false;
	}

	if (!IsImmersiveProcess(NtCurrentProcess()))
	{
		DbgPrint("Not attempting Windows Runtime initialization, not a UWP process\n");
		return false;
	}

	HRESULT result = RoInitialize(RO_INIT_MULTITHREADED);
	if (!SUCCEEDED(result))
	{
		DbgPrint("Failed to initialize Windows Runtime: 0x%08X\n", result);
		return false;
	}

	result = GetClass(winrt_min::RuntimeClass_CoreApplication, &CoreApplication);
	if (!SUCCEEDED(result))
	{
		return false;
	}

	return true;
}

#define COMPARE_IID(a, b) (Base_MemCompare(&(a), &(b), sizeof(IID)) == 0)

// TODO: make refcounting thread safe

struct UnknownBase: public IUnknown
{
  public:
	UnknownBase() : m_refCount(1)
	{
	}
	virtual ~UnknownBase() = default;

	virtual ULONG __stdcall AddRef(void) override
	{
		m_refCount++;
		return m_refCount;
	}

	virtual ULONG __stdcall Release(void) override
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
	ULONG m_refCount;
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

    virtual ULONG __stdcall AddRef(void) override
    {
        return UnknownBase::AddRef();
    }

    virtual ULONG __stdcall Release(void) override
    {
        return UnknownBase::Release();
    }

	virtual HRESULT __stdcall GetIids(ULONG* iidCount, IID** iids) override
	{
		if (!iidCount || !iids)
		{
			return E_POINTER;
		}

		*iidCount = m_iidCount;
		*iids = reinterpret_cast<IID*>(CoTaskMemAlloc(*iidCount * sizeof(IID)));
		if (!iids)
		{
			return E_OUTOFMEMORY;
		}

		Base_MemCopy(*iids, m_iids, *iidCount * sizeof(IID));
		return S_OK;
	}

	virtual HRESULT __stdcall GetRuntimeClassName(HSTRING* className) override
	{
		if (!className)
		{
			return E_POINTER;
		}

		return WindowsCreateString(m_className, m_classNameLength, className);
	}

	virtual HRESULT __stdcall GetTrustLevel(TrustLevel* trustLevel) override
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

struct App: public InspectableBase, public winrt_min::IFrameworkView, public winrt_min::IFrameworkViewSource
{
  public:
	int (*main)();
	int result;
	winrt_min::ICoreWindow* window;

	App() : InspectableBase(NAME, TRUST, IIDS, ArraySize<ULONG>(IIDS))
	{
	}
	virtual ~App() = default;
	virtual HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObject) override
    {
		if (!ppvObject)
		{
			return E_POINTER;
		}

		// https://groups.google.com/g/microsoft.public.win32.programmer.ole/c/5Q-2AC9yk7Q?pli=1
		if (COMPARE_IID(riid, winrt_min::IID_IFrameworkView) || COMPARE_IID(riid, __uuidof(IUnknown)))
		{
			*ppvObject = reinterpret_cast<void*>(static_cast<winrt_min::IFrameworkView*>(this));
		}
		else if (COMPARE_IID(riid, winrt_min::IID_IFrameworkViewSource))
		{
			*ppvObject = reinterpret_cast<void*>(static_cast<winrt_min::IFrameworkViewSource*>(this));
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

    virtual ULONG __stdcall AddRef(void) override
    {
        return InspectableBase::AddRef();
    }

    virtual ULONG __stdcall Release(void) override
    {
        return InspectableBase::Release();
    }

	virtual HRESULT __stdcall GetIids(ULONG* iidCount, IID** iids) override
	{
		return InspectableBase::GetIids(iidCount, iids);
	}

	virtual HRESULT __stdcall GetRuntimeClassName(HSTRING* className) override
	{
		return InspectableBase::GetRuntimeClassName(className);
	}

	virtual HRESULT __stdcall GetTrustLevel(TrustLevel* trustLevel) override
	{
		return InspectableBase::GetTrustLevel(trustLevel);
	}

	virtual HRESULT __stdcall Initialize(winrt_min::ICoreApplicationView* appView) override
	{
		return S_OK;
	}

	virtual HRESULT __stdcall Load(HSTRING entryPoint) override
	{
		return S_OK;
	}

	virtual HRESULT __stdcall Uninitialize() override
	{
		return S_OK;
	}

	virtual HRESULT __stdcall Run() override
	{
		winrt_min::ICoreApplicationView* view = nullptr;
		CoreApplication->GetCurrentView(&view);
		view->CoreWindow(reinterpret_cast<void**>(&window));
		window->Activate();

		result = main();

		return S_OK;
	}

	virtual HRESULT __stdcall SetWindow(winrt_min::ICoreWindow* window) override
	{
		this->window = window;
		return S_OK;
	}

	virtual HRESULT __stdcall CreateView(winrt_min::IFrameworkView** viewProvider) override
	{
		*viewProvider = this;
		return S_OK;
	}

  private:
	static constexpr wchar_t NAME[] = L"Windows.ApplicationModel.Core.IFrameworkView";
	static constexpr TrustLevel TRUST = FullTrust;
	static const IID IIDS[4];
};

const IID App::IIDS[] = {
	winrt_min::IID_IFrameworkView, winrt_min::IID_IFrameworkViewSource, __uuidof(IInspectable), __uuidof(IUnknown)};

static App* s_app;
static UwpVideoCallbacks s_videoCallbacks;

int Base_RunMainWinRt(int (*main)())
{
	s_app = new App();
	s_app->main = main;

	CoreApplication->Run(s_app);

	s32 result = s_app->result;
	// could probably use delete, but might as well do it right so the runtime doesn't explode
	reinterpret_cast<IUnknown*>(s_app)->Release();
	return result;
}

void Base_ShutdownWinRt()
{
}

BASEAPI void Plat_BindUwpVideo(winrt_min::ICoreWindow*& window, const UwpVideoCallbacks& callbacks)
{
	window = s_app->window;
	s_videoCallbacks = callbacks;
}

#endif
