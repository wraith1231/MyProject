#include "stdafx.h"
#include "AiContext.h"
#include "AiState.h"

AiContext::AiContext()
	: bActive(true), bEnable(true)
	,active(NULL), next(NULL)
{

}

AiContext::~AiContext()
{

}

void AiContext::Update()
{
	if (bEnable == false) return;	//¾ë update¸¸ return
	if (bActive == false) return;

	if (active == NULL && next == NULL) return;

	if (active == NULL)
	{
		Play(next, next->ActiveTime);
	}
	else if (next != NULL && active->ActiveTime == 0.0f)
	{
		Play(next, next->ActiveTime);

		next = NULL;
	}
	else
	{
		if (active != NULL)
		{
			active->Update();

			if (active->IsActive() == true)
				active->ActiveTime -= Time::Delta();

			if (active->ActiveTime < 0.0f)
				active->ActiveTime = 0.0f;

		}//if(active)
	}
}

AiState * AiContext::FindState(UINT index)
{
	if (bActive == false) return NULL;

	return states[index];
}

UINT AiContext::AddState(wstring name, AiState * state)
{
	if (bActive == false) return -1;

	state->Name = name;
	states.push_back(state);

	return states.size() - 1;
}

void AiContext::RemoveState(UINT index)
{
	states.erase(states.begin() + index);
}

void AiContext::RemoveStateAll()
{
	states.clear();
}

void AiContext::NextState(UINT index, float activeTime)
{
	if (bActive == false) return;

	next = FindState(index);
	next->ActiveTime = activeTime;
}

void AiContext::StartState(UINT index, float activeTime)
{
	if (bActive == false) return;
	
	active = NULL;
	next = NULL;

	Play(index, activeTime);
}

void AiContext::Play(UINT index, float activeTime)
{
	if (bActive == false) return;

	AiState* state = FindState(index);
	Play(state, activeTime);
}

void AiContext::Play(AiState * state, float activeTime)
{
	if (bActive == false) return;

	if (active != NULL)
		active->Finish();

	active = state;
	active->ActiveTime = activeTime;

	active->Start();
}