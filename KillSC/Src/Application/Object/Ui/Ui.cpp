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

void Ui::Init()
{
	m_currentUiClassId->Init();
}

// タイトルのUI関連の処理＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
void TitleUi::Init()
{
	m_pushLClickTex.Load("Asset/Textures/Ui/shared/PUSHTLCLICK.png");
	m_titleCharRogoTex.Load("Asset/Textures/Ui/Title/KillSC.png");
	m_FTtoyoRogoTex.Load("Asset/Textures/Ui/Title/FTtoyo.png");
	m_time = 0;
	m_addFadeAlpha = false;
	m_fadeAlpha = 1.0f;
	m_bSceneCangePossible = false;
	m_pushLClickAlpha = 1.0f;
	m_bPushLClickAlphaAdd = false;
}

void TitleUi::Update()
{
	m_time++;
	if (m_time >= 480)
	{
		m_time = 480;
		m_bSceneCangePossible = true;

		if (m_bPushLClickAlphaAdd)
		{
			m_pushLClickAlpha += (0.8f / 30);
			if (m_pushLClickAlpha >= 1.0f)
			{
				m_pushLClickAlpha = 1.0f;
				m_bPushLClickAlphaAdd = false;
			}
		}
		else
		{
			m_pushLClickAlpha -= (0.8f / 30);
			if (m_pushLClickAlpha <= 0.2f)
			{
				m_pushLClickAlpha = 0.2f;
				m_bPushLClickAlphaAdd = true;
			}
		}

		if (m_addFadeAlpha)
		{
			m_fadeAlpha += (1.0f / 60.0f);
			if (m_fadeAlpha >= 1.0f)
			{
				m_fadeAlpha = 1.0f;

				if (m_uiType == UiType::title)
				{
					SceneManager::Instance().SetNextScene
					(
						SceneManager::SceneType::select
					);
				}
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
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_FTtoyoRogoTex, 0, 0, (int)m_FTtoyoRogoTex.GetWidth(), (int)m_FTtoyoRogoTex.GetHeight());
	}
	else if (m_time >= 360)
	{
		transMat = Math::Matrix::CreateTranslation(0, 200, 0);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_titleCharRogoTex, 0, 0, (int)m_titleCharRogoTex.GetWidth(), (int)m_titleCharRogoTex.GetHeight());

		transMat = Math::Matrix::CreateTranslation(0, -200, 0);
		Math::Rectangle rc = { 0,0,(int)m_pushLClickTex.GetWidth(),(int)m_pushLClickTex.GetHeight() };
		color = { 1,1,1,m_pushLClickAlpha };
		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_pushLClickTex, 0, 0, (int)m_pushLClickTex.GetWidth(), (int)m_pushLClickTex.GetHeight(), &rc, &color);
	}

	transMat = Math::Matrix::Identity;
	KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
	color = { 0,0,0,m_fadeAlpha };
	KdShaderManager::Instance().m_spriteShader.DrawBox(0, 0, 1280, 720, &color);
}
// ＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝

