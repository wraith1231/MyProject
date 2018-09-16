#include "stdafx.h"
#include "Exporter.h"

#include "Type.h"
#include "Utility.h"
#include "BoneWeights.h"

#include "../Utilities/BinaryFile.h"

Fbx::Exporter::Exporter(wstring fbxFile, bool bXna)
{
	//animation 축이 틀어져있어서 임시로 넣은것
	this->bXna = bXna;

	manager = FbxManager::Create();
	scene = FbxScene::Create(manager, "");
	ios = FbxIOSettings::Create(manager, IOSROOT);
	ios->SetBoolProp(IMP_FBX_TEXTURE, true);
	manager->SetIOSettings(ios);

	importer = FbxImporter::Create(manager, "");

	string sFile = String::ToString(fbxFile);
	bool bCheck = importer->Initialize(sFile.c_str(), -1, ios);
	assert(bCheck == true);										

	bCheck = importer->Import(scene);
	assert(bCheck == true);


	FbxAxisSystem axis = scene->GetGlobalSettings().GetAxisSystem();
	if (axis != FbxAxisSystem::DirectX)
		FbxAxisSystem::DirectX.ConvertScene(scene);

	converter = new FbxGeometryConverter(manager);
}

Fbx::Exporter::~Exporter()
{
	SAFE_DELETE(converter);

	ios->Destroy();
	importer->Destroy();
	scene->Destroy();
	manager->Destroy();
}

void Fbx::Exporter::ExportMaterial(wstring saveFolder, wstring saveName)
{
	int count = scene->GetMaterialCount();
	
	for (int i = 0; i < count; i++)
	{
		FbxSurfaceMaterial* fbxMaterial = scene->GetMaterial(i);
		
		FbxMaterial* material = new FbxMaterial();
		material->Name = fbxMaterial->GetName();

		if (fbxMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId) == true)
		{
			FbxSurfaceLambert* lambert = (FbxSurfaceLambert*)fbxMaterial;

			material->Ambient = Utility::ToColor(lambert->Ambient, lambert->AmbientFactor);
			material->Diffuse = Utility::ToColor(lambert->Diffuse, lambert->DiffuseFactor);
			material->Emissive = Utility::ToColor(lambert->Emissive, lambert->EmissiveFactor);
		}

		if (fbxMaterial->GetClassId().Is(FbxSurfacePhong::ClassId) == true)
		{
			FbxSurfacePhong* phong = (FbxSurfacePhong*)fbxMaterial;

			material->Specular = Utility::ToColor(phong->Specular, phong->SpecularFactor);
			material->Shineness = (float)phong->Shininess;

		}

		FbxProperty prop;
		prop = fbxMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
		
		material->DiffuseFile = Utility::GetTextureFile(prop);

		prop = fbxMaterial->FindProperty(FbxSurfaceMaterial::sSpecular);
		material->SpecularFile = Utility::GetTextureFile(prop);

		prop = fbxMaterial->FindProperty(FbxSurfaceMaterial::sEmissive);
		material->EmissiveFile = Utility::GetTextureFile(prop);

		prop = fbxMaterial->FindProperty(FbxSurfaceMaterial::sBump);
		material->NormalFile = Utility::GetTextureFile(prop);

		materials.push_back(material);
	}

	//Load
	//////////////////////////////////////////////////////////////////////////////////////////////////
	//Save

	if (Path::ExistDirectory(saveFolder) == false)
	{
		CreateDirectory(saveFolder.c_str(), NULL);	
	}

	Json::Value root;
	for (FbxMaterial* material : materials)
	{
		Json::Value val;
		Json::SetValue(val, "Name", material->Name);

		string shaderName = "999_Mesh.hlsl";					
		Json::SetValue(val, "ShaderName", shaderName);

		Json::SetValue(val, "Ambient", material->Ambient);
		Json::SetValue(val, "Diffuse", material->Diffuse);
		Json::SetValue(val, "Specular", material->Specular);
		Json::SetValue(val, "Emissive", material->Emissive);

		Json::SetValue(val, "Shininess", material->Shineness);

		CopyTextureFile(material->DiffuseFile, saveFolder);
		Json::SetValue(val, "DiffuseFile", material->DiffuseFile);

		CopyTextureFile(material->SpecularFile, saveFolder);
		Json::SetValue(val, "SpecularFile", material->SpecularFile);
		
		CopyTextureFile(material->EmissiveFile, saveFolder);
		Json::SetValue(val, "EmissiveFile", material->EmissiveFile);
		
		CopyTextureFile(material->NormalFile, saveFolder);
		Json::SetValue(val, "NormalFile", material->NormalFile);

		root[material->Name.c_str()] = val;
		SAFE_DELETE(material);
	}

	Json::WriteData(saveFolder + saveName + L".material", &root);
}

