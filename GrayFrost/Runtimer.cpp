#pragma once
#include "Runtimer.h"

#include "slate.h"

class CSafeArrayByteVector
{
private:
	SAFEARRAY* m_ar;
	bool m_locked;

public:
	CSafeArrayByteVector()
	{
		m_ar = NULL;
		m_locked = false;
	}
	~CSafeArrayByteVector()
	{
		Clear();
	}

	BYTE* GetDataPtr()
	{
		if (!m_locked)
		{
			return NULL;
		}
		return (BYTE*)m_ar->pvData;
	}

	SAFEARRAY* GetSafeArray()
	{
		return m_ar;
	}

	BOOL Allocate(ULONG size);
	void Clear();
};
 
BOOL CSafeArrayByteVector::Allocate(ULONG size)
{
	Clear();
	m_ar = SafeArrayCreateVector(VT_UI1, 0, size);
	if (!m_ar)
	{
		return FALSE;
	}
	HRESULT hr;
	hr = SafeArrayLock(m_ar);
	if (FAILED(hr))
	{
		Clear();
		return FALSE;
	}
	m_locked = true;
	return TRUE;
}

void CSafeArrayByteVector::Clear()
{
	if (m_ar)
	{
		if (m_locked)
		{
			SafeArrayUnlock(m_ar);
			m_locked = false;
		}
		SafeArrayDestroy(m_ar);
		m_ar = NULL;
	}
}

//Start in .NET CLR 4.0 
bool Bootstrap()
{
	ICLRRuntimeInfo *pRuntimeInfo = NULL;
	ICorRuntimeHost *pCorRuntimeHost = NULL;

	bool win = false;
	HRESULT hr;
	CSafeArrayByteVector RawAssembly;

	IUnknownPtr spAppDomainThunk = NULL;
	_AppDomainPtr spDefaultAppDomain = NULL;

	// Load and start the .NET runtime.
	ICLRMetaHost *pMetaHost = NULL;
	hr = CLRCreateInstance(CLSID_CLRMetaHost, IID_PPV_ARGS(&pMetaHost));
	if (FAILED(hr))
	{
		LPCTSTR	v0 = L"CLRCreateInstance failed";
		MessageBox(0, v0, v0, 0);
		goto DONE;
	}

	// Get the ICLRRuntimeInfo corresponding to a particular CLR version. It 
	// supersedes CorBindToRuntimeEx with STARTUP_LOADER_SAFEMODE.
	//->ICLRRuntimeInfo *pRuntimeInfo = NULL;
	hr = pMetaHost->GetRuntime(CLR_V40, IID_PPV_ARGS(&pRuntimeInfo));

	// Check if the specified runtime can be loaded into the process. This 
	// method will take into account other runtimes that may already be 
	// loaded into the process and set pbLoadable to TRUE if this runtime can 
	// be loaded in an in-process side-by-side fashion. 
	BOOL fLoadable;
	hr = pRuntimeInfo->IsLoadable(&fLoadable);

	// Load the CLR into the current process and return a runtime interface 
	// pointer. ICorRuntimeHost and ICLRRuntimeHost are the two CLR hosting  
	// interfaces supported by CLR 4.0. Here we demo the ICorRuntimeHost 
	// interface that was provided in .NET v1.x, and is compatible with all 
	// .NET Frameworks. 

	// Allow the runtime to load .NET 2.0 mixed-mode libraries. (This covers 2.0-3.5 SP1)
	hr = pRuntimeInfo->BindAsLegacyV2Runtime();

	hr = pRuntimeInfo->GetInterface(CLSID_CorRuntimeHost, IID_PPV_ARGS(&pCorRuntimeHost));

	// Start the CLR.
	hr = pCorRuntimeHost->Start();
	
	// Load the NET assembly. Call the static method GetStringLength of the 
	// class CSSimpleObject. Instantiate the class CSSimpleObject and call 
	// its instance method ToString.	

	// Get a pointer to the default AppDomain in the CLR.
	hr = pCorRuntimeHost->GetDefaultDomain(&spAppDomainThunk);
	hr = spAppDomainThunk->QueryInterface(IID_PPV_ARGS(&spDefaultAppDomain));

	RawAssembly.Allocate(SIZE);
	memcpy((char *)RawAssembly.GetDataPtr(), data, SIZE);

	HRESULT st;

	// load assembly
	mscorlib::_Assembly *asmbly = NULL;
	st = spDefaultAppDomain->Load_3(RawAssembly.GetSafeArray(), &asmbly);

	//// get entryPoint for assembly (void Main())
	mscorlib::_MethodInfo *EntryPointInfo = NULL;
	st = asmbly->get_EntryPoint(&EntryPointInfo);

	//// get run entry point
	VARIANT RetCode;
	VARIANT v2;
	v2.vt = VT_NULL;
	v2.plVal = NULL;
	st = EntryPointInfo->Invoke_3(v2, NULL, &RetCode);

	win = true;

DONE:

	if (pMetaHost != NULL)
	{
		pMetaHost->Release();
		pMetaHost = NULL;
	}

	if (pRuntimeInfo != NULL)
	{
		pRuntimeInfo->Release();
		pRuntimeInfo = NULL;
	}

	if (pCorRuntimeHost)
	{
		// Please note that after a call to Stop, the CLR cannot be 
		// reinitialized into the same process. This step is usually not 
		// necessary. You can leave the .NET runtime loaded in your process.
		//wprintf(L"Stop the .NET runtime\n");
		//pCorRuntimeHost->Stop();

		pCorRuntimeHost->Release();
		pCorRuntimeHost = NULL;
	}

	return win;
}


//get into .NET CLR 2.0 
EXTERN_DLL_EXPORT int getV2Runtime()
{	
	CoInitializeEx(0, COINIT_MULTITHREADED);

	// get runtime in host
	ICorRuntimeHost* pICorRuntimeHost = 0;
	HRESULT st = CoCreateInstance(CLSID_CorRuntimeHost, 0, CLSCTX_ALL, IID_ICorRuntimeHost, (void**)&pICorRuntimeHost);
	if (!pICorRuntimeHost)
		return false;

	// get domains in host
	HDOMAINENUM hEnum = NULL;
	pICorRuntimeHost->EnumDomains(&hEnum);
	if (!hEnum)
		return false;

	// get next domain
	IUnknown * pUunk = 0;
	st = pICorRuntimeHost->NextDomain(hEnum, &pUunk);
	if (!pUunk)
		return false;

	// load domain in to object (pCurDomain)
	mscorlib::_AppDomain* pCurDomain = NULL;
	st = pUunk->QueryInterface(__uuidof(mscorlib::_AppDomain), (VOID**)&pCurDomain);
	if (!pCurDomain)
		return false;

	//get our payload
	CSafeArrayByteVector RawAssembly;
	RawAssembly.Allocate((ULONG)SIZE);
	memcpy((char *)RawAssembly.GetDataPtr(), data, SIZE);

	// load assembly
	mscorlib::_Assembly *asmbly = NULL;
	st = pCurDomain->Load_3(RawAssembly.GetSafeArray(), &asmbly);

	//get entryPoint for assembly (void Main())
	mscorlib::_MethodInfo *EntryPointInfo = NULL;
	st = asmbly->get_EntryPoint(&EntryPointInfo);

	//get runtime entry point
	VARIANT RetCode; VARIANT v2;
	v2.vt = VT_NULL; v2.plVal = NULL;
	st = EntryPointInfo->Invoke_3(v2, NULL, &RetCode);

	return true;
}