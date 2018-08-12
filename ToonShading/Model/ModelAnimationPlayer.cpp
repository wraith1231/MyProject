#include "stdafx.h"
#include "ModelAnimationPlayer.h"

#include "Model.h"
#include "ModelBone.h"
#include "ModelMesh.h"
#include "ModelAnimationClip.h"

ModelAnimPlayer::ModelAnimPlayer(Model * model, wstring animName)
	: model(model)
	, mode(Mode::Play)
	, currentKeyFrame(0)
	, frameTime(0.0f), keyframeFactor(0.0f)
	, nextKeyFrame(0)
	, bUseSlerp(true)
	, animName(animName)
{
	shader = new Shader(Shaders + L"999_Animation.hlsl");
	shader2 = new Shader(Shaders + L"999_Animation.hlsl", "VS_Normal", "PS_Normal");
	shader3 = new Shader(Shaders + L"999_Animation.hlsl", "VS_Depth", "PS_Depth");
	//shader = new Shader(Shaders + L"999_Mesh.hlsl");
	
	vector<Material*>& materials = model->Materials();
	for (Material* material : materials)
		material->SetShader(shader);

	UINT sizee = model->Clips(animName).size();
	currentClip = model->Clips(animName)[sizee-1];
	//currentClip = model->Clips(animName)[0];

	//ModelBone* root = model->Bone(1);
	//GatherBones(bones, root);

	skinTransform.assign(model->BoneCount(), D3DXMATRIX());
	boneAnimation.assign(model->BoneCount(), D3DXMATRIX());
}

ModelAnimPlayer::~ModelAnimPlayer()
{
	SAFE_DELETE(shader);
	SAFE_DELETE(shader2);
	SAFE_DELETE(shader3);
}

void ModelAnimPlayer::Update()
{
	if (currentClip == NULL)
		return;
	else if (mode == Mode::Pause)
		return;
	else if (mode == Mode::Stop)
	{
		if (currentKeyFrame != 0)
		{currentKeyFrame = 0;
		frameTime = 0.0f;
		keyframeFactor = 0.0f;
		nextKeyFrame = 0;
	
			UpdateBone();
			model->Buffer()->SetBones(&skinTransform[0], skinTransform.size());
		}
		
	
		return;
	}

	UpdateTime();
	UpdateBone();

	model->Buffer()->SetBones(&skinTransform[0], skinTransform.size());

}

void ModelAnimPlayer::NormalRender()
{
	model->Buffer()->SetVSBuffer(2);

	vector<Material*>& materials = model->Materials();
	for (Material* material : materials)
		material->SetShader(shader2);
	for (ModelMesh* mesh : model->Meshes())
		mesh->Render();
}

void ModelAnimPlayer::DepthRender()
{
	model->Buffer()->SetVSBuffer(2);

	vector<Material*>& materials = model->Materials();
	for (Material* material : materials)
		material->SetShader(shader3);
	for (ModelMesh* mesh : model->Meshes())
		mesh->Render();
}

void ModelAnimPlayer::Render()
{
	model->Buffer()->SetVSBuffer(2);

	vector<Material*>& materials = model->Materials();
	for (Material* material : materials)
		material->SetShader(shader);
	for (ModelMesh* mesh : model->Meshes())
		mesh->Render();
}

float ModelAnimPlayer::GetFrameTime()
{
	return (1 / currentClip->FrameRate()) * currentKeyFrame + frameTime;
}

float ModelAnimPlayer::GetFrameRate()
{
	return currentClip->FrameRate();
}

void ModelAnimPlayer::SetCurrentKeyFrame(float time)
{
	float rate = 1 / currentClip->FrameRate();

	float x = time / rate;
	int sec = static_cast<int>(x);
	float frac = time - rate * sec;

	currentKeyFrame = sec;
	nextKeyFrame = currentKeyFrame;
	frameTime = frac;
}

int ModelAnimPlayer::GetTotalFrame()
{
	return currentClip->TotalFrame();
}

void ModelAnimPlayer::PostRender()
{
	ImGui::Begin("player");

	{
		//int num = (int)animPlayer->GetMode();
		int num = (int)mode;
		ImGui::SliderInt("0. Play 1.Pause 2.Stop", &num, 0, 2);
		mode = (Mode)num;

		float x = GetFrameTime();
		ImGui::SliderFloat("Frame", &x, 0, GetTotalFrame() / GetFrameRate());
		SetCurrentKeyFrame(x);
	}

	ImGui::End();
}