void Fbx::Exporter::ExportMesh(wstring saveFolder, wstring saveName)
{
	ReadBoneData(scene->GetRootNode(), -1, -1);
	ReadSkinData();

	WriteMeshData(saveFolder, saveName);
}

void Fbx::Exporter::ExportAnimation(wstring saveFolder, wstring saveName)
{
	ReadAnimation();

	WriteAnimaiton(saveFolder, saveName);
}

void Fbx::Exporter::ExportAnimation(wstring saveFolder, wstring saveName, bool xmlstyle)
{
	ReadAnimation();

	if (xmlstyle == true)
		WriteAnimationXml(saveFolder, saveName);
	else
		WriteAnimaiton(saveFolder, saveName);
}

void Fbx::Exporter::CopyTextureFile(string & textureFile, wstring saveFolder)
{
	if (textureFile.length() < 1)
		return;

	wstring file = String::ToWString(textureFile);
	wstring fileName = Path::GetFileName(file);

	if (Path::ExistFile(textureFile) == true)
	{
		CopyFile(file.c_str(), (saveFolder + fileName).c_str(), FALSE);
	}

	textureFile = String::ToString(fileName);
}

void Fbx::Exporter::ReadBoneData(FbxNode * node, int index, int parent)
{
	FbxNodeAttribute* attribute = node->GetNodeAttribute();

	if (attribute != NULL)
	{
		FbxNodeAttribute::EType nodeType = attribute->GetAttributeType();

		bool bCheck = false;
		bCheck |= (nodeType == FbxNodeAttribute::eSkeleton);
		bCheck |= (nodeType == FbxNodeAttribute::eMesh);
		bCheck |= (nodeType == FbxNodeAttribute::eNull);
		bCheck |= (nodeType == FbxNodeAttribute::eMarker);

		if (bCheck == true)
		{
			FbxBoneData* bone = new FbxBoneData;
			bone->Index = index;
			bone->Parent = parent;
			bone->Name = node->GetName();

			D3DXMatrixIdentity(&bone->Transform);
			D3DXMatrixIdentity(&bone->AbsoluteTransform);

			//bone->Transform = Utility::ToMatrix(node->EvaluateLocalTransform(), bXna);
			//bone->AbsoluteTransform = Utility::ToMatrix(node->EvaluateGlobalTransform(), bXna);
			//bone->Transform = Utility::XAxisPIRotate() * Utility::ToMatrix(node->EvaluateLocalTransform());
			//bone->AbsoluteTransform = Utility::XAxisPIRotate() * Utility::ToMatrix(node->EvaluateGlobalTransform());
			bone->Transform = Utility::ToMatrix(node->EvaluateLocalTransform());
			bone->AbsoluteTransform = Utility::ToMatrix(node->EvaluateGlobalTransform());

			boneDatas.push_back(bone);

			if (nodeType == FbxNodeAttribute::eMesh)
			{
				converter->Triangulate(attribute, true, true);

				ReadMeshData(node, index);
			}
		} // bCheck == true

	} //if attribute

	for (int i = 0; i < node->GetChildCount(); i++)
	{
		ReadBoneData(node->GetChild(i), boneDatas.size(), index);
	}
}

