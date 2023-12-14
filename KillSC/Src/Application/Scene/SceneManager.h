#pragma once

class BaseScene;

class SceneManager
{
public:
	// �V�[�����
	enum class SceneType
	{
		title,
		battle,
		challenge,
		result,
		select,
		tutorial,
	};

	void PreUpdate();
	void Update();
	void PostUpdate();

	void PreDraw();
	void Draw();
	void DrawSprite();
	void DrawDebug();

	// �}�l�[�W���[�̏������֐�
	void Init()
	{
		ChangeScene(SceneType::title);
	}

	//void Release();

	// ���̊֐��ł̓V�[���͕ς��Ȃ�
	// �����܂ŗ\��
	void SetNextScene(SceneType _nextScene)
	{
		m_nextSceneType = _nextScene;
	}

	const SceneType GetSceneType() { return m_currentSceneType; }

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

	// ���݂̃V�[�����Ǘ����Ă���|�C���^
	std::shared_ptr<BaseScene> m_currentScene = nullptr;

	// ���݂̃V�[�����Ǘ����Ă���f�[�^�i�ϐ��j
	SceneType m_currentSceneType = SceneType::title;
	// ���̃V�[�����Ǘ����Ă���f�[�^�i�ϐ��j
	SceneType m_nextSceneType = m_currentSceneType;

	bool m_bPlayerWin;           // player�������Ă�
	int  m_pointAddOrSubVal;     // ���Z�l�����Z�l
	bool m_bAddOrSubVal = true;  // true ���Z�l  false ���Z�l
	bool m_bWinEnd;				 // game���I������window������Ƃ�
	int  m_updateStopCnt;        // HitStop���Ɏg�p����
	int  m_enemyTotal;           // �G�̑���
	int  m_enemyIeftover;        // �G�̌��݂̐�
	int  m_enemyDrawTotal;       // �G�̕`�悵�Ă��鑍��
	int  m_screenVibFrames;      // ��ʂ̐U���t���[����
	bool m_bHumanoidEnemy;       // �G���l�^���ǂ����𔻒f���Ă��� �l�^:true �l�^�ł͂Ȃ� : faslse �������� : true

// �V���O���g���p�^�[��
	// ��������C���X�^���X�̐���1�ɐ�������
	// �֗�������
	// �Ȃ�ł�����ł��V���O���g���ɂ���ȁI

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
