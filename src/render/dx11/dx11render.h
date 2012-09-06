#pragma once

#include "../../required.h"
#include "../render.h"
#include "dx11font.h"
#include "../../util/log.h"

//dx11 includes
#include <d3d11.h>
#include <D3DX11.h>
#include <xnamath.h>
#include <D3DX10.h>
#include <d3dcompiler.h>
#include "extra/d3dx11effect.h"
#include "extra/d3dxGlobal.h"
#include "extra/FillFx.h"

#pragma comment(lib, "Effects11.lib")
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "D3DX11.lib")

namespace ion
{
	class dx11render : public render
	{
	public:
		struct COLOR_VERTEX
		{
			D3DXVECTOR3 Position;
			D3DXCOLOR    Color;
		};
		dx11render(ID3D11Device* device, IDXGISwapChain* swapChain);


		//text functions
		virtual void renderText(int flags, const font *fnt, const point& p, const color& col, const boost::format& fmt);
		virtual void renderText(int flags, const font *fnt, const point& p, const color& col, const std::string &fmt);

		virtual size measureText(int flags, const font *fnt, const point& p, const boost::format& fmt);
		virtual size measureText(int flags, const font *fnt, const point& p, const std::string& fmt);

		virtual font* createFont(const std::string& name, int size, int flags);

		//rect
		virtual void fillRect(const rect& r, const color& c);
		virtual void outlineRect(const rect& r, const color& c);
		virtual void fillGradient(const rect& r, const color& top, const color& bottom);

		//line
		virtual void renderLine(const point& begin, const point& end, const color& col);

		virtual ~dx11render() {}

		HRESULT compileShaderFromMem(LPCSTR pFileBuf, SIZE_T FileSize, LPCSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut );

		ID3D11Device* m_device;
		ID3D11Device* getDevice() const { return m_device; }
		void setDevice(ID3D11Device* val) { m_device = val; }
		IDXGISwapChain* m_swapChain;
		IDXGISwapChain* getSwapChain() const { return m_swapChain; }
		void setSwapChain(IDXGISwapChain* val) { m_swapChain = val; }
		ID3D11DeviceContext* m_deviceContext;
		ID3D11DeviceContext* getDeviceContext() const { return m_deviceContext; }
		void setDeviceContext(ID3D11DeviceContext* val) { m_deviceContext = val; }
		IFW1Factory* m_fontFactory;
		ID3DX11EffectTechnique * m_pTechnique;
		ID3DX11Effect * m_pEffect;
		ID3D11InputLayout * m_pInputLayout;
		ID3D11Buffer * m_pVertexBuffer;

	private:
		void drawRect_(D3DXCOLOR &colortop, D3DXCOLOR &colorbottom, float x0, float y0, float x1, float y1 );
	};
}