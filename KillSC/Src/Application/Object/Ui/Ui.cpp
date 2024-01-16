#include "Ui.h"
#include "../../main.h"
#include "../../Scene/SceneManager.h"
#include "../Character/Player/Player.h"
#include "../Character/Enemy/Enemy.h"
#include "../../Camera/GameCamera/GameCamera.h"

void Ui::SetUiType(UiType a_uiType)
{
	m_uiType = a_uiType;
	switch (m_uiType)
	{
	case UiType::title:
		m_currentUiClassId = std::make_shared<TitleUi>();
		break;
	case UiType::training:
		m_currentUiClassId = std::make_shared<TrainingUi>();
		break;
	case UiType::game:
		m_currentUiClassId = std::make_shared<GameUi>();
		break;
	case UiType::tutorial:
		m_currentUiClassId = std::make_shared<TutorialUi>();
		break;
	case UiType::result:
		m_currentUiClassId = std::make_shared<ResultUi>();
		break;
	case UiType::select:
		m_currentUiClassId = std::make_shared<ModeSelectUi>();
		break;
	}
}

void Ui::Update()
{
	m_currentUiClassId->Update();
}

void Ui::PostUpdate()
{
	m_currentUiClassId->PostUpdate();
}

void Ui::DrawSprite()
{
	Math::Matrix transMat = Math::Matrix::Identity;

	m_currentUiClassId->DrawSprite();

	transMat = Math::Matrix::Identity;
	KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
}

void Ui::AddEnemy(std::shared_ptr<Enemy> a_spEnemy)
{
	m_currentUiClassId->AddEnemy(a_spEnemy);
}

void Ui::AddTutorialCnt()
{
	m_currentUiClassId->AddTutorialCnt();
}

void Ui::SetEnemy(std::shared_ptr<Enemy> a_spEnemy)
{
	m_currentUiClassId->SetEnemy(a_spEnemy);
}

void Ui::Init(std::weak_ptr<json11::Json> a_wpJsonObj)
{
	m_currentUiClassId->Init(a_wpJsonObj);
}

bool Ui::ButtomProcessing(Math::Vector2 a_pos, const KdTexture& a_tex, float& a_scale, float a_originalScale, float a_addScaleVal)
{
	PWINDOWINFO pwi = new WINDOWINFO;
	pwi->cbSize = sizeof(WINDOWINFO);
	GetWindowInfo(Application::Instance().GetWindowHandle(), pwi);

	POINT mousePos;
	GetCursorPos(&mousePos);

	mousePos.x -= 640;
	mousePos.y = mousePos.y * -1 + 360;
	Math::Vector3 Dis;
	float mouseX = (float)mousePos.x;
	float mouseY = (float)mousePos.y + (float)(pwi->rcWindow.top + m_mpUiSharedObj["TitleBarHeight"].int_value());

	float MouseLeft = mouseX - static_cast<float>(m_mpUiSharedObj["MouseRadius"].number_value());
	float MouseRight = mouseX + static_cast<float>(m_mpUiSharedObj["MouseRadius"].number_value());
	float MouseTop = mouseY + static_cast<float>(m_mpUiSharedObj["MouseHalfHeight"].number_value());
	float MouseBottom = mouseY - static_cast<float>(m_mpUiSharedObj["MouseHalfHeight"].number_value());

	Math::Vector3 ButtomPos;
	ButtomPos.x = a_pos.x + (float)(pwi->rcWindow.left);
	ButtomPos.y = a_pos.y;

	float TextureWidth  = static_cast<float>(a_tex.GetWidth())  / 2.0f;
	float TextureHeight = static_cast<float>(a_tex.GetHeight()) / 2.0f;

	float ButtomLeft = ButtomPos.x - TextureWidth;
	float ButtomRight = ButtomPos.x + TextureWidth;
	float ButtomTop = ButtomPos.y + TextureHeight;
	float ButtomBottom = ButtomPos.y - TextureHeight;

	if (MouseRight > ButtomLeft && ButtomRight > MouseLeft &&
		MouseTop > ButtomBottom && ButtomTop > MouseBottom)
	{
		a_scale = a_originalScale + a_addScaleVal;

		if (KdInputManager::Instance().IsPress("select"))
		{
			KdSafeDelete(pwi);
			return true;
		}

	}
	else
	{
		a_scale = a_originalScale;
	}

	return false;

	KdSafeDelete(pwi);
}

