#pragma once
class Player;
class Enemy;
class GameCamera;

class Ui : public KdGameObject
{
public:
	Ui() {}
	virtual ~Ui(){}

	enum class UiType
	{
		title,
		game,
		result,
		select,
		tutorial,
		training,
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
	virtual void Init(std::weak_ptr<json11::Json> a_wpJsonObj);
	void DrawSprite()override;
	bool ButtomProcessing(Math::Vector2 a_pos , const KdTexture& a_tex , float& a_scale , float a_originalScale = 1.0f, float a_addScaleVal = 0.2f);

	virtual void SetUiType(UiType a_uiType);
	virtual void SetAddFadeAlpha() { m_currentUiClassId->SetAddFadeAlpha(); }
	virtual int GetTime() {return m_currentUiClassId->GetTime(); }
	virtual const bool GetBOption() { return m_currentUiClassId->GetBOption(); }
	virtual void AddEnemy(std::shared_ptr<Enemy> a_spEnemy);
	virtual void SetEnemy(std::shared_ptr<Enemy> a_spEnemy);
	virtual void SetPlayer(std::shared_ptr<Player> a_spPlayer) { m_currentUiClassId->SetPlayer(a_spPlayer); }
	virtual void SetCamera(std::shared_ptr<GameCamera> a_spCamera) { m_currentUiClassId->SetCamera(a_spCamera); }
	virtual const bool GetBSceneCangePossible() { return m_currentUiClassId->GetBSceneCangePossible(); }
	virtual const bool GetBTutorial() { return m_currentUiClassId->GetBTutorial(); }
	virtual const TutorialType GetTutorialType() { return m_currentUiClassId->GetTutorialType(); }
	virtual void AddTutorialCnt();

	virtual const int GetGameTimeS1() { return m_currentUiClassId->GetGameTimeS1(); }
	virtual const int GetGameTimeS10() { return m_currentUiClassId->GetGameTimeS10(); }
	virtual const int GetGameTimeM1() { return m_currentUiClassId->GetGameTimeM1(); }
	virtual const int GetGameTimeM10() { return m_currentUiClassId->GetGameTimeM10(); }
	virtual void SetWaveCnt(int a_waveCnt) { m_currentUiClassId->SetWaveCnt(a_waveCnt); }
	virtual void SetBWaveChange() { m_currentUiClassId->SetBWaveChange(); }

protected:
	std::map<std::string,json11::Json>  m_mpUiSharedObj;																	// JsonからUi共通オブジェクトの情報をもらうときに使用
	std::map<std::string,json11::Json>  m_mpDedicatedObj;																	// Jsonからそれぞれ専用のオブジェクトの情報をもらうときに使用
	float m_screenAlphaFadeSpeed;																							// スクリーンがフェードする速さ
	float m_pushLClickAlphaFadeSpeed;																						// PushLClickテクスチャのフェード速度
	int   m_mouseRadius;																									// マウスの半径
	int   m_mouseHalfHeight;																								// マウスの半分の高さ
	int   m_titleBarHeight;																									// エグゼのタイトルバーのデカさ
	Math::Vector3 m_titlerogoTexPos;																						// ゲームロゴの座標
	Math::Vector3 m_pushLClickTexPos;																						// PushLClickの座標

private:

	UiType m_uiType = UiType::title;
	std::shared_ptr<Ui> m_currentUiClassId;
};

class TitleUi : public Ui
{
public:
	TitleUi(){}
	~TitleUi(){}

	void Update()override;
	void PostUpdate(){}
	void Init(std::weak_ptr<json11::Json> a_wpJsonObj)override;

	void DrawSprite()override;

	void SetAddFadeAlpha()override { m_addFadeAlpha = true; }
	int GetTime()override { return m_time; }
	const bool GetBSceneCangePossible()override { return m_bSceneCangePossible; }

private:

	KdTexture m_titleCharRogoTex;
	KdTexture m_FTtoyoRogoTex;
	KdTexture m_pushLClickTex;

	int m_time = 0;
	float m_fadeAlpha = 0;
	bool m_addFadeAlpha = false;

	bool m_bSceneCangePossible = false;
	float m_pushLClickAlpha = 0.0f;
	bool  m_bPushLClickAlphaAdd = false;
};

class GameUi : public Ui
{
public:
	GameUi(){}
	~GameUi(){}

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

class ResultUi : public Ui
{
public:
	ResultUi() {}
	~ResultUi() {}

	void Update()override;
	void PostUpdate(){}
	void Init(std::weak_ptr<json11::Json> a_wpJsonObj)override;

	void DrawSprite()override;

	void SetAddFadeAlpha()override { m_addFadeAlpha = true; }
	int GetTime()override { return m_time; }
	const bool GetBSceneCangePossible()override { return m_bSceneCangePossible; }

private:
	KdTexture m_winTex;
	KdTexture m_lossTex;

