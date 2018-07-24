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

//밑에 있는게 위의 것을 관리한다
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
		//얘가 4개가 들어오는데 0.2, 0.3이 있었고 0.5가 들어오면 .2, .3 패스하고 .5넣으라는거
		//.4가 들어모면 .5앞에 두고 하란 얘기
		//insert 없었으면 그냥 마지막에 넣어두고
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

	//return도 &로 쓰는게 좋은 습관임, 원래 포인터가 & 개념이 나오면서
	//포인터를 쓰지 않도록 권고하고 있지만 예전부터 하던 양반들이 안바까줌
	//어떤 값을 할당해서 가져올 때 가장 쉬운 방법은
	//A* a = NULL;로 이차포인터해서 넘기는 것(이 떄 넘겨주는건 A** a)
	//더 좋은 방법은 A& a로 넘겨주고 a 변화해주면 동적할당 필요없이 이용 가능함
	//또 *는 인수에서 *로 받고 &로 넘겨주면 NULL체크를 해주지 않으면 위험하고
	//NULL에도 우린 0으로 define되어있어 쓰레기값은 확인 불가, 그래서 nullptr
	//체크를 해줘야함, 근데 &로 받으면 NULL 체크를 해줄 필요가 없음, 그냥 거기에
	//직빵으로 할당가능하니까 문제 발생 확률도 거의 없어짐
	//08년 즈음 해서 업그레이드된 것들은 거진 &방식으로 바뀌는게 트렌드
	//언리얼도 다 레퍼런스고 C#도 xna해봐서 알지만 다 &임
	//기존 *방식으로 실제로 거의 발생하진 않지만 쓰레기값이 넘어왔을때
	//크래커가 메모리 번지 값을 바꿔서 정상값 인지시키면 문제를 발생시킬 수 있음
	//익스플리싯이라는 해킹기법임, 게임쪽에선 자주 발생하는 문제는 아님
	//그래서 기존 c언어 함수들(strcpy같은)은 더 안전한 함수들로(strcpy_s같은) 사용하도록
	//권고하고 있다 - 특히 보안체크 부분에서 기존꺼 사용하면 큰 문제가 발생할 수 있음
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