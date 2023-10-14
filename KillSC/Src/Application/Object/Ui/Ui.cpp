#include "Ui.h"
#include "../../main.h"
#include "../../Scene/SceneManager.h"
#include "../Character/Player/Player.h"
#include "../Character/Enemy/Enemy.h"
#include "../../Camera/GameCamera/GameCamera.h"

void Ui::Update()
{
	switch (m_uiType)
	{
	case UiType::title:
		TitleUpdate();
		break;
	case UiType::game:
		GameUpdate();
		break;
	case UiType::tutorial:
		TutorialUpdate();
		break;
	case UiType::result:
		ResultUpdate();
		break;
	case UiType::select:
		SelectUpdate();
		break;
	}
}

void Ui::PostUpdate()
{
	if (m_uiType == UiType::game)
	{
		int i = 0;
		for (auto& list : m_wpEnemyList)
		{
			Math::Vector3 pos = Math::Vector3(list.lock()->GetPos().x, list.lock()->GetPos().y + 1.2f, list.lock()->GetPos().z);

			POINT dev;
			KdDirect3D::Instance().WorldToClient(pos, dev, m_wpCamera.lock()->WorkCamera()->GetCameraMatrix(), m_wpCamera.lock()->WorkCamera()->GetProjMatrix());
			dev.x -= (long)640.0f;
			dev.y = (long)(dev.y * -1 + 360.0f);
			float z = m_wpCamera.lock()->GetPos().z - list.lock()->GetPos().z;
			dev.y += (long)(180 - (std::fabs(z) * 1.5f));
			dev.x -= 50;
			m_enemyScPosList[i] = Math::Vector2((float)dev.x,(float)dev.y);
			++i;
		}
	}
}

void Ui::TitleUpdate()
{
	m_time++;
	if (m_time >= 480)
	{
		m_time = 480;
		m_bSceneCangePossible = true;

		if (m_bPushToEnterAlphaAdd)
		{
			m_pushToEnterAlpha += (0.8f / 30);
			if (m_pushToEnterAlpha >= 1.0f)
			{
				m_pushToEnterAlpha = 1.0f;
				m_bPushToEnterAlphaAdd = false;
			}
		}
		else
		{
			m_pushToEnterAlpha -= (0.8f / 30);
			if (m_pushToEnterAlpha <= 0.2f)
			{
				m_pushToEnterAlpha = 0.2f;
				m_bPushToEnterAlphaAdd = true;
			}
		}

		if (m_addFadeAlpha)
		{
			m_fadeAlpha += (1.0f / 60);
			if (m_fadeAlpha >= 1.0f)
			{
				m_fadeAlpha = 1.0f;

				if (m_uiType == UiType::title)
				{
					SceneManager::Instance().SetNextScene
					(
						SceneManager::SceneType::select
					);
				}
			}
		}
	}

	if (m_time >= 0 && m_time <= 120)
	{
		m_fadeAlpha -= (1.0f / 120);
		if (m_fadeAlpha <= 0.0f)
		{
			m_fadeAlpha = 0.0f;
		}
	}
	else if (m_time >= 240 && m_time < 360)
	{
		m_fadeAlpha += (1.0f / 120);
		if (m_fadeAlpha >= 1.0f)
		{
			m_fadeAlpha = 1.0f;
		}
	}
	else if (m_time >= 360 && m_time <= 480)
	{
		m_fadeAlpha -= (1.0f / 120);
		if (m_fadeAlpha <= 0.0f)
		{
			m_fadeAlpha = 0.0f;
		}
	}

	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		if (m_time < 360)
		{
			m_time = 360;
			m_fadeAlpha = 1.0f;
		}
	}
}

void Ui::GameUpdate()
{
	if (m_time == 0)
	{
		m_exitScale = 1.0f;
		m_titleScale = 1.0f;

		m_titlePos = { 0,0,0 };
		m_exitPos = { 0,-250,0 };
		m_fadeAlpha = 0.0f;

		if (SceneManager::Instance().GetSceneType() == SceneManager::SceneType::challenge)
		{
			m_gameTimeS10 = 0;
			m_gameTimeS1 = 0;

			m_gameTimeM10 = 1;
			m_gameTimeM1 = 0;
		}
		else
		{
			m_gameTimeS10 = 0;
			m_gameTimeS1 = 0;

			m_gameTimeM10 = 0;
			m_gameTimeM1 = 5;
		}
	}

	if (m_time <= 240)
	{
		m_time++;
	}

	if (m_time >= 0 && m_time < 30)
	{
		m_countThreeScale += (1.0f / 10.0f);
		if (m_countThreeScale > 1.0f)
		{
			m_countThreeScale = 1.0f;
		}

		m_countThreeAlpha += (1.0f / 30.0f);
		if (m_countThreeAlpha > 1.0f)
		{
			m_countThreeAlpha = 1.0f;
		}
	}
	else if (m_time >= 30 && m_time < 60)
	{
		/*m_countThreeScale += (1.0f / 10.0f);
		if (m_countThreeScale > 1.0f)
		{
			m_countThreeScale = 1.0f;
		}*/

		m_countThreeAlpha -= (1.0f / 30.0f);
		if (m_countThreeAlpha < 0.0f)
		{
			m_countThreeAlpha = 0.0f;
		}
	}
	else if (m_time >= 60 && m_time < 90)
	{
		m_countTwoScale += (1.0f / 10.0f);
		if (m_countTwoScale > 1.0f)
		{
			m_countTwoScale = 1.0f;
		}

		m_countTwoAlpha += (1.0f / 30.0f);
		if (m_countTwoAlpha > 1.0f)
		{
			m_countTwoAlpha = 1.0f;
		}
	}
	else if (m_time >= 90 && m_time < 120)
	{
		/*m_countThreeScale += (1.0f / 10.0f);
		if (m_countThreeScale > 1.0f)
		{
			m_countThreeScale = 1.0f;
		}*/

		m_countTwoAlpha -= (1.0f / 30.0f);
		if (m_countThreeAlpha < 0.0f)
		{
			m_countThreeAlpha = 0.0f;
		}
	}
	else if (m_time >= 120 && m_time < 150)
	{
		m_countOneScale += (1.0f / 10.0f);
		if (m_countOneScale > 1.0f)
		{
			m_countOneScale = 1.0f;
		}

		m_countOneAlpha += (1.0f / 30.0f);
		if (m_countOneAlpha > 1.0f)
		{
			m_countOneAlpha = 1.0f;
		}
	}
	else if (m_time >= 150 && m_time < 180)
	{
		/*m_countThreeScale += (1.0f / 10.0f);
		if (m_countThreeScale > 1.0f)
		{
			m_countThreeScale = 1.0f;
		}*/

		m_countOneAlpha -= (1.0f / 30.0f);
		if (m_countOneAlpha < 0.0f)
		{
			m_countOneAlpha = 0.0f;
		}
	}
	else if (m_time >= 180 && m_time < 210)
	{
		m_countGoScale += (1.0f / 10.0f);
		if (m_countGoScale > 1.0f)
		{
			m_countGoScale = 1.0f;
		}

		m_countGoAlpha += (1.0f / 30.0f);
		if (m_countGoAlpha > 1.0f)
		{
			m_countGoAlpha = 1.0f;
		}
	}
	else if (m_time >= 210 && m_time < 240)
	{
		/*m_countThreeScale += (1.0f / 10.0f);
		if (m_countThreeScale > 1.0f)
		{
			m_countThreeScale = 1.0f;
		}*/

		m_countGoAlpha -= (1.0f / 30.0f);
		if (m_countGoAlpha < 0.0f)
		{
			m_countGoAlpha = 0.0f;
			SetCursorPos(640, 360);
		}
	}

	if (m_time > 240)
	{
		PWINDOWINFO pwi = new WINDOWINFO;
		pwi->cbSize = sizeof(WINDOWINFO);
		GetWindowInfo(Application::Instance().GetWindowHandle(), pwi);
		//pwi->rcWindow.left;    // マウスとの当たり判定関係のX座標に足す
		//pwi->rcWindow.top + 35;// マウスとの当たり判定関係のこれをy座標に足す

		if (GetAsyncKeyState(VK_TAB) & 0x8000)
		{
			if (!m_bTABKey)
			{
				m_bTABKey = true;
				if (!m_bOption)
				{
					m_bOption = true;
					KdAudioManager::Instance().Play("Asset/Audio/SE/メニューを開く1.wav");
					ShowCursor(true); // マウスカーソルを消す
				}
				else if (m_bOption)
				{
					m_bOption = false;
					m_bWeaponDataPage = false;
					m_bWeaponDataHopperPage = false;
					m_bWeaponDataScoPage = true;
					m_bHowToPage = true;
					m_bInfo = false;
					SetCursorPos(640, 360);
					KdAudioManager::Instance().Play("Asset/Audio/SE/メニューを開く1.wav");
					ShowCursor(false); // マウスカーソルを消す
				}
			}
		}
		else
		{
			m_bTABKey = false;
		}

		if (m_bOption)
		{

			POINT mousePos;
			GetCursorPos(&mousePos);
			mousePos.x -= (long)640.0f;
			mousePos.y = (long)(mousePos.y * -1 + 360.0f);
			Math::Vector3 Dis;
			float mouseX = (float)mousePos.x/* + (float)(pwi->rcWindow.left)*/;
			float mouseY = (float)mousePos.y + (float)(pwi->rcWindow.top + 35);

			if (!m_bInfo)
			{
				Math::Vector3 ExitPos;
				ExitPos.x = m_exitPos.x + (float)(pwi->rcWindow.left);
				ExitPos.y = m_exitPos.y /*+ (float)(pwi->rcWindow.top)*/;
				ExitPos.z = m_exitPos.z;

				Dis = ExitPos - Math::Vector3(mouseX, mouseY, 0.0f);
				if (Dis.Length() <= 75)
				{
					m_exitScale = 1.5f;
					m_titleScale = 1.0f;

					if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
					{
						if (!m_addFadeAlpha)
						{
							m_bExit = true;
							m_addFadeAlpha = true;
							m_bFirstExit = true;
							KdAudioManager::Instance().Play("Asset/Audio/SE/各ボタンを押したときの音.wav");
						}
					}

				}
				else
				{
					m_exitScale = 1.0f;
				}

				Math::Vector3 TitlePos;
				TitlePos.x = m_titlePos.x + (float)(pwi->rcWindow.left);
				TitlePos.y = m_titlePos.y /*+ (float)(pwi->rcWindow.top)*/;
				TitlePos.z = m_titlePos.z;

				Dis = TitlePos - Math::Vector3(mouseX, mouseY, 0.0f);
				if (Dis.Length() <= 75)
				{
					m_exitScale = 1.0f;
					m_titleScale = 1.5f;

					if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
					{
						if (!m_addFadeAlpha)
						{
							m_bTitle = true;
							m_addFadeAlpha = true;
							KdAudioManager::Instance().Play("Asset/Audio/SE/各ボタンを押したときの音.wav");
						}
					}

				}
				else
				{
					m_titleScale = 1.0f;
				}

				Math::Vector3 InfoPos;
				InfoPos.x = m_infoPos.x + (float)(pwi->rcWindow.left);
				InfoPos.y = m_infoPos.y /*+ (float)(pwi->rcWindow.top)*/;
				InfoPos.z = m_infoPos.z;

				Dis = InfoPos - Math::Vector3(mouseX, mouseY, 0.0f);
				if (Dis.Length() <= 75)
				{
					m_exitScale = 1.0f;
					m_titleScale = 1.0f;
					m_infoScale = 1.5f;

					if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
					{
						m_bInfo = true;
						KdAudioManager::Instance().Play("Asset/Audio/SE/各ボタンを押したときの音.wav");
					}

				}
				else
				{
					m_infoScale = 1.0f;
				}

				Math::Vector3 BackPos;
				BackPos.x = m_backPos.x + (float)(pwi->rcWindow.left);
				BackPos.y = m_backPos.y /*+ (float)(pwi->rcWindow.top)*/;
				BackPos.z = m_backPos.z;
				Dis = BackPos - Math::Vector3(mouseX, mouseY, 0.0f);
				if (Dis.Length() <= 40)
				{
					m_backScale = 1.0f;
					if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
					{
						if (!bLButtonKey)
						{
							bLButtonKey = true;
							m_bOption = false;
							ShowCursor(false); // マウスカーソルを消す
							SetCursorPos(640, 360);
							KdAudioManager::Instance().Play("Asset/Audio/SE/backPush.wav");
						}
					}
					else
					{
						bLButtonKey = false;
					}

				}
				else
				{
					m_backScale = 0.8f;
				}

				if (m_addFadeAlpha)
				{
					m_fadeAlpha += (1.0f / 90);
					if (m_fadeAlpha >= 1.0f)
					{
						m_fadeAlpha = 1.0f;

						if (m_bTitle)
						{
							SceneManager::Instance().SetNextScene
							(
								SceneManager::SceneType::title
							);
						}

						if (m_bExit)
						{
							if (m_bFirstExit)
							{
								SceneManager::Instance().SetBWinEnd();
								m_bFirstExit = false;
							}
							else
							{
								m_bExit = false;
								m_addFadeAlpha = false;
								m_fadeAlpha = 0.0f;
							}
						}
					}
				}

			}
			else
			{

				Math::Vector3 BackPos;
				BackPos.x = m_backPos.x + (float)(pwi->rcWindow.left);
				BackPos.y = m_backPos.y /*+ (float)(pwi->rcWindow.top)*/;
				BackPos.z = m_backPos.z;
				Dis = BackPos - Math::Vector3(mouseX, mouseY, 0.0f);
				if (Dis.Length() <= 40)
				{
					m_backScale = 1.0f;
					if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
					{
						if (!bLButtonKey)
						{
							bLButtonKey = true;
							m_bWeaponDataPage = false;
							m_bWeaponDataHopperPage = false;
							m_bWeaponDataScoPage = true;
							m_bHowToPage = true;
							m_bInfo = false;
							KdAudioManager::Instance().Play("Asset/Audio/SE/backPush.wav");
						}
					}
					else
					{
						bLButtonKey = false;
					}


				}
				else
				{
					m_backScale = 0.8f;
				}

				Math::Vector3 WeaOrHowLeftYaiPos;
				WeaOrHowLeftYaiPos.x = m_weaOrHowLeftYaiPos.x + (float)(pwi->rcWindow.left);
				WeaOrHowLeftYaiPos.y = m_weaOrHowLeftYaiPos.y /*+ (float)(pwi->rcWindow.top)*/;
				WeaOrHowLeftYaiPos.z = m_weaOrHowLeftYaiPos.z;
				Dis = WeaOrHowLeftYaiPos - Math::Vector3(mouseX, mouseY, 0.0f);
				if (Dis.Length() <= 30)
				{
					m_weaOrHowLeftYaiScale = 1.5f;
					m_weaOrHowRightYaiScale = 1.0f;

					if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
					{
						if (!bLButtonKey)
						{
							bLButtonKey = true;
							if (m_bWeaponDataPage)
							{
								m_bHowToPage = true;
								m_bWeaponDataPage = false;
							}
							else
							{
								m_bHowToPage = false;
								m_bWeaponDataPage = true;
							}
							KdAudioManager::Instance().Play("Asset/Audio/SE/矢印押したときの音.wav");
						}
					}
					else
					{
						bLButtonKey = false;
					}

				}
				else
				{
					m_weaOrHowLeftYaiScale = 1.0f;
				}

				Math::Vector3 WeaOrHowRightYaiPos;
				WeaOrHowRightYaiPos.x = m_weaOrHowRightYaiPos.x + (float)(pwi->rcWindow.left);
				WeaOrHowRightYaiPos.y = m_weaOrHowRightYaiPos.y /*+ (float)(pwi->rcWindow.top)*/;
				WeaOrHowRightYaiPos.z = m_weaOrHowRightYaiPos.z;
				Dis = m_weaOrHowRightYaiPos - Math::Vector3(mouseX, mouseY, 0.0f);
				if (Dis.Length() <= 30)
				{
					m_weaOrHowLeftYaiScale = 1.0f;
					m_weaOrHowRightYaiScale = 1.5f;

					if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
					{
						if (!bLButtonKey)
						{
							bLButtonKey = true;
							if (m_bWeaponDataPage)
							{
								m_bHowToPage = true;
								m_bWeaponDataPage = false;
							}
							else
							{
								m_bHowToPage = false;
								m_bWeaponDataPage = true;
							}
							KdAudioManager::Instance().Play("Asset/Audio/SE/矢印押したときの音.wav");
						}
					}
					else
					{
						bLButtonKey = false;
					}

				}
				else
				{
					m_weaOrHowRightYaiScale = 1.0f;
				}

				if (m_bWeaponDataPage)
				{
					Math::Vector3 WeaponLeftYaiPos;
					WeaponLeftYaiPos.x = m_weaponLeftYaiPos.x + (float)(pwi->rcWindow.left);
					WeaponLeftYaiPos.y = m_weaponLeftYaiPos.y /*+ (float)(pwi->rcWindow.top)*/;
					WeaponLeftYaiPos.z = m_weaponLeftYaiPos.z;

					Dis = WeaponLeftYaiPos - Math::Vector3(mouseX, mouseY, 0.0f);
					if (Dis.Length() <= 30)
					{
						m_weaponLeftYaiScale = 1.5f;
						m_weaponRightYaiScale = 1.0f;

						if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
						{
							if (!bLButtonKey)
							{
								bLButtonKey = true;
								if (m_bWeaponDataHopperPage)
								{
									m_bWeaponDataScoPage = true;
									m_bWeaponDataHopperPage = false;
								}
								else
								{
									m_bWeaponDataScoPage = false;
									m_bWeaponDataHopperPage = true;
								}
								KdAudioManager::Instance().Play("Asset/Audio/SE/矢印押したときの音.wav");
							}
						}
						else
						{
							bLButtonKey = false;
						}

					}
					else
					{
						m_weaponLeftYaiScale = 1.0f;
					}

					Math::Vector3 WeaponRightYaiPos;
					WeaponRightYaiPos.x = m_weaponRightYaiPos.x + (float)(pwi->rcWindow.left);
					WeaponRightYaiPos.y = m_weaponRightYaiPos.y /*+ (float)(pwi->rcWindow.top)*/;
					WeaponRightYaiPos.z = m_weaponRightYaiPos.z;

					Dis = WeaponRightYaiPos - Math::Vector3(mouseX, mouseY, 0.0f);
					if (Dis.Length() <= 30)
					{
						m_weaponLeftYaiScale = 1.0f;
						m_weaponRightYaiScale = 1.5f;

						if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
						{
							if (!bLButtonKey)
							{
								bLButtonKey = true;
								if (m_bWeaponDataHopperPage)
								{
									m_bWeaponDataScoPage = true;
									m_bWeaponDataHopperPage = false;
								}
								else
								{
									m_bWeaponDataScoPage = false;
									m_bWeaponDataHopperPage = true;
								}
								KdAudioManager::Instance().Play("Asset/Audio/SE/矢印押したときの音.wav");
							}
						}
						else
						{
							bLButtonKey = false;
						}

					}
					else
					{
						m_weaponRightYaiScale = 1.0f;
					}
				}
			}
		}
		else
		{
			if (m_gameTimeCntDeray >= 60)
			{
				m_gameTimeCntDeray = 0;
				if (m_gameTimeS1 == 0)
				{
					if (m_gameTimeS10 == 0)
					{
						if (m_gameTimeM1 == 0)
						{
							if (m_gameTimeM10 != 0)
							{
								m_gameTimeM10--;
								m_gameTimeM1 = 9;
								m_gameTimeS10 = 5;
								m_gameTimeS1 = 9;
							}
						}
						else
						{
							m_gameTimeM1--;
							m_gameTimeS10 = 5;
							m_gameTimeS1 = 9;
						}
					}
					else
					{
						m_gameTimeS10--;
						m_gameTimeS1 = 9;
					}
				}
				else
				{
					m_gameTimeS1--;
				}
			}
			else
			{
				++m_gameTimeCntDeray;
			}
		}

		KdSafeDelete(pwi);
	}
}

