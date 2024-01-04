#include "TitleScene.h"
#include "../SceneManager.h"
#include "../../Object/Ui/Ui.h"

void TitleScene::Event()
{
	if (KdInputManager::Instance().IsPress("select"))
	{
		if(!m_bReturn)
		{
			if (m_wpUi.lock()->GetBSceneCangePossible())
			{
				m_bReturn = true;
				m_wpUi.lock()->SetAddFadeAlpha();
				KdAudioManager::Instance().Play("Asset/Audio/SE/start.wav");
			}
		}
	}

	if (m_wpUi.lock()->GetTime() == 360)
	{
		KdAudioManager::Instance().Play("Asset/Audio/BGM/1 Fated Battle loop.wav",true);
	}
}

void TitleScene::Init()
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
	ui->SetUiType(Ui::UiType::title);
	ui->Init(m_spJsonObj);
	m_objList.push_back(ui);
	m_wpUi = ui;
	m_bReturn = false;
	KdAudioManager::Instance().StopAllSound();
}