// ゲームモードのUI関連の処理＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
void GameUi::Init()
{
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

	m_fadeAlpha = 0.0f;
	m_exitScale = 1.0f;
	m_selectScale = 1.0f;

	m_selectPos = { 0,0,0 };
	m_exitPos = { 0,-250,0 };

	m_backPos = { 550,-300,0 };
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
	m_weaponLeftYaiPos = { -500,250,0 };

	m_weaponRightYaiScale = 1.0f;
	m_weaponRightYaiPos = { 500,250,0, };

	m_weaOrHowLeftYaiScale = 1.0f;
	m_weaOrHowLeftYaiPos = { -600,0,0 };

	m_weaOrHowRightYaiScale = 1.0f;
	m_weaOrHowRightYaiPos = { 600,0,0 };

	m_operationScale = 1.0f;
	m_operationPos = { 0,250,0 };
	m_bOperation = false;
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

	if (m_time <= 240)
	{
		m_time++;
	}

	if (SceneManager::Instance().GetSceneType() == SceneManager::SceneType::battle)
	{
		if (m_time >= 0 && m_time < 90)
		{
			m_waveScale += (1.0f / 10.0f);
			if (m_waveScale > 1.0f)
			{
				m_waveScale = 1.0f;
			}

			m_waveAlpha += (1.0f / 30.0f);
			if (m_waveAlpha > 1.0f)
			{
				m_waveAlpha = 1.0f;
			}
		}
		else if (m_time >= 90 && m_time < 180)
		{
			m_waveAlpha -= (1.0f / 60.0f);
			if (m_waveAlpha < 0.0f)
			{
				m_waveAlpha = 0.0f;
			}
		}
		else if (m_time >= 180 && m_time < 210)
		{
			m_countGoScale += (1.0f / 10.0f);
			if (m_countGoScale > 1.0f)
			{
				m_countGoScale = 1.0f;
			}

			m_countGoAlpha += (1.0f / 30.0f);
			if (m_countGoAlpha > 1.0f)
			{
				m_countGoAlpha = 1.0f;
			}
		}
		else if (m_time >= 210 && m_time < 240)
		{
			m_countGoAlpha -= (1.0f / 30.0f);
			if (m_countGoAlpha < 0.0f)
			{
				m_countGoAlpha = 0.0f;
				SetCursorPos(640, 360);
				m_waveAlpha = 0.0f;
				m_waveScale = 0.1f;
			}
		}
	}

	if (m_time >= 0 && m_time < 30)
	{
		m_countThreeScale += (1.0f / 10.0f);
		if (m_countThreeScale > 1.0f)
		{
			m_countThreeScale = 1.0f;
		}

		m_countThreeAlpha += (1.0f / 30.0f);
		if (m_countThreeAlpha > 1.0f)
		{
			m_countThreeAlpha = 1.0f;
		}
	}
	else if (m_time >= 30 && m_time < 60)
	{

		m_countThreeAlpha -= (1.0f / 30.0f);
		if (m_countThreeAlpha < 0.0f)
		{
			m_countThreeAlpha = 0.0f;
		}
	}
	else if (m_time >= 60 && m_time < 90)
	{
		m_countTwoScale += (1.0f / 10.0f);
		if (m_countTwoScale > 1.0f)
		{
			m_countTwoScale = 1.0f;
		}

		m_countTwoAlpha += (1.0f / 30.0f);
		if (m_countTwoAlpha > 1.0f)
		{
			m_countTwoAlpha = 1.0f;
		}
	}
	else if (m_time >= 90 && m_time < 120)
	{
		m_countTwoAlpha -= (1.0f / 30.0f);
		if (m_countThreeAlpha < 0.0f)
		{
			m_countThreeAlpha = 0.0f;
		}
	}
	else if (m_time >= 120 && m_time < 150)
	{
		m_countOneScale += (1.0f / 10.0f);
		if (m_countOneScale > 1.0f)
		{
			m_countOneScale = 1.0f;
		}

		m_countOneAlpha += (1.0f / 30.0f);
		if (m_countOneAlpha > 1.0f)
		{
			m_countOneAlpha = 1.0f;
		}
	}
	else if (m_time >= 150 && m_time < 180)
	{
		m_countOneAlpha -= (1.0f / 30.0f);
		if (m_countOneAlpha < 0.0f)
		{
			m_countOneAlpha = 0.0f;
		}
	}
	else if (m_time >= 180 && m_time < 210)
	{
		m_countGoScale += (1.0f / 10.0f);
		if (m_countGoScale > 1.0f)
		{
			m_countGoScale = 1.0f;
		}

		m_countGoAlpha += (1.0f / 30.0f);
		if (m_countGoAlpha > 1.0f)
		{
			m_countGoAlpha = 1.0f;
		}
	}
	else if (m_time >= 210 && m_time < 240)
	{
		m_countGoAlpha -= (1.0f / 30.0f);
		if (m_countGoAlpha < 0.0f)
		{
			m_countGoAlpha = 0.0f;
			SetCursorPos(640, 360);
		}
	}


	if (m_time > 240)
	{

		if (SceneManager::Instance().GetSceneType() == SceneManager::SceneType::battle)
		{
			if (m_bWaveChange)
			{
				++m_waveTimeCnt;
				if (m_waveTimeCnt >= 0 && m_waveTimeCnt < 90)
				{
					m_waveScale += (1.0f / 10.0f);
					if (m_waveScale > 1.0f)
					{
						m_waveScale = 1.0f;
					}

					m_waveAlpha += (1.0f / 30.0f);
					if (m_waveAlpha > 1.0f)
					{
						m_waveAlpha = 1.0f;
					}
				}
				else if (m_waveTimeCnt >= 90 && m_waveTimeCnt < 180)
				{
					m_waveAlpha -= (1.0f / 60.0f);
					if (m_waveAlpha < 0.0f)
					{
						m_waveAlpha = 0.0f;
					}
				}
				else if (m_waveTimeCnt >= 180)
				{
					m_bWaveChange = false;
					m_waveTimeCnt = 0;
				}
			}
		}

		PWINDOWINFO pwi = new WINDOWINFO;
		pwi->cbSize = sizeof(WINDOWINFO);
		GetWindowInfo(Application::Instance().GetWindowHandle(), pwi);

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

		KdSafeDelete(pwi);
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
		Math::Vector3 pos = Math::Vector3(list.lock()->GetPos().x, list.lock()->GetPos().y + 1.2f, list.lock()->GetPos().z);

		POINT dev;
		KdDirect3D::Instance().WorldToClient(pos, dev, m_wpCamera.lock()->WorkCamera()->GetCameraMatrix(), m_wpCamera.lock()->WorkCamera()->GetProjMatrix());
		dev.x -= (long)640.0f;
		dev.y = (long)(dev.y * -1 + 360.0f);
		float z = m_wpCamera.lock()->GetPos().z - list.lock()->GetPos().z;
		dev.y += (long)(180 - (std::fabs(z) * 1.5f));
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
	std::shared_ptr<Player> spPlayer = m_wpPlayer.lock();

	if (SceneManager::Instance().GetSceneType() == SceneManager::SceneType::battle)
	{
		if (m_time >= 0 && m_time < 180)
		{
			transMat = Math::Matrix::CreateTranslation(-75, 200, 0);
			mat = Math::Matrix::CreateScale(m_waveScale) * transMat;
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			Math::Rectangle rc = { 0,0,(int)m_waveTex.GetWidth(), (int)m_waveTex.GetHeight() };
			color = { 1,1,1,m_waveAlpha };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_waveTex, 0, 0, (int)m_waveTex.GetWidth(), (int)m_waveTex.GetHeight(), &rc, &color);

			transMat = Math::Matrix::CreateTranslation(175, 190, 0);
			mat = Math::Matrix::CreateScale(m_waveScale) * transMat;
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			rc = { 0,0,(int)m_PointTex[1].GetWidth(), (int)m_PointTex[1].GetHeight() };
			color = { 1,1,1,m_waveAlpha };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_PointTex[1], 0, 0, (int)m_PointTex[1].GetWidth(), (int)m_PointTex[1].GetHeight(), &rc, &color);

		}

		if (m_waveCnt >= 2)
		{
			if (m_waveTimeCnt >= 0 && m_waveTimeCnt < 180)
			{
				transMat = Math::Matrix::CreateTranslation(-75, 200, 0);
				mat = Math::Matrix::CreateScale(m_waveScale) * transMat;
				KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
				Math::Rectangle rc = { 0,0,(int)m_waveTex.GetWidth(), (int)m_waveTex.GetHeight() };
				color = { 1,1,1,m_waveAlpha };
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_waveTex, 0, 0, (int)m_waveTex.GetWidth(), (int)m_waveTex.GetHeight(), &rc, &color);

				transMat = Math::Matrix::CreateTranslation(175, 190, 0);
				mat = Math::Matrix::CreateScale(m_waveScale) * transMat;
				KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
				rc = { 0,0,(int)m_PointTex[2].GetWidth(), (int)m_PointTex[2].GetHeight() };
				color = { 1,1,1,m_waveAlpha };
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_PointTex[2], 0, 0, (int)m_PointTex[2].GetWidth(), (int)m_PointTex[2].GetHeight(), &rc, &color);
			}
		}
	}

	transMat = Math::Matrix::Identity;

	if (m_time >= 0 && m_time < 60)
	{
		mat = Math::Matrix::CreateScale(m_countThreeScale) * transMat;
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		Math::Rectangle rc = { 0,0,(int)m_countThreeTex.GetWidth(), (int)m_countThreeTex.GetHeight() };
		color = { 1,1,1,m_countThreeAlpha };
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_countThreeTex, 0, 0, (int)m_countThreeTex.GetWidth(), (int)m_countThreeTex.GetHeight(), &rc, &color);
	}
	else if (m_time >= 60 && m_time < 120)
	{
		mat = Math::Matrix::CreateScale(m_countTwoScale) * transMat;
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		Math::Rectangle rc = { 0,0,(int)m_countTwoTex.GetWidth(), (int)m_countTwoTex.GetHeight() };
		color = { 1,1,1,m_countTwoAlpha };
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_countTwoTex, 0, 0, (int)m_countTwoTex.GetWidth(), (int)m_countTwoTex.GetHeight(), &rc, &color);
	}
	else if (m_time >= 120 && m_time < 180)
	{
		mat = Math::Matrix::CreateScale(m_countOneScale) * transMat;
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		Math::Rectangle rc = { 0,0,(int)m_countOneTex.GetWidth(), (int)m_countOneTex.GetHeight() };
		color = { 1,1,1,m_countOneAlpha };
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_countOneTex, 0, 0, (int)m_countOneTex.GetWidth(), (int)m_countOneTex.GetHeight(), &rc, &color);
	}
	else if (m_time >= 180 && m_time < 240)
	{
		mat = Math::Matrix::CreateScale(m_countGoScale) * transMat;
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		Math::Rectangle rc = { 0,0,(int)m_countGoTex.GetWidth(),(int)m_countGoTex.GetHeight() };
		color = { 1,1,1,m_countGoAlpha };
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_countGoTex, 0, 0, (int)m_countGoTex.GetWidth(), (int)m_countGoTex.GetHeight(), &rc, &color);
	}


	if (spPlayer)
	{
		transMat = Math::Matrix::CreateTranslation(350, -250, 0);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponType1Tex, 0, 0, m_weaponType1Tex.GetWidth(), m_weaponType1Tex.GetHeight());
		if (spPlayer->GetWeaponType() & Player::WeaponType::grassHopper)
		{
			Math::Rectangle rc = { 0,0,(int)m_weaponTypeOvreDarkTex.GetWidth(),(int)m_weaponTypeOvreDarkTex.GetHeight() };
			color = { 1,1,1,0.6f };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponTypeOvreDarkTex, 0, 0, (int)m_weaponTypeOvreDarkTex.GetWidth(), (int)m_weaponTypeOvreDarkTex.GetHeight(), &rc, &color);
		}

		transMat = Math::Matrix::CreateTranslation(500, -250, 0);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponType2Tex, 0, 0, (int)m_weaponType2Tex.GetWidth(), (int)m_weaponType2Tex.GetHeight());
		if (spPlayer->GetWeaponType() & Player::WeaponType::scorpion)
		{
			transMat = Math::Matrix::CreateTranslation(440, -250, 0);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			Math::Rectangle rc = { 0,0,(int)m_weaponTypeOvreDarkTex.GetWidth(),(int)m_weaponTypeOvreDarkTex.GetHeight() };
			color = { 1,1,1,0.6f };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponTypeOvreDarkTex, 0, 0, (int)m_weaponTypeOvreDarkTex.GetWidth(), (int)m_weaponTypeOvreDarkTex.GetHeight(), &rc, &color, Math::Vector2(0, 0.5f));
		}
		else if (spPlayer->GetWeaponType() & Player::WeaponType::grassHopper && spPlayer->GetRGrassHopperPauCnt() != 0)
		{
			transMat = Math::Matrix::CreateTranslation(440, -250, 0);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			Math::Rectangle rc = { 0,0,spPlayer->GetRGrassHopperPauCnt() * 4,(int)m_weaponTypeOvreDarkTex.GetHeight() };
			color = { 1,1,1,0.6f };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponTypeOvreDarkTex, 0, 0, spPlayer->GetRGrassHopperPauCnt() * 4, (int)m_weaponTypeOvreDarkTex.GetHeight(), &rc, &color, Math::Vector2(0, 0.5f));
		}

		transMat = Math::Matrix::CreateTranslation(-500, -250, 0);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponType1Tex, 0, 0, (int)m_weaponType1Tex.GetWidth(), (int)m_weaponType1Tex.GetHeight());
		if (spPlayer->GetWeaponType() & Player::WeaponType::lGrassHopper)
		{
			Math::Rectangle rc = { 0,0,(int)m_weaponTypeOvreDarkTex.GetWidth(),(int)m_weaponTypeOvreDarkTex.GetHeight() };
			color = { 1,1,1,0.6f };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponTypeOvreDarkTex, 0, 0, (int)m_weaponTypeOvreDarkTex.GetWidth(), (int)m_weaponTypeOvreDarkTex.GetHeight(), &rc, &color);
		}

		transMat = Math::Matrix::CreateTranslation(-350, -250, 0);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponType2Tex, 0, 0, (int)m_weaponType2Tex.GetWidth(), (int)m_weaponType2Tex.GetHeight());
		if (spPlayer->GetWeaponType() & Player::WeaponType::lScorpion)
		{
			transMat = Math::Matrix::CreateTranslation(-410, -250, 0);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			Math::Rectangle rc = { 0,0,(int)m_weaponTypeOvreDarkTex.GetWidth(),(int)m_weaponTypeOvreDarkTex.GetHeight() };
			color = { 1,1,1,0.6f };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponTypeOvreDarkTex, 0, 0, (int)m_weaponTypeOvreDarkTex.GetWidth(), (int)m_weaponTypeOvreDarkTex.GetHeight(), &rc, &color, Math::Vector2(0, 0.5f));
		}
		else if (spPlayer->GetWeaponType() & Player::WeaponType::lGrassHopper && spPlayer->GetLGrassHopperPauCnt() != 0)
		{
			transMat = Math::Matrix::CreateTranslation(-410, -250, 0);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			Math::Rectangle rc = { 0,0,spPlayer->GetLGrassHopperPauCnt() * 4,(int)m_weaponTypeOvreDarkTex.GetHeight() };
			color = { 1,1,1,0.6f };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponTypeOvreDarkTex, 0, 0, spPlayer->GetLGrassHopperPauCnt() * 4, (int)m_weaponTypeOvreDarkTex.GetHeight(), &rc, &color, Math::Vector2(0, 0.5f));
		}

		transMat = Math::Matrix::CreateTranslation(-630, 300, 0);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		Math::Rectangle rc = { 0,0,(int)m_enduranceBarTex.GetWidth(),(int)m_enduranceBarTex.GetHeight() };
		color = { 1, 1, 1, 1 };
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_enduranceBarTex, 0, 0, (int)m_enduranceBarTex.GetWidth(), (int)m_enduranceBarTex.GetHeight(), &rc, &color, Math::Vector2(0, 0.5f));

		transMat = Math::Matrix::CreateTranslation(-630, 300, 0);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		rc = { 0,0,(int)(spPlayer->GetEndurance()),(int)m_enduranceTex.GetHeight() };
		color = { 1, 1, 1, 1 };
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_enduranceTex, 0, 0, (int)(spPlayer->GetEndurance()), (int)m_enduranceTex.GetHeight(), &rc, &color, Math::Vector2(0, 0.5f));

		transMat = Math::Matrix::CreateTranslation(-630, 255, 0);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		rc = { 0,0,(int)m_torionBarTex.GetWidth(),(int)m_torionBarTex.GetHeight() };
		color = { 1, 1, 1, 1 };
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_torionBarTex, 0, 0, (int)m_torionBarTex.GetWidth(), (int)m_torionBarTex.GetHeight(), &rc, &color, Math::Vector2(0, 0.5f));

		transMat = Math::Matrix::CreateTranslation(-620, 255, 0);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		rc = { 0,0,(int)(spPlayer->GetVForce()),(int)m_torionTex.GetHeight() };
		color = { 1, 1, 1, 1 };
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_torionTex, 0, 0, (int)(spPlayer->GetVForce()), (int)m_torionTex.GetHeight(), &rc, &color, Math::Vector2(0, 0.5f));

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
				rc = { 0,0,(int)(list.lock()->GetEndurance()),(int)m_enduranceTex.GetHeight() };
				color = { 1, 1, 1, 1 };
				if (SceneManager::Instance().GetSceneType() == SceneManager::SceneType::challenge)
				{
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_enduranceTex, 0, 0, (int)(list.lock()->GetEndurance() * 2.6f), (int)m_enduranceTex.GetHeight(), &rc, &color, Math::Vector2(0, 0.5f));
				}
				else
				{
					if (list.lock()->GetBBoss())
					{
						KdShaderManager::Instance().m_spriteShader.DrawTex(&m_enduranceTex, 0, 0, (int)(list.lock()->GetEndurance()), (int)m_enduranceTex.GetHeight(), &rc, &color, Math::Vector2(0, 0.5f));
					}
					else
					{
						KdShaderManager::Instance().m_spriteShader.DrawTex(&m_enduranceTex, 0, 0, (int)(list.lock()->GetEndurance() * 4.0f), (int)m_enduranceTex.GetHeight(), &rc, &color, Math::Vector2(0, 0.5f));
					}
				}

				rc = { 0,0,400,50 };
				color = { 1, 1, 1, 1 };
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_enduranceBarTex, 0, 0, (int)m_enduranceBarTex.GetWidth(), (int)m_enduranceBarTex.GetHeight(), &rc, &color, Math::Vector2(0, 0.5f));
			}

			if (ang >= 45)
			{
				float m_heightDifference = list.lock()->GetPos().y - m_wpPlayer.lock()->GetPos().y;

				if (m_heightDifference >= 2.0f)
				{
					mat = Math::Matrix::CreateTranslation(-25, 225.0f, 0.0f);
					KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
					rc = { 0,0,(int)m_EnemyDirectionArrowUTex.GetWidth(),(int)m_EnemyDirectionArrowUTex.GetHeight() };
					color = { 1, 1, 1, 1 };
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_EnemyDirectionArrowUTex, 0, 0, (int)m_EnemyDirectionArrowUTex.GetWidth(), (int)m_EnemyDirectionArrowUTex.GetHeight(), &rc, &color, Math::Vector2(0, 0.5f));
				}
				else if (m_heightDifference <= -2.0f)
				{
					mat = Math::Matrix::CreateTranslation(-25, -225.0f, 0.0f);
					KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
					rc = { 0,0,(int)m_EnemyDirectionArrowBTex.GetWidth(),(int)m_EnemyDirectionArrowBTex.GetHeight() };
					color = { 1, 1, 1, 1 };
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_EnemyDirectionArrowBTex, 0, 0, (int)m_EnemyDirectionArrowBTex.GetWidth(), (int)m_EnemyDirectionArrowBTex.GetHeight(), &rc, &color, Math::Vector2(0, 0.5f));
				}


				Math::Vector3 cross = DirectX::XMVector3Cross(nowVec, toVec);

				if (cross.y < 0)
				{
					mat = Math::Matrix::CreateTranslation(-500, 0.0f, 0.0f);
					KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
					rc = { 0,0,(int)m_EnemyDirectionArrowLTex.GetWidth(),(int)m_EnemyDirectionArrowLTex.GetHeight() };
					color = { 1, 1, 1, 1 };
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_EnemyDirectionArrowLTex, 0, 0, (int)m_EnemyDirectionArrowLTex.GetWidth(), (int)m_EnemyDirectionArrowLTex.GetHeight(), &rc, &color, Math::Vector2(0, 0.5f));
				}
				else if (cross.y >= 0)
				{
					mat = Math::Matrix::CreateTranslation(500, 0.0f, 0.0f);
					KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
					rc = { 0,0,(int)m_EnemyDirectionArrowRTex.GetWidth(),(int)m_EnemyDirectionArrowRTex.GetHeight() };
					color = { 1, 1, 1, 1 };
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_EnemyDirectionArrowRTex, 0, 0, (int)m_EnemyDirectionArrowRTex.GetWidth(), (int)m_EnemyDirectionArrowRTex.GetHeight(), &rc, &color, Math::Vector2(0, 0.5f));
				}

			}
			++i;
		}
	}

	if (m_bOption)
	{
		transMat = Math::Matrix::Identity;
		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		color = { 0,0,0,0.2f };
		KdShaderManager::Instance().m_spriteShader.DrawBox(0, 0, 1280, 720, &color);

		if (!m_bOperation)
		{
			mat = Math::Matrix::CreateScale(m_operationScale) * Math::Matrix::CreateTranslation(m_operationPos);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_operationTex, 0, 0, (int)m_operationTex.GetWidth(), (int)m_operationTex.GetHeight());

			mat = Math::Matrix::CreateScale(m_selectScale) * Math::Matrix::CreateTranslation(m_selectPos);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_selectTex, 0, 0, (int)m_selectTex.GetWidth(), (int)m_selectTex.GetHeight());

			mat = Math::Matrix::CreateScale(m_exitScale) * Math::Matrix::CreateTranslation(m_exitPos);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_exitTex, 0, 0, (int)m_exitTex.GetWidth(), (int)m_exitTex.GetHeight());
		}
		else if (m_bOperation)
		{
			if (m_bHowToPage)
			{
				transMat = Math::Matrix::Identity;
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_howToTex, 0, 0, (int)m_howToTex.GetWidth(), (int)m_howToTex.GetHeight());
			}

			if (m_bWeaponDataPage)
			{
				if (m_bWeaponDataHopperPage)
				{
					transMat = Math::Matrix::Identity;
					KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_hopperDataTex, 0, 0, (int)m_hopperDataTex.GetWidth(), (int)m_hopperDataTex.GetHeight());
				}
				else if (m_bWeaponDataScoPage)
				{
					transMat = Math::Matrix::Identity;
					KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponInfoTex, 0, 0, (int)m_weaponInfoTex.GetWidth(), (int)m_weaponInfoTex.GetHeight());
				}

				mat = Math::Matrix::CreateScale(m_weaponRightYaiScale) * Math::Matrix::CreateTranslation(m_weaponRightYaiPos);
				KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_rightYaiTex, 0, 0, (int)m_rightYaiTex.GetWidth(), (int)m_rightYaiTex.GetHeight());

				mat = Math::Matrix::CreateScale(m_weaponLeftYaiScale) * Math::Matrix::CreateTranslation(m_weaponLeftYaiPos);
				KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_leftYaiTex, 0, 0, (int)m_leftYaiTex.GetWidth(), (int)m_leftYaiTex.GetHeight());
			}

			mat = Math::Matrix::CreateScale(m_weaOrHowRightYaiScale) * Math::Matrix::CreateTranslation(m_weaOrHowRightYaiPos);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_rightYaiTex, 0, 0, (int)m_rightYaiTex.GetWidth(), (int)m_rightYaiTex.GetHeight());

			mat = Math::Matrix::CreateScale(m_weaOrHowLeftYaiScale) * Math::Matrix::CreateTranslation(m_weaOrHowLeftYaiPos);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_leftYaiTex, 0, 0, (int)m_leftYaiTex.GetWidth(), (int)m_leftYaiTex.GetHeight());

		}

		mat = Math::Matrix::CreateScale(m_backScale) * Math::Matrix::CreateTranslation(m_backPos);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_backTex, 0, 0, 182, 80);
	}

	mat = Math::Matrix::CreateTranslation(-50, 325, 0);

	KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_timeTex[m_gameTimeM10], 0, 0, (int)m_timeTex[m_gameTimeM10].GetWidth(), (int)m_timeTex[m_gameTimeM10].GetHeight());

	mat = Math::Matrix::CreateTranslation(-20, 325, 0);
	KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_timeTex[m_gameTimeM1], 0, 0, (int)m_timeTex[m_gameTimeM1].GetWidth(), (int)m_timeTex[m_gameTimeM1].GetHeight());

	mat = Math::Matrix::CreateTranslation(0, 325, 0);
	KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_conmaTex, 0, 0, (int)m_conmaTex.GetWidth(), (int)m_conmaTex.GetHeight());

	mat = Math::Matrix::CreateTranslation(20, 325, 0);

	KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_timeTex[m_gameTimeS10], 0, 0, (int)m_timeTex[m_gameTimeS10].GetWidth(), (int)m_timeTex[m_gameTimeS10].GetHeight());

	mat = Math::Matrix::CreateTranslation(50, 325, 0);
	KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_timeTex[m_gameTimeS1], 0, 0, (int)m_timeTex[m_gameTimeS1].GetWidth(), (int)m_timeTex[m_gameTimeS1].GetHeight());

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
	PWINDOWINFO pwi = new WINDOWINFO;
	pwi->cbSize = sizeof(WINDOWINFO);
	GetWindowInfo(Application::Instance().GetWindowHandle(), pwi);

	POINT mousePos;
	GetCursorPos(&mousePos);

	mousePos.x -= 640;
	mousePos.y = mousePos.y * -1 + 360;
	Math::Vector3 Dis;
	float mouseX = (float)mousePos.x/* + (float)(pwi->rcWindow.left)*/;
	float mouseY = (float)mousePos.y + (float)(pwi->rcWindow.top + 35);

	float MouseLeft = mouseX - 2.0f;
	float MouseRight = mouseX + 2.0f;
	float MouseTop = mouseY + 2.0f;
	float MouseBottom = mouseY - 2.0f;

	if (!m_bOperation)
	{
		Math::Vector3 ExitPos;
		ExitPos.x = m_exitPos.x + (float)(pwi->rcWindow.left);
		ExitPos.y = m_exitPos.y /*+ (float)(pwi->rcWindow.top)*/;
		ExitPos.z = m_exitPos.z;

		float ExitLeft = ExitPos.x - 350;
		float ExitRight = ExitPos.x + 350;
		float ExitTop = ExitPos.y + 60;
		float ExitBottom = ExitPos.y - 60;

		if (MouseRight > ExitLeft && ExitRight > MouseLeft &&
			MouseTop > ExitBottom && ExitTop > MouseBottom)
		{
			m_exitScale = 1.2f;

			if (KdInputManager::Instance().IsPress("select"))
			{
				if (!m_addFadeAlpha)
				{
					m_bExit = true;
					m_addFadeAlpha = true;
					m_bFirstExit = true;
					KdAudioManager::Instance().Play("Asset/Audio/SE/PushButton.wav");
				}
			}

		}
		else
		{
			m_exitScale = 1.0f;
		}

		Math::Vector3 ModeSelectPos;
		ModeSelectPos.x = m_selectPos.x + (float)(pwi->rcWindow.left);
		ModeSelectPos.y = m_selectPos.y /*+ (float)(pwi->rcWindow.top)*/;
		ModeSelectPos.z = m_selectPos.z;

		float SelectLeft = ModeSelectPos.x - 350;
		float SelectRight = ModeSelectPos.x + 350;
		float SelectTop = ModeSelectPos.y + 60;
		float SelectBottom = ModeSelectPos.y - 60;

		if (MouseRight > SelectLeft && SelectRight > MouseLeft &&
			MouseTop > SelectBottom && SelectTop > MouseBottom)
		{
			m_exitScale = 1.0f;
			m_selectScale = 1.2f;

			if (KdInputManager::Instance().IsPress("select"))
			{
				if (!m_addFadeAlpha)
				{
					m_bSelect = true;
					m_addFadeAlpha = true;
					//KdEffekseerManager::GetInstance().OnResumeEfkUpdate();
					KdAudioManager::Instance().Play("Asset/Audio/SE/PushButton.wav");
				}
			}

		}
		else
		{
			m_selectScale = 1.0f;
		}

		Math::Vector3 InfoPos;
		InfoPos.x = m_operationPos.x + (float)(pwi->rcWindow.left);
		InfoPos.y = m_operationPos.y /*+ (float)(pwi->rcWindow.top)*/;
		InfoPos.z = m_operationPos.z;

		float InfoLeft = InfoPos.x - 350;
		float InfoRight = InfoPos.x + 350;
		float InfoTop = InfoPos.y + 60;
		float InfoBottom = InfoPos.y - 60;

		if (MouseRight > InfoLeft && InfoRight > MouseLeft &&
			MouseTop > InfoBottom && InfoTop > MouseBottom)
		{
			m_exitScale = 1.0f;
			m_operationScale = 1.2f;

			if (KdInputManager::Instance().IsPress("select"))
			{
				m_bOperation = true;
				KdAudioManager::Instance().Play("Asset/Audio/SE/PushButton.wav");
			}

		}
		else
		{
			m_operationScale = 1.0f;
		}

		Math::Vector3 BackPos;
		BackPos.x = m_backPos.x + (float)(pwi->rcWindow.left);
		BackPos.y = m_backPos.y /*+ (float)(pwi->rcWindow.top)*/;
		BackPos.z = m_backPos.z;
		Dis = BackPos - Math::Vector3(mouseX, mouseY, 0.0f);
		if (Dis.Length() <= 40)
		{
			m_backScale = 1.0f;
			if (KdInputManager::Instance().IsPress("select"))
			{
				m_bOption = false;
				ShowCursor(false); // マウスカーソルを消す
				SetCursorPos(640, 360);
				KdAudioManager::Instance().Play("Asset/Audio/SE/backPush.wav");
				KdEffekseerManager::GetInstance().OnResumeEfkUpdate();
			}
		}
		else
		{
			m_backScale = 0.8f;
		}

		if (m_addFadeAlpha)
		{
			m_fadeAlpha += (1.0f / 90);
			if (m_fadeAlpha >= 1.0f)
			{
				m_fadeAlpha = 1.0f;

				if (m_bSelect)
				{
					ShowCursor(false); // マウスカーソルを消す
					KdEffekseerManager::GetInstance().Reset();
					SceneManager::Instance().SetNextScene
					(
						SceneManager::SceneType::select
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
			}
		}

	}
	else
	{

		Math::Vector3 BackPos;
		BackPos.x = m_backPos.x + (float)(pwi->rcWindow.left);
		BackPos.y = m_backPos.y /*+ (float)(pwi->rcWindow.top)*/;
		BackPos.z = m_backPos.z;
		Dis = BackPos - Math::Vector3(mouseX, mouseY, 0.0f);
		if (Dis.Length() <= 40)
		{
			m_backScale = 1.0f;
			if (KdInputManager::Instance().IsPress("select"))
			{
				m_bWeaponDataPage = false;
				m_bWeaponDataHopperPage = false;
				m_bWeaponDataScoPage = true;
				m_bHowToPage = true;
				m_bOperation = false;
				KdAudioManager::Instance().Play("Asset/Audio/SE/backPush.wav");
			}
		}
		else
		{
			m_backScale = 0.8f;
		}

		Math::Vector3 WeaOrHowLeftYaiPos;
		WeaOrHowLeftYaiPos.x = m_weaOrHowLeftYaiPos.x + (float)(pwi->rcWindow.left);
		WeaOrHowLeftYaiPos.y = m_weaOrHowLeftYaiPos.y /*+ (float)(pwi->rcWindow.top)*/;
		WeaOrHowLeftYaiPos.z = m_weaOrHowLeftYaiPos.z;
		Dis = WeaOrHowLeftYaiPos - Math::Vector3(mouseX, mouseY, 0.0f);
		if (Dis.Length() <= 30)
		{
			m_weaOrHowLeftYaiScale = 1.5f;
			m_weaOrHowRightYaiScale = 1.0f;

			if (KdInputManager::Instance().IsPress("select"))
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
		}
		else
		{
			m_weaOrHowLeftYaiScale = 1.0f;
		}

		Math::Vector3 WeaOrHowRightYaiPos;
		WeaOrHowRightYaiPos.x = m_weaOrHowRightYaiPos.x + (float)(pwi->rcWindow.left);
		WeaOrHowRightYaiPos.y = m_weaOrHowRightYaiPos.y /*+ (float)(pwi->rcWindow.top)*/;
		WeaOrHowRightYaiPos.z = m_weaOrHowRightYaiPos.z;
		Dis = m_weaOrHowRightYaiPos - Math::Vector3(mouseX, mouseY, 0.0f);
		if (Dis.Length() <= 30)
		{
			m_weaOrHowLeftYaiScale = 1.0f;
			m_weaOrHowRightYaiScale = 1.5f;

			if (KdInputManager::Instance().IsPress("select"))
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
		}
		else
		{
			m_weaOrHowRightYaiScale = 1.0f;
		}

		if (m_bWeaponDataPage)
		{
			Math::Vector3 WeaponLeftYaiPos;
			WeaponLeftYaiPos.x = m_weaponLeftYaiPos.x + (float)(pwi->rcWindow.left);
			WeaponLeftYaiPos.y = m_weaponLeftYaiPos.y /*+ (float)(pwi->rcWindow.top)*/;
			WeaponLeftYaiPos.z = m_weaponLeftYaiPos.z;

			Dis = WeaponLeftYaiPos - Math::Vector3(mouseX, mouseY, 0.0f);
			if (Dis.Length() <= 30)
			{
				m_weaponLeftYaiScale = 1.5f;
				m_weaponRightYaiScale = 1.0f;

				if (KdInputManager::Instance().IsPress("select"))
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
			else
			{
				m_weaponLeftYaiScale = 1.0f;
			}

			Math::Vector3 WeaponRightYaiPos;
			WeaponRightYaiPos.x = m_weaponRightYaiPos.x + (float)(pwi->rcWindow.left);
			WeaponRightYaiPos.y = m_weaponRightYaiPos.y /*+ (float)(pwi->rcWindow.top)*/;
			WeaponRightYaiPos.z = m_weaponRightYaiPos.z;

			Dis = WeaponRightYaiPos - Math::Vector3(mouseX, mouseY, 0.0f);
			if (Dis.Length() <= 30)
			{
				m_weaponLeftYaiScale = 1.0f;
				m_weaponRightYaiScale = 1.5f;

				if (KdInputManager::Instance().IsPress("select"))
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
			else
			{
				m_weaponRightYaiScale = 1.0f;
			}
		}
	}
}
// ＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝

// リザルトのUI関連の処理＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
void ResultUi::Init()
{
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
	m_loseCharaTex.Load("Asset/Textures/Ui/Result/loseChara.png");

	m_time = 0;
	m_bSceneCangePossible = false;
	m_pushLClickAlpha = 0.0f;
	m_bPushLClickAlphaAdd = false;
}

void ResultUi::Update()
{
	m_time++;
	if (m_time >= 240)
	{
		m_time = 240;
	}

	if (m_time >= 210)
	{
		if (SceneManager::Instance().GetBAddOrSubVal())
		{
			if (m_pointAddOrSubVal > 0)
			{
				if (m_time == 210)
				{
					KdAudioManager::Instance().Play("Asset/Audio/SE/AddAndSub.wav", true);
				}

				m_pointAddOrSubVal--;
				m_weaponPoint++;
				if (m_weaponPoint >= 99999)
				{
					m_weaponPoint = 99999;
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
				if (m_weaponPoint < 4000)
				{
					ofs << "point" << ',' << m_weaponPoint << ',' << '\n' << "rank" << ',' << "C" << ',' << '\n';
				}
				else if (m_weaponPoint >= 4000 && m_weaponPoint < 8000)
				{
					ofs << "point" << ',' << m_weaponPoint << ',' << '\n' << "rank" << ',' << "B" << ',' << '\n';
				}
				else if (m_weaponPoint >= 8000 && m_weaponPoint < 1200)
				{
					ofs << "point" << ',' << m_weaponPoint << ',' << '\n' << "rank" << ',' << "A" << ',' << '\n' << "Skill" << ',' << "マスター" << '\n';
				}
				else
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
				if (m_time == 210)
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
				if (m_weaponPoint < 4000)
				{
					ofs << "point" << ',' << m_weaponPoint << ',' << '\n' << "rank" << ',' << "C" << ',' << '\n';
				}
				else if (m_weaponPoint >= 4000 && m_weaponPoint < 8000)
				{
					ofs << "point" << ',' << m_weaponPoint << ',' << '\n' << "rank" << ',' << "B" << ',' << '\n';
				}
				else if (m_weaponPoint >= 8000 && m_weaponPoint < 1200)
				{
					ofs << "point" << ',' << m_weaponPoint << ',' << '\n' << "rank" << ',' << "A" << ',' << '\n' << "Skill" << ',' << "マスター" << '\n';
				}
				else
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
					if (m_weaponPoint >= 99999)
					{
						m_weaponPoint = 99999;
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
			m_pushLClickAlpha += (1.0f / 30);
			if (m_pushLClickAlpha >= 1.0f)
			{
				m_pushLClickAlpha = 1.0f;
				m_bPushLClickAlphaAdd = false;
			}
		}
		else
		{
			m_pushLClickAlpha -= (1.0f / 30);
			if (m_pushLClickAlpha <= 0.0f)
			{
				m_pushLClickAlpha = 0.0f;
				m_bPushLClickAlphaAdd = true;
			}
		}
	}

	if (m_addFadeAlpha)
	{
		m_fadeAlpha += (1.0f / 90);
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
		color = { 0.15f,0.15f,0.7f,1.0f };
		KdShaderManager::Instance().m_spriteShader.DrawBox(0, 0, 1280, 720, &color);

	}
	else
	{
		transMat = Math::Matrix::Identity;
		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		color = { 0.4f,0.1f,0.1f,1.0f };
		KdShaderManager::Instance().m_spriteShader.DrawBox(0, 0, 1280, 720, &color);
	}

	if (m_time >= 60)
	{
		int ieftover = 0;
		int ieftoverExcess = 0;

		if (SceneManager::Instance().GetBPlayerWin())
		{
			transMat = Math::Matrix::CreateTranslation(0, 225, 0);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_winTex, 0, 0, 246, 114);

			transMat = Math::Matrix::CreateTranslation(425, 30, 0);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_winPlayerCharaTex, 0, 0, 246, 461);

			switch (SceneManager::Instance().GetEnemyTotal())
			{
			case 1:
				transMat = Math::Matrix::CreateTranslation(-400, 0, 0);
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, 313, 165);
				break;
			case 2:
				switch (SceneManager::Instance().GetEnemyIeftover())
				{
				case 0:
					if (m_time >= 80)
					{
						transMat = Math::Matrix::CreateTranslation(-400, 100, 0);
						KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
						KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, 313, 165);
					}

					if (m_time >= 100)
					{
						transMat = Math::Matrix::CreateTranslation(-400, -100, 0);
						KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
						KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, 313, 165);
					}
					break;
				case 1:
					transMat = Math::Matrix::CreateTranslation(-400, 0, 0);
					KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, 313, 165);
					break;
				}
				break;
			case 3:
				switch (SceneManager::Instance().GetEnemyIeftover())
				{
				case 0:
					if (m_time >= 70)
					{
						transMat = Math::Matrix::CreateTranslation(-400, 200, 0);
						KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
						KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, 313, 165);
					}

					if (m_time >= 90)
					{
						transMat = Math::Matrix::CreateTranslation(-400, 0, 0);
						KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
						KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, 313, 165);
					}

					if (m_time >= 110)
					{
						transMat = Math::Matrix::CreateTranslation(-400, -200, 0);
						KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
						KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, 313, 165);
					}
					break;
				case 1:
					if (m_time >= 80)
					{
						transMat = Math::Matrix::CreateTranslation(-400, 100, 0);
						KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
						KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, 313, 165);
					}

					if (m_time >= 100)
					{
						transMat = Math::Matrix::CreateTranslation(-400, -100, 0);
						KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
						KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, 313, 165);
					}
					break;
				case 2:
					transMat = Math::Matrix::CreateTranslation(-400, 0, 0);
					KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, 313, 165);
					break;
				}
				break;
			case 50:
				ieftover = (50 - SceneManager::Instance().GetEnemyIeftover()) / 5;
				ieftoverExcess = (50 - SceneManager::Instance().GetEnemyIeftover()) % 5;

				for (int i = 0; i < ieftover; i++)
				{
					for (int j = 0; j < 5; j++)
					{
						transMat = Math::Matrix::CreateScale(0.2f) * Math::Matrix::CreateTranslation((float)(-600 + 80 * j), (float)(320 - 70 * i), 0);
						KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
						KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, 313, 165);
					}
				}

				for (int k = 0; k < ieftoverExcess; k++)
				{
					transMat = Math::Matrix::CreateScale(0.2f) * Math::Matrix::CreateTranslation((float)(-600 + 80 * k), (float)(320 - 70 * ieftover), 0);
					KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, 313, 165);
				}

				break;
			case 100:
				ieftover = (100 - SceneManager::Instance().GetEnemyIeftover()) / 5;
				ieftoverExcess = (100 - SceneManager::Instance().GetEnemyIeftover()) % 5;

				for (int i = 0; i < ieftover; i++)
				{
					for (int j = 0; j < 5; j++)
					{
						transMat = Math::Matrix::CreateScale(0.075f) * Math::Matrix::CreateTranslation((float)(-600 + 80 * j), (float)(320 - 35 * i), 0);
						KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
						KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, 313, 165);
					}
				}

				for (int k = 0; k < ieftoverExcess; k++)
				{
					transMat = Math::Matrix::CreateScale(0.075f) * Math::Matrix::CreateTranslation((float)(-600 + 80 * k), (float)(320 - 35 * ieftover), 0);
					KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, 313, 165);
				}

				break;
			}

		}
		else
		{
			transMat = Math::Matrix::CreateTranslation(0, 225, 0);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_lossTex, 0, 0, 512, 172);

			transMat = Math::Matrix::CreateTranslation(400, 0, 0);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, 313, 165);

			switch (SceneManager::Instance().GetEnemyTotal())
			{
			case 1:
				transMat = Math::Matrix::CreateTranslation(-425, 30, 0);
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_winCharaTex, 0, 0, 246, 461);
				break;
			case 2:
				if (m_time >= 80)
				{
					transMat = Math::Matrix::CreateTranslation(-405, 30, 0);
					KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_winCharaTex, 0, 0, 246, 461);
				}

				if (m_time >= 100)
				{
					transMat = Math::Matrix::CreateTranslation(-485, 30, 0);
					KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_winCharaTex, 0, 0, 246, 461);
				}
				break;
			case 3:
				if (m_time >= 70)
				{
					transMat = Math::Matrix::CreateTranslation(-340, 30, 0);
					KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_winCharaTex, 0, 0, 246, 461);
				}

				if (m_time >= 90)
				{
					transMat = Math::Matrix::CreateTranslation(-485, 30, 0);
					KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_winCharaTex, 0, 0, 246, 461);
				}

				if (m_time >= 110)
				{
					transMat = Math::Matrix::CreateTranslation(-420, 30, 0);
					KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_winCharaTex, 0, 0, 246, 461);
				}

				break;
			case 50:
				ieftover = (50 - SceneManager::Instance().GetEnemyIeftover()) / 5;
				ieftoverExcess = (50 - SceneManager::Instance().GetEnemyIeftover()) % 5;

				for (int i = 0; i < ieftover; i++)
				{
					for (int j = 0; j < 5; j++)
					{
						transMat = Math::Matrix::CreateScale(0.2f) * Math::Matrix::CreateTranslation((float)(-600 + 80 * j), (float)(320 - 70 * i), 0);
						KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
						KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, 313, 165);
					}
				}

				for (int k = 0; k < ieftoverExcess; k++)
				{
					transMat = Math::Matrix::CreateScale(0.2f) * Math::Matrix::CreateTranslation((float)(-600 + 80 * k), (float)(320 - 70 * ieftover), 0);
					KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, 313, 165);
				}

				break;
			case 100:
				ieftover = (50 - SceneManager::Instance().GetEnemyIeftover()) / 5;
				ieftoverExcess = (50 - SceneManager::Instance().GetEnemyIeftover()) % 5;

				for (int i = 0; i < ieftover; i++)
				{
					for (int j = 0; j < 5; j++)
					{
						transMat = Math::Matrix::CreateScale(0.075f) * Math::Matrix::CreateTranslation((float)(-600 + 80 * j), (float)(320 - 35 * i), 0);
						KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
						KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, 313, 165);
					}
				}

				for (int k = 0; k < ieftoverExcess; k++)
				{
					transMat = Math::Matrix::CreateScale(0.075f) * Math::Matrix::CreateTranslation((float)(-600 + 80 * k), (float)(320 - 35 * ieftover), 0);
					KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, 313, 165);
				}

				break;
			}
		}
	}

	if (m_time >= 120)
	{
		float xVal = 125;
		for (int i = 1; i <= 1000; i *= 10)
		{
			transMat = Math::Matrix::CreateTranslation(xVal, 45, 0);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_PointTex[(m_pointAddOrSubVal / i) % 10], 0, 0, 102, 114);
			xVal -= 80;
		}
	}

	if (m_time >= 180)
	{
		float xVal = 125;
		for (int i = 1; i <= 10000; i *= 10)
		{
			transMat = Math::Matrix::CreateTranslation(xVal, -80, 0);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_PointTex[(m_weaponPoint / i) % 10], 0, 0, 102, 114);
			xVal -= 80;
		}
	}

	if (m_pointAddOrSubVal == 0)
	{
		transMat = Math::Matrix::CreateTranslation(0, -250, 0);
		Math::Rectangle rc = { 0,0,894,114 };
		color = { 1,1,1,m_pushLClickAlpha };
		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_pushLClickTex, 0, 0, 894, 114, &rc, &color);
	}

	transMat = Math::Matrix::Identity;
	KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
	color = { 0,0,0,m_fadeAlpha };
	KdShaderManager::Instance().m_spriteShader.DrawBox(0, 0, 1280, 720, &color);
}
// ＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝

// モードセレクトのUI関連の処理＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
void ModeSelectUi::Init()
{
	m_modeSelectTex.Load("Asset/Textures/Ui/Select/ModeSelect.png");
	m_pictureFrameTex.Load("Asset/Textures/Ui/Select/PictureFrame.png");

	m_selectTitleTex.Load("Asset/Textures/Ui/Select/title.png");
	m_selectExitTex.Load("Asset/Textures/Ui/Select/exit.png");
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
	//m_chalengehelpTex.Load("Asset/Textures/Ui/Select/threeEnemy.png");

	m_chalenge50Tex.Load("Asset/Textures/Ui/Select/Challenge50.png");
	m_chalenge100Tex.Load("Asset/Textures/Ui/Select/Challenge100.png");

	m_addFadeAlpha = false;
	m_time = 0;
	m_fadeAlpha = 0.0f;

	m_tutorialScale = 1.0f;
	m_gameScale = 1.0f;
	m_challengeScale = 0;
	m_trainingScale = 0;
	m_pictureFramePos = { 265, 0, 0 };
	m_gamePos = { 265,0, 0 };
	m_battleCharaPos = { -350, 225, 0 };
	m_enemyBossTypeOnePos = { 265,  -80, 0 };
	m_enemyBossTypeTwoPos = { 265, -170, 0 };
	m_twoEnemyBossTypeOnePos = { 265, -260, 0 };
	m_helpMkPos = { 485,  305, 0 };
	m_clauseMkPos = { 415,  230, 0 };

	m_challengePos = { 265,    0, 0 };
	m_challengeCharaPos = { -350,   75, 0 };
	m_chalenge50Pos = { 265, -180, 0 };
	m_chalenge100Pos = { 265, -270, 0 };
	m_tutorialPos = { -488, -138, 0 };
	m_trainingPos = { -205, -138, 0 };
	m_selectBackPos = { 0,    0, 0 };

	m_battleCharaScale = 1.0f;
	m_challengeCharaScale = 1.0f;
	m_chalenge50Scale = 1.0f;
	m_chalenge100Scale = 1.0f;
	m_titleScale = 1.0f;
	m_exitScale = 1.0f;

	m_bExit = false;
	m_bOption = false;
	m_bTitle = false;
	m_bFirstExit = true;

	m_titlePos = { -555,-310,0 };
	m_exitPos = { -420,-310,0 };

	m_bChalenge50 = false;
	m_bChalenge100 = false;
	m_bTutorial = false;

	m_clauseMkScale = 1;

	m_helpMkScale = 1.0f;
	m_bBattlehelp = false;

	m_bChalengehelp = false;

	m_bGame = false;
	m_bEnemyBossTypeOneTotal = false;
	m_bEnemyBossTypeTwoTotal = false;
	m_bTwoEnemyBossTypeOneTotal = false;
	m_bBattleChara = false;
	m_bChallengeChara = false;

	m_bBattleSelect = false;
	m_bChallengeSelect = false;
}