void Ui::TutorialUpdate()
{
	if (m_time == 0)
	{
		m_exitScale = 1.0f;
		m_titleScale = 1.0f;

		m_titlePos = { 0,0,0 };
		m_exitPos = { 0,-250,0 };
		m_fadeAlpha = 0.0f;
		++m_time;
	}

	PWINDOWINFO pwi = new WINDOWINFO;
	pwi->cbSize = sizeof(WINDOWINFO);
	GetWindowInfo(Application::Instance().GetWindowHandle(), pwi);
	//pwi->rcWindow.left;    // マウスとの当たり判定関係のX座標に足す
	//pwi->rcWindow.top + 35;// マウスとの当たり判定関係のこれをy座標に足す

	if (GetAsyncKeyState(VK_TAB) & 0x8000)
	{
		if (!m_bTABKey)
		{
			m_bTABKey = true;
			if (!m_bOption)
			{
				m_bOption = true;
				KdAudioManager::Instance().Play("Asset/Audio/SE/メニューを開く1.wav");
				ShowCursor(true); // マウスカーソルを消す
			}
			else if (m_bOption)
			{
				m_bOption = false;
				m_bWeaponDataPage = false;
				m_bWeaponDataHopperPage = false;
				m_bWeaponDataScoPage = true;
				m_bHowToPage = true;
				m_bInfo = false;
				SetCursorPos(640, 360);
				KdAudioManager::Instance().Play("Asset/Audio/SE/メニューを開く1.wav");
				ShowCursor(false); // マウスカーソルを消す
			}
		}
	}
	else
	{
		m_bTABKey = false;
	}

	if (m_bOption)
	{

		POINT mousePos;
		GetCursorPos(&mousePos);
		mousePos.x -= (long)640.0f;
		mousePos.y = (long)(mousePos.y * -1 + 360.0f);
		Math::Vector3 Dis;
		float mouseX = (float)mousePos.x/* + (float)(pwi->rcWindow.left)*/;
		float mouseY = (float)mousePos.y + (float)(pwi->rcWindow.top + 35);

		if (!m_bInfo)
		{
			Math::Vector3 ExitPos;
			ExitPos.x = m_exitPos.x + (float)(pwi->rcWindow.left);
			ExitPos.y = m_exitPos.y /*+ (float)(pwi->rcWindow.top)*/;
			ExitPos.z = m_exitPos.z;

			Dis = ExitPos - Math::Vector3(mouseX, mouseY, 0.0f);
			if (Dis.Length() <= 75)
			{
				m_exitScale = 1.5f;
				m_titleScale = 1.0f;

				if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
				{
					if (!m_addFadeAlpha)
					{
						m_bExit = true;
						m_addFadeAlpha = true;
						m_bFirstExit = true;
						KdAudioManager::Instance().Play("Asset/Audio/SE/各ボタンを押したときの音.wav");
					}
				}

			}
			else
			{
				m_exitScale = 1.0f;
			}

			Math::Vector3 TitlePos;
			TitlePos.x = m_titlePos.x + (float)(pwi->rcWindow.left);
			TitlePos.y = m_titlePos.y /*+ (float)(pwi->rcWindow.top)*/;
			TitlePos.z = m_titlePos.z;

			Dis = TitlePos - Math::Vector3(mouseX, mouseY, 0.0f);
			if (Dis.Length() <= 75)
			{
				m_exitScale = 1.0f;
				m_titleScale = 1.5f;

				if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
				{
					if (!m_addFadeAlpha)
					{
						m_bTitle = true;
						m_addFadeAlpha = true;
						KdAudioManager::Instance().Play("Asset/Audio/SE/各ボタンを押したときの音.wav");
					}
				}

			}
			else
			{
				m_titleScale = 1.0f;
			}

			Math::Vector3 InfoPos;
			InfoPos.x = m_infoPos.x + (float)(pwi->rcWindow.left);
			InfoPos.y = m_infoPos.y /*+ (float)(pwi->rcWindow.top)*/;
			InfoPos.z = m_infoPos.z;

			Dis = InfoPos - Math::Vector3(mouseX, mouseY, 0.0f);
			if (Dis.Length() <= 75)
			{
				m_exitScale = 1.0f;
				m_titleScale = 1.0f;
				m_infoScale = 1.5f;

				if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
				{
					m_bInfo = true;
					KdAudioManager::Instance().Play("Asset/Audio/SE/各ボタンを押したときの音.wav");
				}

			}
			else
			{
				m_infoScale = 1.0f;
			}

			Math::Vector3 BackPos;
			BackPos.x = m_backPos.x + (float)(pwi->rcWindow.left);
			BackPos.y = m_backPos.y /*+ (float)(pwi->rcWindow.top)*/;
			BackPos.z = m_backPos.z;
			Dis = BackPos - Math::Vector3(mouseX, mouseY, 0.0f);
			if (Dis.Length() <= 40)
			{
				m_backScale = 1.0f;
				if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
				{
					if (!bLButtonKey)
					{
						bLButtonKey = true;
						m_bOption = false;
						ShowCursor(false); // マウスカーソルを消す
						SetCursorPos(640, 360);
						KdAudioManager::Instance().Play("Asset/Audio/SE/backPush.wav");
					}
				}
				else
				{
					bLButtonKey = false;
				}

			}
			else
			{
				m_backScale = 0.8f;
			}

			if (m_addFadeAlpha)
			{
				m_fadeAlpha += (1.0f / 90);
				if (m_fadeAlpha >= 1.0f)
				{
					m_fadeAlpha = 1.0f;

					if (m_bTitle)
					{
						SceneManager::Instance().SetNextScene
						(
							SceneManager::SceneType::title
						);
					}

					if (m_bExit)
					{
						if (m_bFirstExit)
						{
							SceneManager::Instance().SetBWinEnd();
							m_bFirstExit = false;
						}
						else
						{
							m_bExit = false;
							m_addFadeAlpha = false;
							m_fadeAlpha = 0.0f;
						}
					}
				}
			}

		}
		else
		{

			Math::Vector3 BackPos;
			BackPos.x = m_backPos.x + (float)(pwi->rcWindow.left);
			BackPos.y = m_backPos.y /*+ (float)(pwi->rcWindow.top)*/;
			BackPos.z = m_backPos.z;
			Dis = BackPos - Math::Vector3(mouseX, mouseY, 0.0f);
			if (Dis.Length() <= 40)
			{
				m_backScale = 1.0f;
				if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
				{
					if (!bLButtonKey)
					{
						bLButtonKey = true;
						m_bWeaponDataPage = false;
						m_bWeaponDataHopperPage = false;
						m_bWeaponDataScoPage = true;
						m_bHowToPage = true;
						m_bInfo = false;
						KdAudioManager::Instance().Play("Asset/Audio/SE/backPush.wav");
					}
				}
				else
				{
					bLButtonKey = false;
				}


			}
			else
			{
				m_backScale = 0.8f;
			}

			Math::Vector3 WeaOrHowLeftYaiPos;
			WeaOrHowLeftYaiPos.x = m_weaOrHowLeftYaiPos.x + (float)(pwi->rcWindow.left);
			WeaOrHowLeftYaiPos.y = m_weaOrHowLeftYaiPos.y /*+ (float)(pwi->rcWindow.top)*/;
			WeaOrHowLeftYaiPos.z = m_weaOrHowLeftYaiPos.z;
			Dis = WeaOrHowLeftYaiPos - Math::Vector3(mouseX, mouseY, 0.0f);
			if (Dis.Length() <= 30)
			{
				m_weaOrHowLeftYaiScale = 1.5f;
				m_weaOrHowRightYaiScale = 1.0f;

				if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
				{
					if (!bLButtonKey)
					{
						bLButtonKey = true;
						if (m_bWeaponDataPage)
						{
							m_bHowToPage = true;
							m_bWeaponDataPage = false;
						}
						else
						{
							m_bHowToPage = false;
							m_bWeaponDataPage = true;
						}
						KdAudioManager::Instance().Play("Asset/Audio/SE/矢印押したときの音.wav");
					}
				}
				else
				{
					bLButtonKey = false;
				}

			}
			else
			{
				m_weaOrHowLeftYaiScale = 1.0f;
			}

			Math::Vector3 WeaOrHowRightYaiPos;
			WeaOrHowRightYaiPos.x = m_weaOrHowRightYaiPos.x + (float)(pwi->rcWindow.left);
			WeaOrHowRightYaiPos.y = m_weaOrHowRightYaiPos.y /*+ (float)(pwi->rcWindow.top)*/;
			WeaOrHowRightYaiPos.z = m_weaOrHowRightYaiPos.z;
			Dis = m_weaOrHowRightYaiPos - Math::Vector3(mouseX, mouseY, 0.0f);
			if (Dis.Length() <= 30)
			{
				m_weaOrHowLeftYaiScale = 1.0f;
				m_weaOrHowRightYaiScale = 1.5f;

				if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
				{
					if (!bLButtonKey)
					{
						bLButtonKey = true;
						if (m_bWeaponDataPage)
						{
							m_bHowToPage = true;
							m_bWeaponDataPage = false;
						}
						else
						{
							m_bHowToPage = false;
							m_bWeaponDataPage = true;
						}
						KdAudioManager::Instance().Play("Asset/Audio/SE/矢印押したときの音.wav");
					}
				}
				else
				{
					bLButtonKey = false;
				}

			}
			else
			{
				m_weaOrHowRightYaiScale = 1.0f;
			}

			if (m_bWeaponDataPage)
			{
				Math::Vector3 WeaponLeftYaiPos;
				WeaponLeftYaiPos.x = m_weaponLeftYaiPos.x + (float)(pwi->rcWindow.left);
				WeaponLeftYaiPos.y = m_weaponLeftYaiPos.y /*+ (float)(pwi->rcWindow.top)*/;
				WeaponLeftYaiPos.z = m_weaponLeftYaiPos.z;

				Dis = WeaponLeftYaiPos - Math::Vector3(mouseX, mouseY, 0.0f);
				if (Dis.Length() <= 30)
				{
					m_weaponLeftYaiScale = 1.5f;
					m_weaponRightYaiScale = 1.0f;

					if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
					{
						if (!bLButtonKey)
						{
							bLButtonKey = true;
							if (m_bWeaponDataHopperPage)
							{
								m_bWeaponDataScoPage = true;
								m_bWeaponDataHopperPage = false;
							}
							else
							{
								m_bWeaponDataScoPage = false;
								m_bWeaponDataHopperPage = true;
							}
							KdAudioManager::Instance().Play("Asset/Audio/SE/矢印押したときの音.wav");
						}
					}
					else
					{
						bLButtonKey = false;
					}

				}
				else
				{
					m_weaponLeftYaiScale = 1.0f;
				}

				Math::Vector3 WeaponRightYaiPos;
				WeaponRightYaiPos.x = m_weaponRightYaiPos.x + (float)(pwi->rcWindow.left);
				WeaponRightYaiPos.y = m_weaponRightYaiPos.y /*+ (float)(pwi->rcWindow.top)*/;
				WeaponRightYaiPos.z = m_weaponRightYaiPos.z;

				Dis = WeaponRightYaiPos - Math::Vector3(mouseX, mouseY, 0.0f);
				if (Dis.Length() <= 30)
				{
					m_weaponLeftYaiScale = 1.0f;
					m_weaponRightYaiScale = 1.5f;

					if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
					{
						if (!bLButtonKey)
						{
							bLButtonKey = true;
							if (m_bWeaponDataHopperPage)
							{
								m_bWeaponDataScoPage = true;
								m_bWeaponDataHopperPage = false;
							}
							else
							{
								m_bWeaponDataScoPage = false;
								m_bWeaponDataHopperPage = true;
							}
							KdAudioManager::Instance().Play("Asset/Audio/SE/矢印押したときの音.wav");
						}
					}
					else
					{
						bLButtonKey = false;
					}

				}
				else
				{
					m_weaponRightYaiScale = 1.0f;
				}
			}
		}
	}
	else
	{
		if (m_bTutorialView)
		{
			if (m_tutorialSwitchAfterTime > 0)
			{
				--m_tutorialSwitchAfterTime;
			}
			else
			{
				if (GetAsyncKeyState(VK_RETURN) & 0x8000)
				{
					m_bTutorialView = false;
				}

				if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
				{
					m_bTutorialView = false;
				}
			}
		}
		else
		{
			switch (m_tutorialType)
			{
			case kihonTu:
				if (m_tutorialCnt >= 5)
				{
					m_tutorialCnt = 0;
					m_bTutorialView = true;
					m_tutorialType = bukiTu;
					m_tutorialSwitchAfterTime = 120;
				}
				break;
			case bukiTu:
				if (m_tutorialCnt >= 5)
				{
					m_tutorialCnt = 0;
					m_bTutorialView = true;
					m_tutorialType = sukoADMoveTu;
					m_tutorialSwitchAfterTime = 120;
				}
				break;
			case sukoADMoveTu:
				if (m_tutorialCnt >= 5)
				{
					m_tutorialCnt = 0;
					m_bTutorialView = true;
					m_tutorialType = hopperTu;
					m_tutorialSwitchAfterTime = 120;
				}
				break;
			case hopperTu:
				if (m_tutorialCnt >= 5)
				{
					m_tutorialCnt = 0;
					m_bTutorialView = true;
					m_tutorialType = sonotaTu;
					m_tutorialSwitchAfterTime = 120;
				}
				break;
			}
		}
	}

	KdSafeDelete(pwi);
}

