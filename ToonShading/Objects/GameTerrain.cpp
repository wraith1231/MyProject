#include "stdafx.h"
#include "GameTerrain.h"

#include "Tree.h"
#include "Water.h"

#include "../Bounding/BoundingBox.h"
#include "../Bounding/ObjectsRay.h"

#include "../Lights/PointLight.h"
#include "../Lights/SpotLight.h"
#include "../Lights/Fog.h"

#include "../Utilities/BinaryFile.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
//editMode는 editor 상태일때만
//shader = 기본, shader2 = normal, shader3 = depth
//texureFile은 맵 texture용, terrainFile은 맵 저장용
//editType : 0 = height 높낮이, 1 = height 조절, 2 = height 0으로, 3 = 나무심기
//terrainBuffer -> type : 0 = 원형 브러쉬, 1 = 사각형 브러쉬
///////////////////////////////////////////////////////////////////////////////////////////////////

GameTerrain::GameTerrain(ExecuteValues* value)
	: values(value)
{
	FirstInit();
	Init();
	CreateNormal();
	CreateBuffer();

}

GameTerrain::~GameTerrain()
{
	Clear();
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
	if (Path::GetExtension(file) != L"terrain")
	{
		file += L".terrain";
	}

	terrainFile = file;

	//material - texture1234 - vertex&index - water - Point Light - Spot Light - Fog - Tree 순 로드

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
		w->UInt(width);
		w->UInt(height);

		w->UInt(vertexSize);
		w->Byte(vertices, sizeof(VertexType) * vertexSize);
		w->UInt(indexSize);
		w->Byte(indices, sizeof(UINT) * indexSize);
	}

	//Water
	{
		w->Bool(useWater);
		if (useWater == true)
		{
			WaterStruct temp = water->GetWaterParameter();
			w->Byte(&temp, sizeof(WaterStruct));
		}
	}

	//Point Light
	{
		w->UInt(pointLight->PointLightSize());
		if (pointLight->PointLightSize() > 0)
		{
			UINT count = 0;
			for (UINT i = 0; i < pointLight->PointLightMaxSize(); i++)
			{
				PointLightSave temp;
				if (pointLight->LightUse(i, temp) == true)
				{
					count++;
					w->Byte(&temp, sizeof(PointLightSave));
				}

				if (count > pointLight->PointLightSize())
					break;
			}
		}
	}

	//Spot Light
	{
		w->UInt(spotLight->SpotLightSize());
		if (spotLight->SpotLightSize() > 0)
		{
			UINT count = 0;
			for (UINT i = 0; i < spotLight->SpotLightMaxSize(); i++)
			{
				SpotLightSave temp;
				if (spotLight->LightUse(i, temp) == true)
				{
					count++;
					w->Byte(&temp, sizeof(SpotLightSave));
				}

				if (count > spotLight->SpotLightSize())
					break;
			}
		}
	}

	//Fog
	{
		w->Bool(useFog);
		if (useFog == true)
		{
			FogSave temp = fog->GetFogData();
			w->Byte(&temp, sizeof(FogSave));
		}
	}

	//Tree
	{
		w->Float(windPower);
		w->UInt(trees.size());
		for (Tree* tree : trees)
		{
			w->String(String::ToString(tree->FileName()));
			w->UInt(tree->Trees().size());
			w->Byte(&tree->Trees()[0], sizeof(TreeStruct) * tree->Trees().size());
		}
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

	Clear();

	//material - texture1234 - vertex&index - water - Point Light - Spot Light - Fog - Tree 순 로드
	BinaryReader* r = new BinaryReader();

	wstring fileName = Path::GetFileLocalPath(saveFile);
	r->Open(fileName);

	bool b = false;

	//material
	string diffuseMap;
	string specularMap;
	string emissiveMap;
	string normalMap;
	string detailMap;
	{
		b = r->Bool();
		if (b == true)
			diffuseMap = r->String();

		b = r->Bool();
		if (b == true)
			specularMap = r->String();

		b = r->Bool();
		if (b == true)
			emissiveMap = r->String();

		b = r->Bool();
		if (b == true)
			normalMap = r->String();

		b = r->Bool();
		if (b == true)
			detailMap = r->String();
	}

	//texture
	wstring tex1Map, tex2Map, tex3Map, tex4Map;
	tex1Map = tex2Map = tex3Map = tex4Map = L"";
	{
		b = r->Bool();
		if (b == true)
			tex1Map = String::ToWString(r->String());

		b = r->Bool();
		if (b == true)
			tex2Map = String::ToWString(r->String());

		b = r->Bool();
		if (b == true)
			tex3Map = String::ToWString(r->String());

		b = r->Bool();
		if (b == true)
			tex4Map = String::ToWString(r->String());
	}

	//vertex & index
	{
		width = r->UInt();
		height = r->UInt();
		FirstInit(width, height);
		Init(width, height);

		vertexSize = r->UInt();
		r->Byte((void**)&vertices, sizeof(VertexType) * vertexSize);
		indexSize = r->UInt();
		r->Byte((void**)&indices, sizeof(UINT) * indexSize);
		CreateNormal();
		CreateBuffer();
	}

	if(diffuseMap.length() > 0)
		material->SetDiffuseMap(diffuseMap);
	if(specularMap.length() > 0)
		material->SetSpecularMap(specularMap);
	if(emissiveMap.length() > 0)
		material->SetEmissiveMap(emissiveMap);
	if(normalMap.length() > 0)
		material->SetNormalMap(normalMap);
	if(detailMap.length() > 0)
		material->SetDetailMap(detailMap);
	if(tex1Map.length() > 0)
		tex1 = new Texture(tex1Map);
	if(tex2Map.length() > 0)
		tex2 = new Texture(tex2Map);
	if(tex3Map.length() > 0)
		tex3 = new Texture(tex3Map);
	if(tex4Map.length() > 0)
		tex4 = new Texture(tex4Map);

	//Water
	{
		useWater = r->Bool();
		if (useWater == true)
		{
			WaterStruct* temp = new WaterStruct();
			r->Byte((void**)&temp, sizeof(WaterStruct));

			water = new Water(width, height);
			water->SetWaterParameter(*temp);

			SAFE_DELETE(temp);
		}
	}

	//Point Light
	{
		UINT size = r->UInt();
		for (UINT i = 0; i < size; i++)
		{
			PointLightSave* temp = new PointLightSave;
			r->Byte((void**)&temp, sizeof(PointLightSave));

			pointLight->AddPointLight(temp->Position, temp->Color, temp->intenstiy, temp->range);
			SAFE_DELETE(temp);
		}
	}

	//Spot Light
	{
		UINT size = r->UInt();
		for (UINT i = 0; i < size; i++)
		{
			SpotLightSave* temp = new SpotLightSave;
			r->Byte((void**)&temp, sizeof(SpotLightSave));

			spotLight->AddSpotLight(temp->Position, temp->Color, temp->Direction, temp->InnerAngle, temp->OuterAngle);
			SAFE_DELETE(temp);
		}
	}

	//Fog
	{
		useFog = r->Bool();
		if (useFog == true)
		{
			FogSave* temp = new FogSave;
			r->Byte((void**)&temp, sizeof(FogSave));
	
			fog = new Fog;
			fog->SetFogData(*temp);
			SAFE_DELETE(temp);
		}
	}

	//tree
	{
		windPower = r->Float();
		UINT num = r->UInt();
		for (UINT i = 0; i < num; i++)
		{
			wstring file = String::ToWString(r->String());
			file = Path::GetFileLocalPath(file);
			UINT size = r->UInt();

			vector<TreeStruct> temp;
			temp.assign(size, TreeStruct());
			void* ptr = (void*)&temp[0];
			r->Byte(&ptr, sizeof(TreeStruct) * size);

			Tree* tree = new Tree(file);
			tree->TreeLoad(temp);
			trees.push_back(tree);
		}

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
		SAFE_DELETE(tex2);

		tex2 = new Texture(fileName);
		break;
	case 3:
		SAFE_DELETE(tex3);

		tex3 = new Texture(fileName);
		break;
	case 4:
		SAFE_DELETE(tex4);

		tex4 = new Texture(fileName);
		break;
	}
}

