#include "ModeSelectUi.h"
#include "../../../Scene/SceneManager.h"

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

	m_battleCharaPos = { static_cast<float>(m_mpDedicatedObj["BattleCharaTexPos"][0].number_value()),
						static_cast<float>(m_mpDedicatedObj["BattleCharaTexPos"][1].number_value()),
						static_cast<float>(m_mpDedicatedObj["BattleCharaTexPos"][2].number_value())
	};
	m_enemyBossTypeOnePos = { static_cast<float>(m_mpDedicatedObj["EnemyBossTypeOneModeTexPos"][0].number_value()),
							 static_cast<float>(m_mpDedicatedObj["EnemyBossTypeOneModeTexPos"][1].number_value()),
							 static_cast<float>(m_mpDedicatedObj["EnemyBossTypeOneModeTexPos"][2].number_value()) };

	m_enemyBossTypeTwoPos = { static_cast<float>(m_mpDedicatedObj["EnemyBossTypeTwoModeTexPos"][0].number_value()),
							 static_cast<float>(m_mpDedicatedObj["EnemyBossTypeTwoModeTexPos"][1].number_value()),
							 static_cast<float>(m_mpDedicatedObj["EnemyBossTypeTwoModeTexPos"][2].number_value()) };

	m_twoEnemyBossTypeOnePos = { static_cast<float>(m_mpDedicatedObj["TwoEnemyBossTypeOneModeTexPos"][0].number_value()),
								static_cast<float>(m_mpDedicatedObj["TwoEnemyBossTypeOneModeTexPos"][1].number_value()),
								static_cast<float>(m_mpDedicatedObj["TwoEnemyBossTypeOneModeTexPos"][2].number_value()) };

	m_helpMkPos = { static_cast<float>(m_mpDedicatedObj["HelpMkTexPos"][0].number_value()),
				   static_cast<float>(m_mpDedicatedObj["HelpMkTexPos"][1].number_value()),
				   static_cast<float>(m_mpDedicatedObj["HelpMkTexPos"][2].number_value()) };

	m_clauseMkPos = { static_cast<float>(m_mpDedicatedObj["ClauseMkTexPos"][0].number_value()),
					 static_cast<float>(m_mpDedicatedObj["ClauseMkTexPos"][1].number_value()),
					 static_cast<float>(m_mpDedicatedObj["ClauseMkTexPos"][2].number_value()) };

	m_challengeCharaPos = { static_cast<float>(m_mpDedicatedObj["ChallengeCharaTexPos"][0].number_value()),
						   static_cast<float>(m_mpDedicatedObj["ChallengeCharaTexPos"][1].number_value()),
						   static_cast<float>(m_mpDedicatedObj["ChallengeCharaTexPos"][2].number_value()) };

	m_chalengeFiftyPos = { static_cast<float>(m_mpDedicatedObj["ChalengeFiftyTexPos"][0].number_value()),
					  static_cast<float>(m_mpDedicatedObj["ChalengeFiftyTexPos"][1].number_value()),
					  static_cast<float>(m_mpDedicatedObj["ChalengeFiftyTexPos"][2].number_value()) };

	m_chalengeHundredPos = { static_cast<float>(m_mpDedicatedObj["ChalengeHundredTexPos"][0].number_value()),
						static_cast<float>(m_mpDedicatedObj["ChalengeHundredTexPos"][1].number_value()),
						static_cast<float>(m_mpDedicatedObj["ChalengeHundredTexPos"][2].number_value()) };

	m_tutorialPos = { static_cast<float>(m_mpDedicatedObj["TutorialTexPos"][0].number_value()),
					 static_cast<float>(m_mpDedicatedObj["TutorialTexPos"][1].number_value()),
					 static_cast<float>(m_mpDedicatedObj["TutorialTexPos"][2].number_value()) };

	m_trainingPos = { static_cast<float>(m_mpDedicatedObj["TrainingTexPos"][0].number_value()),
					 static_cast<float>(m_mpDedicatedObj["TrainingTexPos"][1].number_value()),
					 static_cast<float>(m_mpDedicatedObj["TrainingTexPos"][2].number_value()) };

	m_titlePos = { static_cast<float>(m_mpDedicatedObj["TitlePos"][0].number_value()),
				  static_cast<float>(m_mpDedicatedObj["TitlePos"][1].number_value()),
				  static_cast<float>(m_mpDedicatedObj["TitlePos"][2].number_value()) };

	m_exitPos = { static_cast<float>(m_mpDedicatedObj["ExitPos"][0].number_value()),
				  static_cast<float>(m_mpDedicatedObj["ExitPos"][1].number_value()),
				  static_cast<float>(m_mpDedicatedObj["ExitPos"][2].number_value()) };

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
		if (ButtomProcessing({ m_clauseMkPos.x,m_clauseMkPos.y }, m_clauseMkTex, m_clauseMkScale, 1.0f, 0.1f))
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
		if (ButtomProcessing({ m_tutorialPos.x,m_tutorialPos.y }, m_tutorialTex, m_tutorialScale, 1.0f, 0.1f))
		{
			if (!m_addFadeAlpha)
			{
				m_bTutorial = true;
				m_addFadeAlpha = true;
				KdAudioManager::Instance().Play("Asset/Audio/SE/PushButton.wav");
			}
		}

		if (ButtomProcessing({ m_exitPos.x,m_exitPos.y }, m_exitTex, m_exitScale, 1.0f, 0.1f))
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