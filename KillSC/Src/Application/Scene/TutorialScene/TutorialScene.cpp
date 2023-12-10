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
	// jsonファイルを開く
	std::ifstream ifs("Asset/Data/objectVal.json");
	if (ifs.fail()) { assert(0 && "Json ファイルのパスが間違っています！！！"); };

	// 文字列として全読み込み
	std::string strJson((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

	std::string err;
	std::shared_ptr<json11::Json> jsonObj = std::make_shared<json11::Json>();
	*jsonObj = json11::Json::parse(strJson, err);

	if (err.size() > 0) { assert(0 && "読み込んだファイルのjson変換に失敗"); };

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
	player->Init(jsonObj);
	enemy->Init();

	std::shared_ptr<GameCamera> camera = std::make_shared<GameCamera>();
	camera->SetTarget(player);
	camera->SetPlayer(player);
	player->SetCamera(camera);
	m_objList.push_back(camera);

	std::shared_ptr<Ui> ui = std::make_shared<Ui>();
	ui->SetPlayer(player);
	ui->SetUiType(Ui::UiType::tutorial);
	m_objList.push_back(ui);
	m_wpUi = ui;
	player->SetUi(ui);

	SetCursorPos(640, 360);
}
