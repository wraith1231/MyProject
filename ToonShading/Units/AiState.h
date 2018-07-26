#pragma once
#include "stdafx.h"

struct AiState
{
	wstring Name;
	float ActiveTime;

	function<void(AiState *)> Starting;
	function<void(AiState *)> Finishing;
	function<void(AiState *)> Updating;

	bool IsActive() { return ActiveTime > 0.0f; }

	void Reset()
	{
		ActiveTime = 0.0f;
	}

	void Update()
	{
		if (Updating == NULL) return;

		Updating(this);
	}

	void Start()
	{
		if (Starting == NULL) return;

		Starting(this);
	}

	void Finish()
	{
		if (Finishing == NULL) return;

		Finishing(this);
	}
};