void GameTerrain::Update()
{
	terrainBuffer->Data.On = (UINT)editMode;

	if (editMode == true && pointLightDispose == false && pointLightSelect == false && spotLightDispose == false && spotLightSelect == false)
	{
		terrainBuffer->Data.Point = selTer;
	}
	else
		terrainBuffer->Data.Point = D3DXVECTOR3(-10, -10, -10);

	if (changed == true && Mouse::Get()->Up(0) == true)
	{
		changed = false;
		CreateNormal();
	}
	D3D::GetDC()->UpdateSubresource
	(
		vertexBuffer, 0, NULL, vertices, sizeof(VertexType) * vertexSize, 0
	);
	treeDelay += Time::Delta();

	offset += windPower * Time::Delta();
	if (offset > D3DX_PI * 2) offset -= static_cast<float>(D3DX_PI * 2);
	treeBuffer->Data.Power = sinf(offset);

	QuadTreeUpdate();

	for (Tree* tree : trees)
		tree->Update();

	if (water != NULL && useWater == true)
		water->Update();
}

void GameTerrain::DiffuseRender()
{
	if (pointLight != NULL)
		pointLight->DiffuseRender(pointLightSelect);
	if (spotLight != NULL)
		spotLight->DiffuseRender(spotLightSelect);
	if (fog != NULL)
		fog->DiffuseRender();

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

	material->SetShader(diffuseShader);
	buffer->SetVSBuffer(1);
	terrainBuffer->SetPSBuffer(2);
	material->SetBuffer();

	D3D::GetDC()->DrawIndexed(indexSize, 0, 0);

	if (water != NULL)
		water->DiffuseRender();

	treeBuffer->SetVSBuffer(2);
	for (Tree* tree : trees)
		tree->DiffuseRender();

}

void GameTerrain::NormalRender()
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
	material->SetShader(normalShader);
	buffer->SetVSBuffer(1);
	material->SetBuffer();

	D3D::GetDC()->DrawIndexed(indexSize, 0, 0);
}

