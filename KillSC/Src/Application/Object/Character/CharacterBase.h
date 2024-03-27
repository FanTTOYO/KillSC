#pragma once

class WeaponBase;
class Player;
class Enemy;

class CharacterBase : public KdGameObject
{
public:
	enum WeaponType
	{
		weaponNone = 1 << 0,
		scorpion = 1 << 1,
		grassHopper = 1 << 2,

		lWeaponNone = 1 << 4,
		lScorpion = 1 << 5,
		lGrassHopper = 1 << 6,
	};

	enum CharaState
	{
		idle = 1 << 0,																							// 通常状態
		jump = 1 << 1,																							// ジャンプ
		fall = 1 << 2,																							// 降下
		run = 1 << 3,																							// 走り
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
		step = stepF | stepR | stepL | stepB,																	// ステップ
		blowingAwayRise = 1 << 26,																				// 吹き飛ばされたあとの起き上がり
		iaiKiriRise = 1 << 27,																					// 居合切りされたあとの起き上がり
		rise = iaiKiriRise | blowingAwayRise,																	// 起き上がり
		rlAttackOne = 1 << 28,																					// 両手攻撃1段目
		rlAttackTwo = 1 << 29,																					// 両手攻撃2段目
		rlAttackThree = 1 << 30,																				// 両手攻撃3段目
		rlAttack = rlAttackOne | rlAttackTwo | rlAttackThree,													// 両手攻撃
		rlAttackRush = 1 << 31,																					// ラッシュ
	};

	// 描画関連
	void DrawLit()								override;
	void GenerateDepthMapFromLight_SkinMesh()	override;
	void GenerateDepthMapFromLight()			override;
	void DrawBright()							override;
	void DrawDebug()							override;

	// 攻撃Hit時に呼び出す
	virtual void OnHit(Math::Vector3 a_KnocBackvec,CharacterBase* a_pAttackedCharacter) = 0;																	// 通常攻撃が当たった
	virtual void BlowingAwayAttackOnHit(Math::Vector3 a_KnocBackvec, CharacterBase* a_pAttackedCharacter) = 0;													// 吹き飛ばし攻撃が当たった
	virtual void IaiKiriAttackOnHit(Math::Vector3 a_KnocBackvec) = 0;														// 居合切り系統の攻撃が当たった
	virtual void CutRaiseOnHit(Math::Vector3 a_KnocBackvec) = 0;															// 切り上げ攻撃が当たった
	virtual void HasDefense() {};																							// 攻撃が防御された					


	// セッター
	void SetPos(Math::Vector3 a_pos) { m_pos = a_pos; }
	void SetAttackHit(bool a_attackHit) { m_attackHit = a_attackHit; }
	void SetDefenseSuc(bool a_defenseSuc) { m_bDefenseSuc = a_defenseSuc; }
	void SetBRushAttackPossible(bool a_bRushAttackPossible) { m_bRushAttackPossible = a_bRushAttackPossible; }
	void SetWorldRotationY(float a_mWorldRotY) { m_mWorldRot.y = a_mWorldRotY; }

	// ゲッター
	// 基本情報を返す
	const UINT& GetCharaState() { return m_charaState; }
	const float GetVForce() { return m_vForce; }
	const float GetEndurance() { return m_endurance; }
	const float GetDamageAmount() { return m_damageAmount; }
	const float GetGravity() { return m_gravity; }
	const std::shared_ptr<KdModelWork>& GetModel() { return m_spModel; }
	const Math::Vector3& GetAddCenterVal() { return m_addCenterVal; }

	// 角度を返す
	const float GetAngleY() { return m_mWorldRot.y; }

	// GrassDash関係の情報を返す
	const int GetLGrassHopperTime() { return m_lGrassHopperTime; }
	const int GetRGrassHopperTime() { return m_rGrassHopperTime; }
	const int GetLGrassHopperPauCnt() { return m_lGrassHopperPauCnt; }
	const int GetRGrassHopperPauCnt() { return m_rGrassHopperPauCnt; }
	const float GetDashSpd() { return m_dashSpd; }

	// 死亡・負けたかどうかを返す
	const bool GetBDeath() { return m_bDeath; }
	const bool GetBLose() { return m_bLose; }

	// 攻撃・ダメージ関係を返す
	const int  GetAnimationCnt() { return m_attackAnimeCnt; }
	const bool GetBRushAttackPossible() { return m_bRushAttackPossible; }
	const bool GetAttackHit() { return m_attackHit; }
	const int  GetInvincibilityTimeCnt() { return m_invincibilityTimeCnt; }
	const bool GetDefenseSuc() { return m_bDefenseSuc; }

	const UINT& GetNotHumanoidEnemyState() { return m_notHumanoidEnemyState; }

	// 武器関係
	std::vector<std::shared_ptr<WeaponBase>> GetWeaponList() { return m_weaponList; }
	const UINT& GetWeaponType() { return m_weaponType; }

protected:

