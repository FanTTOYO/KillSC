﻿#pragma once

class BaseScene;

class SceneManager
{
public:
	// シーン情報
	enum class SceneType
	{
		title,
		battle,
		challenge,
		result,
		select,
		tutorial,
		training,
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

	const SceneType GetSceneType() { return m_currentSceneType; }
	const SceneType GetPreviousSceneType() { return m_previousSceneType; }

	const std::list<std::shared_ptr<KdGameObject>>&  GetObjList();
	void AddObject(const std::shared_ptr<KdGameObject>& _obj);

	void SetBPlayerWin() { m_bPlayerWin = true; }
	const bool GetBPlayerWin() { return m_bPlayerWin; }

	void SetPointAddOrSubVal(int a_pointAddOrSubVal) { m_pointAddOrSubVal = a_pointAddOrSubVal; }
	void AddPointAddOrSubVal(int a_pointAddOrSubVal) { m_pointAddOrSubVal += a_pointAddOrSubVal; }

	const int GetPointAddOrSubVal() { return m_pointAddOrSubVal; }

	void SetBAddOrSubVal(int a_bAddOrSubVal) { m_bAddOrSubVal = a_bAddOrSubVal; }
	const bool GetBAddOrSubVal() { return m_bAddOrSubVal; }
	void SetBWinEnd(bool a_bWinEnd = true) { m_bWinEnd = a_bWinEnd; }
	const bool GetBWinEnd() { return m_bWinEnd; }

	void SetUpdateStopCnt(int a_updateStopCnt) { m_updateStopCnt = a_updateStopCnt; }
	const int GetUpdateStopCnt() { return m_updateStopCnt; }

	void SetEnemyTotal(int a_enemyTotal) { m_enemyTotal = a_enemyTotal; }
	void SetEnemyIeftover(int a_enemyIeftover) { m_enemyIeftover = a_enemyIeftover; }
	void SetEnemyDrawTotal(int a_enemyDrawTotal) { m_enemyDrawTotal = a_enemyDrawTotal; }
	void SubEnemyIeftover() { --m_enemyIeftover; }
	void SubEnemyDrawTotal() { --m_enemyDrawTotal; }
	void AddEnemyDrawTotal() { ++m_enemyDrawTotal; }
	const int GetEnemyTotal() { return m_enemyTotal; }
	const int GetEnemyIeftover() { return m_enemyIeftover; }
	const int GetEnemyDrawTotal() { return m_enemyDrawTotal; }

	void SetScreenVibFrames(int a_screenVibFrames) { m_screenVibFrames = a_screenVibFrames; }
	const int GetScreenVibFrames() { return m_screenVibFrames; }
	void SubScreenVibFrames() { --m_screenVibFrames; }

	void SetBHumanoidEnemy(bool a_bHumanoidEnemy) { m_bHumanoidEnemy = a_bHumanoidEnemy; }
	const bool GetBHumanoidEnemy() { return m_bHumanoidEnemy; }

private:

	void ChangeScene(SceneType _sceneType);

	// 現在のシーンを管理しているポインタ
	std::shared_ptr<BaseScene> m_currentScene = nullptr;

	// 現在のシーンを管理しているデータ（変数）
	SceneType m_currentSceneType = SceneType::title;
	// 次のシーンを管理しているデータ（変数）
	SceneType m_nextSceneType = m_currentSceneType;

	// 一つ前のシーンを管理しているデータ
	SceneType m_previousSceneType;

	bool m_bPlayerWin;           // playerが勝ってる
	int  m_pointAddOrSubVal;     // 加算値か減算値
	bool m_bAddOrSubVal = true;  // true 加算値  false 減算値
	bool m_bWinEnd;				 // gameが終了してwindowが閉じるとき
	int  m_updateStopCnt;        // HitStop時に使用する
	int  m_enemyTotal;           // 敵の総数
	int  m_enemyIeftover;        // 敵の現在の数
	int  m_enemyDrawTotal;       // 敵の描画している総数
	int  m_screenVibFrames;      // 画面の振動フレーム数
	bool m_bHumanoidEnemy;       // 敵が人型かどうかを判断している 人型:true 人型ではない : faslse 両方いる : true

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
