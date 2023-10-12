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
		m_wpUi.lock()->GetGameTimeM1()  == 0 &&
		m_wpUi.lock()->GetGameTimeS10() == 0 &&
		m_wpUi.lock()->GetGameTimeS1()  == 0)
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
				m_objList.push_back(enemy);
				m_wpUi.lock()->AddEnemy(enemy);
				SceneManager::Instance().AddEnemyDrawTotal();
			}
		}
	}
}

void GameScene::Init()
{
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

	std::shared_ptr<Ground> ground;
	ground = std::make_shared<Ground>();
	m_objList.push_back(ground);

	std::shared_ptr<Bldg> bldg;
	std::ifstream ifs("Asset/Data/BldgInfo.csv");
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
	}

	std::shared_ptr<InvisibleWall> invisibleWall;
	invisibleWall = std::make_shared<InvisibleWall>();
	m_objList.push_back(invisibleWall);

	std::shared_ptr<Player> player;
	player = std::make_shared<Player>();
	m_objList.push_back(player);
	m_wpPlayer = player;


	std::shared_ptr<Enemy> enemy;
	std::shared_ptr<Ui> ui = std::make_shared<Ui>();
	ui->SetPlayer(player);
	
	int total = SceneManager::Instance().GetEnemyTotal();
	if (total >= 5)
	{
		total = 5;
	}

	SceneManager::Instance().SetEnemyDrawTotal(total);

	for (int i = 0; i < total; ++i)
	{
		enemy = std::make_shared<Enemy>();
		enemy->SetTarget(player);
		player->AddEnemy(enemy);
		enemy->Init();
		switch (total)
		{
		case 1:
			enemy->SetPos(Math::Vector3(0, 0.0f, 20.0f));
			break;
		case 2:
			enemy->SetPos(Math::Vector3(-5.0f + 10.0f * i, 0.0f, 20.0f));
			break;
		case 3:
			enemy->SetPos(Math::Vector3(-5.0f + 5.0f * i, 0.0f, 20.0f));
			break;
		case 4:
			enemy->SetPos(Math::Vector3(-10.0f + 5.0f * i, 0.0f, 20.0f));
			break;
		case 5:
			enemy->SetPos(Math::Vector3(-10.0f + 5.0f * i, 0.0f, 20.0f));
			break;
		}

		m_objList.push_back(enemy);
		ui->AddEnemy(enemy);
	}

	ui->SetPlayer(player);
	ui->SetUiType(Ui::UiType::game);
	m_objList.push_back(ui);
	m_wpUi = ui;

	player->Init();

	std::shared_ptr<GameCamera> camera = std::make_shared<GameCamera>();
	camera->SetTarget(player);
	camera->SetPlayer(player);
	player->SetCamera(camera);
	ui->SetCamera(camera);
	m_objList.push_back(camera);


	SetCursorPos(640, 360);
	KdAudioManager::Instance().Play("Asset/Audio/SE/CntDwon1.wav");

	ShowCursor(false); // マウスカーソルを消す
}