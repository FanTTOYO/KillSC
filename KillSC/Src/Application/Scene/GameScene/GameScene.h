#pragma once
#include "../BaseScene/BaseScene.h"
class Player;
class Ui;

class GameScene:public BaseScene
{
public:
	GameScene() { Init(); }
	~GameScene(){}

private:

	void Event()override;
	void Init()override;

	std::weak_ptr<Player> m_wpPlayer;
	std::weak_ptr<Ui> m_wpUi;
};