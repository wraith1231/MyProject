#include "stdafx.h"
#include "GameTerrain.h"

#include "../Utilities/BinaryFile.h"

GameTerrain::GameTerrain()
	: tex1(NULL), tex2(NULL), tex3(NULL), tex4(NULL)
	, shader(NULL), shader2(NULL), shader3(NULL)
	, vertexBuffer(NULL), indexBuffer(NULL)
{
	Init();
	CreateNormal();
	CreateBuffer();
}

GameTerrain::~GameTerrain()
{
	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);

	SAFE_DELETE(shader);
	SAFE_DELETE(shader2);
	SAFE_DELETE(shader3);

	SAFE_DELETE(tex1);
	SAFE_DELETE(tex2);
	SAFE_DELETE(tex3);
	SAFE_DELETE(tex4);

	SAFE_DELETE(material);
	SAFE_DELETE(buffer);
}

void GameTerrain::SaveTerrain(wstring saveFile)
{
	D3DDesc descc;
	D3D::GetDesc(&descc);

	if (saveFile.length() < 1)
	{
		function<void(wstring)> f;
		f = bind(&GameTerrain::SaveTerrain, this, placeholders::_1);

		Path::OpenFileDialog(L"", Path::TerrainFilter, Landscapes, f, descc.Handle);

		return;
	}

	wstring folder = Path::GetDirectoryName(saveFile);
	if (Path::ExistDirectory(folder) == false)
	{
		//Api Function
		CreateDirectory(folder.c_str(), NULL);
	}

	wstring file = saveFile;
	if (Path::GetExtension(file) != L".terrain")
	{
		file += L".terrain";
	}

	BinaryWriter* w = new BinaryWriter();
	w->Open(file);

	bool b = false;
	//material
	{
		b = material->GetDiffuseMap() != NULL;
		w->Bool(b);
		if (b == true)
			w->String(String::ToString(material->GetDiffuseMap()->GetFile()));

		b = material->GetSpecularMap() != NULL;
		w->Bool(b);
		if (b == true)
			w->String(String::ToString(material->GetSpecularMap()->GetFile()));

		b = material->GetEmissiveMap() != NULL;
		w->Bool(b);
		if (b == true)
			w->String(String::ToString(material->GetEmissiveMap()->GetFile()));

		b = material->GetNormalMap() != NULL;
		w->Bool(b);
		if (b == true)
			w->String(String::ToString(material->GetNormalMap()->GetFile()));

		b = material->GetDetailMap() != NULL;
		w->Bool(b);
		if (b == true)
			w->String(String::ToString(material->GetDetailMap()->GetFile()));
	}

	//texture
	{
		b = tex1 != NULL;
		w->Bool(b);
		if (b == true)
			w->String(String::ToString(tex1->GetFile()));

		b = tex2 != NULL;
		w->Bool(b);
		if (b == true)
			w->String(String::ToString(tex2->GetFile()));

		b = tex3 != NULL;
		w->Bool(b);
		if (b == true)
			w->String(String::ToString(tex3->GetFile()));

		b = tex4 != NULL;
		w->Bool(b);
		if (b == true)
			w->String(String::ToString(tex4->GetFile()));
	}

	//vertex & index
	{
		w->UInt(vertexSize);
		w->Byte(vertices, sizeof(VertexType) * vertexSize);
		w->UInt(indexSize);
		w->Byte(indices, sizeof(UINT) * indexSize);
	}

	w->Close();
	SAFE_DELETE(w);
}

void GameTerrain::LoadTerrain(wstring saveFile)
{
	D3DDesc descc;
	D3D::GetDesc(&descc);

	if (saveFile.length() < 1)
	{
		function<void(wstring)> f;
		f = bind(&GameTerrain::LoadTerrain, this, placeholders::_1);

		Path::OpenFileDialog(L"", Path::TerrainFilter, Landscapes, f, descc.Handle);

		return;
	}

	BinaryReader* r = new BinaryReader();

	wstring fileName = Path::GetFileLocalPath(saveFile);
	r->Open(fileName);

	bool b = false;

	//material
	{
		b = r->Bool();
		if (b == true)
			material->SetDiffuseMap(r->String());

		b = r->Bool();
		if (b == true)
			material->SetSpecularMap(r->String());

		b = r->Bool();
		if (b == true)
			material->SetEmissiveMap(r->String());

		b = r->Bool();
		if (b == true)
			material->SetNormalMap(r->String());

		b = r->Bool();
		if (b == true)
			material->SetDetailMap(r->String());
	}

	SAFE_DELETE(tex1);
	SAFE_DELETE(tex2);
	SAFE_DELETE(tex3);
	SAFE_DELETE(tex4);

	//texture
	{
		b = r->Bool();
		if (b == true)
			tex1 = new Texture(String::ToWString(r->String()));

		b = r->Bool();
		if (b == true)
			tex2 = new Texture(String::ToWString(r->String()));

		b = r->Bool();
		if (b == true)
			tex3 = new Texture(String::ToWString(r->String()));

		b = r->Bool();
		if (b == true)
			tex4 = new Texture(String::ToWString(r->String()));
	}

	//vertex & index
	{

		vertexSize = r->UInt();
		r->Byte((void**)&vertices, sizeof(VertexType) * vertexSize);
		indexSize = r->UInt();
		r->Byte((void**)&indices, sizeof(UINT) * indexSize);
	}

	r->Close();
	SAFE_DELETE(r);

	return;
}

