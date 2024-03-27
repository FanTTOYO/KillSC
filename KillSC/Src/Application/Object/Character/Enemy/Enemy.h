#pragma once
#define ENEMY_AIRBORNE_TIME_TO_BE_CNT_VAL			180
#define ADD_WEAKNES_SEUCCESSION_HIT_CNT_TIME_LIMIT	90 // 連続攻撃と認められる時間
#define ADDROTAYIONATTACKDISTTOPLAYERTIME			120 // 攻撃開始する時間
#define ROTAYIONATTACKDISTTOPLAYERINITTIME			60 // 攻撃開始時間を初期化する時間
#define ATTACKDELAYTIME								120 // 攻撃できない時間
#include "../CharacterBase.h"

class WeaponBase;
class Player;

class Enemy : public CharacterBase
{
public:
	enum WantToMoveState
	{
		wNone = 1 << 0,
		wAttack = 1 << 1,
		wEscape = 1 << 2,
		wDefense = 1 << 3,
		wDashAttack = 1 << 4,
		wRun = 1 << 5,
		wStep = 1 << 6,
		wGrassDash = 1 << 7,
		wAvoidance = 1 << 8,
	};

	enum WantToMoveCategory
	{
		attackCategory = 1 << 0,
		defenseCategory = 1 << 1,
		approachCategory = 1 << 2,
		runCategory = 1 << 3,
		grassCategory = 1 << 4,
	};

	enum EnemyType
	{
		striker = 1 << 0,
		defender = 1 << 1,
		speedSter = 1 << 2,
		allRounder = 1 << 3,
		humanBossEnemy = allRounder | speedSter | defender | striker,
		coarseFishEnemy = 1 << 4,
		wimpEnemyTypeOne = 1 << 5,
		bossEnemyTypeOne = 1 << 6,
	};

	enum NotHumanoidEnemyState
	{
		stand = 1 << 0,
		weaknessHit = 1 << 1,
		rangedAttack = 1 << 2,
		rotationAttack = 1 << 3,

	};

	Enemy() {}
	~Enemy() {}

	void DrawLit_SkinMesh()						override;
	void Init(std::weak_ptr<json11::Json> a_wpJsonObj);

	void Update()override;
	void PostUpdate()override;

	void SetTarget(std::shared_ptr<Player> a_target) { m_target = a_target; }
	void SetWeaponToTarget(std::shared_ptr<Player> a_spPlayer);
	void SetMatrix();
	void SetModelAndType(EnemyType a_enemyType); // この敵が雑魚敵（人型）か雑魚敵（モンスター）かをいれる　ボスの場合はつかわない

	const float& GetMaxEndurance() { return m_maxEndurance; }

	const int GetAnimationCnt() { return m_attackAnimeCnt; }

	void OnHit(Math::Vector3 a_KnocBackvec, CharacterBase* a_pAttackedCharacter)override;
	void BlowingAwayAttackOnHit(Math::Vector3 a_KnocBackvec, CharacterBase* a_pAttackedCharacter)override;
	void IaiKiriAttackOnHit(Math::Vector3 a_KnocBackvec)override;
	void CutRaiseOnHit(Math::Vector3 a_KnocBackvec)override;
	void HasDefense()override;
	void WeaknessOnHit();

	const float GetAngleY() { return m_mWorldRot.y; }

	void SetEnemyNumber(const int& a_enemyNumber) { m_enemyNumber = a_enemyNumber; }

	void SetBBoss(bool a_bBoss) { m_bBoss = a_bBoss; }
	const bool GetBBoss() { return m_bBoss; }

	const UINT& GetEnemyType() { return m_enemyType; }

	const bool GetBShotBeam() { return m_bShotBeam; }
	const bool GetBShotEnergyBullet() { return m_bShotEnergyBullet; }

	const int GetEnemyAttackTotal() { return m_enemyAttackTotal; }
	void AddEnemyAttackTotal() { ++m_enemyAttackTotal; }
	void SetEnemyAttackTotal(int a_enemyAttackTotal) { m_enemyAttackTotal = a_enemyAttackTotal; }

	const bool GetBRangedAttack() { return m_bRangedAttack; }

	const Math::Vector3& GetRockOnPos() { return m_rockOnPos; }
	const float GetAddHpPosY() { return m_addHpPosY; }

private:

	void CollisionUpdate();
	void HumanTypeEnemyRayCollisionUpdate();																		// 人型の敵関連のレイ判定処理
	void HumanTypeEnemySphereCollisionUpdate();																		// 人型の敵関連の球形判定処理
	void MonsterTypeEnemyRayCollisionUpdate();																		// モンスター型の敵関連のレイ判定処理
	void MonsterTypeEnemySphereCollisionUpdate();																	// モンスター型の敵関連の球形判定処理
	void SpeedyMoveWallHitChack(float& a_moveSpd, Math::Vector3 moveVec);											// 速い動きの時の壁との当たり判定
	void BossUpdate();
	void BossEnemyTyepOneUpdate();
	void CoarseFishEnemyUpdate();
	void WimpEnemyTypeOneUpdate();
	void TutorialUpdate();
	void EnemyBeamHitChaeck();
	void EnemyEnergyBulletHitChaeck();
	void UpdateRotate(Math::Vector3& a_srcMoveVec);
	void GrassMoveVecDecision();
	void ScorpionDefenseDecision();
	void ScorpionAttackDecision();
	void ScorpionAttackMove();
	void ScorpionDefenseMove();
	void GrassMove();
	void StepMove();
	void StepVecDecision();
	void NormalMove();
	void NormalMoveVecDecision();
	void HasDefenseMove();
	void Brain();

	void StrikerBrain();
	void DefenderBrain();
	void SpeedSterBrain();
	void AllRounderBrain();
	void CoarseFishEnemyBrain();
	void WimpEnemyBrain();
	void BossEnemyTypeOneBrain();
	void EnergyCharge(bool a_bBeem);
	void RotationAttackMove();
	void RotationAttackChaeck();
	std::string SetGrassDashEffectName(int a_efectNumber);
	int SetRandInfo(int a_randMaxNum, int* a_randItemized);
	void DecideWeaponNumber();

	Math::Vector3 m_rockOnPos;																						// ロックオンされる位置
	float         m_addHpPosY;																						// HPの表示位置に足す

	UINT m_wantToMoveState;
	UINT m_wantToMoveCategory;
	UINT m_enemyType;

	std::weak_ptr<Player> m_target;
	float m_targetAng;

	float m_maxEndurance = 0.0f;    // トリオン体耐久力の最大値
	int m_delayTurnAroundTime;		// 振り返りを遅らせる時間

	bool m_bAttackOrDefense;
	bool m_bEscape;
	bool m_bDashAttack;

	bool m_attackHitImmediatelyAfter; // 攻撃くらった直後
	int m_grassSuccessionDelayCnt;
	int m_enemyAirborneTimetoBeCnt;

	int m_disturbanceCnt = 0;

	bool m_bMantisAttack = false; // マンティスアタックをしてほしい時にtrue	初期値はfalse

	bool m_bMantisPossAng; // マンティスがあたる角度 true
	int m_hasDeTime;

	bool          m_bEnemyBetweenPlayer;
	Math::Vector3 m_enemyBetweenPlayerHitPos;

	int m_enemyNumber;

	bool m_bBoss = false; // true ボス　false 雑魚敵

	int m_coarseFishEnemyAttackDelayCnt;

	int m_idleCnt; // なにもしてな時間を計る

	bool  m_bShotBeam;			// 打った時 : true
	bool  m_bShotEnergyBullet;	// 打った時 : true
	float m_beamRange;			// ビームの当たり判定の長さ
	Math::Vector3 m_beamCollisionPos = {}; // エネルギー弾が当たり判定する場所
	Math::Vector3 m_energyBulletDir = {}; // エネルギー弾が飛んでく方向

	Math::Vector3 m_rangedAttackDir = {}; // 遠距離攻撃が飛んでいく方向
	//Math::Vector3 m_rangedAttackDir = {}; // 遠距離攻撃が飛んでいく方向

	int   m_rangedAttackAnimeCnt; // 遠距離攻撃系のエフェクトのアニメーションカウント
	bool  m_bRangedAttack; // 遠距離攻撃をしてる true;
	Math::Vector3 m_rangedAttackTargetPos; // 遠距離攻撃系の狙っている場所
	Math::Vector3 m_rangedAttackShotPos; // 遠距離攻撃系攻撃を発射した地点


	bool  m_bBeamHitStart;	        // 当たり判定してる時 : true
	bool  m_bEnergyBulletHitStart;	// 当たり判定してる時 : true

	int m_enemyAttackTotal;
	int m_enemyAttackMaxTotal;

	bool m_bAttackEnd; // 攻撃が終了した時 : true

	bool  m_bRangedAttackCapableOfFiring; // 遠距離攻撃を打てるか : true;

	int m_weaknesSuccessionHitCnt;             // 連続で弱点に当たった回数
	int m_addWeaknesSuccessionHitCntTime;      // 弱点Hitから何秒経過したか計る

	int m_addRotationAttackDistToPlayerTime;	    // 回転攻撃範囲内にPlayerがいる時間を計る
	int m_rotationAttackDistToPlayerTimeInitTime;	// 回転攻撃範囲内にPlayerがいない時間を計る
	float m_addRotationAttackDistToPlayerRadius;    // 回転攻撃範囲の半径

	int m_attackDelayTime;

	bool m_bLethalDamageToKickOrPantciOrDashAttackHit; // 致死的なキックかパンチかダッシュアタックが当たった時　true
};