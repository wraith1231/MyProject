#pragma once
#include "GameUnit.h"

enum class UnitClassId;
struct GameEnemySpec;
class AiContext;
class AiState;

namespace Objects
{
	class BoundingBox;
}

class GameEnemy : public GameUnit
{
protected:
	enum class AiType;
	struct AiDesc;

public:
	GameEnemy(wstring matFile, wstring meshFile);
	virtual ~GameEnemy();

	virtual void Update();
	virtual void Render();
	virtual void PreRender();
	virtual void PreRender2();
	virtual void ImGuiRender();

	void SetPlayerBullet(class Bullet* bul) { this->playerBullet = bul; }

private:
	void OnSearch();
	void OnMove();
	void OnAttack();
	void OnTurnLeft();
	void OnTurnRight();

protected:
	virtual void OnAiSearch(AiState* state) {}
	virtual void OnAiMove(AiState* state) {}
	virtual void OnAiAttack(AiState* state) {}
	virtual void OnAiTurnLeft(AiState* state) {}
	virtual void OnAiTurnRight(AiState* state) {}

protected:
	Objects::BoundingBox* box;

	UnitClassId unitClass;
	AiType startAi;
	GameEnemySpec* specData;
	AiContext* aiContext;

	float startAiTime;
	float actionElapsedTime;

	typedef pair<UINT, class AiState*> AiPair;
	AiPair aiSearch;
	AiPair aiMove;
	AiPair aiAttack;
	AiPair aiTurnLeft;
	AiPair aiTurnRight;

	class Bullet* playerBullet;

	vector<D3DXVECTOR3> destinations;
	UINT destIndex;
	float splineTime;

protected:
	enum class AiType
	{
		Search = 0, Move, Attack, 
		TurnLeft, TurnRight,
	};
};