void Fbx::Exporter::ReadMeshData(FbxNode * node, int parentBone)
{

	FbxMesh* mesh = node->GetMesh();
			
	vector<FbxVertex*> vertices;
	
	for (int p = 0; p < mesh->GetPolygonCount(); p++)
	{
		int vertexInPolygon = mesh->GetPolygonSize(p);
		assert(vertexInPolygon == 3);

		//for (int vi = vertexInPolygon - 1; vi >= 0; vi--)
		for (int vi = 0; vi < vertexInPolygon; vi++)
		{
			FbxVertex* vertex = new FbxVertex;
			int cpIndex = mesh->GetPolygonVertex(p, vi);

			vertex->ControlPoint = cpIndex;

			FbxVector4 position = mesh->GetControlPointAt(cpIndex);
			D3DXVECTOR3 temp = Utility::ToVector3(position);
			//D3DXVec3TransformCoord(&vertex->Vertex.position, &temp, &Utility::Negative(bXna));
			D3DXVec3TransformCoord(&vertex->Vertex.position, &temp, &Utility::XAxisPIRotate());
			//vertex->Vertex.position = temp;
			FbxVector4 normal;
			mesh->GetPolygonVertexNormal(p, vi, normal);
			normal.Normalize();
			temp = Utility::ToVector3(normal);
			//사실 이거 w가 0이라 곱해지는 값이 없어서 그냥 coord로 해줘도 상관은 없음
			//D3DXVec3TransformNormal(&vertex->Vertex.normal, &temp, &Utility::Negative(bXna));
			D3DXVec3TransformNormal(&vertex->Vertex.normal, &temp, &Utility::XAxisPIRotate());
			//vertex->Vertex.normal = temp;

			vertex->MaterialName = Utility::GetMaterialName(mesh, p, cpIndex);

			int uvIndex = mesh->GetTextureUVIndex(p, vi);
			vertex->Vertex.uv = Utility::GetUv(mesh, cpIndex, uvIndex);

			vertices.push_back(vertex);
		}	//vi

	}	//p
	
	FbxMeshData* meshData = new FbxMeshData();
	meshData->Name = node->GetName();				
	meshData->ParentBone = parentBone;
	meshData->Vertices = vertices;
	meshData->Mesh = mesh;
	meshDatas.push_back(meshData);
}

void Fbx::Exporter::ReadSkinData()
{
	for (FbxMeshData* meshData : meshDatas)
	{
		FbxMesh* mesh = meshData->Mesh;

		///////////////////////////////////////////////////////////////////////////////////////

		int deformerCount = mesh->GetDeformerCount();
		vector<FbxBoneWeights> boneWeights(mesh->GetControlPointsCount(), FbxBoneWeights());

		for (int i = 0; i < deformerCount; i++)
		{
			FbxDeformer* deformer = mesh->GetDeformer(i, FbxDeformer::eSkin);

			FbxSkin* skin = reinterpret_cast<FbxSkin *>(deformer);
			if (skin == NULL) continue;

			for (int clusterIndex = 0; clusterIndex < skin->GetClusterCount(); clusterIndex++)
			{
				FbxCluster* cluster = skin->GetCluster(clusterIndex);
				assert(cluster->GetLinkMode() == FbxCluster::eNormalize);

				string linkName = cluster->GetLink()->GetName();
				UINT boneIndex = GetBoneIndexByName(linkName);

				FbxAMatrix transform;
				FbxAMatrix linkTransform;
				
				cluster->GetTransformMatrix(transform);
				cluster->GetTransformLinkMatrix(linkTransform);

				//boneDatas[boneIndex]->Transform = Utility::XAxisPIRotate() * Utility::ToMatrix(transform);
				//boneDatas[boneIndex]->AbsoluteTransform = Utility::XAxisPIRotate() * Utility::ToMatrix(linkTransform);
				boneDatas[boneIndex]->Transform = Utility::ToMatrix(transform);
				boneDatas[boneIndex]->AbsoluteTransform = Utility::ToMatrix(linkTransform);

				//D3DXMatrixIdentity(&boneDatas[boneIndex]->Transform);
				//D3DXMatrixIdentity(&boneDatas[boneIndex]->AbsoluteTransform);

				for (int indexCount = 0; indexCount < cluster->GetControlPointIndicesCount(); indexCount++)
				{
					int temp = cluster->GetControlPointIndices()[indexCount];
					double* weights = cluster->GetControlPointWeights();

					boneWeights[temp].AddBoneWeight(boneIndex, (float)weights[indexCount]);
				}
			}//for(clusterIndex)
		}//for(joints)

		for (size_t i = 0; i < boneWeights.size(); i++)
			boneWeights[i].Normalize();


		for (FbxVertex* vertex : meshData->Vertices)
		{
			int cpIndex = vertex->ControlPoint;

			FbxBlendWeight weights;
			boneWeights[cpIndex].GetBlendWeights(weights);
			vertex->Vertex.blendIndices = weights.Indices;
			vertex->Vertex.blendWeights = weights.Weights;
		}

		for (int i = 0; i < scene->GetMaterialCount(); i++)
		{
			FbxSurfaceMaterial* material = scene->GetMaterial(i);
			string materialName = material->GetName();

			vector<FbxVertex *> gather;
			for (FbxVertex* temp : meshData->Vertices)
			{
				if (temp->MaterialName == materialName)
					gather.push_back(temp);
			}
			if (gather.size() < 1) continue;


			FbxMeshPartData* meshPart = new FbxMeshPartData();
			meshPart->MaterialName = materialName;

			for (FbxVertex* temp : gather)
			{
				ModelVertexType vertex;
				vertex = temp->Vertex;

				meshPart->Vertices.push_back(vertex);
				meshPart->Indices.push_back(meshPart->Indices.size());
			}
			meshData->MeshParts.push_back(meshPart);
		}
	}//for(MeshData)
}

