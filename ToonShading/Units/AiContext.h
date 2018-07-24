#pragma once

class AiState;

class AiContext
{
public:
	AiContext();
	~AiContext();

	AiState* Active() { return active; }
	AiState* Next() { return next; }

	void Update();

	AiState* FindState(UINT index);

	UINT AddState(wstring name, AiState* state);
	void RemoveState(UINT index);
	void RemoveStateAll();

	void NextState(UINT index, float activeTime);
	void StartState(UINT index, float activeTime);

private:
	void Play(UINT index, float activeTime);
	void Play(AiState* state, float activeTime);

private:
	bool bActive;

	AiState* active;
	AiState* next;

	vector<AiState*> states;

};