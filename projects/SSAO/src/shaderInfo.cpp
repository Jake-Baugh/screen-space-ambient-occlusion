#include "shaderInfo.h"

ShaderInfo::ShaderInfo()
{}

ShaderInfo::~ShaderInfo()
{
	//Free cbuffers
}

void ShaderInfo::ReflectShader(ID3D10Blob* shader)
{
	D3D11_SHADER_DESC sd;

	D3DReflect(shader->GetBufferPointer(), shader->GetBufferSize(), 
		IID_ID3D11ShaderReflection, (void**)&m_reflector);

	m_reflector->GetDesc(&sd);
	shaderDesc = sd;

	for(UINT i = 0; i < sd.InputParameters; i++) 
	{
		//Mask gives info on how many components
		D3D11_SIGNATURE_PARAMETER_DESC inputparam_desc;
		m_reflector->GetInputParameterDesc(i, &inputparam_desc);
		m_inputParams.push_back(inputparam_desc);

	}
	for(UINT i = 0; i < sd.OutputParameters; i++)
	{
		D3D11_SIGNATURE_PARAMETER_DESC outputparam_desc;
		m_reflector->GetOutputParameterDesc(i, &outputparam_desc);
		m_outputParams.push_back(outputparam_desc);
	}
	for(UINT i = 0; i < sd.BoundResources; i++)
	{
		D3D11_SHADER_INPUT_BIND_DESC resource_desc; //Bind point is the register location
		m_reflector->GetResourceBindingDesc(i, &resource_desc);
		
		if(resource_desc.Type == D3D_SIT_CBUFFER)
		{
			Cbuffer* cbuffer = new Cbuffer();
			cbuffer->input_bind_desc = resource_desc;

			ID3D11ShaderReflectionConstantBuffer* cb =
				m_reflector->GetConstantBufferByName(cbuffer->input_bind_desc.Name);

			cb->GetDesc(&cbuffer->buffer_desc);

			for(UINT j = 0; j < cbuffer->buffer_desc.Variables; j++)
			{
				Variable* var = new Variable();

				ID3D11ShaderReflectionVariable* variable =
					cb->GetVariableByIndex(j);

				variable->GetDesc(&var->var);

				ID3D11ShaderReflectionType* type = 
					variable->GetType();

				type->GetDesc(&var->type);

				cbuffer->vars.push_back(var);
				
				//register cbuff vars with ShaderVars object
// 				switch(var->type.Class) {
// 				case D3D_SVC_MATRIX_COLUMNS:
// 					if(var->type.Rows == 4 && var->type.Columns == 4) {
// 						cbp->AddMatrix(var->var.Name);
// 					}
// 					else if(var->type.Rows == 3 && var->type.Columns == 3) {
// 						
// 					}
// 					break;
// 				case D3D_SVC_VECTOR:
// 					if(var->type.Columns == 4) {
// 						cbp->AddVector(var->var.Name);
// 					}
// 					else if(var->type.Columns == 3) {
// 						
// 					}
// 					else if(var->type.Columns == 2) {
// 						
// 					}
// 					break;
// 				case D3D_SVC_SCALAR:
// 					if(var->type.Type == D3D_SVT_FLOAT) {
// 						cbp->AddScalar(var->var.Name);
// 					}
// 					break;
// 				default:
// 					break;
// 				}

				

			}	
			m_cbuffers.push_back(cbuffer);
		}
		else if(resource_desc.Type == D3D_SIT_SAMPLER)
		{
			m_samplers.push_back(resource_desc);
		}
		else if(resource_desc.Type == D3D_SIT_TEXTURE)
		{
			m_textures.push_back(resource_desc);
		}
		else
		{
			m_resources.push_back(resource_desc);
		}
	}
}

/** 
	If different input slot classes are needed, the caller of this function is responsible
	for that.
**/ 
void ShaderInfo::CreateInputLayout(D3D11_INPUT_ELEMENT_DESC* layout, UINT elementCount, 
	bool perVertex /*= true */, UINT instanceDataStepRate /* = 0 */)
{
	for(UINT i = 0; i < elementCount; i++)
	{
		layout[i].SemanticName		= m_inputParams[i].SemanticName;
		layout[i].SemanticIndex		= m_inputParams[i].SemanticIndex;
		layout[i].Format			= GetDXGIFormat(m_inputParams[i]);
		layout[i].InputSlot			= 0;
		layout[i].InstanceDataStepRate = instanceDataStepRate;
		if(perVertex)
			layout[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		else
			layout[i].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;;
		if(i == 0)
			layout[i].AlignedByteOffset = 0;
		else
			layout[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	}
}

int ShaderInfo::GetResourceBindPoint(const std::string* name)
{
	TextureList::iterator it;
	for(it = m_textures.begin(); it != m_textures.end(); it++)
	{
		if(name->compare(it->Name) == 0)
			return (int)it->BindPoint;
	}

	for(it = m_samplers.begin(); it != m_samplers.end(); it++)
	{
		if(name->compare(it->Name) == 0)
			return (int)it->BindPoint;
	}

	CBufferList::iterator buff_it;
	for(buff_it = m_cbuffers.begin(); buff_it != m_cbuffers.end(); buff_it++)
	{
		if(name->compare((*buff_it)->input_bind_desc.Name) == 0)
			return (int)(*buff_it)->input_bind_desc.BindPoint;
	}

	return -1;
}

DXGI_FORMAT ShaderInfo::GetDXGIFormat(D3D11_SIGNATURE_PARAMETER_DESC pd)
{
	BYTE mask = pd.Mask;
	int varCount = 0;
	while(mask)
	{
		if(mask & 0x01) varCount++;
		mask = mask >> 1;
	}

	if(pd.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
	{
		if(varCount == 4) return DXGI_FORMAT_R32G32B32A32_FLOAT;
		else if(varCount == 3) return DXGI_FORMAT_R32G32B32_FLOAT;
		else if(varCount == 2) return DXGI_FORMAT_R32G32_FLOAT;
		else if(varCount == 1) return DXGI_FORMAT_R32_FLOAT;	
	}
	else if(pd.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
	{
		if(varCount == 4) return DXGI_FORMAT_R32G32B32A32_SINT;
		else if(varCount == 3) return DXGI_FORMAT_R32G32B32_SINT;
		else if(varCount == 2) return DXGI_FORMAT_R32G32_SINT;
		else if(varCount == 1) return DXGI_FORMAT_R32_SINT;
	}
	else if(pd.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
	{
		if(varCount == 4) return DXGI_FORMAT_R32G32B32A32_UINT;
		else if(varCount == 3) return DXGI_FORMAT_R32G32B32_UINT;
		else if(varCount == 2) return DXGI_FORMAT_R32G32_UINT;
		else if(varCount == 1) return DXGI_FORMAT_R32_UINT;
	}	
	else if(pd.ComponentType == D3D_REGISTER_COMPONENT_UNKNOWN)
	{}
	else
	{}

	return DXGI_FORMAT_UNKNOWN;
}