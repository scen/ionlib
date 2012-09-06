#pragma once

#include "../../required.h"
#include "../../util/log.h"
#include "../../mem/vmt.h"
#include <d3d11.h>
#include <D3DX11.h>

namespace ion
{
	class dx11hook
	{
	public:
		typedef HRESULT (__stdcall* Present_t) (IDXGISwapChain *pThis, UINT SyncInterval, UINT Flags);
		const static DWORD VOFFSET_PRESENT = 8;

		dx11hook() {}
		dx11hook(ID3D11Device* pDevice, IDXGISwapChain* pSwapChain) : m_device(pDevice), m_swapChain(pSwapChain)
		{
			gInstance = this;
		}

		//Primary draw hook
		bool hookPresent(std::function<HRESULT(ion::dx11hook*, IDXGISwapChain*, UINT, UINT, Present_t)> lambda);
		
		ID3D11Device* m_device;
		IDXGISwapChain* m_swapChain;
		ion::vmt* swapChainHook;
		std::function<HRESULT(ion::dx11hook*, IDXGISwapChain*, UINT, UINT, Present_t)> callback;
	public:
		static HRESULT __stdcall hkPresent(IDXGISwapChain* pThis, UINT SyncInterval, UINT flags);
		static dx11hook* gInstance;
	};
}