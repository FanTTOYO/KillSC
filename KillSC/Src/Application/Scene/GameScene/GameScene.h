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

	int m_waveCnt = 0;
	int m_bossAppearanceCnt = 0;
	int m_appearanceEffectCnt = 0;
	bool m_bAppearanceEffect;
};