void Ui::ResultUpdate()
{
	m_time++;
	if (m_time >= 270)
	{
		m_time = 270;
	}

	if (m_bFirstInResult)
	{
		m_bFirstInResult = false;
		m_fadeAlpha = 0.0f;
		std::ifstream ifs("Asset/Data/PointAndRank.csv");

		if (!ifs)
		{
			return;
		}

		std::string str, filed;
		std::string aStr[4];
		int i = 0;
		while (i < 4)
		{
			std::getline(ifs, str);
			std::istringstream ss(str);
			while (std::getline(ss, filed, ','))
			{
				aStr[i] = filed;
				++i;
			}
		}

		m_weaponPoint = stoi(aStr[1]);
		m_rank = aStr[3];

		m_pointAddOrSubVal = SceneManager::Instance().GetPointAddOrSubVal();
	}

	if (m_time >= 210)
	{
		if (SceneManager::Instance().GetBAddOrSubVal())
		{
			if (m_time == 210)
			{
				KdAudioManager::Instance().Play("Asset/Audio/SE/AddAndSub.wav", true);
			}

			if (m_pointAddOrSubVal > 0)
			{
				m_pointAddOrSubVal--;
				m_weaponPoint++;
				if (m_weaponPoint >= 99999)
				{
					m_weaponPoint = 99999;
				}
			}
			else if (m_pointAddOrSubVal == 0)
			{
				KdAudioManager::Instance().PauseAllSound();
				if (SceneManager::Instance().GetBPlayerWin())
				{
					KdAudioManager::Instance().Play("Asset/Audio/BGM/7 Fanfare and Victory Lap Loop.wav", true);
				}
				else
				{
					KdAudioManager::Instance().Play("Asset/Audio/BGM/8 Defeat loop.wav", true);
				}
				KdAudioManager::Instance().Play("Asset/Audio/SE/asCompletion.wav");
				m_pointAddOrSubVal--;
				std::ofstream ofs("Asset/Data/PointAndRank.csv"); // 書き込み
				if (!ofs) { return; }
				if (m_weaponPoint < 4000)
				{
					ofs << "point" << ',' << m_weaponPoint << ',' << '\n' << "rank" << ',' << "C" << ',' << '\n';
				}
				else if (m_weaponPoint >= 4000 && m_weaponPoint < 8000)
				{
					ofs << "point" << ',' << m_weaponPoint << ',' << '\n' << "rank" << ',' << "B" << ',' << '\n';
				}
				else if (m_weaponPoint >= 8000 && m_weaponPoint < 1200)
				{
					ofs << "point" << ',' << m_weaponPoint << ',' << '\n' << "rank" << ',' << "A" << ',' << '\n' << "Skill" << ',' << "マスター" << '\n';
				}
				else
				{
					ofs << "point" << ',' << m_weaponPoint << ',' << '\n' << "rank" << ',' << "A" << ',' << '\n' << "Skill" << ',' << "マスター" << '\n';
				}
				ofs.close();
				m_time = 210;
			}
		}
		else
		{
			if (m_time == 210)
			{
				KdAudioManager::Instance().Play("Asset/Audio/SE/AddAndSub.wav", true);
			}

			if (m_pointAddOrSubVal > 0)
			{
				m_pointAddOrSubVal--;
				m_weaponPoint--;
				if (m_weaponPoint <= 0)
				{
					m_weaponPoint = 0;
				}
			}
			else if (m_pointAddOrSubVal == 0)
			{
				KdAudioManager::Instance().PauseAllSound();
				if (SceneManager::Instance().GetBPlayerWin())
				{
					KdAudioManager::Instance().Play("Asset/Audio/BGM/7 Fanfare and Victory Lap Loop.wav", true);
				}
				else
				{
					KdAudioManager::Instance().Play("Asset/Audio/BGM/8 Defeat loop.wav", true);
				}
				KdAudioManager::Instance().Play("Asset/Audio/SE/asCompletion.wav");
				m_pointAddOrSubVal--;
				std::ofstream ofs("Asset/Data/PointAndRank.csv"); // 書き込み
				if (!ofs) { return; }
				if (m_weaponPoint < 4000)
				{
					ofs << "point" << ',' << m_weaponPoint << ',' << '\n' << "rank" << ',' << "C" << ',' << '\n';
				}
				else if (m_weaponPoint >= 4000 && m_weaponPoint < 8000)
				{
					ofs << "point" << ',' << m_weaponPoint << ',' << '\n' << "rank" << ',' << "B" << ',' << '\n';
				}
				else if (m_weaponPoint >= 8000 && m_weaponPoint < 1200)
				{
					ofs << "point" << ',' << m_weaponPoint << ',' << '\n' << "rank" << ',' << "A" << ',' << '\n' << "Skill" << ',' << "マスター" << '\n';
				}
				else
				{
					ofs << "point" << ',' << m_weaponPoint << ',' << '\n' << "rank" << ',' << "A" << ',' << '\n' << "Skill" << ',' << "マスター" << '\n';
				}
				ofs.close();
				m_time = 210;
			}
		}

		if (GetAsyncKeyState(VK_RETURN) & 0x8000)
		{
			if (!m_bEnterKey)
			{
				if (m_pointAddOrSubVal > 0)
				{
					if (SceneManager::Instance().GetBAddOrSubVal())
					{
						m_weaponPoint += m_pointAddOrSubVal;
						if (m_weaponPoint >= 99999)
						{
							m_weaponPoint = 99999;
						}
					}
					else
					{
						m_weaponPoint -= m_pointAddOrSubVal;
						if (m_weaponPoint <= 0)
						{
							m_weaponPoint = 0;
						}
					}

					m_pointAddOrSubVal = 0;
				}
				m_bEnterKey = true;
			}
		}
		else
		{
			m_bEnterKey = false;
		}
	}

	if (m_time >= 270 && m_pointAddOrSubVal < 0)
	{
		m_bSceneCangePossible = true;
	}

	if (m_bSceneCangePossible)
	{
		if (m_bPushToEnterAlphaAdd)
		{
			m_pushToEnterAlpha += (1.0f / 30);
			if (m_pushToEnterAlpha >= 1.0f)
			{
				m_pushToEnterAlpha = 1.0f;
				m_bPushToEnterAlphaAdd = false;
			}
		}
		else
		{
			m_pushToEnterAlpha -= (1.0f / 30);
			if (m_pushToEnterAlpha <= 0.0f)
			{
				m_pushToEnterAlpha = 0.0f;
				m_bPushToEnterAlphaAdd = true;
			}
		}
	}

	if (m_addFadeAlpha)
	{
		m_fadeAlpha += (1.0f / 90);
		if (m_fadeAlpha >= 1.0f)
		{
			m_fadeAlpha = 1.0f;

			if (m_uiType == UiType::result)
			{
				SceneManager::Instance().SetNextScene
				(
					SceneManager::SceneType::title
				);
			}
		}
	}
}

