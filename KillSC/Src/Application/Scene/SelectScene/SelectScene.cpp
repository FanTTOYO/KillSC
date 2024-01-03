#include "SelectScene.h"
#include "../SceneManager.h"
#include "../../Object/Ui/Ui.h"

void SelectScene::Event()
{
	
}

void SelectScene::Init()
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
	ui->SetUiType(Ui::UiType::select, m_spJsonObj);
	ui->Init();
	m_objList.push_back(ui);
	m_wpUi = ui;
}
