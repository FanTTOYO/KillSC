#pragma once
class Player;
class Enemy;
class GameCamera;

class Ui : public KdGameObject
{
public:
	Ui() { Init(); }
	~Ui(){}

	enum class UiType
	{
		title,
		game,
		result,
		select,
		tutorial,
	};

	enum TutorialType
	{
		kihonTu,
		bukiTu,
		hopperTu,
		sonotaTu,
		sukoADMoveTu,
	};

	void Update()override;
	void PostUpdate()override;
	void TitleUpdate();
	void GameUpdate();
	void TutorialUpdate();
	void ResultUpdate();
	void SelectUpdate();
	void DrawSprite()override;
	void DrawUnLit()override;
	//void PreDraw()override;
	void SetUiType(UiType a_uiType) { m_uiType = a_uiType; }
	void SetAddFadeAlpha() { m_addFadeAlpha = true; }
	int GetTime() {return m_time;}
	const bool GetBOption() { return m_bOption; }
	void SetPlayer(std::shared_ptr<Player> a_spPlayer) { m_wpPlayer = a_spPlayer; }
	void AddEnemy(std::shared_ptr<Enemy> a_spEnemy);
	void SetCamera(std::shared_ptr<GameCamera> a_spCamera) { m_wpCamera = a_spCamera; }
	const bool GetBSceneCangePossible() { return m_bSceneCangePossible; }
	const bool GetBTutorial() { return m_bTutorialView; }
	const TutorialType GetTutorialType() { return m_tutorialType; }
	void AddTutorialCnt();

	const int GetGameTimeS1() { return m_gameTimeS1; }
	const int GetGameTimeS10() { return m_gameTimeS10; }

	const int GetGameTimeM1() { return m_gameTimeM1; }
	const int GetGameTimeM10() { return m_gameTimeM10; }
	const bool GetBEnterKey() { return m_bEnterKey; }

	void SetWaveCnt(int a_waveCnt) { m_waveCnt = a_waveCnt; }
	void SetBWaveChange() { m_bWaveChange = true; }

private:
	void Init()override;

	UiType m_uiType = UiType::title;

	KdTexture m_titleCharRogoTex;
	KdTexture m_FTtoyoRogoTex;

	KdTexture m_weaponType1Tex;
	KdTexture m_weaponType2Tex;
	KdTexture m_weaponTypeOvreDarkTex;
	KdTexture m_torionTex;
	KdTexture m_torionBarTex;
	KdTexture m_enduranceTex;
	KdTexture m_enduranceBarTex;
	KdTexture m_pushToEnterTex;

	KdPolygon m_torionPolygon;
	KdPolygon m_torionBarPolygon;
	KdPolygon m_endurancePolygon;
	KdPolygon m_enduranceBarPolygon;

	Math::Matrix m_torionMat;
	Math::Matrix m_torionBarMat;
	Math::Matrix m_enduranceMat;
	Math::Matrix m_enduranceBarMat;

	KdTexture m_winTex;
	KdTexture m_lossTex;

	KdTexture m_PointTex[10];

	KdTexture m_exitTex;
	KdTexture m_selectTex;

	KdTexture m_selectTitleTex;
	KdTexture m_selectExitTex;
	KdTexture m_optionTex;

	KdTexture m_gameTex;
	KdTexture m_challengeTex;
	KdTexture m_selectBackTex;
	KdTexture m_trainingTex;
	KdTexture m_tutorialTex;


	float m_optionScale = 0;
	float m_titleScale = 0;
	float m_selectScale = 0;
	float m_exitScale = 0;

	float m_gameScale = 0;
	float m_tutorialScale = 0;
	float m_challengeScale = 0;
	float m_trainingScale = 0;

	Math::Vector3 m_exitPos = {};
	Math::Vector3 m_optionPos = {};
	Math::Vector3 m_titlePos = {};
	Math::Vector3 m_selectPos = {};

	Math::Vector3 m_tutorialPos   = {};
	Math::Vector3 m_gamePos       = {};
	Math::Vector3 m_challengePos  = {};
	Math::Vector3 m_selectBackPos = {};
	Math::Vector3 m_trainingPos   = {};


	bool m_bExit = false;
	bool m_bFirstExit = false;
	bool m_bOption = false;

	bool m_bTutorial = false;
	bool m_bGame = false;
	bool m_bTitle = false;
	bool m_bSelect = false;
	bool m_bChallenge = false;
	bool m_bTraining = false;