void Ui::SelectUpdate()
{
	if (m_time == 0)
	{
		m_fadeAlpha = 0.0f;
		m_time++;
	}

	PWINDOWINFO pwi = new WINDOWINFO;
	pwi->cbSize = sizeof(WINDOWINFO);
	GetWindowInfo(Application::Instance().GetWindowHandle(), pwi);
	//pwi->rcWindow.left;    // マウスとの当たり判定関係のX座標に足す
	//pwi->rcWindow.top + 35;// マウスとの当たり判定関係のこれをy座標に足す

	POINT mousePos;
	GetCursorPos(&mousePos);


	if (m_bBattlehelp)
	{
		mousePos.x -= 640;
		mousePos.y = mousePos.y * -1 + 360;
		Math::Vector3 Dis;
		float mouseX = (float)mousePos.x/* + (float)(pwi->rcWindow.left)*/;
		float mouseY = (float)mousePos.y + (float)(pwi->rcWindow.top + 35);

		Math::Vector3 BackPos;
		BackPos.x = m_backPos.x + (float)(pwi->rcWindow.left);
		BackPos.y = m_backPos.y /*+ (float)(pwi->rcWindow.top)*/;
		BackPos.z = m_backPos.z;
		Dis = BackPos - Math::Vector3(mouseX, mouseY, 0.0f);
		if (Dis.Length() <= 40)
		{
			m_backScale = 1.0f;
			if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
			{
				if (!bLButtonKey)
				{
					bLButtonKey = true;
					m_bBattlehelp = false;
					SetCursorPos(640, 360);
					KdAudioManager::Instance().Play("Asset/Audio/SE/backPush.wav");
				}
			}
			else
			{
				bLButtonKey = false;
			}

		}
		else
		{
			m_backScale = 0.8f;
		}
	}
	else if (m_bChalengehelp)
	{
		mousePos.x -= 640;
		mousePos.y = mousePos.y * -1 + 360;
		Math::Vector3 Dis;
		float mouseX = (float)mousePos.x/* + (float)(pwi->rcWindow.left)*/;
		float mouseY = (float)mousePos.y + (float)(pwi->rcWindow.top + 35);

		Math::Vector3 BackPos;
		BackPos.x = m_backPos.x + (float)(pwi->rcWindow.left);
		BackPos.y = m_backPos.y /*+ (float)(pwi->rcWindow.top)*/;
		BackPos.z = m_backPos.z;
		Dis = BackPos - Math::Vector3(mouseX, mouseY, 0.0f);
		if (Dis.Length() <= 40)
		{
			m_backScale = 1.0f;
			if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
			{
				if (!bLButtonKey)
				{
					bLButtonKey = true;
					m_bChalengehelp = false;
					SetCursorPos(640, 360);
					KdAudioManager::Instance().Play("Asset/Audio/SE/backPush.wav");
				}
			}
			else
			{
				bLButtonKey = false;
			}

		}
		else
		{
			m_backScale = 0.8f;
		}
	}
	else
	{


		mousePos.x -= 640;
		mousePos.y = mousePos.y * -1 + 360;
		Math::Vector3 Dis;
		float mouseX = (float)mousePos.x/* + (float)(pwi->rcWindow.left)*/;
		float mouseY = (float)mousePos.y + (float)(pwi->rcWindow.top + 35);

		Math::Vector3 artPos;
		artPos.x = m_tutorialPos.x + (float)(pwi->rcWindow.left);
		artPos.y = m_tutorialPos.y /*+ (float)(pwi->rcWindow.top)*/;
		artPos.z = m_tutorialPos.z;

		Dis = artPos - Math::Vector3(mouseX, mouseY, 0.0f);
		if (Dis.Length() <= 75)
		{
			m_tutorialScale = 1.1f;

			if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
			{
				if (!m_addFadeAlpha)
				{
					m_bTutorial = true;
					m_addFadeAlpha = true;
					KdAudioManager::Instance().Play("Asset/Audio/SE/各ボタンを押したときの音.wav");
				}
			}

		}
		else
		{
			m_tutorialScale = 1.0f;
		}

		Math::Vector3 ExitPos;
		ExitPos.x = m_exitPos.x + (float)(pwi->rcWindow.left);
		ExitPos.y = m_exitPos.y /*+ (float)(pwi->rcWindow.top)*/;
		ExitPos.z = m_exitPos.z;

		Dis = ExitPos - Math::Vector3(mouseX, mouseY, 0.0f);
		if (Dis.Length() <= 25)
		{
			m_exitScale = 1.1f;

			if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
			{
				if (!m_addFadeAlpha)
				{
					m_bExit = true;
					m_addFadeAlpha = true;
					m_bFirstExit = true;
					KdAudioManager::Instance().Play("Asset/Audio/SE/各ボタンを押したときの音.wav");
				}
			}

		}
		else
		{
			m_exitScale = 1.0f;
		}

		Math::Vector3 GamePos;
		GamePos.x = m_oneEnemyTotalPos.x + (float)(pwi->rcWindow.left);
		GamePos.y = m_oneEnemyTotalPos.y /*+ (float)(pwi->rcWindow.top)*/;
		GamePos.z = m_oneEnemyTotalPos.z;

		Dis = GamePos - Math::Vector3(mouseX, mouseY, 0.0f);
		if (Dis.Length() <= 30)
		{
			m_oneEnemyScale = 1.1f;

			if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
			{
				if (!m_addFadeAlpha)
				{
					m_bOneEnemyTotal = true;
					m_bGame = true;
					m_addFadeAlpha = true;
					KdAudioManager::Instance().Play("Asset/Audio/SE/各ボタンを押したときの音.wav");
				}
			}

		}
		else
		{
			m_oneEnemyScale = 1.0f;
		}

		GamePos.x = m_twoEnemyTotalPos.x + (float)(pwi->rcWindow.left);
		GamePos.y = m_twoEnemyTotalPos.y /*+ (float)(pwi->rcWindow.top)*/;
		GamePos.z = m_twoEnemyTotalPos.z;

		Dis = GamePos - Math::Vector3(mouseX, mouseY, 0.0f);
		if (Dis.Length() <= 30)
		{
			m_twoEnemyScale = 1.1f;

			if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
			{
				if (!m_addFadeAlpha)
				{
					m_bTwoEnemyTotal = true;
					m_bGame = true;
					m_addFadeAlpha = true;
					KdAudioManager::Instance().Play("Asset/Audio/SE/各ボタンを押したときの音.wav");
				}
			}

		}
		else
		{
			m_twoEnemyScale = 1.0f;
		}

		GamePos.x = m_threeEnemyTotalPos.x + (float)(pwi->rcWindow.left);
		GamePos.y = m_threeEnemyTotalPos.y /*+ (float)(pwi->rcWindow.top)*/;
		GamePos.z = m_threeEnemyTotalPos.z;

		Dis = GamePos - Math::Vector3(mouseX, mouseY, 0.0f);
		if (Dis.Length() <= 30)
		{
			m_threeEnemyScale = 1.1f;

			if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
			{
				if (!m_addFadeAlpha)
				{
					m_bThreeEnemyTotal = true;
					m_bGame = true;
					m_addFadeAlpha = true;
					KdAudioManager::Instance().Play("Asset/Audio/SE/各ボタンを押したときの音.wav");
				}
			}

		}
		else
		{
			m_threeEnemyScale = 1.0f;
		}

		Math::Vector3 ChallengePos;
		ChallengePos.x = m_chalenge50Pos.x + (float)(pwi->rcWindow.left);
		ChallengePos.y = m_chalenge50Pos.y /*+ (float)(pwi->rcWindow.top)*/;
		ChallengePos.z = m_chalenge50Pos.z;

		Dis = ChallengePos - Math::Vector3(mouseX, mouseY, 0.0f);
		if (Dis.Length() <= 30)
		{
			m_chalenge50Scale = 1.1f;

			if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
			{
				if (!m_addFadeAlpha)
				{
					m_bChalenge50 = true;
					m_addFadeAlpha = true;
					KdAudioManager::Instance().Play("Asset/Audio/SE/各ボタンを押したときの音.wav");
				}
			}

		}
		else
		{
			m_chalenge50Scale = 1.0f;
		}

		ChallengePos;
		ChallengePos.x = m_chalenge100Pos.x + (float)(pwi->rcWindow.left);
		ChallengePos.y = m_chalenge100Pos.y /*+ (float)(pwi->rcWindow.top)*/;
		ChallengePos.z = m_chalenge100Pos.z;

		Dis = ChallengePos - Math::Vector3(mouseX, mouseY, 0.0f);
		if (Dis.Length() <= 30)
		{
			m_chalenge100Scale = 1.1f;

			if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
			{
				if (!m_addFadeAlpha)
				{
					m_bChalenge100 = true;
					m_addFadeAlpha = true;
					KdAudioManager::Instance().Play("Asset/Audio/SE/各ボタンを押したときの音.wav");
				}
			}

		}
		else
		{
			m_chalenge100Scale = 1.0f;
		}

		Math::Vector3 TrainingPos;
		TrainingPos.x = m_trainingPos.x + (float)(pwi->rcWindow.left);
		TrainingPos.y = m_trainingPos.y /*+ (float)(pwi->rcWindow.top)*/;
		TrainingPos.z = m_trainingPos.z;

		Dis = TrainingPos - Math::Vector3(mouseX, mouseY, 0.0f);
		if (Dis.Length() <= 75)
		{
			m_trainingScale = 1.1f;

			//m_optionScale    = 1.1f;

			/*if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
			{
				if (!m_addFadeAlpha)
				{
					m_bTraining = true;
					m_addFadeAlpha = true;
					KdAudioManager::Instance().Play("Asset/Audio/SE/各ボタンを押したときの音.wav");
				}
			}*/

		}
		else
		{
			m_trainingScale = 1.0f;
		}

		Math::Vector3 TitlePos;
		TitlePos.x = m_titlePos.x + (float)(pwi->rcWindow.left);
		TitlePos.y = m_titlePos.y /*+ (float)(pwi->rcWindow.top)*/;
		TitlePos.z = m_titlePos.z;

		Dis = TitlePos - Math::Vector3(mouseX, mouseY, 0.0f);
		if (Dis.Length() <= 25)
		{
			m_titleScale = 1.1f;

			if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
			{
				if (!m_addFadeAlpha)
				{
					m_bTitle = true;
					m_addFadeAlpha = true;
					KdAudioManager::Instance().Play("Asset/Audio/SE/各ボタンを押したときの音.wav");
				}
			}

		}
		else
		{
			m_titleScale = 1.0f;
		}

		Math::Vector3 BattlehelpMkPos;
		BattlehelpMkPos.x = m_BattlehelpMkPos.x + (float)(pwi->rcWindow.left);
		BattlehelpMkPos.y = m_BattlehelpMkPos.y /*+ (float)(pwi->rcWindow.top)*/;
		BattlehelpMkPos.z = m_BattlehelpMkPos.z;

		Dis = BattlehelpMkPos - Math::Vector3(mouseX, mouseY, 0.0f);
		if (Dis.Length() <= 25)
		{
			m_BattlehelpMkScale = 1.1f;

			if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
			{
				if (!m_addFadeAlpha)
				{
					m_bBattlehelp = true;
					KdAudioManager::Instance().Play("Asset/Audio/SE/各ボタンを押したときの音.wav");
				}
			}

		}
		else
		{
			m_BattlehelpMkScale = 1.0f;
		}

		Math::Vector3 ChalengehelpMkPos;
		ChalengehelpMkPos.x = m_chalengehelpMkPos.x + (float)(pwi->rcWindow.left);
		ChalengehelpMkPos.y = m_chalengehelpMkPos.y /*+ (float)(pwi->rcWindow.top)*/;
		ChalengehelpMkPos.z = m_chalengehelpMkPos.z;

		Dis = ChalengehelpMkPos - Math::Vector3(mouseX, mouseY, 0.0f);
		if (Dis.Length() <= 25)
		{
			m_chalengehelpMkScale = 1.1f;

			if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
			{
				if (!m_addFadeAlpha)
				{
					m_bBattlehelp = true;
					KdAudioManager::Instance().Play("Asset/Audio/SE/各ボタンを押したときの音.wav");
				}
			}

		}
		else
		{
			m_chalengehelpMkScale = 1.0f;
		}
	}

	if (m_addFadeAlpha)
	{
		m_fadeAlpha += (1.0f / 90);
		if (m_fadeAlpha >= 1.0f)
		{
			m_fadeAlpha = 1.0f;

			if (m_bTitle)
			{
				SceneManager::Instance().SetNextScene
				(
					SceneManager::SceneType::title
				);
			}

			if (m_bGame)
			{
				if (m_bOneEnemyTotal)
				{
					SceneManager::Instance().SetEnemyTotal(1);
					SceneManager::Instance().SetEnemyIeftover(1);
				}
				else if (m_bTwoEnemyTotal)
				{
					SceneManager::Instance().SetEnemyTotal(2);
					SceneManager::Instance().SetEnemyIeftover(2);
				}
				else if (m_bThreeEnemyTotal)
				{
					SceneManager::Instance().SetEnemyTotal(3);
					SceneManager::Instance().SetEnemyIeftover(3);
				}

				SceneManager::Instance().SetNextScene
				(
					SceneManager::SceneType::battle
				);
			}

			if (m_bTutorial)
			{
				SceneManager::Instance().SetNextScene
				(
					SceneManager::SceneType::tutorial
				);
			}

			if (m_bExit)
			{
				if (m_bFirstExit)
				{
					SceneManager::Instance().SetBWinEnd();
					m_bFirstExit = false;
				}
				else
				{
					m_bExit = false;
					m_addFadeAlpha = false;
					m_fadeAlpha = 0.0f;
				}
			}

			if (m_bChalenge50)
			{
				SceneManager::Instance().SetEnemyTotal(50);
				SceneManager::Instance().SetEnemyIeftover(50);

				SceneManager::Instance().SetNextScene
				(
					SceneManager::SceneType::challenge
				);
			}

			if (m_bChalenge100)
			{
				SceneManager::Instance().SetEnemyTotal(100);
				SceneManager::Instance().SetEnemyIeftover(100);

				SceneManager::Instance().SetNextScene
				(
					SceneManager::SceneType::challenge
				);
			}
		}
	}

	KdSafeDelete(pwi);
	//delete(pwi);
}

