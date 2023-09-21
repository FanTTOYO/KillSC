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
	ShowCursor(true); // マウスカーソルを消す
}
