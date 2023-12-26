#pragma once
#include "../BaseScene/BaseScene.h"
#define MAX_WAVE_CNT 2
#define MAX_BOSS_ENEMY_CNT 2
#define MIN_BOSS_ENEMY_CNT 1
#define CHARENGE_FIFTY_ENEMY_CNT 50
#define CHARENGE_ONE_HUMDRED_ENEMY_CNT 100
#define MAX_ENEMY_DRAW_TOTAL 5
#define MAX_STATE_TIME_CNT 240
#define MAX_ENEMY_ATTACK_CNT 4
#define ENEMY_APPEARANCE_MOMENT 25

class Player;
class Ui;
class Enemy;
class GameCamera;

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
	std::weak_ptr<GameCamera> m_wpGameCamera;

	int m_waveCnt = 0;
	int m_bossAppearanceCnt = 0;
	int m_appearanceEffectCnt = 0;
	bool m_bAppearanceEffect;

	std::list<std::weak_ptr<Enemy>> m_wpEnemyList;
	std::shared_ptr<json11::Json> m_spJsonObj;
};