void Fbx::Exporter::WriteMeshData(wstring saveFolder, wstring saveName)
{
	if (Path::ExistDirectory(saveFolder) == false)
	{
		//Api Function
		CreateDirectory(saveFolder.c_str(), NULL);	
	}
	
	BinaryWriter* w = new BinaryWriter();
	w->Open(saveFolder + saveName + L".mesh");

	//////////////////////////////////////////////////////////////////////////////////////////////////

	{
		w->UInt(boneDatas.size());
		for (FbxBoneData* bone : boneDatas)
		{
			w->Int(bone->Index);
			w->String(bone->Name);
			
			w->Int(bone->Parent);
			w->Matrix(bone->Transform);
			w->Matrix(bone->AbsoluteTransform);

			SAFE_DELETE(bone);
		}

	}

	//////////////////////////////////////////////////////////////////////////////////////////////////
	{
		w->UInt(meshDatas.size());
		for (FbxMeshData* meshData : meshDatas)
		{
			w->String(meshData->Name);
			w->Int(meshData->ParentBone);
			
			w->UInt(meshData->MeshParts.size());
			for (FbxMeshPartData* part : meshData->MeshParts)
			{
				w->String(part->MaterialName);
				
				w->UInt(part->Vertices.size());
				w->Byte(&part->Vertices[0], sizeof(ModelVertexType) * part->Vertices.size());

				w->UInt(part->Indices.size());
				w->Byte(&part->Indices[0], sizeof(UINT) * part->Indices.size());
				
				SAFE_DELETE(part);
			}
			SAFE_DELETE(meshData);
		}

	} //

	w->Close();
	SAFE_DELETE(w);
}

UINT Fbx::Exporter::GetBoneIndexByName(string name)
{
	for (size_t i = 0; i < boneDatas.size(); i++)
	{
		if (boneDatas[i]->Name == name)
			return i;
	}

	return -1;
}

void Fbx::Exporter::ReadAnimation()
{
	FbxTime::EMode mode = scene->GetGlobalSettings().GetTimeMode();
	float frameRate = (float)FbxTime::GetFrameRate(mode);

	for (int i = 0; i < importer->GetAnimStackCount(); i++)
	{
		FbxAnimation* animation = new FbxAnimation();
		animation->FrameRate = frameRate;

		FbxTakeInfo* takeInfo = importer->GetTakeInfo(i);
		animation->Name = takeInfo->mName.Buffer();

		FbxTimeSpan span = takeInfo->mLocalTimeSpan;
		int start = (int)span.GetStart().GetFrameCount();
		int end = (int)span.GetStop().GetFrameCount();
		if (start < end)
		{
			ReadAnimation(animation, scene->GetRootNode(), start, end);
		}//if(start<end)
		
		animation->TotalFrame = (int)animation->KeyFrames[0]->Transforms.size();

		animDatas.push_back(animation);
	}//for(i)
}