// タイトルのUI関連の処理＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
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

	m_titlerogoTexPos = {(float)m_mpDedicatedObj["TitlerogoTexPos"][0].number_value(),
						 (float)m_mpDedicatedObj["TitlerogoTexPos"][1].number_value(),
						 (float)m_mpDedicatedObj["TitlerogoTexPos"][2].number_value()};

	m_pushLClickTexPos = {(float)m_mpDedicatedObj["PushLClickTexPos"][0].number_value(),
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
// ＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝

// ゲームモードのUI関連の処理＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
void GameUi::Init(std::weak_ptr<json11::Json> a_wpJsonObj)
{
	m_mpUiSharedObj = (*a_wpJsonObj.lock())["Ui"].object_items();
	m_mpDedicatedObj = m_mpUiSharedObj["GameUi"].object_items();

	m_screenAlphaFadeSpeed = static_cast<float>(m_mpUiSharedObj["ScreenAlphaFadeSpeed"].number_value());
	m_pushLClickAlphaFadeSpeed = static_cast<float>(m_mpUiSharedObj["PushLClickAlphaFadeSpeed"].number_value());
	m_mouseRadius = m_mpUiSharedObj["MouseRadius"].int_value();
	m_mouseHalfHeight = m_mpUiSharedObj["MouseHalfHeight"].int_value();
	m_titleBarHeight = m_mpUiSharedObj["TitleBarHeight"].int_value();

	m_exitTex.Load("Asset/Textures/Ui/OPTION/EXIT.png");
	m_selectTex.Load("Asset/Textures/Ui/OPTION/ModeSelect.png");
	m_operationTex.Load("Asset/Textures/Ui/OPTION/Operation.png");

	m_weaponType1Tex.Load("Asset/Textures/Ui/Game/WeaponType1.png");
	m_weaponType2Tex.Load("Asset/Textures/Ui/Game/WeaponType2.png");
	m_weaponTypeOvreDarkTex.Load("Asset/Textures/Ui/Game/WeaponTypeOvreDark.png");

	m_torionTex.Load("Asset/Textures/Ui/Game/Torion.png");
	m_torionBarTex.Load("Asset/Textures/Ui/Game/TorionBar.png");

	m_enduranceBarTex.Load("Asset/Textures/Ui/Game/enduranceBar.png");
	m_enduranceTex.Load("Asset/Textures/Ui/Game/endurance.png");

	m_hopperDataTex.Load("Asset/Textures/Ui/Game/hopperData.png");
	m_weaponDataTex.Load("Asset/Textures/Ui/Game/weaponData.png");
	m_weaponInfoTex.Load("Asset/Textures/Ui/Game/weaponInfo.png");
	m_leftYaiTex.Load("Asset/Textures/Ui/Game/leftYaji.png");
	m_rightYaiTex.Load("Asset/Textures/Ui/Game/rightYaji.png");
	m_howToTex.Load("Asset/Textures/Ui/Game/how-to.png");

	m_backTex.Load("Asset/Textures/Ui/shared/Back.png");

	m_countOneTex.Load("Asset/Textures/Ui/Game/one.png");
	m_countTwoTex.Load("Asset/Textures/Ui/Game/two.png");
	m_countThreeTex.Load("Asset/Textures/Ui/Game/three.png");
	m_countGoTex.Load("Asset/Textures/Ui/Game/GO.png");

	m_timeTex[0].Load("Asset/Textures/Ui/shared/Zero.png");
	m_timeTex[1].Load("Asset/Textures/Ui/shared/One.png");
	m_timeTex[2].Load("Asset/Textures/Ui/shared/Two.png");
	m_timeTex[3].Load("Asset/Textures/Ui/shared/Three.png");
	m_timeTex[4].Load("Asset/Textures/Ui/shared/Four.png");
	m_timeTex[5].Load("Asset/Textures/Ui/shared/Five.png");
	m_timeTex[6].Load("Asset/Textures/Ui/shared/Six.png");
	m_timeTex[7].Load("Asset/Textures/Ui/shared/Seven.png");
	m_timeTex[8].Load("Asset/Textures/Ui/shared/Eight.png");
	m_timeTex[9].Load("Asset/Textures/Ui/shared/Nine.png");
	m_conmaTex.Load("Asset/Textures/Ui/shared/Conma.png");

	m_PointTex[0].Load("Asset/Textures/Ui/Result/p0.png");
	m_PointTex[1].Load("Asset/Textures/Ui/Result/p1.png");
	m_PointTex[2].Load("Asset/Textures/Ui/Result/p2.png");

	m_EnemyDirectionArrowLTex.Load("Asset/Textures/Ui/Game/enemyDirectionArrowL.png");
	m_EnemyDirectionArrowRTex.Load("Asset/Textures/Ui/Game/enemyDirectionArrowR.png");
	m_EnemyDirectionArrowUTex.Load("Asset/Textures/Ui/Game/enemyDirectionArrowU.png");
	m_EnemyDirectionArrowBTex.Load("Asset/Textures/Ui/Game/enemyDirectionArrowB.png");
	m_waveTex.Load("Asset/Textures/Ui/Game/wave.png");

	m_lWeaponChangeKeyTex.Load("Asset/Textures/Ui/Game/Q-Key.png");
	m_rWeaponChangeKeyTex.Load("Asset/Textures/Ui/Game/E-Key.png");

	m_lWeaponChangeKeyPushStateTex.Load("Asset/Textures/Ui/Game/PushStateQ-Key.png");
	m_rWeaponChangeKeyPushStateTex.Load("Asset/Textures/Ui/Game/PushStateE-Key.png");


	m_addFadeAlpha = false;
	m_time = 0;

	m_fadeAlpha = 0.0f;
	m_exitScale = 1.0f;
	m_selectScale = 1.0f;

	m_selectPos = { 0,0,0 };
	m_exitPos = {static_cast<float>(m_mpDedicatedObj["ExitButtomPos"][0].number_value()),
				 static_cast<float>(m_mpDedicatedObj["ExitButtomPos"][1].number_value()),
				 static_cast<float>(m_mpDedicatedObj["ExitButtomPos"][2].number_value())};

	m_backPos = {static_cast<float>(m_mpDedicatedObj["BackButtomPos"][0].number_value()),
				 static_cast<float>(m_mpDedicatedObj["BackButtomPos"][1].number_value()),
				 static_cast<float>(m_mpDedicatedObj["BackButtomPos"][2].number_value())};

	m_backScale = 0.8f;

	m_countOneScale = 0.1f;
	m_countOneAlpha = 0.0f;

	m_countTwoScale = 0.1f;
	m_countTwoAlpha = 0.0f;

	m_countThreeScale = 0.1f;
	m_countThreeAlpha = 0.0f;

	m_countGoScale = 0.1f;
	m_countGoAlpha = 0.0f;

	m_gameTimeCntDeray = 0;

	m_waveScale = 0.1f;
	m_waveAlpha = 0.0f;
	m_waveTimeCnt = 0;
	m_waveCnt = 0;

	m_bTABKey = false;

	m_bWeaponDataPage = false;
	m_bWeaponDataHopperPage = false;
	m_bWeaponDataScoPage = true;
	m_bHowToPage = true;

	m_weaponLeftYaiScale = 1.0f;
	m_weaponLeftYaiPos = {static_cast<float>(m_mpDedicatedObj["WeaponLeftYaiButtomPos"][0].number_value()),
				          static_cast<float>(m_mpDedicatedObj["WeaponLeftYaiButtomPos"][1].number_value()),
				          static_cast<float>(m_mpDedicatedObj["WeaponLeftYaiButtomPos"][2].number_value())};

	m_weaponRightYaiScale = 1.0f;
	m_weaponRightYaiPos = {static_cast<float>(m_mpDedicatedObj["WeaponRightYaiButtomPos"][0].number_value()),
						   static_cast<float>(m_mpDedicatedObj["WeaponRightYaiButtomPos"][1].number_value()),
						   static_cast<float>(m_mpDedicatedObj["WeaponRightYaiButtomPos"][2].number_value())};

	m_weaOrHowLeftYaiScale = 1.0f;
	m_weaOrHowLeftYaiPos = {static_cast<float>(m_mpDedicatedObj["WeaOrHowLeftYaiButtomPos"][0].number_value()),
						    static_cast<float>(m_mpDedicatedObj["WeaOrHowLeftYaiButtomPos"][1].number_value()),
						    static_cast<float>(m_mpDedicatedObj["WeaOrHowLeftYaiButtomPos"][2].number_value())};

	m_weaOrHowRightYaiScale = 1.0f;
	m_weaOrHowRightYaiPos = {static_cast<float>(m_mpDedicatedObj["WeaOrHowRightYaiButtomPos"][0].number_value()),
						     static_cast<float>(m_mpDedicatedObj["WeaOrHowRightYaiButtomPos"][1].number_value()),
						     static_cast<float>(m_mpDedicatedObj["WeaOrHowRightYaiButtomPos"][2].number_value())};

	m_operationScale = 1.0f;
	m_operationPos = {static_cast<float>(m_mpDedicatedObj["OperationButtomPos"][0].number_value()),
					  static_cast<float>(m_mpDedicatedObj["OperationButtomPos"][1].number_value()),
				      static_cast<float>(m_mpDedicatedObj["OperationButtomPos"][2].number_value())};

	m_bOperation = false;
	m_bFirstExit = true;
	m_scaleFadeSpeed = static_cast<float>(m_mpDedicatedObj["ScaleFadeSpeed"].number_value());

	m_charFadeSpeed = static_cast<float>(m_mpDedicatedObj["CharFadeSpeed"].number_value());
	m_charLastFadeOutSpeed = static_cast<float>(m_mpDedicatedObj["CharLastFadeOutSpeed"].number_value());

}

void GameUi::Update()
{
	if (m_time == 0)
	{
		if (SceneManager::Instance().GetSceneType() == SceneManager::SceneType::challenge)
		{
			m_gameTimeS10 = 0;
			m_gameTimeS1 = 0;

			m_gameTimeM10 = 1;
			m_gameTimeM1 = 0;
		}
		else
		{
			m_gameTimeS10 = 0;
			m_gameTimeS1 = 0;

			m_gameTimeM10 = 0;
			m_gameTimeM1 = 5;
		}
	}

	if (m_time <= m_mpDedicatedObj["MaxTime"].int_value())
	{
		m_time++;
	}

	if (SceneManager::Instance().GetSceneType() == SceneManager::SceneType::battle)
	{
		if (m_time >= 0 && m_time < m_mpDedicatedObj["WaveTexDepartureTime"][0].int_value())
		{
			m_waveScale += m_scaleFadeSpeed;
			if (m_waveScale > 1.0f)
			{
				m_waveScale = 1.0f;
			}

			m_waveAlpha += m_charFadeSpeed;
			if (m_waveAlpha > 1.0f)
			{
				m_waveAlpha = 1.0f;
			}
		}
		else if (m_time >= m_mpDedicatedObj["WaveTexDepartureTime"][0].int_value() && 
			     m_time < m_mpDedicatedObj["WaveTexDepartureTime"][1].int_value())
		{
			m_waveAlpha -= m_charLastFadeOutSpeed;
			if (m_waveAlpha < 0.0f)
			{
				m_waveAlpha = 0.0f;
			}
		}
	}

	if (m_time >= 0 && m_time < m_mpDedicatedObj["CountFadeTime"][0].int_value())
	{
		m_countThreeScale += m_scaleFadeSpeed;
		if (m_countThreeScale > 1.0f)
		{
			m_countThreeScale = 1.0f;
		}

		m_countThreeAlpha += m_charFadeSpeed;
		if (m_countThreeAlpha > 1.0f)
		{
			m_countThreeAlpha = 1.0f;
		}
	}
	else if (m_time >= m_mpDedicatedObj["CountFadeTime"][0].int_value() && 
		     m_time <  m_mpDedicatedObj["CountFadeTime"][1].int_value())
	{

		m_countThreeAlpha -= m_charFadeSpeed;
		if (m_countThreeAlpha < 0.0f)
		{
			m_countThreeAlpha = 0.0f;
		}
	}
	else if (m_time >= m_mpDedicatedObj["CountFadeTime"][1].int_value() && 
		     m_time <  m_mpDedicatedObj["CountFadeTime"][2].int_value())
	{
		m_countTwoScale += m_scaleFadeSpeed;
		if (m_countTwoScale > 1.0f)
		{
			m_countTwoScale = 1.0f;
		}

		m_countTwoAlpha += m_charFadeSpeed;
		if (m_countTwoAlpha > 1.0f)
		{
			m_countTwoAlpha = 1.0f;
		}
	}
	else if (m_time >= m_mpDedicatedObj["CountFadeTime"][2].int_value() && 
		     m_time <  m_mpDedicatedObj["CountFadeTime"][3].int_value())
	{
		m_countTwoAlpha -= m_charFadeSpeed;
		if (m_countThreeAlpha < 0.0f)
		{
			m_countThreeAlpha = 0.0f;
		}
	}
	else if (m_time >= m_mpDedicatedObj["CountFadeTime"][3].int_value() &&
		     m_time <  m_mpDedicatedObj["CountFadeTime"][4].int_value())
	{
		m_countOneScale += m_scaleFadeSpeed;
		if (m_countOneScale > 1.0f)
		{
			m_countOneScale = 1.0f;
		}

		m_countOneAlpha += m_charFadeSpeed;
		if (m_countOneAlpha > 1.0f)
		{
			m_countOneAlpha = 1.0f;
		}
	}
	else if (m_time >= m_mpDedicatedObj["CountFadeTime"][4].int_value() && 
		     m_time <  m_mpDedicatedObj["CountFadeTime"][5].int_value())
	{
		m_countOneAlpha -= m_charFadeSpeed;
		if (m_countOneAlpha < 0.0f)
		{
			m_countOneAlpha = 0.0f;
		}
	}
	else if (m_time >= m_mpDedicatedObj["CountFadeTime"][5].int_value() &&
		     m_time <  m_mpDedicatedObj["CountFadeTime"][6].int_value())
	{
		m_countGoScale += m_scaleFadeSpeed;
		if (m_countGoScale > 1.0f)
		{
			m_countGoScale = 1.0f;
		}

		m_countGoAlpha += m_charFadeSpeed;
		if (m_countGoAlpha > 1.0f)
		{
			m_countGoAlpha = 1.0f;
		}
	}
	else if (m_time >= m_mpDedicatedObj["CountFadeTime"][6].int_value() &&
		     m_time <  m_mpDedicatedObj["CountFadeTime"][7].int_value())
	{
		m_countGoAlpha -= m_charFadeSpeed;
		if (m_countGoAlpha < 0.0f)
		{
			m_countGoAlpha = 0.0f;
			SetCursorPos(640, 360);
		}
	}


	if (m_time > m_mpDedicatedObj["CountFadeTime"][7].int_value())
	{

		if (SceneManager::Instance().GetSceneType() == SceneManager::SceneType::battle)
		{
			if (m_bWaveChange)
			{
				++m_waveTimeCnt;
				if (m_waveTimeCnt >= 0 && m_waveTimeCnt < m_mpDedicatedObj["CountFadeTime"][2].int_value())
				{
					m_waveScale += m_scaleFadeSpeed;
					if (m_waveScale > 1.0f)
					{
						m_waveScale = 1.0f;
					}

					m_waveAlpha += m_charFadeSpeed;
					if (m_waveAlpha > 1.0f)
					{
						m_waveAlpha = 1.0f;
					}
				}
				else if (m_waveTimeCnt >= m_mpDedicatedObj["CountFadeTime"][2].int_value() && 
					     m_waveTimeCnt <  m_mpDedicatedObj["CountFadeTime"][5].int_value())
				{
					m_waveAlpha -= m_charLastFadeOutSpeed;
					if (m_waveAlpha < 0.0f)
					{
						m_waveAlpha = 0.0f;
					}
				}
				else if (m_waveTimeCnt >= m_mpDedicatedObj["CountFadeTime"][5].int_value())
				{
					m_bWaveChange = false;
					m_waveTimeCnt = 0;
				}
			}
		}

		if (GetAsyncKeyState(VK_TAB) & 0x8000)
		{
			if (!m_bTABKey)
			{
				m_bTABKey = true;
				if (!m_bOption)
				{
					m_bOption = true;
					KdAudioManager::Instance().Play("Asset/Audio/SE/OpenMenu.wav");
					ShowCursor(true); // マウスカーソルを消す
					KdEffekseerManager::GetInstance().OnPauseEfkUpdate();
				}
				else if (m_bOption)
				{
					m_bOption = false;
					m_bWeaponDataPage = false;
					m_bWeaponDataHopperPage = false;
					m_bWeaponDataScoPage = true;
					m_bHowToPage = true;
					m_bOperation = false;
					SetCursorPos(640, 360);
					KdAudioManager::Instance().Play("Asset/Audio/SE/OpenMenu.wav");
					ShowCursor(false); // マウスカーソルを消す
					KdEffekseerManager::GetInstance().OnResumeEfkUpdate();
				}
			}
		}
		else
		{
			m_bTABKey = false;
		}

		if (m_bOption)
		{
			OptionUpdate();
		}
		else
		{
			if (m_gameTimeCntDeray >= 60)
			{
				m_gameTimeCntDeray = 0;
				if (m_gameTimeS1 == 0)
				{
					if (m_gameTimeS10 == 0)
					{
						if (m_gameTimeM1 == 0)
						{
							if (m_gameTimeM10 != 0)
							{
								m_gameTimeM10--;
								m_gameTimeM1 = 9;
								m_gameTimeS10 = 5;
								m_gameTimeS1 = 9;
							}
						}
						else
						{
							m_gameTimeM1--;
							m_gameTimeS10 = 5;
							m_gameTimeS1 = 9;
						}
					}
					else
					{
						m_gameTimeS10--;
						m_gameTimeS1 = 9;
					}
				}
				else
				{
					m_gameTimeS1--;
				}
			}
			else
			{
				++m_gameTimeCntDeray;
			}
		}
	}
}

void GameUi::PostUpdate()
{
	auto it = m_wpEnemyList.begin();
	while (it != m_wpEnemyList.end())
	{
		// 不要になったオブジェクトを消す
		if ((*it).expired())
		{
			// 消す
			it = m_wpEnemyList.erase(it);
		}
		else
		{
			++it; // 次へ
		}
	}

	int i = 0;
	for (auto& list : m_wpEnemyList)
	{
		Math::Vector3 pos = Math::Vector3(list.lock()->GetPos().x, list.lock()->GetPos().y + list.lock()->GetRockOnPos().y + list.lock()->GetAddHpPosY(), list.lock()->GetPos().z);

		POINT dev;
		KdDirect3D::Instance().WorldToClient(pos, dev, m_wpCamera.lock()->WorkCamera()->GetCameraMatrix(), m_wpCamera.lock()->WorkCamera()->GetProjMatrix());
		dev.x -= (long)640.0f;
		dev.y = (long)(dev.y * -1 + 360.0f);
		dev.x -= 50;
		m_enemyScPosList[i] = Math::Vector2((float)dev.x, (float)dev.y);
		++i;
	}
}

void GameUi::DrawSprite()
{
	Math::Matrix transMat = Math::Matrix::Identity;
	Math::Matrix mat = Math::Matrix::Identity;
	KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
	Math::Color color = {};
	Math::Rectangle rc = {};
	std::shared_ptr<Player> spPlayer = m_wpPlayer.lock();

	if (SceneManager::Instance().GetSceneType() == SceneManager::SceneType::battle)
	{
		if (m_time >= 0 && m_time < m_mpDedicatedObj["CountFadeTime"][5].int_value())
		{
			transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["WaveTexPos"][0].number_value()),
				                                       static_cast<float>(m_mpDedicatedObj["WaveTexPos"][1].number_value()), 
				                                       static_cast<float>(m_mpDedicatedObj["WaveTexPos"][2].number_value()));

			mat = Math::Matrix::CreateScale(m_waveScale) * transMat;
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			rc = { 0,0,static_cast<int>(m_waveTex.GetWidth()), static_cast<int>(m_waveTex.GetHeight()) };
			color = { 1,1,1,m_waveAlpha };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_waveTex, 0, 0, static_cast<int>(m_waveTex.GetWidth()), static_cast<int>(m_waveTex.GetHeight()), &rc, &color);

			transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["WaveCntTexPos"][0].number_value()),
				                                       static_cast<float>(m_mpDedicatedObj["WaveCntTexPos"][1].number_value()),
				                                       static_cast<float>(m_mpDedicatedObj["WaveCntTexPos"][2].number_value()));

			mat = Math::Matrix::CreateScale(m_waveScale) * transMat;
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			rc = { 0,0,static_cast<int>(m_PointTex[1].GetWidth()), static_cast<int>(m_PointTex[1].GetHeight()) };
			color = { 1,1,1,m_waveAlpha };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_PointTex[1], 0, 0, static_cast<int>(m_PointTex[1].GetWidth()), static_cast<int>(m_PointTex[1].GetHeight()), &rc, &color);

		}

		if (m_waveCnt >= 2)
		{
			if (m_waveTimeCnt >= 0 && m_waveTimeCnt < m_mpDedicatedObj["CountFadeTime"][5].int_value())
			{
				transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["WaveTexPos"][0].number_value()),
					                                       static_cast<float>(m_mpDedicatedObj["WaveTexPos"][1].number_value()),
					                                       static_cast<float>(m_mpDedicatedObj["WaveTexPos"][2].number_value()));

				mat = Math::Matrix::CreateScale(m_waveScale) * transMat;
				KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
				rc = { 0,0,static_cast<int>(m_waveTex.GetWidth()), static_cast<int>(m_waveTex.GetHeight()) };
				color = { 1,1,1,m_waveAlpha };
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_waveTex, 0, 0, static_cast<int>(m_waveTex.GetWidth()), static_cast<int>(m_waveTex.GetHeight()), &rc, &color);

				transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["WaveCntTexPos"][0].number_value()),
					                                       static_cast<float>(m_mpDedicatedObj["WaveCntTexPos"][1].number_value()),
					                                       static_cast<float>(m_mpDedicatedObj["WaveCntTexPos"][2].number_value()));

				mat = Math::Matrix::CreateScale(m_waveScale) * transMat;
				KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
				rc = { 0,0,static_cast<int>(m_PointTex[2].GetWidth()), static_cast<int>(m_PointTex[2].GetHeight()) };
				color = { 1,1,1,m_waveAlpha };
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_PointTex[2], 0, 0, static_cast<int>(m_PointTex[2].GetWidth()), static_cast<int>(m_PointTex[2].GetHeight()), &rc, &color);
			}
		}
	}

	transMat = Math::Matrix::Identity;

	if (m_time >= 0 && m_time < m_mpDedicatedObj["CountFadeTime"][1].int_value())
	{
		mat = Math::Matrix::CreateScale(m_countThreeScale) * transMat;
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		rc = { 0,0,static_cast<int>(m_countThreeTex.GetWidth()), static_cast<int>(m_countThreeTex.GetHeight()) };
		color = { 1,1,1,m_countThreeAlpha };
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_countThreeTex, 0, 0, static_cast<int>(m_countThreeTex.GetWidth()), static_cast<int>(m_countThreeTex.GetHeight()), &rc, &color);
	}
	else if (m_time >= m_mpDedicatedObj["CountFadeTime"][1].int_value() && m_time < m_mpDedicatedObj["CountFadeTime"][3].int_value())
	{
		mat = Math::Matrix::CreateScale(m_countTwoScale) * transMat;
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		rc = { 0,0,static_cast<int>(m_countTwoTex.GetWidth()), static_cast<int>(m_countTwoTex.GetHeight()) };
		color = { 1,1,1,m_countTwoAlpha };
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_countTwoTex, 0, 0, static_cast<int>(m_countTwoTex.GetWidth()), static_cast<int>(m_countTwoTex.GetHeight()), &rc, &color);
	}
	else if (m_time >= m_mpDedicatedObj["CountFadeTime"][3].int_value() && m_time < m_mpDedicatedObj["CountFadeTime"][5].int_value())
	{
		mat = Math::Matrix::CreateScale(m_countOneScale) * transMat;
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		rc = { 0,0,static_cast<int>(m_countOneTex.GetWidth()), static_cast<int>(m_countOneTex.GetHeight()) };
		color = { 1,1,1,m_countOneAlpha };
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_countOneTex, 0, 0, static_cast<int>(m_countOneTex.GetWidth()), static_cast<int>(m_countOneTex.GetHeight()), &rc, &color);
	}
	else if (m_time >= m_mpDedicatedObj["CountFadeTime"][5].int_value() && m_time < m_mpDedicatedObj["CountFadeTime"][7].int_value())
	{
		mat = Math::Matrix::CreateScale(m_countGoScale) * transMat;
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		rc = { 0,0,static_cast<int>(m_countGoTex.GetWidth()),static_cast<int>(m_countGoTex.GetHeight()) };
		color = { 1,1,1,m_countGoAlpha };
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_countGoTex, 0, 0, static_cast<int>(m_countGoTex.GetWidth()), static_cast<int>(m_countGoTex.GetHeight()), &rc, &color);
	}


	if (spPlayer)
	{
		transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["WeaponTypeOneTexPos_R"][0].number_value()),
			static_cast<float>(m_mpDedicatedObj["WeaponTypeOneTexPos_R"][1].number_value()),
			static_cast<float>(m_mpDedicatedObj["WeaponTypeOneTexPos_R"][2].number_value()));

		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponType1Tex, 0, 0, m_weaponType1Tex.GetWidth(), m_weaponType1Tex.GetHeight());
		if (spPlayer->GetWeaponType() & Player::WeaponType::grassHopper)
		{
			rc = { 0,0,static_cast<int>(m_weaponTypeOvreDarkTex.GetWidth()),static_cast<int>(m_weaponTypeOvreDarkTex.GetHeight()) };
			color = { 1,1,1,0.6f };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponTypeOvreDarkTex, 0, 0, static_cast<int>(m_weaponTypeOvreDarkTex.GetWidth()), static_cast<int>(m_weaponTypeOvreDarkTex.GetHeight()), &rc, &color);
		}

		transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoTexPos_R"][0].number_value()),
			static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoTexPos_R"][1].number_value()),
			static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoTexPos_R"][2].number_value()));

		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponType2Tex, 0, 0, static_cast<int>(m_weaponType2Tex.GetWidth()), static_cast<int>(m_weaponType2Tex.GetHeight()));
		if (spPlayer->GetWeaponType() & Player::WeaponType::scorpion)
		{
			transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoOvreDarkTexPos_R"][0].number_value()),
				static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoOvreDarkTexPos_R"][1].number_value()),
				static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoOvreDarkTexPos_R"][2].number_value()));

			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			rc = { 0,0,static_cast<int>(m_weaponTypeOvreDarkTex.GetWidth()),static_cast<int>(m_weaponTypeOvreDarkTex.GetHeight()) };
			color = { 1,1,1,0.6f };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponTypeOvreDarkTex, 0, 0, static_cast<int>(m_weaponTypeOvreDarkTex.GetWidth()), static_cast<int>(m_weaponTypeOvreDarkTex.GetHeight()), &rc, &color, Math::Vector2(0, 0.5f));
		}
		else if (spPlayer->GetWeaponType() & Player::WeaponType::grassHopper && spPlayer->GetRGrassHopperPauCnt() != 0)
		{
			transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoOvreDarkTexPos_R"][0].number_value()),
				static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoOvreDarkTexPos_R"][1].number_value()),
				static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoOvreDarkTexPos_R"][2].number_value()));

			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			rc = { 0,0,spPlayer->GetRGrassHopperPauCnt() * 4,static_cast<int>(m_weaponTypeOvreDarkTex.GetHeight()) };
			color = { 1,1,1,0.6f };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponTypeOvreDarkTex, 0, 0, spPlayer->GetRGrassHopperPauCnt() * 4, static_cast<int>(m_weaponTypeOvreDarkTex.GetHeight()), &rc, &color, Math::Vector2(0, 0.5f));
		}

		if (KdInputManager::Instance().GetButtonState("rWeaponChange"))
		{
			transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["WeaponChangeKeyPushStateTexPos_R"][0].number_value()),
				static_cast<float>(m_mpDedicatedObj["WeaponChangeKeyPushStateTexPos_R"][1].number_value()),
				static_cast<float>(m_mpDedicatedObj["WeaponChangeKeyPushStateTexPos_R"][2].number_value()));

			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_rWeaponChangeKeyPushStateTex, 0, 0, static_cast<int>(m_rWeaponChangeKeyPushStateTex.GetWidth()), static_cast<int>(m_rWeaponChangeKeyPushStateTex.GetHeight()));
		}
		else
		{
			transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["WeaponChangeKeyPushStateTexPos_R"][0].number_value()),
				static_cast<float>(m_mpDedicatedObj["WeaponChangeKeyPushStateTexPos_R"][1].number_value()),
				static_cast<float>(m_mpDedicatedObj["WeaponChangeKeyPushStateTexPos_R"][2].number_value()));

			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_rWeaponChangeKeyTex, 0, 0, static_cast<int>(m_rWeaponChangeKeyTex.GetWidth()), static_cast<int>(m_rWeaponChangeKeyTex.GetHeight()));
		}

		transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["WeaponTypeOneTexPos_L"][0].number_value()),
			static_cast<float>(m_mpDedicatedObj["WeaponTypeOneTexPos_L"][1].number_value()),
			static_cast<float>(m_mpDedicatedObj["WeaponTypeOneTexPos_L"][2].number_value()));

		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponType1Tex, 0, 0, static_cast<int>(m_weaponType1Tex.GetWidth()), static_cast<int>(m_weaponType1Tex.GetHeight()));
		if (spPlayer->GetWeaponType() & Player::WeaponType::lGrassHopper)
		{
			rc = { 0,0,static_cast<int>(m_weaponTypeOvreDarkTex.GetWidth()),static_cast<int>(m_weaponTypeOvreDarkTex.GetHeight()) };
			color = { 1,1,1,0.6f };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponTypeOvreDarkTex, 0, 0, static_cast<int>(m_weaponTypeOvreDarkTex.GetWidth()), static_cast<int>(m_weaponTypeOvreDarkTex.GetHeight()), &rc, &color);
		}

		transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoTexPos_L"][0].number_value()),
			static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoTexPos_L"][1].number_value()),
			static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoTexPos_L"][2].number_value()));

		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponType2Tex, 0, 0, static_cast<int>(m_weaponType2Tex.GetWidth()), static_cast<int>(m_weaponType2Tex.GetHeight()));
		if (spPlayer->GetWeaponType() & Player::WeaponType::lScorpion)
		{
			transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoOvreDarkTexPos_L"][0].number_value()),
				static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoOvreDarkTexPos_L"][1].number_value()),
				static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoOvreDarkTexPos_L"][2].number_value()));

			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			rc = { 0,0,static_cast<int>(m_weaponTypeOvreDarkTex.GetWidth()), static_cast<int>(m_weaponTypeOvreDarkTex.GetHeight()) };
			color = { 1,1,1,0.6f };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponTypeOvreDarkTex, 0, 0, static_cast<int>(m_weaponTypeOvreDarkTex.GetWidth()), static_cast<int>(m_weaponTypeOvreDarkTex.GetHeight()), &rc, &color, Math::Vector2(0, 0.5f));
		}
		else if (spPlayer->GetWeaponType() & Player::WeaponType::lGrassHopper && spPlayer->GetLGrassHopperPauCnt() != 0)
		{
			transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoOvreDarkTexPos_L"][0].number_value()),
				static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoOvreDarkTexPos_L"][1].number_value()),
				static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoOvreDarkTexPos_L"][2].number_value()));

			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			rc = { 0,0,spPlayer->GetLGrassHopperPauCnt() * 4,static_cast<int>(m_weaponTypeOvreDarkTex.GetHeight()) };
			color = { 1,1,1,0.6f };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponTypeOvreDarkTex, 0, 0, spPlayer->GetLGrassHopperPauCnt() * 4, static_cast<int>(m_weaponTypeOvreDarkTex.GetHeight()), &rc, &color, Math::Vector2(0, 0.5f));
		}

		if (KdInputManager::Instance().GetButtonState("lWeaponChange"))
		{
			transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["WeaponChangeKeyPushStateTexPos_L"][0].number_value()),
				static_cast<float>(m_mpDedicatedObj["WeaponChangeKeyPushStateTexPos_L"][1].number_value()),
				static_cast<float>(m_mpDedicatedObj["WeaponChangeKeyPushStateTexPos_L"][2].number_value()));

			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_lWeaponChangeKeyPushStateTex, 0, 0, static_cast<int>(m_lWeaponChangeKeyPushStateTex.GetWidth()), static_cast<int>(m_lWeaponChangeKeyPushStateTex.GetHeight()));
		}
		else
		{
			transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["WeaponChangeKeyPushStateTexPos_L"][0].number_value()),
				static_cast<float>(m_mpDedicatedObj["WeaponChangeKeyPushStateTexPos_L"][1].number_value()),
				static_cast<float>(m_mpDedicatedObj["WeaponChangeKeyPushStateTexPos_L"][2].number_value()));

			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_lWeaponChangeKeyTex, 0, 0, static_cast<int>(m_lWeaponChangeKeyTex.GetWidth()), static_cast<int>(m_lWeaponChangeKeyTex.GetHeight()));
		}

		transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["enduranceRelationTexPos"][0].number_value()),
			static_cast<float>(m_mpDedicatedObj["enduranceRelationTexPos"][1].number_value()),
			static_cast<float>(m_mpDedicatedObj["enduranceRelationTexPos"][2].number_value()));

		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		rc = { 0,0,static_cast<int>(m_enduranceBarTex.GetWidth()),static_cast<int>(m_enduranceBarTex.GetHeight()) };
		color = { 1, 1, 1, 1 };
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_enduranceBarTex, 0, 0, static_cast<int>(m_enduranceBarTex.GetWidth()), static_cast<int>(m_enduranceBarTex.GetHeight()), &rc, &color, Math::Vector2(0, 0.5f));

		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		rc = { 0,0,static_cast<int>((spPlayer->GetEndurance())), static_cast<int>(m_enduranceTex.GetHeight()) };
		color = { 1, 1, 1, 1 };
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_enduranceTex, 0, 0, static_cast<int>((spPlayer->GetEndurance())), static_cast<int>(m_enduranceTex.GetHeight()), &rc, &color, Math::Vector2(0, 0.5f));

		transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["TorionBarTexPos"][0].number_value()),
			static_cast<float>(m_mpDedicatedObj["TorionBarTexPos"][1].number_value()),
			static_cast<float>(m_mpDedicatedObj["TorionBarTexPos"][2].number_value()));

		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		rc = { 0,0,static_cast<int>(m_torionBarTex.GetWidth()),static_cast<int>(m_torionBarTex.GetHeight()) };
		color = { 1, 1, 1, 1 };
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_torionBarTex, 0, 0, static_cast<int>(m_torionBarTex.GetWidth()), static_cast<int>(m_torionBarTex.GetHeight()), &rc, &color, Math::Vector2(0, 0.5f));

		transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["TorionTexPos"][0].number_value()),
			static_cast<float>(m_mpDedicatedObj["TorionTexPos"][1].number_value()),
			static_cast<float>(m_mpDedicatedObj["TorionTexPos"][2].number_value()));

		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		rc = { 0,0,static_cast<int>(spPlayer->GetVForce()),static_cast<int>(m_torionTex.GetHeight()) };
		color = { 1, 1, 1, 1 };
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_torionTex, 0, 0, static_cast<int>((spPlayer->GetVForce())), static_cast<int>(m_torionTex.GetHeight()), &rc, &color, Math::Vector2(0, 0.5f));

		int i = 0;
		for (auto& list : m_wpEnemyList)
		{
			if (list.expired())continue;
			if (list.lock()->GetBEnemyDeath())
			{
				++i;
				continue;
			}

			Math::Vector3 nowVec = m_wpCamera.lock()->GetMatrix().Backward();
			nowVec.Normalize();

			// 向きたい方向
			Math::Vector3 toVec = list.lock()->GetPos() - m_wpCamera.lock()->GetPos();
			toVec.Normalize();

			Math::Vector3 dot = DirectX::XMVector3Dot(nowVec, toVec);
			if (dot.x > 1)
			{
				dot.x = 1;
			}
			if (dot.x < -1)
			{
				dot.x = -1;
			}

			// 角度を取得
			float ang = DirectX::XMConvertToDegrees(acos(dot.x));

			if (ang <= 85)
			{
				mat = Math::Matrix::CreateScale(0.2f) * Math::Matrix::CreateTranslation(m_enemyScPosList[i].x, m_enemyScPosList[i].y, 0.0f);

				KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
				rc = { 0,0,static_cast<int>(list.lock()->GetEndurance()),static_cast<int>(m_enduranceTex.GetHeight()) };
				color = { 1, 1, 1, 1 };
				if (SceneManager::Instance().GetSceneType() == SceneManager::SceneType::challenge)
				{
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_enduranceTex, 0, 0, static_cast<int>(list.lock()->GetEndurance() * 2.6f), static_cast<int>(m_enduranceTex.GetHeight()), &rc, &color, Math::Vector2(0, 0.5f));
				}
				else
				{
					if (list.lock()->GetBBoss())
					{
						KdShaderManager::Instance().m_spriteShader.DrawTex(&m_enduranceTex, 0, 0, static_cast<int>((list.lock()->GetEndurance())), static_cast<int>(m_enduranceTex.GetHeight()), &rc, &color, Math::Vector2(0, 0.5f));
					}
					else
					{
						KdShaderManager::Instance().m_spriteShader.DrawTex(&m_enduranceTex, 0, 0, static_cast<int>(list.lock()->GetEndurance() * (static_cast<float>(m_mpDedicatedObj["EnduranceTexWidth"].number_value()) / list.lock()->GetMaxEndurance())), 
							                                               static_cast<int>(m_enduranceTex.GetHeight()), &rc, &color, Math::Vector2(0, 0.5f));
					}
				}

				rc = { 0,0,400,50 };
				color = { 1, 1, 1, 1 };
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_enduranceBarTex, 0, 0, static_cast<int>(m_enduranceBarTex.GetWidth()), static_cast<int>(m_enduranceBarTex.GetHeight()), &rc, &color, Math::Vector2(0, 0.5f));
			}

			if (ang >= 45)
			{
				float m_heightDifference = list.lock()->GetPos().y - m_wpPlayer.lock()->GetPos().y; // Y座標の差

				if (m_heightDifference >= 2.0f) // 敵がプレイヤーの上にいるとき
				{
					mat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["AroowUpAndDownPos"][0].number_value()),
						                                  static_cast<float>(m_mpDedicatedObj["AroowUpAndDownPos"][1].number_value()),
						                                  static_cast<float>(m_mpDedicatedObj["AroowUpAndDownPos"][2].number_value()));

					KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
					rc = { 0,0,static_cast<int>(m_EnemyDirectionArrowUTex.GetWidth()), static_cast<int>(m_EnemyDirectionArrowUTex.GetHeight()) };
					color = { 1, 1, 1, 1 };
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_EnemyDirectionArrowUTex, 0, 0, static_cast<int>(m_EnemyDirectionArrowUTex.GetWidth()), static_cast<int>(m_EnemyDirectionArrowUTex.GetHeight()), &rc, &color, Math::Vector2(0, 0.5f));
				}
				else if (m_heightDifference <= -2.0f) // 敵がプレイヤーの下にいるとき
				{
					mat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["AroowUpAndDownPos"][0].number_value()),
						                                 -static_cast<float>(m_mpDedicatedObj["AroowUpAndDownPos"][1].number_value()),
						                                  static_cast<float>(m_mpDedicatedObj["AroowUpAndDownPos"][2].number_value()));

					KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
					rc = { 0,0,static_cast<int>(m_EnemyDirectionArrowBTex.GetWidth()), static_cast<int>(m_EnemyDirectionArrowBTex.GetHeight()) };
					color = { 1, 1, 1, 1 };
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_EnemyDirectionArrowBTex, 0, 0, static_cast<int>(m_EnemyDirectionArrowBTex.GetWidth()), static_cast<int>(m_EnemyDirectionArrowBTex.GetHeight()), &rc, &color, Math::Vector2(0, 0.5f));
				}


				Math::Vector3 cross = DirectX::XMVector3Cross(nowVec, toVec);

				if (cross.y < 0) // 敵がプレイヤーの左側にいるとき
				{
					mat = Math::Matrix::CreateTranslation(-static_cast<float>(m_mpDedicatedObj["AroowLeftAndRightPos"][0].number_value()),
						                                   static_cast<float>(m_mpDedicatedObj["AroowLeftAndRightPos"][1].number_value()),
						                                   static_cast<float>(m_mpDedicatedObj["AroowLeftAndRightPos"][2].number_value()));

					KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
					rc = { 0,0, static_cast<int>(m_EnemyDirectionArrowLTex.GetWidth()), static_cast<int>(m_EnemyDirectionArrowLTex.GetHeight()) };
					color = { 1, 1, 1, 1 };
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_EnemyDirectionArrowLTex, 0, 0, static_cast<int>(m_EnemyDirectionArrowLTex.GetWidth()), static_cast<int>(m_EnemyDirectionArrowLTex.GetHeight()), &rc, &color, Math::Vector2(0, 0.5f));
				}
				else if (cross.y >= 0) // 敵がプレイヤーの右側にいるとき
				{
					mat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["AroowLeftAndRightPos"][0].number_value()),
						                                  static_cast<float>(m_mpDedicatedObj["AroowLeftAndRightPos"][1].number_value()),
						                                  static_cast<float>(m_mpDedicatedObj["AroowLeftAndRightPos"][2].number_value()));

					KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
					rc = { 0,0,static_cast<int>(m_EnemyDirectionArrowRTex.GetWidth()), static_cast<int>(m_EnemyDirectionArrowRTex.GetHeight()) };
					color = { 1, 1, 1, 1 };
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_EnemyDirectionArrowRTex, 0, 0, static_cast<int>(m_EnemyDirectionArrowRTex.GetWidth()), static_cast<int>(m_EnemyDirectionArrowRTex.GetHeight()), &rc, &color, Math::Vector2(0, 0.5f));
				}

			}
			++i;
		}
	}

	if (m_bOption)
	{
		transMat = Math::Matrix::Identity;
		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		color = { 0,0,0,0.6f };
		KdShaderManager::Instance().m_spriteShader.DrawBox(0, 0, 1280, 720, &color);

		if (!m_bOperation)
		{
			mat = Math::Matrix::CreateScale(m_operationScale) * Math::Matrix::CreateTranslation(m_operationPos);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_operationTex, 0, 0, static_cast<int>(m_operationTex.GetWidth()), static_cast<int>(m_operationTex.GetHeight()));

			mat = Math::Matrix::CreateScale(m_selectScale) * Math::Matrix::CreateTranslation(m_selectPos);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_selectTex, 0, 0, static_cast<int>(m_selectTex.GetWidth()), static_cast<int>(m_selectTex.GetHeight()));

			mat = Math::Matrix::CreateScale(m_exitScale) * Math::Matrix::CreateTranslation(m_exitPos);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_exitTex, 0, 0, static_cast<int>(m_exitTex.GetWidth()), static_cast<int>(m_exitTex.GetHeight()));
		}
		else if (m_bOperation)
		{
			if (m_bHowToPage)
			{
				transMat = Math::Matrix::Identity;
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_howToTex, 0, 0, static_cast<int>(m_howToTex.GetWidth()), static_cast<int>(m_howToTex.GetHeight()));
			}

			if (m_bWeaponDataPage)
			{
				if (m_bWeaponDataHopperPage)
				{
					transMat = Math::Matrix::Identity;
					KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_hopperDataTex, 0, 0, static_cast<int>(m_hopperDataTex.GetWidth()), static_cast<int>(m_hopperDataTex.GetHeight()));
				}
				else if (m_bWeaponDataScoPage)
				{
					transMat = Math::Matrix::Identity;
					KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponInfoTex, 0, 0, static_cast<int>(m_weaponInfoTex.GetWidth()), static_cast<int>(m_weaponInfoTex.GetHeight()));
				}

				mat = Math::Matrix::CreateScale(m_weaponRightYaiScale) * Math::Matrix::CreateTranslation(m_weaponRightYaiPos);
				KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_rightYaiTex, 0, 0, static_cast<int>(m_rightYaiTex.GetWidth()), static_cast<int>(m_rightYaiTex.GetHeight()));

				mat = Math::Matrix::CreateScale(m_weaponLeftYaiScale) * Math::Matrix::CreateTranslation(m_weaponLeftYaiPos);
				KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_leftYaiTex, 0, 0, static_cast<int>(m_leftYaiTex.GetWidth()), static_cast<int>(m_leftYaiTex.GetHeight()));
			}

			mat = Math::Matrix::CreateScale(m_weaOrHowRightYaiScale) * Math::Matrix::CreateTranslation(m_weaOrHowRightYaiPos);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_rightYaiTex, 0, 0, static_cast<int>(m_rightYaiTex.GetWidth()), static_cast<int>(m_rightYaiTex.GetHeight()));

			mat = Math::Matrix::CreateScale(m_weaOrHowLeftYaiScale) * Math::Matrix::CreateTranslation(m_weaOrHowLeftYaiPos);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_leftYaiTex, 0, 0, static_cast<int>(m_leftYaiTex.GetWidth()), static_cast<int>(m_leftYaiTex.GetHeight()));

		}

		mat = Math::Matrix::CreateScale(m_backScale) * Math::Matrix::CreateTranslation(m_backPos);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_backTex, 0, 0, 182, 80);
	}

	mat = Math::Matrix::CreateTranslation(-static_cast<float>(m_mpDedicatedObj["GameTime10"][0].number_value()),
		                                   static_cast<float>(m_mpDedicatedObj["GameTime10"][1].number_value()),
		                                   static_cast<float>(m_mpDedicatedObj["GameTime10"][2].number_value()));

	KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_timeTex[m_gameTimeM10], 0, 0, static_cast<int>(m_timeTex[m_gameTimeM10].GetWidth()), static_cast<int>(m_timeTex[m_gameTimeM10].GetHeight()));

	mat = Math::Matrix::CreateTranslation(-static_cast<float>(m_mpDedicatedObj["GameTime1"][0].number_value()),
		                                   static_cast<float>(m_mpDedicatedObj["GameTime1"][1].number_value()),
		                                   static_cast<float>(m_mpDedicatedObj["GameTime1"][2].number_value()));

	KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_timeTex[m_gameTimeM1], 0, 0, static_cast<int>(m_timeTex[m_gameTimeM1].GetWidth()), static_cast<int>(m_timeTex[m_gameTimeM1].GetHeight()));

	mat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["GameConmaTex"][0].number_value()),
		                                  static_cast<float>(m_mpDedicatedObj["GameConmaTex"][1].number_value()),
		                                  static_cast<float>(m_mpDedicatedObj["GameConmaTex"][2].number_value()));

	KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_conmaTex, 0, 0, static_cast<int>(m_conmaTex.GetWidth()), static_cast<int>(m_conmaTex.GetHeight()));

	mat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["GameTime1"][0].number_value()),
		                                  static_cast<float>(m_mpDedicatedObj["GameTime1"][1].number_value()),
		                                  static_cast<float>(m_mpDedicatedObj["GameTime1"][2].number_value()));

	KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_timeTex[m_gameTimeS10], 0, 0, static_cast<int>(m_timeTex[m_gameTimeS10].GetWidth()), static_cast<int>(m_timeTex[m_gameTimeS10].GetHeight()));

	mat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["GameTime10"][0].number_value()),
		                                  static_cast<float>(m_mpDedicatedObj["GameTime10"][1].number_value()),
		                                  static_cast<float>(m_mpDedicatedObj["GameTime10"][2].number_value()));

	KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_timeTex[m_gameTimeS1], 0, 0, static_cast<int>(m_timeTex[m_gameTimeS1].GetWidth()), static_cast<int>(m_timeTex[m_gameTimeS1].GetHeight()));

	transMat = Math::Matrix::Identity;
	KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
	color = { 0,0,0,m_fadeAlpha };
	KdShaderManager::Instance().m_spriteShader.DrawBox(0, 0, 1280, 720, &color);
}

