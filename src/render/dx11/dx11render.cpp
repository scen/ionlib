#include "dx11render.h"

namespace ion
{

	dx11render::dx11render(ID3D11Device* device, IDXGISwapChain* swapChain) : m_device(device), m_swapChain(swapChain)
	{
		if (m_device == NULL && m_swapChain)
			m_swapChain->GetDevice(__uuidof(m_device), (void**)&m_device);
		HRESULT hResult = FW1CreateFactory(FW1_VERSION, &m_fontFactory);
		if (SUCCEEDED(hResult))
		{
			dbglog("Created font factory\n");
		}
		else
		{
			dbglog("Error creating font factory\n");
			return;
		}

		ID3DBlob* compiledFX = NULL;
		HRESULT hr = compileShaderFromMem(fillfx, sizeof fillfx, "FillFx.fx", "FillTech", "fx_5_0", &compiledFX);

		if (SUCCEEDED(hr))
		{
			dbglog("Compiled fillfx shader\n");
		}

		hr = D3DX11CreateEffectFromMemory( compiledFX->GetBufferPointer(), compiledFX->GetBufferSize(), 0, m_device, &m_pEffect );

		if (SUCCEEDED(hr))
		{
			dbglog("Created effect from shader\n");
		}

		compiledFX->Release();
		m_pTechnique = m_pEffect->GetTechniqueByName( "FillTech" );
		if (m_pTechnique)
		{
			dbglog("Success getting 'FillTech' technique\n");
		}

		D3D11_INPUT_ELEMENT_DESC lineRectLayout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },  
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		D3DX11_PASS_DESC passDesc;

		if( FAILED( m_pTechnique->GetPassByIndex( 0 )->GetDesc( &passDesc ) ) )
		{
			dbglog("Failed to GetPassByIndex\n");
			return;
		}

		if( FAILED( m_device->CreateInputLayout( 
			lineRectLayout, 
			sizeof( lineRectLayout ) / sizeof( lineRectLayout[0] ), 
			passDesc.pIAInputSignature, 
			passDesc.IAInputSignatureSize, 
			&m_pInputLayout ) ) )
		{
			dbglog("Failed to CreateInputLayout\n");
			return;
		}

		D3D11_BUFFER_DESC bufferDesc;

		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = 4 * sizeof( COLOR_VERTEX );
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;

