#pragma once

class ModelAnimPlayer
{
public:
	enum class Mode
	{
		Play = 0, Pause, Stop,
	};

	//ModelAnimPlayer(class Model* model);
	ModelAnimPlayer(class Model* model, wstring animName);
	~ModelAnimPlayer();

	//이건 아마 clip쪽에 뺄듯
	void Update();

	void ShadowRender();
	void PreRender();
	void ImGuiRender();

	Mode GetMode() { return mode; }
	void SetMode(UINT num) { mode = (Mode)num; }

	float GetFrameTime();
	float GetFrameRate();

	int GetCurrentKeyframe() { return currentKeyFrame; }
	void SetCurrentKeyFrame(float time);
	int GetTotalFrame();

	wstring GetAnimName() { return animName; }

	bool GetEnd() { return isEnd; }
	void SetEnd(bool end) { isEnd = end; }
private:
	void UpdateTime();
	void UpdateBone();
	//얘는 root만 모을라고 써놓은 것
	//void GatherBones(vector<class ModelBone*>& bones, class ModelBone* bone);

private:
	Shader* shadowShader;
	Shader* shader;

	Model* model;
	class ModelAnimClip* currentClip;

	Mode mode;
	
	wstring animName;

	int currentKeyFrame;
	float frameTime;			//현재 프레임돌입한 시간으로부터 경과시간
	float keyframeFactor;		//현재 프레임과 다음 프레임 사이의 보간 값

	int nextKeyFrame;

	bool bUseSlerp;				//구면보간을 할건지 안할건지, 안하면 속도는 빨라지지만 결과가 별로

	//vector<class ModelBone*> bones;
	vector<D3DXMATRIX> boneAnimation;
	vector<D3DXMATRIX> skinTransform;	//실제 렌더링될 행렬

	bool isEnd;
};