void GameUi::AddEnemy(std::shared_ptr<Enemy> a_spEnemy)
{
	m_wpEnemyList.push_back(a_spEnemy);
	m_enemyScPosList.push_back(Math::Vector2::Zero);
}

void GameUi::OptionUpdate()
{
	if (!m_bOperation)
	{
		if (ButtomProcessing({ m_exitPos.x,m_exitPos.y }, m_exitTex, m_exitScale))
		{
			if (!m_addFadeAlpha)
			{
				m_bExit = true;
				m_addFadeAlpha = true;
				m_bFirstExit = true;
				KdAudioManager::Instance().Play("Asset/Audio/SE/PushButton.wav");
			}
		}

		if (ButtomProcessing({ m_selectPos.x,m_selectPos.y }, m_selectTex, m_selectScale))
		{
			m_bSelect = true;
			m_addFadeAlpha = true;
			KdAudioManager::Instance().Play("Asset/Audio/SE/PushButton.wav");
		}

		if (ButtomProcessing({ m_operationPos.x,m_operationPos.y }, m_operationTex, m_operationScale))
		{
			m_bOperation = true;
			KdAudioManager::Instance().Play("Asset/Audio/SE/PushButton.wav");
		}

		if (ButtomProcessing({ m_backPos.x,m_backPos.y }, m_backTex, m_backScale))
		{
			m_bOption = false;
			ShowCursor(false); // マウスカーソルを消す
			SetCursorPos(640, 360);
			KdAudioManager::Instance().Play("Asset/Audio/SE/backPush.wav");
			KdEffekseerManager::GetInstance().OnResumeEfkUpdate();
		}

		if (m_addFadeAlpha)
		{
			m_fadeAlpha += (1.0f / 90);
			if (m_fadeAlpha >= 1.0f)
			{
				m_fadeAlpha = 1.0f;

				if (m_bExit)
				{
					if (m_bFirstExit)
					{
						SceneManager::Instance().SetBWinEnd();
						m_bFirstExit = false;
					}
					else
					{
						m_bExit = false;
						m_addFadeAlpha = false;
						m_bFirstExit = true;
						m_fadeAlpha = 0.0f;
					}
				}

				if (m_bSelect)
				{
					KdEffekseerManager::GetInstance().Reset();
					SceneManager::Instance().SetNextScene
					(
						SceneManager::SceneType::select
					);
				}
			}
		}
	}
	else
	{
		if (ButtomProcessing({ m_backPos.x,m_backPos.y }, m_backTex, m_backScale))
		{
			m_bWeaponDataPage = false;
			m_bWeaponDataHopperPage = false;
			m_bWeaponDataScoPage = true;
			m_bHowToPage = true;
			m_bOperation = false;
			KdAudioManager::Instance().Play("Asset/Audio/SE/backPush.wav");
		}

		if (ButtomProcessing({ m_weaOrHowLeftYaiPos.x,m_weaOrHowLeftYaiPos.y }, m_leftYaiTex, m_weaOrHowLeftYaiScale))
		{
			if (m_bWeaponDataPage)
			{
				m_bHowToPage = true;
				m_bWeaponDataPage = false;
			}
			else
			{
				m_bHowToPage = false;
				m_bWeaponDataPage = true;
			}
			KdAudioManager::Instance().Play("Asset/Audio/SE/PushArrow.wav");
		}

		if (ButtomProcessing({ m_weaOrHowRightYaiPos.x,m_weaOrHowRightYaiPos.y }, m_rightYaiTex, m_weaOrHowRightYaiScale))
		{
			if (m_bWeaponDataPage)
			{
				m_bHowToPage = true;
				m_bWeaponDataPage = false;
			}
			else
			{
				m_bHowToPage = false;
				m_bWeaponDataPage = true;
			}
			KdAudioManager::Instance().Play("Asset/Audio/SE/PushArrow.wav");
		}

		if (m_bWeaponDataPage)
		{
			if (ButtomProcessing({m_weaponLeftYaiPos.x,m_weaponLeftYaiPos.y }, m_leftYaiTex, m_weaponLeftYaiScale))
			{
				if (m_bWeaponDataHopperPage)
				{
					m_bWeaponDataScoPage = true;
					m_bWeaponDataHopperPage = false;
				}
				else
				{
					m_bWeaponDataScoPage = false;
					m_bWeaponDataHopperPage = true;
				}
				KdAudioManager::Instance().Play("Asset/Audio/SE/PushArrow.wav");
			}

			if (ButtomProcessing({ m_weaponRightYaiPos.x,m_weaponRightYaiPos.y }, m_rightYaiTex, m_weaponRightYaiScale))
			{
				if (m_bWeaponDataHopperPage)
				{
					m_bWeaponDataScoPage = true;
					m_bWeaponDataHopperPage = false;
				}
				else
				{
					m_bWeaponDataScoPage = false;
					m_bWeaponDataHopperPage = true;
				}
				KdAudioManager::Instance().Play("Asset/Audio/SE/PushArrow.wav");
			}
		}
	}
}
// ＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝

// リザルトのUI関連の処理＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
void ResultUi::Init(std::weak_ptr<json11::Json> a_wpJsonObj)
{
	m_mpUiSharedObj = (*a_wpJsonObj.lock())["Ui"].object_items();
	m_mpDedicatedObj = m_mpUiSharedObj["ResultUi"].object_items();

	m_screenAlphaFadeSpeed = static_cast<float>(m_mpUiSharedObj["ScreenAlphaFadeSpeed"].number_value());
	m_pushLClickAlphaFadeSpeed = static_cast<float>(m_mpUiSharedObj["PushLClickAlphaFadeSpeed"].number_value());
	m_mouseRadius = m_mpUiSharedObj["MouseRadius"].int_value();
	m_mouseHalfHeight = m_mpUiSharedObj["MouseHalfHeight"].int_value();
	m_titleBarHeight = m_mpUiSharedObj["TitleBarHeight"].int_value();

	m_addFadeAlpha = false;
	m_fadeAlpha = 0.0f;
	std::ifstream ifs("Asset/Data/PointAndRank.csv");

	if (!ifs)
	{
		return;
	}

	std::string str, filed;
	std::string aStr[4];
	int i = 0;
	while (i < 4)
	{
		std::getline(ifs, str);
		std::istringstream ss(str);
		while (std::getline(ss, filed, ','))
		{
			aStr[i] = filed;
			++i;
		}
	}

	m_weaponPoint = stoi(aStr[1]);
	m_rank = aStr[3];

	m_pointAddOrSubVal = SceneManager::Instance().GetPointAddOrSubVal();

	m_winTex.Load("Asset/Textures/Ui/Result/WIN.png");
	m_lossTex.Load("Asset/Textures/Ui/Result/LOSS.png");
	m_pushLClickTex.Load("Asset/Textures/Ui/shared/PUSHTLCLICK.png");
	m_PointTex[0].Load("Asset/Textures/Ui/Result/p0.png");
	m_PointTex[1].Load("Asset/Textures/Ui/Result/p1.png");
	m_PointTex[2].Load("Asset/Textures/Ui/Result/p2.png");
	m_PointTex[3].Load("Asset/Textures/Ui/Result/p3.png");
	m_PointTex[4].Load("Asset/Textures/Ui/Result/p4.png");
	m_PointTex[5].Load("Asset/Textures/Ui/Result/p5.png");
	m_PointTex[6].Load("Asset/Textures/Ui/Result/p6.png");
	m_PointTex[7].Load("Asset/Textures/Ui/Result/p7.png");
	m_PointTex[8].Load("Asset/Textures/Ui/Result/p8.png");
	m_PointTex[9].Load("Asset/Textures/Ui/Result/p9.png");

	m_winCharaTex.Load("Asset/Textures/Ui/Result/winChara.png");
	m_winPlayerCharaTex.Load("Asset/Textures/Ui/Result/winPlayerChara.png");
	m_losePlayerCharaTex.Load("Asset/Textures/Ui/Result/LosePlayerChara.png");
	m_loseCharaTex.Load("Asset/Textures/Ui/Result/loseChara.png");

	m_time = 0;
	m_bSceneCangePossible = false;
	m_pushLClickAlpha = 0.0f;
	m_bPushLClickAlphaAdd = false;
}

void ResultUi::Update()
{
	m_time++;
	if (m_time >= m_mpDedicatedObj["MaxTime"].int_value())
	{
		m_time = m_mpDedicatedObj["MaxTime"].int_value();
	}

	if (m_time >= m_mpDedicatedObj["PointAddMoment"].int_value())
	{
		if (SceneManager::Instance().GetBAddOrSubVal())
		{
			if (m_pointAddOrSubVal > 0)
			{
				if (m_time == m_mpDedicatedObj["PointAddMoment"].int_value())
				{
					KdAudioManager::Instance().Play("Asset/Audio/SE/AddAndSub.wav", true);
				}

				m_pointAddOrSubVal--;
				m_weaponPoint++;
				if (m_weaponPoint >= m_mpDedicatedObj["MaxWeaponPoint"].int_value())
				{
					m_weaponPoint = m_mpDedicatedObj["MaxWeaponPoint"].int_value();
				}
			}
			else if (m_pointAddOrSubVal == 0 && !m_bSceneCangePossible)
			{
				KdAudioManager::Instance().StopAllSound();
				if (SceneManager::Instance().GetBPlayerWin())
				{
					KdAudioManager::Instance().Play("Asset/Audio/BGM/7 Fanfare and Victory Lap Loop.wav", true);
				}
				else
				{
					KdAudioManager::Instance().Play("Asset/Audio/BGM/8 Defeat loop.wav", true);
				}
				KdAudioManager::Instance().Play("Asset/Audio/SE/asCompletion.wav");
				std::ofstream ofs("Asset/Data/PointAndRank.csv"); // 書き込み
				if (!ofs) { return; }
				if (m_weaponPoint < m_mpDedicatedObj["BLevelMaxPoint"].int_value())
				{
					ofs << "point" << ',' << m_weaponPoint << ',' << '\n' << "rank" << ',' << "C" << ',' << '\n';
				}
				else if (m_weaponPoint >= m_mpDedicatedObj["BLevelMaxPoint"].int_value() && m_weaponPoint < m_mpDedicatedObj["ALevelBorderPoint"].int_value())
				{
					ofs << "point" << ',' << m_weaponPoint << ',' << '\n' << "rank" << ',' << "B" << ',' << '\n';
				}
				else if (m_weaponPoint >= m_mpDedicatedObj["ALevelBorderPoint"].int_value())
				{
					ofs << "point" << ',' << m_weaponPoint << ',' << '\n' << "rank" << ',' << "A" << ',' << '\n' << "Skill" << ',' << "マスター" << '\n';
				}
				ofs.close();
				m_bSceneCangePossible = true;
			}
		}
		else
		{
			if (m_pointAddOrSubVal > 0)
			{
				if (m_time == m_mpDedicatedObj["PointAddMoment"].int_value())
				{
					KdAudioManager::Instance().Play("Asset/Audio/SE/AddAndSub.wav", true);
				}

				m_pointAddOrSubVal--;
				m_weaponPoint--;
				if (m_weaponPoint <= 0)
				{
					m_weaponPoint = 0;
				}
			}
			else if (m_pointAddOrSubVal == 0 && !m_bSceneCangePossible)
			{
				KdAudioManager::Instance().StopAllSound();
				if (SceneManager::Instance().GetBPlayerWin())
				{
					KdAudioManager::Instance().Play("Asset/Audio/BGM/7 Fanfare and Victory Lap Loop.wav", true);
				}
				else
				{
					KdAudioManager::Instance().Play("Asset/Audio/BGM/8 Defeat loop.wav", true);
				}
				KdAudioManager::Instance().Play("Asset/Audio/SE/asCompletion.wav");
				std::ofstream ofs("Asset/Data/PointAndRank.csv"); // 書き込み
				if (!ofs) { return; }
				if (m_weaponPoint < m_mpDedicatedObj["BLevelMaxPoint"].int_value())
				{
					ofs << "point" << ',' << m_weaponPoint << ',' << '\n' << "rank" << ',' << "C" << ',' << '\n';
				}
				else if (m_weaponPoint >= m_mpDedicatedObj["BLevelMaxPoint"].int_value() && m_weaponPoint < m_mpDedicatedObj["ALevelBorderPoint"].int_value())
				{
					ofs << "point" << ',' << m_weaponPoint << ',' << '\n' << "rank" << ',' << "B" << ',' << '\n';
				}
				else if (m_weaponPoint >= m_mpDedicatedObj["ALevelBorderPoint"].int_value())
				{
					ofs << "point" << ',' << m_weaponPoint << ',' << '\n' << "rank" << ',' << "A" << ',' << '\n' << "Skill" << ',' << "マスター" << '\n';
				}
				ofs.close();
				m_bSceneCangePossible = true;
			}
		}

		if (KdInputManager::Instance().IsPress("select"))
		{
			if (m_pointAddOrSubVal > 0)
			{
				if (SceneManager::Instance().GetBAddOrSubVal())
				{
					m_weaponPoint += m_pointAddOrSubVal;
					if (m_weaponPoint >= m_mpDedicatedObj["MaxWeaponPoint"].int_value())
					{
						m_weaponPoint = m_mpDedicatedObj["MaxWeaponPoint"].int_value();
					}
				}
				else
				{
					m_weaponPoint -= m_pointAddOrSubVal;
					if (m_weaponPoint <= 0)
					{
						m_weaponPoint = 0;
					}
				}

				m_pointAddOrSubVal = 0;
			}
		}
	}

	if (m_bSceneCangePossible)
	{
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
	}

	if (m_addFadeAlpha)
	{
		m_fadeAlpha += m_screenAlphaFadeSpeed;
		if (m_fadeAlpha >= 1.0f)
		{
			m_fadeAlpha = 1.0f;

			SceneManager::Instance().SetNextScene
			(
				SceneManager::SceneType::title
			);
		}
	}
}

