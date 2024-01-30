#include "../../main.h"
#include "Ui.h"
#include "GameUi/GameUi.h"
#include "TitleUi/TitleUi.h"
#include "ModeSelectUi/ModeSelectUi.h"
#include "ResultUi/ResultUi.h"
#include "TrainingUi/TrainingUi.h"
#include "TutorialUi/TutorialUi.h"
#include "../Character/Enemy/Enemy.h"

void Ui::SetUiType(UiType a_uiType)
{
	m_uiType = a_uiType;
	switch (m_uiType)
	{
	case UiType::title:
		m_currentUiClassId = std::make_shared<TitleUi>();
		break;
	case UiType::training:
		m_currentUiClassId = std::make_shared<TrainingUi>();
		break;
	case UiType::game:
		m_currentUiClassId = std::make_shared<GameUi>();
		break;
	case UiType::tutorial:
		m_currentUiClassId = std::make_shared<TutorialUi>();
		break;
	case UiType::result:
		m_currentUiClassId = std::make_shared<ResultUi>();
		break;
	case UiType::select:
		m_currentUiClassId = std::make_shared<ModeSelectUi>();
		break;
	}
}

void Ui::Update()
{
	m_currentUiClassId->Update();
}

void Ui::PostUpdate()
{
	m_currentUiClassId->PostUpdate();
}

void Ui::DrawSprite()
{
	Math::Matrix transMat = Math::Matrix::Identity;

	m_currentUiClassId->DrawSprite();

	transMat = Math::Matrix::Identity;
	KdShaderManager::Instance().m_spriteShader.SetMatrix(transMat);
}

void Ui::AddEnemy(std::shared_ptr<Enemy> a_spEnemy)
{
	m_currentUiClassId->AddEnemy(a_spEnemy);
}

void Ui::AddTutorialCnt()
{
	m_currentUiClassId->AddTutorialCnt();
}

void Ui::SetEnemy(std::shared_ptr<Enemy> a_spEnemy)
{
	m_currentUiClassId->SetEnemy(a_spEnemy);
}

void Ui::Init(std::weak_ptr<json11::Json> a_wpJsonObj)
{
	m_currentUiClassId->Init(a_wpJsonObj);
}

bool Ui::ButtomProcessing(Math::Vector2 a_pos, const KdTexture& a_tex, float& a_scale, float a_originalScale, float a_addScaleVal)
{
	PWINDOWINFO pwi = new WINDOWINFO;
	pwi->cbSize = sizeof(WINDOWINFO);
	GetWindowInfo(Application::Instance().GetWindowHandle(), pwi);

	POINT mousePos;
	GetCursorPos(&mousePos);

	mousePos.x -= 640;
	mousePos.y = mousePos.y * -1 + 360;
	Math::Vector3 Dis;
	float mouseX = (float)mousePos.x;
	float mouseY = (float)mousePos.y + (float)(pwi->rcWindow.top + m_mpUiSharedObj["TitleBarHeight"].int_value());

	float MouseLeft = mouseX - static_cast<float>(m_mpUiSharedObj["MouseRadius"].number_value());
	float MouseRight = mouseX + static_cast<float>(m_mpUiSharedObj["MouseRadius"].number_value());
	float MouseTop = mouseY + static_cast<float>(m_mpUiSharedObj["MouseHalfHeight"].number_value());
	float MouseBottom = mouseY - static_cast<float>(m_mpUiSharedObj["MouseHalfHeight"].number_value());

	Math::Vector3 ButtomPos;
	ButtomPos.x = a_pos.x + (float)(pwi->rcWindow.left);
	ButtomPos.y = a_pos.y;

	float TextureWidth  = static_cast<float>(a_tex.GetWidth())  / 2.0f;
	float TextureHeight = static_cast<float>(a_tex.GetHeight()) / 2.0f;

	float ButtomLeft = ButtomPos.x - TextureWidth;
	float ButtomRight = ButtomPos.x + TextureWidth;
	float ButtomTop = ButtomPos.y + TextureHeight;
	float ButtomBottom = ButtomPos.y - TextureHeight;

	if (MouseRight > ButtomLeft && ButtomRight > MouseLeft &&
		MouseTop > ButtomBottom && ButtomTop > MouseBottom)
	{
		a_scale = a_originalScale + a_addScaleVal;

		if (KdInputManager::Instance().IsPress("select"))
		{
			KdSafeDelete(pwi);
			return true;
		}
	}
	else
	{
		a_scale = a_originalScale;
	}

	return false;

	KdSafeDelete(pwi);
}