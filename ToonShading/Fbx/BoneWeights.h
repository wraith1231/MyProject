#pragma once
#include "stdafx.h"

struct FbxBlendWeight
{
	D3DXVECTOR4 Indices = D3DXVECTOR4(0, 0, 0, 0);
	D3DXVECTOR4 Weights = D3DXVECTOR4(0, 0, 0, 0);

	//0 - x, 1 - y, 2 - z, 3 - w
	void Set(int index, int boneIndex, float weight)
	{
		float x = (float)boneIndex;
		float w = weight;

		switch (index)
		{
		case 0: Indices.x = x; Weights.x = w; break;
		case 1: Indices.y = x; Weights.y = w; break;
		case 2: Indices.z = x; Weights.z = w; break;
		case 3: Indices.w = x; Weights.w = w; break;
		}
	}
};

//�ؿ� �ִ°� ���� ���� �����Ѵ�
struct FbxBoneWeights
{
private:
	typedef pair<int, float> Pair;
	vector<Pair> BoneWeights;

public:
	void AddBoneWeight(int boneIndex, float boneWeight)
	{
		if (boneWeight <= 0.0f) return;

		bool bAdd = false;
		vector<Pair>::iterator iter = BoneWeights.begin();
		for (; iter != BoneWeights.end(); iter++)
		{
			if (boneWeight > iter->second)
			{
				BoneWeights.insert(iter, Pair(boneIndex, boneWeight));
				bAdd = true;

				break;
			}
		}	//for(iter)

		if (bAdd == false)
			BoneWeights.push_back(Pair(boneIndex, boneWeight));
		//�갡 4���� �����µ� 0.2, 0.3�� �־��� 0.5�� ������ .2, .3 �н��ϰ� .5������°�
		//.4�� ����� .5�տ� �ΰ� �϶� ���
		//insert �������� �׳� �������� �־�ΰ�
	}//AddBoneWeight

	void AddBoneWeights(FbxBoneWeights& boneWeights)
	{
		vector<Pair>& temp = boneWeights.BoneWeights;

		for (size_t i = 0; i < temp.size(); i++)
		{
			AddBoneWeight(temp[i].first, temp[i].second);
		}
	}//AddBoneWeights

	void Normalize()
	{
		float totalWeight = 0.0f;

		int i = 0;
		vector<Pair>::iterator it = BoneWeights.begin();
		while (it != BoneWeights.end())
		{
			if (i < 4)
			{
				totalWeight += it->second;
				++i; ++it;
			}
			else
			{
				it = BoneWeights.erase(it);
			}
		}//while(it)

		float scale = 1.0f / totalWeight;
		
		it = BoneWeights.begin();
		while (it != BoneWeights.end())
		{
			it->second *= scale;
			it++;
		}//while(it)
	}//Normalize

	//return�� &�� ���°� ���� ������, ���� �����Ͱ� & ������ �����鼭
	//�����͸� ���� �ʵ��� �ǰ��ϰ� ������ �������� �ϴ� ��ݵ��� �ȹٱ���
	//� ���� �Ҵ��ؼ� ������ �� ���� ���� �����
	//A* a = NULL;�� �����������ؼ� �ѱ�� ��(�� �� �Ѱ��ִ°� A** a)
	//�� ���� ����� A& a�� �Ѱ��ְ� a ��ȭ���ָ� �����Ҵ� �ʿ���� �̿� ������
	//�� *�� �μ����� *�� �ް� &�� �Ѱ��ָ� NULLüũ�� ������ ������ �����ϰ�
	//NULL���� �츰 0���� define�Ǿ��־� �����Ⱚ�� Ȯ�� �Ұ�, �׷��� nullptr
	//üũ�� �������, �ٵ� &�� ������ NULL üũ�� ���� �ʿ䰡 ����, �׳� �ű⿡
	//�������� �Ҵ簡���ϴϱ� ���� �߻� Ȯ���� ���� ������
	//08�� ���� �ؼ� ���׷��̵�� �͵��� ���� &������� �ٲ�°� Ʈ����
	//�𸮾� �� ���۷����� C#�� xna�غ��� ������ �� &��
	//���� *������� ������ ���� �߻����� ������ �����Ⱚ�� �Ѿ������
	//ũ��Ŀ�� �޸� ���� ���� �ٲ㼭 ���� ������Ű�� ������ �߻���ų �� ����
	//�ͽ��ø����̶�� ��ŷ�����, �����ʿ��� ���� �߻��ϴ� ������ �ƴ�
	//�׷��� ���� c��� �Լ���(strcpy����)�� �� ������ �Լ����(strcpy_s����) ����ϵ���
	//�ǰ��ϰ� �ִ� - Ư�� ����üũ �κп��� ������ ����ϸ� ū ������ �߻��� �� ����
	void GetBlendWeights(FbxBlendWeight& blendWeights)
	{
		int i = 0;
		vector<Pair>::iterator it = BoneWeights.begin();

		while (it != BoneWeights.end())
		{
			blendWeights.Set(i, it->first, it->second);
			i++; it++;

			if (i >= 4)
				return;
		}//while(it)

	}//GetBlendWeights
};