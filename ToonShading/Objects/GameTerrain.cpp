#include "stdafx.h"
#include "GameTerrain.h"

#include "Tree.h"
#include "Water.h"

#include "../Lights/PointLight.h"

#include "../Utilities/BinaryFile.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
//editMode는 editor 상태일때만
//shader = 기본, shader2 = normal, shader3 = depth
//texureFile은 맵 texture용, terrainFile은 맵 저장용
//editType : 0 = height 높낮이, 1 = height 조절, 2 = height 0으로, 3 = 나무심기
//terrainBuffer -> type : 0 = 원형 브러쉬, 1 = 사각형 브러쉬
///////////////////////////////////////////////////////////////////////////////////////////////////

GameTerrain::GameTerrain()
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

	//Water
	{
		w->Bool(useWater);
		if (useWater == true)
		{
			WaterStruct temp = water->GetWaterParameter();
			w->Byte(&temp, sizeof(WaterStruct));
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

	//Water
	{
		useWater = r->Bool();
		if (useWater == true)
		{
			WaterStruct* temp = new WaterStruct();
			r->Byte((void**)&temp, sizeof(WaterStruct));

			water = new Water(width, height);
			water->SetWaterParameter(*temp);
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

	if (editMode == true)
	{
		terrainBuffer->Data.Point = selTer;
	}

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

void GameTerrain::Render()
{
	if (pointLight != NULL)
		pointLight->Render();

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
	terrainBuffer->SetPSBuffer(2);
	material->SetBuffer();

	D3D::GetDC()->DrawIndexed(indexSize, 0, 0);

	if (water != NULL && useWater == true)
		water->Render();

	treeBuffer->SetVSBuffer(2);
	for (Tree* tree : trees)
		tree->Render();

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

	treeBuffer->SetVSBuffer(2);
	for (Tree* tree : trees)
		tree->PostRender2();
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
				editMode = !editMode;

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
	if (editMode == true)
	{
		ImGui::Begin("Terrain Edit");

		{
			ImGui::Checkbox("Use Water", &useWater);
			if (useWater == true && water == NULL)
			{
				water = new Water(width, height);
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

		if (water != NULL)
			water->ImGuiRender();
	}
}

bool GameTerrain::Intersect(D3DXVECTOR3 cam, D3DXVECTOR3 camDir, float & dis)
{
	//root
	D3DXVECTOR3 lb, lt, rb, rt;
	lb = quadRoot.LB;
	lt = quadRoot.LT;
	rb = quadRoot.RB;
	rt = quadRoot.RT;
	
	if (D3DXIntersectTri(&lb, &lt, &rb, &cam, &camDir, NULL, NULL, &dis) == TRUE || D3DXIntersectTri(&rb, &lt, &rt, &cam, &camDir, NULL, NULL, &dis) == TRUE)
	{
		//quadruple
		for (size_t i = 0; i < 4; i++)
		{
			lb = quadruple[i].LB;
			lt = quadruple[i].LT;
			rb = quadruple[i].RB;
			rt = quadruple[i].RT;
	
			if (D3DXIntersectTri(&lb, &lt, &rb, &cam, &camDir, NULL, NULL, &dis) == TRUE || D3DXIntersectTri(&rb, &lt, &rt, &cam, &camDir, NULL, NULL, &dis) == TRUE)
			{
				//sexdecuple
				for (size_t j = i * 4; j < i * 4 + 4; j++)
				{
					lb = sexdecuple[j].LB;
					lt = sexdecuple[j].LT;
					rb = sexdecuple[j].RB;
					rt = sexdecuple[j].RT;
	
					if (D3DXIntersectTri(&lb, &lt, &rb, &cam, &camDir, NULL, NULL, &dis) == TRUE || D3DXIntersectTri(&rb, &lt, &rt, &cam, &camDir, NULL, NULL, &dis) == TRUE)
					{
						for (int z = lb.z; z <= lt.z; z++)
						{
							if (z < 0 || z >= height) continue;
							for (int x = lb.x; x <= rb.x; x++)
							{
								if (x <0 || x >= width) continue;
	
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
					}	//sexdecuple Intersect
				}	//for sexdecuple
			}	//quadruple Intersect
		}	//for quadruple
	}	//quadRoot Intersect
	
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
	}
}

void GameTerrain::EditTerrain()
{
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
	shader = shader2 = shader3 = NULL;
	vertexBuffer = indexBuffer = NULL;
	pointLight = NULL;

	terrainFile = L"";
	editMode = changed = treeDisposed = useWater = false;
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


	shader = new Shader(Shaders + L"999_Terrain.hlsl");
	shader2 = new Shader(Shaders + L"999_Terrain.hlsl", "VS_Normal", "PS_Normal");
	shader3 = new Shader(Shaders + L"999_Terrain.hlsl", "VS_Depth", "PS_Depth");
	buffer = new WorldBuffer();
	terrainBuffer = new TerrainBuffer();
	treeBuffer = new TreeBuffer();
	D3DXMatrixRotationY(&treeBuffer->Data.Rotate, (float)D3DX_PI);
	pointLight = new PointLight();

	material = new Material;
	material->SetShader(shader);
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

void GameTerrain::Clear()
{
	SAFE_DELETE(pointLight);

	SAFE_DELETE(water);

	for (Tree* tree : trees)
		SAFE_DELETE(tree);
	trees.clear();

	SAFE_DELETE(treeBuffer);
	SAFE_DELETE(terrainBuffer);

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

	treeSet = new Tree(file);
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
	quadRoot.LB = vertices[0].position;
	quadRoot.LT = vertices[(width + 1) * height].position;
	quadRoot.RB = vertices[width].position;
	quadRoot.RT = vertices[(width + 1) * height + width].position;

	{
		//quadruple
		//0 - Left Bottom
		quadruple[0].LB = quadRoot.LB;
		quadruple[0].LT = quadRoot.LT / 2;
		quadruple[0].RB = quadRoot.RB / 2;
		quadruple[0].RT = quadRoot.RT / 2;

		//1 - Left Top
		quadruple[1].LB = quadruple[0].LT;
		quadruple[1].RB = quadruple[0].RT;
		quadruple[1].LT = quadRoot.LT;
		quadruple[1].RT = quadRoot.RT;
		quadruple[1].RT.x /= 2;

		//2 - Right Bottom
		quadruple[2].LB = quadruple[0].RB;
		quadruple[2].LT = quadruple[0].RT;
		quadruple[2].RB = quadRoot.RB;
		quadruple[2].RT = quadRoot.RT;
		quadruple[2].RT.z /= 2;

		//3 - Right Top
		quadruple[3].LB = quadruple[0].RT;
		quadruple[3].LT = quadruple[1].RT;
		quadruple[3].RB = quadruple[2].RT;
		quadruple[3].RT = quadRoot.RT;
	}
	{
		//sexdecuple
		for (UINT i = 0; i < 16; i += 4)
		{
			//0 - Left Bottom
			sexdecuple[i + 0].LB = quadruple[i / 4].LB;
			sexdecuple[i + 0].LT = quadruple[i / 4].LT / 2;
			sexdecuple[i + 0].RB = quadruple[i / 4].RB / 2;
			sexdecuple[i + 0].RT = quadruple[i / 4].RT / 2;

			//1 - Left Top
			sexdecuple[i + 1].LB = sexdecuple[i + 0].LT;
			sexdecuple[i + 1].RB = sexdecuple[i + 0].RT;
			sexdecuple[i + 1].LT = quadruple[i / 4].LT;
			sexdecuple[i + 1].RT = quadruple[i / 4].RT;
			sexdecuple[i + 1].RT.x /= 2;

			//2 - Right Bottom
			sexdecuple[i + 2].LB = sexdecuple[i + 0].RB;
			sexdecuple[i + 2].RB = quadruple[i / 4].RB;
			sexdecuple[i + 2].LT = sexdecuple[i + 0].RT;
			sexdecuple[i + 2].RT = quadruple[i / 4].RT;
			sexdecuple[i + 2].RT.z /= 2;

			//3 - Right Top
			sexdecuple[i + 3].LB = sexdecuple[i + 0].RT;
			sexdecuple[i + 3].RB = sexdecuple[i + 2].RT;
			sexdecuple[i + 3].LT = sexdecuple[i + 1].RT;
			sexdecuple[i + 3].RT = quadruple[i / 4].RT;
		}
	}
}
