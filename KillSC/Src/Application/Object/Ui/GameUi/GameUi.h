#pragma once
#include "../Ui.h"

class Player;
class Enemy;
class GameCamera;

class GameUi : public Ui
{
public:
	GameUi() {}
	~GameUi() {}

	void Update()override;
	void PostUpdate()override;
	void Init(std::weak_ptr<json11::Json> a_wpJsonObj)override;

	void DrawSprite()override;

	void SetAddFadeAlpha()override { m_addFadeAlpha = true; }
	int GetTime()override { return m_time; }
	const bool GetBOption()override { return m_bOption; }
	void SetPlayer(std::shared_ptr<Player> a_spPlayer)override { m_wpPlayer = a_spPlayer; }
	void AddEnemy(std::shared_ptr<Enemy> a_spEnemy)override;
	void SetCamera(std::shared_ptr<GameCamera> a_spCamera)override { m_wpCamera = a_spCamera; }
	const int GetGameTimeS1()override { return m_gameTimeS1; }
	const int GetGameTimeS10()override { return m_gameTimeS10; }

	const int GetGameTimeM1()override { return m_gameTimeM1; }
	const int GetGameTimeM10()override { return m_gameTimeM10; }

	void SetWaveCnt(int a_waveCnt)override { m_waveCnt = a_waveCnt; }
	void SetBWaveChange()override { m_bWaveChange = true; }

private:

	void OptionUpdate();

	int m_time = 0;
	float m_fadeAlpha = 0;
	bool m_addFadeAlpha = false;

	KdTexture m_EnemyDirectionArrowLTex;
	KdTexture m_EnemyDirectionArrowRTex;
	KdTexture m_EnemyDirectionArrowUTex;
	KdTexture m_EnemyDirectionArrowBTex;

	KdTexture m_conmaTex;
	KdTexture m_timeTex[10];
	KdTexture m_PointTex[3];
	KdTexture m_exitTex;
	KdTexture m_selectTex;

	float m_selectScale = 0;
	float m_exitScale = 0;

	Math::Vector3 m_exitPos = {};
	Math::Vector3 m_selectPos = {};

	float m_countOneScale;
	float m_countOneAlpha;
	KdTexture m_countOneTex;

	float m_countTwoScale;
	float m_countTwoAlpha;
	KdTexture m_countTwoTex;

	float m_countThreeScale;
	float m_countThreeAlpha;
	KdTexture m_countThreeTex;

	float m_countGoScale;
	float m_countGoAlpha;
	KdTexture m_countGoTex;

	std::vector<Math::Vector2> m_enemyScPosList;

	int           m_gameTimeS10 = 0;
	int           m_gameTimeS1 = 0;

	int           m_gameTimeM10 = 0;
	int           m_gameTimeM1 = 0;
	int           m_gameTimeCntDeray = 0;

	float m_waveScale;
	float m_waveAlpha;
	KdTexture m_waveTex;
	int m_waveCnt;
	int m_waveTimeCnt; // 2ウェーブからはこのカウントを使ってフェードイン・フェードアウトする
	bool m_bWaveChange; // ウェーブ数が変化した時　true

	bool m_bExit = false;
	bool m_bFirstExit = false;
	bool m_bOption = false;
	bool m_bSelect = false;

	std::list<std::weak_ptr<Enemy>> m_wpEnemyList;
	std::weak_ptr<Player> m_wpPlayer;
	std::weak_ptr<GameCamera> m_wpCamera;

	bool m_bTABKey = false;


	KdTexture m_backTex;
	Math::Vector3 m_backPos;
	float m_backScale;

	KdTexture m_operationTex;
	KdTexture m_hopperDataTex;
	KdTexture m_weaponDataTex;
	KdTexture m_weaponInfoTex;
	KdTexture m_howToTex;
	KdTexture m_leftYaiTex;
	KdTexture m_rightYaiTex;

	float         m_operationScale;
	Math::Vector3 m_operationPos;
	bool m_bOperation;

	float     m_weaponLeftYaiScale;
	Math::Vector3 m_weaponLeftYaiPos;

	float     m_weaponRightYaiScale;
	Math::Vector3 m_weaponRightYaiPos;

	float     m_weaOrHowLeftYaiScale;
	Math::Vector3 m_weaOrHowLeftYaiPos;

	float     m_weaOrHowRightYaiScale;
	Math::Vector3 m_weaOrHowRightYaiPos;

	bool m_bWeaponDataPage = false;
	bool m_bWeaponDataHopperPage = false;
	bool m_bWeaponDataScoPage = false;
	bool m_bHowToPage = false;

	KdTexture m_weaponType1Tex;
	KdTexture m_weaponType2Tex;
	KdTexture m_weaponTypeOvreDarkTex;
	KdTexture m_torionTex;
	KdTexture m_torionBarTex;
	KdTexture m_enduranceTex;
	KdTexture m_enduranceBarTex;

	Math::Matrix m_torionMat;
	Math::Matrix m_torionBarMat;
	Math::Matrix m_enduranceMat;
	Math::Matrix m_enduranceBarMat;

	KdTexture m_lWeaponChangeKeyTex; // 左の通常状態画像
	KdTexture m_rWeaponChangeKeyTex; // 右の通常状態画像

	KdTexture m_lWeaponChangeKeyPushStateTex; // 左の押された状態画像
	KdTexture m_rWeaponChangeKeyPushStateTex; // 右の押された状態画像

	float m_scaleFadeSpeed; // スケールのフェード速度
	float m_charFadeSpeed;  // 文字のフェード速度
	float m_charLastFadeOutSpeed;  // 文字の最後のフェードアウト速度
};