void GameTerrain::DepthRender()
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
	material->SetShader(depthShader);
	buffer->SetVSBuffer(1);
	material->SetBuffer();

	D3D::GetDC()->DrawIndexed(indexSize, 0, 0);

	if (water != NULL)
		water->DepthRender();
	treeBuffer->SetVSBuffer(2);
	for (Tree* tree : trees)
		tree->DepthRender();
}

void GameTerrain::ImGuiRender()
{
	//상단 메뉴바
	ImGui::BeginMainMenuBar();
	{
		if (ImGui::BeginMenu("Terrain"))
		{
			if (ImGui::MenuItem("Save Terrain"))
				SaveTerrain();
			if (ImGui::MenuItem("Load Terrain"))
				LoadTerrain();

			if (ImGui::MenuItem("Terrain Edit"))
			{
				editMode = !editMode;
				pointLightDispose = false;
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Point Light Dispose"))
			{
				if(editMode == false)
					editMode = true;

				if (editMode == true)
				{
					pointLightDispose = !pointLightDispose;
					pointLightSelect = false;
					spotLightDispose = false;
					spotLightSelect = false;
				}
				else
				{
					pointLightDispose = false;
				}
			}
			if (ImGui::MenuItem("Point Light Select"))
			{
				if (editMode == false)
					editMode = true;

				if (editMode == true)
				{
					pointLightDispose = false;
					pointLightSelect = !pointLightSelect;
					spotLightDispose = false;
					spotLightSelect = false;
				}
				else
					pointLightSelect = false;
			}

			if (ImGui::MenuItem("Spot Light Dispose"))
			{
				if (editMode == false)
					editMode = true;

				if (editMode == true)
				{
					pointLightDispose = false;
					pointLightSelect = false;
					spotLightDispose = !spotLightDispose;
					spotLightSelect = false;
				}
				else
				{
					pointLightDispose = false;
				}
			}
			if (ImGui::MenuItem("Spot Light Select"))
			{
				if (editMode == false)
					editMode = true;

				if (editMode == true)
				{
					pointLightDispose = false;
					pointLightSelect = false;
					spotLightDispose = false;
					spotLightSelect = !spotLightSelect;
				}
				else
					pointLightSelect = false;
			}

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

	//imgui
	if (editMode == true && pointLightDispose == false && spotLightDispose == false && pointLightSelect == false && spotLightSelect == false)
	{
		ImGui::Begin("Terrain Edit");

		{
			ImGui::Text("Cam : %.3f, %.3f, %.3f", selTer.x, selTer.y, selTer.z);
			ImGui::Checkbox("Use Water", &useWater);
			if (useWater == true && water == NULL)
			{
				water = new Water(width, height);
			}

			bool b = useFog;
			ImGui::Checkbox("Use Fog", &useFog);
			if (useFog == true && fog == NULL)
			{
				fog = new Fog;
			}
			if (b != useFog)
			{
				if (b == true) fog->SetUse(false);
				else fog->SetUse(true);
			}

			ImGui::InputInt("Width", (int*)&widthEdit);
			ImGui::SameLine();
			ImGui::InputInt("Height", (int*)&heightEdit);
			if (ImGui::Button("Terrain Resize : All Terrain Data Clear"))
			{
				if ((widthEdit >= 15 && heightEdit >= 15) && ((widthEdit % 4 == 3) && (heightEdit % 4 == 3)))
				{
					Clear();
					FirstInit(widthEdit, heightEdit);
					Init(widthEdit, heightEdit);
					CreateNormal();
					CreateBuffer();
				}
				else
				{
					D3DDesc desc;
					D3D::GetDesc(&desc);
					MessageBox(desc.Handle, L"Width and Height will over than 14 and quadruple - 1", L"Error", MB_OK);
				}
			}

			ImGui::SliderFloat("Wind Power", &windPower, 0.0f, 5.0f);
			ImGui::SliderInt("Edit Type", (int*)&editType, 0, 4);
			ImGui::Text("Edit Type : 0 = Height change");
			ImGui::Text("1 = Height standardize");
			ImGui::Text("2 = Height 0 3 = Splatting");
			ImGui::Text("4 = Tree");
			ImGui::SliderInt("Type", (int*)&terrainBuffer->Data.Type, 0, 1);
			ImGui::Text("Type : 0 = Circle, 1 = Square");
			ImGui::SliderFloat("Distance", &terrainBuffer->Data.Distance, 0.1f, 10.0f);
			if (editType == 0)
				ImGui::InputFloat("Power", &power);
			if (editType == 2)
				ImGui::InputFloat("Height Set", &heightSet);
			else if (editType == 3)
			{
				ImGui::ColorEdit4("Splatting", splat);
			}
			else if (editType == 4)
			{
				if (ImGui::Button("Tree Texture"))
				{
					TreeFile();
				}
				ImGui::SliderFloat("Tree Scale", &treeScale, 0.1f, 20.0f);
				ImGui::SliderInt("Tree Num", (int*)&treeNum, 1, 20);
			}
		}

		ImGui::End();

		if (fog != NULL && useFog == true)
			fog->ImGuiRender();

		if (water != NULL && useWater == true)
			water->ImGuiRender();
	}

	if (spotLightSelect == true)
		spotLight->ImGuiRender();

	if (pointLightSelect == true)
		pointLight->ImGuiRender();
}

bool GameTerrain::Intersect(D3DXVECTOR3 cam, D3DXVECTOR3 camDir, float & dis)
{
	//root
	D3DXVECTOR3 LB, LT, RB, RT;
	if (quadTreeRoot->FindQuad(cam, camDir, dis, LB, LT, RB, RT) == true)
	{
		for (int z = (int)LB.z - 1; z <= (int)LT.z + 1; z++)
		{
			if (z < 0 || z >= (int)height) continue;
			for (int x = (int)LB.x - 1; x <= (int)RB.x + 1; x++)
			{
				if (x <0 || x >= (int)width) continue;
		
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
					selTer = cam + camDir*dis;
					return true;
				}
				
				if (D3DXIntersectTri(&pos2, &pos1, &pos3, &cam, &camDir, NULL, NULL, &dis))
				{
					selTer = cam + camDir*dis;
					return true;
				}
			}	//for x
		}	//for z
	}

	return false;
}

void GameTerrain::EditMode(bool val)
{
	editMode = val;

	if (editMode == false)
	{
		if (treeDisposed == false)
			SAFE_DELETE(treeSet);
		treeSet = NULL;
		treeDisposed = false;

		pointLightDispose = false;
		pointLightSelect = false;
	}
}

void GameTerrain::EditTerrain()
{
	if (pointLightDispose == true || pointLightSelect == true || spotLightDispose == true || spotLightSelect == true)
		return;

	if (terrainBuffer->Data.Type == 0)
	{
		//circle brush edit

		//Height
		if (editType < 4)
		{
			for (int z = (int)selTer.z - (int)terrainBuffer->Data.Distance; z < (int)selTer.z + (int)terrainBuffer->Data.Distance; z++)
			{
				if (z < 0 || z >(int)height) continue;
				for (int x = (int)selTer.x - (int)terrainBuffer->Data.Distance; x < (int)selTer.x + (int)terrainBuffer->Data.Distance; x++)
				{
					if (x < 0 || x >(int)width) continue;

					float xdis = selTer.x - (float)x;
					float zdis = selTer.z - (float)z;

					float dis = sqrtf(xdis*xdis + zdis*zdis);
					if (dis < terrainBuffer->Data.Distance)
					{
						float inten = 1.0f - dis / terrainBuffer->Data.Distance;
						if (editType == 0)
						{
							//Height Change
							if (Keyboard::Get()->Press(VK_SHIFT))
								vertices[x + z * (width + 1)].position.y -= inten * power * Time::Delta();
							else
								vertices[x + z * (width + 1)].position.y += inten * power * Time::Delta();
						}
						else if (editType == 1)
						{
							//Height Standardize
							float h = 0.0f;

							int count = 0;

							UINT c = x + (z + 1) * (width + 1);
							if (c >= 0 && c <= vertexSize)
							{
								h += vertices[x + (z + 1) * (width + 1)].position.y;
								count++;
							}

							c = x - 1 + z * (width + 1);
							if (c >= 0 && c <= vertexSize)
							{
								h += vertices[x - 1 + z * (width + 1)].position.y;
								count++;
							}

							h += vertices[x + z * (width + 1)].position.y;
							count++;

							c = x + 1 + z * (width + 1);
							if (c >= 0 && c <= vertexSize)
							{
								h += vertices[x + 1 + z * (width + 1)].position.y;
								count++;
							}
							c = x + (z - 1) * (width + 1);
							if (c >= 0 && c <= vertexSize)
							{
								h += vertices[x + (z - 1) * (width + 1)].position.y;
								count++;
							}

							h /= count;
							h -= vertices[x + z * (width + 1)].position.y;

							vertices[x + z * (width + 1)].position.y += h * Time::Delta();
						}
						else if (editType == 2)
						{
							vertices[x + z * (width + 1)].position.y = heightSet;
						}
						else if (editType == 3)
						{
							//splatting
							D3DXCOLOR ori = vertices[x + z * (width + 1)].color;
							inten *= 0.2f;
							D3DXCOLOR temp = SplatColor(ori, splat, inten);

							vertices[x + z * (width + 1)].color = temp;
						}
					}// if dis < distance
				} //for x
			}//for z
		}//editType < 4
		else if (editType == 4)
		{
			if (treeSet != NULL && treeDelay >= 0.5f)
			{
				treeDelay = 0.0f;
				for (UINT i = 0; i < treeNum; i++)
				{
					float minx = selTer.x - terrainBuffer->Data.Distance;
					float minz = selTer.z - terrainBuffer->Data.Distance;
					float maxx = selTer.x + terrainBuffer->Data.Distance;
					float maxz = selTer.z + terrainBuffer->Data.Distance;

					float x = Math::Random(minx, maxx);
					float z = Math::Random(minz, maxz);
					float y;
					if (GetHeight(x, z, y) == true)
					{
						treeSet->AddTree(treeScale, D3DXVECTOR3(x, y, z));

						if (treeDisposed == false)
						{
							treeDisposed = true;
							trees.push_back(treeSet);
						}
					}
				} //for i

			} //treeSet != NULL
		} //editType == 4
	}	//terrainBuffer->Data.type == 0
	else if (terrainBuffer->Data.Type == 1)
	{
		//square brush edit

		//Height
		if (editType < 4)
		{
			for (int z = (int)selTer.z - (int)terrainBuffer->Data.Distance; z < (int)selTer.z + (int)terrainBuffer->Data.Distance; z++)
			{
				if (z < 0 || z >(int)height) continue;
				for (int x = (int)selTer.x - (int)terrainBuffer->Data.Distance; x < (int)selTer.x + (int)terrainBuffer->Data.Distance; x++)
				{
					if (x < 0 || x >(int)width) continue;
					
					if (editType == 0)
					{
						//Height Change
						if (Keyboard::Get()->Press(VK_SHIFT))
							vertices[x + z * (width + 1)].position.y -= power * Time::Delta();
						else
							vertices[x + z * (width + 1)].position.y += power * Time::Delta();
					}
					else if (editType == 1)
					{
						//Height Standardize
						float h = 0.0f;

						int count = 0;

						UINT c = x + (z + 1) * (width + 1);
						if (c >= 0 && c <= vertexSize)
						{
							h += vertices[x + (z + 1) * (width + 1)].position.y;
							count++;
						}

						c = x - 1 + z * (width + 1);
						if (c >= 0 && c <= vertexSize)
						{
							h += vertices[x - 1 + z * (width + 1)].position.y;
							count++;
						}

						h += vertices[x + z * (width + 1)].position.y;
						count++;

						c = x + 1 + z * (width + 1);
						if (c >= 0 && c <= vertexSize)
						{
							h += vertices[x + 1 + z * (width + 1)].position.y;
							count++;
						}
						c = x + (z - 1) * (width + 1);
						if (c >= 0 && c <= vertexSize)
						{
							h += vertices[x + (z - 1) * (width + 1)].position.y;
							count++;
						}

						h /= count;
						h -= vertices[x + z * (width + 1)].position.y;

						vertices[x + z * (width + 1)].position.y += h * Time::Delta();
					}
					else if (editType == 2)
					{
						vertices[x + z * (width + 1)].position.y = heightSet;
					}
					else if (editType == 3)
					{
						//splatting
						float xdis = selTer.x - x;
						float zdis = selTer.z - z;
						float dis = sqrt(xdis * xdis + zdis * zdis);
						if (dis > terrainBuffer->Data.Distance - 0.01f) dis = terrainBuffer->Data.Distance - 0.01f;

						D3DXCOLOR ori = vertices[x + z * (width + 1)].color;
						float inten = 1.0f - dis / terrainBuffer->Data.Distance;
						inten *= 0.2f;
						D3DXCOLOR temp = SplatColor(ori, splat, inten);

						vertices[x + z * (width + 1)].color = temp;
					}
				}//for x
			}//for z
		}//editType < 4
		else if (editType == 4)
		{
			if (treeSet != NULL && treeDelay >= 0.5f)
			{
				treeDelay = 0.0f;
				for (UINT i = 0; i < treeNum; i++)
				{
					float minx = selTer.x - terrainBuffer->Data.Distance;
					float minz = selTer.z - terrainBuffer->Data.Distance;
					float maxx = selTer.x + terrainBuffer->Data.Distance;
					float maxz = selTer.z + terrainBuffer->Data.Distance;

					float x = Math::Random(minx, maxx);
					float z = Math::Random(minz, maxz);
					float y;
					if (GetHeight(x, z, y) == true)
					{
						treeSet->AddTree(treeScale, D3DXVECTOR3(x, y, z));

						if (treeDisposed == false)
						{
							treeDisposed = true;
							trees.push_back(treeSet);
						}
					}
				} //for i

			} //treeSet != NULL

		}//editType == 4
	}

	D3D::GetDC()->UpdateSubresource
	(
		vertexBuffer, 0, NULL, vertices, sizeof(VertexType) * vertexSize, 0
	);
	changed = true;
}

void GameTerrain::PointLightDispose(D3DXVECTOR3 pos)
{
	if (pointLightDispose == true)
	{
		UINT num = pointLight->AddPointLight(pos);
		if (num < pointLight->PointLightMaxSize())
		{
			pointLightDispose = false;
			pointLightSelect = true;
			pointLight->LightSelect(true, num);
		}
	}
}

void GameTerrain::PointLightSelect(Objects::Ray* ray)
{
	if (pointLightSelect == true)
	{
		pointLight->LightSelect(ray);
	}
}

void GameTerrain::SpotLightDispose(D3DXVECTOR3 pos)
{
	if (spotLightDispose == true)
	{
		UINT num = spotLight->AddSpotLight(pos);
		if (num < spotLight->SpotLightMaxSize())
		{
			spotLightDispose = false;
			spotLightSelect = true;
			spotLight->LightSelect(true, num);
		}
	}
}

void GameTerrain::SpotLightSelect(Objects::Ray * ray)
{
	if (spotLightSelect == true)
	{
		spotLight->LightSelect(ray);
	}
}

bool GameTerrain::GetHeight(float x, float z, float & y)
{
	if (x < 0 || x >= width)
		return 0.0f;

	if (z < 0 || z >= height)
		return 0.0f;

	UINT index0 = (width + 1) * (UINT)z + (UINT)x;
	UINT index1 = (width + 1) * (UINT)(z + 1) + (UINT)x;
	UINT index2 = (width + 1) * (UINT)z + (UINT)x + 1;
	UINT index3 = (width + 1) * (UINT)(z + 1) + (UINT)(x + 1);

	D3DXVECTOR3 v0 = vertices[index0].position;
	D3DXVECTOR3 v1 = vertices[index1].position;
	D3DXVECTOR3 v2 = vertices[index2].position;
	D3DXVECTOR3 v3 = vertices[index3].position;

	float deltaX = x - v0.x;
	float deltaZ = z - v0.z;

	D3DXVECTOR3 temp;
	if (deltaX + deltaZ <= 1)
		temp = v0 + (v2 - v0) * deltaX + (v1 - v0) * deltaZ;
	else
	{
		deltaX = 1 - deltaX;
		deltaZ = 1 - deltaZ;

		temp = v3 + (v2 - v3) * deltaX + (v1 - v3) * deltaZ;
	}

	y = temp.y;

	return true;
}

bool GameTerrain::GetHeight(D3DXVECTOR3 & pos)
{
	return GetHeight(pos.x, pos.z, pos.y);
}

void GameTerrain::FirstInit(UINT width, UINT height)
{
	tex1 = tex2 = tex3 = tex4 = NULL;
	diffuseShader = normalShader = depthShader = NULL;
	vertexBuffer = indexBuffer = NULL;
	pointLight = NULL;
	spotLight = NULL;
	quadTreeRoot = NULL;
	fog = NULL;

	terrainFile = L"";
	editMode = changed = treeDisposed = useWater = useFog = false;
	pointLightDispose = pointLightSelect = false;
	spotLightDispose = spotLightSelect = false;
	heightSet = treeDelay = offset = windPower = 0.0f;
	widthEdit = width;
	heightEdit = height;

	treeSet = NULL;
	water = NULL;
	editType = 0;
	treeNum = 1;
	treeScale = 1.0f;
	power = 5.0f;
	splat = D3DXCOLOR(0, 0, 0, 0);


	diffuseShader = new Shader(Shaders + L"999_Terrain.hlsl");
	normalShader = new Shader(Shaders + L"999_Terrain.hlsl", "VS_Normal", "PS_Normal");
	depthShader = new Shader(Shaders + L"999_Terrain.hlsl", "VS_Depth", "PS_Depth");
	buffer = new WorldBuffer();
	terrainBuffer = new TerrainBuffer();
	treeBuffer = new TreeBuffer();
	D3DXMatrixRotationY(&treeBuffer->Data.Rotate, (float)D3DX_PI);
	pointLight = new PointLight();
	spotLight = new SpotLight();

	QuadTreeMake(widthEdit, heightEdit);

	material = new Material;
}

void GameTerrain::Init(UINT width, UINT height)
{
	this->width = width;
	this->height = height;

	vertexSize = (this->height+1) * (this->width+1);
	vertices = new VertexType[vertexSize];

	UINT index = 0;
	for (UINT z = 0; z <= this->height; z++)
	{
		for (UINT x = 0; x <= this->width; x++)
		{
			vertices[index].position = D3DXVECTOR3((float)x, 0, (float)z);
			vertices[index].uv = D3DXVECTOR2((float)x / (float)this->width, (float)z / (float)this->height);

			vertices[index].normal = D3DXVECTOR3(0, 0, 0);
			vertices[index].color = D3DXCOLOR(0, 0, 0, 0);
			index++;
		}
	}

	indexSize = this->height * this->width * 6;
	indices = new UINT[indexSize];

	index = 0;
	for (UINT z = 0; z < this->height; z++)
	{
		for (UINT x = 0; x < this->width; x++)
		{
			indices[index + 0] = (this->width + 1) * z + x;
			indices[index + 1] = (this->width + 1) * (z+1) + x;
			indices[index + 2] = (this->width + 1) * z + x + 1;
			indices[index + 3] = (this->width + 1) * z + x + 1;
			indices[index + 4] = (this->width + 1) * (z+1) + x;
			indices[index + 5] = (this->width + 1) * (z+1) + x+1;

			index += 6;
		}
	}

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
	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);

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

void GameTerrain::QuadTreeMake(UINT width, UINT height)
{
	quadTreeRoot = new QuadTree();

	quadTreeRoot->LB = D3DXVECTOR3(0, 0, 0);
	quadTreeRoot->LT = D3DXVECTOR3(0, 0, (float)height);
	quadTreeRoot->RB = D3DXVECTOR3((float)width, 0, 0);
	quadTreeRoot->RT = D3DXVECTOR3((float)width, 0, (float)height);

	QuadTree* temp = quadTreeRoot;
	QuadTree* parent = quadTreeRoot;

	temp->ChildMake();

	//Root-Child
	{
		//0 -> Left Bottom
		temp = parent->child[0];
		temp->LB = parent->LB;
		temp->LT = parent->LT;
		temp->LT.z -= (parent->LT.z - parent->LB.z) / 2;
		temp->RB = parent->RB;
		temp->RB.x -= (parent->RB.x - parent->LB.x) / 2;
		temp->RT = parent->RT;
		temp->RT.x = temp->RB.x;
		temp->RT.z = temp->LT.z;

		//1 -> Left Top
		temp = parent->child[1];
		temp->LB = parent->child[0]->LT;
		temp->LT = parent->LT;
		temp->RB = parent->child[0]->RT;
		temp->RT = parent->RT;
		temp->RT.x = temp->RB.x;

		//2 -> Right Bottom
		temp = parent->child[2];
		temp->LB = parent->child[0]->RB;
		temp->LT = parent->child[0]->RT;
		temp->RB = parent->RB;
		temp->RT = parent->RT;
		temp->RT.z = temp->LT.z;

		//3 -> Right Top
		temp = parent->child[3];
		temp->LB = parent->child[0]->RT;
		temp->LT = parent->child[1]->RT;
		temp->RB = parent->child[2]->RT;
		temp->RT = parent->RT;
	}

	for (size_t i = 0; i < 4; i++)
		quadTreeRoot->child[i]->ChildMake();

	for (size_t i = 0; i < 4; i++)
	{
		parent = quadTreeRoot->child[i];

		{
			//0 -> Left Bottom
			temp = parent->child[0];
			temp->LB = parent->LB;
			temp->LT = parent->LT;
			temp->LT.z -= (parent->LT.z - parent->LB.z) / 2;
			temp->RB = parent->RB;
			temp->RB.x -=  (parent->RB.x - parent->LB.x) / 2;
			temp->RT = parent->RT;
			temp->RT.x = temp->RB.x;
			temp->RT.z = temp->LT.z;

			//1 -> Left Top
			temp = parent->child[1];
			temp->LB = parent->child[0]->LT;
			temp->LT = parent->LT;
			temp->RB = parent->child[0]->RT;
			temp->RT = parent->RT;
			temp->RT.x = temp->RB.x;

			//2 -> Right Bottom
			temp = parent->child[2];
			temp->LB = parent->child[0]->RB;
			temp->LT = parent->child[0]->RT;
			temp->RB = parent->RB;
			temp->RT = parent->RT;
			temp->RT.z = temp->LT.z;

			//3 -> Right Top
			temp = parent->child[3];
			temp->LB = parent->child[0]->RT;
			temp->LT = parent->child[1]->RT;
			temp->RB = parent->child[2]->RT;
			temp->RT = parent->RT;
		}
	}
}

void GameTerrain::Clear()
{
	if (fog != NULL)
	{
		fog->SetUse(false);
		fog->DiffuseRender();
	}
	SAFE_DELETE(fog);
	SAFE_DELETE(spotLight);
	SAFE_DELETE(pointLight);

	SAFE_DELETE(water);

	for (Tree* tree : trees)
		SAFE_DELETE(tree);
	trees.clear();

	SAFE_DELETE(treeBuffer);
	SAFE_DELETE(terrainBuffer);

	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);

	SAFE_DELETE(diffuseShader);
	SAFE_DELETE(normalShader);
	SAFE_DELETE(depthShader);

	SAFE_DELETE(tex1);
	SAFE_DELETE(tex2);
	SAFE_DELETE(tex3);
	SAFE_DELETE(tex4);

	SAFE_DELETE(material);
	SAFE_DELETE(buffer);

	SAFE_DELETE_ARRAY(vertices);
	SAFE_DELETE_ARRAY(indices);
}

void GameTerrain::TreeFile(wstring file)
{
	D3DDesc descc;
	D3D::GetDesc(&descc);

	if (file.length() < 1)
	{
		function<void(wstring)> f;
		f = bind(&GameTerrain::TreeFile, this, placeholders::_1);

		Path::OpenFileDialog(L"", Path::ImageFilter, Textures, f, descc.Handle);

		return;
	}

	if (treeDisposed == false && treeSet != NULL)
		SAFE_DELETE(treeSet);

	treeSet = new Tree(file, values);
	treeDisposed = false;
}

float GameTerrain::SplatColor(float origin, float brush, float inten)
{
	float ret = inten;
	if (brush > origin)
	{
		if (origin + inten > brush) ret = brush;
		else ret = origin + inten;
	}
	else
	{
		//bru.r < col.r
		if (origin - inten < brush) ret = brush;
		else ret = origin - inten;
	}
	return ret;
}

D3DXCOLOR GameTerrain::SplatColor(D3DXCOLOR origin, D3DXCOLOR brush, float inten)
{
	D3DXCOLOR ret;

	ret.r = SplatColor(origin.r, brush.r, inten);
	ret.g = SplatColor(origin.g, brush.g, inten);
	ret.b = SplatColor(origin.b, brush.b, inten);
	ret.a = SplatColor(origin.a, brush.a, inten);

	return ret;
}

void GameTerrain::QuadTreeUpdate()
{
	//root
	quadTreeRoot->LB = vertices[0].position;
	quadTreeRoot->LT = vertices[(width + 1) * height].position;
	quadTreeRoot->RB = vertices[width].position;
	quadTreeRoot->RT = vertices[(width + 1) * height + width].position;

	QuadTree* temp;
	QuadTree* parent = quadTreeRoot;

	//Root-Child
	{
		//0 -> Left Bottom
		temp = parent->child[0];
		temp->LB = parent->LB;
		temp->LT = parent->LT;
		temp->LT.z -= (parent->LT.z - parent->LB.z) / 2;
		temp->RB = parent->RB;
		temp->RB.x -= (parent->RB.x - parent->LB.x) / 2;
		temp->RT = parent->RT;
		temp->RT.x = temp->RB.x;
		temp->RT.z = temp->LT.z;

		//1 -> Left Top
		temp = parent->child[1];
		temp->LB = parent->child[0]->LT;
		temp->LT = parent->LT;
		temp->RB = parent->child[0]->RT;
		temp->RT = parent->RT;
		temp->RT.x = temp->RB.x;

		//2 -> Right Bottom
		temp = parent->child[2];
		temp->LB = parent->child[0]->RB;
		temp->LT = parent->child[0]->RT;
		temp->RB = parent->RB;
		temp->RT = parent->RT;
		temp->RT.z = temp->LT.z;

		//3 -> Right Top
		temp = parent->child[3];
		temp->LB = parent->child[0]->RT;
		temp->LT = parent->child[1]->RT;
		temp->RB = parent->child[2]->RT;
		temp->RT = parent->RT;
	}

	for (size_t i = 0; i < 4; i++)
	{
		parent = quadTreeRoot->child[i];

		{
			//0 -> Left Bottom
			temp = parent->child[0];
			temp->LB = parent->LB;
			temp->LT = parent->LT;
			temp->LT.z -= (parent->LT.z - parent->LB.z) / 2;
			temp->RB = parent->RB;
			temp->RB.x -= (parent->RB.x - parent->LB.x) / 2;
			temp->RT = parent->RT;
			temp->RT.x = temp->RB.x;
			temp->RT.z = temp->LT.z;

			//1 -> Left Top
			temp = parent->child[1];
			temp->LB = parent->child[0]->LT;
			temp->LT = parent->LT;
			temp->RB = parent->child[0]->RT;
			temp->RT = parent->RT;
			temp->RT.x = temp->RB.x;

			//2 -> Right Bottom
			temp = parent->child[2];
			temp->LB = parent->child[0]->RB;
			temp->LT = parent->child[0]->RT;
			temp->RB = parent->RB;
			temp->RT = parent->RT;
			temp->RT.z = temp->LT.z;

			//3 -> Right Top
			temp = parent->child[3];
			temp->LB = parent->child[0]->RT;
			temp->LT = parent->child[1]->RT;
			temp->RB = parent->child[2]->RT;
			temp->RT = parent->RT;
		}
	}
}

bool QuadTree::FindQuad(D3DXVECTOR3 cam, D3DXVECTOR3 camDir, float & dis, D3DXVECTOR3 & LB, D3DXVECTOR3& LT, D3DXVECTOR3& RB, D3DXVECTOR3& RT)
{
	D3DXVECTOR3 v0, v1, v2, v3;
	v0 = this->LB;
	v1 = this->LT;
	v2 = this->RB;
	v3 = this->RT;

	if (D3DXIntersectTri(&v0, &v1, &v2, &cam, &camDir, NULL, NULL, &dis) == TRUE)
	{
		if (child[0] != NULL)
		{
			if (child[0]->FindQuad(cam, camDir, dis, LB, LT, RB, RT) == true)
				return true;
		}
		if (child[1] != NULL)
		{
			if (child[1]->FindQuad(cam, camDir, dis, LB, LT, RB, RT) == true)
				return true;
		}
		if (child[2] != NULL)
		{
			if (child[2]->FindQuad(cam, camDir, dis, LB, LT, RB, RT) == true)
				return true;
		}

		if (child[0] == NULL && child[1] == NULL && child[2] == NULL)
		{
			LB = v0;
			LT = v1;
			RB = v2;
			RT = v3;
			return true;
		}
	}
	else if (D3DXIntersectTri(&v2, &v1, &v3, &cam, &camDir, NULL, NULL, &dis) == TRUE)
	{
		if (child[1] != NULL)
		{
			if (child[1]->FindQuad(cam, camDir, dis, LB, LT, RB, RT) == true)
				return true;
		}
		if (child[2] != NULL)
		{
			if (child[2]->FindQuad(cam, camDir, dis, LB, LT, RB, RT) == true)
				return true;
		}
		if (child[3] != NULL)
		{
			if (child[3]->FindQuad(cam, camDir, dis, LB, LT, RB, RT) == true)
				return true;
		}

		if (child[1] == NULL && child[2] == NULL && child[3] == NULL)
		{
			LB = v0;
			LT = v1;
			RB = v2;
			RT = v3;
			return true;
		}
	}

	return false;
}

void QuadTree::ChildMake()
{
	for (size_t i = 0; i < 4; i++)
		SAFE_DELETE(child[i]);

	for (size_t i = 0; i < 4; i++)
	{
		child[i] = new QuadTree;
		child[i]->root = this;
	}
}
