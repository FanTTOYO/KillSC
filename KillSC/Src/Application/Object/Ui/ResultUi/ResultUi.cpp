#include "ResultUi.h"
#include "../../../Scene/SceneManager.h"

void ResultUi::Init(std::weak_ptr<json11::Json> a_wpJsonObj)
{
	m_mpUiSharedObj = (*a_wpJsonObj.lock())["Ui"].object_items();
	m_mpDedicatedObj = m_mpUiSharedObj["ResultUi"].object_items();

	m_screenAlphaFadeSpeed = static_cast<float>(m_mpUiSharedObj["ScreenAlphaFadeSpeed"].number_value());
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
	m_continueButtonTex.Load("Asset/Textures/Ui/Result/continue.png");
	m_modeSelectButtonTex.Load("Asset/Textures/Ui/Result/modeSelect.png");


	m_time = 0;
	m_bSceneCangePossible = false;

	m_continueButtonPos = Math::Vector3(static_cast<float>(m_mpDedicatedObj["ContinueButtonPos"][0].number_value()),
			                            static_cast<float>(m_mpDedicatedObj["ContinueButtonPos"][1].number_value()),
			                            static_cast<float>(m_mpDedicatedObj["ContinueButtonPos"][2].number_value()));

	m_modeSelectButtonPos = Math::Vector3(static_cast<float>(m_mpDedicatedObj["ModeSelectButtonPos"][0].number_value()),
		                                  static_cast<float>(m_mpDedicatedObj["ModeSelectButtonPos"][1].number_value()),
		                                  static_cast<float>(m_mpDedicatedObj["ModeSelectButtonPos"][2].number_value()));

	m_continueButtonScale = 1.0f;
	m_modeSelectButtonScale = 1.0f;

	m_bContinueButton = false;
	m_bModeSelectButton = false;
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
				std::ofstream ofs("Asset/Data/PointAndRank.csv"); // èëÇ´çûÇ›
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
					ofs << "point" << ',' << m_weaponPoint << ',' << '\n' << "rank" << ',' << "A" << ',' << '\n' << "Skill" << ',' << "É}ÉXÉ^Å[" << '\n';
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
				std::ofstream ofs("Asset/Data/PointAndRank.csv"); // èëÇ´çûÇ›
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
					ofs << "point" << ',' << m_weaponPoint << ',' << '\n' << "rank" << ',' << "A" << ',' << '\n' << "Skill" << ',' << "É}ÉXÉ^Å[" << '\n';
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
		if (ButtomProcessing({ m_continueButtonPos.x,m_continueButtonPos.y }, m_continueButtonTex, m_continueButtonScale))
		{
			if (!m_addFadeAlpha)
			{
				m_bContinueButton = true;
				m_addFadeAlpha = true;
				KdAudioManager::Instance().Play("Asset/Audio/SE/PushButton.wav");
			}
		}

		if (ButtomProcessing({ m_modeSelectButtonPos.x,m_modeSelectButtonPos.y }, m_modeSelectButtonTex, m_modeSelectButtonScale))
		{
			if (!m_addFadeAlpha)
			{
				m_bModeSelectButton = true;
				m_addFadeAlpha = true;
				KdAudioManager::Instance().Play("Asset/Audio/SE/PushButton.wav");
			}
		}
	}

	if (m_addFadeAlpha)
	{
		m_fadeAlpha += m_screenAlphaFadeSpeed;
		if (m_fadeAlpha >= 1.0f)
		{
			m_fadeAlpha = 1.0f;

			
			if (m_bModeSelectButton)
			{
				KdAudioManager::Instance().StopAllSound();
				KdAudioManager::Instance().Play("Asset/Audio/BGM/1 Fated Battle loop.wav", true);
				SceneManager::Instance().SetNextScene
				(
					SceneManager::SceneType::select
				);
			}

			if (m_bContinueButton)
			{
				if (SceneManager::Instance().GetPreviousSceneType() == SceneManager::SceneType::battle)
				{
					SceneManager::Instance().SetNextScene
					(
						SceneManager::SceneType::battle
					);
				}
				else if (SceneManager::Instance().GetPreviousSceneType() == SceneManager::SceneType::challenge)
				{
					SceneManager::Instance().SetNextScene
					(
						SceneManager::SceneType::challenge
					);
				}
			}
			
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
				  1.0f };

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
				// ìGëSëÃÇÃêî            // ìGÇÃåªë∂êî               
				ieftover = (50 - SceneManager::Instance().GetEnemyIeftover()) / 5;				// ìGÇÃåÇîjêîÇÇTï™äÑÇµÇΩíl 
				ieftoverExcess = (50 - SceneManager::Instance().GetEnemyIeftover()) % 5;        // ìGÇÃåÇîjêîÇÇTï™äÑÇµÇΩó]ÇË

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
				ieftover = (100 - SceneManager::Instance().GetEnemyIeftover()) / 5;				// ìGÇÃåÇîjêîÇÇTï™äÑÇµÇΩíl 
				ieftoverExcess = (100 - SceneManager::Instance().GetEnemyIeftover()) % 5;;		// ìGÇÃåÇîjêîÇÇTï™äÑÇµÇΩó]ÇË

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
		mat = Math::Matrix::CreateScale(m_continueButtonScale) * Math::Matrix::CreateTranslation(m_continueButtonPos);

		Math::Rectangle rc = { 0,0,static_cast<int>(m_continueButtonTex.GetWidth()), static_cast<int>(m_continueButtonTex.GetHeight()) };
		color = { 1,1,1,1 };
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_continueButtonTex, 0, 0, static_cast<int>(m_continueButtonTex.GetWidth()), static_cast<int>(m_continueButtonTex.GetHeight()), &rc, &color);

		mat = Math::Matrix::CreateScale(m_modeSelectButtonScale) * Math::Matrix::CreateTranslation(m_modeSelectButtonPos);

		rc = { 0,0,static_cast<int>(m_modeSelectButtonTex.GetWidth()), static_cast<int>(m_modeSelectButtonTex.GetHeight()) };
		color = { 1,1,1,1 };
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_modeSelectButtonTex, 0, 0, static_cast<int>(m_modeSelectButtonTex.GetWidth()), static_cast<int>(m_modeSelectButtonTex.GetHeight()), &rc, &color);
	}

	transMat = Math::Matrix::Identity;
	KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
	color = { 0,0,0,m_fadeAlpha };
	KdShaderManager::Instance().m_spriteShader.DrawBox(0, 0, 1280, 720, &color);
}