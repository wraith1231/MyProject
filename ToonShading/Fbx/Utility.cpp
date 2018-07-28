#include "stdafx.h"
#include "Utility.h"

D3DXMATRIX Fbx::Utility::Negative(bool bXna)
{
	D3DXMATRIX scale;
	D3DXMatrixScaling(&scale, -1.0f, 1.0f, 1.0f);

	if(bXna == true)
	{
		D3DXMATRIX rotationX;
		D3DXMatrixRotationX(&rotationX, -Math::ToRadian(90));

		D3DXMATRIX rotationY;
		D3DXMatrixRotationY(&rotationY, Math::ToRadian(180));
		
		return scale * rotationX * rotationY;
	}
	else
	{
		D3DXMATRIX rotation;
		D3DXMatrixRotationY(&rotation, Math::ToRadian(180));

		return scale * rotation;
	}
}

D3DXVECTOR2 Fbx::Utility::ToVector2(FbxVector2 & vec)
{
	return D3DXVECTOR2((float)vec.mData[0], (float)vec.mData[1]);
}

D3DXVECTOR3 Fbx::Utility::ToVector3(FbxVector4 & vec)
{
	return D3DXVECTOR3((float)vec.mData[0], (float)vec.mData[1], (float)vec.mData[2]);
}

D3DXCOLOR Fbx::Utility::ToColor(FbxVector4 & vec)
{
	return D3DXCOLOR((float)vec.mData[0], (float)vec.mData[1], (float)vec.mData[2], 1);
}

D3DXCOLOR Fbx::Utility::ToColor(FbxPropertyT<FbxDouble3>& vec, FbxPropertyT<FbxDouble>& factor)
{
	FbxDouble3 color = vec;
	FbxDouble temp = factor;

	D3DXCOLOR result;
	result.r = (float)color.mData[0];
	result.g = (float)color.mData[1];
	result.b = (float)color.mData[2];
	result.a = (float)factor;

	return result;
}

D3DXMATRIX Fbx::Utility::ToMatrix(FbxAMatrix & matrix)
{
	FbxVector4 r1 = matrix.GetRow(0);
	FbxVector4 r2 = matrix.GetRow(1);
	FbxVector4 r3 = matrix.GetRow(2);
	FbxVector4 r4 = matrix.GetRow(3);

	D3DXMATRIX mat = D3DXMATRIX
	(
		(float)r1.mData[0], (float)r1.mData[1], (float)r1.mData[2], (float)r1.mData[3],
		(float)r2.mData[0], (float)r2.mData[1], (float)r2.mData[2], (float)r2.mData[3],
		(float)r3.mData[0], (float)r3.mData[1], (float)r3.mData[2], (float)r3.mData[3],
		(float)r4.mData[0], (float)r4.mData[1], (float)r4.mData[2], (float)r4.mData[3]
	);

	return mat;
}

D3DXMATRIX Fbx::Utility::ToMatrix(FbxAMatrix & matrix, bool bXna)
{
	FbxVector4 r1 = matrix.GetRow(0);
	FbxVector4 r2 = matrix.GetRow(1);
	FbxVector4 r3 = matrix.GetRow(2);
	FbxVector4 r4 = matrix.GetRow(3);

	D3DXMATRIX mat = D3DXMATRIX
	(
		(float)r1.mData[0], (float)r1.mData[1], (float)r1.mData[2], (float)r1.mData[3],
		(float)r2.mData[0], (float)r2.mData[1], (float)r2.mData[2], (float)r2.mData[3],
		(float)r3.mData[0], (float)r3.mData[1], (float)r3.mData[2], (float)r3.mData[3],
		(float)r4.mData[0], (float)r4.mData[1], (float)r4.mData[2], (float)r4.mData[3]
	);

	return Negative(bXna) * mat * Negative(bXna);
}

