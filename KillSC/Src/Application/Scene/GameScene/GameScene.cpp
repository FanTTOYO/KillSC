#include "GameScene.h"
#include "../../Object/Character/Player/Player.h"
#include "../../Object/Character/Enemy/Enemy.h"
#include "../../Object/invisibleWall/InvisibleWall.h"
#include "../../Object/Sky/Sky.h"
#include "../../Object/Ground/Ground.h"
#include "../../Object/Bldg/Bldg.h"
#include "../../Camera/GameCamera/GameCamera.h"
#include "../SceneManager.h"
#include "../../Object/Ui/Ui.h"

void GameScene::Event()
{
	/*if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		SceneManager::Instance().SetNextScene
		(
			SceneManager::SceneType::result
		);
	}*/

	if (m_wpUi.lock()->GetTime() >= 0 && m_wpUi.lock()->GetTime() < 240 || m_wpUi.lock()->GetBOption())
	{
		m_bCountDown = true;
	}
	else
	{
		m_bCountDown = false;
	}

	if (m_bCountDown)
	{
		m_wpUi.lock()->Update();
	}

	if (m_wpUi.lock()->GetGameTimeM10() == 0 &&
		m_wpUi.lock()->GetGameTimeM1() == 0 &&
		m_wpUi.lock()->GetGameTimeS10() == 0 &&
		m_wpUi.lock()->GetGameTimeS1() == 0)
	{
		if (SceneManager::Instance().GetSceneType() == SceneManager::SceneType::challenge)
		{
			SceneManager::Instance().SetBAddOrSubVal(false);
			SceneManager::Instance().SetPointAddOrSubVal(500);
		}
		else
		{
			if (SceneManager::Instance().GetEnemyIeftover() < SceneManager::Instance().GetEnemyTotal())
			{
				SceneManager::Instance().SetBAddOrSubVal(true);
				SceneManager::Instance().SetBPlayerWin();
			}
			else
			{
				SceneManager::Instance().SetBAddOrSubVal(false);
				SceneManager::Instance().SetPointAddOrSubVal(500);
			}
		}

		SceneManager::Instance().SetNextScene(SceneManager::SceneType::result);
	}

	if (SceneManager::Instance().GetEnemyIeftover() == 0)
	{
		SceneManager::Instance().SetBAddOrSubVal(true);
		SceneManager::Instance().SetBPlayerWin();
		SceneManager::Instance().SetNextScene(SceneManager::SceneType::result);


		if (SceneManager::Instance().GetEnemyTotal() == 100)
		{
			SceneManager::Instance().SetPointAddOrSubVal(6000);
		}
		else if (SceneManager::Instance().GetEnemyTotal() == 50)
		{
			SceneManager::Instance().SetPointAddOrSubVal(3000);
		}
		else if (SceneManager::Instance().GetEnemyTotal() == 1)
		{
			SceneManager::Instance().SetPointAddOrSubVal(500);
		}
		else if (SceneManager::Instance().GetEnemyTotal() == 2)
		{
			SceneManager::Instance().SetPointAddOrSubVal(1000);
		}
		else if (SceneManager::Instance().GetEnemyTotal() == 3)
		{
			SceneManager::Instance().SetPointAddOrSubVal(1500);
		}
	}

	if (SceneManager::Instance().GetScreenVibFrames() > 0)
	{
		SceneManager::Instance().SubScreenVibFrames();
	}

	if (SceneManager::Instance().GetSceneType() == SceneManager::SceneType::challenge)
	{
		if (SceneManager::Instance().GetEnemyIeftover() >= 5)
		{
			if (SceneManager::Instance().GetEnemyDrawTotal() < 5)
			{
				std::shared_ptr<Enemy> enemy;
				enemy = std::make_shared<Enemy>();
				enemy->SetTarget(m_wpPlayer.lock());
				m_wpPlayer.lock()->AddEnemy(enemy);
				m_wpPlayer.lock()->AddWeaponToEnemy(enemy);
				enemy->Init();
				enemy->SetBBoss(true);
				m_objList.push_back(enemy);
				m_wpUi.lock()->AddEnemy(enemy);
				SceneManager::Instance().AddEnemyDrawTotal();
			}
		}
	}
	else if (SceneManager::Instance().GetSceneType() == SceneManager::SceneType::battle && !m_bCountDown)
	{
		if (!KdEffekseerManager::GetInstance().IsPlaying("EnemyAppearance.efk"))
		{
			m_appearanceEffectCnt = 0;
			m_bAppearanceEffect = false;
		}

		if (SceneManager::Instance().GetEnemyDrawTotal() == 0 && m_waveCnt < 2)
		{

			if (!m_bAppearanceEffect)
			{
				m_appearanceEffectCnt = 0;
				m_bAppearanceEffect = true;

				KdEffekseerManager::GetInstance().
					Play("EnemyAppearance.efk", { 0,1,20.0f });
				KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("EnemyAppearance.efk"); // これでループしない


				KdEffekseerManager::GetInstance().
					Play("EnemyAppearance.efk", { 0,1,-30.0f });
				KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("EnemyAppearance.efk"); // これでループしない


				KdEffekseerManager::GetInstance().
					Play("EnemyAppearance.efk", { 20,1,0.0f });
				KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("EnemyAppearance.efk"); // これでループしない


				KdEffekseerManager::GetInstance().
					Play("EnemyAppearance.efk", { -20,1,0.0f });
				KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("EnemyAppearance.efk"); // これでループしない

			}

			if (m_bAppearanceEffect)
			{
				++m_appearanceEffectCnt;
			}

			if (m_appearanceEffectCnt >= 25)
			{
				std::shared_ptr<Enemy> enemy;
				int total = 2;
				m_waveCnt++;
				m_wpUi.lock()->SetWaveCnt(m_waveCnt);

				if (m_waveCnt >= 2)
				{
					m_wpUi.lock()->SetBWaveChange();
				}

				// 前方向
				for (int i = 0; i < total; ++i)
				{
					SceneManager::Instance().AddEnemyDrawTotal();
					enemy = std::make_shared<Enemy>();
					enemy->SetTarget(m_wpPlayer.lock());
					m_wpPlayer.lock()->AddEnemy(enemy);
					m_wpPlayer.lock()->AddWeaponToEnemy(enemy);
					m_wpUi.lock()->AddEnemy(enemy);
					enemy->Init();
					enemy->SetEnemyNumber(i + 1);
					enemy->SetBBoss(false);

					enemy->SetPos(Math::Vector3(-0.5f + 1.0f * i, 0.0f, 20.0f));
					enemy->SetMatrix(Math::Vector3(-10.0f + 5.0f * i, 0.0f, 20.0f));
					enemy->SetWorldRotationY(DirectX::XMConvertToRadians(180));

					m_objList.push_back(enemy);
				}

				// 後ろ方向
				for (int i = 0; i < total; ++i)
				{
					SceneManager::Instance().AddEnemyDrawTotal();
					enemy = std::make_shared<Enemy>();
					enemy->SetTarget(m_wpPlayer.lock());
					m_wpPlayer.lock()->AddEnemy(enemy);
					m_wpPlayer.lock()->AddWeaponToEnemy(enemy);
					m_wpUi.lock()->AddEnemy(enemy);
					enemy->Init();
					enemy->SetEnemyNumber(i + 1);
					enemy->SetBBoss(false);

					enemy->SetPos(Math::Vector3(-0.5f + 1.0f * i, 0.0f, -30.0f));
					enemy->SetMatrix(Math::Vector3(-10.0f + 5.0f * i, 0.0f, 20.0f));
					enemy->SetWorldRotationY(DirectX::XMConvertToRadians(0));

					m_objList.push_back(enemy);
				}

				// 右方向
				for (int i = 0; i < total; ++i)
				{
					SceneManager::Instance().AddEnemyDrawTotal();
					enemy = std::make_shared<Enemy>();
					enemy->SetTarget(m_wpPlayer.lock());
					m_wpPlayer.lock()->AddEnemy(enemy);
					m_wpPlayer.lock()->AddWeaponToEnemy(enemy);
					m_wpUi.lock()->AddEnemy(enemy);
					enemy->Init();
					enemy->SetEnemyNumber(i + 1);
					enemy->SetBBoss(false);

					enemy->SetPos(Math::Vector3(20, 0.0f, -0.5f + 1.0f * i));
					enemy->SetMatrix(Math::Vector3(20, 0.0f, -0.5f + 1.0f * i));
					enemy->SetWorldRotationY(DirectX::XMConvertToRadians(270));

					m_objList.push_back(enemy);
				}

				// 左方向
				for (int i = 0; i < total; ++i)
				{
					SceneManager::Instance().AddEnemyDrawTotal();
					enemy = std::make_shared<Enemy>();
					enemy->SetTarget(m_wpPlayer.lock());
					m_wpPlayer.lock()->AddEnemy(enemy);
					m_wpPlayer.lock()->AddWeaponToEnemy(enemy);
					m_wpUi.lock()->AddEnemy(enemy);
					enemy->Init();
					enemy->SetEnemyNumber(i + 1);
					enemy->SetBBoss(false);

					enemy->SetPos(Math::Vector3(-20.0f, 0.0f, -0.5f + 1.0f * i));
					enemy->SetMatrix(Math::Vector3(-20.0f, 0.0f, -0.5f + 1.0f * i));
					enemy->SetWorldRotationY(DirectX::XMConvertToRadians(90));

					m_objList.push_back(enemy);
				}
			}
		}
		else if (SceneManager::Instance().GetEnemyDrawTotal() == 0 && m_waveCnt >= 2 && m_bossAppearanceCnt < SceneManager::Instance().GetEnemyTotal())
		{
			if (!m_bAppearanceEffect)
			{
				m_appearanceEffectCnt = 0;
				m_bAppearanceEffect = true;

				KdEffekseerManager::GetInstance().
					Play("EnemyAppearance.efk", { 0, 10.0f, 30.0f });
				KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("EnemyAppearance.efk"); // これでループしない

				Math::Matrix efcMat = Math::Matrix::CreateScale(1.5f) * Math::Matrix::CreateTranslation({ 0, 10.0f, 30.0f });
				KdEffekseerManager::GetInstance().SetWorldMatrix("EnemyAppearance.efk", efcMat);

			}

			if (m_bAppearanceEffect)
			{
				++m_appearanceEffectCnt;
			}

			if (m_appearanceEffectCnt >= 25)
			{
				int total = SceneManager::Instance().GetEnemyTotal();
				std::shared_ptr<Enemy> enemy;
				// 前方向
				for (int i = 0; i < total; ++i)
				{
					m_bossAppearanceCnt++;
					SceneManager::Instance().AddEnemyDrawTotal();
					enemy = std::make_shared<Enemy>();
					enemy->SetTarget(m_wpPlayer.lock());
					m_wpPlayer.lock()->AddEnemy(enemy);
					m_wpPlayer.lock()->AddWeaponToEnemy(enemy);
					m_wpUi.lock()->AddEnemy(enemy);
					enemy->Init();
					enemy->SetEnemyNumber(1);
					enemy->SetBBoss(true);
					switch (total)
					{
					case 1:
						enemy->SetPos(Math::Vector3(0, 10.0f, 30.0f));
						enemy->SetMatrix(Math::Vector3(0, 10.0f, 30.0f));
						break;
					case 2:
						enemy->SetPos(Math::Vector3(-3.0f + 6.0f * i, 10.0f, 30.0f));
						enemy->SetMatrix(Math::Vector3(0, 10.0f, 30.0f));
						break;
					case 3:
						enemy->SetPos(Math::Vector3(-3.0f + 3.0f * i, 10.0f, 30.0f));
						enemy->SetMatrix(Math::Vector3(0, 10.0f, 30.0f));
						break;
					}
					m_objList.push_back(enemy);

				}
			}
		}
	}
}

