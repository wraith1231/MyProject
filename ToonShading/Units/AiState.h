#pragma once
//AIBase

class AiState
{
public:
	function<void(float)> Updating;			//������ ���� start~end�� ����
	function<void()> Starting;					//finish�� �ְ�
	function<void()> Finishing;					//destroy�� class?
												
	void Name(wstring val) { name = val; }
	wstring Name() { return name; }

	void ActiveTime(float val) { activeTime = val; }
	float ActiveTime() { return activeTime; }
	bool IsActive() { return activeTime > 0.0f; }	//active time�� 0���� ũ�� Ȱ��

	void Reset();
	void Update(float time);

	void Start();
	void Finish();

private:
	wstring name;	//���� �̸�
	float activeTime;	//Ȱ�� �ð�


};