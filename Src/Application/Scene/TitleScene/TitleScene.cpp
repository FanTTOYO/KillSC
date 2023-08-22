#include "TitleScene.h"
#include "../SceneManager.h"
#include "../../Object/Ui/Ui.h"

void TitleScene::Event()
{
	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
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
	std::shared_ptr<Ui> ui = std::make_shared<Ui>();
	ui->SetUiType(Ui::UiType::title);
	m_objList.push_back(ui);
	m_wpUi = ui;
	m_bReturn = false;
	KdAudioManager::Instance().StopAllSound();
}