void GameScene::Init()
{
	m_waveCnt = 0;
	m_bossAppearanceCnt = 0;
	m_appearanceEffectCnt = 0;
	m_bAppearanceEffect = false;

	KdAudioManager::Instance().StopAllSound();

	KdInputCollector* gamepadCollector = new KdInputCollector();

	//	KdInputButtonForWindows* pForward = new KdInputButtonForWindows(VK_GAMEPAD_LEFT_THUMBSTICK_UP);
	KdInputButtonForWindows* pForward = new KdInputButtonForWindows({ 'W' , VK_GAMEPAD_LEFT_THUMBSTICK_UP });
	KdInputButtonForWindows* pLeft = new KdInputButtonForWindows({ 'A' , VK_GAMEPAD_LEFT_THUMBSTICK_LEFT });
	KdInputButtonForWindows* pBackWard = new KdInputButtonForWindows({ 'S' , VK_GAMEPAD_LEFT_THUMBSTICK_DOWN });
	KdInputButtonForWindows* pRight = new KdInputButtonForWindows({ 'D' ,VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT });
	KdInputButtonForWindows* pLAttack = new KdInputButtonForWindows(VK_LBUTTON);
	KdInputButtonForWindows* pRAttack = new KdInputButtonForWindows(VK_RBUTTON);
	KdInputButtonForWindows* pJump = new KdInputButtonForWindows(VK_SPACE);

	//gamepadCollector->AddButton("right", pRight);
	//gamepadCollector->AddButton("left", pLeft);
	gamepadCollector->AddButton("forward", pForward);
	gamepadCollector->AddButton("left", pLeft);
	gamepadCollector->AddButton("backward", pBackWard);
	gamepadCollector->AddButton("right", pRight);

	KdInputAxisForWindows* pMove = new KdInputAxisForWindows(gamepadCollector->GetButton("forward"), gamepadCollector->GetButton("right"),
		gamepadCollector->GetButton("backward"), gamepadCollector->GetButton("left"));

	gamepadCollector->AddAxis("move", pMove);

	gamepadCollector->AddButton("rAttack", pRAttack);
	gamepadCollector->AddButton("lAttack", pLAttack);
	gamepadCollector->AddButton("jump", pJump);

	KdInputManager::Instance().AddDevice("GamePad", gamepadCollector);

	std::shared_ptr<Sky> sky;
	sky = std::make_shared<Sky>();
	m_objList.push_back(sky);

	std::shared_ptr<GameCamera> camera = std::make_shared<GameCamera>();
	std::shared_ptr<Ground> ground;
	ground = std::make_shared<Ground>();
	m_objList.push_back(ground);
	camera->SetHitObj(ground);

	std::shared_ptr<Bldg> bldg;
	std::ifstream ifs("Asset/Data/BldgInfoBackUp.csv");
	std::string str, filed;
	while (getline(ifs, str))
	{
		bldg = std::make_shared<Bldg>();
		std::istringstream ss(str);
		int i = 0;
		std::string aStr[8];

		while (getline(ss, filed, ','))
		{
			aStr[i] = filed;
			i++;
		}

		if (i >= 7)
		{
			bldg->CreateBldg(aStr[0], Math::Vector3(stof(aStr[1]), stof(aStr[2]), stof(aStr[3])), aStr[4], Math::Vector3(stof(aStr[5]), stof(aStr[6]), stof(aStr[7])));
		}
		else
		{
			bldg->CreateBldg(aStr[0], Math::Vector3(stof(aStr[1]), stof(aStr[2]), stof(aStr[3])), aStr[4]);
		}
		m_objList.push_back(bldg);

		camera->SetHitObj(bldg);
	}

	std::shared_ptr<InvisibleWall> invisibleWall;
	invisibleWall = std::make_shared<InvisibleWall>();
	m_objList.push_back(invisibleWall);

	std::shared_ptr<Player> player;
	player = std::make_shared<Player>();
	m_objList.push_back(player);
	m_wpPlayer = player;

	std::shared_ptr<Ui> ui = std::make_shared<Ui>();
	ui->SetPlayer(player);

	ui->SetPlayer(player);
	ui->SetUiType(Ui::UiType::game);
	ui->SetWaveCnt(m_waveCnt);
	m_objList.push_back(ui);
	m_wpUi = ui;

	player->Init();


	camera->SetTarget(player);
	camera->SetPlayer(player);
	player->SetCamera(camera);
	ui->SetCamera(camera);
	m_objList.push_back(camera);

	KdEffekseerManager::GetInstance().SetCamera(camera);


	SetCursorPos(640, 360);
	KdAudioManager::Instance().Play("Asset/Audio/SE/CntDwon1.wav");

	ShowCursor(false); // マウスカーソルを消す
}