void Ui::DrawSprite()
{
	Math::Matrix transMat = Math::Matrix::Identity;
	Math::Matrix mat = Math::Matrix::Identity;
	KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
	Math::Color color = {};
	std::shared_ptr<Player> m_spPlayer = m_wpPlayer.lock();

	switch (m_uiType)
	{
	case UiType::title:

		color = { 0,0,0,1 };
		KdShaderManager::Instance().m_spriteShader.DrawBox(0, 0, 1280, 720, &color);

		if (m_time >= 0 && m_time < 360)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_FTtoyoRogoTex, 0, 0, 500, 500);
		}
		else if (m_time >= 360)
		{
			transMat = Math::Matrix::CreateTranslation(0, 200, 0);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_titleCharRogoTex, 0, 0, 692, 172);

			transMat = Math::Matrix::CreateTranslation(0, -200, 0);
			Math::Rectangle rc = { 0,0,894,114 };
			color = { 1,1,1,m_pushToEnterAlpha };
			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_pushToEnterTex, 0, 0, 894, 114, &rc, &color);
		}

		transMat = Math::Matrix::Identity;
		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		color = { 0,0,0,m_fadeAlpha };
		KdShaderManager::Instance().m_spriteShader.DrawBox(0, 0, 1280, 720, &color);

		break;
	case UiType::game:

		transMat = Math::Matrix::Identity;
		if (m_time >= 0 && m_time < 60)
		{
			mat = Math::Matrix::CreateScale(m_countThreeScale) * transMat;
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			Math::Rectangle rc = { 0,0,121, 136 };
			color = { 1,1,1,m_countThreeAlpha };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_countThreeTex, 0, 0, 121, 136, &rc, &color);
		}
		else if (m_time >= 60 && m_time < 120)
		{
			mat = Math::Matrix::CreateScale(m_countTwoScale) * transMat;
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			Math::Rectangle rc = { 0,0,121, 136 };
			color = { 1,1,1,m_countTwoAlpha };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_countTwoTex, 0, 0, 121, 136, &rc, &color);
		}
		else if (m_time >= 120 && m_time < 180)
		{
			mat = Math::Matrix::CreateScale(m_countOneScale) * transMat;
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			Math::Rectangle rc = { 0,0,121, 136 };
			color = { 1,1,1,m_countOneAlpha };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_countOneTex, 0, 0, 121, 136, &rc, &color);
		}
		else if (m_time >= 180 && m_time < 240)
		{
			mat = Math::Matrix::CreateScale(m_countGoScale) * transMat;
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			Math::Rectangle rc = { 0,0,211, 136 };
			color = { 1,1,1,m_countGoAlpha };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_countGoTex, 0, 0, 211, 136, &rc, &color);
		}

		if (m_spPlayer)
		{
			transMat = Math::Matrix::CreateTranslation(350, -250, 0);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponType1Tex, 0, 0, 120, 50);
			if (m_spPlayer->GetWeaponType() & grassHopper)
			{
				Math::Rectangle rc = { 0,0,120,50 };
				color = { 1,1,1,0.6f };
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponTypeOvreDarkTex, 0, 0, 120, 50, &rc, &color);
			}

			transMat = Math::Matrix::CreateTranslation(500, -250, 0);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponType2Tex, 0, 0, 120, 50);
			if (m_spPlayer->GetWeaponType() & scorpion)
			{
				transMat = Math::Matrix::CreateTranslation(440, -250, 0);
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				Math::Rectangle rc = { 0,0,120,50 };
				color = { 1,1,1,0.6f };
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponTypeOvreDarkTex, 0, 0, 120, 50, &rc, &color, Math::Vector2(0, 0.5f));
			}
			else if (m_spPlayer->GetWeaponType() & grassHopper && m_spPlayer->GetRGrassHopperPauCnt() != 0)
			{
				transMat = Math::Matrix::CreateTranslation(440, -250, 0);
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				Math::Rectangle rc = { 0,0,m_spPlayer->GetRGrassHopperPauCnt() * 4,50 };
				color = { 1,1,1,0.6f };
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponTypeOvreDarkTex, 0, 0, m_spPlayer->GetRGrassHopperPauCnt() * 4, 50, &rc, &color, Math::Vector2(0, 0.5f));
			}

			transMat = Math::Matrix::CreateTranslation(-500, -250, 0);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponType1Tex, 0, 0, 120, 50);
			if (m_spPlayer->GetWeaponType() & lGrassHopper)
			{
				Math::Rectangle rc = { 0,0,120,50 };
				color = { 1,1,1,0.6f };
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponTypeOvreDarkTex, 0, 0, 120, 50, &rc, &color);
			}

			transMat = Math::Matrix::CreateTranslation(-350, -250, 0);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponType2Tex, 0, 0, 120, 50);
			if (m_spPlayer->GetWeaponType() & lScorpion)
			{
				transMat = Math::Matrix::CreateTranslation(-410, -250, 0);
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				Math::Rectangle rc = { 0,0,120,50 };
				color = { 1,1,1,0.6f };
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponTypeOvreDarkTex, 0, 0, 120, 50, &rc, &color, Math::Vector2(0, 0.5f));
			}
			else if (m_spPlayer->GetWeaponType() & lGrassHopper && m_spPlayer->GetLGrassHopperPauCnt() != 0)
			{
				transMat = Math::Matrix::CreateTranslation(-410, -250, 0);
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				Math::Rectangle rc = { 0,0,m_spPlayer->GetLGrassHopperPauCnt() * 4,50 };
				color = { 1,1,1,0.6f };
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponTypeOvreDarkTex, 0, 0, m_spPlayer->GetLGrassHopperPauCnt() * 4, 50, &rc, &color, Math::Vector2(0, 0.5f));
			}

			transMat = Math::Matrix::CreateTranslation(-630, 300, 0);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			Math::Rectangle rc = { 0,0,400,50 };
			color = { 1, 1, 1, 1 };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_enduranceBarTex, 0, 0, 400, 50, &rc, &color, Math::Vector2(0, 0.5f));

			transMat = Math::Matrix::CreateTranslation(-630, 300, 0);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			rc = { 0,0,(int)(m_spPlayer->GetEndurance()),50 };
			color = { 1, 1, 1, 1 };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_enduranceTex, 0, 0, (int)(m_spPlayer->GetEndurance()), 50, &rc, &color, Math::Vector2(0, 0.5f));

			transMat = Math::Matrix::CreateTranslation(-630, 255, 0);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			rc = { 0,0,320,40 };
			color = { 1, 1, 1, 1 };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_torionBarTex, 0, 0, 320, 40, &rc, &color, Math::Vector2(0, 0.5f));

			transMat = Math::Matrix::CreateTranslation(-620, 255, 0);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			rc = { 0,0,(int)(m_spPlayer->GetTorion()),30 };
			color = { 1, 1, 1, 1 };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_torionTex, 0, 0, (int)(m_spPlayer->GetTorion()), 30, &rc, &color, Math::Vector2(0, 0.5f));

			int i = 0;
			for (auto& list : m_wpEnemyList)
			{
				if (list.lock()->GetBEnemyDeath())
				{
					++i;
					continue;
				}

				Math::Vector3 nowVec = m_wpCamera.lock()->GetMatrix().Backward();
				nowVec.y = 0.0f;
				nowVec.x = 0.0f;
				nowVec.Normalize();

				// 向きたい方向
				Math::Vector3 toVec = list.lock()->GetPos() - m_wpCamera.lock()->GetPos();
				toVec.y = 0.0f;
				toVec.x = 0.0f;
				toVec.Normalize();

				Math::Vector3 dot = DirectX::XMVector3Dot(nowVec, toVec);
				if (dot.x > 1)
				{
					dot.x = 1;
				}
				if (dot.x < -1)
				{
					dot.x = -1;
				}

				// 角度を取得
				float ang = DirectX::XMConvertToDegrees(acos(dot.x));

				if (ang <= 85)
				{
					/*Math::Vector3 cameraPos = Math::Vector3(list.lock()->GetPos().x, list.lock()->GetPos().y + 1.8f, list.lock()->GetPos().z) - m_wpCamera.lock()->GetCamera()->GetCameraMatrix().Invert().Translation();

					POINT dev;
					KdDirect3D::Instance().WorldToClient(cameraPos, dev, m_wpCamera.lock()->GetCamera()->GetCameraMatrix(), m_wpCamera.lock()->GetCamera()->GetProjMatrix());
					dev.y += 120;*/
					mat = Math::Matrix::CreateScale(0.2f) * Math::Matrix::CreateTranslation(m_enemyScPosList[i].x, m_enemyScPosList[i].y, 0.0f);

					KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
					rc = { 0,0,(int)(list.lock()->GetEndurance()),50 };
					color = { 1, 1, 1, 1 };
					if (SceneManager::Instance().GetSceneType() == SceneManager::SceneType::challenge)
					{
						KdShaderManager::Instance().m_spriteShader.DrawTex(&m_enduranceTex, 0, 0, (int)(list.lock()->GetEndurance() * 2.6f), 50, &rc, &color, Math::Vector2(0, 0.5f));
					}
					else
					{
						KdShaderManager::Instance().m_spriteShader.DrawTex(&m_enduranceTex, 0, 0, (int)(list.lock()->GetEndurance()), 50, &rc, &color, Math::Vector2(0, 0.5f));
					}

					rc = { 0,0,400,50 };
					color = { 1, 1, 1, 1 };
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_enduranceBarTex, 0, 0, 400, 50, &rc, &color, Math::Vector2(0, 0.5f));
				}
				++i;
			}



		}

		if (m_bOption)
		{
			transMat = Math::Matrix::Identity;
			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			color = { 0,0,0,0.2f };
			KdShaderManager::Instance().m_spriteShader.DrawBox(0, 0, 1280, 720, &color);

			if (!m_bInfo)
			{
				mat = Math::Matrix::CreateScale(m_infoScale) * Math::Matrix::CreateTranslation(m_infoPos);
				KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_infoTex, 0, 0, 700, 120);

				mat = Math::Matrix::CreateScale(m_titleScale) * Math::Matrix::CreateTranslation(m_titlePos);
				KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_titleTex, 0, 0, 700, 120);

				mat = Math::Matrix::CreateScale(m_exitScale) * Math::Matrix::CreateTranslation(m_exitPos);
				KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_exitTex, 0, 0, 700, 120);
			}
			else if (m_bInfo)
			{
				if (m_bHowToPage)
				{
					transMat = Math::Matrix::Identity;
					KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_howToTex, 0, 0, 1250, 500);
				}

				if (m_bWeaponDataPage)
				{
					if (m_bWeaponDataHopperPage)
					{
						transMat = Math::Matrix::Identity;
						KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
						KdShaderManager::Instance().m_spriteShader.DrawTex(&m_hopperDataTex, 0, 0, 1156, 260);
					}
					else if (m_bWeaponDataScoPage)
					{
						transMat = Math::Matrix::Identity;
						KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
						KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponInfoTex, 0, 0, 1151, 431);
					}

					mat = Math::Matrix::CreateScale(m_weaponRightYaiScale) * Math::Matrix::CreateTranslation(m_weaponRightYaiPos);
					KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_rightYaiTex, 0, 0, 70, 55);

					mat = Math::Matrix::CreateScale(m_weaponLeftYaiScale) * Math::Matrix::CreateTranslation(m_weaponLeftYaiPos);
					KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_leftYaiTex, 0, 0, 70, 55);
				}

				mat = Math::Matrix::CreateScale(m_weaOrHowRightYaiScale) * Math::Matrix::CreateTranslation(m_weaOrHowRightYaiPos);
				KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_rightYaiTex, 0, 0, 70, 55);

				mat = Math::Matrix::CreateScale(m_weaOrHowLeftYaiScale) * Math::Matrix::CreateTranslation(m_weaOrHowLeftYaiPos);
				KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_leftYaiTex, 0, 0, 70, 55);

			}

			mat = Math::Matrix::CreateScale(m_backScale) * Math::Matrix::CreateTranslation(m_backPos);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_backTex, 0, 0, 182, 80);
		}

		mat = Math::Matrix::CreateTranslation(-50,325,0);
		switch (m_gameTimeM10)
		{
		case 1:
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_time1Tex, 0, 0, 28, 40);
			break;
		case 2:
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_time2Tex, 0, 0, 28, 40);
			break;
		case 3:
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_time3Tex, 0, 0, 28, 40);
			break;
		case 4:
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_time4Tex, 0, 0, 28, 40);
			break;
		case 5:
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_time5Tex, 0, 0, 28, 40);
			break;
		case 6:
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_time6Tex, 0, 0, 28, 40);
			break;
		case 7:
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_time7Tex, 0, 0, 28, 40);
			break;
		case 8:
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_time8Tex, 0, 0, 28, 40);
			break;
		case 9:
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_time9Tex, 0, 0, 28, 40);
			break;
		case 0:
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_time0Tex, 0, 0, 28, 40);
			break;
		}

		mat = Math::Matrix::CreateTranslation(-20, 325, 0);
		switch (m_gameTimeM1)
		{
		case 1:
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_time1Tex, 0, 0, 28, 40);
			break;
		case 2:
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_time2Tex, 0, 0, 28, 40);
			break;
		case 3:
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_time3Tex, 0, 0, 28, 40);
			break;
		case 4:
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_time4Tex, 0, 0, 28, 40);
			break;
		case 5:
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_time5Tex, 0, 0, 28, 40);
			break;
		case 6:
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_time6Tex, 0, 0, 28, 40);
			break;
		case 7:
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_time7Tex, 0, 0, 28, 40);
			break;
		case 8:
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_time8Tex, 0, 0, 28, 40);
			break;
		case 9:
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_time9Tex, 0, 0, 28, 40);
			break;
		case 0:
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_time0Tex, 0, 0, 28, 40);
			break;
		}

		mat = Math::Matrix::CreateTranslation(0, 325, 0);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_conmaTex, 0, 0, 28, 40);

		mat = Math::Matrix::CreateTranslation(20, 325, 0);
		switch (m_gameTimeS10)
		{
		case 1:
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_time1Tex, 0, 0, 28, 40);
			break;
		case 2:
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_time2Tex, 0, 0, 28, 40);
			break;
		case 3:
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_time3Tex, 0, 0, 28, 40);
			break;
		case 4:
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_time4Tex, 0, 0, 28, 40);
			break;
		case 5:
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_time5Tex, 0, 0, 28, 40);
			break;
		case 6:
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_time6Tex, 0, 0, 28, 40);
			break;
		case 7:
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_time7Tex, 0, 0, 28, 40);
			break;
		case 8:
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_time8Tex, 0, 0, 28, 40);
			break;
		case 9:
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_time9Tex, 0, 0, 28, 40);
			break;
		case 0:
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_time0Tex, 0, 0, 28, 40);
			break;
		}

		mat = Math::Matrix::CreateTranslation(50, 325, 0);
		switch (m_gameTimeS1)
		{
		case 1:
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_time1Tex, 0, 0, 28, 40);
			break;
		case 2:
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_time2Tex, 0, 0, 28, 40);
			break;
		case 3:
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_time3Tex, 0, 0, 28, 40);
			break;
		case 4:
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_time4Tex, 0, 0, 28, 40);
			break;
		case 5:
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_time5Tex, 0, 0, 28, 40);
			break;
		case 6:
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_time6Tex, 0, 0, 28, 40);
			break;
		case 7:
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_time7Tex, 0, 0, 28, 40);
			break;
		case 8:
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_time8Tex, 0, 0, 28, 40);
			break;
		case 9:
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_time9Tex, 0, 0, 28, 40);
			break;
		case 0:
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_time0Tex, 0, 0, 28, 40);
			break;
		}

		transMat = Math::Matrix::Identity;
		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		color = { 0,0,0,m_fadeAlpha };
		KdShaderManager::Instance().m_spriteShader.DrawBox(0, 0, 1280, 720, &color);

		break;
	case UiType::tutorial:
		if (m_spPlayer)
		{
			transMat = Math::Matrix::CreateTranslation(350, -250, 0);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponType1Tex, 0, 0, 120, 50);
			if (m_spPlayer->GetWeaponType() & grassHopper)
			{
				Math::Rectangle rc = { 0,0,120,50 };
				color = { 1,1,1,0.6f };
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponTypeOvreDarkTex, 0, 0, 120, 50, &rc, &color);
			}

			transMat = Math::Matrix::CreateTranslation(500, -250, 0);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponType2Tex, 0, 0, 120, 50);
			if (m_spPlayer->GetWeaponType() & scorpion)
			{
				transMat = Math::Matrix::CreateTranslation(440, -250, 0);
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				Math::Rectangle rc = { 0,0,120,50 };
				color = { 1,1,1,0.6f };
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponTypeOvreDarkTex, 0, 0, 120, 50, &rc, &color, Math::Vector2(0, 0.5f));
			}
			else if (m_spPlayer->GetWeaponType() & grassHopper && m_spPlayer->GetRGrassHopperPauCnt() != 0)
			{
				transMat = Math::Matrix::CreateTranslation(440, -250, 0);
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				Math::Rectangle rc = { 0,0,m_spPlayer->GetRGrassHopperPauCnt() * 4,50 };
				color = { 1,1,1,0.6f };
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponTypeOvreDarkTex, 0, 0, m_spPlayer->GetRGrassHopperPauCnt() * 4, 50, &rc, &color, Math::Vector2(0, 0.5f));
			}

			transMat = Math::Matrix::CreateTranslation(-500, -250, 0);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponType1Tex, 0, 0, 120, 50);
			if (m_spPlayer->GetWeaponType() & lGrassHopper)
			{
				Math::Rectangle rc = { 0,0,120,50 };
				color = { 1,1,1,0.6f };
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponTypeOvreDarkTex, 0, 0, 120, 50, &rc, &color);
			}

			transMat = Math::Matrix::CreateTranslation(-350, -250, 0);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponType2Tex, 0, 0, 120, 50);
			if (m_spPlayer->GetWeaponType() & lScorpion)
			{
				transMat = Math::Matrix::CreateTranslation(-410, -250, 0);
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				Math::Rectangle rc = { 0,0,120,50 };
				color = { 1,1,1,0.6f };
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponTypeOvreDarkTex, 0, 0, 120, 50, &rc, &color, Math::Vector2(0, 0.5f));
			}
			else if (m_spPlayer->GetWeaponType() & lGrassHopper && m_spPlayer->GetLGrassHopperPauCnt() != 0)
			{
				transMat = Math::Matrix::CreateTranslation(-410, -250, 0);
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				Math::Rectangle rc = { 0,0,m_spPlayer->GetLGrassHopperPauCnt() * 4,50 };
				color = { 1,1,1,0.6f };
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponTypeOvreDarkTex, 0, 0, m_spPlayer->GetLGrassHopperPauCnt() * 4, 50, &rc, &color, Math::Vector2(0, 0.5f));
			}

			transMat = Math::Matrix::CreateTranslation(-630, 300, 0);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			Math::Rectangle rc = { 0,0,400,50 };
			color = { 1, 1, 1, 1 };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_enduranceBarTex, 0, 0, 400, 50, &rc, &color, Math::Vector2(0, 0.5f));

			transMat = Math::Matrix::CreateTranslation(-630, 300, 0);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			rc = { 0,0,(int)(m_spPlayer->GetEndurance()),50 };
			color = { 1, 1, 1, 1 };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_enduranceTex, 0, 0, (int)(m_spPlayer->GetEndurance()), 50, &rc, &color, Math::Vector2(0, 0.5f));

			transMat = Math::Matrix::CreateTranslation(-630, 255, 0);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			rc = { 0,0,320,40 };
			color = { 1, 1, 1, 1 };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_torionBarTex, 0, 0, 320, 40, &rc, &color, Math::Vector2(0, 0.5f));

			transMat = Math::Matrix::CreateTranslation(-620, 255, 0);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			rc = { 0,0,(int)(m_spPlayer->GetTorion()),30 };
			color = { 1, 1, 1, 1 };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_torionTex, 0, 0, (int)(m_spPlayer->GetTorion()), 30, &rc, &color, Math::Vector2(0, 0.5f));

		}

		if (m_tutorialType != sonotaTu)
		{
			transMat = Math::Matrix::CreateTranslation(450, -300, 0);
			switch (m_tutorialCnt)
			{
			case 0:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point0Tex, 0, 0, 102, 114);
				break;
			case 1:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point1Tex, 0, 0, 102, 114);
				break;
			case 2:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point2Tex, 0, 0, 102, 114);
				break;
			case 3:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point3Tex, 0, 0, 102, 114);
				break;
			case 4:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point4Tex, 0, 0, 102, 114);
				break;
			case 5:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point5Tex, 0, 0, 102, 114);
				break;
			}

			transMat = Math::Matrix::CreateTranslation(550, -300, 0);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point5Tex, 0, 0, 102, 114);
		}

		if (m_bOption)
		{
			transMat = Math::Matrix::Identity;
			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			color = { 0,0,0,0.2f };
			KdShaderManager::Instance().m_spriteShader.DrawBox(0, 0, 1280, 720, &color);

			if (!m_bInfo)
			{
				mat = Math::Matrix::CreateScale(m_infoScale) * Math::Matrix::CreateTranslation(m_infoPos);
				KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_infoTex, 0, 0, 700, 120);

				mat = Math::Matrix::CreateScale(m_titleScale) * Math::Matrix::CreateTranslation(m_titlePos);
				KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_titleTex, 0, 0, 700, 120);

				mat = Math::Matrix::CreateScale(m_exitScale) * Math::Matrix::CreateTranslation(m_exitPos);
				KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_exitTex, 0, 0, 700, 120);
			}
			else if (m_bInfo)
			{
				if (m_bHowToPage)
				{
					transMat = Math::Matrix::Identity;
					KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_howToTex, 0, 0, 1250, 500);
				}

				if (m_bWeaponDataPage)
				{
					if (m_bWeaponDataHopperPage)
					{
						transMat = Math::Matrix::Identity;
						KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
						KdShaderManager::Instance().m_spriteShader.DrawTex(&m_hopperDataTex, 0, 0, 1156, 260);
					}
					else if (m_bWeaponDataScoPage)
					{
						transMat = Math::Matrix::Identity;
						KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
						KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponInfoTex, 0, 0, 1151, 431);
					}

					mat = Math::Matrix::CreateScale(m_weaponRightYaiScale) * Math::Matrix::CreateTranslation(m_weaponRightYaiPos);
					KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_rightYaiTex, 0, 0, 70, 55);

					mat = Math::Matrix::CreateScale(m_weaponLeftYaiScale) * Math::Matrix::CreateTranslation(m_weaponLeftYaiPos);
					KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_leftYaiTex, 0, 0, 70, 55);
				}

				mat = Math::Matrix::CreateScale(m_weaOrHowRightYaiScale) * Math::Matrix::CreateTranslation(m_weaOrHowRightYaiPos);
				KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_rightYaiTex, 0, 0, 70, 55);

				mat = Math::Matrix::CreateScale(m_weaOrHowLeftYaiScale) * Math::Matrix::CreateTranslation(m_weaOrHowLeftYaiPos);
				KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_leftYaiTex, 0, 0, 70, 55);

			}

			mat = Math::Matrix::CreateScale(m_backScale) * Math::Matrix::CreateTranslation(m_backPos);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_backTex, 0, 0, 182, 80);
		}

		if (m_bTutorialView)
		{
			transMat = Math::Matrix::Identity;
			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			color = { 0,0.,0,0.4f };
			KdShaderManager::Instance().m_spriteShader.DrawBox(0, 0, 1280, 720, &color);

			switch (m_tutorialType)
			{
			case kihonTu:
				mat = Math::Matrix::CreateScale(m_backScale) * Math::Matrix::CreateTranslation(0,0,0);
				KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_tyuKihonTex, 0, 0, 1000, 500);
				break;
			case bukiTu:
				mat = Math::Matrix::CreateScale(m_backScale) * Math::Matrix::CreateTranslation(0, 0, 0);
				KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_tyubukiTex, 0, 0, 1000, 500);
				break;
			case sukoADMoveTu:
				mat = Math::Matrix::CreateScale(m_backScale) * Math::Matrix::CreateTranslation(0, 0, 0);
				KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_sukoADMoveTex, 0, 0, 1000, 500);
				break;
			case hopperTu:
				mat = Math::Matrix::CreateScale(m_backScale) * Math::Matrix::CreateTranslation(0, 0, 0);
				KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_hopperTyuTex, 0, 0, 1000, 500);
				break;
			case sonotaTu:
				mat = Math::Matrix::CreateScale(m_backScale) * Math::Matrix::CreateTranslation(0, 0, 0);
				KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_sonotaTyuTex, 0, 0, 1000, 500);
				break;
			}
		}

		transMat = Math::Matrix::Identity;
		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		color = { 0,0,0,m_fadeAlpha };
		KdShaderManager::Instance().m_spriteShader.DrawBox(0, 0, 1280, 720, &color);

		break;
	case UiType::result:
		if (SceneManager::Instance().GetBPlayerWin())
		{
			transMat = Math::Matrix::Identity;
			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			color = { 0.15f,0.15f,0.7f,1.0f };
			KdShaderManager::Instance().m_spriteShader.DrawBox(0, 0, 1280, 720, &color);

		}
		else
		{
			transMat = Math::Matrix::Identity;
			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			color = { 0.4f,0.1f,0.1f,1.0f };
			KdShaderManager::Instance().m_spriteShader.DrawBox(0, 0, 1280, 720, &color);
		}

		if (m_time >= 60)
		{
			int ieftover = 0;
			int ieftoverExcess = 0;

			if (SceneManager::Instance().GetBPlayerWin())
			{
				transMat = Math::Matrix::CreateTranslation(0, 225, 0);
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_winTex, 0, 0, 246, 114);

				transMat = Math::Matrix::CreateTranslation(425, 30, 0);
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_winCharaTex, 0, 0, 246, 461);

				switch (SceneManager::Instance().GetEnemyTotal())
				{
				case 1:
					transMat = Math::Matrix::CreateTranslation(-400, 0, 0);
					KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, 313, 165);
					break;
				case 2:
					switch (SceneManager::Instance().GetEnemyIeftover())
					{
					case 0:
						if (m_time >= 80)
						{
							transMat = Math::Matrix::CreateTranslation(-400, 100, 0);
							KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
							KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, 313, 165);
						}

						if (m_time >= 100)
						{
							transMat = Math::Matrix::CreateTranslation(-400, -100, 0);
							KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
							KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, 313, 165);
						}
						break;
					case 1:
						transMat = Math::Matrix::CreateTranslation(-400, 0, 0);
						KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
						KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, 313, 165);
						break;
					}
					break;
				case 3:
					switch (SceneManager::Instance().GetEnemyIeftover())
					{
					case 0:
						if (m_time >= 70)
						{
							transMat = Math::Matrix::CreateTranslation(-400, 200, 0);
							KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
							KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, 313, 165);
						}

						if (m_time >= 90)
						{
							transMat = Math::Matrix::CreateTranslation(-400, 0, 0);
							KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
							KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, 313, 165);
						}

						if (m_time >= 110)
						{
							transMat = Math::Matrix::CreateTranslation(-400, -200, 0);
							KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
							KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, 313, 165);
						}
						break;
					case 1:
						if (m_time >= 80)
						{
							transMat = Math::Matrix::CreateTranslation(-400, 100, 0);
							KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
							KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, 313, 165);
						}

						if (m_time >= 100)
						{
							transMat = Math::Matrix::CreateTranslation(-400, -100, 0);
							KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
							KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, 313, 165);
						}
						break;
					case 2:
						transMat = Math::Matrix::CreateTranslation(-400, 0, 0);
						KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
						KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, 313, 165);
						break;
					}
					break;
				case 50:
					ieftover       = (50 - SceneManager::Instance().GetEnemyIeftover()) / 5;
					ieftoverExcess = (50 - SceneManager::Instance().GetEnemyIeftover()) % 5;

					for (int i = 0; i < ieftover; i++)
					{
						for (int j = 0; j < 5; j++)
						{
							transMat = Math::Matrix::CreateScale(0.2f) * Math::Matrix::CreateTranslation((float)( -600 + 80 * j), (float)(320 - 70 * i), 0);
							KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
							KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, 313, 165);
						}
					}

					for (int k = 0; k < ieftoverExcess; k++)
					{
						transMat = Math::Matrix::CreateScale(0.2f) * Math::Matrix::CreateTranslation((float)(-600 + 80 * k), (float)(320 - 70 * ieftover), 0);
						KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
						KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, 313, 165);
					}

					break;
				case 100:
					ieftover = (100 - SceneManager::Instance().GetEnemyIeftover()) / 5;
					ieftoverExcess = (100 - SceneManager::Instance().GetEnemyIeftover()) % 5;

					for (int i = 0; i < ieftover; i++)
					{
						for (int j = 0; j < 5; j++)
						{
							transMat = Math::Matrix::CreateScale(0.075f) * Math::Matrix::CreateTranslation((float)(-600 + 80 * j), (float)(320 - 35 * i), 0);
							KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
							KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, 313, 165);
						}
					}

					for (int k = 0; k < ieftoverExcess; k++)
					{
						transMat = Math::Matrix::CreateScale(0.075f) * Math::Matrix::CreateTranslation((float)(-600 + 80 * k), (float)(320 - 35 * ieftover), 0);
						KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
						KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, 313, 165);
					}

					break;
				}

			}
			else
			{
				transMat = Math::Matrix::CreateTranslation(0, 225, 0);
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_lossTex, 0, 0, 512, 172);

				transMat = Math::Matrix::CreateTranslation(400, 0, 0);
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, 313, 165);

				switch (SceneManager::Instance().GetEnemyTotal())
				{
				case 1:
					transMat = Math::Matrix::CreateTranslation(-425, 30, 0);
					KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
					KdShaderManager::Instance().m_spriteShader.DrawTex(&m_winCharaTex, 0, 0, 246, 461);
					break;
				case 2:
					if (m_time >= 80)
					{
						transMat = Math::Matrix::CreateTranslation(-405, 30, 0);
						KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
						KdShaderManager::Instance().m_spriteShader.DrawTex(&m_winCharaTex, 0, 0, 246, 461);
					}
						
					if (m_time >= 100)
					{
						transMat = Math::Matrix::CreateTranslation(-485, 30, 0);
						KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
						KdShaderManager::Instance().m_spriteShader.DrawTex(&m_winCharaTex, 0, 0, 246, 461);
					}
					break;
				case 3:
					if (m_time >= 70)
					{
						transMat = Math::Matrix::CreateTranslation(-340, 30, 0);
						KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
						KdShaderManager::Instance().m_spriteShader.DrawTex(&m_winCharaTex, 0, 0, 246, 461);
					}

					if (m_time >= 90)
					{
						transMat = Math::Matrix::CreateTranslation(-485, 30, 0);
						KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
						KdShaderManager::Instance().m_spriteShader.DrawTex(&m_winCharaTex, 0, 0, 246, 461);
					}

					if (m_time >= 110)
					{
						transMat = Math::Matrix::CreateTranslation(-420, 30, 0);
						KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
						KdShaderManager::Instance().m_spriteShader.DrawTex(&m_winCharaTex, 0, 0, 246, 461);
					}

					break;
				case 50:
					ieftover = (50 - SceneManager::Instance().GetEnemyIeftover()) / 5;
					ieftoverExcess = (50 - SceneManager::Instance().GetEnemyIeftover()) % 5;

					for (int i = 0; i < ieftover; i++)
					{
						for (int j = 0; j < 5; j++)
						{
							transMat = Math::Matrix::CreateScale(0.2f) * Math::Matrix::CreateTranslation((float)(-600 + 80 * j), (float)(320 - 70 * i), 0);
							KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
							KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, 313, 165);
						}
					}

					for (int k = 0; k < ieftoverExcess; k++)
					{
						transMat = Math::Matrix::CreateScale(0.2f) * Math::Matrix::CreateTranslation((float)(-600 + 80 * k), (float)(320 - 70 * ieftover), 0);
						KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
						KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, 313, 165);
					}

					break;
				case 100:
					ieftover = (50 - SceneManager::Instance().GetEnemyIeftover()) / 5;
					ieftoverExcess = (50 - SceneManager::Instance().GetEnemyIeftover()) % 5;

					for (int i = 0; i < ieftover; i++)
					{
						for (int j = 0; j < 5; j++)
						{
							transMat = Math::Matrix::CreateScale(0.075f) * Math::Matrix::CreateTranslation((float)(-600 + 80 * j), (float)(320 - 35 * i), 0);
							KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
							KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, 313, 165);
						}
					}

					for (int k = 0; k < ieftoverExcess; k++)
					{
						transMat = Math::Matrix::CreateScale(0.075f) * Math::Matrix::CreateTranslation((float)(-600 + 80 * k), (float)(320 - 35 * ieftover), 0);
						KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
						KdShaderManager::Instance().m_spriteShader.DrawTex(&m_loseCharaTex, 0, 0, 313, 165);
					}

					break;
				}
			}
		}

		if (m_time >= 120)
		{
			transMat = Math::Matrix::CreateTranslation(-125, 45, 0);
			switch ((m_pointAddOrSubVal / 1000) % 10)
			{
			case 0:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point0Tex, 0, 0, 102, 114);
				break;
			case 1:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point1Tex, 0, 0, 102, 114);
				break;
			case 2:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point2Tex, 0, 0, 102, 114);
				break;
			case 3:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point3Tex, 0, 0, 102, 114);
				break;
			case 4:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point4Tex, 0, 0, 102, 114);
				break;
			case 5:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point5Tex, 0, 0, 102, 114);
				break;
			case 6:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point6Tex, 0, 0, 102, 114);
				break;
			case 7:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point7Tex, 0, 0, 102, 114);
				break;
			case 8:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point8Tex, 0, 0, 102, 114);
				break;
			case 9:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point9Tex, 0, 0, 102, 114);
				break;
			default:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point0Tex, 0, 0, 102, 114);
				break;
			}

			transMat = Math::Matrix::CreateTranslation(-45, 45, 0);
			switch ((m_pointAddOrSubVal / 100) % 10)
			{
			case 0:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point0Tex, 0, 0, 102, 114);
				break;
			case 1:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point1Tex, 0, 0, 102, 114);
				break;
			case 2:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point2Tex, 0, 0, 102, 114);
				break;
			case 3:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point3Tex, 0, 0, 102, 114);
				break;
			case 4:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point4Tex, 0, 0, 102, 114);
				break;
			case 5:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point5Tex, 0, 0, 102, 114);
				break;
			case 6:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point6Tex, 0, 0, 102, 114);
				break;
			case 7:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point7Tex, 0, 0, 102, 114);
				break;
			case 8:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point8Tex, 0, 0, 102, 114);
				break;
			case 9:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point9Tex, 0, 0, 102, 114);
				break;
			default:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point0Tex, 0, 0, 102, 114);
				break;
			}

			transMat = Math::Matrix::CreateTranslation(35, 45, 0);
			switch ((m_pointAddOrSubVal / 10) % 10)
			{
			case 0:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point0Tex, 0, 0, 102, 114);
				break;
			case 1:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point1Tex, 0, 0, 102, 114);
				break;
			case 2:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point2Tex, 0, 0, 102, 114);
				break;
			case 3:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point3Tex, 0, 0, 102, 114);
				break;
			case 4:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point4Tex, 0, 0, 102, 114);
				break;
			case 5:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point5Tex, 0, 0, 102, 114);
				break;
			case 6:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point6Tex, 0, 0, 102, 114);
				break;
			case 7:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point7Tex, 0, 0, 102, 114);
				break;
			case 8:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point8Tex, 0, 0, 102, 114);
				break;
			case 9:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point9Tex, 0, 0, 102, 114);
				break;
			default:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point0Tex, 0, 0, 102, 114);
				break;
			}

			transMat = Math::Matrix::CreateTranslation(125, 45, 0);
			switch ((m_pointAddOrSubVal / 1) % 10)
			{
			case 0:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point0Tex, 0, 0, 102, 114);
				break;
			case 1:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point1Tex, 0, 0, 102, 114);
				break;
			case 2:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point2Tex, 0, 0, 102, 114);
				break;
			case 3:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point3Tex, 0, 0, 102, 114);
				break;
			case 4:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point4Tex, 0, 0, 102, 114);
				break;
			case 5:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point5Tex, 0, 0, 102, 114);
				break;
			case 6:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point6Tex, 0, 0, 102, 114);
				break;
			case 7:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point7Tex, 0, 0, 102, 114);
				break;
			case 8:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point8Tex, 0, 0, 102, 114);
				break;
			case 9:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point9Tex, 0, 0, 102, 114);
				break;
			default:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point0Tex, 0, 0, 102, 114);
				break;
			}
		}

		if (m_time >= 180)
		{
			transMat = Math::Matrix::CreateTranslation(-205, -80, 0);
			switch ((m_weaponPoint / 10000) % 10)
			{
			case 1:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point1Tex, 0, 0, 102, 114);
				break;
			case 2:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point2Tex, 0, 0, 102, 114);
				break;
			case 3:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point3Tex, 0, 0, 102, 114);
				break;
			case 4:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point4Tex, 0, 0, 102, 114);
				break;
			case 5:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point5Tex, 0, 0, 102, 114);
				break;
			case 6:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point6Tex, 0, 0, 102, 114);
				break;
			case 7:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point7Tex, 0, 0, 102, 114);
				break;
			case 8:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point8Tex, 0, 0, 102, 114);
				break;
			case 9:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point9Tex, 0, 0, 102, 114);
				break;
			}

			transMat = Math::Matrix::CreateTranslation(-125, -80, 0);
			switch ((m_weaponPoint / 1000) % 10)
			{
			case 0:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point0Tex, 0, 0, 102, 114);
				break;
			case 1:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point1Tex, 0, 0, 102, 114);
				break;
			case 2:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point2Tex, 0, 0, 102, 114);
				break;
			case 3:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point3Tex, 0, 0, 102, 114);
				break;
			case 4:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point4Tex, 0, 0, 102, 114);
				break;
			case 5:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point5Tex, 0, 0, 102, 114);
				break;
			case 6:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point6Tex, 0, 0, 102, 114);
				break;
			case 7:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point7Tex, 0, 0, 102, 114);
				break;
			case 8:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point8Tex, 0, 0, 102, 114);
				break;
			case 9:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point9Tex, 0, 0, 102, 114);
				break;
			default:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point0Tex, 0, 0, 102, 114);
				break;
			}

			transMat = Math::Matrix::CreateTranslation(-45, -80, 0);
			switch ((m_weaponPoint / 100) % 10)
			{
			case 0:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point0Tex, 0, 0, 102, 114);
				break;
			case 1:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point1Tex, 0, 0, 102, 114);
				break;
			case 2:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point2Tex, 0, 0, 102, 114);
				break;
			case 3:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point3Tex, 0, 0, 102, 114);
				break;
			case 4:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point4Tex, 0, 0, 102, 114);
				break;
			case 5:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point5Tex, 0, 0, 102, 114);
				break;
			case 6:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point6Tex, 0, 0, 102, 114);
				break;
			case 7:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point7Tex, 0, 0, 102, 114);
				break;
			case 8:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point8Tex, 0, 0, 102, 114);
				break;
			case 9:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point9Tex, 0, 0, 102, 114);
				break;
			default:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point0Tex, 0, 0, 102, 114);
				break;
			}

			transMat = Math::Matrix::CreateTranslation(35, -80, 0);
			switch ((m_weaponPoint / 10) % 10)
			{
			case 0:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point0Tex, 0, 0, 102, 114);
				break;
			case 1:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point1Tex, 0, 0, 102, 114);
				break;
			case 2:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point2Tex, 0, 0, 102, 114);
				break;
			case 3:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point3Tex, 0, 0, 102, 114);
				break;
			case 4:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point4Tex, 0, 0, 102, 114);
				break;
			case 5:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point5Tex, 0, 0, 102, 114);
				break;
			case 6:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point6Tex, 0, 0, 102, 114);
				break;
			case 7:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point7Tex, 0, 0, 102, 114);
				break;
			case 8:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point8Tex, 0, 0, 102, 114);
				break;
			case 9:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point9Tex, 0, 0, 102, 114);
				break;
			default:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point0Tex, 0, 0, 102, 114);
				break;
			}

			transMat = Math::Matrix::CreateTranslation(125, -80, 0);
			switch ((m_weaponPoint / 1) % 10)
			{
			case 0:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point0Tex, 0, 0, 102, 114);
				break;
			case 1:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point1Tex, 0, 0, 102, 114);
				break;
			case 2:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point2Tex, 0, 0, 102, 114);
				break;
			case 3:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point3Tex, 0, 0, 102, 114);
				break;
			case 4:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point4Tex, 0, 0, 102, 114);
				break;
			case 5:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point5Tex, 0, 0, 102, 114);
				break;
			case 6:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point6Tex, 0, 0, 102, 114);
				break;
			case 7:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point7Tex, 0, 0, 102, 114);
				break;
			case 8:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point8Tex, 0, 0, 102, 114);
				break;
			case 9:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point9Tex, 0, 0, 102, 114);
				break;
			default:
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Point0Tex, 0, 0, 102, 114);
				break;
			}
		}

		if (m_pointAddOrSubVal < 0)
		{
			transMat = Math::Matrix::CreateTranslation(0, -250, 0);
			Math::Rectangle rc = { 0,0,894,114 };
			color = { 1,1,1,m_pushToEnterAlpha };
			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_pushToEnterTex, 0, 0, 894, 114, &rc, &color);
		}

		transMat = Math::Matrix::Identity;
		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		color = { 0,0,0,m_fadeAlpha };
		KdShaderManager::Instance().m_spriteShader.DrawBox(0, 0, 1280, 720, &color);
		break;
	case UiType::select:
		mat = Math::Matrix::Identity;
		mat = Math::Matrix::CreateTranslation(m_selectBackPos);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_selectBackTex, 0, 0, 1280, 720);

		mat = Math::Matrix::CreateScale(m_gameScale) * Math::Matrix::CreateTranslation(m_gamePos);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_gameTex, 0, 0, 420, 580);

		mat = Math::Matrix::CreateScale(m_oneEnemyScale) * Math::Matrix::CreateTranslation(m_oneEnemyTotalPos);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_oneEnemyTotalTex, 0, 0, 210, 80);

		mat = Math::Matrix::CreateScale(m_twoEnemyScale) * Math::Matrix::CreateTranslation(m_twoEnemyTotalPos);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_twoEnemyTotalTex, 0, 0, 210, 80);

		mat = Math::Matrix::CreateScale(m_threeEnemyScale) * Math::Matrix::CreateTranslation(m_threeEnemyTotalPos);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_threeEnemyTotalTex, 0, 0, 210, 80);

		mat = Math::Matrix::CreateScale(m_BattlehelpMkScale) * Math::Matrix::CreateTranslation(m_BattlehelpMkPos);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_helpMkTex, 0, 0, 50, 50);

		mat = Math::Matrix::CreateTranslation(m_challengePos);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_challengeTex, 0, 0, 420, 580);

		mat = Math::Matrix::CreateScale(m_chalenge50Scale) * Math::Matrix::CreateTranslation(m_chalenge50Pos);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_chalenge50Tex, 0, 0, 210, 80);

		mat = Math::Matrix::CreateScale(m_chalenge100Scale) * Math::Matrix::CreateTranslation(m_chalenge100Pos);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_chalenge100Tex, 0, 0, 210, 80);

		mat = Math::Matrix::CreateScale(m_chalengehelpMkScale) * Math::Matrix::CreateTranslation(m_chalengehelpMkPos);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_helpMkTex, 0, 0, 50, 50);

		mat = Math::Matrix::CreateScale(m_tutorialScale) * Math::Matrix::CreateTranslation(m_tutorialPos);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_tutorialTex, 0, 0, 150, 265);

		mat = Math::Matrix::CreateScale(m_trainingScale) * Math::Matrix::CreateTranslation(m_trainingPos);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_trainingTex, 0, 0, 150, 265);

		mat = Math::Matrix::CreateScale(m_titleScale) * Math::Matrix::CreateTranslation(m_titlePos);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_selectTitleTex, 0, 0, 100, 50);

		mat = Math::Matrix::CreateScale(m_exitScale) * Math::Matrix::CreateTranslation(m_exitPos);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_selectExitTex, 0, 0, 100, 50);

		mat = Math::Matrix::Identity;
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		color = { 0,0,0,m_fadeAlpha };
		KdShaderManager::Instance().m_spriteShader.DrawBox(0, 0, 1280, 720, &color);

		if (m_bBattlehelp)
		{
			KdShaderManager::Instance().m_spriteShader.SetMatrix(Math::Matrix::Identity);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_battlehelpTex, 0, 0, 880, 520);

			mat = Math::Matrix::CreateScale(m_backScale) * Math::Matrix::CreateTranslation(m_backPos);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_backTex, 0, 0, 182, 80);
		}
		else if (m_bChalengehelp)
		{
			KdShaderManager::Instance().m_spriteShader.SetMatrix(Math::Matrix::Identity);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_chalengebattlehelpTex, 0, 0, 880, 520);

			mat = Math::Matrix::CreateScale(m_backScale) * Math::Matrix::CreateTranslation(m_backPos);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_backTex, 0, 0, 182, 80);
		}

		break;
	}

	transMat = Math::Matrix::Identity;
	KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
}

