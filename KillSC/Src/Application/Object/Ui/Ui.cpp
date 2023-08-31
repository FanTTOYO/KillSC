#include "Ui.h"
#include "../../main.h"
#include "../../Scene/SceneManager.h"
#include "../Character/Player/Player.h"

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
	case UiType::result:
		ResultUpdate();
		break;
	case UiType::select:
		SelectUpdate();
		break;
	case UiType::art:
		ArtUpdate();
		break;
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
			float mouseX = (float)mousePos.x;
			float mouseY = (float)mousePos.y;

			if (!m_bInfo)
			{
				Dis = m_exitPos - Math::Vector3(mouseX, mouseY, 0.0f);
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

				Dis = m_titlePos - Math::Vector3(mouseX, mouseY, 0.0f);
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

				Dis = m_infoPos - Math::Vector3(mouseX, mouseY, 0.0f);
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

				Dis = m_backPos - Math::Vector3(mouseX, mouseY, 0.0f);
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

				Dis = m_backPos - Math::Vector3(mouseX, mouseY, 0.0f);
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

				Dis = m_weaOrHowLeftYaiPos - Math::Vector3(mouseX, mouseY, 0.0f);
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
					Dis = m_weaponLeftYaiPos - Math::Vector3(mouseX, mouseY, 0.0f);
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

					Dis = m_weaponRightYaiPos - Math::Vector3(mouseX, mouseY, 0.0f);
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

		KdSafeDelete(pwi);
	}
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
				KdAudioManager::Instance().Play("Asset/Audio/SE/AddAndSub.wav",true);
			}

			if (m_pointAddOrSubVal > 0)
			{
				m_pointAddOrSubVal--;
				m_weaponPoint++;
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
				KdAudioManager::Instance().Play("Asset/Audio/SE/AddAndSub.wav",true);
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
			if (m_pointAddOrSubVal > 0)
			{
				if (SceneManager::Instance().GetBAddOrSubVal())
				{
					m_weaponPoint += m_pointAddOrSubVal;
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

	// マウスの当たり判定はここを参考に＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
	PWINDOWINFO pwi = new WINDOWINFO;
	pwi->cbSize = sizeof(WINDOWINFO);
	GetWindowInfo(Application::Instance().GetWindowHandle(), pwi);
	//pwi->rcWindow.left;    // マウスとの当たり判定関係のX座標に足す
	//pwi->rcWindow.top + 35;// マウスとの当たり判定関係のこれをy座標に足す

	POINT mousePos;
	GetCursorPos(&mousePos);

	mousePos.x -= 640;
	mousePos.y = mousePos.y * -1 + 360;
	Math::Vector3 Dis;
	float mouseX = (float)mousePos.x/* + (float)(pwi->rcWindow.left)*/;
	float mouseY = (float)mousePos.y + (float)(pwi->rcWindow.top + 35);

	Math::Vector3 artPos;
	artPos.x = m_artPos.x + (float)(pwi->rcWindow.left);
	artPos.y = m_artPos.y /*+ (float)(pwi->rcWindow.top)*/;
	artPos.z = m_artPos.z;
   // ＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
	Dis = artPos - Math::Vector3(mouseX, mouseY, 0.0f);
	if (Dis.Length() <= 75)
	{
		m_artScale = 1.1f;
		m_exitScale = 1.0f;
		m_gameScale = 1.0f;
		m_optionScale = 1.0f;
		m_titleScale = 1.0f;

		if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
		{
			if (!m_addFadeAlpha)
			{
				m_bArt = true;
				m_addFadeAlpha = true;
				KdAudioManager::Instance().Play("Asset/Audio/SE/各ボタンを押したときの音.wav");
			}
		}

	}
	else
	{
		m_artScale = 1.0f;
	}

	Math::Vector3 ExitPos;
	ExitPos.x = m_exitPos.x + (float)(pwi->rcWindow.left);
	ExitPos.y = m_exitPos.y /*+ (float)(pwi->rcWindow.top)*/;
	ExitPos.z = m_exitPos.z;

	Dis = ExitPos - Math::Vector3(mouseX, mouseY, 0.0f);
	if (Dis.Length() <= 75)
	{
		m_artScale = 1.0f;
		m_exitScale = 1.1f;
		m_gameScale = 1.0f;
		m_optionScale = 1.0f;
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

	Math::Vector3 GamePos;
	GamePos.x = m_gamePos.x + (float)(pwi->rcWindow.left);
	GamePos.y = m_gamePos.y /*+ (float)(pwi->rcWindow.top)*/;
	GamePos.z = m_gamePos.z;

	Dis = GamePos - Math::Vector3(mouseX,mouseY,0.0f);
	if (Dis.Length() <= 75)
	{
		m_artScale = 1.0f;
		m_exitScale = 1.0f;
		m_gameScale = 1.1f;
		m_optionScale = 1.0f;
		m_titleScale = 1.0f;

		if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
		{
			if (!m_addFadeAlpha)
			{
				m_bGame = true;
				m_addFadeAlpha = true;
				KdAudioManager::Instance().Play("Asset/Audio/SE/各ボタンを押したときの音.wav");
			}
		}

	}
	else
	{
		m_gameScale = 1.0f;
	}

	Math::Vector3 OptionPos;
	OptionPos.x = m_optionPos.x + (float)(pwi->rcWindow.left);
	OptionPos.y = m_optionPos.y /*+ (float)(pwi->rcWindow.top)*/;
	OptionPos.z = m_optionPos.z;

	Dis = OptionPos - Math::Vector3(mouseX, mouseY, 0.0f);
	if (Dis.Length() <= 75)
	{
		m_artScale = 1.0f;
		m_exitScale = 1.0f;
		m_gameScale = 1.0f;
		m_optionScale = 1.1f;
		m_titleScale = 1.0f;

		if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
		{
			/*if (!m_addFadeAlpha)
			{
				m_bOption = true;
				m_addFadeAlpha = true;
				KdAudioManager::Instance().Play("Asset/Audio/SE/各ボタンを押したときの音.wav");
			}*/
		}

	}
	else
	{
		m_optionScale = 1.0f;
	}

	Math::Vector3 TitlePos;
	TitlePos.x = m_titlePos.x + (float)(pwi->rcWindow.left);
	TitlePos.y = m_titlePos.y /*+ (float)(pwi->rcWindow.top)*/;
	TitlePos.z = m_titlePos.z;

	Dis = m_titlePos - Math::Vector3(mouseX, mouseY, 0.0f);
	if (Dis.Length() <= 75)
	{
		m_artScale = 1.0f;
		m_exitScale = 1.0f;
		m_gameScale = 1.0f;
		m_optionScale = 1.0f;
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
				SceneManager::Instance().SetNextScene
				(
					SceneManager::SceneType::game
				);
			}

			if (m_bArt)
			{
				SceneManager::Instance().SetNextScene
				(
					SceneManager::SceneType::art
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

	KdSafeDelete(pwi);
	//delete(pwi);
}

void Ui::ArtUpdate()
{
	if (m_time == 0)
	{
		m_fadeAlpha = 0.0f;
		m_time++;
	}

	POINT mousePos;
	GetCursorPos(&mousePos);

	mousePos.x -= 640;
	mousePos.y = mousePos.y * -1 + 360;
	Math::Vector3 Dis;
	float mouseX = (float)mousePos.x;
	float mouseY = (float)mousePos.y;

	Dis = m_backPos - Math::Vector3(mouseX, mouseY, 0.0f);
	if (Dis.Length() <= 40)
	{
		m_backScale = 1.0f;
		if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
		{
			KdAudioManager::Instance().Play("Asset/Audio/SE/backPush.wav");
			if (m_uiType == UiType::art)
			{
				SceneManager::Instance().SetNextScene
				(
					SceneManager::SceneType::select
				);
			}
		}

	}
	else
	{
		m_backScale = 0.8f;
	}
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
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_countThreeTex, 0, 0, 121, 136,&rc,&color);
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
				color = { 1,1,1,0.4f };
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
				color = { 1,1,1,0.4f };
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponTypeOvreDarkTex, 0, 0, 120, 50, &rc, &color, Math::Vector2(0, 0.5f));
			}
			else if (m_spPlayer->GetWeaponType() & grassHopper && m_spPlayer->GetRGrassHopperPauCnt() != 0)
			{
				transMat = Math::Matrix::CreateTranslation(440, -250, 0);
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				Math::Rectangle rc = { 0,0,m_spPlayer->GetRGrassHopperPauCnt() * 4,50 };
				color = { 1,1,1,0.4f };
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponTypeOvreDarkTex, 0, 0, m_spPlayer->GetRGrassHopperPauCnt() * 4, 50, &rc, &color, Math::Vector2(0, 0.5f));
			}

			transMat = Math::Matrix::CreateTranslation(-500, -250, 0);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponType1Tex, 0, 0, 120, 50);
			if (m_spPlayer->GetWeaponType() & lGrassHopper)
			{
				Math::Rectangle rc = { 0,0,120,50 };
				color = { 1,1,1,0.4f };
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
				color = { 1,1,1,0.4f };
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponTypeOvreDarkTex, 0, 0, 120, 50, &rc, &color, Math::Vector2(0, 0.5f));
			}
			else if (m_spPlayer->GetWeaponType() & lGrassHopper && m_spPlayer->GetLGrassHopperPauCnt() != 0)
			{
				transMat = Math::Matrix::CreateTranslation(-410, -250, 0);
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				Math::Rectangle rc = { 0,0,m_spPlayer->GetLGrassHopperPauCnt() * 4,50 };
				color = { 1,1,1,0.4f };
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_weaponTypeOvreDarkTex, 0, 0, m_spPlayer->GetLGrassHopperPauCnt() * 4, 50, &rc, &color, Math::Vector2(0, 0.5f));
			}

			transMat = Math::Matrix::CreateTranslation(-630, 300, 0);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			Math::Rectangle rc = { 0,0,320,70 };
			color = { 1, 1, 1, 1 };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_torionBarTex, 0, 0, 320, 70, &rc, &color, Math::Vector2(0, 0.5f));

			transMat = Math::Matrix::CreateTranslation(-620, 300, 0);
			KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
			rc = { 0,0,(int)(m_spPlayer->GetTorion()),50};
			color = { 1, 1, 1, 1 };
			KdShaderManager::Instance().m_spriteShader.DrawTex(&m_torionTex, 0, 0, (int)(m_spPlayer->GetTorion()), 50, &rc, &color, Math::Vector2(0, 0.5f));
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
					else if(m_bWeaponDataScoPage)
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

		transMat = Math::Matrix::Identity;
		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		color = { 0,0,0,m_fadeAlpha };
		KdShaderManager::Instance().m_spriteShader.DrawBox(0, 0, 1280, 720, &color);

		break;
	case UiType::result:
		transMat = Math::Matrix::Identity;
		KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
		color = { 0,0.2f,0.6f,1.0f };
		KdShaderManager::Instance().m_spriteShader.DrawBox(0, 0, 1280, 720, &color);

		if (m_time >= 60)
		{
			if (SceneManager::Instance().GetBPlayerWin())
			{
				transMat = Math::Matrix::CreateTranslation(0, 225, 0);
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_winTex, 0, 0, 246, 114);

			}
			else
			{
				transMat = Math::Matrix::CreateTranslation(0, 225, 0);
				KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
				KdShaderManager::Instance().m_spriteShader.DrawTex(&m_lossTex, 0, 0, 512, 172);
			}
		}

		if (m_time >= 120)
		{
			transMat = Math::Matrix::CreateTranslation(-145, 25, 0);
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

			transMat = Math::Matrix::CreateTranslation(-65, 25, 0);
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

			transMat = Math::Matrix::CreateTranslation(15, 25, 0);
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

			transMat = Math::Matrix::CreateTranslation(105, 25, 0);
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
			transMat = Math::Matrix::CreateTranslation(-145, -100, 0);
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

			transMat = Math::Matrix::CreateTranslation(-65, -100, 0);
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

			transMat = Math::Matrix::CreateTranslation(15, -100, 0);
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

			transMat = Math::Matrix::CreateTranslation(105, -100, 0);
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
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		color = { 0,0,0,1.0f };
		KdShaderManager::Instance().m_spriteShader.DrawBox(0, 0, 1280, 720, &color);

		mat = Math::Matrix::CreateScale(m_gameScale) * Math::Matrix::CreateTranslation(m_gamePos);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_gameTex, 0, 0, 700, 120);

		mat = Math::Matrix::CreateScale(m_optionScale) * Math::Matrix::CreateTranslation(m_optionPos);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_optionTex, 0, 0, 700, 120);

		mat = Math::Matrix::CreateScale(m_artScale) * Math::Matrix::CreateTranslation(m_artPos);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_artTex, 0, 0, 700, 120);

		mat = Math::Matrix::CreateScale(m_titleScale) * Math::Matrix::CreateTranslation(m_titlePos);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_titleTex, 0, 0, 700, 120);

		mat = Math::Matrix::CreateScale(m_exitScale) * Math::Matrix::CreateTranslation(m_exitPos);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_exitTex, 0, 0, 700, 120);

		mat = Math::Matrix::Identity;
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		color = { 0,0,0,m_fadeAlpha };
		KdShaderManager::Instance().m_spriteShader.DrawBox(0, 0, 1280, 720, &color);
		break;

	case UiType::art:
		mat = Math::Matrix::Identity;
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		color = { 0,0,0,1.0f };
		KdShaderManager::Instance().m_spriteShader.DrawBox(0, 0, 1280, 720, &color);

		mat = Math::Matrix::CreateScale(m_artFinleScale) * Math::Matrix::CreateTranslation(m_artFinlePos);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_artFinleTex, 0, 0, 500, 500);

		mat = Math::Matrix::CreateScale(m_artInScale) * Math::Matrix::CreateTranslation(m_artInPos);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_artInTex, 0, 0, 500, 500);

		mat = Math::Matrix::CreateScale(m_backScale) * Math::Matrix::CreateTranslation(m_backPos);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		KdShaderManager::Instance().m_spriteShader.DrawTex(&m_backTex, 0, 0, 182, 80);

		mat = Math::Matrix::Identity;
		KdShaderManager::Instance().m_spriteShader.SetMatrix(mat);
		color = { 0,0,0,m_fadeAlpha };
		KdShaderManager::Instance().m_spriteShader.DrawBox(0, 0, 1280, 720, &color);
		break;
	}

	transMat = Math::Matrix::Identity;
	KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
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

	m_artTex.Load("Asset/Textures/Ui/OPTION/ART.png");
	m_exitTex.Load("Asset/Textures/Ui/OPTION/EXIT.png");
	m_gameTex.Load("Asset/Textures/Ui/OPTION/GAME.png");
	m_optionTex.Load("Asset/Textures/Ui/OPTION/OPTION.png");
	m_titleTex.Load("Asset/Textures/Ui/OPTION/TITLE.png");

	m_countOneTex.Load("Asset/Textures/Ui/Game/one.png");
	m_countTwoTex.Load("Asset/Textures/Ui/Game/two.png");
	m_countThreeTex.Load("Asset/Textures/Ui/Game/three.png");
	m_countGoTex.Load("Asset/Textures/Ui/Game/GO.png");

	m_artFinleTex.Load("Asset/Textures/Ui/art/titleRogoFinle.png");
	m_artInTex.Load("Asset/Textures/Ui/art/titleRogoRin.png");

	m_backTex.Load("Asset/Textures/Ui/shared/Back.png");

	m_hopperDataTex.Load("Asset/Textures/Ui/Game/hopperData.png");
	m_weaponDataTex.Load("Asset/Textures/Ui/Game/weaponData.png");
	m_weaponInfoTex.Load("Asset/Textures/Ui/Game/weaponInfo.png");
	m_leftYaiTex.Load("Asset/Textures/Ui/Game/leftYaji.png");
	m_rightYaiTex.Load("Asset/Textures/Ui/Game/rightYaji.png");
	m_howToTex.Load("Asset/Textures/Ui/Game/how-to.png");
	m_infoTex.Load("Asset/Textures/Ui/OPTION/info.png");

	m_fadeAlpha = 1.0f;
	m_addFadeAlpha = false;
	m_bFirstInResult = true;
	m_bSceneCangePossible = false;
	m_pushToEnterAlpha = 1.0f;
	m_bPushToEnterAlphaAdd = false;

	m_artScale = 1.0f;
	m_exitScale = 1.0f;
	m_gameScale = 1.0f;
	m_optionScale = 1.0f;
	m_titleScale = 1.0f;

	m_gamePos = { 0,280,0 };
	m_optionPos = { 0,140,0 };
	m_artPos = { 0,0,0 };
	m_titlePos = { 0,-140,0 };
	m_exitPos = { 0,-280,0 };

	m_bArt = false;
	m_bExit = false;
	m_bFirstExit = false;
	m_bGame = false;
	m_bOption = false;
	m_bTitle = false;

	m_countOneScale = 0.1f;
	m_countOneAlpha = 0.0f;

	m_countTwoScale = 0.1f;
	m_countTwoAlpha = 0.0f;

	m_countThreeScale = 0.1f;
	m_countThreeAlpha = 0.0f;

	m_countGoScale = 0.1f;
	m_countGoAlpha = 0.0f;
	m_bFirstExit = true;

	m_artFinleScale = 1.0f;
	m_artFinlePos = {350,0,0};

	m_artInScale = 1.0f;
	m_artInPos = {-350,0,0};

	m_backPos = {550,-300,0};
	m_backScale = 0.8f;

	m_bTABKey = false;

	m_bWeaponDataPage = false;
	m_bWeaponDataHopperPage = false;
	m_bWeaponDataScoPage = true;
	m_bHowToPage = true;

	m_weaponLeftYaiScale = 1.0f;
	m_weaponLeftYaiPos = {-500,250,0};

	m_weaponRightYaiScale = 1.0f;
	m_weaponRightYaiPos = {500,250,0,};

	m_weaOrHowLeftYaiScale = 1.0f;
	m_weaOrHowLeftYaiPos = {-600,0,0};

	m_weaOrHowRightYaiScale = 1.0f;
	m_weaOrHowRightYaiPos = {600,0,0};

	m_infoScale = 1.0f;
	m_infoPos = {0,250,0};
	m_bInfo = false;
	bLButtonKey = false;
}