void Fbx::Exporter::ReadAnimation(FbxAnimation * animation, FbxNode * node, int start, int end)
{
	FbxNodeAttribute* attribute = node->GetNodeAttribute();

	if (attribute != NULL)
	{
		FbxNodeAttribute::EType nodeType = attribute->GetAttributeType();
		if (nodeType == FbxNodeAttribute::eSkeleton)
		{
			FbxKeyframe* keyFrame = new FbxKeyframe();
			keyFrame->BoneName = node->GetName();
			
			for(int i = start; i <= end; i++)
			{
				FbxTime animationTime;
				animationTime.SetFrame(i);

				FbxAMatrix matrix = node->EvaluateLocalTransform(animationTime);	//아무것도 않넣으면 가장 기본적인게 나오고 시간을 넣으면 해당 시간으로 변환된 로컬 transform이 넘어온다
				//D3DXMATRIX transform = Utility::ToMatrix(matrix, bXna);
				//D3DXMATRIX transform = Utility::XAxisPIRotate() * Utility::ToMatrix(matrix) * Utility::XAxisPIRotate();
				D3DXMATRIX transform = Utility::ToMatrix(matrix);

				FbxKeyFrameData data;

				D3DXMatrixDecompose(&data.Scale, &data.Rotation, &data.Translate, &transform);
				//data.Transform = transform;
				//data.Translate = D3DXVECTOR3(transform._41, transform._42, transform._43);
				//
				//D3DXQuaternionRotationMatrix(&data.Rotation, &transform);
				//
				//FbxVector4 scale = node->EvaluateLocalScaling(animationTime);
				//data.Scale = Utility::ToVector3(scale);

				keyFrame->Transforms.push_back(data);
			}

			animation->KeyFrames.push_back(keyFrame);
		}//if(nodeType)

	}//if(attribute)

	for (int i = 0; i < node->GetChildCount(); i++)
	{
		ReadAnimation(animation, node->GetChild(i), start, end);
	}
}

void Fbx::Exporter::WriteAnimaiton(wstring saveFolder, wstring saveName)
{
	if (Path::ExistDirectory(saveFolder) == false)
	{
		CreateDirectory(saveFolder.c_str(), NULL);
	}

	BinaryWriter* w = new BinaryWriter();
	w->Open(saveFolder + saveName + L".anim");

	w->UInt(animDatas.size());
	for (FbxAnimation* animation : animDatas)
	{
		w->String(animation->Name);

		w->Int(animation->TotalFrame);
		w->Float(animation->FrameRate);

		w->UInt(animation->KeyFrames.size());
		for (FbxKeyframe* frame : animation->KeyFrames)
		{
			w->String(frame->BoneName);

			w->UInt(frame->Transforms.size());
			w->Byte(&frame->Transforms[0], sizeof(FbxKeyFrameData) * frame->Transforms.size());

			SAFE_DELETE(frame);
		}

		SAFE_DELETE(animation);
	}

	w->Close();
	SAFE_DELETE(w);
}

void Fbx::Exporter::WriteAnimationXml(wstring saveFolder, wstring saveName)
{
	if (Path::ExistDirectory(saveFolder) == false)
		CreateDirectory(saveFolder.c_str(), NULL);

	BinaryWriter* w = new BinaryWriter();
	w->Open(saveFolder + saveName + L".anim");

	w->UInt(animDatas[0]->KeyFrames.size());
	float duration = animDatas[0]->TotalFrame / animDatas[0]->FrameRate;
	w->Float(duration);

	for (FbxKeyframe* keyframe : animDatas[0]->KeyFrames)
	{
		w->String(keyframe->BoneName);

		w->UInt(keyframe->Transforms.size());
		vector<D3DXVECTOR3> translations;
		vector<D3DXQUATERNION> rotations;
		vector<D3DXVECTOR3> scales;

		for (FbxKeyFrameData data : keyframe->Transforms)
		{
			translations.push_back(data.Translate);
			rotations.push_back(data.Rotation);
			scales.push_back(data.Scale);
		}
		keyframe->Transforms.clear();

		w->Float(duration);
		w->Float(duration / animDatas[0]->TotalFrame);

		UINT size = 0;
		size = translations.size();
		w->UInt(size);
		w->Byte(&translations[0], sizeof(D3DXVECTOR3) * size);
		
		size = rotations.size();
		w->UInt(size);
		w->Byte(&rotations[0], sizeof(D3DXQUATERNION) * size);

		size = scales.size();
		w->UInt(size);
		w->Byte(&scales[0], sizeof(D3DXVECTOR3) * size);

		size = 0;
		w->UInt(0);

		SAFE_DELETE(keyframe);
	}

	w->Close();
	SAFE_DELETE(w);
}