void Ui::DrawUnLit()
{
	//for (auto& list : m_wpEnemyList)
	//{
	//	Math::Vector3 pos = list.lock()->GetPos();
	//	Math::Matrix scaleMat = Math::Matrix::CreateScale(0.001 * pos.z);
	//	pos.z = 0.0f;
	//	Math::Matrix mat = scaleMat * Math::Matrix::CreateTranslation(pos);
	//	/*mat.Backward(m_wpCamera.lock()->GetCamera()->GetCameraMatrix().Backward());
	//	mat.Up(m_wpCamera.lock()->GetCamera()->GetCameraMatrix().Up());
	//	mat.Right(m_wpCamera.lock()->GetCamera()->GetCameraMatrix().Right());*/

	//	KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
	//	Math::Rectangle rc = { 0,0,(int)(list.lock()->GetEndurance()),50 };
	//	Math::Color color = { 1, 1, 1, 1 };
	//	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_enduranceTex, 0, 0, (int)(list.lock()->GetEndurance()), 50, &rc, &color, Math::Vector2(0, 0.5f));

	//	rc = { 0,0,400,50 };
	//	color = { 1, 1, 1, 1 };
	//	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_enduranceBarTex, 0, 0, 400, 50, &rc, &color, Math::Vector2(0, 0.5f));
	//}
}