void GameTerrain::OpenTexture(wstring file)
{
	D3DDesc descc;
	D3D::GetDesc(&descc);

	if (file.length() < 1)
	{
		function<void(wstring)> f;
		f = bind(&GameTerrain::OpenTexture, this, placeholders::_1);

		Path::OpenFileDialog(L"", Path::ImageFilter, Textures, f, descc.Handle);

		return;
	}

	textureFile = file;
	return;
}

void GameTerrain::SetTexture(UINT num, wstring file)
{
	if (num > 5) return;
	if (file.length() < 1) return;

	wstring fileName = Path::GetFileLocalPath(file);

	switch (num)
	{
	case 0:
		material->SetDiffuseMap(fileName);
		break;
	case 1:
		SAFE_DELETE(tex1);

		tex1 = new Texture(fileName);
		break;
	case 2:
		SAFE_DELETE(tex1);

		tex2 = new Texture(fileName);
		break;
	case 3:
		SAFE_DELETE(tex1);

		tex3 = new Texture(fileName);
		break;
	case 4:
		SAFE_DELETE(tex1);

		tex4 = new Texture(fileName);
		break;
	}
}

void GameTerrain::Update()
{
	intersectCheck[0] = vertices[0].position;
	intersectCheck[1] = vertices[(width + 1) * height].position;
	intersectCheck[2] = vertices[width].position;
	intersectCheck[3] = vertices[(width + 1) * height + width].position;
}

void GameTerrain::Render()
{
	if (tex1 != NULL)
		tex1->SetShaderResource(5);
	if (tex2 != NULL)
		tex2->SetShaderResource(6);
	if (tex3 != NULL)
		tex3->SetShaderResource(7);
	if (tex4 != NULL)
		tex4->SetShaderResource(8);

	UINT stride = sizeof(VertexType);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	material->SetShader(shader);
	buffer->SetVSBuffer(1);
	material->SetBuffer();

	D3D::GetDC()->DrawIndexed(indexSize, 0, 0);
}

void GameTerrain::PreRender()
{
	if (tex1 != NULL)
		tex1->SetShaderResource(5);
	if (tex2 != NULL)
		tex2->SetShaderResource(6);
	if (tex3 != NULL)
		tex3->SetShaderResource(7);
	if (tex4 != NULL)
		tex4->SetShaderResource(8);

	UINT stride = sizeof(VertexType);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	material->SetShader(shader2);
	buffer->SetVSBuffer(1);
	material->SetBuffer();

	D3D::GetDC()->DrawIndexed(indexSize, 0, 0);
}

void GameTerrain::PreRender2()
{
	if (tex1 != NULL)
		tex1->SetShaderResource(5);
	if (tex2 != NULL)
		tex2->SetShaderResource(6);
	if (tex3 != NULL)
		tex3->SetShaderResource(7);
	if (tex4 != NULL)
		tex4->SetShaderResource(8);

	UINT stride = sizeof(VertexType);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	material->SetShader(shader3);
	buffer->SetVSBuffer(1);
	material->SetBuffer();

	D3D::GetDC()->DrawIndexed(indexSize, 0, 0);
}

void GameTerrain::ImGuiRender()
{
	ImGui::BeginMainMenuBar();
	{
		if (ImGui::BeginMenu("Terrain"))
		{
			if (ImGui::MenuItem("Save Terrain"))
				SaveTerrain();
			if (ImGui::MenuItem("Load Terrain"))
				LoadTerrain();

			ImGui::Separator();

			if (ImGui::MenuItem("Texture 1"))
			{
				OpenTexture();
				SetTexture(0, textureFile);
			}
			if (ImGui::MenuItem("Texture 2"))
			{
				OpenTexture();
				SetTexture(1, textureFile);
			}
			if (ImGui::MenuItem("Texture 3"))
			{
				OpenTexture();
				SetTexture(2, textureFile);
			}
			if (ImGui::MenuItem("Texture 4"))
			{
				OpenTexture();
				SetTexture(3, textureFile);
			}
			if (ImGui::MenuItem("Texture 5"))
			{
				OpenTexture();
				SetTexture(4, textureFile);
			}

			ImGui::EndMenu();
		}
	}
	ImGui::EndMainMenuBar();
}