	KdTexture m_PointTex[10];
	KdTexture m_winCharaTex;
	KdTexture m_winPlayerCharaTex;
	KdTexture m_loseCharaTex;
	KdTexture m_pushLClickTex;

	int m_weaponPoint = 0;
	std::string m_rank;

	int m_pointAddOrSubVal = 0;
	int m_time = 0;
	float m_fadeAlpha = 0;
	bool m_addFadeAlpha = false;

	bool m_bSceneCangePossible = false;
	float m_pushLClickAlpha = 0.0f;
	bool  m_bPushLClickAlphaAdd = false;
};

class ModeSelectUi : public Ui
{
public:
	ModeSelectUi() {}
	~ModeSelectUi() {}

	void Update()override;
	void PostUpdate(){}
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
	Math::Vector3 m_gamePos = {};
	Math::Vector3 m_pictureFramePos = {};
	Math::Vector3 m_challengePos = {};
	Math::Vector3 m_selectBackPos = {};
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

	KdTexture      m_chalenge50Tex;
	Math::Vector3  m_chalenge50Pos;
	float          m_chalenge50Scale;
	bool           m_bChalenge50;

	KdTexture      m_chalenge100Tex;
	Math::Vector3  m_chalenge100Pos;
	float          m_chalenge100Scale;
	bool           m_bChalenge100;

	bool m_bExit = false;
	bool m_bFirstExit = false;
	bool m_bOption = false;
	bool m_bTitle = false;
	bool m_bSelect = false;
};

class TutorialUi : public Ui
{
public:
	TutorialUi() {}
	~TutorialUi() {}

	void Update()override;
	void PostUpdate(){}
	void Init(std::weak_ptr<json11::Json> a_wpJsonObj)override;

	void DrawSprite()override;

	void SetPlayer(std::shared_ptr<Player> a_spPlayer)override { m_wpPlayer = a_spPlayer; }
	void SetCamera(std::shared_ptr<GameCamera> a_spCamera)override { m_wpCamera = a_spCamera; }
	void SetAddFadeAlpha()override { m_addFadeAlpha = true; }
	int GetTime()override { return m_time; }
	const bool GetBOption()override { return m_bOption; }
	void AddTutorialCnt()override;
	const bool GetBTutorial()override { return m_bTutorialView; }
	const TutorialType GetTutorialType()override { return m_tutorialType; }

private:
	void OptionUpdate();

	KdTexture m_hopperTyuTex;
	KdTexture m_sonotaTyuTex;
	KdTexture m_sukoADMoveTex;
	KdTexture m_tyubukiTex;
	KdTexture m_tyuKihonTex;
	KdTexture m_exitTex;
	KdTexture m_selectTex;

	float m_selectScale = 0;
	float m_exitScale = 0;

	int m_time = 0;
	float m_fadeAlpha = 0;
	bool m_addFadeAlpha = false;

	Math::Vector3 m_exitPos = {};
	Math::Vector3 m_selectPos = {};

	bool m_bTutorialView = false;
	int  m_tutorialCnt = 0;
	TutorialType m_tutorialType = kihonTu;
	int m_tutorialSwitchAfterTime;
	KdTexture m_PointTex[6];

	bool m_bExit = false;
	bool m_bFirstExit = false;
	bool m_bOption = false;
	bool m_bSelect = false;

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
};

class TrainingUi : public Ui
{
public:
	TrainingUi() {}
	~TrainingUi() {}

	void Update()override;
	void PostUpdate()override;
	void Init(std::weak_ptr<json11::Json> a_wpJsonObj)override;

	void DrawSprite()override;

	void SetAddFadeAlpha()override { m_addFadeAlpha = true; }
	int GetTime()override { return m_time; }
	const bool GetBOption()override { return m_bOption; }

	void SetPlayer(std::shared_ptr<Player> a_spPlayer)override { m_wpPlayer = a_spPlayer; }
	void SetEnemy(std::shared_ptr<Enemy> a_spEnemy)override;
	void SetCamera(std::shared_ptr<GameCamera> a_spCamera)override { m_wpCamera = a_spCamera; }
	const int GetGameTimeS1()override { return m_gameTimeS1; }
	const int GetGameTimeS10()override { return m_gameTimeS10; }

	const int GetGameTimeM1()override { return m_gameTimeM1; }
	const int GetGameTimeM10()override { return m_gameTimeM10; }

	void SetWaveCnt(int a_waveCnt)override { m_waveCnt = a_waveCnt; }
	void SetBWaveChange()override { m_bWaveChange = true; }

private:
	void OptionUpdate();

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

	int m_time = 0;
	float m_fadeAlpha = 0;
	bool m_addFadeAlpha = false;

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

	std::weak_ptr<Enemy> m_wpEnemy;
	std::weak_ptr<Player> m_wpPlayer;
	std::weak_ptr<GameCamera> m_wpCamera;
	Math::Vector2        m_enemyScPos;

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
};