	struct HitInfo
	{
		bool hit = false;
		float maxOverLap = 0;
		Math::Vector3 hitDir = {}; // 当たった方向
		Math::Vector3 groundPos = {};
		void Init() 
		{
			hit = false;
			maxOverLap = 0;
			hitDir = {}; // 当たった方向
			groundPos = {};
		}
	};

	void Init();

	struct stStatusTable
	{
		int AddCenterVal;
		int WimpEnemyTypeOne;

		// etc...
	};

	void KickHitAttackChaeck(std::shared_ptr<CharacterBase> a_target, CharacterBase* a_pAttakedCharacter);											// キックの当たり判定
	void OverStageChaeck();																						// ステージから出たかどうかを確認
	void SetGrassDashInfo(Math::Vector3 a_dashDir, CharaState a_charaState, std::shared_ptr<KdAnimationData> a_animationInfo, float a_hopperBaseAng,
		             int a_weaponNumber, Math::Vector3 a_vec, float a_grassDashAng, float a_grassDashAddAng = 0.0f, std::string a_legPoint = "GrassHopperLegRPoint", std::string a_effectName = "GrassDashBlurPlayer.efk"); // グラスダッシュに必要な情報をセットする
	bool SetStepDashInfo(Math::Vector3 a_dashDir, CharaState a_charaState, std::shared_ptr<KdAnimationData> a_animationInfo); // ステップに必要な情報をセットする
	void SetHitMoveInfo(CharaState a_charaState, Math::Vector3 a_knockBackVec,float a_hitMoveSpd,float a_damgage,int a_hitStopCnt,int a_UpdateStopCnt,std::shared_ptr<KdAnimationData> a_animationInfo,float a_firstGraduallyTorionDecVal,
		                float a_graduallyTorionDecVal,int a_invincibilityTimeCnt = 0);
	// 球当たり判定
	HitInfo SphereColisionCheck(Math::Vector3 a_center, float a_radius, KdCollider::Type a_type, std::shared_ptr<WeaponBase> a_weapon);
	HitInfo SphereColisionCheck(Math::Vector3 a_center, float a_radius, KdCollider::Type a_type, std::shared_ptr<Enemy> a_enemy);
	HitInfo SphereColisionCheck(Math::Vector3 a_center, float a_radius, KdCollider::Type a_type, std::shared_ptr<Player> a_player);
	HitInfo SphereColisionCheck(Math::Vector3 a_center, float a_radius, KdCollider::Type a_type, std::shared_ptr<KdGameObject> a_obj);
	HitInfo SphereColisionCheck(Math::Vector3 a_center, float a_radius, KdCollider::Type a_type);

	// レイ当たり判定
	HitInfo RayColisionCheck(Math::Vector3 a_pos, Math::Vector3 a_dir, float a_range,KdCollider::Type a_type, std::shared_ptr<WeaponBase> a_weapon);
	HitInfo RayColisionCheck(Math::Vector3 a_pos, Math::Vector3 a_dir, float a_range,KdCollider::Type a_type, std::shared_ptr<Enemy> a_enemy);
	HitInfo RayColisionCheck(Math::Vector3 a_pos, Math::Vector3 a_dir, float a_range,KdCollider::Type a_type, std::shared_ptr<Player> a_player);
	HitInfo RayColisionCheck(Math::Vector3 a_pos, Math::Vector3 a_dir, float a_range,KdCollider::Type a_type, std::shared_ptr<KdGameObject> a_obj);
	HitInfo RayColisionCheck(Math::Vector3 a_pos, Math::Vector3 a_dir, float a_range,KdCollider::Type a_type);

	void EffectTrackingUpdate(std::string a_efectName, float a_scale, float a_rotY, float a_rotX, Math::Vector3 a_pos, Math::Vector3 a_addGrassDashEffectPosVal, Math::Vector3 vec = Math::Vector3::Zero);
	void SetAttackInfo(CharaState a_nextState, CharaState a_logicalAndState, Math::Vector3 a_attackMoveDir, std::shared_ptr<KdAnimationData> a_animationInfo, float a_attackMoveSpd);
	bool PanchiAttackHitChaeck(std::shared_ptr<CharacterBase> a_target,std::string a_hitPointName,float a_addBottomYVal,float a_radius, KdCollider::Type a_type, CharacterBase* a_pAttackedCharacter);
	void GrassDashAfterAnimationChange();
	void AttackInfoReset(CharaState a_logicalAndState);
	void WeaponMantisReset();
	bool HitOverRapComparison(float& a_compareFromLen, float a_compareWithLen, Math::Vector3 &a_compareFromDir, Math::Vector3 a_compareWithDir);

	// JSON
	std::map<std::string, json11::Json>  m_mpObj;																// Jsonからキャラごとの情報をもらうときに使用
	std::weak_ptr<json11::Json> m_wpJsonObj;																	// 共通情報をもらうときに使用