void ModeSelectUi::Update()
{
	PWINDOWINFO pwi = new WINDOWINFO;
	pwi->cbSize = sizeof(WINDOWINFO);
	GetWindowInfo(Application::Instance().GetWindowHandle(), pwi);

	POINT mousePos;
	GetCursorPos(&mousePos);


	if (m_bBattlehelp || m_bChalengehelp)
	{
		mousePos.x -= 640;
		mousePos.y = mousePos.y * -1 + 360;
		Math::Vector3 Dis;
		float mouseX = (float)mousePos.x/* + (float)(pwi->rcWindow.left)*/;
		float mouseY = (float)mousePos.y + (float)(pwi->rcWindow.top + 35);

		Math::Vector3 ClauseMkPos;
		ClauseMkPos.x = m_clauseMkPos.x + (float)(pwi->rcWindow.left);
		ClauseMkPos.y = m_clauseMkPos.y /*+ (float)(pwi->rcWindow.top)*/;
		ClauseMkPos.z = m_clauseMkPos.z;

		Dis = ClauseMkPos - Math::Vector3(mouseX, mouseY, 0.0f);

		if (Dis.Length() < 25)
		{
			m_clauseMkScale = 1.1f;

			if (KdInputManager::Instance().IsPress("select"))
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
			m_clauseMkScale = 1.0f;
		}
	}
	else
	{


		mousePos.x -= 640;
		mousePos.y = mousePos.y * -1 + 360;
		Math::Vector3 Dis;
		float mouseX = (float)mousePos.x/* + (float)(pwi->rcWindow.left)*/;
		float mouseY = (float)mousePos.y + (float)(pwi->rcWindow.top + 35);

		Math::Vector3 TutrorialPos;
		TutrorialPos.x = m_tutorialPos.x + (float)(pwi->rcWindow.left);
		TutrorialPos.y = m_tutorialPos.y /*+ (float)(pwi->rcWindow.top)*/;
		TutrorialPos.z = m_tutorialPos.z;

		float MouseLeft = mouseX - 2.0f;
		float MouseRight = mouseX + 2.0f;
		float MouseTop = mouseY + 2.0f;
		float MouseBottom = mouseY - 2.0f;

		float TutorialLeft = TutrorialPos.x - 117.5f;
		float TutorialRight = TutrorialPos.x + 117.5f;
		float TutorialTop = TutrorialPos.y + 132.5f;
		float TutorialBottom = TutrorialPos.y - 132.5f;

		if (MouseRight > TutorialLeft && TutorialRight > MouseLeft &&
			MouseTop > TutorialBottom && TutorialTop > MouseBottom)
		{
			m_tutorialScale = 1.1f;

			if (KdInputManager::Instance().IsPress("select"))
			{
				if (!m_addFadeAlpha)
				{
					m_bTutorial = true;
					m_addFadeAlpha = true;
					KdAudioManager::Instance().Play("Asset/Audio/SE/PushButton.wav");
				}
			}

		}
		else
		{
			m_tutorialScale = 1.0f;
		}

		Math::Vector3 ExitPos;
		ExitPos.x = m_exitPos.x + (float)(pwi->rcWindow.left);
		ExitPos.y = m_exitPos.y /*+ (float)(pwi->rcWindow.top)*/;
		ExitPos.z = m_exitPos.z;

		float ExitLeft = ExitPos.x - 50;
		float ExitRight = ExitPos.x + 50;
		float ExitTop = ExitPos.y + 25;
		float ExitBottom = ExitPos.y - 25;

		if (MouseRight > ExitLeft && ExitRight > MouseLeft &&
			MouseTop > ExitBottom && ExitTop > MouseBottom)
		{
			m_exitScale = 1.1f;

			if (KdInputManager::Instance().IsPress("select"))
			{
				if (!m_addFadeAlpha)
				{
					m_bExit = true;
					m_addFadeAlpha = true;
					m_bFirstExit = true;
					KdAudioManager::Instance().Play("Asset/Audio/SE/PushButton.wav");
				}
			}

		}
		else
		{
			m_exitScale = 1.0f;
		}

		Math::Vector3 GamePos;
		GamePos.x = m_battleCharaPos.x + (float)(pwi->rcWindow.left);
		GamePos.y = m_battleCharaPos.y /*+ (float)(pwi->rcWindow.top)*/;
		GamePos.z = m_battleCharaPos.z;

		float GameLeft = GamePos.x - 260;
		float GameRight = GamePos.x + 260;
		float GameTop = GamePos.y + 65;
		float GameBottom = GamePos.y - 65;

		if (MouseRight > GameLeft && GameRight > MouseLeft &&
			MouseTop > GameBottom && GameTop > MouseBottom)
		{
			m_battleCharaScale = 1.1f;

			if (KdInputManager::Instance().IsPress("select"))
			{
				if (!m_addFadeAlpha && !m_bBattleSelect)
				{
					m_bBattleChara = !m_bBattleChara;

					if (m_bChallengeChara)
					{
						m_bChallengeChara = false;
					}

					m_bBattleSelect = true;
					KdAudioManager::Instance().Play("Asset/Audio/SE/PushButton.wav");
				}
			}
			else
			{
				m_bBattleSelect = false;
			}

		}
		else
		{
			m_battleCharaScale = 1.0f;
		}

		if (m_bBattleChara)
		{
			GamePos.x = m_enemyBossTypeOnePos.x + (float)(pwi->rcWindow.left);
			GamePos.y = m_enemyBossTypeOnePos.y /*+ (float)(pwi->rcWindow.top)*/;
			GamePos.z = m_enemyBossTypeOnePos.z;

			GameLeft = GamePos.x - 105;
			GameRight = GamePos.x + 105;
			GameTop = GamePos.y + 40;
			GameBottom = GamePos.y - 40;

			if (MouseRight > GameLeft && GameRight > MouseLeft &&
				MouseTop > GameBottom && GameTop > MouseBottom)
			{
				m_enemyBossTypeOneScale = 1.1f;

				if (KdInputManager::Instance().IsPress("select"))
				{
					if (!m_addFadeAlpha)
					{
						m_bEnemyBossTypeOneTotal = true;
						m_bGame = true;
						m_addFadeAlpha = true;
						KdAudioManager::Instance().Play("Asset/Audio/SE/PushButton.wav");
					}
				}

			}
			else
			{
				m_enemyBossTypeOneScale = 1.0f;
			}

			GamePos.x = m_enemyBossTypeTwoPos.x + (float)(pwi->rcWindow.left);
			GamePos.y = m_enemyBossTypeTwoPos.y /*+ (float)(pwi->rcWindow.top)*/;
			GamePos.z = m_enemyBossTypeTwoPos.z;

			GameLeft = GamePos.x - 105;
			GameRight = GamePos.x + 105;
			GameTop = GamePos.y + 40;
			GameBottom = GamePos.y - 40;

			if (MouseRight > GameLeft && GameRight > MouseLeft &&
				MouseTop > GameBottom && GameTop > MouseBottom)
			{
				m_enemyBossTypeTwoScale = 1.1f;

				if (KdInputManager::Instance().IsPress("select"))
				{
					if (!m_addFadeAlpha)
					{
						m_bEnemyBossTypeTwoTotal = true;
						m_bGame = true;
						m_addFadeAlpha = true;
						KdAudioManager::Instance().Play("Asset/Audio/SE/PushButton.wav");
					}
				}

			}
			else
			{
				m_enemyBossTypeTwoScale = 1.0f;
			}

			GamePos.x = m_twoEnemyBossTypeOnePos.x + (float)(pwi->rcWindow.left);
			GamePos.y = m_twoEnemyBossTypeOnePos.y /*+ (float)(pwi->rcWindow.top)*/;
			GamePos.z = m_twoEnemyBossTypeOnePos.z;

			GameLeft = GamePos.x - 105;
			GameRight = GamePos.x + 105;
			GameTop = GamePos.y + 40;
			GameBottom = GamePos.y - 40;

			if (MouseRight > GameLeft && GameRight > MouseLeft &&
				MouseTop > GameBottom && GameTop > MouseBottom)
			{
				m_twoEnemyBossTypeOneScale = 1.1f;

				if (KdInputManager::Instance().IsPress("select"))
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
			else
			{
				m_twoEnemyBossTypeOneScale = 1.0f;
			}
		}

		Math::Vector3 ChallengePos;
		ChallengePos.x = m_challengeCharaPos.x + (float)(pwi->rcWindow.left);
		ChallengePos.y = m_challengeCharaPos.y /*+ (float)(pwi->rcWindow.top)*/;
		ChallengePos.z = m_challengeCharaPos.z;

		float ChallengeLeft = ChallengePos.x - 260;
		float ChallengeRight = ChallengePos.x + 260;
		float ChallengeTop = ChallengePos.y + 65;
		float ChallengeBottom = ChallengePos.y - 65;

		if (MouseRight > ChallengeLeft && ChallengeRight > MouseLeft &&
			MouseTop > ChallengeBottom && ChallengeTop > MouseBottom)
		{
			m_challengeCharaScale = 1.1f;

			if (KdInputManager::Instance().IsPress("select"))
			{
				if (!m_addFadeAlpha && !m_bChallengeSelect)
				{
					m_bChallengeChara = !m_bChallengeChara;

					if (m_bBattleChara)
					{
						m_bBattleChara = false;
					}

					m_bChallengeSelect = true;
					KdAudioManager::Instance().Play("Asset/Audio/SE/PushButton.wav");
				}
			}
			else
			{
				m_bChallengeSelect = false;
			}

		}
		else
		{
			m_challengeCharaScale = 1.0f;
		}

		if (m_bChallengeChara)
		{
			ChallengePos.x = m_chalenge50Pos.x + (float)(pwi->rcWindow.left);
			ChallengePos.y = m_chalenge50Pos.y /*+ (float)(pwi->rcWindow.top)*/;
			ChallengePos.z = m_chalenge50Pos.z;

			ChallengeLeft = ChallengePos.x - 105;
			ChallengeRight = ChallengePos.x + 105;
			ChallengeTop = ChallengePos.y + 40;
			ChallengeBottom = ChallengePos.y - 40;

			if (MouseRight > ChallengeLeft && ChallengeRight > MouseLeft &&
				MouseTop > ChallengeBottom && ChallengeTop > MouseBottom)
			{
				m_chalenge50Scale = 1.1f;

				if (KdInputManager::Instance().IsPress("select"))
				{
					if (!m_addFadeAlpha)
					{
						m_bChalenge50 = true;
						m_addFadeAlpha = true;
						KdAudioManager::Instance().Play("Asset/Audio/SE/PushButton.wav");
					}
				}

			}
			else
			{
				m_chalenge50Scale = 1.0f;
			}

			ChallengePos;
			ChallengePos.x = m_chalenge100Pos.x + (float)(pwi->rcWindow.left);
			ChallengePos.y = m_chalenge100Pos.y /*+ (float)(pwi->rcWindow.top)*/;
			ChallengePos.z = m_chalenge100Pos.z;

			ChallengeLeft = ChallengePos.x - 105;
			ChallengeRight = ChallengePos.x + 105;
			ChallengeTop = ChallengePos.y + 40;
			ChallengeBottom = ChallengePos.y - 40;

			if (MouseRight > ChallengeLeft && ChallengeRight > MouseLeft &&
				MouseTop > ChallengeBottom && ChallengeTop > MouseBottom)
			{
				m_chalenge100Scale = 1.1f;

				if (KdInputManager::Instance().IsPress("select"))
				{
					if (!m_addFadeAlpha)
					{
						m_bChalenge100 = true;
						m_addFadeAlpha = true;
						KdAudioManager::Instance().Play("Asset/Audio/SE/PushButton.wav");
					}
				}

			}
			else
			{
				m_chalenge100Scale = 1.0f;
			}
		}

		Math::Vector3 TrainingPos;
		TrainingPos.x = m_trainingPos.x + (float)(pwi->rcWindow.left);
		TrainingPos.y = m_trainingPos.y /*+ (float)(pwi->rcWindow.top)*/;
		TrainingPos.z = m_trainingPos.z;

		float TrainingLeft = TrainingPos.x - 117.5f;
		float TrainingRight = TrainingPos.x + 117.5f;
		float TrainingTop = TrainingPos.y + 132.5f;
		float TrainingBottom = TrainingPos.y - 132.5f;

		if (MouseRight > TrainingLeft && TrainingRight > MouseLeft &&
			MouseTop > TrainingBottom && TrainingTop > MouseBottom)
		{
			m_trainingScale = 1.1f;

			if (KdInputManager::Instance().IsPress("select"))
			{
				if (!m_addFadeAlpha)
				{
					m_bTraining = true;
					m_addFadeAlpha = true;
					KdAudioManager::Instance().Play("Asset/Audio/SE/PushButton.wav");
				}
			}

		}
		else
		{
			m_trainingScale = 1.0f;
		}

		Math::Vector3 TitlePos;
		TitlePos.x = m_titlePos.x + (float)(pwi->rcWindow.left);
		TitlePos.y = m_titlePos.y /*+ (float)(pwi->rcWindow.top)*/;
		TitlePos.z = m_titlePos.z;

		float TitleLeft = TitlePos.x - 50;
		float TitleRight = TitlePos.x + 50;
		float TitleTop = TitlePos.y + 25;
		float TitleBottom = TitlePos.y - 25;

		if (MouseRight > TitleLeft && TitleRight > MouseLeft &&
			MouseTop > TitleBottom && TitleTop > MouseBottom)
		{
			m_titleScale = 1.1f;

			if (KdInputManager::Instance().IsPress("select"))
			{
				if (!m_addFadeAlpha)
				{
					m_bTitle = true;
					m_addFadeAlpha = true;
					KdAudioManager::Instance().Play("Asset/Audio/SE/PushButton.wav");
				}
			}

		}
		else
		{
			m_titleScale = 1.0f;
		}

		Math::Vector3 helpMkPos;
		helpMkPos.x = m_helpMkPos.x + (float)(pwi->rcWindow.left);
		helpMkPos.y = m_helpMkPos.y /*+ (float)(pwi->rcWindow.top)*/;
		helpMkPos.z = m_helpMkPos.z;

		Dis = helpMkPos - Math::Vector3(mouseX, mouseY, 0.0f);

		if (Dis.Length() < 25)
		{
			m_helpMkScale = 1.1f;

			if (KdInputManager::Instance().IsPress("select"))
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
		else
		{
			m_helpMkScale = 1.0f;
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

			if (m_bChalenge50)
			{
				SceneManager::Instance().SetEnemyTotal(50);
				SceneManager::Instance().SetEnemyIeftover(50);

				SceneManager::Instance().SetNextScene
				(
					SceneManager::SceneType::challenge
				);
			}

			if (m_bChalenge100)
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

	KdSafeDelete(pwi);
}

void ModeSelectUi::DrawSprite()
{
	Math::Matrix transMat = Math::Matrix::Identity;
	Math::Matrix mat = Math::Matrix::Identity;
	KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
	Math::Color color = {};

	mat = Math::Matrix::Identity;
	mat = Math::Matrix::CreateTranslation(m_selectBackPos);
	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_selectBackTex, 0, 0, 1280, 720);

	mat = Math::Matrix::CreateScale(0.75f) * Math::Matrix::CreateTranslation({ -380,325,0 });
	KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_modeSelectTex, 0, 0, 632, 62);

	mat = Math::Matrix::CreateTranslation(m_pictureFramePos);
	KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_pictureFrameTex, 0, 0, 580, 700);

	if (m_bBattleChara)
	{
		mat = Math::Matrix::CreateTranslation(m_gamePos);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_gameTex, 0, 0, 507, 660);

		mat = Math::Matrix::CreateScale(m_enemyBossTypeOneScale) * Math::Matrix::CreateTranslation(m_enemyBossTypeOnePos);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_enemyBossTypeOneTex, 0, 0, 210, 80);

		mat = Math::Matrix::CreateScale(m_enemyBossTypeTwoScale) * Math::Matrix::CreateTranslation(m_enemyBossTypeTwoPos);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_enemyBossTypeTwoTotalTex, 0, 0, 210, 80);

		mat = Math::Matrix::CreateScale(m_twoEnemyBossTypeOneScale) * Math::Matrix::CreateTranslation(m_twoEnemyBossTypeOnePos);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_twoEnemyBossTypeOneTex, 0, 0, 210, 80);

		mat = Math::Matrix::CreateScale(m_helpMkScale) * Math::Matrix::CreateTranslation(m_helpMkPos);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_helpMkTex, 0, 0, 50, 50);
	}

	mat = Math::Matrix::CreateScale(m_battleCharaScale) * Math::Matrix::CreateTranslation(m_battleCharaPos);
	KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_battleCharaTex, 0, 0, 520, 125);

	if (m_bChallengeChara)
	{
		mat = Math::Matrix::CreateTranslation(m_challengePos);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_challengeTex, 0, 0, 507, 660);


		mat = Math::Matrix::CreateScale(m_chalenge50Scale) * Math::Matrix::CreateTranslation(m_chalenge50Pos);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_chalenge50Tex, 0, 0, 210, 80);

		mat = Math::Matrix::CreateScale(m_chalenge100Scale) * Math::Matrix::CreateTranslation(m_chalenge100Pos);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_chalenge100Tex, 0, 0, 210, 80);


		/*mat = Math::Matrix::CreateScale(m_helpMkScale) * Math::Matrix::CreateTranslation(m_helpMkPos);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_helpMkTex, 0, 0, 50, 50);*/
	}

	mat = Math::Matrix::CreateScale(m_challengeCharaScale) * Math::Matrix::CreateTranslation(m_challengeCharaPos);
	KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_challengeCharaTex, 0, 0, 520, 125);

	mat = Math::Matrix::CreateScale(m_tutorialScale) * Math::Matrix::CreateTranslation(m_tutorialPos);
	KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_tutorialTex, 0, 0, 235, 265);

	mat = Math::Matrix::CreateScale(m_trainingScale) * Math::Matrix::CreateTranslation(m_trainingPos);
	KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_trainingTex, 0, 0, 235, 265);

	mat = Math::Matrix::CreateScale(m_titleScale) * Math::Matrix::CreateTranslation(m_titlePos);
	KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_selectTitleTex, 0, 0, 100, 50);

	mat = Math::Matrix::CreateScale(m_exitScale) * Math::Matrix::CreateTranslation(m_exitPos);
	KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_selectExitTex, 0, 0, 100, 50);

	mat = Math::Matrix::Identity;
	KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
	color = { 0,0,0,m_fadeAlpha };
	KdShaderManager::Instance().m_spriteShader.DrawBox(0, 0, 1280, 720, &color);

	if (m_bBattlehelp)
	{
		KdShaderManager::Instance().m_spriteShader.SetMatrix(Math::Matrix::Identity);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_battlehelpTex, 0, 0, 880, 520);

		mat = Math::Matrix::CreateScale(m_clauseMkScale) * Math::Matrix::CreateTranslation(m_clauseMkPos);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_clauseMkTex, 0, 0, 50, 50);
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
void TutorialUi::Init()
{
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

	m_PointTex[0].Load("Asset/Textures/Ui/Result/p0.png");
	m_PointTex[1].Load("Asset/Textures/Ui/Result/p1.png");
	m_PointTex[2].Load("Asset/Textures/Ui/Result/p2.png");
	m_PointTex[3].Load("Asset/Textures/Ui/Result/p3.png");
	m_PointTex[4].Load("Asset/Textures/Ui/Result/p4.png");

	m_addFadeAlpha = false;
	m_time = 0;

	m_exitScale = 1.0f;
	m_exitPos = { 0,-250,0 };

	m_fadeAlpha = 0.0f;
	m_backPos = { 550,-300,0 };
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
	m_weaponLeftYaiPos = { -500,250,0 };

	m_weaponRightYaiScale = 1.0f;
	m_weaponRightYaiPos = { 500,250,0, };

	m_weaOrHowLeftYaiScale = 1.0f;
	m_weaOrHowLeftYaiPos = { -600,0,0 };

	m_weaOrHowRightYaiScale = 1.0f;
	m_weaOrHowRightYaiPos = { 600,0,0 };

	m_operationScale = 1.0f;
	m_operationPos = { 0,250,0 };
	m_bOperation = false;
	m_tutorialCnt = 0;
}

