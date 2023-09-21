#pragma once
class Player;

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
	void TitleUpdate();
	void GameUpdate();
	void TutorialUpdate();
	void ResultUpdate();
	void SelectUpdate();
	void DrawSprite()override;
	void SetUiType(UiType a_uiType) { m_uiType = a_uiType; }
	void SetAddFadeAlpha() { m_addFadeAlpha = true; }
	int GetTime() {return m_time;}
	const bool GetBOption() { return m_bOption; }
	void SetPlayer(std::shared_ptr<Player> a_spPlayer) { m_wpPlayer = a_spPlayer; }
	const bool GetBSceneCangePossible() { return m_bSceneCangePossible; }
	const bool GetBTutorial() { return m_bTutorialView; }
	const TutorialType GetTutorialType() { return m_tutorialType; }
	void AddTutorialCnt();

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

	KdTexture m_winTex;
	KdTexture m_lossTex;
	KdTexture m_Point0Tex;
	KdTexture m_Point1Tex;
	KdTexture m_Point2Tex;
	KdTexture m_Point3Tex;
	KdTexture m_Point4Tex;
	KdTexture m_Point5Tex;
	KdTexture m_Point6Tex;
	KdTexture m_Point7Tex;
	KdTexture m_Point8Tex;
	KdTexture m_Point9Tex;

	KdTexture m_tutorialTex;
	KdTexture m_exitTex;
	KdTexture m_gameTex;
	KdTexture m_optionTex;
	KdTexture m_titleTex;

	float m_tutorialScale = 0;
	float m_exitScale = 0;
	float m_gameScale = 0;
	float m_optionScale = 0;
	float m_titleScale = 0;

	Math::Vector3 m_tutorialPos = {};
	Math::Vector3 m_exitPos = {};
	Math::Vector3 m_gamePos = {};
	Math::Vector3 m_optionPos = {};
	Math::Vector3 m_titlePos = {};

	bool m_bTutorial = false;
	bool m_bExit = false;
	bool m_bFirstExit = false;
	bool m_bGame = false;
	bool m_bOption = false;
	bool m_bTitle = false;

	int m_weaponPoint = 0;
	std::string m_rank;
	int m_time = 0; 
	float m_fadeAlpha = 0;
	bool m_addFadeAlpha = false;
	std::weak_ptr<Player> m_wpPlayer;
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
};