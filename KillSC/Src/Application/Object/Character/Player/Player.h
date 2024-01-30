#pragma once
#define GRASS_HOPPER_PAUCNT 30  // グラスホッパーが使えない時間

class CameraBase;
class WeaponBase;
class Enemy;
class Ui;

class Player : public KdGameObject
{
public:
	enum WeaponType
	{
		scorpion = 1 << 0,				// 右短剣装備
		grassHopper = 1 << 1,				// 右ホッパー装備

		lGrassHopper = 1 << 5,				// 左短剣装備
		lScorpion = 1 << 4,				// 左ホッパー装備
	};

	enum PlayerState
	{
		idle = 1 << 0,																							// 通常状態
		jump = 1 << 1,																							// ジャンプ
		fall = 1 << 2,																							// 降下
		run  = 1 << 3,																							// 走り
		grassHopperDashF = 1 << 4,																				// ホッパーでの高速移動（前）
		grassHopperDashB = 1 << 5,																				// ホッパーでの高速移動（後ろ）
		grassHopperDashR = 1 << 6,																				// ホッパーでの高速移動（右）
		grassHopperDashL = 1 << 7,																				// ホッパーでの高速移動（左）
		grassHopperDash = grassHopperDashF | grassHopperDashB | grassHopperDashR | grassHopperDashL,			// ホッパーでの高速移動
		grassHopperDashUp = 1 << 8,																				// ホッパーでの高速移動（上）
		lAttackOne = 1 << 9,																					// 左攻撃1段目
		lAttackTwo = 1 << 10,																					// 左攻撃2段目
		lAttackThree = 1 << 11,																					// 左攻撃3段目
		lAttack = lAttackOne | lAttackTwo | lAttackThree,														// 左攻撃
		rAttackOne = 1 << 12,																					// 右攻撃1段目
		rAttackTwo = 1 << 13,																					// 右攻撃2段目
		rAttackThree = 1 << 14,																					// 右攻撃3段目
		rAttack = rAttackOne | rAttackTwo | rAttackThree,														// 右攻撃
		defense = 1 << 15,																						// 防御
		mantis = 1 << 16,																						// 防御不可攻撃
		hasDefense = 1 << 17,																					// 攻撃を防がれた
		blowingAwayHit = 1 << 18,																				// 吹き飛ばし攻撃を受けた
		iaiKiriHit = 1 << 19,																					// 居合切りをされた
		nomalHit = 1 << 20,																						// 通常の攻撃を受けた
		cutRaiseHit = 1 << 21,																					// 切り上げ攻撃をされた
		hit = nomalHit | iaiKiriHit | blowingAwayHit | cutRaiseHit,												// 攻撃を受けた
		stepF = 1 << 22,																						// ステップ（前）
		stepR = 1 << 23,																						// ステップ（右）
		stepL = 1 << 24,																						// ステップ（左）
		stepB = 1 << 25,																						// ステップ（後ろ）
		step  = stepF | stepB | stepR | stepL,																	// ステップ
		blowingAwayRise  = 1 << 26,																				// 吹き飛ばされたあとの起き上がり
		iaiKiriRise      = 1 << 27,																				// 居合切りされたあとの起き上がり
		rise             = iaiKiriRise | blowingAwayRise,														// 起き上がり
		rlAttackOne      = 1 << 28,																				// 両手攻撃1段目
		rlAttackTwo      = 1 << 29,																				// 両手攻撃2段目
		rlAttackThree    = 1 << 30,																				// 両手攻撃3段目
		rlAttack         = rlAttackOne | rlAttackTwo | rlAttackThree,											// 両手攻撃
		rlAttackRush     = 1 << 31,																				// ラッシュ
	};

	Player() {}
	~Player() {}																								
	
	void PreUpdate()							override;
	void Update()								override;														
	void PostUpdate()							override;														
	void DrawLit()								override;														
	void DrawUnLit()							override;														
	void DrawBright()							override;																										
	void GenerateDepthMapFromLight()			override;														
	void GenerateDepthMapFromLight_SkinMesh()	override;														
	void DrawLit_SkinMesh()						override;
	void DrawDebug()							override;
	void Init(std::weak_ptr<json11::Json> a_wpJsonObj);
	void Release() {};
																												
	void SetCamera(std::shared_ptr<CameraBase> a_spCcamera) { m_wpCamera = a_spCcamera; }						// カメラクラスのをもらう
	void AddEnemy(std::shared_ptr<Enemy> a_spEnemy) { m_enemyList.push_back(a_spEnemy); }						// 敵を増やす
	void AddWeaponToEnemy(std::shared_ptr<Enemy> a_spEnemy);													// 武器に未登録の敵の情報をいれる
	void SetUi(std::shared_ptr<Ui> a_spUi) { m_wpUi = a_spUi; }													// UIクラスをもらう
																												
	const UINT& GetPlayerState() { return m_playerState; }														// プレイヤーの状態を渡す
	const UINT& GetWeaponType() { return m_weaponType; }														// 今装備している装備の情報を渡す
																												
