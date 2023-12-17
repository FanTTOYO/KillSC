#include "TrainingScene.h"
#include "../../Object/Character/Player/Player.h"
#include "../../Object/Character/Enemy/Enemy.h"
#include "../../Object/invisibleWall/InvisibleWall.h"
#include "../../Object/Sky/Sky.h"
#include "../../Object/Ground/Ground.h"
#include "../../Object/Bldg/Bldg.h"
#include "../../Camera/GameCamera/GameCamera.h"
#include "../SceneManager.h"
#include "../../Object/Ui/Ui.h"

void TrainingScene::Event()
{
	if (m_wpUi.lock()->GetTime() >= 0 && m_wpUi.lock()->GetTime() < 240 || m_wpUi.lock()->GetBOption())
	{
		if (!m_bCountDown)
		{
			m_bCountDown = true;
		}
	}
	else
	{
		m_bCountDown = false;
	}

	if (m_bCountDown)
	{
		m_wpUi.lock()->Update();
	}

	if (SceneManager::Instance().GetEnemyDrawTotal() == 0)
	{
		std::shared_ptr<Enemy> enemy;
		enemy = std::make_shared<Enemy>();
		enemy->SetTarget(m_wpPlayer.lock());
		m_wpPlayer.lock()->AddEnemy(enemy);
		m_wpPlayer.lock()->AddWeaponToEnemy(enemy);
		enemy->Init();
		enemy->SetBBoss(true);
		m_wpUi.lock()->AddEnemy(enemy);
		enemy->SetEnemyNumber(kOne);
		m_objList.push_back(enemy);
		SceneManager::Instance().AddEnemyDrawTotal();
		SceneManager::Instance().SetEnemyIeftover(kOne);
		m_wpEnemy = enemy;
	}

	if (m_wpPlayer.expired())
	{
		std::shared_ptr<json11::Json> jsonObj = std::make_shared<json11::Json>();
		{
			// json�t�@�C�����J��
			std::ifstream ifs("Asset/Data/objectVal.json");
			if (ifs.fail()) { assert(0 && "Json �t�@�C���̃p�X���Ԉ���Ă��܂��I�I�I"); };

			// ������Ƃ��đS�ǂݍ���
			std::string strJson((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

			std::string err;
			*jsonObj = json11::Json::parse(strJson, err);

			if (err.size() > 0) { assert(0 && "�ǂݍ��񂾃t�@�C����json�ϊ��Ɏ��s"); };
		}

		std::shared_ptr<Player> player;
		player = std::make_shared<Player>();
		player->AddEnemy(m_wpEnemy.lock());
		m_wpGameCamera.lock()->SetTarget(player);
		m_wpGameCamera.lock()->SetPlayer(player);
		player->SetCamera(m_wpGameCamera.lock());
		m_wpUi.lock()->SetPlayer(player);
		player->SetUi(m_wpUi.lock());
		m_objList.push_back(player);
		player->Init(jsonObj);
		m_wpPlayer = player;
		m_wpEnemy.lock()->SetTarget(player);
		m_wpEnemy.lock()->SetWeaponToTarget(player);
	}
}

void TrainingScene::Init()
{
	std::shared_ptr<json11::Json> jsonObj = std::make_shared<json11::Json>();
	{
		// json�t�@�C�����J��
		std::ifstream ifs("Asset/Data/objectVal.json");
		if (ifs.fail()) { assert(0 && "Json �t�@�C���̃p�X���Ԉ���Ă��܂��I�I�I"); };

		// ������Ƃ��đS�ǂݍ���
		std::string strJson((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

		std::string err;
		*jsonObj = json11::Json::parse(strJson, err);

		if (err.size() > 0) { assert(0 && "�ǂݍ��񂾃t�@�C����json�ϊ��Ɏ��s"); };
	}

	KdAudioManager::Instance().StopAllSound();

	std::shared_ptr<Sky> sky;
	sky = std::make_shared<Sky>();
	m_objList.push_back(sky);

	std::shared_ptr<GameCamera> camera = std::make_shared<GameCamera>();

	std::shared_ptr<InvisibleWall> invisibleWall;
	invisibleWall = std::make_shared<InvisibleWall>();
	m_objList.push_back(invisibleWall);
	camera->SetHitObj(invisibleWall);

	std::shared_ptr<Player> player;
	player = std::make_shared<Player>();
	m_objList.push_back(player);
	m_wpPlayer = player;

	std::shared_ptr<Enemy> enemy;
	enemy = std::make_shared<Enemy>();
	enemy->SetTarget(player);
	player->AddEnemy(enemy);
	enemy->SetPos(Math::Vector3(0, 0.0f, 20.0f));
	enemy->SetWorldRotationY(180);
	enemy->SetMatrix();
	enemy->Init();
	enemy->SetBBoss(true);
	m_objList.push_back(enemy);
	SceneManager::Instance().AddEnemyDrawTotal();
	SceneManager::Instance().SetEnemyIeftover(kOne);
	m_wpEnemy = enemy;

	std::shared_ptr<Ui> ui = std::make_shared<Ui>();
	ui->SetPlayer(player);

	ui->SetPlayer(player);
	ui->AddEnemy(enemy);
	ui->SetUiType(Ui::UiType::training);
	m_objList.push_back(ui);
	m_wpUi = ui;

	player->Init(jsonObj);

	camera->SetTarget(player);
	camera->SetPlayer(player);
	player->SetCamera(camera);
	ui->SetCamera(camera);
	m_objList.push_back(camera);

	KdEffekseerManager::GetInstance().SetCamera(camera);
	m_wpGameCamera = camera;


	SetCursorPos(640, 360);
	KdAudioManager::Instance().Play("Asset/Audio/SE/CntDwon1.wav");
}