bool GameTerrain::Intersect(D3DXVECTOR3 cam, D3DXVECTOR3 camDir, float & dis)
{
	if (D3DXIntersectTri(&intersectCheck[0], &intersectCheck[1], &intersectCheck[2], &cam, &camDir, NULL, NULL, &dis) == true)
	{

	}
	else if (D3DXIntersectTri(&intersectCheck[2], &intersectCheck[1], &intersectCheck[3], &cam, &camDir, NULL, NULL, &dis) == true)
	{

	}
	else
		return false;

	for (UINT z = 0; z < height; z++)
	{
		for (UINT x = 0; x < width; x++)
		{
			UINT ind0 = (width + 1) * z + x;
			UINT ind1 = (width + 1) * (z+ 1) + x;
			UINT ind2 = (width + 1) * z + x + 1;
			UINT ind3 = (width + 1) * (z + 1) + (x + 1);

			D3DXVECTOR3 pos0 = vertices[ind0].position;
			D3DXVECTOR3 pos1 = vertices[ind1].position;
			D3DXVECTOR3 pos2 = vertices[ind2].position;
			D3DXVECTOR3 pos3 = vertices[ind3].position;

			if (D3DXIntersectTri(&pos0, &pos1, &pos2, &cam, &camDir, NULL, NULL, &dis))
			{
				return true;
			}

			if (D3DXIntersectTri(&pos2, &pos1, &pos3, &cam, &camDir, NULL, NULL, &dis))
			{
				return true;
			}
		}
	}

	return false;
}

void GameTerrain::Init()
{
	shader = new Shader(Shaders + L"999_Terrain.hlsl");
	shader2 = new Shader(Shaders + L"999_Terrain.hlsl", "VS_Normal", "PS_Normal");
	shader3 = new Shader(Shaders + L"999_Terrain.hlsl", "VS_Depth", "PS_Depth");
	buffer = new WorldBuffer();

	material = new Material;
	material->SetShader(shader);

	width = 255;
	height = 255;

	vertexSize = (height+1) * (width+1);
	vertices = new VertexType[vertexSize];

	UINT index = 0;
	for (UINT z = 0; z <= height; z++)
	{
		for (UINT x = 0; x <= width; x++)
		{
			vertices[index].position = D3DXVECTOR3((float)x, 0, (float)z);
			vertices[index].uv = D3DXVECTOR2((float)x / (float)width, (float)z / (float)height);

			vertices[index].normal = D3DXVECTOR3(0, 0, 0);
			vertices[index].color = D3DXCOLOR(0, 0, 0, 0);
			index++;
		}
	}

	indexSize = height * width * 6;
	indices = new UINT[indexSize];

	index = 0;
	for (UINT z = 0; z < height; z++)
	{
		for (UINT x = 0; x < width; x++)
		{
			indices[index + 0] = (width + 1) * z + x;
			indices[index + 1] = (width + 1) * (z+1) + x;
			indices[index + 2] = (width + 1) * z + x + 1;
			indices[index + 3] = (width + 1) * z + x + 1;
			indices[index + 4] = (width + 1) * (z+1) + x;
			indices[index + 5] = (width + 1) * (z+1) + x+1;

			index += 6;
		}
	}

	int a = 10;
}

void GameTerrain::CreateNormal()
{
	for (UINT i = 0; i < indexSize / 3; i++)
	{
		UINT index0 = indices[i * 3 + 0];
		UINT index1 = indices[i * 3 + 1];
		UINT index2 = indices[i * 3 + 2];

		D3DXVECTOR3 v0 = vertices[index0].position;
		D3DXVECTOR3 v1 = vertices[index1].position;
		D3DXVECTOR3 v2 = vertices[index2].position;

		D3DXVECTOR3 d1 = v1 - v0;
		D3DXVECTOR3 d2 = v2 - v0;

		D3DXVECTOR3 normal;
		D3DXVec3Cross(&normal, &d1, &d2);

		vertices[index0].normal += normal;
		vertices[index1].normal += normal;
		vertices[index2].normal += normal;
	}

	for (UINT i = 0; i < vertexSize; i++)
		D3DXVec3Normalize(&vertices[i].normal, &vertices[i].normal);
}

void GameTerrain::CreateBuffer()
{
	//Vertex Buffer Create
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexType) * vertexSize;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = vertices;

		HRESULT hr;
		hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}

	//Index Buffer Create
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(UINT) * indexSize;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = indices;

		HRESULT hr;
		hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
		assert(SUCCEEDED(hr));
	}
}
