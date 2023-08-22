#include "ArtScene.h"
#include "../SceneManager.h"
#include "../../Object/Ui/Ui.h"

void ArtScene::Event()
{
	/*if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		if (m_spUi->GetBSceneCangePossible())
		{
			m_spUi->SetAddFadeAlpha();
		}
	}*/
}

void ArtScene::Init()
{
	std::shared_ptr<Ui> ui = std::make_shared<Ui>();
	ui->SetUiType(Ui::UiType::art);
	m_objList.push_back(ui);
	m_wpUi = ui;
}
