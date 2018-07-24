#include "stdafx.h"
#include "Material.h"
#include "../Utilities/String.h"

Material::Material()
	: shader(NULL)
	, diffuseMap(NULL), specularMap(NULL), emissiveMap(NULL), normalMap(NULL), detailMap(NULL)
{
	buffer = new MaterialBuffer();

	bShaderDelete = false;
}

Material::Material(wstring shaderFile)
{
	assert(shaderFile.length() > 0);

	buffer = new MaterialBuffer();

	bShaderDelete = true;
	shader = new Shader(shaderFile);
}

Material::~Material()
{
	if (bShaderDelete == true)
		SAFE_DELETE(shader);

	SAFE_DELETE(diffuseMap);
	SAFE_DELETE(specularMap);
	SAFE_DELETE(emissiveMap);
	SAFE_DELETE(normalMap);
	SAFE_DELETE(detailMap);

	SAFE_DELETE(buffer);
}

void Material::Clone(void ** clone)
{
	Material* material = new Material();

	material->SetNumber(this->number);
	material->SetName(this->name);
	material->SetShader(this->shader->GetFile());

	material->SetAmbient(*this->GetAmbient());
	material->SetDiffuse(*this->GetDiffuse());
	material->SetSpecular(*this->GetSpecular());
	material->SetEmissive(*this->GetEmissive());

	material->SetShininess(*this->GetShininess());
	material->SetDetailFactor(*this->GetDetailFactor());


	if (this->diffuseMap != NULL)
		material->SetDiffuseMap(this->diffuseMap->GetFile());

	if (this->specularMap != NULL)
		material->SetSpecularMap(this->specularMap->GetFile());

	if (this->emissiveMap != NULL)
		material->SetEmissiveMap(this->emissiveMap->GetFile());

	if (this->normalMap != NULL)
		material->SetNormalMap(this->normalMap->GetFile());

	if (this->detailMap != NULL)
		material->SetDetailMap(this->detailMap->GetFile());

	*clone = material;
}

void Material::SetShader(string file)
{
	SetShader(String::ToWString(file));
}

void Material::SetShader(wstring file)
{
	if (bShaderDelete == true)
		SAFE_DELETE(shader);

	bShaderDelete = false;
	if (file.length() > 0)
	{
		bShaderDelete = true;
		shader = new Shader(file);
	}
}

void Material::SetShader(Shader * shader)
{
	if (bShaderDelete == true)
		SAFE_DELETE(this->shader);

	bShaderDelete = false;
	this->shader = shader;
}

void Material::SetDiffuseMap(string file, D3DX11_IMAGE_LOAD_INFO * info)
{
	SetDiffuseMap(String::ToWString(file));
}

void Material::SetDiffuseMap(wstring file, D3DX11_IMAGE_LOAD_INFO * info)
{
	if(diffuseMap != nullptr)
		SAFE_DELETE(diffuseMap);

	diffuseMap = new Texture(file, info);
}

void Material::SetSpecularMap(string file, D3DX11_IMAGE_LOAD_INFO * info)
{
	SetSpecularMap(String::ToWString(file));
}

void Material::SetSpecularMap(wstring file, D3DX11_IMAGE_LOAD_INFO * info)
{
	SAFE_DELETE(specularMap);

	specularMap = new Texture(file, info);
}

void Material::SetEmissiveMap(string file, D3DX11_IMAGE_LOAD_INFO * info)
{
	SetEmissiveMap(String::ToWString(file));
}

void Material::SetEmissiveMap(wstring file, D3DX11_IMAGE_LOAD_INFO * info)
{
	SAFE_DELETE(emissiveMap);

	emissiveMap = new Texture(file, info);
}

void Material::SetNormalMap(string file, D3DX11_IMAGE_LOAD_INFO * info)
{
	SetNormalMap(String::ToWString(file));
}

void Material::SetNormalMap(wstring file, D3DX11_IMAGE_LOAD_INFO * info)
{
	SAFE_DELETE(normalMap);

	normalMap = new Texture(file, info);
}

void Material::SetDetailMap(string file, D3DX11_IMAGE_LOAD_INFO * info)
{
	SetDetailMap(String::ToWString(file));
}

void Material::SetDetailMap(wstring file, D3DX11_IMAGE_LOAD_INFO * info)
{
	SAFE_DELETE(detailMap);

	detailMap = new Texture(file, info);
}

void Material::SetBuffer()
{
	if (shader != NULL)
		shader->Render();

	ID3D11ShaderResourceView* null[1]{ NULL };

	UINT slot = 0;
	if (diffuseMap != NULL)
	{
		diffuseMap->SetShaderResource(slot);
		diffuseMap->SetShaderSampler(slot);
	}
	else
	{
		Texture::SetBlankShaderResource(slot);
		Texture::SetBlankSamplerState(slot);
	}


	slot = 1;
	if (specularMap != NULL)
	{
		specularMap->SetShaderResource(slot);
		specularMap->SetShaderSampler(slot);
	}
	else
	{
		Texture::SetBlankShaderResource(slot);
		Texture::SetBlankSamplerState(slot);
	}


	slot = 2;
	if (emissiveMap != NULL)
	{
		emissiveMap->SetShaderResource(slot);
		emissiveMap->SetShaderSampler(slot);
	}
	else
	{
		Texture::SetBlankShaderResource(slot);
		Texture::SetBlankSamplerState(slot);
	}


	slot = 3;
	if (normalMap != NULL)
	{
		normalMap->SetShaderResource(slot);
		normalMap->SetShaderSampler(slot);
	}
	else
	{
		Texture::SetBlankShaderResource(slot);
		Texture::SetBlankSamplerState(slot);
	}


	slot = 4;
	if (detailMap != NULL)
	{
		detailMap->SetShaderResource(slot);
		detailMap->SetShaderSampler(slot);
	}
	else
	{
		Texture::SetBlankShaderResource(slot);
		Texture::SetBlankSamplerState(slot);
	}

	buffer->SetPSBuffer(1);
}