void ModelAnimPlayer::UpdateTime()
{
	frameTime += Time::Delta();

	float invFrameRate = 1.0f / currentClip->FrameRate();
	while (frameTime > invFrameRate)
	{
		int keyframeCount = currentClip->TotalFrame();

		currentKeyFrame = (currentKeyFrame + 1) % keyframeCount;
		nextKeyFrame = (currentKeyFrame + 1) % keyframeCount;

		frameTime -= invFrameRate;
	}
	if (nextKeyFrame == currentClip->TotalFrame() - 1)
		isEnd = true;

	keyframeFactor = frameTime / invFrameRate;
}

void ModelAnimPlayer::UpdateBone()
{
	for (UINT i = 0; i < model->BoneCount(); i++)
	{
		ModelBone* bone = model->Bone(i);

		D3DXMATRIX matAnimation;
		D3DXMatrixIdentity(&matAnimation);
		D3DXMATRIX matParentAnimation;

		D3DXMATRIX matInvBindPos = bone->AbsoluteTransform();

		//D3DXVECTOR3 temp = D3DXVECTOR3(matInvBindPos._21, matInvBindPos._22, matInvBindPos._23);
		//matInvBindPos._21 = matInvBindPos._31;
		//matInvBindPos._22 = matInvBindPos._32;
		//matInvBindPos._23 = matInvBindPos._33;
		//matInvBindPos._31 = temp.x;
		//matInvBindPos._32 = temp.y;
		//matInvBindPos._33 = temp.z;
		D3DXMatrixInverse(&matInvBindPos, NULL, &matInvBindPos);

		ModelKeyframe* frame = currentClip->Keyframe(bone->Name());
		if (frame == NULL)
		{
			D3DXMatrixIdentity(&boneAnimation[i]);
			D3DXMatrixIdentity(&skinTransform[i]);
			continue;
		}

		if (bUseSlerp == true)
		{
			D3DXMATRIX S, R, T;

			ModelKeyframeData current = frame->Datas[currentKeyFrame];
			ModelKeyframeData next = frame->Datas[nextKeyFrame];

			D3DXVECTOR3 s1 = current.Scale;
			D3DXVECTOR3 s2 = next.Scale;

			D3DXVECTOR3 s;
			D3DXVec3Lerp(&s, &s1, &s2, keyframeFactor);
			D3DXMatrixScaling(&S, s.x, s.y, s.z);


			D3DXQUATERNION q1 = current.Rotation;
			D3DXQUATERNION q2 = next.Rotation;

			D3DXQUATERNION q;
			D3DXQuaternionSlerp(&q, &q1, &q2, keyframeFactor);
			D3DXMatrixRotationQuaternion(&R, &q);

			D3DXVECTOR3 t1 = current.Translation;
			D3DXVECTOR3 t2 = next.Translation;

			D3DXVECTOR3 t;
			D3DXVec3Lerp(&t, &t1, &t2, keyframeFactor);
			D3DXMatrixTranslation(&T, t.x, t.y, t.z);

			matAnimation = S * R * T;
		}
		else
		{
			matAnimation = frame->Datas[currentKeyFrame].Transform;
		}

		int parentIndex = bone->ParentIndex() - 1;
		if (parentIndex < 0)
			D3DXMatrixIdentity(&matParentAnimation);
			//model->Root()->Transform();
		else
			matParentAnimation = boneAnimation[parentIndex];

		boneAnimation[i] = matAnimation *matParentAnimation;	//�̰� �������� ��
		skinTransform[i] = matInvBindPos * boneAnimation[i];
		//skinTransform[i] = boneAnimation[i];
	}

	/*
	if(parentIndex < 0) �� ��� root�ε�, identity ���� root�� global transform��
	�־���� �� - �����϶� �װ� �ݿ���Ű���� root�� ����ִ°� ���� ����
	����� ���� ���Ұ��Ӥ�

	animation�߿� slash�� ����, paladin�� ���ʿ��� �ΰ� ���� ���� ĳ���� �ϳ� �α�,
	���� �̵��ؼ� ������ ��, Į �浹 üũ�� ����� ��, ������� ĸ�� �ϳ� �ΰ�
	Į���� ĸ�� �ּ� ĸ���� ĸ���� �浹, ĸ���� �� �ΰ��� ���� �ϳ��� ����
	ĸ�� �׷����� �浹 ����� �Ǵ��� Ȯ���ϰ� ���� - ������ �´� ��ǵ� ���ؾ���
	�� �Ʒ� ���� �¿츸�ϰ� ĸ�� �׸��°͵� ����� �浹���� �ľ��� �� ����

	meshpart �θ��� meshpart�� x�� -, + �ִ� ������ ���ϰ� ������ �������� ĸ�� �׸���
	*/
}