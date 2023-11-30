#include "ResultScene.h"
#include "../SceneManager.h"
#include "../../Object/Ui/Ui.h"

void ResultScene::Event()
{
	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		if (m_wpUi.lock()->GetBSceneCangePossible())
		{
			m_wpUi.lock()->SetAddFadeAlpha();
		}
	}

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
	std::shared_ptr<Ui> ui = std::make_shared<Ui>();
	ui->SetUiType(Ui::UiType::result);
	m_objList.push_back(ui);
	m_wpUi = ui;
	KdAudioManager::Instance().StopAllSound();
}
