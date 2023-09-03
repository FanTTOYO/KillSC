#pragma once

class BaseScene;

class SceneManager
{
public:
	// シーン情報
	enum class SceneType
	{
		title,
		game,
		result,
		select,
		art,
	};

	void PreUpdate();
	void Update();
	void PostUpdate();

	void PreDraw();
	void Draw();
	void DrawSprite();
	void DrawDebug();

	// マネージャーの初期化関数
	void Init()
	{
		ChangeScene(SceneType::title);
	}

	//void Release();

	// この関数ではシーンは変わらない
	// あくまで予約
	void SetNextScene(SceneType _nextScene)
	{
		m_nextSceneType = _nextScene;
	}

	const std::list<std::shared_ptr<KdGameObject>>&  GetObjList();
	void AddObject(const std::shared_ptr<KdGameObject>& _obj);

	void SetBPlayerWin() { m_bPlayerWin = true; }
	const bool GetBPlayerWin() { return m_bPlayerWin; }

	void SetPointAddOrSubVal(int a_pointAddOrSubVal) { m_pointAddOrSubVal = a_pointAddOrSubVal; }
	const int GetPointAddOrSubVal() { return m_pointAddOrSubVal; }

	void SetBAddOrSubVal(int a_bAddOrSubVal) { m_bAddOrSubVal = a_bAddOrSubVal; }
	const bool GetBAddOrSubVal() { return m_bAddOrSubVal; }
	void SetBWinEnd(bool a_bWinEnd = true) { m_bWinEnd = a_bWinEnd; }
	const bool GetBWinEnd() { return m_bWinEnd; }

	void SetUpdateStopCnt(int a_updateStopCnt) { m_updateStopCnt = a_updateStopCnt; }
	const int GetUpdateStopCnt() { return m_updateStopCnt; }

private:

	void ChangeScene(SceneType _sceneType);

	// 現在のシーンを管理しているポインタ
	std::shared_ptr<BaseScene> m_currentScene = nullptr;

	// 現在のシーンを管理しているデータ（変数）
	SceneType m_currentSceneType = SceneType::title;
	// 次のシーンを管理しているデータ（変数）
	SceneType m_nextSceneType = m_currentSceneType;

	bool m_bPlayerWin; // playerが勝ってる
	int m_pointAddOrSubVal; // 加算値か減算値
	bool m_bAddOrSubVal = true; // true 加算値  false 減算値
	bool m_bWinEnd;   // gameが終了してwindowが閉じるとき
	int m_updateStopCnt;

// シングルトンパターン
	// 生成するインスタンスの数を1つに制限する
	// 便利すぎる
	// なんでもかんでもシングルトンにすんな！

private:
	SceneManager() { Init(); }
	~SceneManager(){}

public:

	static SceneManager& Instance()
	{
		static SceneManager instance;
		return instance;
	}
};
