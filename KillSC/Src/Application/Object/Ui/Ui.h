#pragma once
class Player;
class Enemy;
class GameCamera;

class Ui : public KdGameObject
{
public:
	Ui() {}
	virtual ~Ui(){}

	enum class UiType
	{
		title,
		game,
		result,
		select,
		tutorial,
		training,
	};

	enum TutorialType
	{
		kihonTu,
		bukiTu,
		hopperTu,
		sonotaTu,
		sukoADMoveTu,
	};

	void Update()override;
	void PostUpdate()override;
	virtual void Init(std::weak_ptr<json11::Json> a_wpJsonObj);
	void DrawSprite()override;
	bool ButtomProcessing(Math::Vector2 a_pos , const KdTexture& a_tex , float& a_scale , float a_originalScale = 1.0f, float a_addScaleVal = 0.2f);

	virtual void SetUiType(UiType a_uiType);
	virtual void SetAddFadeAlpha() { m_currentUiClassId->SetAddFadeAlpha(); }
	virtual int GetTime() {return m_currentUiClassId->GetTime(); }
	virtual const bool GetBOption() { return m_currentUiClassId->GetBOption(); }
	virtual void AddEnemy(std::shared_ptr<Enemy> a_spEnemy);
	virtual void SetEnemy(std::shared_ptr<Enemy> a_spEnemy);
	virtual void SetPlayer(std::shared_ptr<Player> a_spPlayer) { m_currentUiClassId->SetPlayer(a_spPlayer); }
	virtual void SetCamera(std::shared_ptr<GameCamera> a_spCamera) { m_currentUiClassId->SetCamera(a_spCamera); }
	virtual const bool GetBSceneCangePossible() { return m_currentUiClassId->GetBSceneCangePossible(); }
	virtual const bool GetBTutorial() { return m_currentUiClassId->GetBTutorial(); }
	virtual const TutorialType GetTutorialType() { return m_currentUiClassId->GetTutorialType(); }
	virtual void AddTutorialCnt();

	virtual const int GetGameTimeS1() { return m_currentUiClassId->GetGameTimeS1(); }
	virtual const int GetGameTimeS10() { return m_currentUiClassId->GetGameTimeS10(); }
	virtual const int GetGameTimeM1() { return m_currentUiClassId->GetGameTimeM1(); }
	virtual const int GetGameTimeM10() { return m_currentUiClassId->GetGameTimeM10(); }
	virtual void SetWaveCnt(int a_waveCnt) { m_currentUiClassId->SetWaveCnt(a_waveCnt); }
	virtual void SetBWaveChange() { m_currentUiClassId->SetBWaveChange(); }

protected:
	std::map<std::string,json11::Json>  m_mpUiSharedObj;																	// JsonからUi共通オブジェクトの情報をもらうときに使用
	std::map<std::string,json11::Json>  m_mpDedicatedObj;																	// Jsonからそれぞれ専用のオブジェクトの情報をもらうときに使用
	float m_screenAlphaFadeSpeed;																							// スクリーンがフェードする速さ
	float m_pushLClickAlphaFadeSpeed;																						// PushLClickテクスチャのフェード速度
	int   m_mouseRadius;																									// マウスの半径
	int   m_mouseHalfHeight;																								// マウスの半分の高さ
	int   m_titleBarHeight;																									// エグゼのタイトルバーのデカさ
	Math::Vector3 m_titlerogoTexPos;																						// ゲームロゴの座標
	Math::Vector3 m_pushLClickTexPos;																						// PushLClickの座標

private:

	UiType m_uiType = UiType::title;
	std::shared_ptr<Ui> m_currentUiClassId;
};

class OptionUi : public Ui
{
public:

private:
};