void Ui::AddEnemy(std::shared_ptr<Enemy> a_spEnemy)
{
	m_wpEnemyList.push_back(a_spEnemy);
	m_enemyScPosList.push_back(Math::Vector2::Zero);
}

void Ui::AddTutorialCnt()
{
	if (m_tutorialCnt < 5)
	{
		m_tutorialCnt++;
	}
}

void Ui::Init()
{
	m_titleCharRogoTex.Load("Asset/Textures/Ui/Title/KillSC2.png");
	m_FTtoyoRogoTex.Load("Asset/Textures/Ui/Title/FTtoyo.png");

	m_weaponType1Tex.Load("Asset/Textures/Ui/Game/WeaponType1.png");
	m_weaponType2Tex.Load("Asset/Textures/Ui/Game/WeaponType2.png");
	m_weaponTypeOvreDarkTex.Load("Asset/Textures/Ui/Game/WeaponTypeOvreDark.png");

	m_torionTex.Load("Asset/Textures/Ui/Game/Torion.png");
	m_torionBarTex.Load("Asset/Textures/Ui/Game/TorionBar.png");

	m_enduranceBarTex.Load("Asset/Textures/Ui/Game/enduranceBar.png");
	m_enduranceTex.Load("Asset/Textures/Ui/Game/endurance.png");

	m_winTex.Load("Asset/Textures/Ui/Result/WIN.png");
	m_lossTex.Load("Asset/Textures/Ui/Result/LOSS.png");
	m_Point0Tex.Load("Asset/Textures/Ui/Result/p0.png");
	m_Point1Tex.Load("Asset/Textures/Ui/Result/p1.png");
	m_Point2Tex.Load("Asset/Textures/Ui/Result/p2.png");
	m_Point3Tex.Load("Asset/Textures/Ui/Result/p3.png");
	m_Point4Tex.Load("Asset/Textures/Ui/Result/p4.png");
	m_Point5Tex.Load("Asset/Textures/Ui/Result/p5.png");
	m_Point6Tex.Load("Asset/Textures/Ui/Result/p6.png");
	m_Point7Tex.Load("Asset/Textures/Ui/Result/p7.png");
	m_Point8Tex.Load("Asset/Textures/Ui/Result/p8.png");
	m_Point9Tex.Load("Asset/Textures/Ui/Result/p9.png");

	m_pushToEnterTex.Load("Asset/Textures/Ui/shared/PushToEnter.png");

	m_exitTex.Load("Asset/Textures/Ui/OPTION/EXIT.png");
	m_optionTex.Load("Asset/Textures/Ui/OPTION/OPTION.png");
	m_titleTex.Load("Asset/Textures/Ui/OPTION/TITLE.png");

	m_countOneTex.Load("Asset/Textures/Ui/Game/one.png");
	m_countTwoTex.Load("Asset/Textures/Ui/Game/two.png");
	m_countThreeTex.Load("Asset/Textures/Ui/Game/three.png");
	m_countGoTex.Load("Asset/Textures/Ui/Game/GO.png");

	m_backTex.Load("Asset/Textures/Ui/shared/Back.png");

	m_hopperDataTex.Load("Asset/Textures/Ui/Game/hopperData.png");
	m_weaponDataTex.Load("Asset/Textures/Ui/Game/weaponData.png");
	m_weaponInfoTex.Load("Asset/Textures/Ui/Game/weaponInfo.png");
	m_leftYaiTex.Load("Asset/Textures/Ui/Game/leftYaji.png");
	m_rightYaiTex.Load("Asset/Textures/Ui/Game/rightYaji.png");
	m_howToTex.Load("Asset/Textures/Ui/Game/how-to.png");
	m_infoTex.Load("Asset/Textures/Ui/OPTION/info.png");

	m_hopperTyuTex.Load("Asset/Textures/Ui/Tutorial/hopperTyu.png");
	m_sonotaTyuTex.Load("Asset/Textures/Ui/Tutorial/sonotaTyu.png");
	m_sukoADMoveTex.Load("Asset/Textures/Ui/Tutorial/sukoADMove.png");
	m_tyubukiTex.Load("Asset/Textures/Ui/Tutorial/tyubuki.png");
	m_tyuKihonTex.Load("Asset/Textures/Ui/Tutorial/tyuKihon.png");

	m_selectTitleTex.Load("Asset/Textures/Ui/Select/title.png");
	m_selectExitTex.Load("Asset/Textures/Ui/Select/exit.png");
	m_gameTex.Load("Asset/Textures/Ui/Select/Battle.png");
	m_challengeTex.Load("Asset/Textures/Ui/Select/Challenge.png");
	m_selectBackTex.Load("Asset/Textures/Ui/Select/SelectBack.png");
	m_trainingTex.Load("Asset/Textures/Ui/Select/Training.png");
	m_tutorialTex.Load("Asset/Textures/Ui/Select/Tutorial.png");


	m_oneEnemyTotalTex.Load("Asset/Textures/Ui/Select/oneEnemy.png");
	m_twoEnemyTotalTex.Load("Asset/Textures/Ui/Select/TwoEnemy.png");
	m_threeEnemyTotalTex.Load("Asset/Textures/Ui/Select/threeEnemy.png");

	m_helpMkTex.Load("Asset/Textures/Ui/Select/help.png");
	m_battlehelpTex.Load("Asset/Textures/Ui/Select/BattleHelp.png");
	//m_chalengebattlehelpTex.Load("Asset/Textures/Ui/Select/threeEnemy.png");

	m_time0Tex.Load("Asset/Textures/Ui/shared/Zero.png");
	m_time1Tex.Load("Asset/Textures/Ui/shared/One.png");
	m_time2Tex.Load("Asset/Textures/Ui/shared/Two.png");
	m_time3Tex.Load("Asset/Textures/Ui/shared/Three.png");
	m_time4Tex.Load("Asset/Textures/Ui/shared/Four.png");
	m_time5Tex.Load("Asset/Textures/Ui/shared/Five.png");
	m_time6Tex.Load("Asset/Textures/Ui/shared/Six.png");
	m_time7Tex.Load("Asset/Textures/Ui/shared/Seven.png");
	m_time8Tex.Load("Asset/Textures/Ui/shared/Eight.png");
	m_time9Tex.Load("Asset/Textures/Ui/shared/Nine.png");
	m_conmaTex.Load("Asset/Textures/Ui/shared/Conma.png");

	m_winCharaTex.Load("Asset/Textures/Ui/Result/winChara.png");
	m_loseCharaTex.Load("Asset/Textures/Ui/Result/loseChara.png");

	m_chalenge50Tex.Load("Asset/Textures/Ui/Select/Challenge50.png");
	m_chalenge100Tex.Load("Asset/Textures/Ui/Select/Challenge100.png");

	m_fadeAlpha = 1.0f;
	m_addFadeAlpha = false;
	m_bFirstInResult = true;
	m_bSceneCangePossible = false;
	m_pushToEnterAlpha = 1.0f;
	m_bPushToEnterAlphaAdd = false;

	m_tutorialScale = 1.0f;
	m_gameScale = 1.0f;
	m_challengeScale = 0;
	m_trainingScale = 0;

	m_titleScale  = 1.0f;
	m_optionScale = 1.0f;
	m_exitScale   = 1.0f;

	m_optionPos = { 550,-125,0 };
	m_titlePos =  { 550,-180,0 };
	m_exitPos =   { 550,-250,0 };

	m_gamePos            = { -195,  -20, 0};
	m_oneEnemyTotalPos   = { -195,  -80, 0};
	m_twoEnemyTotalPos   = { -195, -170, 0};
	m_threeEnemyTotalPos = { -195, -260, 0};
	m_BattlehelpMkPos    = {  -35,  235, 0};

	m_challengePos       = {  265,  -20, 0};
	m_chalenge50Pos      = {  265, -180, 0 };
	m_chalenge100Pos     = {  265, -270, 0 };
	m_chalengehelpMkPos  = {  425, 235, 0};
	m_tutorialPos        = { -535, 138, 0};
	m_trainingPos        = { -535,-178, 0};
	m_selectBackPos      = {    0,   0, 0};


	m_chalenge50Scale = 1.0f;
	m_chalenge100Scale = 1.0f;

	m_bChalenge50 = false;
	m_bChalenge100 = false;
	m_bTutorial = false;
	m_bExit = false;
	m_bFirstExit = false;
	m_bGame = false;
	m_bOption = false;
	m_bTitle = false;
	m_bOneEnemyTotal = false;
	m_bTwoEnemyTotal = false;
	m_bThreeEnemyTotal = false;


	m_countOneScale = 0.1f;
	m_countOneAlpha = 0.0f;

	m_countTwoScale = 0.1f;
	m_countTwoAlpha = 0.0f;

	m_countThreeScale = 0.1f;
	m_countThreeAlpha = 0.0f;

	m_countGoScale = 0.1f;
	m_countGoAlpha = 0.0f;
	m_bFirstExit = true;

	m_backPos = { 550,-300,0 };
	m_backScale = 0.8f;

	m_bTABKey = false;

	m_bWeaponDataPage = false;
	m_bWeaponDataHopperPage = false;
	m_bWeaponDataScoPage = true;
	m_bHowToPage = true;

	m_weaponLeftYaiScale = 1.0f;
	m_weaponLeftYaiPos = { -500,250,0 };

	m_weaponRightYaiScale = 1.0f;
	m_weaponRightYaiPos = { 500,250,0, };

	m_weaOrHowLeftYaiScale = 1.0f;
	m_weaOrHowLeftYaiPos = { -600,0,0 };

	m_weaOrHowRightYaiScale = 1.0f;
	m_weaOrHowRightYaiPos = { 600,0,0 };

	m_infoScale = 1.0f;
	m_infoPos = { 0,250,0 };
	m_bInfo = false;
	bLButtonKey = false;

	m_tutorialType = kihonTu;
	m_bTutorialView = true;
	m_tutorialSwitchAfterTime = 120;

	m_BattlehelpMkScale = 1.0f;
	m_bBattlehelp = false;

	m_chalengehelpMkScale = 1.0f;
	m_bChalengehelp = false;
	m_gameTimeCntDeray = 0;

}