	const int& GetLGrassHopperTime() { return m_lGrassHopperTime; }												// 左のホッパーの使用時間を渡す
	const int& GetRGrassHopperTime() { return m_rGrassHopperTime; }												// 右のホッパーの使用時間を渡す
																												
	const int& GetLGrassHopperPauCnt() { return m_lGrassHopperPauCnt; }											// 左のホッパーの使用不可時間を渡す
	const int& GetRGrassHopperPauCnt() { return m_rGrassHopperPauCnt; }											// 右のホッパーの使用不可時間を渡す
																												 
																												 
	const float& GetGravity() { return m_gravity; }																// かかってる重力を渡す
	const float& GetDashSpd() { return m_dashSpd;}																// 高速移動の速さを渡す
	const float& GetVForce()  { return m_vForce; }																// 現在のエネルギー量を渡す
	const float& GetEndurance()  { return m_endurance; }														// 戦闘体の耐久力を渡す
	const std::shared_ptr<KdModelWork>& GetModel() { return m_spModel; }										// モデルの情報を渡す
	const int GetInvincibilityTimeCnt() { return m_invincibilityTimeCnt; }										// 現在の無敵時間を渡す
	const bool GetBRushAttackPossible() { return m_bRushAttackPossible; }										// ラッシュアタックが可能かどうかを渡す
	void SetBRushAttackPossible(bool a_bRushAttackPossible) { m_bRushAttackPossible = a_bRushAttackPossible; }	// ラッシュアタックが可能かどうかが渡される
																												
	const int GetAnimationCnt() { return m_attackAnimeCnt; }													// 攻撃のアニメーションフレーム数渡す
	std::vector<std::shared_ptr<WeaponBase>> GetWeaponList() { return m_weaponList; }							// 武器のリストを渡す
																												
	void OnHit(Math::Vector3 a_KnocBackvec);																	// 通常攻撃が当たった
	void BlowingAwayAttackOnHit(Math::Vector3 a_KnocBackvec);													// 吹き飛ばし攻撃が当たった
	void IaiKiriAttackOnHit(Math::Vector3 a_KnocBackvec);														// 居合切り系統の攻撃が当たった
	void CutRaiseOnHit(Math::Vector3 a_KnocBackvec);															// 切り上げ攻撃が当たった
	void HasDefense();																							// 攻撃が防御された
																												
	const bool GetBPlayerDeath() { return m_bPlayerDeath; }														// プレイヤーがDeathしたかどうかを渡す
	const bool GetBPlayerLose() { return m_bPlayerLose; }														// プレイヤーが負けたかどうかを渡す
																												
	const float GetAngleY() { return m_mWorldRot.y;}															// プレイヤーの向いてる角度を渡す
																												 
	const bool GetBRushRp() {return m_bRushRp;}																	// 回転ラッシュかどうかを渡す

	const Math::Vector3& GetAddCenterVal() { return m_addCenterVal; };											// 足すとcharacterの真ん中になる値 
																												 
private:

	void WeaponChangeProcess();																						// 武器切替の処理
	void AnimationUpdate();																							// アニメーションの更新
	void PlayerKickHitAttackChaeck();																				// プレイヤーのキック当たり判定
	void PlayerPanchiHitAttackChaeck();																				// プレイヤーのパンチ当たり判定
	void UpdateRotate(Math::Vector3& a_srcMoveVec);																	// プレイヤーの回転を更新
	void GrassMoveVecDecision();																					// ホッパーで進む方向を決める
	void ScorpionActionDecision();																					// 短剣で何をするかを決める
	void GrassMove();																								// 高速移動時の動きを扱う
	void StepMove();																								// ステップ時の動きを扱う																						
	void NormalMove();																								// 歩きなどの動きを扱う
	void ScorpionAttackMove();																						// 短剣での攻撃を扱う
	void ScorpionAttackEndProcess();																				// 短剣での攻撃終了処理
	void ScorpionAttackAnimationMoveProcess();																		// 短剣での攻撃アニメーション中の処理
	void ScorpionDefenseMove();																						// 短剣での防御を扱う
	void HasDefenseMove();																							// 防御された時の動きを扱う
	void TutorialUpdate();																							// チュートリアル時のみ行う処理
	void OverStageChaeck();																							// ステージから出たかどうかを確認
	void EnemyRockOn();																								// 敵をロックオンする処理
	void HitStateUpdate();																							// Hit状態の時の処理
	void SpeedyMoveWallHitChack(float& a_moveSpd, Math::Vector3 moveVec);											// 速い動きの時の壁との当たり判定
	void RayCollisionUpdate();																						// レイ判定処理
	void SphereCollisionUpdate();																					// 球形判定処理

	std::map<std::string,json11::Json>  m_mpObj;																	// JsonからPlayerオブジェクトの情報をもらうときに使用
	std::weak_ptr<json11::Json> m_wpJsonObj;																		// Playerオブジェクト以外の情報をもらうときに使用
																													
	std::shared_ptr<KdModelWork> m_spModel;																			// モデル
																													
	int m_maxWeaponType = 2;																						// 武器の最大のナンバー
	const int FIRST_WEAPON_TYPE_NUMBER = 1;																			// 武器の最初のナンバー

