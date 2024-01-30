#pragma once
#include "../Ui.h"

class TitleUi : public Ui
{
public:
	TitleUi() {}
	~TitleUi() {}

	void Update()override;
	void PostUpdate() {}
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