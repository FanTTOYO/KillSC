#include "SelectScene.h"
#include "../SceneManager.h"
#include "../../Object/Ui/Ui.h"

void SelectScene::Event()
{
	/*if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		if (m_spUi->GetBSceneCangePossible())
		{
			m_spUi->SetAddFadeAlpha();
		}
	}*/
}

void SelectScene::Init()
{
	std::shared_ptr<Ui> ui = std::make_shared<Ui>();
	ui->SetUiType(Ui::UiType::select);
	m_objList.push_back(ui);
	m_wpUi = ui;

	ShowCursor(true); // マウスカーソルを消す
}
