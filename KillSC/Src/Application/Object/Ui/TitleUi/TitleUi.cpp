#include "TitleUi.h"
#include "../../../Scene/SceneManager.h"

void TitleUi::Init(std::weak_ptr<json11::Json> a_wpJsonObj)
{
	m_mpUiSharedObj = (*a_wpJsonObj.lock())["Ui"].object_items();
	m_mpDedicatedObj = m_mpUiSharedObj["TitleUi"].object_items();

	m_pushLClickTex.Load("Asset/Textures/Ui/shared/PUSHTLCLICK.png");
	m_titleCharRogoTex.Load("Asset/Textures/Ui/Title/KillSC.png");
	m_FTtoyoRogoTex.Load("Asset/Textures/Ui/Title/FTtoyo.png");
	m_time = 0;
	m_addFadeAlpha = false;
	m_fadeAlpha = 1.0f;
	m_bSceneCangePossible = false;
	m_pushLClickAlpha = 1.0f;
	m_bPushLClickAlphaAdd = false;

	m_titlerogoTexPos = { (float)m_mpDedicatedObj["TitlerogoTexPos"][0].number_value(),
						 (float)m_mpDedicatedObj["TitlerogoTexPos"][1].number_value(),
						 (float)m_mpDedicatedObj["TitlerogoTexPos"][2].number_value() };

	m_pushLClickTexPos = { (float)m_mpDedicatedObj["PushLClickTexPos"][0].number_value(),
						  (float)m_mpDedicatedObj["PushLClickTexPos"][1].number_value(),
						  (float)m_mpDedicatedObj["PushLClickTexPos"][2].number_value() };

	m_screenAlphaFadeSpeed = static_cast<float>(m_mpUiSharedObj["ScreenAlphaFadeSpeed"].number_value());
	m_pushLClickAlphaFadeSpeed = static_cast<float>(m_mpUiSharedObj["PushLClickAlphaFadeSpeed"].number_value());
	m_mouseRadius = m_mpUiSharedObj["MouseRadius"].int_value();
	m_mouseHalfHeight = m_mpUiSharedObj["MouseHalfHeight"].int_value();
	m_titleBarHeight = m_mpUiSharedObj["TitleBarHeight"].int_value();
}

void TitleUi::Update()
{
	m_time++;
	if (m_time >= m_mpDedicatedObj["MaxTime"].int_value())
	{
		m_time = m_mpDedicatedObj["MaxTime"].int_value();
		m_bSceneCangePossible = true;

		if (m_bPushLClickAlphaAdd)
		{
			m_pushLClickAlpha += m_pushLClickAlphaFadeSpeed;
			if (m_pushLClickAlpha >= 1.0f)
			{
				m_pushLClickAlpha = 1.0f;
				m_bPushLClickAlphaAdd = false;
			}
		}
		else
		{
			m_pushLClickAlpha -= m_pushLClickAlphaFadeSpeed;
			if (m_pushLClickAlpha <= 0.2f)
			{
				m_pushLClickAlpha = 0.2f;
				m_bPushLClickAlphaAdd = true;
			}
		}

		if (m_addFadeAlpha)
		{
			m_fadeAlpha += m_screenAlphaFadeSpeed;
			if (m_fadeAlpha >= 1.0f)
			{
				m_fadeAlpha = 1.0f;

				SceneManager::Instance().SetNextScene
				(
					SceneManager::SceneType::select
				);
			}
		}
	}

	if (m_time >= 0 && m_time <= 120)
	{
		m_fadeAlpha -= (1.0f / 120.0f);
		if (m_fadeAlpha <= 0.0f)
		{
			m_fadeAlpha = 0.0f;
		}
	}
	else if (m_time >= 240 && m_time < 360)
	{
		m_fadeAlpha += (1.0f / 120.0f);
		if (m_fadeAlpha >= 1.0f)
		{
			m_fadeAlpha = 1.0f;
		}
	}
	else if (m_time >= 360 && m_time <= 480)
	{
		m_fadeAlpha -= (1.0f / 120.0f);
		if (m_fadeAlpha <= 0.0f)
		{
			m_fadeAlpha = 0.0f;
		}
	}

	if (KdInputManager::Instance().IsPress("select"))
	{
		if (m_time < 360)
		{
			m_time = 360;
			m_fadeAlpha = 1.0f;
		}
	}
}

void TitleUi::DrawSprite()
{
	Math::Matrix transMat = Math::Matrix::Identity;
	Math::Matrix mat = Math::Matrix::Identity;
	Math::Color color = {};

	KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
	color = { 0,0,0,1 };
	KdShaderManager::Instance().m_spriteShader.DrawBox(0, 0, 1280, 720, &color);

	if (m_time >= 0 && m_time < 360)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_FTtoyoRogoTex, 0, 0, static_cast<int>(m_FTtoyoRogoTex.GetWidth()), static_cast<int>(m_FTtoyoRogoTex.GetHeight()));
	}
	else if (m_time >= 360)
	{
		transMat = Math::Matrix::CreateTranslation(m_titlerogoTexPos);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_titleCharRogoTex, 0, 0, static_cast<int>(m_titleCharRogoTex.GetWidth()), static_cast<int>(m_titleCharRogoTex.GetHeight()));

		transMat = Math::Matrix::CreateTranslation(m_pushLClickTexPos);
		Math::Rectangle rc = { 0,0,static_cast<int>(m_pushLClickTex.GetWidth()),static_cast<int>(m_pushLClickTex.GetHeight()) };
		color = { 1,1,1,m_pushLClickAlpha };
		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_pushLClickTex, 0, 0, static_cast<int>(m_pushLClickTex.GetWidth()), static_cast<int>(m_pushLClickTex.GetHeight()), &rc, &color);
	}

	transMat = Math::Matrix::Identity;
	KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
	color = { 0,0,0,m_fadeAlpha };
	KdShaderManager::Instance().m_spriteShader.DrawBox(0, 0, 1280, 720, &color);
}