		if( FAILED( m_device->CreateBuffer( &bufferDesc, NULL, &m_pVertexBuffer ) ) )
		{
			dbglog("device->CreateBuffer failed\n");
			return;
		}
		dbglog("Success initializing DX11 draw resources\n")
	}

	HRESULT dx11render::compileShaderFromMem(LPCSTR pFileBuf, SIZE_T FileSize, LPCSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut )
	{
		HRESULT hr = S_OK;

		DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

		ID3DBlob* pErrorBlob;
		hr = D3DX11CompileFromMemory( pFileBuf, FileSize, szFileName, NULL, NULL, szEntryPoint, szShaderModel, 
			dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL );
		if( FAILED(hr) )
		{
			if( pErrorBlob ) pErrorBlob->Release();
			return hr;
		}
		if( pErrorBlob ) pErrorBlob->Release();

		return S_OK;
	}

	font* dx11render::createFont(const std::string& name, int size, int flags)
	{
		dx11font* f = new dx11font();
		f->setName(name);
		f->setSize(size);
		//TODO make wide string utility
		wchar_t* w = new wchar_t[name.length() + 1];
		mbstowcs(w, name.c_str(), name.length());
		HRESULT hResult = m_fontFactory->CreateFontWrapper(m_device, w, &f->m_fontWrapper);
		if (SUCCEEDED(hResult))
		{
			dbglogn("Created font wrapper for " << name);
		}
		else
		{
			dbglog("Error creating font wrapper.\n");
		}
		delete[] w;
		return f;
	}

	void dx11render::renderText(int flags, const font *fnt, const point& p, const color& col, const std::string &fmt)
	{
		auto f = reinterpret_cast<const dx11font*>(fnt);
		int rflags = FW1_RESTORESTATE;
		//convert render flags to lib flags
		if (flags & VCenter)
			rflags |= FW1_VCENTER;
		if (flags & Center)
			rflags |= FW1_CENTER;
		if (flags & LAlign)
			rflags |= FW1_LEFT;
		if (flags & RAlign)
			rflags |= FW1_RIGHT;
		if (flags & TAlign)
			rflags |= FW1_TOP;
		if (flags & BAlign)
			rflags |= FW1_BOTTOM;

		//TODO string convert
		wchar_t buf[1024];
		mbstowcs(buf, fmt.c_str(), fmt.length());
		buf[fmt.length()] = 0;
		if (f->getFontWrapper())
		{
			f->getFontWrapper()->DrawString(getDeviceContext(), buf, f->getSize(), (FLOAT)p.getX(), (FLOAT)p.getY(), col.toABGR(), rflags);
		}
	}
	
	void dx11render::renderText(int flags, const font *fnt, const point& p, const color& col, const boost::format& fmt)
	{
		renderText(flags, fnt, p, col, boost::str(fmt));
	}

	ion::size dx11render::measureText(int flags, const font *fnt, const point& p, const std::string& fmt)
	{
		return size();
	}

	ion::size dx11render::measureText(int flags, const font *fnt, const point& p, const boost::format& fmt)
	{
		return size();
	}

	void dx11render::renderLine(const point& begin, const point& end, const color& col)
	{
		if( m_device == NULL ) return;

		UINT viewportNumber = 1;

		D3D11_VIEWPORT vp;

		m_deviceContext->RSGetViewports( &viewportNumber, &vp );

		float xx0 = 2.0f * ( begin.getX() - 0.5f ) / vp.Width - 1.0f;
		float yy0 = 1.0f - 2.0f * ( begin.getY() - 0.5f ) / vp.Height;
		float xx1 = 2.0f * ( end.getX() - 0.5f ) / vp.Width - 1.0f;
		float yy1 = 1.0f - 2.0f * ( end.getY() - 0.5f ) / vp.Height;

		COLOR_VERTEX* v = NULL;
		D3D11_MAPPED_SUBRESOURCE mapData;
		if( FAILED( m_deviceContext->Map( m_pVertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapData ) ) )
			return;

		v = ( COLOR_VERTEX* ) mapData.pData;

		v[0].Position.x = xx0;
		v[0].Position.y = yy0;
		v[0].Position.z = 0;
		v[0].Color = col.toD3DXCOLOR();

		v[1].Position.x = xx1;
		v[1].Position.y = yy1;
		v[1].Position.z = 0;
		v[1].Color = col.toD3DXCOLOR();

		m_deviceContext->Unmap( m_pVertexBuffer, NULL );
		m_deviceContext->IASetInputLayout( m_pInputLayout );

		UINT Stride = sizeof( COLOR_VERTEX );
		UINT Offset = 0;

		m_deviceContext->IASetVertexBuffers( 0, 1, &m_pVertexBuffer, &Stride, &Offset );
		m_deviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP );

		D3DX11_TECHNIQUE_DESC techDesc;

		if( FAILED( m_pTechnique->GetDesc( &techDesc ) ) )
		{
			return;
		}

		for( UINT p = 0; p < techDesc.Passes; ++p )
		{
			m_pTechnique->GetPassByIndex( p )->Apply( 0, m_deviceContext );
			m_deviceContext->Draw( 2, 0 );
		}
	}

	void dx11render::fillRect(const rect& r, const color& c)
	{
		drawRect_(c.toD3DXCOLOR(), c.toD3DXCOLOR(), r.getX(), r.getY(), r.getX() + r.getW(), r.getY() + r.getH());
	}

	void dx11render::outlineRect(const rect& r, const color& c)
	{
		renderLine(point(r.getX(), r.getY()), point(r.getX() + r.getW() - 1, r.getY()), c); //TopLeft to TopRight
		renderLine(point(r.getX(), r.getY()), point(r.getX(), r.getY() + r.getH() - 1), c); //TopLeft to BottomLeft
		renderLine(point(r.getX() + r.getW() - 1, r.getY()), point(r.getX() + r.getW() - 1, r.getY() + r.getH() - 1), c); //TopRight to BottomRight
		renderLine(point(r.getX(), r.getY() + r.getH() - 1), point(r.getX() + r.getW(), r.getY() + r.getH() - 1), c); //BottomLeft to BottomRight
	}

	void dx11render::fillGradient(const rect& r, const color& top, const color& bottom)
	{
		drawRect_(top.toD3DXCOLOR(), bottom.toD3DXCOLOR(), r.getX(), r.getY(), r.getX() + r.getW(), r.getY() + r.getH());
	}

	void dx11render::drawRect_(D3DXCOLOR &colortop, D3DXCOLOR &colorbottom, float x0, float y0, float x1, float y1 )
	{
		if( m_device == NULL ) return;

		UINT viewportNumber = 1;

		D3D11_VIEWPORT vp;

		m_deviceContext->RSGetViewports( &viewportNumber, &vp );

		float xx0 = 2.0f * ( x0 - 0.5f ) / vp.Width - 1.0f;
		float yy0 = 1.0f - 2.0f * ( y0 - 0.5f ) / vp.Height;
		float xx1 = 2.0f * ( x1 - 0.5f ) / vp.Width - 1.0f;
		float yy1 = 1.0f - 2.0f * ( y1 - 0.5f ) / vp.Height;

		COLOR_VERTEX* v = NULL;

		D3D11_MAPPED_SUBRESOURCE mapData;

		if( FAILED( m_deviceContext->Map( m_pVertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapData ) ) )
		{
			return;
		}

		v = ( COLOR_VERTEX* ) mapData.pData;

		v[0].Position.x = x0;
		v[0].Position.y = y0;
		v[0].Position.z = 0;
		v[0].Color = colorbottom;

		v[1].Position.x = x1;
		v[1].Position.y = y1;
		v[1].Position.z = 0;
		v[1].Color = colorbottom;

		v[0].Position.x = xx0;
		v[0].Position.y = yy0;
		v[0].Position.z = 0;
		v[0].Color = colortop;

		v[1].Position.x = xx1;
		v[1].Position.y = yy0;
		v[1].Position.z = 0;
		v[1].Color = colortop;

		v[2].Position.x = xx0;
		v[2].Position.y = yy1;
		v[2].Position.z = 0;
		v[2].Color = colorbottom;

		v[3].Position.x = xx1;
		v[3].Position.y = yy1;
		v[3].Position.z = 0;
		v[3].Color = colorbottom;

		m_deviceContext->Unmap( m_pVertexBuffer, NULL );

		m_deviceContext->IASetInputLayout( m_pInputLayout );

		UINT Stride = sizeof( COLOR_VERTEX );
		UINT Offset = 0;

		m_deviceContext->IASetVertexBuffers( 0, 1, &m_pVertexBuffer, &Stride, &Offset );

		m_deviceContext->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

		D3DX11_TECHNIQUE_DESC techDesc;

		if( FAILED( m_pTechnique->GetDesc( &techDesc ) ) )
		{
			return;
		}
		for( UINT p = 0; p < techDesc.Passes; ++p )
		{
			m_pTechnique->GetPassByIndex( p )->Apply( 0, m_deviceContext );

			m_deviceContext->Draw( 4, 0 );
		}
	}
}