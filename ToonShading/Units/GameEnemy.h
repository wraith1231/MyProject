#pragma once
#include "GameUnit.h"

enum class UnitClassId;
struct GameEnemySpec;
struct AiState;
struct ExecuteValues;
class AiContext;

namespace Objects
{
	class BoundingBox;
	class Ray;
}

class GameEnemy : public GameUnit
{
public:
	enum class AiType;

public:
	GameEnemy(wstring matFile, wstring meshFile, ExecuteValues* value = NULL);
	virtual ~GameEnemy();

	virtual void Update();
	virtual void EditUpdate();

	virtual void ShadowRender();
	virtual void PreRender();

	virtual void ImGuiRender();

	void StartPoint(D3DXVECTOR3& vec) { start = vec; }
	D3DXVECTOR3 StartPoint() { return start; }

	bool SelectEnemy(Objects::Ray* ray);
	void Unselect() { selected = false; }
	
	class Bullet* GetBullet() { return bullet; }
	Objects::BoundingBox* Box() { return box; }

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
	class Bullet* bullet;

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

	D3DXVECTOR3 start;
	Objects::BoundingBox* box;

	bool selected;
	D3DXVECTOR2 rot;

	
public:
	enum class AiType
	{
		Search = 0, Move, Attack, 
		TurnLeft, TurnRight,
	};
};