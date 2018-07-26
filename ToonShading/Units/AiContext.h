#pragma once
#include "AiState.h"

struct AiState;

class AiContext
{
public:
	AiContext();
	~AiContext();

	void Enable(bool val) { bEnable = val; }

	//ÀÌ°Ç AiState°ª
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
	bool bEnable;

	AiState* active;
	AiState* next;

	vector<AiState*> states;

};