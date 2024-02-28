#pragma once
#include "../Ui.h"

class ResultUi : public Ui
{
public:
	ResultUi() {}
	~ResultUi() {}

	void Update()override;
	void PostUpdate() {}
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
	KdTexture m_losePlayerCharaTex;
	KdTexture m_loseCharaTex;
	KdTexture m_pushLClickTex;
	KdTexture m_continueButtonTex;
	KdTexture m_modeSelectButtonTex;

	int m_weaponPoint = 0;
	std::string m_rank;

	int m_pointAddOrSubVal = 0;
	int m_time = 0;
	float m_fadeAlpha = 0;
	bool m_addFadeAlpha = false;

	bool m_bSceneCangePossible = false;
	float m_pushLClickAlpha = 0.0f;
	bool  m_bPushLClickAlphaAdd = false;

	Math::Vector3 m_continueButtonPos;
	Math::Vector3 m_modeSelectButtonPos;

	float m_continueButtonScale;
	float m_modeSelectButtonScale;

	bool m_bContinueButton;
	bool m_bModeSelectButton;

	
};