void ResultUi::DrawSprite()
{
	Math::Matrix transMat = Math::Matrix::Identity;
	Math::Matrix mat = Math::Matrix::Identity;
	KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
	Math::Color color = {};

	if (SceneManager::Instance().GetBPlayerWin())
	{
		transMat = Math::Matrix::Identity;
		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		color = { static_cast<float>(m_mpDedicatedObj["PlayerWinBackColor"][0].number_value()),
			      static_cast<float>(m_mpDedicatedObj["PlayerWinBackColor"][1].number_value()),
			      static_cast<float>(m_mpDedicatedObj["PlayerWinBackColor"][2].number_value()),
			      1.0f};

		KdShaderManager::Instance().m_spriteShader.DrawBox(0, 0, 1280, 720, &color);

	}
	else
	{
		transMat = Math::Matrix::Identity;
		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		color = { static_cast<float>(m_mpDedicatedObj["PlayerLoseBackColor"][0].number_value()),
				  static_cast<float>(m_mpDedicatedObj["PlayerLoseBackColor"][1].number_value()),
				  static_cast<float>(m_mpDedicatedObj["PlayerLoseBackColor"][2].number_value()),
				  1.0f };

		KdShaderManager::Instance().m_spriteShader.DrawBox(0, 0, 1280, 720, &color);
	}

	if (m_time >= 60)
	{
		int ieftover = 0;
		int ieftoverExcess = 0;

		if (SceneManager::Instance().GetBPlayerWin())
		{
			transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["WinTexPos"][0].number_value()),
				                                       static_cast<float>(m_mpDedicatedObj["WinTexPos"][1].number_value()),
				                                       static_cast<float>(m_mpDedicatedObj["WinTexPos"][2].number_value()));

			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_winTex, 0, 0, static_cast<int>(m_winTex.GetWidth()), static_cast<int>(m_winTex.GetHeight()));

			transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["WinPlayerCharaTexPos"][0].number_value()),
				                                       static_cast<float>(m_mpDedicatedObj["WinPlayerCharaTexPos"][1].number_value()),
				                                       static_cast<float>(m_mpDedicatedObj["WinPlayerCharaTexPos"][2].number_value()));

			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_winPlayerCharaTex, 0, 0, static_cast<int>(m_winPlayerCharaTex.GetWidth()), static_cast<int>(m_winPlayerCharaTex.GetHeight()));

			switch (SceneManager::Instance().GetEnemyTotal())
			{
			case 1:
				transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["EnemyKillsTotalOneLoseEnemyCharaTexPos"][0].number_value()),
					                                       static_cast<float>(m_mpDedicatedObj["EnemyKillsTotalOneLoseEnemyCharaTexPos"][1].number_value()),
					                                       static_cast<float>(m_mpDedicatedObj["EnemyKillsTotalOneLoseEnemyCharaTexPos"][2].number_value()));

				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, static_cast<int>(m_loseCharaTex.GetWidth()), static_cast<int>(m_loseCharaTex.GetHeight()));
				break;
			case 2:
				switch (SceneManager::Instance().GetEnemyIeftover())
				{
				case 0:
					if (m_time >= 80)
					{
						transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["EnemyKillsTotalTwoWhenToLoseEnemyCharaTexPos_One"][0].number_value()),
							                                       static_cast<float>(m_mpDedicatedObj["EnemyKillsTotalTwoWhenToLoseEnemyCharaTexPos_One"][1].number_value()),
							                                       static_cast<float>(m_mpDedicatedObj["EnemyKillsTotalTwoWhenToLoseEnemyCharaTexPos_One"][2].number_value()));

						KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
						KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, static_cast<int>(m_loseCharaTex.GetWidth()), static_cast<int>(m_loseCharaTex.GetHeight()));
					}

					if (m_time >= 100)
					{
						transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["EnemyKillsTotalTwoWhenToLoseEnemyCharaTexPos_Two"][0].number_value()),
							                                       static_cast<float>(m_mpDedicatedObj["EnemyKillsTotalTwoWhenToLoseEnemyCharaTexPos_Two"][1].number_value()),
							                                       static_cast<float>(m_mpDedicatedObj["EnemyKillsTotalTwoWhenToLoseEnemyCharaTexPos_Two"][2].number_value()));

						KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
						KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, static_cast<int>(m_loseCharaTex.GetWidth()), static_cast<int>(m_loseCharaTex.GetHeight()));
					}
					break;
				case 1:
					transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["EnemyKillsTotalOneLoseEnemyCharaTexPos"][0].number_value()),
						                                       static_cast<float>(m_mpDedicatedObj["EnemyKillsTotalOneLoseEnemyCharaTexPos"][1].number_value()),
						                                       static_cast<float>(m_mpDedicatedObj["EnemyKillsTotalOneLoseEnemyCharaTexPos"][2].number_value()));

					KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, static_cast<int>(m_loseCharaTex.GetWidth()), static_cast<int>(m_loseCharaTex.GetHeight()));
					break;
				}
				break;
			case 3:
				switch (SceneManager::Instance().GetEnemyIeftover())
				{
				case 0:
					if (m_time >= 70)
					{
						transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["EnemyKillsTotalThreeWhenToLoseEnemyCharaTexPos_One"][0].number_value()),
							                                       static_cast<float>(m_mpDedicatedObj["EnemyKillsTotalThreeWhenToLoseEnemyCharaTexPos_One"][1].number_value()),
							                                       static_cast<float>(m_mpDedicatedObj["EnemyKillsTotalThreeWhenToLoseEnemyCharaTexPos_One"][2].number_value()));

						KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
						KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, static_cast<int>(m_loseCharaTex.GetWidth()), static_cast<int>(m_loseCharaTex.GetHeight()));
					}

					if (m_time >= 90)
					{
						transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["EnemyKillsTotalThreeWhenToLoseEnemyCharaTexPos_Two"][0].number_value()),
							                                       static_cast<float>(m_mpDedicatedObj["EnemyKillsTotalThreeWhenToLoseEnemyCharaTexPos_Two"][1].number_value()),
							                                       static_cast<float>(m_mpDedicatedObj["EnemyKillsTotalThreeWhenToLoseEnemyCharaTexPos_Two"][2].number_value()));

						KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
						KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, static_cast<int>(m_loseCharaTex.GetWidth()), static_cast<int>(m_loseCharaTex.GetHeight()));
					}

					if (m_time >= 110)
					{
						transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["EnemyKillsTotalThreeWhenToLoseEnemyCharaTexPos_Three"][0].number_value()),
							                                       static_cast<float>(m_mpDedicatedObj["EnemyKillsTotalThreeWhenToLoseEnemyCharaTexPos_Three"][1].number_value()),
							                                       static_cast<float>(m_mpDedicatedObj["EnemyKillsTotalThreeWhenToLoseEnemyCharaTexPos_Three"][2].number_value()));

						KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
						KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, static_cast<int>(m_loseCharaTex.GetWidth()), static_cast<int>(m_loseCharaTex.GetHeight()));
					}
					break;
				case 1:
					if (m_time >= 80)
					{
						transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["EnemyKillsTotalTwoWhenToLoseEnemyCharaTexPos_One"][0].number_value()),
							                                       static_cast<float>(m_mpDedicatedObj["EnemyKillsTotalTwoWhenToLoseEnemyCharaTexPos_One"][1].number_value()),
							                                       static_cast<float>(m_mpDedicatedObj["EnemyKillsTotalTwoWhenToLoseEnemyCharaTexPos_One"][2].number_value()));

						KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
						KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, static_cast<int>(m_loseCharaTex.GetWidth()), static_cast<int>(m_loseCharaTex.GetHeight()));
					}

					if (m_time >= 100)
					{
						transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["EnemyKillsTotalTwoWhenToLoseEnemyCharaTexPos_Two"][0].number_value()),
							                                       static_cast<float>(m_mpDedicatedObj["EnemyKillsTotalTwoWhenToLoseEnemyCharaTexPos_Two"][1].number_value()),
							                                       static_cast<float>(m_mpDedicatedObj["EnemyKillsTotalTwoWhenToLoseEnemyCharaTexPos_Two"][2].number_value()));

						KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
						KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, static_cast<int>(m_loseCharaTex.GetWidth()), static_cast<int>(m_loseCharaTex.GetHeight()));
					}
					break;
				case 2:
					transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["EnemyKillsTotalOneLoseEnemyCharaTexPos"][0].number_value()),
						                                       static_cast<float>(m_mpDedicatedObj["EnemyKillsTotalOneLoseEnemyCharaTexPos"][1].number_value()),
						                                       static_cast<float>(m_mpDedicatedObj["EnemyKillsTotalOneLoseEnemyCharaTexPos"][2].number_value()));

					KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, static_cast<int>(m_loseCharaTex.GetWidth()), static_cast<int>(m_loseCharaTex.GetHeight()));
					break;
				}
				break;
			case 50:
				       // 敵全体の数            // 敵の現存数               
				ieftover = (50 - SceneManager::Instance().GetEnemyIeftover()) / 5;				// 敵の撃破数を５分割した値 
				ieftoverExcess = (50 - SceneManager::Instance().GetEnemyIeftover()) % 5;        // 敵の撃破数を５分割した余り

				for (int i = 0; i < ieftover; i++)
				{
					for (int j = 0; j < 5; j++)
					{
						transMat = Math::Matrix::CreateScale(0.2f) * Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["ChallengeEnemyCharaTexPos"][0].int_value() + m_mpDedicatedObj["ChallengeFiftyTexPosAddVal"][0].int_value() * j), 
							                                                                         static_cast<float>(m_mpDedicatedObj["ChallengeEnemyCharaTexPos"][1].int_value() - m_mpDedicatedObj["ChallengeFiftyTexPosAddVal"][1].int_value() * i),
							                                                                         static_cast<float>(m_mpDedicatedObj["ChallengeEnemyCharaTexPos"][2].int_value()));
						KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
						KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, static_cast<int>(m_loseCharaTex.GetWidth()), static_cast<int>(m_loseCharaTex.GetHeight()));
					}
				}

				for (int k = 0; k < ieftoverExcess; k++)
				{
					transMat = Math::Matrix::CreateScale(0.2f) * Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["ChallengeEnemyCharaTexPos"][0].int_value() + m_mpDedicatedObj["ChallengeFiftyTexPosAddVal"][0].int_value() * k),
						                                                                         static_cast<float>(m_mpDedicatedObj["ChallengeEnemyCharaTexPos"][1].int_value() - m_mpDedicatedObj["ChallengeFiftyTexPosAddVal"][1].int_value() * ieftover),
						                                                                         static_cast<float>(m_mpDedicatedObj["ChallengeEnemyCharaTexPos"][2].int_value()));

					KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, static_cast<int>(m_loseCharaTex.GetWidth()), static_cast<int>(m_loseCharaTex.GetHeight()));
				}

				break;
			case 100:
				ieftover = (100 - SceneManager::Instance().GetEnemyIeftover()) / 5;				// 敵の撃破数を５分割した値 
				ieftoverExcess = (100 - SceneManager::Instance().GetEnemyIeftover()) % 5;;		// 敵の撃破数を５分割した余り

				for (int i = 0; i < ieftover; i++)
				{
					for (int j = 0; j < 5; j++)
					{
						transMat = Math::Matrix::CreateScale(0.075f) * Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["ChallengeEnemyCharaTexPos"][0].int_value() + m_mpDedicatedObj["ChallengeHundredTexPosAddVal"][0].int_value() * j),
																	                                   static_cast<float>(m_mpDedicatedObj["ChallengeEnemyCharaTexPos"][1].int_value() - m_mpDedicatedObj["ChallengeHundredTexPosAddVal"][1].int_value() * i),
																	                                   static_cast<float>(m_mpDedicatedObj["ChallengeEnemyCharaTexPos"][2].int_value()));

						KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
						KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, static_cast<int>(m_loseCharaTex.GetWidth()), static_cast<int>(m_loseCharaTex.GetHeight()));
					}
				}

				for (int k = 0; k < ieftoverExcess; k++)
				{
					transMat = Math::Matrix::CreateScale(0.075f) * Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["ChallengeEnemyCharaTexPos"][0].int_value() + m_mpDedicatedObj["ChallengeHundredTexPosAddVal"][0].int_value() * k),
						                                                                           static_cast<float>(m_mpDedicatedObj["ChallengeEnemyCharaTexPos"][1].int_value() - m_mpDedicatedObj["ChallengeHundredTexPosAddVal"][1].int_value() * ieftover),
						                                                                           static_cast<float>(m_mpDedicatedObj["ChallengeEnemyCharaTexPos"][2].int_value()));

					KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, static_cast<int>(m_loseCharaTex.GetWidth()), static_cast<int>(m_loseCharaTex.GetHeight()));
				}

				break;
			}

		}
		else
		{
			transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["LoseTexPos"][0].number_value()),
				                                       static_cast<float>(m_mpDedicatedObj["LoseTexPos"][1].number_value()),
				                                       static_cast<float>(m_mpDedicatedObj["LoseTexPos"][2].number_value()));

			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_lossTex, 0, 0, static_cast<int>(m_lossTex.GetWidth()), static_cast<int>(m_lossTex.GetHeight()));

			transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["LosePlayerCharaTexPos"][0].number_value()),
				                                       static_cast<float>(m_mpDedicatedObj["LosePlayerCharaTexPos"][1].number_value()),
				                                       static_cast<float>(m_mpDedicatedObj["LosePlayerCharaTexPos"][2].number_value()));

			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_losePlayerCharaTex, 0, 0, static_cast<int>(m_losePlayerCharaTex.GetWidth()), static_cast<int>(m_losePlayerCharaTex.GetHeight()));

			switch (SceneManager::Instance().GetEnemyTotal())
			{
			case 1:
				transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["EnemyTotalOneWinEnemyCharaTexPos"][0].number_value()),
					                                       static_cast<float>(m_mpDedicatedObj["EnemyTotalOneWinEnemyCharaTexPos"][1].number_value()),
					                                       static_cast<float>(m_mpDedicatedObj["EnemyTotalOneWinEnemyCharaTexPos"][2].number_value()));

				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_winCharaTex, 0, 0, static_cast<int>(m_winCharaTex.GetWidth()), static_cast<int>(m_winCharaTex.GetHeight()));
				break;
			case 2:
				if (m_time >= 80)
				{
					transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["EnemyTotalTwoWhenToWinEnemyCharaTexPos_One"][0].number_value()),
						                                       static_cast<float>(m_mpDedicatedObj["EnemyTotalTwoWhenToWinEnemyCharaTexPos_One"][1].number_value()),
						                                       static_cast<float>(m_mpDedicatedObj["EnemyTotalTwoWhenToWinEnemyCharaTexPos_One"][2].number_value()));

					KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_winCharaTex, 0, 0, static_cast<int>(m_winCharaTex.GetWidth()), static_cast<int>(m_winCharaTex.GetHeight()));
				}

				if (m_time >= 100)
				{
					transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["EnemyTotalTwoWhenToWinEnemyCharaTexPos_Two"][0].number_value()),
						                                       static_cast<float>(m_mpDedicatedObj["EnemyTotalTwoWhenToWinEnemyCharaTexPos_Two"][1].number_value()),
						                                       static_cast<float>(m_mpDedicatedObj["EnemyTotalTwoWhenToWinEnemyCharaTexPos_Two"][2].number_value()));

					KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_winCharaTex, 0, 0, static_cast<int>(m_winCharaTex.GetWidth()), static_cast<int>(m_winCharaTex.GetHeight()));
				}
				break;
			case 3:
				if (m_time >= 70)
				{
					transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["EnemyTotalThreeWhenToWinEnemyCharaTexPos_One"][0].number_value()),
						                                       static_cast<float>(m_mpDedicatedObj["EnemyTotalThreeWhenToWinEnemyCharaTexPos_One"][1].number_value()),
						                                       static_cast<float>(m_mpDedicatedObj["EnemyTotalThreeWhenToWinEnemyCharaTexPos_One"][2].number_value()));

					KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_winCharaTex, 0, 0, static_cast<int>(m_winCharaTex.GetWidth()), static_cast<int>(m_winCharaTex.GetHeight()));
				}

				if (m_time >= 90)
				{
					transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["EnemyTotalThreeWhenToWinEnemyCharaTexPos_Two"][0].number_value()),
						                                       static_cast<float>(m_mpDedicatedObj["EnemyTotalThreeWhenToWinEnemyCharaTexPos_Two"][1].number_value()),
						                                       static_cast<float>(m_mpDedicatedObj["EnemyTotalThreeWhenToWinEnemyCharaTexPos_Two"][2].number_value()));

					KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_winCharaTex, 0, 0, static_cast<int>(m_winCharaTex.GetWidth()), static_cast<int>(m_winCharaTex.GetHeight()));
				}

				if (m_time >= 110)
				{
					transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["EnemyTotalThreeWhenToWinEnemyCharaTexPos_Three"][0].number_value()),
						                                       static_cast<float>(m_mpDedicatedObj["EnemyTotalThreeWhenToWinEnemyCharaTexPos_Three"][1].number_value()),
						                                       static_cast<float>(m_mpDedicatedObj["EnemyTotalThreeWhenToWinEnemyCharaTexPos_Three"][2].number_value()));

					KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_winCharaTex, 0, 0, static_cast<int>(m_winCharaTex.GetWidth()), static_cast<int>(m_winCharaTex.GetHeight()));
				}

				break;
			case 50:
				ieftover = (50 - SceneManager::Instance().GetEnemyIeftover()) / 5;
				ieftoverExcess = (50 - SceneManager::Instance().GetEnemyIeftover()) % 5;

				for (int i = 0; i < ieftover; i++)
				{
					for (int j = 0; j < 5; j++)
					{
						transMat = Math::Matrix::CreateScale(0.2f) * Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["ChallengeEnemyCharaTexPos"][0].int_value() + m_mpDedicatedObj["ChallengeFiftyTexPosAddVal"][0].int_value() * j),
							                                                                         static_cast<float>(m_mpDedicatedObj["ChallengeEnemyCharaTexPos"][1].int_value() - m_mpDedicatedObj["ChallengeFiftyTexPosAddVal"][1].int_value() * i),
							                                                                         static_cast<float>(m_mpDedicatedObj["ChallengeEnemyCharaTexPos"][2].int_value()));

						KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
						KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, static_cast<int>(m_loseCharaTex.GetWidth()), static_cast<int>(m_loseCharaTex.GetHeight()));
					}
				}

				for (int k = 0; k < ieftoverExcess; k++)
				{
					transMat = Math::Matrix::CreateScale(0.2f) * Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["ChallengeEnemyCharaTexPos"][0].int_value() + m_mpDedicatedObj["ChallengeFiftyTexPosAddVal"][0].int_value() * k),
						                                                                         static_cast<float>(m_mpDedicatedObj["ChallengeEnemyCharaTexPos"][1].int_value() - m_mpDedicatedObj["ChallengeFiftyTexPosAddVal"][1].int_value() * ieftover),
						                                                                         static_cast<float>(m_mpDedicatedObj["ChallengeEnemyCharaTexPos"][2].int_value()));

					KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, static_cast<int>(m_loseCharaTex.GetWidth()), static_cast<int>(m_loseCharaTex.GetHeight()));
				}

				break;
			case 100:
				ieftover = (50 - SceneManager::Instance().GetEnemyIeftover()) / 5;
				ieftoverExcess = (50 - SceneManager::Instance().GetEnemyIeftover()) % 5;

				for (int i = 0; i < ieftover; i++)
				{
					for (int j = 0; j < 5; j++)
					{
						transMat = Math::Matrix::CreateScale(0.075f) * Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["ChallengeEnemyCharaTexPos"][0].int_value() + m_mpDedicatedObj["ChallengeHundredTexPosAddVal"][0].int_value() * j),
							                                                                           static_cast<float>(m_mpDedicatedObj["ChallengeEnemyCharaTexPos"][1].int_value() - m_mpDedicatedObj["ChallengeHundredTexPosAddVal"][1].int_value() * i),
							                                                                           static_cast<float>(m_mpDedicatedObj["ChallengeEnemyCharaTexPos"][2].int_value()));

						KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
						KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, static_cast<int>(m_loseCharaTex.GetWidth()), static_cast<int>(m_loseCharaTex.GetHeight()));
					}
				}

				for (int k = 0; k < ieftoverExcess; k++)
				{
					transMat = Math::Matrix::CreateScale(0.075f) * Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["ChallengeEnemyCharaTexPos"][0].int_value() + m_mpDedicatedObj["ChallengeHundredTexPosAddVal"][0].int_value() * k),
						static_cast<float>(m_mpDedicatedObj["ChallengeEnemyCharaTexPos"][1].int_value() - m_mpDedicatedObj["ChallengeHundredTexPosAddVal"][1].int_value() * ieftover),
						static_cast<float>(m_mpDedicatedObj["ChallengeEnemyCharaTexPos"][2].int_value()));

					KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, static_cast<int>(m_loseCharaTex.GetWidth()), static_cast<int>(m_loseCharaTex.GetHeight()));
				}

				break;
			}
		}
	}

	if (m_time >= 120)
	{
		float xVal = static_cast<float>(m_mpDedicatedObj["WeaponPointAddOrSubVal"][0].number_value());
		for (int i = 1; i <= 1000; i *= 10)
		{
			transMat = Math::Matrix::CreateTranslation(xVal,
				                                       static_cast<float>(m_mpDedicatedObj["WeaponPointAddOrSubVal"][1].number_value()),
				                                       static_cast<float>(m_mpDedicatedObj["WeaponPointAddOrSubVal"][2].number_value()));

			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_PointTex[(m_pointAddOrSubVal / i) % 10], 0, 0, static_cast<int>(m_PointTex[(m_pointAddOrSubVal / i) % 10].GetWidth()),
				                                                                                                 static_cast<int>(m_PointTex[(m_pointAddOrSubVal / i) % 10].GetHeight()));

			xVal -= 80;
		}
	}

	if (m_time >= 180)
	{
		float xVal = static_cast<float>(m_mpDedicatedObj["WeaponPointTexPos"][0].number_value());
		for (int i = 1; i <= 10000; i *= 10)
		{
			transMat = Math::Matrix::CreateTranslation(xVal,
				                                       static_cast<float>(m_mpDedicatedObj["WeaponPointTexPos"][1].number_value()),
				                                       static_cast<float>(m_mpDedicatedObj["WeaponPointTexPos"][2].number_value()));

			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_PointTex[(m_weaponPoint / i) % 10], 0, 0, static_cast<int>(m_PointTex[(m_weaponPoint / i) % 10].GetWidth()),
				                                                                                            static_cast<int>(m_PointTex[(m_weaponPoint / i) % 10].GetHeight()));

			xVal -= 80;
		}
	}

	if (m_pointAddOrSubVal == 0)
	{
		transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["PushLClickTexPos"][0].number_value()),
			                                       static_cast<float>(m_mpDedicatedObj["PushLClickTexPos"][1].number_value()),
			                                       static_cast<float>(m_mpDedicatedObj["PushLClickTexPos"][2].number_value()));

		Math::Rectangle rc = { 0,0,894,114 };
		color = { 1,1,1,m_pushLClickAlpha };
		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_pushLClickTex, 0, 0, static_cast<int>(m_pushLClickTex.GetWidth()), static_cast<int>(m_pushLClickTex.GetHeight()), &rc, &color);
	}

	transMat = Math::Matrix::Identity;
	KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
	color = { 0,0,0,m_fadeAlpha };
	KdShaderManager::Instance().m_spriteShader.DrawBox(0, 0, 1280, 720, &color);
}
// ＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝

// モードセレクトのUI関連の処理＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
void ModeSelectUi::Init(std::weak_ptr<json11::Json> a_wpJsonObj)
{
	m_mpUiSharedObj = (*a_wpJsonObj.lock())["Ui"].object_items();
	m_mpDedicatedObj = m_mpUiSharedObj["ModeSelectUi"].object_items();

	m_screenAlphaFadeSpeed = static_cast<float>(m_mpUiSharedObj["ScreenAlphaFadeSpeed"].number_value());
	m_pushLClickAlphaFadeSpeed = static_cast<float>(m_mpUiSharedObj["PushLClickAlphaFadeSpeed"].number_value());
	m_mouseRadius = m_mpUiSharedObj["MouseRadius"].int_value();
	m_mouseHalfHeight = m_mpUiSharedObj["MouseHalfHeight"].int_value();
	m_titleBarHeight = m_mpUiSharedObj["TitleBarHeight"].int_value();

	m_modeSelectTex.Load("Asset/Textures/Ui/Select/ModeSelect.png");
	m_pictureFrameTex.Load("Asset/Textures/Ui/Select/PictureFrame.png");

	m_titleTex.Load("Asset/Textures/Ui/Select/title.png");
	m_exitTex.Load("Asset/Textures/Ui/Select/exit.png");
	m_gameTex.Load("Asset/Textures/Ui/Select/Battle.png");
	m_battleCharaTex.Load("Asset/Textures/Ui/Select/BattleChara.png");
	m_challengeTex.Load("Asset/Textures/Ui/Select/Challenge.png");
	m_challengeCharaTex.Load("Asset/Textures/Ui/Select/ChallengeChara.png");
	m_selectBackTex.Load("Asset/Textures/Ui/Select/SelectBack.png");
	m_trainingTex.Load("Asset/Textures/Ui/Select/Training.png");
	m_tutorialTex.Load("Asset/Textures/Ui/Select/Tutorial.png");

	m_enemyBossTypeOneTex.Load("Asset/Textures/Ui/Select/EnemyBossTypeOne.png");
	m_enemyBossTypeTwoTotalTex.Load("Asset/Textures/Ui/Select/EnemyBossTypeTwo.png");
	m_twoEnemyBossTypeOneTex.Load("Asset/Textures/Ui/Select/twoEnemyBossTypeOne.png");

	m_helpMkTex.Load("Asset/Textures/Ui/Select/help.png");
	m_clauseMkTex.Load("Asset/Textures/Ui/Select/clause.png");
	m_battlehelpTex.Load("Asset/Textures/Ui/Select/BattleHelp.png");

	m_chalengeFiftyTex.Load("Asset/Textures/Ui/Select/Challenge50.png");
	m_chalengeHundredTex.Load("Asset/Textures/Ui/Select/Challenge100.png");

	m_addFadeAlpha = false;
	m_time = 0;
	m_fadeAlpha = 0.0f;

	m_tutorialScale = 1.0f;
	m_gameScale = 1.0f;
	m_challengeScale = 0;
	m_trainingScale = 0;

	m_battleCharaPos = {static_cast<float>(m_mpDedicatedObj["BattleCharaTexPos"][0].number_value()),
		                static_cast<float>(m_mpDedicatedObj["BattleCharaTexPos"][1].number_value()),
		                static_cast<float>(m_mpDedicatedObj["BattleCharaTexPos"][2].number_value())
};
	m_enemyBossTypeOnePos = {static_cast<float>(m_mpDedicatedObj["EnemyBossTypeOneModeTexPos"][0].number_value()),
						     static_cast<float>(m_mpDedicatedObj["EnemyBossTypeOneModeTexPos"][1].number_value()),
						     static_cast<float>(m_mpDedicatedObj["EnemyBossTypeOneModeTexPos"][2].number_value()) };

	m_enemyBossTypeTwoPos = {static_cast<float>(m_mpDedicatedObj["EnemyBossTypeTwoModeTexPos"][0].number_value()),
						     static_cast<float>(m_mpDedicatedObj["EnemyBossTypeTwoModeTexPos"][1].number_value()),
						     static_cast<float>(m_mpDedicatedObj["EnemyBossTypeTwoModeTexPos"][2].number_value()) };

	m_twoEnemyBossTypeOnePos = {static_cast<float>(m_mpDedicatedObj["TwoEnemyBossTypeOneModeTexPos"][0].number_value()),
							    static_cast<float>(m_mpDedicatedObj["TwoEnemyBossTypeOneModeTexPos"][1].number_value()),
							    static_cast<float>(m_mpDedicatedObj["TwoEnemyBossTypeOneModeTexPos"][2].number_value()) };

	m_helpMkPos = {static_cast<float>(m_mpDedicatedObj["HelpMkTexPos"][0].number_value()),
				   static_cast<float>(m_mpDedicatedObj["HelpMkTexPos"][1].number_value()),
				   static_cast<float>(m_mpDedicatedObj["HelpMkTexPos"][2].number_value())};

	m_clauseMkPos = {static_cast<float>(m_mpDedicatedObj["ClauseMkTexPos"][0].number_value()),
				     static_cast<float>(m_mpDedicatedObj["ClauseMkTexPos"][1].number_value()),
				     static_cast<float>(m_mpDedicatedObj["ClauseMkTexPos"][2].number_value())};

	m_challengeCharaPos = {static_cast<float>(m_mpDedicatedObj["ChallengeCharaTexPos"][0].number_value()),
				           static_cast<float>(m_mpDedicatedObj["ChallengeCharaTexPos"][1].number_value()),
				           static_cast<float>(m_mpDedicatedObj["ChallengeCharaTexPos"][2].number_value())};

	m_chalengeFiftyPos = {static_cast<float>(m_mpDedicatedObj["ChalengeFiftyTexPos"][0].number_value()),
				      static_cast<float>(m_mpDedicatedObj["ChalengeFiftyTexPos"][1].number_value()),
				      static_cast<float>(m_mpDedicatedObj["ChalengeFiftyTexPos"][2].number_value())};

	m_chalengeHundredPos = {static_cast<float>(m_mpDedicatedObj["ChalengeHundredTexPos"][0].number_value()),
				        static_cast<float>(m_mpDedicatedObj["ChalengeHundredTexPos"][1].number_value()),
				        static_cast<float>(m_mpDedicatedObj["ChalengeHundredTexPos"][2].number_value())};

	m_tutorialPos = {static_cast<float>(m_mpDedicatedObj["TutorialTexPos"][0].number_value()),
				     static_cast<float>(m_mpDedicatedObj["TutorialTexPos"][1].number_value()),
				     static_cast<float>(m_mpDedicatedObj["TutorialTexPos"][2].number_value())};

	m_trainingPos = {static_cast<float>(m_mpDedicatedObj["TrainingTexPos"][0].number_value()),
				     static_cast<float>(m_mpDedicatedObj["TrainingTexPos"][1].number_value()),
				     static_cast<float>(m_mpDedicatedObj["TrainingTexPos"][2].number_value())};

	m_titlePos = {static_cast<float>(m_mpDedicatedObj["TitlePos"][0].number_value()),
				  static_cast<float>(m_mpDedicatedObj["TitlePos"][1].number_value()),
				  static_cast<float>(m_mpDedicatedObj["TitlePos"][2].number_value())};

	m_exitPos = { static_cast<float>(m_mpDedicatedObj["ExitPos"][0].number_value()),
				  static_cast<float>(m_mpDedicatedObj["ExitPos"][1].number_value()),
				  static_cast<float>(m_mpDedicatedObj["ExitPos"][2].number_value())};

	m_battleCharaScale = 1.0f;
	m_challengeCharaScale = 1.0f;
	m_chalengeFiftyScale = 1.0f;
	m_chalengeHundredScale = 1.0f;
	m_titleScale = 1.0f;
	m_exitScale = 1.0f;

	m_bExit = false;
	m_bOption = false;
	m_bTitle = false;
	m_bFirstExit = true;


	m_bChalengeFifty = false;
	m_bChalengeHundred = false;
	m_bTutorial = false;

	m_clauseMkScale = 1.0f;

	m_helpMkScale = 1.0f;
	m_bBattlehelp = false;

	m_bChalengehelp = false;

	m_bGame = false;
	m_bEnemyBossTypeOneTotal = false;
	m_bEnemyBossTypeTwoTotal = false;
	m_bTwoEnemyBossTypeOneTotal = false;
	m_bBattleChara = false;
	m_bChallengeChara = false;
}

