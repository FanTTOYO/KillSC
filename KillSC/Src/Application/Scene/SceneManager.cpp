#include "SceneManager.h"

#include "BaseScene/BaseScene.h"
#include "GameScene/GameScene.h"
#include "TitleScene/TitleScene.h"
#include "ResultScene/ResultScene.h"
#include "SelectScene/SelectScene.h"
#include "ArtScene/ArtScene.h"

void SceneManager::PreUpdate()
{
	// シーン切替
	if (m_currentSceneType != m_nextSceneType)
	{
		ChangeScene(m_nextSceneType);
	}

	// 現在のシーン処理
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
	// 次のシーンを作成し、現在のシーンにする
	switch (_sceneType)
	{
	case SceneType::title:
		m_bPlayerWin = false;
		m_currentScene = std::make_shared<TitleScene>();
		break;
	case SceneType::game:
		m_currentScene = std::make_shared<GameScene>();
		break;
	case SceneType::result:
		m_currentScene = std::make_shared<ResultScene>();
		break;
	case SceneType::select:
		m_currentScene = std::make_shared<SelectScene>();
		break;
	case SceneType::art:
		m_currentScene = std::make_shared<ArtScene>();
		break;
	}

	m_currentSceneType = _sceneType;
}