	int m_weaponPoint = 0;
	std::string m_rank;
	int m_time = 0; 
	float m_fadeAlpha = 0;
	bool m_addFadeAlpha = false;
	std::weak_ptr<Player> m_wpPlayer;
	std::list<std::weak_ptr<Enemy>> m_wpEnemyList;
	std::weak_ptr<GameCamera> m_wpCamera;
	bool m_bFirstInResult = false;
	int m_pointAddOrSubVal = 0;
	bool m_bSceneCangePossible = false;
	float m_pushToEnterAlpha = 0.0f;
	bool  m_bPushToEnterAlphaAdd = false;

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

	KdTexture m_backTex;
	Math::Vector3 m_backPos;
	float m_backScale;

	bool m_bTABKey = false;
	bool m_bEnterKey = false;

	KdTexture m_infoTex;
	KdTexture m_hopperDataTex;
	KdTexture m_weaponDataTex;
	KdTexture m_weaponInfoTex;
	KdTexture m_howToTex;
	KdTexture m_leftYaiTex;
	KdTexture m_rightYaiTex;

	KdTexture m_hopperTyuTex;
	KdTexture m_sonotaTyuTex;
	KdTexture m_sukoADMoveTex;
	KdTexture m_tyubukiTex;
	KdTexture m_tyuKihonTex;

	float         m_infoScale;
	Math::Vector3 m_infoPos;
	bool m_bInfo;

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

	bool bLButtonKey = false;
	bool m_bTutorialView = false;
	int  m_tutorialCnt  =     0;
	TutorialType m_tutorialType = kihonTu;
	int m_tutorialSwitchAfterTime;

	std::vector<Math::Vector2> m_enemyScPosList;

	KdTexture      m_oneEnemyTotalTex;
	Math::Vector3  m_oneEnemyTotalPos;
	float          m_oneEnemyScale;
	bool           m_bOneEnemyTotal;

	KdTexture      m_twoEnemyTotalTex;
	Math::Vector3  m_twoEnemyTotalPos;
	float          m_twoEnemyScale;
	bool           m_bTwoEnemyTotal;

	KdTexture      m_threeEnemyTotalTex;
	Math::Vector3  m_threeEnemyTotalPos;
	float          m_threeEnemyScale;
	bool           m_bThreeEnemyTotal;

	KdTexture m_helpMkTex;

	KdTexture     m_battlehelpTex;
	Math::Vector3 m_BattlehelpMkPos;
	float         m_BattlehelpMkScale;
	bool          m_bBattlehelp;

	KdTexture     m_battleCharaTex;
	Math::Vector3 m_battleCharaPos;
	float         m_battleCharaScale;
	bool          m_bBattleChara;

	KdTexture     m_chalengehelpTex;
	Math::Vector3 m_chalengehelpMkPos;
	float         m_chalengehelpMkScale;
	bool          m_bChalengehelp;

	KdTexture     m_challengeCharaTex;
	Math::Vector3 m_challengeCharaPos;
	float         m_challengeCharaScale;
	bool          m_bChallengeChara;

	KdTexture      m_chalenge50Tex;
	Math::Vector3  m_chalenge50Pos;
	float          m_chalenge50Scale;
	bool           m_bChalenge50;

	KdTexture      m_chalenge100Tex;
	Math::Vector3  m_chalenge100Pos;
	float          m_chalenge100Scale;
	bool           m_bChalenge100;

	int           m_gameTimeS10 = 0;
	int           m_gameTimeS1 = 0;

	int           m_gameTimeM10 = 0;
	int           m_gameTimeM1 = 0;
	int           m_gameTimeCntDeray = 0;

	KdTexture m_time0Tex;
	KdTexture m_time1Tex;
	KdTexture m_time2Tex;
	KdTexture m_time3Tex;
	KdTexture m_time4Tex;
	KdTexture m_time5Tex;
	KdTexture m_time6Tex;
	KdTexture m_time7Tex;
	KdTexture m_time8Tex;
	KdTexture m_time9Tex;
	KdTexture m_conmaTex;

	KdTexture m_winCharaTex;
	KdTexture m_loseCharaTex;

	bool m_bBattleSelect = false;
	bool m_bChallengeSelect = false;

	KdTexture m_EnemyDirectionArrowLTex;
	KdTexture m_EnemyDirectionArrowRTex;
	KdTexture m_EnemyDirectionArrowUTex;
	KdTexture m_EnemyDirectionArrowBTex;

	float m_waveScale;
	float m_waveAlpha;
	KdTexture m_waveTex;
	int m_waveCnt;
	int m_waveTimeCnt; // 2ウェーブからはこのカウントを使ってフェードイン・フェードアウトする
	bool m_bWaveChange; // ウェーブ数が変化した時　true
};