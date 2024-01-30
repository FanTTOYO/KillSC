#pragma once

#include "../Ui.h"

class TutorialUi : public Ui
{
public:
	TutorialUi() {}
	~TutorialUi() {}

	void Update()override;
	void PostUpdate() {}
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
	KdTexture m_sukoADMoveTex;
	KdTexture m_tyubukiTex;
	KdTexture m_tyuKihonTex;

	KdTexture m_hopperTyuToDoTex;
	KdTexture m_sukoADMoveToDoTex;
	KdTexture m_tyubukiToDoTex;
	KdTexture m_tyuKihonToDoTex;

	KdTexture m_sonotaTyuTex;
	KdTexture m_enemyPosToZeroWayTex;

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
	KdTexture m_cntTex[6];
	KdTexture m_divisionSignTex;

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

	KdTexture m_lWeaponChangeKeyTex; // ç∂ÇÃí èÌèÛë‘âÊëú
	KdTexture m_rWeaponChangeKeyTex; // âEÇÃí èÌèÛë‘âÊëú

	KdTexture m_lWeaponChangeKeyPushStateTex; // ç∂ÇÃâüÇ≥ÇÍÇΩèÛë‘âÊëú
	KdTexture m_rWeaponChangeKeyPushStateTex; // âEÇÃâüÇ≥ÇÍÇΩèÛë‘âÊëú
};