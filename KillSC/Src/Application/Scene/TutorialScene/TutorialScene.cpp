#include "TutorialScene.h"
#include "../../Object/Character/Player/Player.h"
#include "../../Object/Character/Enemy/Enemy.h"
#include "../../Object/invisibleWall/InvisibleWall.h"
#include "../../Object/Sky/Sky.h"
#include "../../Object/Ground/Ground.h"
#include "../../Object/Bldg/Bldg.h"
#include "../../Camera/GameCamera/GameCamera.h"
#include "../SceneManager.h"
#include "../../Object/Ui/Ui.h"

void TutorialScene::Event()
{
	if (m_wpUi.lock()->GetBOption() || m_wpUi.lock()->GetBTutorial())
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
}

void TutorialScene::Init()
{
	m_spJsonObj = std::make_shared<json11::Json>();
	{
		// json�t�@�C�����J��
		std::ifstream ifs("Asset/Data/objectVal.json");
		if (ifs.fail()) { assert(0 && "Json �t�@�C���̃p�X���Ԉ���Ă��܂��I�I�I"); };

		// ������Ƃ��đS�ǂݍ���
		std::string strJson((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

		std::string err;
		*m_spJsonObj = json11::Json::parse(strJson, err);

		if (err.size() > 0) { assert(0 && "�ǂݍ��񂾃t�@�C����json�ϊ��Ɏ��s"); };
	}

	KdAudioManager::Instance().StopAllSound();

	std::shared_ptr<InvisibleWall> invisibleWall;
	invisibleWall = std::make_shared<InvisibleWall>();
	m_objList.push_back(invisibleWall);

	std::shared_ptr<Enemy> enemy;
	enemy = std::make_shared<Enemy>();
	m_objList.push_back(enemy);

	std::shared_ptr<Player> player;
	player = std::make_shared<Player>();
	m_objList.push_back(player);
	m_wpPlayer = player;
	enemy->SetTarget(player);
	player->AddEnemy(enemy);
	player->Init(m_spJsonObj);
	enemy->Init(m_spJsonObj);

	std::shared_ptr<GameCamera> camera = std::make_shared<GameCamera>();
	camera->SetTarget(player);
	camera->SetPlayer(player);
	player->SetCamera(camera);
	camera->SetHitObj(invisibleWall);
	m_objList.push_back(camera);
	KdEffekseerManager::GetInstance().SetCamera(camera);

	std::shared_ptr<Ui> ui = std::make_shared<Ui>();
	ui->SetPlayer(player);
	player->SetUi(ui);
	ui->SetUiType(Ui::UiType::tutorial);
	m_objList.push_back(ui);
	m_wpUi = ui;

	SetCursorPos(640, 360);
}
