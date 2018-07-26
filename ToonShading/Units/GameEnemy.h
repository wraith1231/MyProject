#pragma once
#include "GameUnit.h"

enum class UnitClassId;
struct GameEnemySpec;
struct AiState;
class AiContext;

class GameEnemy : public GameUnit
{
public:
	enum class AiType;

public:
	GameEnemy(wstring matFile, wstring meshFile);
	virtual ~GameEnemy();

	virtual void Update();
	virtual void Render();

private:
	void OnSearch(AiState* state);
	void OnMove(AiState* state);
	void OnAttack(AiState* state);
	void OnTurnLeft(AiState* state);
	void OnTurnRight(AiState* state);

protected:
	virtual void OnAiSearch(AiState* state) {}
	virtual void OnAiMove(AiState* state) {}
	virtual void OnAiAttack(AiState* state) {}
	virtual void OnAiTurnLeft(AiState* state) {}
	virtual void OnAiTurnRight(AiState* state) {}

public:
	void StartAi(AiType type, float time);
	void NextAi(AiType type, float time);

protected:
	UnitClassId unitClass;
	AiType startAi;
	GameEnemySpec* specData;
	AiContext* aiContext;

	float startAiTime;
	float actionElapsedTime;

	typedef pair<UINT, AiState*> AiPair;
	AiPair aiSearch;
	AiPair aiMove;
	AiPair aiAttack;
	AiPair aiTurnLeft;
	AiPair aiTurnRight;

public:
	enum class AiType
	{
		Search = 0, Move, Attack, 
		TurnLeft, TurnRight,
	};
};