void TutorialUi::Update()
{
	PWINDOWINFO pwi = new WINDOWINFO;
	pwi->cbSize = sizeof(WINDOWINFO);
	GetWindowInfo(Application::Instance().GetWindowHandle(), pwi);

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

	KdSafeDelete(pwi);
}

void TutorialUi::DrawSprite()
{
	Math::Matrix transMat = Math::Matrix::Identity;
	Math::Matrix mat = Math::Matrix::Identity;
	KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
	Math::Color color = {};
	std::shared_ptr<Player> spPlayer = m_wpPlayer.lock();

	if (spPlayer)
	{
		transMat = Math::Matrix::CreateTranslation(350, -250, 0);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponType1Tex, 0, 0, 120, 50);
		if (spPlayer->GetWeaponType() & Player::WeaponType::grassHopper)
		{
			Math::Rectangle rc = { 0,0,120,50 };
			color = { 1,1,1,0.6f };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponTypeOvreDarkTex, 0, 0, 120, 50, &rc, &color);
		}

		transMat = Math::Matrix::CreateTranslation(500, -250, 0);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponType2Tex, 0, 0, 120, 50);
		if (spPlayer->GetWeaponType() & Player::WeaponType::scorpion)
		{
			transMat = Math::Matrix::CreateTranslation(440, -250, 0);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			Math::Rectangle rc = { 0,0,120,50 };
			color = { 1,1,1,0.6f };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponTypeOvreDarkTex, 0, 0, 120, 50, &rc, &color, Math::Vector2(0, 0.5f));
		}
		else if (spPlayer->GetWeaponType() & Player::WeaponType::grassHopper && spPlayer->GetRGrassHopperPauCnt() != 0)
		{
			transMat = Math::Matrix::CreateTranslation(440, -250, 0);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			Math::Rectangle rc = { 0,0,spPlayer->GetRGrassHopperPauCnt() * 4,50 };
			color = { 1,1,1,0.6f };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponTypeOvreDarkTex, 0, 0, spPlayer->GetRGrassHopperPauCnt() * 4, 50, &rc, &color, Math::Vector2(0, 0.5f));
		}

		transMat = Math::Matrix::CreateTranslation(-500, -250, 0);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponType1Tex, 0, 0, 120, 50);
		if (spPlayer->GetWeaponType() & Player::WeaponType::lGrassHopper)
		{
			Math::Rectangle rc = { 0,0,120,50 };
			color = { 1,1,1,0.6f };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponTypeOvreDarkTex, 0, 0, 120, 50, &rc, &color);
		}

		transMat = Math::Matrix::CreateTranslation(-350, -250, 0);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponType2Tex, 0, 0, 120, 50);
		if (spPlayer->GetWeaponType() & Player::WeaponType::lScorpion)
		{
			transMat = Math::Matrix::CreateTranslation(-410, -250, 0);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			Math::Rectangle rc = { 0,0,120,50 };
			color = { 1,1,1,0.6f };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponTypeOvreDarkTex, 0, 0, 120, 50, &rc, &color, Math::Vector2(0, 0.5f));
		}
		else if (spPlayer->GetWeaponType() & Player::WeaponType::lGrassHopper && spPlayer->GetLGrassHopperPauCnt() != 0)
		{
			transMat = Math::Matrix::CreateTranslation(-410, -250, 0);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			Math::Rectangle rc = { 0,0,spPlayer->GetLGrassHopperPauCnt() * 4,50 };
			color = { 1,1,1,0.6f };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponTypeOvreDarkTex, 0, 0, spPlayer->GetLGrassHopperPauCnt() * 4, 50, &rc, &color, Math::Vector2(0, 0.5f));
		}

		transMat = Math::Matrix::CreateTranslation(-630, 300, 0);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		Math::Rectangle rc = { 0,0,400,50 };
		color = { 1, 1, 1, 1 };
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_enduranceBarTex, 0, 0, 400, 50, &rc, &color, Math::Vector2(0, 0.5f));

		transMat = Math::Matrix::CreateTranslation(-630, 300, 0);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		rc = { 0,0,(int)(spPlayer->GetEndurance()),50 };
		color = { 1, 1, 1, 1 };
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_enduranceTex, 0, 0, (int)(spPlayer->GetEndurance()), 50, &rc, &color, Math::Vector2(0, 0.5f));

		transMat = Math::Matrix::CreateTranslation(-630, 255, 0);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		rc = { 0,0,320,40 };
		color = { 1, 1, 1, 1 };
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_torionBarTex, 0, 0, 320, 40, &rc, &color, Math::Vector2(0, 0.5f));

		transMat = Math::Matrix::CreateTranslation(-620, 255, 0);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		rc = { 0,0,(int)(spPlayer->GetVForce()),30 };
		color = { 1, 1, 1, 1 };
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_torionTex, 0, 0, (int)(spPlayer->GetVForce()), 30, &rc, &color, Math::Vector2(0, 0.5f));

	}

	if (m_tutorialType != sonotaTu)
	{
		transMat = Math::Matrix::CreateTranslation(450, -300, 0);

		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_PointTex[m_tutorialCnt], 0, 0, 102, 114);

		transMat = Math::Matrix::CreateTranslation(550, -300, 0);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_PointTex[5], 0, 0, 102, 114);
	}

	if (m_bOption)
	{
		transMat = Math::Matrix::Identity;
		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		color = { 0,0,0,0.2f };
		KdShaderManager::Instance().m_spriteShader.DrawBox(0, 0, 1280, 720, &color);

		if (!m_bOperation)
		{
			mat = Math::Matrix::CreateScale(m_operationScale) * Math::Matrix::CreateTranslation(m_operationPos);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_operationTex, 0, 0, 700, 120);

			mat = Math::Matrix::CreateScale(m_selectScale) * Math::Matrix::CreateTranslation(m_selectPos);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_selectTex, 0, 0, 700, 120);

			mat = Math::Matrix::CreateScale(m_exitScale) * Math::Matrix::CreateTranslation(m_exitPos);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_exitTex, 0, 0, 700, 120);
		}
		else if (m_bOperation)
		{
			if (m_bHowToPage)
			{
				transMat = Math::Matrix::Identity;
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_howToTex, 0, 0, 1250, 500);
			}

			if (m_bWeaponDataPage)
			{
				if (m_bWeaponDataHopperPage)
				{
					transMat = Math::Matrix::Identity;
					KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_hopperDataTex, 0, 0, 1156, 260);
				}
				else if (m_bWeaponDataScoPage)
				{
					transMat = Math::Matrix::Identity;
					KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponInfoTex, 0, 0, 1151, 431);
				}

				mat = Math::Matrix::CreateScale(m_weaponRightYaiScale) * Math::Matrix::CreateTranslation(m_weaponRightYaiPos);
				KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_rightYaiTex, 0, 0, 70, 55);

				mat = Math::Matrix::CreateScale(m_weaponLeftYaiScale) * Math::Matrix::CreateTranslation(m_weaponLeftYaiPos);
				KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_leftYaiTex, 0, 0, 70, 55);
			}

			mat = Math::Matrix::CreateScale(m_weaOrHowRightYaiScale) * Math::Matrix::CreateTranslation(m_weaOrHowRightYaiPos);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_rightYaiTex, 0, 0, 70, 55);

			mat = Math::Matrix::CreateScale(m_weaOrHowLeftYaiScale) * Math::Matrix::CreateTranslation(m_weaOrHowLeftYaiPos);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_leftYaiTex, 0, 0, 70, 55);

		}

		mat = Math::Matrix::CreateScale(m_backScale) * Math::Matrix::CreateTranslation(m_backPos);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_backTex, 0, 0, 182, 80);
	}

	if (m_bTutorialView)
	{
		transMat = Math::Matrix::Identity;
		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		color = { 0,0.,0,0.4f };
		KdShaderManager::Instance().m_spriteShader.DrawBox(0, 0, 1280, 720, &color);

		switch (m_tutorialType)
		{
		case kihonTu:
			mat = Math::Matrix::CreateScale(m_backScale) * Math::Matrix::CreateTranslation(0, 0, 0);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_tyuKihonTex, 0, 0, 1000, 500);
			break;
		case bukiTu:
			mat = Math::Matrix::CreateScale(m_backScale) * Math::Matrix::CreateTranslation(0, 0, 0);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_tyubukiTex, 0, 0, 1000, 500);
			break;
		case sukoADMoveTu:
			mat = Math::Matrix::CreateScale(m_backScale) * Math::Matrix::CreateTranslation(0, 0, 0);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_sukoADMoveTex, 0, 0, 1000, 500);
			break;
		case hopperTu:
			mat = Math::Matrix::CreateScale(m_backScale) * Math::Matrix::CreateTranslation(0, 0, 0);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_hopperTyuTex, 0, 0, 1000, 500);
			break;
		case sonotaTu:
			mat = Math::Matrix::CreateScale(m_backScale) * Math::Matrix::CreateTranslation(0, 0, 0);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_sonotaTyuTex, 0, 0, 1000, 500);
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
	PWINDOWINFO pwi = new WINDOWINFO;
	pwi->cbSize = sizeof(WINDOWINFO);
	GetWindowInfo(Application::Instance().GetWindowHandle(), pwi);

	POINT mousePos;
	GetCursorPos(&mousePos);

	mousePos.x -= 640;
	mousePos.y = mousePos.y * -1 + 360;
	Math::Vector3 Dis;
	float mouseX = (float)mousePos.x/* + (float)(pwi->rcWindow.left)*/;
	float mouseY = (float)mousePos.y + (float)(pwi->rcWindow.top + 35);

	float MouseLeft = mouseX - 2.0f;
	float MouseRight = mouseX + 2.0f;
	float MouseTop = mouseY + 2.0f;
	float MouseBottom = mouseY - 2.0f;

	if (!m_bOperation)
	{
		Math::Vector3 ExitPos;
		ExitPos.x = m_exitPos.x + (float)(pwi->rcWindow.left);
		ExitPos.y = m_exitPos.y /*+ (float)(pwi->rcWindow.top)*/;
		ExitPos.z = m_exitPos.z;

		float ExitLeft = ExitPos.x - 350;
		float ExitRight = ExitPos.x + 350;
		float ExitTop = ExitPos.y + 60;
		float ExitBottom = ExitPos.y - 60;

		if (MouseRight > ExitLeft && ExitRight > MouseLeft &&
			MouseTop > ExitBottom && ExitTop > MouseBottom)
		{
			m_exitScale = 1.2f;

			if (KdInputManager::Instance().IsPress("select"))
			{
				if (!m_addFadeAlpha)
				{
					m_bExit = true;
					m_addFadeAlpha = true;
					m_bFirstExit = true;
					KdAudioManager::Instance().Play("Asset/Audio/SE/PushButton.wav");
				}
			}

		}
		else
		{
			m_exitScale = 1.0f;
		}

		Math::Vector3 ModeSelectPos;
		ModeSelectPos.x = m_selectPos.x + (float)(pwi->rcWindow.left);
		ModeSelectPos.y = m_selectPos.y /*+ (float)(pwi->rcWindow.top)*/;
		ModeSelectPos.z = m_selectPos.z;

		float SelectLeft = ModeSelectPos.x - 350;
		float SelectRight = ModeSelectPos.x + 350;
		float SelectTop = ModeSelectPos.y + 60;
		float SelectBottom = ModeSelectPos.y - 60;

		if (MouseRight > SelectLeft && SelectRight > MouseLeft &&
			MouseTop > SelectBottom && SelectTop > MouseBottom)
		{
			m_exitScale = 1.0f;
			m_selectScale = 1.2f;

			if (KdInputManager::Instance().IsPress("select"))
			{
				if (!m_addFadeAlpha)
				{
					m_bSelect = true;
					m_addFadeAlpha = true;
					//KdEffekseerManager::GetInstance().OnResumeEfkUpdate();
					KdAudioManager::Instance().Play("Asset/Audio/SE/PushButton.wav");
				}
			}

		}
		else
		{
			m_selectScale = 1.0f;
		}

		Math::Vector3 InfoPos;
		InfoPos.x = m_operationPos.x + (float)(pwi->rcWindow.left);
		InfoPos.y = m_operationPos.y /*+ (float)(pwi->rcWindow.top)*/;
		InfoPos.z = m_operationPos.z;

		float InfoLeft = InfoPos.x - 350;
		float InfoRight = InfoPos.x + 350;
		float InfoTop = InfoPos.y + 60;
		float InfoBottom = InfoPos.y - 60;

		if (MouseRight > InfoLeft && InfoRight > MouseLeft &&
			MouseTop > InfoBottom && InfoTop > MouseBottom)
		{
			m_exitScale = 1.0f;
			m_operationScale = 1.2f;

			if (KdInputManager::Instance().IsPress("select"))
			{
				m_bOperation = true;
				KdAudioManager::Instance().Play("Asset/Audio/SE/PushButton.wav");
			}

		}
		else
		{
			m_operationScale = 1.0f;
		}

		Math::Vector3 BackPos;
		BackPos.x = m_backPos.x + (float)(pwi->rcWindow.left);
		BackPos.y = m_backPos.y /*+ (float)(pwi->rcWindow.top)*/;
		BackPos.z = m_backPos.z;
		Dis = BackPos - Math::Vector3(mouseX, mouseY, 0.0f);
		if (Dis.Length() <= 40)
		{
			m_backScale = 1.0f;
			if (KdInputManager::Instance().IsPress("select"))
			{
				m_bOption = false;
				ShowCursor(false); // マウスカーソルを消す
				SetCursorPos(640, 360);
				KdAudioManager::Instance().Play("Asset/Audio/SE/backPush.wav");
				KdEffekseerManager::GetInstance().OnResumeEfkUpdate();
			}
		}
		else
		{
			m_backScale = 0.8f;
		}

		if (m_addFadeAlpha)
		{
			m_fadeAlpha += (1.0f / 90);
			if (m_fadeAlpha >= 1.0f)
			{
				m_fadeAlpha = 1.0f;

				if (m_bSelect)
				{
					ShowCursor(false); // マウスカーソルを消す
					KdEffekseerManager::GetInstance().Reset();
					SceneManager::Instance().SetNextScene
					(
						SceneManager::SceneType::select
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
			}
		}

	}
	else
	{

		Math::Vector3 BackPos;
		BackPos.x = m_backPos.x + (float)(pwi->rcWindow.left);
		BackPos.y = m_backPos.y /*+ (float)(pwi->rcWindow.top)*/;
		BackPos.z = m_backPos.z;
		Dis = BackPos - Math::Vector3(mouseX, mouseY, 0.0f);
		if (Dis.Length() <= 40)
		{
			m_backScale = 1.0f;
			if (KdInputManager::Instance().IsPress("select"))
			{
				m_bWeaponDataPage = false;
				m_bWeaponDataHopperPage = false;
				m_bWeaponDataScoPage = true;
				m_bHowToPage = true;
				m_bOperation = false;
				KdAudioManager::Instance().Play("Asset/Audio/SE/backPush.wav");
			}
		}
		else
		{
			m_backScale = 0.8f;
		}

		Math::Vector3 WeaOrHowLeftYaiPos;
		WeaOrHowLeftYaiPos.x = m_weaOrHowLeftYaiPos.x + (float)(pwi->rcWindow.left);
		WeaOrHowLeftYaiPos.y = m_weaOrHowLeftYaiPos.y /*+ (float)(pwi->rcWindow.top)*/;
		WeaOrHowLeftYaiPos.z = m_weaOrHowLeftYaiPos.z;
		Dis = WeaOrHowLeftYaiPos - Math::Vector3(mouseX, mouseY, 0.0f);
		if (Dis.Length() <= 30)
		{
			m_weaOrHowLeftYaiScale = 1.5f;
			m_weaOrHowRightYaiScale = 1.0f;

			if (KdInputManager::Instance().IsPress("select"))
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
		}
		else
		{
			m_weaOrHowLeftYaiScale = 1.0f;
		}

		Math::Vector3 WeaOrHowRightYaiPos;
		WeaOrHowRightYaiPos.x = m_weaOrHowRightYaiPos.x + (float)(pwi->rcWindow.left);
		WeaOrHowRightYaiPos.y = m_weaOrHowRightYaiPos.y /*+ (float)(pwi->rcWindow.top)*/;
		WeaOrHowRightYaiPos.z = m_weaOrHowRightYaiPos.z;
		Dis = m_weaOrHowRightYaiPos - Math::Vector3(mouseX, mouseY, 0.0f);
		if (Dis.Length() <= 30)
		{
			m_weaOrHowLeftYaiScale = 1.0f;
			m_weaOrHowRightYaiScale = 1.5f;

			if (KdInputManager::Instance().IsPress("select"))
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
		}
		else
		{
			m_weaOrHowRightYaiScale = 1.0f;
		}

		if (m_bWeaponDataPage)
		{
			Math::Vector3 WeaponLeftYaiPos;
			WeaponLeftYaiPos.x = m_weaponLeftYaiPos.x + (float)(pwi->rcWindow.left);
			WeaponLeftYaiPos.y = m_weaponLeftYaiPos.y /*+ (float)(pwi->rcWindow.top)*/;
			WeaponLeftYaiPos.z = m_weaponLeftYaiPos.z;

			Dis = WeaponLeftYaiPos - Math::Vector3(mouseX, mouseY, 0.0f);
			if (Dis.Length() <= 30)
			{
				m_weaponLeftYaiScale = 1.5f;
				m_weaponRightYaiScale = 1.0f;

				if (KdInputManager::Instance().IsPress("select"))
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
			else
			{
				m_weaponLeftYaiScale = 1.0f;
			}

			Math::Vector3 WeaponRightYaiPos;
			WeaponRightYaiPos.x = m_weaponRightYaiPos.x + (float)(pwi->rcWindow.left);
			WeaponRightYaiPos.y = m_weaponRightYaiPos.y /*+ (float)(pwi->rcWindow.top)*/;
			WeaponRightYaiPos.z = m_weaponRightYaiPos.z;

			Dis = WeaponRightYaiPos - Math::Vector3(mouseX, mouseY, 0.0f);
			if (Dis.Length() <= 30)
			{
				m_weaponLeftYaiScale = 1.0f;
				m_weaponRightYaiScale = 1.5f;

				if (KdInputManager::Instance().IsPress("select"))
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
			else
			{
				m_weaponRightYaiScale = 1.0f;
			}
		}
	}
}
// ＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝

// トレーニングモードのUI関連の処理＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
void TrainingUi::Init()
{
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

	m_selectPos = { 0,0,0 };
	m_exitPos = { 0,-250,0 };
	m_fadeAlpha = 0.0f;

	m_backPos = { 550,-300,0 };
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
	m_weaponLeftYaiPos = { -500,250,0 };

	m_weaponRightYaiScale = 1.0f;
	m_weaponRightYaiPos = { 500,250,0, };

	m_weaOrHowLeftYaiScale = 1.0f;
	m_weaOrHowLeftYaiPos = { -600,0,0 };

	m_weaOrHowRightYaiScale = 1.0f;
	m_weaOrHowRightYaiPos = { 600,0,0 };

	m_operationScale = 1.0f;
	m_operationPos = { 0,250,0 };
	m_bOperation = false;
}

void TrainingUi::Update()
{
	if (m_time <= 240)
	{
		m_time++;
	}

	if (m_time >= 0 && m_time < 30)
	{
		m_countThreeScale += (1.0f / 10.0f);
		if (m_countThreeScale > 1.0f)
		{
			m_countThreeScale = 1.0f;
		}

		m_countThreeAlpha += (1.0f / 30.0f);
		if (m_countThreeAlpha > 1.0f)
		{
			m_countThreeAlpha = 1.0f;
		}
	}
	else if (m_time >= 30 && m_time < 60)
	{

		m_countThreeAlpha -= (1.0f / 30.0f);
		if (m_countThreeAlpha < 0.0f)
		{
			m_countThreeAlpha = 0.0f;
		}
	}
	else if (m_time >= 60 && m_time < 90)
	{
		m_countTwoScale += (1.0f / 10.0f);
		if (m_countTwoScale > 1.0f)
		{
			m_countTwoScale = 1.0f;
		}

		m_countTwoAlpha += (1.0f / 30.0f);
		if (m_countTwoAlpha > 1.0f)
		{
			m_countTwoAlpha = 1.0f;
		}
	}
	else if (m_time >= 90 && m_time < 120)
	{
		m_countTwoAlpha -= (1.0f / 30.0f);
		if (m_countThreeAlpha < 0.0f)
		{
			m_countThreeAlpha = 0.0f;
		}
	}
	else if (m_time >= 120 && m_time < 150)
	{
		m_countOneScale += (1.0f / 10.0f);
		if (m_countOneScale > 1.0f)
		{
			m_countOneScale = 1.0f;
		}

		m_countOneAlpha += (1.0f / 30.0f);
		if (m_countOneAlpha > 1.0f)
		{
			m_countOneAlpha = 1.0f;
		}
	}
	else if (m_time >= 150 && m_time < 180)
	{
		m_countOneAlpha -= (1.0f / 30.0f);
		if (m_countOneAlpha < 0.0f)
		{
			m_countOneAlpha = 0.0f;
		}
	}
	else if (m_time >= 180 && m_time < 210)
	{
		m_countGoScale += (1.0f / 10.0f);
		if (m_countGoScale > 1.0f)
		{
			m_countGoScale = 1.0f;
		}

		m_countGoAlpha += (1.0f / 30.0f);
		if (m_countGoAlpha > 1.0f)
		{
			m_countGoAlpha = 1.0f;
		}
	}
	else if (m_time >= 210 && m_time < 240)
	{
		m_countGoAlpha -= (1.0f / 30.0f);
		if (m_countGoAlpha < 0.0f)
		{
			m_countGoAlpha = 0.0f;
			SetCursorPos(640, 360);
		}
	}

	PWINDOWINFO pwi = new WINDOWINFO;
	pwi->cbSize = sizeof(WINDOWINFO);
	GetWindowInfo(Application::Instance().GetWindowHandle(), pwi);

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

	KdSafeDelete(pwi);
}

void TrainingUi::PostUpdate()
{
	Math::Vector3 pos = Math::Vector3(m_wpEnemy.lock()->GetPos().x, m_wpEnemy.lock()->GetPos().y + m_wpEnemy.lock()->GetAddCenterVal().y, m_wpEnemy.lock()->GetPos().z);

	{
		POINT dev;
		KdDirect3D::Instance().WorldToClient(pos, dev, m_wpCamera.lock()->WorkCamera()->GetCameraMatrix(), m_wpCamera.lock()->WorkCamera()->GetProjMatrix());
		dev.x -= (long)640.0f;
		dev.y = (long)(dev.y * -1 + 360.0f);
		float z = m_wpCamera.lock()->GetPos().z - m_wpEnemy.lock()->GetPos().z;
		dev.y += (long)(180 - (std::fabs(z) * 1.5f));
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
	std::shared_ptr<Player> spPlayer = m_wpPlayer.lock();

	transMat = Math::Matrix::Identity;

	if (m_time >= 0 && m_time < 60)
	{
		mat = Math::Matrix::CreateScale(m_countThreeScale) * transMat;
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		Math::Rectangle rc = { 0,0,121, 136 };
		color = { 1,1,1,m_countThreeAlpha };
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_countThreeTex, 0, 0, 121, 136, &rc, &color);
	}
	else if (m_time >= 60 && m_time < 120)
	{
		mat = Math::Matrix::CreateScale(m_countTwoScale) * transMat;
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		Math::Rectangle rc = { 0,0,121, 136 };
		color = { 1,1,1,m_countTwoAlpha };
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_countTwoTex, 0, 0, 121, 136, &rc, &color);
	}
	else if (m_time >= 120 && m_time < 180)
	{
		mat = Math::Matrix::CreateScale(m_countOneScale) * transMat;
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		Math::Rectangle rc = { 0,0,121, 136 };
		color = { 1,1,1,m_countOneAlpha };
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_countOneTex, 0, 0, 121, 136, &rc, &color);
	}
	else if (m_time >= 180 && m_time < 240)
	{
		mat = Math::Matrix::CreateScale(m_countGoScale) * transMat;
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		Math::Rectangle rc = { 0,0,211, 136 };
		color = { 1,1,1,m_countGoAlpha };
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_countGoTex, 0, 0, 211, 136, &rc, &color);
	}


	if (spPlayer)
	{
		transMat = Math::Matrix::CreateTranslation(350, -250, 0);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponType1Tex, 0, 0, 120, 50);
		if (spPlayer->GetWeaponType() & Player::WeaponType::grassHopper)
		{
			Math::Rectangle rc = { 0,0,120,50 };
			color = { 1,1,1,0.6f };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponTypeOvreDarkTex, 0, 0, 120, 50, &rc, &color);
		}

		transMat = Math::Matrix::CreateTranslation(500, -250, 0);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponType2Tex, 0, 0, 120, 50);
		if (spPlayer->GetWeaponType() & Player::WeaponType::scorpion)
		{
			transMat = Math::Matrix::CreateTranslation(440, -250, 0);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			Math::Rectangle rc = { 0,0,120,50 };
			color = { 1,1,1,0.6f };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponTypeOvreDarkTex, 0, 0, 120, 50, &rc, &color, Math::Vector2(0, 0.5f));
		}
		else if (spPlayer->GetWeaponType() & Player::WeaponType::grassHopper && spPlayer->GetRGrassHopperPauCnt() != 0)
		{
			transMat = Math::Matrix::CreateTranslation(440, -250, 0);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			Math::Rectangle rc = { 0,0,spPlayer->GetRGrassHopperPauCnt() * 4,50 };
			color = { 1,1,1,0.6f };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponTypeOvreDarkTex, 0, 0, spPlayer->GetRGrassHopperPauCnt() * 4, 50, &rc, &color, Math::Vector2(0, 0.5f));
		}

		transMat = Math::Matrix::CreateTranslation(-500, -250, 0);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponType1Tex, 0, 0, 120, 50);
		if (spPlayer->GetWeaponType() & Player::WeaponType::lGrassHopper)
		{
			Math::Rectangle rc = { 0,0,120,50 };
			color = { 1,1,1,0.6f };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponTypeOvreDarkTex, 0, 0, 120, 50, &rc, &color);
		}

		transMat = Math::Matrix::CreateTranslation(-350, -250, 0);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponType2Tex, 0, 0, 120, 50);
		if (spPlayer->GetWeaponType() & Player::WeaponType::lScorpion)
		{
			transMat = Math::Matrix::CreateTranslation(-410, -250, 0);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			Math::Rectangle rc = { 0,0,120,50 };
			color = { 1,1,1,0.6f };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponTypeOvreDarkTex, 0, 0, 120, 50, &rc, &color, Math::Vector2(0, 0.5f));
		}
		else if (spPlayer->GetWeaponType() & Player::WeaponType::lGrassHopper && spPlayer->GetLGrassHopperPauCnt() != 0)
		{
			transMat = Math::Matrix::CreateTranslation(-410, -250, 0);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			Math::Rectangle rc = { 0,0,spPlayer->GetLGrassHopperPauCnt() * 4,50 };
			color = { 1,1,1,0.6f };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponTypeOvreDarkTex, 0, 0, spPlayer->GetLGrassHopperPauCnt() * 4, 50, &rc, &color, Math::Vector2(0, 0.5f));
		}

		transMat = Math::Matrix::CreateTranslation(-630, 300, 0);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		Math::Rectangle rc = { 0,0,400,50 };
		color = { 1, 1, 1, 1 };
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_enduranceBarTex, 0, 0, 400, 50, &rc, &color, Math::Vector2(0, 0.5f));

		transMat = Math::Matrix::CreateTranslation(-630, 300, 0);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		rc = { 0,0,(int)(spPlayer->GetEndurance()),50 };
		color = { 1, 1, 1, 1 };
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_enduranceTex, 0, 0, (int)(spPlayer->GetEndurance()), 50, &rc, &color, Math::Vector2(0, 0.5f));

		transMat = Math::Matrix::CreateTranslation(-630, 255, 0);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		rc = { 0,0,320,40 };
		color = { 1, 1, 1, 1 };
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_torionBarTex, 0, 0, 320, 40, &rc, &color, Math::Vector2(0, 0.5f));

		transMat = Math::Matrix::CreateTranslation(-620, 255, 0);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		rc = { 0,0,(int)(spPlayer->GetVForce()),30 };
		color = { 1, 1, 1, 1 };
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_torionTex, 0, 0, (int)(spPlayer->GetVForce()), 30, &rc, &color, Math::Vector2(0, 0.5f));

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
				rc = { 0,0,(int)(m_wpEnemy.lock()->GetEndurance()),50 };
				color = { 1, 1, 1, 1 };
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_enduranceTex, 0, 0, (int)(m_wpEnemy.lock()->GetEndurance()), 50, &rc, &color, Math::Vector2(0, 0.5f));
				rc = { 0,0,400,50 };
				color = { 1, 1, 1, 1 };
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_enduranceBarTex, 0, 0, 400, 50, &rc, &color, Math::Vector2(0, 0.5f));
			}

			if (ang >= 45)
			{
				float m_heightDifference = m_wpEnemy.lock()->GetPos().y - m_wpPlayer.lock()->GetPos().y;

				if (m_heightDifference >= 2.0f)
				{
					mat = Math::Matrix::CreateTranslation(-25, 225.0f, 0.0f);
					KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
					rc = { 0,0,100,125 };
					color = { 1, 1, 1, 1 };
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_EnemyDirectionArrowUTex, 0, 0, m_EnemyDirectionArrowUTex.GetWidth(), m_EnemyDirectionArrowUTex.GetHeight(), &rc, &color, Math::Vector2(0, 0.5f));
				}
				else if (m_heightDifference <= -2.0f)
				{
					mat = Math::Matrix::CreateTranslation(-25, -225.0f, 0.0f);
					KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
					rc = { 0,0,100,125 };
					color = { 1, 1, 1, 1 };
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_EnemyDirectionArrowBTex, 0, 0, m_EnemyDirectionArrowBTex.GetWidth(), m_EnemyDirectionArrowBTex.GetHeight(), &rc, &color, Math::Vector2(0, 0.5f));
				}


				Math::Vector3 cross = DirectX::XMVector3Cross(nowVec, toVec);

				if (cross.y < 0)
				{
					mat = Math::Matrix::CreateTranslation(-500, 0.0f, 0.0f);
					KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
					rc = { 0,0,125,100 };
					color = { 1, 1, 1, 1 };
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_EnemyDirectionArrowLTex, 0, 0, m_EnemyDirectionArrowLTex.GetWidth(), m_EnemyDirectionArrowLTex.GetHeight(), &rc, &color, Math::Vector2(0, 0.5f));
				}
				else if (cross.y >= 0)
				{
					mat = Math::Matrix::CreateTranslation(500, 0.0f, 0.0f);
					KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
					rc = { 0,0,125,100 };
					color = { 1, 1, 1, 1 };
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_EnemyDirectionArrowRTex, 0, 0, m_EnemyDirectionArrowRTex.GetWidth(), m_EnemyDirectionArrowRTex.GetHeight(), &rc, &color, Math::Vector2(0, 0.5f));
				}

			}
		}
	}

	if (m_bOption)
	{
		transMat = Math::Matrix::Identity;
		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		color = { 0,0,0,0.2f };
		KdShaderManager::Instance().m_spriteShader.DrawBox(0, 0, 1280, 720, &color);

		if (!m_bOperation)
		{
			mat = Math::Matrix::CreateScale(m_operationScale) * Math::Matrix::CreateTranslation(m_operationPos);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_operationTex, 0, 0, 700, 120);

			mat = Math::Matrix::CreateScale(m_selectScale) * Math::Matrix::CreateTranslation(m_selectPos);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_selectTex, 0, 0, 700, 120);

			mat = Math::Matrix::CreateScale(m_exitScale) * Math::Matrix::CreateTranslation(m_exitPos);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_exitTex, 0, 0, 700, 120);
		}
		else if (m_bOperation)
		{
			if (m_bHowToPage)
			{
				transMat = Math::Matrix::Identity;
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_howToTex, 0, 0, 1250, 500);
			}

			if (m_bWeaponDataPage)
			{
				if (m_bWeaponDataHopperPage)
				{
					transMat = Math::Matrix::Identity;
					KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_hopperDataTex, 0, 0, 1156, 260);
				}
				else if (m_bWeaponDataScoPage)
				{
					transMat = Math::Matrix::Identity;
					KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponInfoTex, 0, 0, 1151, 431);
				}

				mat = Math::Matrix::CreateScale(m_weaponRightYaiScale) * Math::Matrix::CreateTranslation(m_weaponRightYaiPos);
				KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_rightYaiTex, 0, 0, 70, 55);

				mat = Math::Matrix::CreateScale(m_weaponLeftYaiScale) * Math::Matrix::CreateTranslation(m_weaponLeftYaiPos);
				KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_leftYaiTex, 0, 0, 70, 55);
			}

			mat = Math::Matrix::CreateScale(m_weaOrHowRightYaiScale) * Math::Matrix::CreateTranslation(m_weaOrHowRightYaiPos);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_rightYaiTex, 0, 0, 70, 55);

			mat = Math::Matrix::CreateScale(m_weaOrHowLeftYaiScale) * Math::Matrix::CreateTranslation(m_weaOrHowLeftYaiPos);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_leftYaiTex, 0, 0, 70, 55);

		}

		mat = Math::Matrix::CreateScale(m_backScale) * Math::Matrix::CreateTranslation(m_backPos);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_backTex, 0, 0, 182, 80);
	}

	mat = Math::Matrix::CreateTranslation(-50, 325, 0);

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
	PWINDOWINFO pwi = new WINDOWINFO;
	pwi->cbSize = sizeof(WINDOWINFO);
	GetWindowInfo(Application::Instance().GetWindowHandle(), pwi);

	POINT mousePos;
	GetCursorPos(&mousePos);

	mousePos.x -= 640;
	mousePos.y = mousePos.y * -1 + 360;
	Math::Vector3 Dis;
	float mouseX = (float)mousePos.x/* + (float)(pwi->rcWindow.left)*/;
	float mouseY = (float)mousePos.y + (float)(pwi->rcWindow.top + 35);

	float MouseLeft = mouseX - 2.0f;
	float MouseRight = mouseX + 2.0f;
	float MouseTop = mouseY + 2.0f;
	float MouseBottom = mouseY - 2.0f;

	if (!m_bOperation)
	{
		Math::Vector3 ExitPos;
		ExitPos.x = m_exitPos.x + (float)(pwi->rcWindow.left);
		ExitPos.y = m_exitPos.y /*+ (float)(pwi->rcWindow.top)*/;
		ExitPos.z = m_exitPos.z;

		float ExitLeft = ExitPos.x - 350;
		float ExitRight = ExitPos.x + 350;
		float ExitTop = ExitPos.y + 60;
		float ExitBottom = ExitPos.y - 60;

		if (MouseRight > ExitLeft && ExitRight > MouseLeft &&
			MouseTop > ExitBottom && ExitTop > MouseBottom)
		{
			m_exitScale = 1.2f;

			if (KdInputManager::Instance().IsPress("select"))
			{
				if (!m_addFadeAlpha)
				{
					m_bExit = true;
					m_addFadeAlpha = true;
					m_bFirstExit = true;
					KdAudioManager::Instance().Play("Asset/Audio/SE/PushButton.wav");
				}
			}

		}
		else
		{
			m_exitScale = 1.0f;
		}

		Math::Vector3 ModeSelectPos;
		ModeSelectPos.x = m_selectPos.x + (float)(pwi->rcWindow.left);
		ModeSelectPos.y = m_selectPos.y /*+ (float)(pwi->rcWindow.top)*/;
		ModeSelectPos.z = m_selectPos.z;

		float SelectLeft = ModeSelectPos.x - 350;
		float SelectRight = ModeSelectPos.x + 350;
		float SelectTop = ModeSelectPos.y + 60;
		float SelectBottom = ModeSelectPos.y - 60;

		if (MouseRight > SelectLeft && SelectRight > MouseLeft &&
			MouseTop > SelectBottom && SelectTop > MouseBottom)
		{
			m_exitScale = 1.0f;
			m_selectScale = 1.2f;

			if (KdInputManager::Instance().IsPress("select"))
			{
				if (!m_addFadeAlpha)
				{
					m_bSelect = true;
					m_addFadeAlpha = true;
					//KdEffekseerManager::GetInstance().OnResumeEfkUpdate();
					KdAudioManager::Instance().Play("Asset/Audio/SE/PushButton.wav");
				}
			}

		}
		else
		{
			m_selectScale = 1.0f;
		}

		Math::Vector3 InfoPos;
		InfoPos.x = m_operationPos.x + (float)(pwi->rcWindow.left);
		InfoPos.y = m_operationPos.y /*+ (float)(pwi->rcWindow.top)*/;
		InfoPos.z = m_operationPos.z;

		float InfoLeft = InfoPos.x - 350;
		float InfoRight = InfoPos.x + 350;
		float InfoTop = InfoPos.y + 60;
		float InfoBottom = InfoPos.y - 60;

		if (MouseRight > InfoLeft && InfoRight > MouseLeft &&
			MouseTop > InfoBottom && InfoTop > MouseBottom)
		{
			m_exitScale = 1.0f;
			m_operationScale = 1.2f;

			if (KdInputManager::Instance().IsPress("select"))
			{
				m_bOperation = true;
				KdAudioManager::Instance().Play("Asset/Audio/SE/PushButton.wav");
			}

		}
		else
		{
			m_operationScale = 1.0f;
		}

		Math::Vector3 BackPos;
		BackPos.x = m_backPos.x + (float)(pwi->rcWindow.left);
		BackPos.y = m_backPos.y /*+ (float)(pwi->rcWindow.top)*/;
		BackPos.z = m_backPos.z;
		Dis = BackPos - Math::Vector3(mouseX, mouseY, 0.0f);
		if (Dis.Length() <= 40)
		{
			m_backScale = 1.0f;
			if (KdInputManager::Instance().IsPress("select"))
			{
				m_bOption = false;
				ShowCursor(false); // マウスカーソルを消す
				SetCursorPos(640, 360);
				KdAudioManager::Instance().Play("Asset/Audio/SE/backPush.wav");
				KdEffekseerManager::GetInstance().OnResumeEfkUpdate();
			}
		}
		else
		{
			m_backScale = 0.8f;
		}

		if (m_addFadeAlpha)
		{
			m_fadeAlpha += (1.0f / 90);
			if (m_fadeAlpha >= 1.0f)
			{
				m_fadeAlpha = 1.0f;

				if (m_bSelect)
				{
					ShowCursor(false); // マウスカーソルを消す
					KdEffekseerManager::GetInstance().Reset();
					SceneManager::Instance().SetNextScene
					(
						SceneManager::SceneType::select
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
			}
		}

	}
	else
	{

		Math::Vector3 BackPos;
		BackPos.x = m_backPos.x + (float)(pwi->rcWindow.left);
		BackPos.y = m_backPos.y /*+ (float)(pwi->rcWindow.top)*/;
		BackPos.z = m_backPos.z;
		Dis = BackPos - Math::Vector3(mouseX, mouseY, 0.0f);
		if (Dis.Length() <= 40)
		{
			m_backScale = 1.0f;
			if (KdInputManager::Instance().IsPress("select"))
			{
				m_bWeaponDataPage = false;
				m_bWeaponDataHopperPage = false;
				m_bWeaponDataScoPage = true;
				m_bHowToPage = true;
				m_bOperation = false;
				KdAudioManager::Instance().Play("Asset/Audio/SE/backPush.wav");
			}
		}
		else
		{
			m_backScale = 0.8f;
		}

		Math::Vector3 WeaOrHowLeftYaiPos;
		WeaOrHowLeftYaiPos.x = m_weaOrHowLeftYaiPos.x + (float)(pwi->rcWindow.left);
		WeaOrHowLeftYaiPos.y = m_weaOrHowLeftYaiPos.y /*+ (float)(pwi->rcWindow.top)*/;
		WeaOrHowLeftYaiPos.z = m_weaOrHowLeftYaiPos.z;
		Dis = WeaOrHowLeftYaiPos - Math::Vector3(mouseX, mouseY, 0.0f);
		if (Dis.Length() <= 30)
		{
			m_weaOrHowLeftYaiScale = 1.5f;
			m_weaOrHowRightYaiScale = 1.0f;

			if (KdInputManager::Instance().IsPress("select"))
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
		}
		else
		{
			m_weaOrHowLeftYaiScale = 1.0f;
		}

		Math::Vector3 WeaOrHowRightYaiPos;
		WeaOrHowRightYaiPos.x = m_weaOrHowRightYaiPos.x + (float)(pwi->rcWindow.left);
		WeaOrHowRightYaiPos.y = m_weaOrHowRightYaiPos.y /*+ (float)(pwi->rcWindow.top)*/;
		WeaOrHowRightYaiPos.z = m_weaOrHowRightYaiPos.z;
		Dis = m_weaOrHowRightYaiPos - Math::Vector3(mouseX, mouseY, 0.0f);
		if (Dis.Length() <= 30)
		{
			m_weaOrHowLeftYaiScale = 1.0f;
			m_weaOrHowRightYaiScale = 1.5f;

			if (KdInputManager::Instance().IsPress("select"))
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
		}
		else
		{
			m_weaOrHowRightYaiScale = 1.0f;
		}

		if (m_bWeaponDataPage)
		{
			Math::Vector3 WeaponLeftYaiPos;
			WeaponLeftYaiPos.x = m_weaponLeftYaiPos.x + (float)(pwi->rcWindow.left);
			WeaponLeftYaiPos.y = m_weaponLeftYaiPos.y /*+ (float)(pwi->rcWindow.top)*/;
			WeaponLeftYaiPos.z = m_weaponLeftYaiPos.z;

			Dis = WeaponLeftYaiPos - Math::Vector3(mouseX, mouseY, 0.0f);
			if (Dis.Length() <= 30)
			{
				m_weaponLeftYaiScale = 1.5f;
				m_weaponRightYaiScale = 1.0f;

				if (KdInputManager::Instance().IsPress("select"))
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
			else
			{
				m_weaponLeftYaiScale = 1.0f;
			}

			Math::Vector3 WeaponRightYaiPos;
			WeaponRightYaiPos.x = m_weaponRightYaiPos.x + (float)(pwi->rcWindow.left);
			WeaponRightYaiPos.y = m_weaponRightYaiPos.y /*+ (float)(pwi->rcWindow.top)*/;
			WeaponRightYaiPos.z = m_weaponRightYaiPos.z;

			Dis = WeaponRightYaiPos - Math::Vector3(mouseX, mouseY, 0.0f);
			if (Dis.Length() <= 30)
			{
				m_weaponLeftYaiScale = 1.0f;
				m_weaponRightYaiScale = 1.5f;

				if (KdInputManager::Instance().IsPress("select"))
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
			else
			{
				m_weaponRightYaiScale = 1.0f;
			}
		}
	}
}
// ＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