#ifndef CH_XBOX360
#include "base/basicstr.h"
#include "winrt_min.h"

bool Base_InitWinRt()
{
	HRESULT result = RoInitialize(RO_INIT_MULTITHREADED);
	if (!SUCCEEDED(result))
	{
		DbgPrint("Failed to initialize Windows Runtime: 0x%08X\n", result);
		return false;
	}

	return true;
}

// TODO: make refcounting thread safe

struct UnknownBase: public IUnknown
{
	UnknownBase() : m_refCount(1)
	{
	}
	virtual ~UnknownBase() = default;

	virtual HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObject)
	{
		if (!ppvObject)
		{
			return E_POINTER;
		}

		AddRef();
		*ppvObject = this;
		return S_OK;
	}

	virtual ULONG __stdcall AddRef(void)
	{
		m_refCount++;
		return m_refCount;
	}

	virtual ULONG __stdcall Release(void)
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
	virtual ~InspectableBase() = default;

	virtual HRESULT __stdcall GetIids(ULONG* iidCount, IID** iids)
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

	virtual HRESULT __stdcall GetRuntimeClassName(HSTRING* className)
	{
		if (!className)
		{
			return E_POINTER;
		}

		return WindowsCreateString(m_className, m_classNameLength, className);
	}

	virtual HRESULT __stdcall GetTrustLevel(TrustLevel* trustLevel)
	{
		if (!trustLevel)
		{
			return E_POINTER;
		}

		*trustLevel = m_trustLevel;
		return S_OK;
	}

  protected:
	InspectableBase(PCWSTR className, TrustLevel trustLevel, const IID* iids, ULONG iidCount)
		: UnknownBase(), m_className(className), m_classNameLength(Base_StrLength(m_className)), m_trustLevel(trustLevel), m_iids(iids),
		  m_iidCount(iidCount)
	{
	}

  private:
	PCWSTR m_className;
	ULONG m_classNameLength;
	TrustLevel m_trustLevel;
	const IID* m_iids;
	ULONG m_iidCount;
};

struct App: public winrt_min::IFrameworkView, public winrt_min::IFrameworkViewSource,
			public InspectableBase
{
	int (*main)();
	int result;
	const winrt_min::ICoreWindow* window;

	virtual ~App() = default;

	virtual void __stdcall Initialize(const winrt_min::ICoreApplicationView& appView)
	{
		winrt_min::TypedEventHandler<winrt_min::ICoreApplicationView*, void*> handler;
		appView.Activated(&handler, &token);
	}

	virtual HRESULT __stdcall Uninitialize() override
	{
		return S_OK;
	}

	virtual HRESULT __stdcall Run() override
	{
		result = main();
		return S_OK;
	}

	virtual void __stdcall SetWindow(const winrt_min::ICoreWindow& window) override
	{
		this->window = &window;
	}

	virtual HRESULT __stdcall CreateView(winrt_min::IFrameworkView** viewProvider) override
	{
		*viewProvider = this;
		return S_OK;
	}
};

HRESULT GetClass(PCWSTR name, const IID& iid, void** instance)
{
	HSTRING string = nullptr;
	u32 length = Base_StrLength(name);
	ASSERT(SUCCEEDED(WindowsCreateString(name, length, &string)));

	HRESULT result = RoGetActivationFactory(string, iid, instance);
	if (!SUCCEEDED(result))
	{
		DbgPrint("Failed to get activation factory for class %ls: HRESULT 0x%08X\n", name, result);
	}

	WindowsDeleteString(string);
	return result;
}

int Base_RunMainWinRt(int (*main)())
{
	winrt_min::ICoreApplication* coreApp;
	HRESULT result = GetClass(winrt_min::RuntimeClass_CoreApplication, IID_PPV_ARGS(&coreApp));
	if (!SUCCEEDED(result))
	{
		return result;
	}
	winrt_min::ICoreWindow* coreWindow;
	result = GetClass(winrt_min::RuntimeClass_CoreWindow, IID_PPV_ARGS(&coreWindow));
	if (!SUCCEEDED(result))
	{
		return result;
	}

	App* app = new App();
	app->main = main;
	coreApp->Run(app);

	s32 result = app->result;
	reinterpret_cast<IUnknown*>(app)->Release();
	return result;
}

void Base_ShutdownWinRt()
{
}

#endif