void ModeSelectUi::Update()
{
	if (m_bBattlehelp || m_bChalengehelp)
	{
		if (ButtomProcessing({ m_clauseMkPos.x,m_clauseMkPos.y}, m_clauseMkTex, m_clauseMkScale,1.0f,0.1f))
		{
			if (!m_addFadeAlpha)
			{
				if (m_bChalengehelp)
				{
					m_bChalengehelp = false;
				}
				else if (m_bBattlehelp)
				{
					m_bBattlehelp = false;
				}
				KdAudioManager::Instance().Play("Asset/Audio/SE/backPush.wav");
			}
		}
	}
	else
	{
		if (ButtomProcessing({ m_tutorialPos.x,m_tutorialPos.y }, m_tutorialTex, m_tutorialScale,1.0f, 0.1f))
		{
			if (!m_addFadeAlpha)
			{
				m_bTutorial = true;
				m_addFadeAlpha = true;
				KdAudioManager::Instance().Play("Asset/Audio/SE/PushButton.wav");
			}
		}

		if (ButtomProcessing({ m_exitPos.x,m_exitPos.y }, m_exitTex, m_exitScale,1.0f,0.1f))
		{
			if (!m_addFadeAlpha)
			{
				m_bExit = true;
				m_addFadeAlpha = true;
				m_bFirstExit = true;
				KdAudioManager::Instance().Play("Asset/Audio/SE/PushButton.wav");
			}
		}

		if (ButtomProcessing({ m_battleCharaPos.x,m_battleCharaPos.y }, m_battleCharaTex, m_battleCharaScale, 1.0f, 0.1f))
		{
			if (!m_addFadeAlpha)
			{
				m_bBattleChara = !m_bBattleChara;

				if (m_bChallengeChara)
				{
					m_bChallengeChara = false;
				}
				KdAudioManager::Instance().Play("Asset/Audio/SE/PushButton.wav");
			}
		}

		if (m_bBattleChara)
		{
			if (ButtomProcessing({ m_enemyBossTypeOnePos.x,m_enemyBossTypeOnePos.y }, m_enemyBossTypeOneTex, m_enemyBossTypeOneScale, 1.0f, 0.1f))
			{
				if (!m_addFadeAlpha)
				{
					m_bEnemyBossTypeOneTotal = true;
					m_bGame = true;
					m_addFadeAlpha = true;
					KdAudioManager::Instance().Play("Asset/Audio/SE/PushButton.wav");
				}
			}
			
			if (ButtomProcessing({ m_enemyBossTypeTwoPos.x,m_enemyBossTypeTwoPos.y }, m_enemyBossTypeTwoTotalTex, m_enemyBossTypeTwoScale, 1.0f, 0.1f))
			{
				if (!m_addFadeAlpha)
				{
					m_bEnemyBossTypeTwoTotal = true;
					m_bGame = true;
					m_addFadeAlpha = true;
					KdAudioManager::Instance().Play("Asset/Audio/SE/PushButton.wav");
				}
			}
	
			if (ButtomProcessing({ m_twoEnemyBossTypeOnePos.x,m_twoEnemyBossTypeOnePos.y }, m_twoEnemyBossTypeOneTex, m_twoEnemyBossTypeOneScale, 1.0f, 0.1f))
			{
				if (!m_addFadeAlpha)
				{
					m_bTwoEnemyBossTypeOneTotal = true;
					m_bGame = true;
					m_addFadeAlpha = true;
					KdAudioManager::Instance().Play("Asset/Audio/SE/PushButton.wav");
				}
			}
		}

		if (ButtomProcessing({ m_challengeCharaPos.x,m_challengeCharaPos.y }, m_challengeCharaTex, m_challengeCharaScale, 1.0f, 0.1f))
		{
			if (!m_addFadeAlpha)
			{
				m_bChallengeChara = !m_bChallengeChara;

				if (m_bBattleChara)
				{
					m_bBattleChara = false;
				}
				KdAudioManager::Instance().Play("Asset/Audio/SE/PushButton.wav");
			}
		}

		if (m_bChallengeChara)
		{
			if (ButtomProcessing({ m_chalengeFiftyPos.x,m_chalengeFiftyPos.y }, m_chalengeFiftyTex, m_chalengeFiftyScale, 1.0f, 0.1f))
			{
				if (!m_addFadeAlpha)
				{
					m_bChalengeFifty = true;
					m_addFadeAlpha = true;
					KdAudioManager::Instance().Play("Asset/Audio/SE/PushButton.wav");
				}
			}

			if (ButtomProcessing({ m_chalengeHundredPos.x,m_chalengeHundredPos.y }, m_chalengeHundredTex, m_chalengeHundredScale, 1.0f, 0.1f))
			{
				if (!m_addFadeAlpha)
				{
					m_bChalengeHundred = true;
					m_addFadeAlpha = true;
					KdAudioManager::Instance().Play("Asset/Audio/SE/PushButton.wav");
				}
			}
		}

		if (ButtomProcessing({ m_trainingPos.x,m_trainingPos.y }, m_trainingTex, m_trainingScale, 1.0f, 0.1f))
		{
			if (!m_addFadeAlpha)
			{
				m_bTraining = true;
				m_addFadeAlpha = true;
				KdAudioManager::Instance().Play("Asset/Audio/SE/PushButton.wav");
			}
		}

		if (ButtomProcessing({ m_titlePos.x,m_titlePos.y }, m_titleTex, m_titleScale, 1.0f, 0.1f))
		{
			if (!m_addFadeAlpha)
			{
				m_bTitle = true;
				m_addFadeAlpha = true;
				KdAudioManager::Instance().Play("Asset/Audio/SE/PushButton.wav");
			}
		}

		if (ButtomProcessing({ m_helpMkPos.x,m_helpMkPos.y }, m_helpMkTex, m_helpMkScale, 1.0f, 0.1f))
		{
			if (!m_addFadeAlpha)
			{
				if (m_bChallengeChara)
				{
					//m_bBattlehelp = true;
				}
				else if (m_bBattleChara)
				{
					m_bBattlehelp = true;
				}
				KdAudioManager::Instance().Play("Asset/Audio/SE/PushButton.wav");
			}
		}
	}

	if (m_addFadeAlpha)
	{
		m_fadeAlpha += (1.0f / 90);
		if (m_fadeAlpha >= 1.0f)
		{
			m_fadeAlpha = 1.0f;

			if (m_bTitle)
			{
				SceneManager::Instance().SetNextScene
				(
					SceneManager::SceneType::title
				);
			}

			if (m_bGame)
			{
				if (m_bEnemyBossTypeOneTotal)
				{
					SceneManager::Instance().SetEnemyTotal(1);
					SceneManager::Instance().SetEnemyIeftover(1);
					SceneManager::Instance().SetBHumanoidEnemy(true);
				}
				else if (m_bEnemyBossTypeTwoTotal)
				{
					SceneManager::Instance().SetEnemyTotal(1);
					SceneManager::Instance().SetEnemyIeftover(1);
					SceneManager::Instance().SetBHumanoidEnemy(false);
				}
				else if (m_bTwoEnemyBossTypeOneTotal)
				{
					SceneManager::Instance().SetEnemyTotal(2);
					SceneManager::Instance().SetEnemyIeftover(2);
					SceneManager::Instance().SetBHumanoidEnemy(true);
				}

				SceneManager::Instance().SetNextScene
				(
					SceneManager::SceneType::battle
				);
			}

			if (m_bTutorial)
			{
				SceneManager::Instance().SetNextScene
				(
					SceneManager::SceneType::tutorial
				);
			}

			if (m_bTraining)
			{
				SceneManager::Instance().SetEnemyTotal(1);
				SceneManager::Instance().SetEnemyIeftover(1);
				SceneManager::Instance().SetBHumanoidEnemy(true);

				SceneManager::Instance().SetNextScene
				(
					SceneManager::SceneType::training
				);

			}

			if (m_bExit)
			{
				if (m_bFirstExit)
				{
					SceneManager::Instance().SetBWinEnd();
					m_bFirstExit = false;
				}
				else
				{
					m_bExit = false;
					m_addFadeAlpha = false;
					m_fadeAlpha = 0.0f;
				}
			}

			if (m_bChalengeFifty)
			{
				SceneManager::Instance().SetEnemyTotal(50);
				SceneManager::Instance().SetEnemyIeftover(50);

				SceneManager::Instance().SetNextScene
				(
					SceneManager::SceneType::challenge
				);
			}

			if (m_bChalengeHundred)
			{
				SceneManager::Instance().SetEnemyTotal(100);
				SceneManager::Instance().SetEnemyIeftover(100);

				SceneManager::Instance().SetNextScene
				(
					SceneManager::SceneType::challenge
				);
			}
		}
	}
}

void ModeSelectUi::DrawSprite()
{
	Math::Matrix transMat = Math::Matrix::Identity;
	Math::Matrix mat = Math::Matrix::Identity;
	KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
	Math::Color color = {};

	mat = Math::Matrix::Identity;
	mat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["SelectBackTexPos"][0].number_value()),
									      static_cast<float>(m_mpDedicatedObj["SelectBackTexPos"][1].number_value()),
									      static_cast<float>(m_mpDedicatedObj["SelectBackTexPos"][2].number_value()));

	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_selectBackTex, 0, 0, static_cast<int>(m_selectBackTex.GetWidth()), static_cast<int>(m_selectBackTex.GetHeight()));

	mat = Math::Matrix::CreateScale(0.75f) * Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["ModeSelectTexPos"][0].number_value()),
		                                                                     static_cast<float>(m_mpDedicatedObj["ModeSelectTexPos"][1].number_value()),
		                                                                     static_cast<float>(m_mpDedicatedObj["ModeSelectTexPos"][2].number_value()));

	KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_modeSelectTex, 0, 0, static_cast<int>(m_modeSelectTex.GetWidth()), static_cast<int>(m_modeSelectTex.GetHeight()));

	mat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["PictureFrameTexPos"][0].number_value()),
										  static_cast<float>(m_mpDedicatedObj["PictureFrameTexPos"][1].number_value()),
										  static_cast<float>(m_mpDedicatedObj["PictureFrameTexPos"][2].number_value()));

	KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_pictureFrameTex, 0, 0, static_cast<int>(m_pictureFrameTex.GetWidth()), static_cast<int>(m_pictureFrameTex.GetHeight()));

	if (m_bBattleChara)
	{
		mat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["GameTexPos"][0].number_value()),
											  static_cast<float>(m_mpDedicatedObj["GameTexPos"][1].number_value()),
											  static_cast<float>(m_mpDedicatedObj["GameTexPos"][2].number_value()));

		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_gameTex, 0, 0, static_cast<int>(m_gameTex.GetWidth()), static_cast<int>(m_gameTex.GetHeight()));

		mat = Math::Matrix::CreateScale(m_enemyBossTypeOneScale) * Math::Matrix::CreateTranslation(m_enemyBossTypeOnePos);

		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_enemyBossTypeOneTex, 0, 0, static_cast<int>(m_enemyBossTypeOneTex.GetWidth()), static_cast<int>(m_enemyBossTypeOneTex.GetHeight()));

		mat = Math::Matrix::CreateScale(m_enemyBossTypeTwoScale) * Math::Matrix::CreateTranslation(m_enemyBossTypeTwoPos);

		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_enemyBossTypeTwoTotalTex, 0, 0, static_cast<int>(m_enemyBossTypeTwoTotalTex.GetWidth()), static_cast<int>(m_enemyBossTypeTwoTotalTex.GetHeight()));

		mat = Math::Matrix::CreateScale(m_twoEnemyBossTypeOneScale) * Math::Matrix::CreateTranslation(m_twoEnemyBossTypeOnePos);

		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_twoEnemyBossTypeOneTex, 0, 0, static_cast<int>(m_twoEnemyBossTypeOneTex.GetWidth()), static_cast<int>(m_twoEnemyBossTypeOneTex.GetHeight()));

		mat = Math::Matrix::CreateScale(m_helpMkScale) * Math::Matrix::CreateTranslation(m_helpMkPos);

		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_helpMkTex, 0, 0, static_cast<int>(m_helpMkTex.GetWidth()), static_cast<int>(m_helpMkTex.GetHeight()));
	}

	mat = Math::Matrix::CreateScale(m_battleCharaScale) * Math::Matrix::CreateTranslation(m_battleCharaPos);

	KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_battleCharaTex, 0, 0, static_cast<int>(m_battleCharaTex.GetWidth()), static_cast<int>(m_battleCharaTex.GetHeight()));

	if (m_bChallengeChara)
	{
		mat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["ChallengeTexPos"][0].number_value()),
			                                  static_cast<float>(m_mpDedicatedObj["ChallengeTexPos"][1].number_value()),
			                                  static_cast<float>(m_mpDedicatedObj["ChallengeTexPos"][2].number_value()));

		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_challengeTex, 0, 0, static_cast<int>(m_challengeTex.GetWidth()), static_cast<int>(m_challengeTex.GetHeight()));


		mat = Math::Matrix::CreateScale(m_chalengeFiftyScale) * Math::Matrix::CreateTranslation(m_chalengeFiftyPos);

		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_chalengeFiftyTex, 0, 0, static_cast<int>(m_chalengeFiftyTex.GetWidth()), static_cast<int>(m_chalengeFiftyTex.GetHeight()));

		mat = Math::Matrix::CreateScale(m_chalengeHundredScale) * Math::Matrix::CreateTranslation(m_chalengeHundredPos);

		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_chalengeHundredTex, 0, 0, static_cast<int>(m_chalengeHundredTex.GetWidth()), static_cast<int>(m_chalengeHundredTex.GetHeight()));


		/*mat = Math::Matrix::CreateScale(m_helpMkScale) * Math::Matrix::CreateTranslation(m_helpMkPos);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_helpMkTex, 0, 0, 50, 50);*/
	}

	mat = Math::Matrix::CreateScale(m_challengeCharaScale) * Math::Matrix::CreateTranslation(m_challengeCharaPos);

	KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_challengeCharaTex, 0, 0, static_cast<int>(m_challengeCharaTex.GetWidth()), static_cast<int>(m_challengeCharaTex.GetHeight()));

	mat = Math::Matrix::CreateScale(m_tutorialScale) * Math::Matrix::CreateTranslation(m_tutorialPos);

	KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_tutorialTex, 0, 0, static_cast<int>(m_tutorialTex.GetWidth()), static_cast<int>(m_tutorialTex.GetHeight()));

	mat = Math::Matrix::CreateScale(m_trainingScale) * Math::Matrix::CreateTranslation(m_trainingPos);

	KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_trainingTex, 0, 0, static_cast<int>(m_trainingTex.GetWidth()), static_cast<int>(m_trainingTex.GetHeight()));

	mat = Math::Matrix::CreateScale(m_titleScale) * Math::Matrix::CreateTranslation(m_titlePos);
	KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_titleTex, 0, 0, static_cast<int>(m_titleTex.GetWidth()), static_cast<int>(m_titleTex.GetHeight()));

	mat = Math::Matrix::CreateScale(m_exitScale) * Math::Matrix::CreateTranslation(m_exitPos);
	KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_exitTex, 0, 0, static_cast<int>(m_exitTex.GetWidth()), static_cast<int>(m_exitTex.GetHeight()));

	mat = Math::Matrix::Identity;
	KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
	color = { 0,0,0,m_fadeAlpha };
	KdShaderManager::Instance().m_spriteShader.DrawBox(0, 0, 1280, 720, &color);

	if (m_bBattlehelp)
	{
		KdShaderManager::Instance().m_spriteShader.SetMatrix(Math::Matrix::Identity);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_battlehelpTex, 0, 0, static_cast<int>(m_battlehelpTex.GetWidth()), static_cast<int>(m_battlehelpTex.GetHeight()));

		mat = Math::Matrix::CreateScale(m_clauseMkScale) * Math::Matrix::CreateTranslation(m_clauseMkPos);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_clauseMkTex, 0, 0, static_cast<int>(m_clauseMkTex.GetWidth()), static_cast<int>(m_clauseMkTex.GetHeight()));
	}
	/*else if (m_bChalengehelp)
	{
		KdShaderManager::Instance().m_spriteShader.SetMatrix(Math::Matrix::Identity);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_chalengehelpTex, 0, 0, 880, 520);

		mat = Math::Matrix::CreateScale(m_clauseMkScale) * Math::Matrix::CreateTranslation(m_clauseMkPos);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_clauseMkTex, 0, 0, 50, 50);
	}*/

}
// ＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝

// チュートリアルモードのUI関連の処理＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
void TutorialUi::Init(std::weak_ptr<json11::Json> a_wpJsonObj)
{
	m_mpUiSharedObj = (*a_wpJsonObj.lock())["Ui"].object_items();
	m_mpDedicatedObj = m_mpUiSharedObj["TutorialUi"].object_items();

	m_screenAlphaFadeSpeed = static_cast<float>(m_mpUiSharedObj["ScreenAlphaFadeSpeed"].number_value());
	m_pushLClickAlphaFadeSpeed = static_cast<float>(m_mpUiSharedObj["PushLClickAlphaFadeSpeed"].number_value());
	m_mouseRadius = m_mpUiSharedObj["MouseRadius"].int_value();
	m_mouseHalfHeight = m_mpUiSharedObj["MouseHalfHeight"].int_value();
	m_titleBarHeight = m_mpUiSharedObj["TitleBarHeight"].int_value();

	m_lWeaponChangeKeyTex.Load("Asset/Textures/Ui/Game/Q-Key.png");
	m_rWeaponChangeKeyTex.Load("Asset/Textures/Ui/Game/E-Key.png");

	m_lWeaponChangeKeyPushStateTex.Load("Asset/Textures/Ui/Game/PushStateQ-Key.png");
	m_rWeaponChangeKeyPushStateTex.Load("Asset/Textures/Ui/Game/PushStateE-Key.png");

	m_exitTex.Load("Asset/Textures/Ui/OPTION/EXIT.png");
	m_selectTex.Load("Asset/Textures/Ui/OPTION/ModeSelect.png");
	m_operationTex.Load("Asset/Textures/Ui/OPTION/Operation.png");

	m_weaponType1Tex.Load("Asset/Textures/Ui/Game/WeaponType1.png");
	m_weaponType2Tex.Load("Asset/Textures/Ui/Game/WeaponType2.png");
	m_weaponTypeOvreDarkTex.Load("Asset/Textures/Ui/Game/WeaponTypeOvreDark.png");

	m_hopperDataTex.Load("Asset/Textures/Ui/Game/hopperData.png");
	m_weaponDataTex.Load("Asset/Textures/Ui/Game/weaponData.png");
	m_weaponInfoTex.Load("Asset/Textures/Ui/Game/weaponInfo.png");
	m_leftYaiTex.Load("Asset/Textures/Ui/Game/leftYaji.png");
	m_rightYaiTex.Load("Asset/Textures/Ui/Game/rightYaji.png");
	m_howToTex.Load("Asset/Textures/Ui/Game/how-to.png");

	m_backTex.Load("Asset/Textures/Ui/shared/Back.png");

	m_hopperTyuTex.Load("Asset/Textures/Ui/Tutorial/hopperTyu.png");
	m_sonotaTyuTex.Load("Asset/Textures/Ui/Tutorial/sonotaTyu.png");
	m_sukoADMoveTex.Load("Asset/Textures/Ui/Tutorial/sukoADMove.png");
	m_tyubukiTex.Load("Asset/Textures/Ui/Tutorial/tyubuki.png");
	m_tyuKihonTex.Load("Asset/Textures/Ui/Tutorial/tyuKihon.png");

	m_hopperTyuToDoTex.Load("Asset/Textures/Ui/Tutorial/hopperTyuToDo.png");
	m_sukoADMoveToDoTex.Load("Asset/Textures/Ui/Tutorial/sukoADMoveToDo.png");
	m_tyubukiToDoTex.Load("Asset/Textures/Ui/Tutorial/tyubukiToDo.png");
	m_tyuKihonToDoTex.Load("Asset/Textures/Ui/Tutorial/tyuKihonToDo.png");
	m_enemyPosToZeroWayTex.Load("Asset/Textures/Ui/Tutorial/EnemyPosToZeroWay.png");

	m_cntTex[0].Load("Asset/Textures/Ui/Result/p0.png");
	m_cntTex[1].Load("Asset/Textures/Ui/Result/p1.png");
	m_cntTex[2].Load("Asset/Textures/Ui/Result/p2.png");
	m_cntTex[3].Load("Asset/Textures/Ui/Result/p3.png");
	m_cntTex[4].Load("Asset/Textures/Ui/Result/p4.png");
	m_cntTex[5].Load("Asset/Textures/Ui/Result/p5.png");
	m_divisionSignTex.Load("Asset/Textures/Ui/Tutorial/DivisionSign.png");

	m_addFadeAlpha = false;
	m_time = 0;

	m_exitScale = 1.0f;
	m_exitPos = {(float)(m_mpDedicatedObj["ExitButtomPos"][0].number_value()),
			     (float)(m_mpDedicatedObj["ExitButtomPos"][1].number_value()),
				 (float)(m_mpDedicatedObj["ExitButtomPos"][2].number_value()) };

	m_fadeAlpha = 0.0f;
	m_backPos = {(float)(m_mpDedicatedObj["BackButtomPos"][0].number_value()),
				 (float)(m_mpDedicatedObj["BackButtomPos"][1].number_value()),
				 (float)(m_mpDedicatedObj["BackButtomPos"][2].number_value())};

	m_backScale = 0.8f;

	m_tutorialType = kihonTu;
	m_bTutorialView = true;
	m_tutorialSwitchAfterTime = 120;

	m_bTABKey = false;

	m_bWeaponDataPage = false;
	m_bWeaponDataHopperPage = false;
	m_bWeaponDataScoPage = true;
	m_bHowToPage = true;

	m_weaponLeftYaiScale = 1.0f;
	m_weaponLeftYaiPos = { (float)(m_mpDedicatedObj["WeaponLeftYaiButtomPos"][0].number_value()),
				           (float)(m_mpDedicatedObj["WeaponLeftYaiButtomPos"][1].number_value()),
				           (float)(m_mpDedicatedObj["WeaponLeftYaiButtomPos"][2].number_value()) };

	m_weaponRightYaiScale = 1.0f;
	m_weaponRightYaiPos = {(float)(m_mpDedicatedObj["WeaponRightYaiButtomPos"][0].number_value()),
						   (float)(m_mpDedicatedObj["WeaponRightYaiButtomPos"][1].number_value()),
						   (float)(m_mpDedicatedObj["WeaponRightYaiButtomPos"][2].number_value()) };

	m_weaOrHowLeftYaiScale = 1.0f;
	m_weaOrHowLeftYaiPos = {(float)(m_mpDedicatedObj["WeaOrHowLeftYaiButtomPos"][0].number_value()),
						    (float)(m_mpDedicatedObj["WeaOrHowLeftYaiButtomPos"][1].number_value()),
						    (float)(m_mpDedicatedObj["WeaOrHowLeftYaiButtomPos"][2].number_value()) };

	m_weaOrHowRightYaiScale = 1.0f;
	m_weaOrHowRightYaiPos = {(float)(m_mpDedicatedObj["WeaOrHowRightYaiButtomPos"][0].number_value()),
							 (float)(m_mpDedicatedObj["WeaOrHowRightYaiButtomPos"][1].number_value()),
							 (float)(m_mpDedicatedObj["WeaOrHowRightYaiButtomPos"][2].number_value()) };

	m_operationScale = 1.0f;
	m_operationPos = {(float)(m_mpDedicatedObj["OperationButtomPos"][0].number_value()),
					  (float)(m_mpDedicatedObj["OperationButtomPos"][1].number_value()),
					  (float)(m_mpDedicatedObj["OperationButtomPos"][2].number_value()) };
	m_bOperation = false;
	m_tutorialCnt = 0;
}

