#pragma once
#include "../BaseScene/BaseScene.h"
class Player;
class Ui;
class GameCamera;
class Enemy;

class TrainingScene :public BaseScene
{
public:
	TrainingScene() { Init(); }
	~TrainingScene(){}

private:

	void Event()override;
	void Init()override;

	std::weak_ptr<Player> m_wpPlayer;
	std::weak_ptr<Enemy> m_wpEnemy;
	std::weak_ptr<Ui> m_wpUi;
	std::weak_ptr<GameCamera> m_wpGameCamera;
};