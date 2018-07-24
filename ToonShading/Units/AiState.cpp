#include "stdafx.h"
#include "AiState.h"

void AiState::Reset()
{
	activeTime = 0.0f;
}

void AiState::Update(float time)
{
	if (Updating == NULL)
		return;

	Updating(time);
}

void AiState::Start()
{
	if (Starting == NULL)
		return;

	Starting();
}

void AiState::Finish()
{
	if (Finishing == NULL)
		return;

	Finishing();
}
