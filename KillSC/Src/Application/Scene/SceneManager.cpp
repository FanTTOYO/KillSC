#include "SceneManager.h"

#include "BaseScene/BaseScene.h"
#include "GameScene/GameScene.h"
#include "TitleScene/TitleScene.h"
#include "ResultScene/ResultScene.h"
#include "SelectScene/SelectScene.h"
#include "TutorialScene/TutorialScene.h"

void SceneManager::PreUpdate()
{
	// �V�[���ؑ�
	if (m_currentSceneType != m_nextSceneType)
	{
		ChangeScene(m_nextSceneType);
	}

	// ���݂̃V�[������
	m_currentScene->PreUpdate();
}

void SceneManager::Update()
{
	m_currentScene->Update();
}

void SceneManager::PostUpdate()
{
	m_currentScene->PostUpdate();
}

void SceneManager::PreDraw()
{
	m_currentScene->PreDraw();
}

void SceneManager::Draw()
{
	m_currentScene->Draw();
}

void SceneManager::DrawSprite()
{
	m_currentScene->DrawSprite();
}

void SceneManager::DrawDebug()
{
#ifdef _DEBUG
	m_currentScene->DrawDebug();
#endif

}

const std::list<std::shared_ptr<KdGameObject>>& SceneManager::GetObjList()
{
	return m_currentScene->GetObjList();
}

void SceneManager::AddObject(const std::shared_ptr<KdGameObject>& _obj)
{
	m_currentScene->AddObject(_obj);
}

void SceneManager::ChangeScene(SceneType _sceneType)
{
	// ���̃V�[�����쐬���A���݂̃V�[���ɂ���
	switch (_sceneType)
	{
	case SceneType::title:
		ShowCursor(false); // �}�E�X�J�[�\��������); // �}�E�X�J�[�\��������)
		m_bPlayerWin = false;
		m_currentScene = std::make_shared<TitleScene>();
		break;
	case SceneType::battle:
	case SceneType::challenge:
		ShowCursor(false); // �}�E�X�J�[�\��������); // �}�E�X�J�[�\��������)
		m_bPlayerWin = false;
		m_pointAddOrSubVal = 0;
		m_bAddOrSubVal = true;
		m_bWinEnd = false;
		m_updateStopCnt = 0;
		m_enemyDrawTotal = 0;
		m_screenVibFrames = 0;
		m_currentScene = std::make_shared<GameScene>();
		break;
	case SceneType::result:
		ShowCursor(true); // �}�E�X�J�[�\��������); // �}�E�X�J�[�\��������)
		m_currentScene = std::make_shared<ResultScene>();
		break;
	case SceneType::select:
		ShowCursor(true); // �}�E�X�J�[�\��������); // �}�E�X�J�[�\��������)
		m_currentScene = std::make_shared<SelectScene>();
		break;
	case SceneType::tutorial:
		ShowCursor(false); // �}�E�X�J�[�\��������); // �}�E�X�J�[�\��������)
		m_currentScene = std::make_shared<TutorialScene>();
		break;
	}

	m_currentSceneType = _sceneType;
}
