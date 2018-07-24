#pragma once
//AIBase

class AiState
{
public:
	function<void(float)> Updating;			//묶을때 보통 start~end로 묶음
	function<void()> Starting;					//finish도 있고
	function<void()> Finishing;					//destroy는 class?
												
	void Name(wstring val) { name = val; }
	wstring Name() { return name; }

	void ActiveTime(float val) { activeTime = val; }
	float ActiveTime() { return activeTime; }
	bool IsActive() { return activeTime > 0.0f; }	//active time이 0보다 크면 활성

	void Reset();
	void Update(float time);

	void Start();
	void Finish();

private:
	wstring name;	//상태 이름
	float activeTime;	//활동 시간


};