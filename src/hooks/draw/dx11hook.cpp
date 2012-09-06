#include "dx11hook.h"

namespace ion
{
	dx11hook* dx11hook::gInstance;

	bool dx11hook::hookPresent( std::function<HRESULT(ion::dx11hook*, IDXGISwapChain*, UINT, UINT, Present_t)> lambda )
	{
		callback = lambda;
		swapChainHook = new ion::vmt(m_swapChain);
		swapChainHook->hookMethod(&hkPresent, VOFFSET_PRESENT);
		return true;
	}

	HRESULT __stdcall dx11hook::hkPresent(IDXGISwapChain* pThis, UINT SyncInterval, UINT flags)
	{
		return gInstance->callback(gInstance, pThis, SyncInterval, flags, gInstance->swapChainHook->getMethod<Present_t>(VOFFSET_PRESENT));
	}
}


