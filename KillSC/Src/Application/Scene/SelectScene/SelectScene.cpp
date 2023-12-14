#include "SelectScene.h"
#include "../SceneManager.h"
#include "../../Object/Ui/Ui.h"

void SelectScene::Event()
{
	
}

void SelectScene::Init()
{
	std::shared_ptr<Ui> ui = std::make_shared<Ui>();
	ui->SetUiType(Ui::UiType::select);
	m_objList.push_back(ui);
	m_wpUi = ui;
}
