/*
* Copyright (c) 2012 Stanley Cen
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
* LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
* OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
* WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef _FILL_FX_H_
#define _FILL_FX_H_

//C:/tmp/meh/convertshadertoarray

char const fillfx[] =
"BlendState FillTechBlend\n"
"{\n"
"    AlphaToCoverageEnable = FALSE;\n"
"    BlendEnable[0] = TRUE;\n"
"    SrcBlend = SRC_ALPHA;\n"
"    DestBlend = INV_SRC_ALPHA;\n"
"    BlendOp = ADD;\n"
"    SrcBlendAlpha = ONE;\n"
"    DestBlendAlpha = ZERO;\n"
"    BlendOpAlpha = ADD;\n"
"    RenderTargetWriteMask[0] = 0x0F;\n"
"};\n"
"\n"
"DepthStencilState FillTechDepth\n"
"{\n"
"    DepthEnable = false;\n"
"};\n"
"\n"
"struct VSOut\n"
"{\n"
"    float4 Col : COLOR;\n"
"    float4 Pos : SV_POSITION;\n"
"};\n"
"\n"
"VSOut VS(float4 Col : COLOR, float4 Pos : POSITION)\n"
"{\n"
"    VSOut Output;\n"
"    Output.Pos = Pos;\n"
"    Output.Col = Col;\n"
"    return Output;\n"
"}\n"
"\n"
"float4 PS(float4 Col : COLOR) : SV_TARGET\n"
"{\n"
"    return Col;\n"
"}\n"
"\n"
"technique11 FillTech\n"
"{\n"
"    pass Pass_0\n"
"    {\n"
"        SetVertexShader(CompileShader(vs_4_0, VS()));\n"
"        SetGeometryShader(NULL);\n"
"        SetPixelShader(CompileShader(ps_4_0, PS()));\n"
"        SetDepthStencilState(FillTechDepth, 0);\n"
"        SetBlendState(FillTechBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);\n"
"    }\n"
"}\n";

#endif