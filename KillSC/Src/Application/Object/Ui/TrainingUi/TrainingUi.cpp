#include "TrainingUi.h"
#include "../../../Scene/SceneManager.h"
#include "../../Character/Player/Player.h"
#include "../../Character/Enemy/Enemy.h"
#include "../../../Camera/GameCamera/GameCamera.h"

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

	m_selectPos = { (float)m_mpDedicatedObj["ModeSelectButtomPos"][0].number_value(),
				   (float)m_mpDedicatedObj["ModeSelectButtomPos"][1].number_value(),
				   (float)m_mpDedicatedObj["ModeSelectButtomPos"][2].number_value() };

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
	m_weaponLeftYaiPos = { (float)m_mpDedicatedObj["WeaponLeftYaiButtomPos"][0].number_value(),
						  (float)m_mpDedicatedObj["WeaponLeftYaiButtomPos"][1].number_value(),
						  (float)m_mpDedicatedObj["WeaponLeftYaiButtomPos"][2].number_value() };

	m_weaponRightYaiScale = 1.0f;
	m_weaponRightYaiPos = { (float)m_mpDedicatedObj["WeaponRightYaiButtomPos"][0].number_value(),
						   (float)m_mpDedicatedObj["WeaponRightYaiButtomPos"][1].number_value(),
						   (float)m_mpDedicatedObj["WeaponRightYaiButtomPos"][2].number_value() };

	m_weaOrHowLeftYaiScale = 1.0f;
	m_weaOrHowLeftYaiPos = { (float)m_mpDedicatedObj["WeaOrHowLeftYaiButtomPos"][0].number_value(),
							(float)m_mpDedicatedObj["WeaOrHowLeftYaiButtomPos"][1].number_value(),
							(float)m_mpDedicatedObj["WeaOrHowLeftYaiButtomPos"][2].number_value() };

	m_weaOrHowRightYaiScale = 1.0f;
	m_weaOrHowRightYaiPos = { (float)m_mpDedicatedObj["WeaOrHowRightYaiButtomPos"][0].number_value(),
							 (float)m_mpDedicatedObj["WeaOrHowRightYaiButtomPos"][1].number_value(),
							 (float)m_mpDedicatedObj["WeaOrHowRightYaiButtomPos"][2].number_value() };

	m_operationScale = 1.0f;
	m_operationPos = { (float)m_mpDedicatedObj["OperationButtomPos"][0].number_value(),
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

		if (ButtomProcessing({ m_backPos.x,m_backPos.y }, m_backTex, m_backScale, 0.8f))
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
		if (ButtomProcessing({ m_backPos.x,m_backPos.y }, m_backTex, m_backScale, 0.8f))
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