void TutorialUi::Update()
{
	if (GetAsyncKeyState(VK_TAB) & 0x8000)
	{
		if (!m_bTABKey)
		{
			m_bTABKey = true;
			if (!m_bOption)
			{
				m_bOption = true;
				KdAudioManager::Instance().Play("Asset/Audio/SE/OpenMenu.wav");
				ShowCursor(true); // マウスカーソルを消す
			}
			else if (m_bOption)
			{
				m_bOption = false;
				m_bWeaponDataPage = false;
				m_bWeaponDataHopperPage = false;
				m_bWeaponDataScoPage = true;
				m_bHowToPage = true;
				m_bOperation = false;
				SetCursorPos(640, 360);
				KdAudioManager::Instance().Play("Asset/Audio/SE/OpenMenu.wav");
				ShowCursor(false); // マウスカーソルを消す
			}
		}
	}
	else
	{
		m_bTABKey = false;
	}

	if (m_bOption)
	{
		OptionUpdate();
	}
	else
	{
		if (m_bTutorialView)
		{
			if (m_tutorialSwitchAfterTime > 0)
			{
				--m_tutorialSwitchAfterTime;
			}
			else
			{
				if (KdInputManager::Instance().IsPress("select"))
				{
					m_bTutorialView = false;
				}
			}
		}
		else
		{
			switch (m_tutorialType)
			{
			case kihonTu:
				if (m_tutorialCnt >= 5)
				{
					m_tutorialCnt = 0;
					m_bTutorialView = true;
					m_tutorialType = bukiTu;
					m_tutorialSwitchAfterTime = 120;
				}
				break;
			case bukiTu:
				if (m_tutorialCnt >= 5)
				{
					m_tutorialCnt = 0;
					m_bTutorialView = true;
					m_tutorialType = sukoADMoveTu;
					m_tutorialSwitchAfterTime = 120;
				}
				break;
			case sukoADMoveTu:
				if (m_tutorialCnt >= 5)
				{
					m_tutorialCnt = 0;
					m_bTutorialView = true;
					m_tutorialType = hopperTu;
					m_tutorialSwitchAfterTime = 120;
				}
				break;
			case hopperTu:
				if (m_tutorialCnt >= 5)
				{
					m_tutorialCnt = 0;
					m_bTutorialView = true;
					m_tutorialType = sonotaTu;
					m_tutorialSwitchAfterTime = 120;
				}
				break;
			}
		}
	}
}

void TutorialUi::DrawSprite()
{
	Math::Matrix transMat = Math::Matrix::Identity;
	Math::Matrix mat = Math::Matrix::Identity;
	KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
	Math::Color color = {};
	std::shared_ptr<Player> spPlayer = m_wpPlayer.lock();
	Math::Rectangle rc;

	if (spPlayer)
	{
		transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["WeaponTypeOneTexPos_R"][0].number_value()),
			                                       static_cast<float>(m_mpDedicatedObj["WeaponTypeOneTexPos_R"][1].number_value()),
			                                       static_cast<float>(m_mpDedicatedObj["WeaponTypeOneTexPos_R"][2].number_value()));

		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponType1Tex, 0, 0, m_weaponType1Tex.GetWidth(), m_weaponType1Tex.GetHeight());
		if (spPlayer->GetWeaponType() & Player::WeaponType::grassHopper)
		{
			rc = { 0,0,static_cast<int>(m_weaponTypeOvreDarkTex.GetWidth()),static_cast<int>(m_weaponTypeOvreDarkTex.GetHeight()) };
			color = { 1,1,1,0.6f };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponTypeOvreDarkTex, 0, 0, static_cast<int>(m_weaponTypeOvreDarkTex.GetWidth()), static_cast<int>(m_weaponTypeOvreDarkTex.GetHeight()), &rc, &color);
		}

		transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoTexPos_R"][0].number_value()),
			                                       static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoTexPos_R"][1].number_value()),
			                                       static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoTexPos_R"][2].number_value()));

		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponType2Tex, 0, 0, static_cast<int>(m_weaponType2Tex.GetWidth()), static_cast<int>(m_weaponType2Tex.GetHeight()));
		if (spPlayer->GetWeaponType() & Player::WeaponType::scorpion)
		{
			transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoOvreDarkTexPos_R"][0].number_value()),
				                                       static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoOvreDarkTexPos_R"][1].number_value()),
				                                       static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoOvreDarkTexPos_R"][2].number_value()));

			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			rc = { 0,0,static_cast<int>(m_weaponTypeOvreDarkTex.GetWidth()),static_cast<int>(m_weaponTypeOvreDarkTex.GetHeight()) };
			color = { 1,1,1,0.6f };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponTypeOvreDarkTex, 0, 0, static_cast<int>(m_weaponTypeOvreDarkTex.GetWidth()), static_cast<int>(m_weaponTypeOvreDarkTex.GetHeight()), &rc, &color, Math::Vector2(0, 0.5f));
		}
		else if (spPlayer->GetWeaponType() & Player::WeaponType::grassHopper && spPlayer->GetRGrassHopperPauCnt() != 0)
		{
			transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoOvreDarkTexPos_R"][0].number_value()),
				                                       static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoOvreDarkTexPos_R"][1].number_value()),
				                                       static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoOvreDarkTexPos_R"][2].number_value()));

			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			rc = { 0,0,spPlayer->GetRGrassHopperPauCnt() * 4,static_cast<int>(m_weaponTypeOvreDarkTex.GetHeight()) };
			color = { 1,1,1,0.6f };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponTypeOvreDarkTex, 0, 0, spPlayer->GetRGrassHopperPauCnt() * 4, static_cast<int>(m_weaponTypeOvreDarkTex.GetHeight()), &rc, &color, Math::Vector2(0, 0.5f));
		}

		if (KdInputManager::Instance().GetButtonState("rWeaponChange"))
		{
			transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["WeaponChangeKeyPushStateTexPos_R"][0].number_value()),
				                                       static_cast<float>(m_mpDedicatedObj["WeaponChangeKeyPushStateTexPos_R"][1].number_value()),
				                                       static_cast<float>(m_mpDedicatedObj["WeaponChangeKeyPushStateTexPos_R"][2].number_value()));

			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_rWeaponChangeKeyPushStateTex, 0, 0, static_cast<int>(m_rWeaponChangeKeyPushStateTex.GetWidth()), static_cast<int>(m_rWeaponChangeKeyPushStateTex.GetHeight()));
		}
		else
		{
			transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["WeaponChangeKeyPushStateTexPos_R"][0].number_value()),
				                                       static_cast<float>(m_mpDedicatedObj["WeaponChangeKeyPushStateTexPos_R"][1].number_value()),
				                                       static_cast<float>(m_mpDedicatedObj["WeaponChangeKeyPushStateTexPos_R"][2].number_value()));

			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_rWeaponChangeKeyTex, 0, 0, static_cast<int>(m_rWeaponChangeKeyTex.GetWidth()), static_cast<int>(m_rWeaponChangeKeyTex.GetHeight()));
		}

		transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["WeaponTypeOneTexPos_L"][0].number_value()),
			                                       static_cast<float>(m_mpDedicatedObj["WeaponTypeOneTexPos_L"][1].number_value()),
			                                       static_cast<float>(m_mpDedicatedObj["WeaponTypeOneTexPos_L"][2].number_value()));

		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponType1Tex, 0, 0, static_cast<int>(m_weaponType1Tex.GetWidth()), static_cast<int>(m_weaponType1Tex.GetHeight()));
		if (spPlayer->GetWeaponType() & Player::WeaponType::lGrassHopper)
		{
			rc = { 0,0,static_cast<int>(m_weaponTypeOvreDarkTex.GetWidth()),static_cast<int>(m_weaponTypeOvreDarkTex.GetHeight()) };
			color = { 1,1,1,0.6f };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponTypeOvreDarkTex, 0, 0, static_cast<int>(m_weaponTypeOvreDarkTex.GetWidth()), static_cast<int>(m_weaponTypeOvreDarkTex.GetHeight()), &rc, &color);
		}

		transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoTexPos_L"][0].number_value()),
			                                       static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoTexPos_L"][1].number_value()),
			                                       static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoTexPos_L"][2].number_value()));

		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponType2Tex, 0, 0, static_cast<int>(m_weaponType2Tex.GetWidth()), static_cast<int>(m_weaponType2Tex.GetHeight()));
		if (spPlayer->GetWeaponType() & Player::WeaponType::lScorpion)
		{
			transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoOvreDarkTexPos_L"][0].number_value()),
				                                       static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoOvreDarkTexPos_L"][1].number_value()),
				                                       static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoOvreDarkTexPos_L"][2].number_value()));

			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			rc = { 0,0,static_cast<int>(m_weaponTypeOvreDarkTex.GetWidth()), static_cast<int>(m_weaponTypeOvreDarkTex.GetHeight()) };
			color = { 1,1,1,0.6f };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponTypeOvreDarkTex, 0, 0, static_cast<int>(m_weaponTypeOvreDarkTex.GetWidth()), static_cast<int>(m_weaponTypeOvreDarkTex.GetHeight()), &rc, &color, Math::Vector2(0, 0.5f));
		}
		else if (spPlayer->GetWeaponType() & Player::WeaponType::lGrassHopper && spPlayer->GetLGrassHopperPauCnt() != 0)
		{
			transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoOvreDarkTexPos_L"][0].number_value()),
				                                       static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoOvreDarkTexPos_L"][1].number_value()),
				                                       static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoOvreDarkTexPos_L"][2].number_value()));

			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			rc = { 0,0,spPlayer->GetLGrassHopperPauCnt() * 4,static_cast<int>(m_weaponTypeOvreDarkTex.GetHeight()) };
			color = { 1,1,1,0.6f };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponTypeOvreDarkTex, 0, 0, spPlayer->GetLGrassHopperPauCnt() * 4, static_cast<int>(m_weaponTypeOvreDarkTex.GetHeight()), &rc, &color, Math::Vector2(0, 0.5f));
		}

		if (KdInputManager::Instance().GetButtonState("lWeaponChange"))
		{
			transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["WeaponChangeKeyPushStateTexPos_L"][0].number_value()),
				                                       static_cast<float>(m_mpDedicatedObj["WeaponChangeKeyPushStateTexPos_L"][1].number_value()),
				                                       static_cast<float>(m_mpDedicatedObj["WeaponChangeKeyPushStateTexPos_L"][2].number_value()));

			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_lWeaponChangeKeyPushStateTex, 0, 0, static_cast<int>(m_lWeaponChangeKeyPushStateTex.GetWidth()), static_cast<int>(m_lWeaponChangeKeyPushStateTex.GetHeight()));
		}
		else
		{
			transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["WeaponChangeKeyPushStateTexPos_L"][0].number_value()),
				                                       static_cast<float>(m_mpDedicatedObj["WeaponChangeKeyPushStateTexPos_L"][1].number_value()),
				                                       static_cast<float>(m_mpDedicatedObj["WeaponChangeKeyPushStateTexPos_L"][2].number_value()));

			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_lWeaponChangeKeyTex, 0, 0, static_cast<int>(m_lWeaponChangeKeyTex.GetWidth()), static_cast<int>(m_lWeaponChangeKeyTex.GetHeight()));
		}
	}

	if (m_tutorialType != sonotaTu)
	{
		transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["ToDoExecutedCntPos"][0].number_value()),
												   static_cast<float>(m_mpDedicatedObj["ToDoExecutedCntPos"][1].number_value()),
												   static_cast<float>(m_mpDedicatedObj["ToDoExecutedCntPos"][2].number_value()));
		
		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_cntTex[m_tutorialCnt], 0, 0, static_cast<int>(m_cntTex[m_tutorialCnt].GetWidth()), static_cast<int>(m_cntTex[m_tutorialCnt].GetHeight()));

		transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["MaxToDoExecutedCntPos"][0].number_value()),
										           static_cast<float>(m_mpDedicatedObj["MaxToDoExecutedCntPos"][1].number_value()),
										           static_cast<float>(m_mpDedicatedObj["MaxToDoExecutedCntPos"][2].number_value()));

		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_cntTex[5], 0, 0, static_cast<int>(m_cntTex[5].GetWidth()), static_cast<int>(m_cntTex[5].GetHeight()));

		transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["DivisionSignPos"][0].number_value()),
			                                       static_cast<float>(m_mpDedicatedObj["DivisionSignPos"][1].number_value()),
			                                       static_cast<float>(m_mpDedicatedObj["DivisionSignPos"][2].number_value()));

		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_divisionSignTex, 0, 0, static_cast<int>(m_divisionSignTex.GetWidth()), static_cast<int>(m_divisionSignTex.GetHeight()));
	}

	if (m_bOption)
	{
		transMat = Math::Matrix::Identity;
		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		color = { 0,0,0,0.6f };
		KdShaderManager::Instance().m_spriteShader.DrawBox(0, 0, 1280, 720, &color);

		if (!m_bOperation)
		{
			mat = Math::Matrix::CreateScale(m_operationScale) * Math::Matrix::CreateTranslation(m_operationPos);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_operationTex, 0, 0, static_cast<int>(m_operationTex.GetWidth()), static_cast<int>(m_operationTex.GetHeight()));

			mat = Math::Matrix::CreateScale(m_selectScale) * Math::Matrix::CreateTranslation(m_selectPos);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_selectTex, 0, 0, static_cast<int>(m_selectTex.GetWidth()), static_cast<int>(m_selectTex.GetHeight()));

			mat = Math::Matrix::CreateScale(m_exitScale) * Math::Matrix::CreateTranslation(m_exitPos);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_exitTex, 0, 0, static_cast<int>(m_exitTex.GetWidth()), static_cast<int>(m_exitTex.GetHeight()));
		}
		else if (m_bOperation)
		{
			if (m_bHowToPage)
			{
				transMat = Math::Matrix::Identity;
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_howToTex, 0, 0, static_cast<int>(m_howToTex.GetWidth()), static_cast<int>(m_howToTex.GetHeight()));
			}

			if (m_bWeaponDataPage)
			{
				if (m_bWeaponDataHopperPage)
				{
					transMat = Math::Matrix::Identity;
					KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_hopperDataTex, 0, 0, static_cast<int>(m_hopperDataTex.GetWidth()), static_cast<int>(m_hopperDataTex.GetHeight()));
				}
				else if (m_bWeaponDataScoPage)
				{
					transMat = Math::Matrix::Identity;
					KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponInfoTex, 0, 0, static_cast<int>(m_weaponInfoTex.GetWidth()), static_cast<int>(m_weaponInfoTex.GetHeight()));
				}

				mat = Math::Matrix::CreateScale(m_weaponRightYaiScale) * Math::Matrix::CreateTranslation(m_weaponRightYaiPos);
				KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_rightYaiTex, 0, 0, static_cast<int>(m_rightYaiTex.GetWidth()), static_cast<int>(m_rightYaiTex.GetHeight()));

				mat = Math::Matrix::CreateScale(m_weaponLeftYaiScale) * Math::Matrix::CreateTranslation(m_weaponLeftYaiPos);
				KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_leftYaiTex, 0, 0, static_cast<int>(m_leftYaiTex.GetWidth()), static_cast<int>(m_leftYaiTex.GetHeight()));
			}

			mat = Math::Matrix::CreateScale(m_weaOrHowRightYaiScale) * Math::Matrix::CreateTranslation(m_weaOrHowRightYaiPos);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_rightYaiTex, 0, 0, static_cast<int>(m_rightYaiTex.GetWidth()), static_cast<int>(m_rightYaiTex.GetHeight()));

			mat = Math::Matrix::CreateScale(m_weaOrHowLeftYaiScale) * Math::Matrix::CreateTranslation(m_weaOrHowLeftYaiPos);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_leftYaiTex, 0, 0, static_cast<int>(m_leftYaiTex.GetWidth()), static_cast<int>(m_leftYaiTex.GetHeight()));

		}

		mat = Math::Matrix::CreateScale(m_backScale) * Math::Matrix::CreateTranslation(m_backPos);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_backTex, 0, 0, static_cast<int>(m_backTex.GetWidth()), static_cast<int>(m_backTex.GetHeight()));
	}

	if (m_bTutorialView)
	{
		transMat = Math::Matrix::Identity;
		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		color = { 0,0,0,0.4f };
		KdShaderManager::Instance().m_spriteShader.DrawBox(0, 0, 1280, 720, &color);

		switch (m_tutorialType)
		{
		case kihonTu:
			mat = Math::Matrix::CreateScale(m_backScale) * Math::Matrix::CreateTranslation(0, 0, 0);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_tyuKihonTex, 0, 0, static_cast<int>(m_tyuKihonTex.GetWidth()), static_cast<int>(m_tyuKihonTex.GetHeight()));
			break;
		case bukiTu:
			mat = Math::Matrix::CreateScale(m_backScale) * Math::Matrix::CreateTranslation(0, 0, 0);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_tyubukiTex, 0, 0, static_cast<int>(m_tyubukiTex.GetWidth()), static_cast<int>(m_tyubukiTex.GetHeight()));
			break;
		case sukoADMoveTu:
			mat = Math::Matrix::CreateScale(m_backScale) * Math::Matrix::CreateTranslation(0, 0, 0);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_sukoADMoveTex, 0, 0, static_cast<int>(m_sukoADMoveTex.GetWidth()), static_cast<int>(m_sukoADMoveTex.GetHeight()));
			break;
		case hopperTu:
			mat = Math::Matrix::CreateScale(m_backScale) * Math::Matrix::CreateTranslation(0, 0, 0);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_hopperTyuTex, 0, 0, static_cast<int>(m_hopperTyuTex.GetWidth()), static_cast<int>(m_hopperTyuTex.GetHeight()));
			break;
		case sonotaTu:
			mat = Math::Matrix::CreateScale(m_backScale) * Math::Matrix::CreateTranslation(0, 0, 0);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_sonotaTyuTex, 0, 0, static_cast<int>(m_sonotaTyuTex.GetWidth()), static_cast<int>(m_sonotaTyuTex.GetHeight()));
			break;
		}
	}
	else
	{
		transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["EnemyPosToZeroWayTexPos"][0].number_value()),
										           static_cast<float>(m_mpDedicatedObj["EnemyPosToZeroWayTexPos"][1].number_value()),
										           static_cast<float>(m_mpDedicatedObj["EnemyPosToZeroWayTexPos"][2].number_value()));

		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_enemyPosToZeroWayTex, 0, 0, static_cast<int>(m_enemyPosToZeroWayTex.GetWidth()), static_cast<int>(m_enemyPosToZeroWayTex.GetHeight()));

		switch (m_tutorialType)
		{
		case kihonTu:
			transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["TutorialToDoTexPos"][0].number_value()),
				static_cast<float>(m_mpDedicatedObj["TutorialToDoTexPos"][1].number_value()),
				static_cast<float>(m_mpDedicatedObj["TutorialToDoTexPos"][2].number_value()));

			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_tyuKihonToDoTex, 0, 0, static_cast<int>(m_tyuKihonToDoTex.GetWidth()), static_cast<int>(m_tyuKihonToDoTex.GetHeight()));
			break;
		case bukiTu:
			transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["TutorialToDoTexPos"][0].number_value()),
				static_cast<float>(m_mpDedicatedObj["TutorialToDoTexPos"][1].number_value()),
				static_cast<float>(m_mpDedicatedObj["TutorialToDoTexPos"][2].number_value()));

			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_tyubukiToDoTex, 0, 0, static_cast<int>(m_tyubukiToDoTex.GetWidth()), static_cast<int>(m_tyubukiToDoTex.GetHeight()));
			break;
		case sukoADMoveTu:
			transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["TutorialToDoTexPos"][0].number_value()),
				static_cast<float>(m_mpDedicatedObj["TutorialToDoTexPos"][1].number_value()),
				static_cast<float>(m_mpDedicatedObj["TutorialToDoTexPos"][2].number_value()));

			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_sukoADMoveToDoTex, 0, 0, static_cast<int>(m_sukoADMoveToDoTex.GetWidth()), static_cast<int>(m_sukoADMoveToDoTex.GetHeight()));
			break;
		case hopperTu:
			transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["TutorialToDoTexPos"][0].number_value()),
				static_cast<float>(m_mpDedicatedObj["TutorialToDoTexPos"][1].number_value()),
				static_cast<float>(m_mpDedicatedObj["TutorialToDoTexPos"][2].number_value()));

			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_hopperTyuToDoTex, 0, 0, static_cast<int>(m_hopperTyuToDoTex.GetWidth()), static_cast<int>(m_hopperTyuToDoTex.GetHeight()));
			break;
		}
	}

	transMat = Math::Matrix::Identity;
	KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
	color = { 0,0,0,m_fadeAlpha };
	KdShaderManager::Instance().m_spriteShader.DrawBox(0, 0, 1280, 720, &color);
}

