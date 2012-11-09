#include "shaderProgram.h"

ShaderProgram::ShaderProgram(ID3D11Device* device, ID3D11DeviceContext* dc)
{
	m_device		= device;
	m_deviceContext = dc;
}

ShaderProgram::~ShaderProgram()
{
	if(m_device)		m_device->Release();
	if(m_deviceContext) m_deviceContext->Release();
	if(m_inputLayout) m_inputLayout->Release();
	if(m_pixelShader) m_pixelShader->Release();
	if(m_vertexShader) m_vertexShader->Release();
	if(m_psInfo) delete [] m_psInfo;
	if(m_vsInfo) delete [] m_vsInfo;
}

void ShaderProgram::CompileShaders(std::wstring vs_fpath, std::wstring ps_fpath)
{
	HRESULT hret;
	ID3D10Blob* vsBuffer, *psBuffer, *errorMessage;

	//Compile shaders
	hret = D3DX11CompileFromFile(vs_fpath.c_str(), 0, 0, "main", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
		0, 0, &vsBuffer, &errorMessage, 0);
	if(FAILED(hret))
	{
		if(errorMessage) { HandleCompileError(errorMessage); return; }
		else
		{}
	}
	hret = D3DX11CompileFromFile(ps_fpath.c_str(), 0, 0, "main", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
		0, 0, &psBuffer, &errorMessage, 0);
	if(FAILED(hret))
	{
		if(errorMessage) { HandleCompileError(errorMessage); return; }
		else
		{}
	}

	//Create interface
	hret = m_device->CreateVertexShader(vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(),
		0, &m_vertexShader);
	if(FAILED(hret))
	{}

	hret = m_device->CreatePixelShader(psBuffer->GetBufferPointer(), psBuffer->GetBufferSize(),
		0, &m_pixelShader);
	if(FAILED(hret))
	{}

	//Reflect
	m_vsInfo = new ShaderInfo();
	m_psInfo = new ShaderInfo();

	m_vsInfo->ReflectShader(vsBuffer);
	m_psInfo->ReflectShader(psBuffer);

	//Create input layout
	UINT elementCount = m_vsInfo->shaderDesc.InputParameters;
	D3D11_INPUT_ELEMENT_DESC* layout = new D3D11_INPUT_ELEMENT_DESC[elementCount];
	m_vsInfo->CreateInputLayout(layout, elementCount);

	hret = m_device->CreateInputLayout(layout, elementCount, vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), &m_inputLayout);
	if(FAILED(hret))
	{}


	//Clean up
	vsBuffer->Release(); vsBuffer = 0;
	psBuffer->Release(); psBuffer = 0;
	if(errorMessage) errorMessage->Release();
	errorMessage = 0;
}

void ShaderProgram::SetActive()
{
	m_deviceContext->IASetInputLayout(m_inputLayout);
	m_deviceContext->PSSetShader(m_pixelShader, 0, 0);
	m_deviceContext->VSSetShader(m_vertexShader, 0, 0);
}

void ShaderProgram::HandleCompileError(ID3D10Blob* errorMessage)
{
	std::string msg = (char*)errorMessage->GetBufferPointer();
	exit(1);
	//MessageBox(0, msg.c_str(), L"ShaderError", MB_OK);
}