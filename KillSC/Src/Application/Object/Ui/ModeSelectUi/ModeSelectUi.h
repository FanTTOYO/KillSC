#pragma once
#include "../Ui.h"

class ModeSelectUi : public Ui
{
public:
	ModeSelectUi() {}
	~ModeSelectUi() {}

	void Update()override;
	void PostUpdate() {}
	void Init(std::weak_ptr<json11::Json> a_wpJsonObj)override;

	void DrawSprite()override;

	void SetAddFadeAlpha()override { m_addFadeAlpha = true; }
	int GetTime()override { return m_time; }

private:
	KdTexture m_modeSelectTex;
	KdTexture m_pictureFrameTex;

	KdTexture m_titleTex;
	KdTexture m_exitTex;

	KdTexture m_gameTex;
	KdTexture m_challengeTex;
	KdTexture m_selectBackTex;
	KdTexture m_trainingTex;
	KdTexture m_tutorialTex;

	float m_titleScale = 0;
	float m_selectScale = 0;
	float m_exitScale = 0;

	float m_gameScale = 0;
	float m_tutorialScale = 0;
	float m_challengeScale = 0;
	float m_trainingScale = 0;

	Math::Vector3 m_tutorialPos = {};
	Math::Vector3 m_trainingPos = {};
	Math::Vector3 m_titlePos = {};
	Math::Vector3 m_exitPos = {};

	bool m_bTutorial = false;
	bool m_bGame = false;
	bool m_bChallenge = false;
	bool m_bTraining = false;

	int m_time = 0;
	float m_fadeAlpha = 0;
	bool m_addFadeAlpha = false;

	KdTexture      m_enemyBossTypeOneTex;
	Math::Vector3  m_enemyBossTypeOnePos;
	float          m_enemyBossTypeOneScale;
	bool           m_bEnemyBossTypeOneTotal;

	KdTexture      m_enemyBossTypeTwoTotalTex;
	Math::Vector3  m_enemyBossTypeTwoPos;
	float          m_enemyBossTypeTwoScale;
	bool           m_bEnemyBossTypeTwoTotal;

	KdTexture      m_twoEnemyBossTypeOneTex;
	Math::Vector3  m_twoEnemyBossTypeOnePos;
	float          m_twoEnemyBossTypeOneScale;
	bool           m_bTwoEnemyBossTypeOneTotal;

	KdTexture	  m_helpMkTex;
	Math::Vector3 m_helpMkPos;
	float         m_helpMkScale;
	bool          m_bhelp;
	bool          m_bBattlehelp;

	KdTexture	  m_clauseMkTex;
	Math::Vector3 m_clauseMkPos;
	float         m_clauseMkScale;

	KdTexture     m_battlehelpTex;

	KdTexture     m_battleCharaTex;
	Math::Vector3 m_battleCharaPos;
	float         m_battleCharaScale;
	bool          m_bBattleChara;

	KdTexture     m_chalengehelpTex;
	bool          m_bChalengehelp;

	KdTexture     m_challengeCharaTex;
	Math::Vector3 m_challengeCharaPos;
	float         m_challengeCharaScale;
	bool          m_bChallengeChara;

	KdTexture      m_chalengeFiftyTex;
	Math::Vector3  m_chalengeFiftyPos;
	float          m_chalengeFiftyScale;
	bool           m_bChalengeFifty;

	KdTexture      m_chalengeHundredTex;
	Math::Vector3  m_chalengeHundredPos;
	float          m_chalengeHundredScale;
	bool           m_bChalengeHundred;

	bool m_bExit = false;
	bool m_bFirstExit = false;
	bool m_bOption = false;
	bool m_bTitle = false;
	bool m_bSelect = false;
};