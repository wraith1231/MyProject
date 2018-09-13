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

	//�̰� �Ƹ� clip�ʿ� ����
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
	//��� root�� ������� ����� ��
	//void GatherBones(vector<class ModelBone*>& bones, class ModelBone* bone);

private:
	Shader* shadowShader;
	Shader* shader;

	Model* model;
	class ModelAnimClip* currentClip;

	Mode mode;
	
	wstring animName;

	int currentKeyFrame;
	float frameTime;			//���� �����ӵ����� �ð����κ��� ����ð�
	float keyframeFactor;		//���� �����Ӱ� ���� ������ ������ ���� ��

	int nextKeyFrame;

	bool bUseSlerp;				//���麸���� �Ұ��� ���Ұ���, ���ϸ� �ӵ��� ���������� ����� ����

	//vector<class ModelBone*> bones;
	vector<D3DXMATRIX> boneAnimation;
	vector<D3DXMATRIX> skinTransform;	//���� �������� ���

	bool isEnd;
};