void TutorialUi::AddTutorialCnt()
{
	if (m_tutorialCnt < 5)
	{
		m_tutorialCnt++;
	}
}

void TutorialUi::OptionUpdate()
{
	if (!m_bOperation)
	{
		if (ButtomProcessing({ m_exitPos.x,m_exitPos.y }, m_exitTex, m_exitScale))
		{
			if (!m_addFadeAlpha)
			{
				m_bExit = true;
				m_addFadeAlpha = true;
				m_bFirstExit = true;
				KdAudioManager::Instance().Play("Asset/Audio/SE/PushButton.wav");
			}
		}

		if (ButtomProcessing({ m_selectPos.x,m_selectPos.y }, m_selectTex, m_selectScale))
		{
			m_bSelect = true;
			m_addFadeAlpha = true;
			KdAudioManager::Instance().Play("Asset/Audio/SE/PushButton.wav");
		}

		if (ButtomProcessing({ m_operationPos.x,m_operationPos.y }, m_operationTex, m_operationScale))
		{
			m_bOperation = true;
			KdAudioManager::Instance().Play("Asset/Audio/SE/PushButton.wav");
		}

		if (ButtomProcessing({ m_backPos.x,m_backPos.y }, m_backTex, m_backScale,0.8f))
		{
			m_bOption = false;
			ShowCursor(false); // マウスカーソルを消す
			SetCursorPos(640, 360);
			KdAudioManager::Instance().Play("Asset/Audio/SE/backPush.wav");
			KdEffekseerManager::GetInstance().OnResumeEfkUpdate();
		}

		if (m_addFadeAlpha)
		{
			m_fadeAlpha += (1.0f / 90);
			if (m_fadeAlpha >= 1.0f)
			{
				m_fadeAlpha = 1.0f;

				if (m_bExit)
				{
					if (m_bFirstExit)
					{
						SceneManager::Instance().SetBWinEnd();
						m_bFirstExit = false;
					}
					else
					{
						m_bExit = false;
						m_addFadeAlpha = false;
						m_bFirstExit = true;
						m_fadeAlpha = 0.0f;
					}
				}

				if (m_bSelect)
				{
					KdEffekseerManager::GetInstance().Reset();
					SceneManager::Instance().SetNextScene
					(
						SceneManager::SceneType::select
					);
				}
			}
		}
	}
	else
	{
		if (ButtomProcessing({ m_backPos.x,m_backPos.y }, m_backTex, m_backScale,0.8f))
		{
			m_bWeaponDataPage = false;
			m_bWeaponDataHopperPage = false;
			m_bWeaponDataScoPage = true;
			m_bHowToPage = true;
			m_bOperation = false;
			KdAudioManager::Instance().Play("Asset/Audio/SE/backPush.wav");
		}

		if (ButtomProcessing({ m_weaOrHowLeftYaiPos.x,m_weaOrHowLeftYaiPos.y }, m_leftYaiTex, m_weaOrHowLeftYaiScale))
		{
			if (m_bWeaponDataPage)
			{
				m_bHowToPage = true;
				m_bWeaponDataPage = false;
			}
			else
			{
				m_bHowToPage = false;
				m_bWeaponDataPage = true;
			}
			KdAudioManager::Instance().Play("Asset/Audio/SE/PushArrow.wav");
		}

		if (ButtomProcessing({ m_weaOrHowRightYaiPos.x,m_weaOrHowRightYaiPos.y }, m_rightYaiTex, m_weaOrHowRightYaiScale))
		{
			if (m_bWeaponDataPage)
			{
				m_bHowToPage = true;
				m_bWeaponDataPage = false;
			}
			else
			{
				m_bHowToPage = false;
				m_bWeaponDataPage = true;
			}
			KdAudioManager::Instance().Play("Asset/Audio/SE/PushArrow.wav");
		}

		if (m_bWeaponDataPage)
		{
			if (ButtomProcessing({ m_weaponLeftYaiPos.x,m_weaponLeftYaiPos.y }, m_leftYaiTex, m_weaponLeftYaiScale))
			{
				if (m_bWeaponDataHopperPage)
				{
					m_bWeaponDataScoPage = true;
					m_bWeaponDataHopperPage = false;
				}
				else
				{
					m_bWeaponDataScoPage = false;
					m_bWeaponDataHopperPage = true;
				}
				KdAudioManager::Instance().Play("Asset/Audio/SE/PushArrow.wav");
			}

			if (ButtomProcessing({ m_weaponRightYaiPos.x,m_weaponRightYaiPos.y }, m_rightYaiTex, m_weaponRightYaiScale))
			{
				if (m_bWeaponDataHopperPage)
				{
					m_bWeaponDataScoPage = true;
					m_bWeaponDataHopperPage = false;
				}
				else
				{
					m_bWeaponDataScoPage = false;
					m_bWeaponDataHopperPage = true;
				}
				KdAudioManager::Instance().Play("Asset/Audio/SE/PushArrow.wav");
			}
		}
	}
}
// ＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝

// トレーニングモードのUI関連の処理＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
void TrainingUi::Init(std::weak_ptr<json11::Json> a_wpJsonObj)
{
	m_mpUiSharedObj = (*a_wpJsonObj.lock())["Ui"].object_items();
	m_mpDedicatedObj = m_mpUiSharedObj["TrainingUi"].object_items();

	m_screenAlphaFadeSpeed = static_cast<float>(m_mpUiSharedObj["ScreenAlphaFadeSpeed"].number_value());
	m_pushLClickAlphaFadeSpeed = static_cast<float>(m_mpUiSharedObj["PushLClickAlphaFadeSpeed"].number_value());
	m_mouseRadius = m_mpUiSharedObj["MouseRadius"].int_value();
	m_mouseHalfHeight = m_mpUiSharedObj["MouseHalfHeight"].int_value();

	m_lWeaponChangeKeyTex.Load("Asset/Textures/Ui/Game/Q-Key.png");
	m_rWeaponChangeKeyTex.Load("Asset/Textures/Ui/Game/E-Key.png");

	m_lWeaponChangeKeyPushStateTex.Load("Asset/Textures/Ui/Game/PushStateQ-Key.png");
	m_rWeaponChangeKeyPushStateTex.Load("Asset/Textures/Ui/Game/PushStateE-Key.png");

	m_titleBarHeight = m_mpUiSharedObj["TitleBarHeight"].int_value();
	m_exitTex.Load("Asset/Textures/Ui/OPTION/EXIT.png");
	m_selectTex.Load("Asset/Textures/Ui/OPTION/ModeSelect.png");
	m_operationTex.Load("Asset/Textures/Ui/OPTION/Operation.png");

	m_weaponType1Tex.Load("Asset/Textures/Ui/Game/WeaponType1.png");
	m_weaponType2Tex.Load("Asset/Textures/Ui/Game/WeaponType2.png");
	m_weaponTypeOvreDarkTex.Load("Asset/Textures/Ui/Game/WeaponTypeOvreDark.png");

	m_torionTex.Load("Asset/Textures/Ui/Game/Torion.png");
	m_torionBarTex.Load("Asset/Textures/Ui/Game/TorionBar.png");

	m_enduranceBarTex.Load("Asset/Textures/Ui/Game/enduranceBar.png");
	m_enduranceTex.Load("Asset/Textures/Ui/Game/endurance.png");

	m_hopperDataTex.Load("Asset/Textures/Ui/Game/hopperData.png");
	m_weaponDataTex.Load("Asset/Textures/Ui/Game/weaponData.png");
	m_weaponInfoTex.Load("Asset/Textures/Ui/Game/weaponInfo.png");
	m_leftYaiTex.Load("Asset/Textures/Ui/Game/leftYaji.png");
	m_rightYaiTex.Load("Asset/Textures/Ui/Game/rightYaji.png");
	m_howToTex.Load("Asset/Textures/Ui/Game/how-to.png");

	m_backTex.Load("Asset/Textures/Ui/shared/Back.png");

	m_countOneTex.Load("Asset/Textures/Ui/Game/one.png");
	m_countTwoTex.Load("Asset/Textures/Ui/Game/two.png");
	m_countThreeTex.Load("Asset/Textures/Ui/Game/three.png");
	m_countGoTex.Load("Asset/Textures/Ui/Game/GO.png");

	m_timeTex[0].Load("Asset/Textures/Ui/shared/Zero.png");
	m_timeTex[1].Load("Asset/Textures/Ui/shared/One.png");
	m_timeTex[2].Load("Asset/Textures/Ui/shared/Two.png");
	m_timeTex[3].Load("Asset/Textures/Ui/shared/Three.png");
	m_timeTex[4].Load("Asset/Textures/Ui/shared/Four.png");
	m_timeTex[5].Load("Asset/Textures/Ui/shared/Five.png");
	m_timeTex[6].Load("Asset/Textures/Ui/shared/Six.png");
	m_timeTex[7].Load("Asset/Textures/Ui/shared/Seven.png");
	m_timeTex[8].Load("Asset/Textures/Ui/shared/Eight.png");
	m_timeTex[9].Load("Asset/Textures/Ui/shared/Nine.png");
	m_conmaTex.Load("Asset/Textures/Ui/shared/Conma.png");

	m_PointTex[0].Load("Asset/Textures/Ui/Result/p0.png");
	m_PointTex[1].Load("Asset/Textures/Ui/Result/p1.png");
	m_PointTex[2].Load("Asset/Textures/Ui/Result/p2.png");

	m_EnemyDirectionArrowLTex.Load("Asset/Textures/Ui/Game/enemyDirectionArrowL.png");
	m_EnemyDirectionArrowRTex.Load("Asset/Textures/Ui/Game/enemyDirectionArrowR.png");
	m_EnemyDirectionArrowUTex.Load("Asset/Textures/Ui/Game/enemyDirectionArrowU.png");
	m_EnemyDirectionArrowBTex.Load("Asset/Textures/Ui/Game/enemyDirectionArrowB.png");
	m_waveTex.Load("Asset/Textures/Ui/Game/wave.png");

	m_addFadeAlpha = false;
	m_time = 0;

	m_exitScale = 1.0f;
	m_selectScale = 1.0f;

	m_selectPos = {(float)m_mpDedicatedObj["ModeSelectButtomPos"][0].number_value(),
		           (float)m_mpDedicatedObj["ModeSelectButtomPos"][1].number_value(),
		           (float)m_mpDedicatedObj["ModeSelectButtomPos"][2].number_value()};

	m_exitPos = { (float)m_mpDedicatedObj["ExitButtomPos"][0].number_value(),
				  (float)m_mpDedicatedObj["ExitButtomPos"][1].number_value(),
				  (float)m_mpDedicatedObj["ExitButtomPos"][2].number_value() };

	m_fadeAlpha = 0.0f;

	m_backPos = { (float)m_mpDedicatedObj["BackButtomPos"][0].number_value(),
				  (float)m_mpDedicatedObj["BackButtomPos"][1].number_value(),
				  (float)m_mpDedicatedObj["BackButtomPos"][2].number_value() };
	m_backScale = 0.8f;

	m_countOneScale = 0.1f;
	m_countOneAlpha = 0.0f;

	m_countTwoScale = 0.1f;
	m_countTwoAlpha = 0.0f;

	m_countThreeScale = 0.1f;
	m_countThreeAlpha = 0.0f;

	m_countGoScale = 0.1f;
	m_countGoAlpha = 0.0f;

	m_gameTimeCntDeray = 0;

	m_waveScale = 0.1f;
	m_waveAlpha = 0.0f;
	m_waveTimeCnt = 0;
	m_waveCnt = 0;

	m_bTABKey = false;

	m_bWeaponDataPage = false;
	m_bWeaponDataHopperPage = false;
	m_bWeaponDataScoPage = true;
	m_bHowToPage = true;

	m_weaponLeftYaiScale = 1.0f;
	m_weaponLeftYaiPos = {(float)m_mpDedicatedObj["WeaponLeftYaiButtomPos"][0].number_value(),
				          (float)m_mpDedicatedObj["WeaponLeftYaiButtomPos"][1].number_value(),
				          (float)m_mpDedicatedObj["WeaponLeftYaiButtomPos"][2].number_value()};

	m_weaponRightYaiScale = 1.0f;
	m_weaponRightYaiPos = {(float)m_mpDedicatedObj["WeaponRightYaiButtomPos"][0].number_value(),
						   (float)m_mpDedicatedObj["WeaponRightYaiButtomPos"][1].number_value(),
						   (float)m_mpDedicatedObj["WeaponRightYaiButtomPos"][2].number_value() };

	m_weaOrHowLeftYaiScale = 1.0f;
	m_weaOrHowLeftYaiPos = {(float)m_mpDedicatedObj["WeaOrHowLeftYaiButtomPos"][0].number_value(),
						    (float)m_mpDedicatedObj["WeaOrHowLeftYaiButtomPos"][1].number_value(),
						    (float)m_mpDedicatedObj["WeaOrHowLeftYaiButtomPos"][2].number_value() };

	m_weaOrHowRightYaiScale = 1.0f;
	m_weaOrHowRightYaiPos = {(float)m_mpDedicatedObj["WeaOrHowRightYaiButtomPos"][0].number_value(),
							 (float)m_mpDedicatedObj["WeaOrHowRightYaiButtomPos"][1].number_value(),
							 (float)m_mpDedicatedObj["WeaOrHowRightYaiButtomPos"][2].number_value() };

	m_operationScale = 1.0f;
	m_operationPos = {(float)m_mpDedicatedObj["OperationButtomPos"][0].number_value(),
					  (float)m_mpDedicatedObj["OperationButtomPos"][1].number_value(),
					  (float)m_mpDedicatedObj["OperationButtomPos"][2].number_value() };
	m_bOperation = false;

	m_scaleFadeSpeed = static_cast<float>(m_mpDedicatedObj["ScaleFadeSpeed"].number_value());

	m_charFadeSpeed = static_cast<float>(m_mpDedicatedObj["CharFadeSpeed"].number_value());
}

void TrainingUi::Update()
{
	if (m_time <= m_mpDedicatedObj["MaxTime"].int_value())
	{
		m_time++;
	}

	if (m_time >= 0 && m_time < m_mpDedicatedObj["CountFadeTime"][0].int_value())
	{
		m_countThreeScale += m_scaleFadeSpeed;
		if (m_countThreeScale > 1.0f)
		{
			m_countThreeScale = 1.0f;
		}

		m_countThreeAlpha += m_scaleFadeSpeed;
		if (m_countThreeAlpha > 1.0f)
		{
			m_countThreeAlpha = 1.0f;
		}
	}
	else if (m_time >= m_mpDedicatedObj["CountFadeTime"][0].int_value() && m_time < m_mpDedicatedObj["CountFadeTime"][1].int_value())
	{

		m_countThreeAlpha -= m_charFadeSpeed;
		if (m_countThreeAlpha < 0.0f)
		{
			m_countThreeAlpha = 0.0f;
		}
	}
	else if (m_time >= m_mpDedicatedObj["CountFadeTime"][1].int_value() && m_time < m_mpDedicatedObj["CountFadeTime"][2].int_value())
	{
		m_countTwoScale += m_scaleFadeSpeed;
		if (m_countTwoScale > 1.0f)
		{
			m_countTwoScale = 1.0f;
		}

		m_countTwoAlpha += m_charFadeSpeed;
		if (m_countTwoAlpha > 1.0f)
		{
			m_countTwoAlpha = 1.0f;
		}
	}
	else if (m_time >= m_mpDedicatedObj["CountFadeTime"][2].int_value() && m_time < m_mpDedicatedObj["CountFadeTime"][3].int_value())
	{
		m_countTwoAlpha -= m_charFadeSpeed;
		if (m_countThreeAlpha < 0.0f)
		{
			m_countThreeAlpha = 0.0f;
		}
	}
	else if (m_time >= m_mpDedicatedObj["CountFadeTime"][3].int_value() && m_time < m_mpDedicatedObj["CountFadeTime"][4].int_value())
	{
		m_countOneScale += m_scaleFadeSpeed;
		if (m_countOneScale > 1.0f)
		{
			m_countOneScale = 1.0f;
		}

		m_countOneAlpha += m_charFadeSpeed;
		if (m_countOneAlpha > 1.0f)
		{
			m_countOneAlpha = 1.0f;
		}
	}
	else if (m_time >= m_mpDedicatedObj["CountFadeTime"][4].int_value() && m_time < m_mpDedicatedObj["CountFadeTime"][5].int_value())
	{
		m_countOneAlpha -= m_charFadeSpeed;
		if (m_countOneAlpha < 0.0f)
		{
			m_countOneAlpha = 0.0f;
		}
	}
	else if (m_time >= m_mpDedicatedObj["CountFadeTime"][5].int_value() && m_time < m_mpDedicatedObj["CountFadeTime"][6].int_value())
	{
		m_countGoScale += m_scaleFadeSpeed;
		if (m_countGoScale > 1.0f)
		{
			m_countGoScale = 1.0f;
		}

		m_countGoAlpha += m_charFadeSpeed;
		if (m_countGoAlpha > 1.0f)
		{
			m_countGoAlpha = 1.0f;
		}
	}
	else if (m_time >= m_mpDedicatedObj["CountFadeTime"][6].int_value() && m_time < m_mpDedicatedObj["CountFadeTime"][7].int_value())
	{
		m_countGoAlpha -= m_charFadeSpeed;
		if (m_countGoAlpha < 0.0f)
		{
			m_countGoAlpha = 0.0f;
			SetCursorPos(640, 360);
		}
	}

	if (GetAsyncKeyState(VK_TAB) & 0x8000)
	{
		if (!m_bTABKey)
		{
			m_bTABKey = true;
			if (!m_bOption)
			{
				m_bOption = true;
				KdAudioManager::Instance().Play("Asset/Audio/SE/OpenMenu.wav");
				ShowCursor(true); // マウスカーソルを消す
				KdEffekseerManager::GetInstance().OnPauseEfkUpdate();
			}
			else if (m_bOption)
			{
				m_bOption = false;
				m_bWeaponDataPage = false;
				m_bWeaponDataHopperPage = false;
				m_bWeaponDataScoPage = true;
				m_bHowToPage = true;
				m_bOperation = false;
				SetCursorPos(640, 360);
				KdAudioManager::Instance().Play("Asset/Audio/SE/OpenMenu.wav");
				ShowCursor(false); // マウスカーソルを消す
				KdEffekseerManager::GetInstance().OnResumeEfkUpdate();
			}
		}
	}
	else
	{
		m_bTABKey = false;
	}

	if (m_bOption)
	{
		OptionUpdate();
	}
}

void TrainingUi::PostUpdate()
{
	Math::Vector3 pos = Math::Vector3(m_wpEnemy.lock()->GetPos().x, m_wpEnemy.lock()->GetPos().y + m_wpEnemy.lock()->GetRockOnPos().y + m_wpEnemy.lock()->GetAddHpPosY(), m_wpEnemy.lock()->GetPos().z);

	{
		POINT dev;
		KdDirect3D::Instance().WorldToClient(pos, dev, m_wpCamera.lock()->WorkCamera()->GetCameraMatrix(), m_wpCamera.lock()->WorkCamera()->GetProjMatrix());
		dev.x -= (long)640.0f;
		dev.y = (long)(dev.y * -1 + 360.0f);
		dev.x -= 50;
		m_enemyScPos = Math::Vector2((float)dev.x, (float)dev.y);
	}
}