	// 基本情報
	UINT             m_charaState;
	Math::Vector3    m_pos = {};																				// 座標
	std::shared_ptr<KdModelWork> m_spModel = nullptr;															// モデル
	std::shared_ptr<KdAnimator> m_spAnimator;																	// アニメーションさせる為の変数
	float m_vForce = 0.0f;																						// Vフォース残量
	float m_endurance = 0.0f;																					// Vフォース体耐久力
	float m_damageAmount = 0.0f;																				// ダメージ量
	float m_vForceDownValue = 0.0f;																				// ホッパー使用時のVフォース消費量
	float m_graduallyTorionDecVal = 0.0f;																		// 徐々に減ってくVフォース量
	float m_gravity = 0.0f;																						// 重力
	Math::Vector3 m_mWorldRot = {};																				// 角度
	int m_invincibilityTimeCnt = 0;																				// 無敵時間
	Math::Vector3 m_addCenterVal = {};																			// 足したら中心になる値
	bool m_bMove = false;																						// 動いてる true

	// 武器関連
	std::vector<std::shared_ptr<WeaponBase>> m_weaponList;														// 武器リスト
	UINT m_weaponType = 0;																						// 今現在装備している武器
	int m_rightWeaponNumber = 0;                                                                                // 今現在右に装備している武器ナンバー
	int m_leftWeaponNumber = 0;																					// 今現在左に装備している武器ナンバー

	// Hoperrを使ってのダッシュ関連
	Math::Vector3 m_grassHopperDashDir = {};                                                                    // 高速移動する方向ベクトル
	Math::Vector3 m_addGrassDashEffectPosVal = {};																// プレイヤーの座標に足すとエフェクトを出す位置になる
	int m_lGrassHopperPauCnt = 0;																				// 休止する時間
	int m_rGrassHopperPauCnt = 0;																				// 休止する時間
	int m_lGrassHopperTime = 0;																					// 左ホッパーでの高速移動時間
	int m_rGrassHopperTime = 0;																					// 右ホッパーでの高速移動時間
	float m_dashSpd = 0;                                                                                        // 高速移動の速さ

	// Scopion関連
	Math::Vector3 m_attackMoveDir;																				// 攻撃した時に移動する方向
	float m_attackMoveSpd;																						// 攻撃した時に移動する方向
	int m_attackAnimeCnt = 0;																					// 攻撃アニメーションのフレーム数
	int m_attackAnimeDelayCnt = 0;																				// 攻撃アニメーションを遅らせる時間
	bool m_bAttackAnimeDelay = false;																			// 攻撃アニメーションを遅らせる true
	int m_toleranceComboTime = 0;																				// コンボ許容時間
	bool m_bRushAttackPossible = false;																			// ラッシュアタックできる true		


	// 走りアニメーション関連
	int m_runAnimeCnt = 0;																						// 走りアニメーションのフレーム数

	// Hit関連
	int   m_hitStopCnt = 0;																						// 攻撃を受けてとまる時間
	int   m_hitColorChangeTimeCnt = 0;																			// 攻撃を受け赤くなる時間
	float m_hitMoveSpd = 0.0f;																					// 攻撃を受けたときの吹き飛ばしスピード
	Math::Vector3 m_knockBackVec;																				// ノックバックする方向
	bool m_bBlowingAwayHitB;																					// 後ろから吹き飛ばし攻撃を受けた true1
	int m_notUnderAttackTime;																					// 攻撃を受けていない時間

	// Step関連
	int m_stepCnt;																								// ステップをする時間															
	Math::Vector3 m_stepDashDir;																				// ステップする方向ベクトル

	// その他
	bool m_bFirstUpdate = false;
	int m_overStageTime = 0;																					// ステージ外に出た時間
	bool m_bDeath = false;																						// やられた true
	bool m_bLose = false;																						// 負けた true
	bool m_attackHit = false;																					// 一つの攻撃が複数回当たらないためのフラグ
	bool m_bDefenseSuc = false;																					// 防御に成功したフラグ
	bool m_bWaponDrawPosible = false;                                                                           // 武器を描画できる　true
	float m_moveSpeedDecelerationamount = 0.0f;																	// 減速時に使用

	UINT m_notHumanoidEnemyState;

private:
		HitInfo SphereColisionCheck(Math::Vector3 a_center, float a_radius, KdCollider::Type a_type, std::shared_ptr<WeaponBase> a_weapon, std::shared_ptr<Enemy> a_enemy, std::shared_ptr<Player> a_player, std::shared_ptr<KdGameObject> a_obj);
		HitInfo RayColisionCheck(Math::Vector3 a_pos, Math::Vector3 a_dir, float a_range, KdCollider::Type a_type, std::shared_ptr<WeaponBase> a_weapon, std::shared_ptr<Enemy> a_enemy, std::shared_ptr<Player> a_player, std::shared_ptr<KdGameObject> a_obj);
};