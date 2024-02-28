#include "ResultScene.h"
#include "../SceneManager.h"
#include "../../Object/Ui/Ui.h"

void ResultScene::Event()
{
	if (m_wpUi.lock()->GetTime() == 1)
	{
		if (SceneManager::Instance().GetBPlayerWin())
		{
			KdAudioManager::Instance().Play("Asset/Audio/BGM/7 Fanfare and Victory Lap Loop.wav", true);
		}
		else
		{
			KdAudioManager::Instance().Play("Asset/Audio/BGM/8 Defeat loop.wav", true);
		}
	}
}

void ResultScene::Init()
{
	m_spJsonObj = std::make_shared<json11::Json>();
	{
		// jsonファイルを開く
		std::ifstream ifs("Asset/Data/objectVal.json");
		if (ifs.fail()) { assert(0 && "Json ファイルのパスが間違っています！！！"); };

		// 文字列として全読み込み
		std::string strJson((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

		std::string err;
		*m_spJsonObj = json11::Json::parse(strJson, err);

		if (err.size() > 0) { assert(0 && "読み込んだファイルのjson変換に失敗"); };
	}

	std::shared_ptr<Ui> ui = std::make_shared<Ui>();
	ui->SetUiType(Ui::UiType::result);
	ui->Init(m_spJsonObj);
	m_objList.push_back(ui);
	m_wpUi = ui;
	KdAudioManager::Instance().StopAllSound();
}