void TrainingUi::DrawSprite()
{
	Math::Matrix transMat = Math::Matrix::Identity;
	Math::Matrix mat = Math::Matrix::Identity;
	KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
	Math::Color color = {};
	Math::Rectangle rc = {};
	std::shared_ptr<Player> spPlayer = m_wpPlayer.lock();

	transMat = Math::Matrix::Identity;

	if (m_time >= 0 && m_time < m_mpDedicatedObj["CountFadeTime"][1].int_value())
	{
		mat = Math::Matrix::CreateScale(m_countThreeScale) * transMat;
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		rc = { 0,0,static_cast<int>(m_countThreeTex.GetWidth()), static_cast<int>(m_countThreeTex.GetHeight()) };
		color = { 1,1,1,m_countThreeAlpha };
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_countThreeTex, 0, 0, static_cast<int>(m_countThreeTex.GetWidth()), static_cast<int>(m_countThreeTex.GetHeight()), &rc, &color);
	}
	else if (m_time >= m_mpDedicatedObj["CountFadeTime"][1].int_value() && m_time < m_mpDedicatedObj["CountFadeTime"][3].int_value())
	{
		mat = Math::Matrix::CreateScale(m_countTwoScale) * transMat;
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		rc = { 0,0,static_cast<int>(m_countTwoTex.GetWidth()), static_cast<int>(m_countTwoTex.GetHeight()) };
		color = { 1,1,1,m_countTwoAlpha };
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_countTwoTex, 0, 0, static_cast<int>(m_countTwoTex.GetWidth()), static_cast<int>(m_countTwoTex.GetHeight()), &rc, &color);
	}
	else if (m_time >= m_mpDedicatedObj["CountFadeTime"][3].int_value() && m_time < m_mpDedicatedObj["CountFadeTime"][5].int_value())
	{
		mat = Math::Matrix::CreateScale(m_countOneScale) * transMat;
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		rc = { 0,0,static_cast<int>(m_countOneTex.GetWidth()), static_cast<int>(m_countOneTex.GetHeight()) };
		color = { 1,1,1,m_countOneAlpha };
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_countOneTex, 0, 0, static_cast<int>(m_countOneTex.GetWidth()), static_cast<int>(m_countOneTex.GetHeight()), &rc, &color);
	}
	else if (m_time >= m_mpDedicatedObj["CountFadeTime"][5].int_value() && m_time < m_mpDedicatedObj["CountFadeTime"][7].int_value())
	{
		mat = Math::Matrix::CreateScale(m_countGoScale) * transMat;
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		rc = { 0,0,static_cast<int>(m_countGoTex.GetWidth()),static_cast<int>(m_countGoTex.GetHeight()) };
		color = { 1,1,1,m_countGoAlpha };
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_countGoTex, 0, 0, static_cast<int>(m_countGoTex.GetWidth()), static_cast<int>(m_countGoTex.GetHeight()), &rc, &color);
	}


	if (spPlayer)
	{
		transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["WeaponTypeOneTexPos_R"][0].number_value()),
			static_cast<float>(m_mpDedicatedObj["WeaponTypeOneTexPos_R"][1].number_value()),
			static_cast<float>(m_mpDedicatedObj["WeaponTypeOneTexPos_R"][2].number_value()));

		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponType1Tex, 0, 0, m_weaponType1Tex.GetWidth(), m_weaponType1Tex.GetHeight());
		if (spPlayer->GetWeaponType() & Player::WeaponType::grassHopper)
		{
			rc = { 0,0,static_cast<int>(m_weaponTypeOvreDarkTex.GetWidth()),static_cast<int>(m_weaponTypeOvreDarkTex.GetHeight()) };
			color = { 1,1,1,0.6f };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponTypeOvreDarkTex, 0, 0, static_cast<int>(m_weaponTypeOvreDarkTex.GetWidth()), static_cast<int>(m_weaponTypeOvreDarkTex.GetHeight()), &rc, &color);
		}

		transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoTexPos_R"][0].number_value()),
			static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoTexPos_R"][1].number_value()),
			static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoTexPos_R"][2].number_value()));

		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponType2Tex, 0, 0, static_cast<int>(m_weaponType2Tex.GetWidth()), static_cast<int>(m_weaponType2Tex.GetHeight()));
		if (spPlayer->GetWeaponType() & Player::WeaponType::scorpion)
		{
			transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoOvreDarkTexPos_R"][0].number_value()),
				static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoOvreDarkTexPos_R"][1].number_value()),
				static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoOvreDarkTexPos_R"][2].number_value()));

			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			rc = { 0,0,static_cast<int>(m_weaponTypeOvreDarkTex.GetWidth()),static_cast<int>(m_weaponTypeOvreDarkTex.GetHeight()) };
			color = { 1,1,1,0.6f };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponTypeOvreDarkTex, 0, 0, static_cast<int>(m_weaponTypeOvreDarkTex.GetWidth()), static_cast<int>(m_weaponTypeOvreDarkTex.GetHeight()), &rc, &color, Math::Vector2(0, 0.5f));
		}
		else if (spPlayer->GetWeaponType() & Player::WeaponType::grassHopper && spPlayer->GetRGrassHopperPauCnt() != 0)
		{
			transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoOvreDarkTexPos_R"][0].number_value()),
				static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoOvreDarkTexPos_R"][1].number_value()),
				static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoOvreDarkTexPos_R"][2].number_value()));

			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			rc = { 0,0,spPlayer->GetRGrassHopperPauCnt() * 4,static_cast<int>(m_weaponTypeOvreDarkTex.GetHeight()) };
			color = { 1,1,1,0.6f };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponTypeOvreDarkTex, 0, 0, spPlayer->GetRGrassHopperPauCnt() * 4, static_cast<int>(m_weaponTypeOvreDarkTex.GetHeight()), &rc, &color, Math::Vector2(0, 0.5f));
		}

		if (KdInputManager::Instance().GetButtonState("rWeaponChange"))
		{
			transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["WeaponChangeKeyPushStateTexPos_R"][0].number_value()),
				static_cast<float>(m_mpDedicatedObj["WeaponChangeKeyPushStateTexPos_R"][1].number_value()),
				static_cast<float>(m_mpDedicatedObj["WeaponChangeKeyPushStateTexPos_R"][2].number_value()));

			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_rWeaponChangeKeyPushStateTex, 0, 0, static_cast<int>(m_rWeaponChangeKeyPushStateTex.GetWidth()), static_cast<int>(m_rWeaponChangeKeyPushStateTex.GetHeight()));
		}
		else
		{
			transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["WeaponChangeKeyPushStateTexPos_R"][0].number_value()),
				static_cast<float>(m_mpDedicatedObj["WeaponChangeKeyPushStateTexPos_R"][1].number_value()),
				static_cast<float>(m_mpDedicatedObj["WeaponChangeKeyPushStateTexPos_R"][2].number_value()));

			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_rWeaponChangeKeyTex, 0, 0, static_cast<int>(m_rWeaponChangeKeyTex.GetWidth()), static_cast<int>(m_rWeaponChangeKeyTex.GetHeight()));
		}

		transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["WeaponTypeOneTexPos_L"][0].number_value()),
			static_cast<float>(m_mpDedicatedObj["WeaponTypeOneTexPos_L"][1].number_value()),
			static_cast<float>(m_mpDedicatedObj["WeaponTypeOneTexPos_L"][2].number_value()));

		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponType1Tex, 0, 0, static_cast<int>(m_weaponType1Tex.GetWidth()), static_cast<int>(m_weaponType1Tex.GetHeight()));
		if (spPlayer->GetWeaponType() & Player::WeaponType::lGrassHopper)
		{
			rc = { 0,0,static_cast<int>(m_weaponTypeOvreDarkTex.GetWidth()),static_cast<int>(m_weaponTypeOvreDarkTex.GetHeight()) };
			color = { 1,1,1,0.6f };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponTypeOvreDarkTex, 0, 0, static_cast<int>(m_weaponTypeOvreDarkTex.GetWidth()), static_cast<int>(m_weaponTypeOvreDarkTex.GetHeight()), &rc, &color);
		}

		transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoTexPos_L"][0].number_value()),
			static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoTexPos_L"][1].number_value()),
			static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoTexPos_L"][2].number_value()));

		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponType2Tex, 0, 0, static_cast<int>(m_weaponType2Tex.GetWidth()), static_cast<int>(m_weaponType2Tex.GetHeight()));
		if (spPlayer->GetWeaponType() & Player::WeaponType::lScorpion)
		{
			transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoOvreDarkTexPos_L"][0].number_value()),
				static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoOvreDarkTexPos_L"][1].number_value()),
				static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoOvreDarkTexPos_L"][2].number_value()));

			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			rc = { 0,0,static_cast<int>(m_weaponTypeOvreDarkTex.GetWidth()), static_cast<int>(m_weaponTypeOvreDarkTex.GetHeight()) };
			color = { 1,1,1,0.6f };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponTypeOvreDarkTex, 0, 0, static_cast<int>(m_weaponTypeOvreDarkTex.GetWidth()), static_cast<int>(m_weaponTypeOvreDarkTex.GetHeight()), &rc, &color, Math::Vector2(0, 0.5f));
		}
		else if (spPlayer->GetWeaponType() & Player::WeaponType::lGrassHopper && spPlayer->GetLGrassHopperPauCnt() != 0)
		{
			transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoOvreDarkTexPos_L"][0].number_value()),
				static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoOvreDarkTexPos_L"][1].number_value()),
				static_cast<float>(m_mpDedicatedObj["WeaponTypeTwoOvreDarkTexPos_L"][2].number_value()));

			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			rc = { 0,0,spPlayer->GetLGrassHopperPauCnt() * 4,static_cast<int>(m_weaponTypeOvreDarkTex.GetHeight()) };
			color = { 1,1,1,0.6f };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponTypeOvreDarkTex, 0, 0, spPlayer->GetLGrassHopperPauCnt() * 4, static_cast<int>(m_weaponTypeOvreDarkTex.GetHeight()), &rc, &color, Math::Vector2(0, 0.5f));
		}

		if (KdInputManager::Instance().GetButtonState("lWeaponChange"))
		{
			transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["WeaponChangeKeyPushStateTexPos_L"][0].number_value()),
				static_cast<float>(m_mpDedicatedObj["WeaponChangeKeyPushStateTexPos_L"][1].number_value()),
				static_cast<float>(m_mpDedicatedObj["WeaponChangeKeyPushStateTexPos_L"][2].number_value()));

			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_lWeaponChangeKeyPushStateTex, 0, 0, static_cast<int>(m_lWeaponChangeKeyPushStateTex.GetWidth()), static_cast<int>(m_lWeaponChangeKeyPushStateTex.GetHeight()));
		}
		else
		{
			transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["WeaponChangeKeyPushStateTexPos_L"][0].number_value()),
				static_cast<float>(m_mpDedicatedObj["WeaponChangeKeyPushStateTexPos_L"][1].number_value()),
				static_cast<float>(m_mpDedicatedObj["WeaponChangeKeyPushStateTexPos_L"][2].number_value()));

			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_lWeaponChangeKeyTex, 0, 0, static_cast<int>(m_lWeaponChangeKeyTex.GetWidth()), static_cast<int>(m_lWeaponChangeKeyTex.GetHeight()));
		}

		transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["enduranceRelationTexPos"][0].number_value()),
			static_cast<float>(m_mpDedicatedObj["enduranceRelationTexPos"][1].number_value()),
			static_cast<float>(m_mpDedicatedObj["enduranceRelationTexPos"][2].number_value()));

		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		rc = { 0,0,static_cast<int>(m_enduranceBarTex.GetWidth()),static_cast<int>(m_enduranceBarTex.GetHeight()) };
		color = { 1, 1, 1, 1 };
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_enduranceBarTex, 0, 0, static_cast<int>(m_enduranceBarTex.GetWidth()), static_cast<int>(m_enduranceBarTex.GetHeight()), &rc, &color, Math::Vector2(0, 0.5f));

		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		rc = { 0,0,static_cast<int>((spPlayer->GetEndurance())), static_cast<int>(m_enduranceTex.GetHeight()) };
		color = { 1, 1, 1, 1 };
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_enduranceTex, 0, 0, static_cast<int>((spPlayer->GetEndurance())), static_cast<int>(m_enduranceTex.GetHeight()), &rc, &color, Math::Vector2(0, 0.5f));

		transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["TorionBarTexPos"][0].number_value()),
			static_cast<float>(m_mpDedicatedObj["TorionBarTexPos"][1].number_value()),
			static_cast<float>(m_mpDedicatedObj["TorionBarTexPos"][2].number_value()));

		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		rc = { 0,0,static_cast<int>(m_torionBarTex.GetWidth()),static_cast<int>(m_torionBarTex.GetHeight()) };
		color = { 1, 1, 1, 1 };
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_torionBarTex, 0, 0, static_cast<int>(m_torionBarTex.GetWidth()), static_cast<int>(m_torionBarTex.GetHeight()), &rc, &color, Math::Vector2(0, 0.5f));

		transMat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["TorionTexPos"][0].number_value()),
			static_cast<float>(m_mpDedicatedObj["TorionTexPos"][1].number_value()),
			static_cast<float>(m_mpDedicatedObj["TorionTexPos"][2].number_value()));

		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		rc = { 0,0,static_cast<int>(spPlayer->GetVForce()),static_cast<int>(m_torionTex.GetHeight()) };
		color = { 1, 1, 1, 1 };
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_torionTex, 0, 0, static_cast<int>((spPlayer->GetVForce())), static_cast<int>(m_torionTex.GetHeight()), &rc, &color, Math::Vector2(0, 0.5f));

		if (!m_wpEnemy.expired() && !m_wpEnemy.lock()->GetBEnemyDeath())
		{
			Math::Vector3 nowVec = m_wpCamera.lock()->GetMatrix().Backward();
			nowVec.Normalize();

			// 向きたい方向
			Math::Vector3 toVec = m_wpEnemy.lock()->GetPos() - m_wpCamera.lock()->GetPos();
			toVec.Normalize();

			Math::Vector3 dot = DirectX::XMVector3Dot(nowVec, toVec);
			if (dot.x > 1)
			{
				dot.x = 1;
			}
			if (dot.x < -1)
			{
				dot.x = -1;
			}

			// 角度を取得
			float ang = DirectX::XMConvertToDegrees(acos(dot.x));

			if (ang <= 85)
			{
				mat = Math::Matrix::CreateScale(0.2f) * Math::Matrix::CreateTranslation(m_enemyScPos.x, m_enemyScPos.y, 0.0f);

				KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
				rc = { 0,0,static_cast<int>(m_wpEnemy.lock()->GetEndurance()),static_cast<int>(m_enduranceTex.GetHeight()) };
				color = { 1, 1, 1, 1 };
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_enduranceTex, 0, 0, static_cast<int>((m_wpEnemy.lock()->GetEndurance())), static_cast<int>(m_enduranceTex.GetHeight()), &rc, &color, Math::Vector2(0, 0.5f));
				rc = { 0,0,400,50 };
				color = { 1, 1, 1, 1 };
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_enduranceBarTex, 0, 0, static_cast<int>(m_enduranceBarTex.GetWidth()), static_cast<int>(m_enduranceBarTex.GetHeight()), &rc, &color, Math::Vector2(0, 0.5f));
			}

			if (ang >= 45)
			{
				float m_heightDifference = m_wpEnemy.lock()->GetPos().y - m_wpPlayer.lock()->GetPos().y;

				if (m_heightDifference >= 2.0f)
				{
					mat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["AroowUpAndDownPos"][0].number_value()),
						static_cast<float>(m_mpDedicatedObj["AroowUpAndDownPos"][1].number_value()),
						static_cast<float>(m_mpDedicatedObj["AroowUpAndDownPos"][2].number_value()));

					KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
					rc = { 0,0,static_cast<int>(m_EnemyDirectionArrowUTex.GetWidth()), static_cast<int>(m_EnemyDirectionArrowUTex.GetHeight()) };
					color = { 1, 1, 1, 1 };
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_EnemyDirectionArrowUTex, 0, 0, static_cast<int>(m_EnemyDirectionArrowUTex.GetWidth()), static_cast<int>(m_EnemyDirectionArrowUTex.GetHeight()), &rc, &color, Math::Vector2(0, 0.5f));
				}
				else if (m_heightDifference <= -2.0f)
				{
					mat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["AroowUpAndDownPos"][0].number_value()),
						-static_cast<float>(m_mpDedicatedObj["AroowUpAndDownPos"][1].number_value()),
						static_cast<float>(m_mpDedicatedObj["AroowUpAndDownPos"][2].number_value()));

					KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
					rc = { 0,0,static_cast<int>(m_EnemyDirectionArrowBTex.GetWidth()), static_cast<int>(m_EnemyDirectionArrowBTex.GetHeight()) };
					color = { 1, 1, 1, 1 };
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_EnemyDirectionArrowBTex, 0, 0, static_cast<int>(m_EnemyDirectionArrowBTex.GetWidth()), static_cast<int>(m_EnemyDirectionArrowBTex.GetHeight()), &rc, &color, Math::Vector2(0, 0.5f));
				}


				Math::Vector3 cross = DirectX::XMVector3Cross(nowVec, toVec);

				if (cross.y < 0)
				{
					mat = Math::Matrix::CreateTranslation(-static_cast<float>(m_mpDedicatedObj["AroowLeftAndRightPos"][0].number_value()),
						static_cast<float>(m_mpDedicatedObj["AroowLeftAndRightPos"][1].number_value()),
						static_cast<float>(m_mpDedicatedObj["AroowLeftAndRightPos"][2].number_value()));

					KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
					rc = { 0,0, static_cast<int>(m_EnemyDirectionArrowLTex.GetWidth()), static_cast<int>(m_EnemyDirectionArrowLTex.GetHeight()) };
					color = { 1, 1, 1, 1 };
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_EnemyDirectionArrowLTex, 0, 0, static_cast<int>(m_EnemyDirectionArrowLTex.GetWidth()), static_cast<int>(m_EnemyDirectionArrowLTex.GetHeight()), &rc, &color, Math::Vector2(0, 0.5f));
				}
				else if (cross.y >= 0)
				{
					mat = Math::Matrix::CreateTranslation(static_cast<float>(m_mpDedicatedObj["AroowLeftAndRightPos"][0].number_value()),
						static_cast<float>(m_mpDedicatedObj["AroowLeftAndRightPos"][1].number_value()),
						static_cast<float>(m_mpDedicatedObj["AroowLeftAndRightPos"][2].number_value()));

					KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
					rc = { 0,0,static_cast<int>(m_EnemyDirectionArrowRTex.GetWidth()), static_cast<int>(m_EnemyDirectionArrowRTex.GetHeight()) };
					color = { 1, 1, 1, 1 };
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_EnemyDirectionArrowRTex, 0, 0, static_cast<int>(m_EnemyDirectionArrowRTex.GetWidth()), static_cast<int>(m_EnemyDirectionArrowRTex.GetHeight()), &rc, &color, Math::Vector2(0, 0.5f));
				}
			}
		}
	}

	if (m_bOption)
	{
		transMat = Math::Matrix::Identity;
		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		color = { 0,0,0,0.6f };
		KdShaderManager::Instance().m_spriteShader.DrawBox(0, 0, 1280, 720, &color);

		if (!m_bOperation)
		{
			mat = Math::Matrix::CreateScale(m_operationScale) * Math::Matrix::CreateTranslation(m_operationPos);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_operationTex, 0, 0, static_cast<int>(m_operationTex.GetWidth()), static_cast<int>(m_operationTex.GetHeight()));

			mat = Math::Matrix::CreateScale(m_selectScale) * Math::Matrix::CreateTranslation(m_selectPos);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_selectTex, 0, 0, static_cast<int>(m_selectTex.GetWidth()), static_cast<int>(m_selectTex.GetHeight()));

			mat = Math::Matrix::CreateScale(m_exitScale) * Math::Matrix::CreateTranslation(m_exitPos);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_exitTex, 0, 0, static_cast<int>(m_exitTex.GetWidth()), static_cast<int>(m_exitTex.GetHeight()));
		}
		else if (m_bOperation)
		{
			if (m_bHowToPage)
			{
				transMat = Math::Matrix::Identity;
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_howToTex, 0, 0, static_cast<int>(m_howToTex.GetWidth()), static_cast<int>(m_howToTex.GetHeight()));
			}

			if (m_bWeaponDataPage)
			{
				if (m_bWeaponDataHopperPage)
				{
					transMat = Math::Matrix::Identity;
					KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_hopperDataTex, 0, 0, static_cast<int>(m_hopperDataTex.GetWidth()), static_cast<int>(m_hopperDataTex.GetHeight()));
				}
				else if (m_bWeaponDataScoPage)
				{
					transMat = Math::Matrix::Identity;
					KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponInfoTex, 0, 0, static_cast<int>(m_weaponInfoTex.GetWidth()), static_cast<int>(m_weaponInfoTex.GetHeight()));
				}

				mat = Math::Matrix::CreateScale(m_weaponRightYaiScale) * Math::Matrix::CreateTranslation(m_weaponRightYaiPos);
				KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_rightYaiTex, 0, 0, static_cast<int>(m_rightYaiTex.GetWidth()), static_cast<int>(m_rightYaiTex.GetHeight()));

				mat = Math::Matrix::CreateScale(m_weaponLeftYaiScale) * Math::Matrix::CreateTranslation(m_weaponLeftYaiPos);
				KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_leftYaiTex, 0, 0, static_cast<int>(m_leftYaiTex.GetWidth()), static_cast<int>(m_leftYaiTex.GetHeight()));
			}

			mat = Math::Matrix::CreateScale(m_weaOrHowRightYaiScale) * Math::Matrix::CreateTranslation(m_weaOrHowRightYaiPos);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_rightYaiTex, 0, 0, static_cast<int>(m_rightYaiTex.GetWidth()), static_cast<int>(m_rightYaiTex.GetHeight()));

			mat = Math::Matrix::CreateScale(m_weaOrHowLeftYaiScale) * Math::Matrix::CreateTranslation(m_weaOrHowLeftYaiPos);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_leftYaiTex, 0, 0, static_cast<int>(m_leftYaiTex.GetWidth()), static_cast<int>(m_leftYaiTex.GetHeight()));

		}

		mat = Math::Matrix::CreateScale(m_backScale) * Math::Matrix::CreateTranslation(m_backPos);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_backTex, 0, 0, static_cast<int>(m_backTex.GetWidth()), static_cast<int>(m_backTex.GetHeight()));
	}

	transMat = Math::Matrix::Identity;
	KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
	color = { 0,0,0,m_fadeAlpha };
	KdShaderManager::Instance().m_spriteShader.DrawBox(0, 0, 1280, 720, &color);
}

void TrainingUi::SetEnemy(std::shared_ptr<Enemy> a_spEnemy)
{
	m_wpEnemy = a_spEnemy;
}

void TrainingUi::OptionUpdate()
{
	if (!m_bOperation)
	{
		if (ButtomProcessing({ m_exitPos.x,m_exitPos.y }, m_exitTex, m_exitScale))
		{
			if (!m_addFadeAlpha)
			{
				m_bExit = true;
				m_addFadeAlpha = true;
				m_bFirstExit = true;
				KdAudioManager::Instance().Play("Asset/Audio/SE/PushButton.wav");
			}
		}

		if (ButtomProcessing({ m_selectPos.x,m_selectPos.y }, m_selectTex, m_selectScale))
		{
			m_bSelect = true;
			m_addFadeAlpha = true;
			KdAudioManager::Instance().Play("Asset/Audio/SE/PushButton.wav");
		}

		if (ButtomProcessing({ m_operationPos.x,m_operationPos.y }, m_operationTex, m_operationScale))
		{
			m_bOperation = true;
			KdAudioManager::Instance().Play("Asset/Audio/SE/PushButton.wav");
		}

		if (ButtomProcessing({ m_backPos.x,m_backPos.y }, m_backTex, m_backScale,0.8f))
		{
			m_bOption = false;
			ShowCursor(false); // マウスカーソルを消す
			SetCursorPos(640, 360);
			KdAudioManager::Instance().Play("Asset/Audio/SE/backPush.wav");
			KdEffekseerManager::GetInstance().OnResumeEfkUpdate();
		}

		if (m_addFadeAlpha)
		{
			m_fadeAlpha += m_screenAlphaFadeSpeed;
			if (m_fadeAlpha >= 1.0f)
			{
				m_fadeAlpha = 1.0f;

				if (m_bExit)
				{
					if (m_bFirstExit)
					{
						SceneManager::Instance().SetBWinEnd();
						m_bFirstExit = false;
					}
					else
					{
						m_bExit = false;
						m_addFadeAlpha = false;
						m_bFirstExit = true;
						m_fadeAlpha = 0.0f;
					}
				}

				if (m_bSelect)
				{
					KdEffekseerManager::GetInstance().Reset();
					SceneManager::Instance().SetNextScene
					(
						SceneManager::SceneType::select
					);
				}
			}
		}
	}
	else
	{
		if (ButtomProcessing({ m_backPos.x,m_backPos.y }, m_backTex, m_backScale,0.8f))
		{
			m_bWeaponDataPage = false;
			m_bWeaponDataHopperPage = false;
			m_bWeaponDataScoPage = true;
			m_bHowToPage = true;
			m_bOperation = false;
			KdAudioManager::Instance().Play("Asset/Audio/SE/backPush.wav");
		}

		if (ButtomProcessing({ m_weaOrHowLeftYaiPos.x,m_weaOrHowLeftYaiPos.y }, m_leftYaiTex, m_weaOrHowLeftYaiScale))
		{
			if (m_bWeaponDataPage)
			{
				m_bHowToPage = true;
				m_bWeaponDataPage = false;
			}
			else
			{
				m_bHowToPage = false;
				m_bWeaponDataPage = true;
			}
			KdAudioManager::Instance().Play("Asset/Audio/SE/PushArrow.wav");
		}

		if (ButtomProcessing({ m_weaOrHowRightYaiPos.x,m_weaOrHowRightYaiPos.y }, m_rightYaiTex, m_weaOrHowRightYaiScale))
		{
			if (m_bWeaponDataPage)
			{
				m_bHowToPage = true;
				m_bWeaponDataPage = false;
			}
			else
			{
				m_bHowToPage = false;
				m_bWeaponDataPage = true;
			}
			KdAudioManager::Instance().Play("Asset/Audio/SE/PushArrow.wav");
		}

		if (m_bWeaponDataPage)
		{
			if (ButtomProcessing({ m_weaponLeftYaiPos.x,m_weaponLeftYaiPos.y }, m_leftYaiTex, m_weaponLeftYaiScale))
			{
				if (m_bWeaponDataHopperPage)
				{
					m_bWeaponDataScoPage = true;
					m_bWeaponDataHopperPage = false;
				}
				else
				{
					m_bWeaponDataScoPage = false;
					m_bWeaponDataHopperPage = true;
				}
				KdAudioManager::Instance().Play("Asset/Audio/SE/PushArrow.wav");
			}

			if (ButtomProcessing({ m_weaponRightYaiPos.x,m_weaponRightYaiPos.y }, m_rightYaiTex, m_weaponRightYaiScale))
			{
				if (m_bWeaponDataHopperPage)
				{
					m_bWeaponDataScoPage = true;
					m_bWeaponDataHopperPage = false;
				}
				else
				{
					m_bWeaponDataScoPage = false;
					m_bWeaponDataHopperPage = true;
				}
				KdAudioManager::Instance().Play("Asset/Audio/SE/PushArrow.wav");
			}
		}
	}
}
// ＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