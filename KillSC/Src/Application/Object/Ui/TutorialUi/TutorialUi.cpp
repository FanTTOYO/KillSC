#include "TutorialUi.h"
#include "../../../Scene/SceneManager.h"
#include "../../Character/Player/Player.h"

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
	m_exitPos = { (float)(m_mpDedicatedObj["ExitButtomPos"][0].number_value()),
				 (float)(m_mpDedicatedObj["ExitButtomPos"][1].number_value()),
				 (float)(m_mpDedicatedObj["ExitButtomPos"][2].number_value()) };

	m_fadeAlpha = 0.0f;
	m_backPos = { (float)(m_mpDedicatedObj["BackButtomPos"][0].number_value()),
				 (float)(m_mpDedicatedObj["BackButtomPos"][1].number_value()),
				 (float)(m_mpDedicatedObj["BackButtomPos"][2].number_value()) };

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
	m_weaponRightYaiPos = { (float)(m_mpDedicatedObj["WeaponRightYaiButtomPos"][0].number_value()),
						   (float)(m_mpDedicatedObj["WeaponRightYaiButtomPos"][1].number_value()),
						   (float)(m_mpDedicatedObj["WeaponRightYaiButtomPos"][2].number_value()) };

	m_weaOrHowLeftYaiScale = 1.0f;
	m_weaOrHowLeftYaiPos = { (float)(m_mpDedicatedObj["WeaOrHowLeftYaiButtomPos"][0].number_value()),
							(float)(m_mpDedicatedObj["WeaOrHowLeftYaiButtomPos"][1].number_value()),
							(float)(m_mpDedicatedObj["WeaOrHowLeftYaiButtomPos"][2].number_value()) };

	m_weaOrHowRightYaiScale = 1.0f;
	m_weaOrHowRightYaiPos = { (float)(m_mpDedicatedObj["WeaOrHowRightYaiButtomPos"][0].number_value()),
							 (float)(m_mpDedicatedObj["WeaOrHowRightYaiButtomPos"][1].number_value()),
							 (float)(m_mpDedicatedObj["WeaOrHowRightYaiButtomPos"][2].number_value()) };

	m_operationScale = 1.0f;
	m_operationPos = { (float)(m_mpDedicatedObj["OperationButtomPos"][0].number_value()),
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