	Math::Vector3 m_addCenterVal;																					// 足したら中心になる値
	Math::Vector3 m_addGrassDashEffectPosVal;																		// プレイヤーの座標に足すとエフェクトを出す位置になる

	float m_gravity = 0;																							// 重力
	int m_toleranceComboTime = 0;																					// コンボ許容時間
	int m_m_toleranceAttackTime = 0;																				// 再攻撃間隔
	UINT m_playerState;																								// プレイヤーの状態
	UINT m_weaponType;																								// 今現在装備している武器

	int m_rightWeaponNumber = 0;                                                                                    // 今現在右に装備している武器ナンバー
	int m_leftWeaponNumber = 0;																						// 今現在左に装備している武器ナンバー

	bool m_bMove = false;																							// 動いてる true

	bool m_bMButtonState;																							// ホイールをクリックしてる true

	Math::Vector3    m_pos = {};																					// 座標
	Math::Vector3    m_move = {};																					// 動く方向
	Math::Vector3    m_scale = {};																					// デカさ

	std::weak_ptr<CameraBase>        m_wpCamera;																	// カメラのアドレスを保持
	Math::Vector3					 m_mWorldRot;																	// 回転情報を保持

	int m_lGrassHopperPauCnt = 0;																					// 休止する時間
	int m_rGrassHopperPauCnt = 0;																					// 休止する時間

	int m_lGrassHopperTime = 0;																						// 左ホッパーでの高速移動時間
	int m_rGrassHopperTime = 0;																						// 右ホッパーでの高速移動時間
	Math::Matrix m_grasRotYMat;																						// 高速移動する方向

	Math::Vector3 m_grassHopperDashDir = {};                                                                        // 高速移動する方向ベクトル
	float m_dashSpd = 0;                                                                                            // 高速移動の速さ

	std::shared_ptr<KdAnimator> m_spAnimator;																		// アニメーションさせる為の変数

	std::vector<std::shared_ptr<WeaponBase>> m_weaponList;															// 武器リスト

	bool m_bFirstUpdate = false;																					// 初めての更新
	bool m_bEwaponDrawPosible = false;                                                                              // 武器を描画できる　true


	std::weak_ptr<Enemy> m_wpEnemy;																					// ロックオンしてる敵
	std::list<std::weak_ptr<Enemy>> m_enemyList;																	// 敵のリスト

	int m_attackAnimeCnt = 0;																						// 攻撃アニメーションのフレーム数
	int m_runAnimeCnt = 0;																							// 走りアニメーションのフレーム数
	int m_attackAnimeDelayCnt = 0;																					// 攻撃アニメーションを遅らせる時間
	bool m_bAttackAnimeDelay = false;																				// 攻撃アニメーションを遅れてる true
	bool m_bAttackAnimeCnt = false;																					// 攻撃アニメーションのフレーム数を加算してもいい　true

	int   m_hitStopCnt = 0;																							// 攻撃を受けてとなる時間
	int   m_hitColorChangeTimeCnt = 0;																				// 攻撃を受け赤くなる時間
	float m_hitMoveSpd = 0.0f;																						// ノックバックする速さ
	float m_gardMoveSpd;																							// 攻撃を防御した時のノックバック速度
	bool  m_bTough;																									// 高い攻撃力の攻撃をくらうORガードしたかどうか
	Math::Vector3 m_knockBackVec;																					// ノックバックする方向
	float m_vForce = 0.0f;																							// Vフォース残量
	float m_endurance = 0.0f;																						// Vフォース体耐久力
	float m_vForceDownValue = 0.0f;																					// Vフォース消費量
	float m_graduallyTorionDecVal;																					// 徐々に減ってくVフォース量
	int m_delayTurnAroundTime;																						// 振り返りを遅らせる時間

	int m_stepCnt;																									// ステップをする時間															
	Math::Vector3 m_stepDashDir;																					// ステップする方向ベクトル

	Math::Vector3 m_attackMoveDir;																					// 攻撃した時に移動する方向
	float         m_attackMoveSpd;																					// 攻撃した時に移動する方向

	int m_invincibilityTimeCnt;																						// 無敵時間

	bool m_bRushAttackPossible = false;																				// ラッシュアタックできる true													
	std::weak_ptr<Ui> m_wpUi;																						// UIクラスの情報を持ってる
	int m_tuGardTime;																								// チュートリアルの際の防御してる時間を数えてる

	int m_overStageTime;																							// ステージ外に出た時間

	bool m_bPlayerDeath;																							// プレイヤーがやられた true
	bool m_bPlayerLose;																								// プレイヤーが負けた true

	KdSquarePolygon m_rocKOnPolygon;																				// ロックオン用ポリゴン

	Math::Matrix m_rockOnPolyMat;																					// ロックオン用行列

	bool m_bRushRp;																									// 回転ラッシュ攻撃 true	
	bool m_bBlowingAwayHitB;																						// 後ろから吹き飛ばし攻撃を受けた true1

	bool m_bAtttackMoveSpeedDec;																					// 減速させる : true
};