string Fbx::Utility::GetTextureFile(FbxProperty & prop)
{
	if (prop.IsValid() == true)
	{
		if (prop.GetSrcObjectCount() > 0)
		{
			FbxFileTexture* texture = prop.GetSrcObject<FbxFileTexture>();
			if (texture != NULL)
			{
				return string(texture->GetFileName());
			}
		}
	}

	return "";
}

string Fbx::Utility::GetMaterialName(FbxMesh * mesh, int polygonIndex, int cpIndex)
{
	FbxNode* node = mesh->GetNode();
	if (node == NULL) return "";

	FbxLayerElementMaterial* material = mesh->GetLayer(0)->GetMaterials();

	if (material == NULL)
		return "";

	FbxLayerElement::EMappingMode mappingMode = material->GetMappingMode();
	FbxLayerElement::EReferenceMode refMode = material->GetReferenceMode();

	int mappingIndex = -1;
	switch (mappingMode)
	{
	case FbxLayerElement::eAllSame:
		mappingIndex = 0;
		break;
	case FbxLayerElement::eByPolygon:
		mappingIndex = polygonIndex;
		break;
	case FbxLayerElement::eByControlPoint:
		mappingIndex = cpIndex;
		break;
	case FbxLayerElement::eByPolygonVertex:
		mappingIndex = polygonIndex * 3;
		break;
	default:
		assert(false);
		break;
	}

	FbxSurfaceMaterial* findMaterial = NULL;
	if (refMode == FbxLayerElement::eDirect)
	{
		if (mappingIndex < node->GetMaterialCount())
			findMaterial = node->GetMaterial(mappingIndex);
	}
	else if (refMode == FbxLayerElement::eIndexToDirect)
	{
		FbxLayerElementArrayTemplate<int>& indexArray = material->GetIndexArray();
		if (mappingIndex < indexArray.GetCount())
		{
			int tempIndex = indexArray.GetAt(mappingIndex);

			if (tempIndex < node->GetMaterialCount())
				findMaterial = node->GetMaterial(tempIndex);
		}
	}

	if (findMaterial == NULL) return "";

	return findMaterial->GetName();
}

D3DXVECTOR2 Fbx::Utility::GetUv(FbxMesh * mesh, int cpIndex, int uvIndex)
{
	D3DXVECTOR2 result = D3DXVECTOR2(0, 0);

	FbxLayerElementUV* uv = mesh->GetLayer(0)->GetUVs();
	if (uv == NULL)
		return result;

	FbxLayerElementUV::EMappingMode mappingMode = uv->GetMappingMode();
	FbxLayerElement::EReferenceMode refMode = uv->GetReferenceMode();
	switch (mappingMode)
	{
	case FbxLayerElement::eByControlPoint:
		{
			{
				if (refMode == FbxLayerElement::eDirect)
				{
					//직접적으로 소유한 경우
					result.x = (float)uv->GetDirectArray().GetAt(cpIndex).mData[0];
					result.y = (float)uv->GetDirectArray().GetAt(cpIndex).mData[1];
				}
				else if (refMode == FbxLayerElement::eIndexToDirect)
				{
					int index = uv->GetIndexArray().GetAt(cpIndex);

					result.x = (float)uv->GetDirectArray().GetAt(index).mData[0];
					result.y = (float)uv->GetDirectArray().GetAt(index).mData[1];
				}
			}

			break;
		}
	case FbxLayerElement::eByPolygonVertex:
		{
		//해당 정점의 번호가 uv번호가 되는 경우 - polygon이 아닌 polygon 안의 vertex 번호임
			{
				//reference mode 구분할 필요 없음
				//그냥 uvIndex면 됨
				result.x = (float)uv->GetDirectArray().GetAt(uvIndex).mData[0];
				result.y = (float)uv->GetDirectArray().GetAt(uvIndex).mData[1];
			}

			break;
		}
	}

	result.y = 1.0f - result.y;		//사각형 평면일떈 uv 뒤집혀지면 쉽게 느껴지는데 모델이면 전혀 안느껴짐
									//그래서 특히 주의해야함

	return result;
}
