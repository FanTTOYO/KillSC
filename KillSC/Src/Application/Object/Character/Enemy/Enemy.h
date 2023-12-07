#pragma once
#define ENEMYAIRBORNETIMETOBECNTVAL			180
#define INITIALPOSZ							 10
#define INITIALROTATIONY					180
#define ADDWEAKNESSEUCCESSIONHITCNTTIMELIMIT 90 // 連続攻撃と認められる時間
#define ADDROTAYIONATTACKDISTTOPLAYERTIME   120 // 攻撃開始する時間
#define ROTAYIONATTACKDISTTOPLAYERINITTIME   60 // 攻撃開始時間を初期化する時間
#define ATTACKDELAYTIME						120 // 攻撃できない時間

class WeaponBase;
class Player;

enum eWeaponType
{
	eWeaponNone  = 1 << 0,
	eScorpion    = 1 << 1,
	eGrassHopper = 1 << 2,

	eLWeaponNone   = 1 << 4,
	eLScorpion    = 1 << 5,
	eLGrassHopper = 1 << 6,
};

class Enemy : public KdGameObject
{
public:
	enum EnemyState
	{
		idle = 1 << 0,
		jump = 1 << 1,
		fall = 1 << 2,
		run = 1 << 3,
		grassHopperDashF = 1 << 4,
		grassHopperDashB = 1 << 5,
		grassHopperDashR = 1 << 6,
		grassHopperDashL = 1 << 7,
		grassHopperDash = grassHopperDashF | grassHopperDashB | grassHopperDashR | grassHopperDashL,
		grassHopperDashUp = 1 << 8,
		lAttackOne = 1 << 9,
		lAttackTwo = 1 << 10,
		lAttackThree = 1 << 11,
		lAttack = lAttackOne | lAttackTwo | lAttackThree,
		rAttackOne = 1 << 12,
		rAttackTwo = 1 << 13,
		rAttackThree = 1 << 14,
		rAttack = rAttackOne | rAttackTwo | rAttackThree,
		defense = 1 << 15,
		mantis = 1 << 16,
		hasDefense = 1 << 17,
		blowingAwayHit = 1 << 18,
		iaiKiriHit = 1 << 19,
		nomalHit = 1 << 20,
		cutRaiseHit = 1 << 21,
		hit = nomalHit | iaiKiriHit | blowingAwayHit | cutRaiseHit,
		stepF = 1 << 22,
		stepR = 1 << 23,
		stepL = 1 << 24,
		stepB = 1 << 25,
		step = stepF | stepB | stepR | stepL,
		blowingAwayRise = 1 << 26,
		iaiKiriRise = 1 << 27,
		rise = iaiKiriRise | blowingAwayRise,
		rlAttackOne = 1 << 28,
		rlAttackTwo = 1 << 29,
		rlAttackThree = 1 << 30,
		rlAttack = rlAttackOne | rlAttackTwo | rlAttackThree,
		rlAttackRush = 1 << 31,
	};

	enum WantToMoveState
	{
		wNone             = 1 << 0,
		wAttack           = 1 << 1,
		wEscape           = 1 << 2,
		wDefense          = 1 << 3,
		wDashAttack       = 1 << 4,
		wRun              = 1 << 5,
		wStep             = 1 << 6,
		wGrassDash        = 1 << 7,
		wAvoidance        = 1 << 8,
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
		coarseFishEnemy = 1 << 4,
		wimpEnemyTypeOne = 1 << 5,
		bossEnemyTypeOne = 1 << 6,
	};

	enum NotHumanoidEnemyState
	{
		stand			= 1 << 0,
		weaknessHit		= 1 << 1,
		rangedAttack	= 1 << 2,
		rotationAttack	= 1 << 3,

	};

	Enemy() {}
	~Enemy() {}

	void DrawLit()override;
	void DrawUnLit()override;
	void GenerateDepthMapFromLight_SkinMesh()	override;
	void DrawLit_SkinMesh()						override;
	void Init()override;

	void Update()override;
	void PostUpdate()override;
	void DrawBright()override;
	void DrawSprite()override;
	void DrawDebug()override;

	void GenerateDepthMapFromLight()override;
	void SetTarget(std::shared_ptr<Player> a_target) { m_target = a_target; }
	void SetPos(Math::Vector3 a_pos) { m_pos = a_pos;}
	void SetMatrix(Math::Vector3 a_pos) { m_mWorld = Math::Matrix::CreateTranslation(a_pos); }
	void SetWorldRotationY(float a_mWorldRotY) { m_mWorldRot.y = a_mWorldRotY; }
	void SetModelAndType(EnemyType a_enemyType); // この敵が雑魚敵（人型）か雑魚敵（モンスター）かをいれる　ボスの場合はつかわない
	//void Release() {};

	const UINT& GetEnemyState() { return m_EnemyState; }
	const UINT& GetWeaponType() { return m_weaponType; }

	const int& GetLGrassHopperTime() { return m_lGrassHopperTime; }
	const int& GetRGrassHopperTime() { return m_rGrassHopperTime; }

	const int& GetLGrassHopperPauCnt() { return m_lGrassHopperPauCnt; }
	const int& GetRGrassHopperPauCnt() { return m_rGrassHopperPauCnt; }


	const float& GetGravity() { return m_gravity; }
	const float& GetDashSpd() { return m_dashSpd;}
	const float& GetEndurance() { return m_endurance; }
	const int GetInvincibilityTimeCnt() { return m_invincibilityTimeCnt; }
	const std::shared_ptr<KdModelWork>& GetModel() { return m_model; }
	const bool GetBRushAttackPossible() { return m_bRushAttackPossible; }
	void SetBRushAttackPossible(bool a_bRushAttackPossible) { m_bRushAttackPossible = a_bRushAttackPossible; }

	const int GetAnimationCnt() { return m_attackAnimeCnt; }
	std::vector<std::shared_ptr<WeaponBase>> GetWeaponList() { return m_weaponList; }
	void OnHit(Math::Vector3 a_KnocBackvec);
	void BlowingAwayAttackOnHit(Math::Vector3 a_KnocBackvec);
	void IaiKiriAttackOnHit(Math::Vector3 a_KnocBackvec);
	void CutRaiseOnHit(Math::Vector3 a_KnocBackvec);
	void WeaknessOnHit();
	void HasDefense();

	const float GetAngleY() { return m_mWorldRot.y; }

	const bool GetBEnemyLose() { return m_bEnemyLose; }

	const bool GetBEnemyDeath() { return m_bEnemyDeath; }

	void SetEnemyNumber(const int& a_enemyNumber) { m_enemyNumber = a_enemyNumber; }

	void SetBBoss(bool a_bBoss) { m_bBoss = a_bBoss; }
	const bool GetBBoss() { return m_bBoss; }

	const UINT& GetEnemyType() { return m_enemyType; }
	const UINT& GetNotHumanoidEnemyState() { return m_notHumanoidEnemyState; }

	const bool GetBShotBeam() { return m_bShotBeam; }
	const bool GetBShotEnergyBullet() { return m_bShotEnergyBullet; }

	const int GetEnemyAttackTotal() { return m_enemyAttackTotal; }
	void AddEnemyAttackTotal() { ++m_enemyAttackTotal; }
	void SetEnemyAttackTotal(int a_enemyAttackTotal) { m_enemyAttackTotal = a_enemyAttackTotal; }

	const bool GetBRangedAttack() { return m_bRangedAttack; }

private:

	void CollisionUpdate();
	void BossUpdate();
	void BossEnemyTyepOneUpdate();
	void CoarseFishEnemyUpdate();
	void WimpEnemyTypeOneUpdate();
	void TutorialUpdate();
	void EnemyKickHitAttackChaeck();
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

	std::shared_ptr<KdModelWork> m_model;

	const int MAXWEAPONTYPE = 2;
	const int FIRSTWEAPONTYPENUMBER = 1;

	float m_gravity = 0;
	int m_toleranceComboTime = 0; // コンボ許容時間
	int m_m_toleranceAttackTime = 0; // 再攻撃間隔
	UINT m_EnemyState;
	UINT m_weaponType;
	UINT m_wantToMoveState;
	UINT m_wantToMoveCategory;
	UINT m_enemyType;
	UINT m_notHumanoidEnemyState;

	int m_rightWeaponNumber = 0;
	int m_leftWeaponNumber = 0;

	bool m_bMove = false;
	bool m_bStateUnChange = false; // 状態を変更できないときにtrue  もしかしたらここら辺ビット管理にするかも
	bool m_bUnMove = false; // 動けないときにtrue
	bool m_bJump = false;

	bool m_bLeftWallHit = false;
	bool m_bRightWallHit = false;

	Math::Vector3    m_pos = {};
	Math::Vector3    m_move = {};
	Math::Vector3    m_scale = {};

	Math::Matrix m_deathWorldMat;
	int          m_isExpiredCnt = 0;
	bool         m_firstUnLive = false;
	int          m_dashDerayCnt = 0;
	float        m_damageTotal = 0;
	int          m_damageTotalAddTime = 0;
	KdTexture m_hPTex;
	KdTexture m_hPBarTex;
	KdTexture m_mPTex;

	Math::Vector3					 m_mWorldRot;

	bool m_bSpaceState;
	int m_lGrassHopperPauCnt = 0; // 休止する時間
	int m_rGrassHopperPauCnt = 0; // 休止する時間

	int m_lGrassHopperTime = 0;
	int m_rGrassHopperTime = 0;

	bool m_bLShiftKeyState = false;
	bool m_bFKeyState = false;
	Math::Vector3 m_grassHopperDashDir = {};
	float m_dashSpd = 0;

	std::shared_ptr<KdAnimator> m_animator;

	std::vector<std::shared_ptr<WeaponBase>> m_weaponList;

	bool m_bFirstUpdate = false;
	bool m_bEwaponDrawPosible = false;

	std::weak_ptr<Player> m_target;
	float m_targetAng;

	int m_attackAnimeCnt = 0;
	int m_attackAnimeDelayCnt = 0;
	bool m_bAttackAnimeDelay = false;
	bool m_bAttackAnimeCnt = false;

	int m_runAnimeCnt = 0;
	int   m_hitStopCnt = 0;
	int   m_hitColorChangeTimeCnt = 0;
	int   m_defenseKnockbackCnt = 0;
	float m_hitMoveSpd = 0.0f;
	float m_gardMoveSpd;           // 攻撃を防御した時のノックバック速度
	bool  m_bTough;                // 高い攻撃力の攻撃をくらうORガードしたかどうか
	Math::Vector3 m_knockBackVec;  // ノックバックする方向
	float m_torion = 0.0f;
	float m_endurance = 0.0f;    // トリオン体耐久力
	float m_graduallyTorionDecVal; // 徐々に減ってくトリオン量
	int m_delayTurnAroundTime; // 振り返りを遅らせる時間

	//int m_reactionAttackOrDefenseDelayTime;                   //アタックかディフェンスを選ぶのにかかるまでの時間を記憶
	//int m_reactionAttackOrDefenseDelayTimeVal;                //アタックかディフェンスを選ぶのにかかるまでの時間							
	//int m_reactionRunDelayTime;					              //走るのにかかるまでの時間を記憶
	//int m_reactionRunDelayTimeVal;				              //走るのにかかるまでの時間
	//int m_reactionSwitchingDelayTime;			              //武器の切替にかかるまでの時間を記憶											
	//int m_reactionSwitchingDelayTimeVal;		              //武器の切替にかかるまでの時間

	int m_actionDelayTimeVal;
	int m_actionDelayTime;
	int m_thinkActionDelayTimeVal;
	int m_thinkActionDelayTime;

	bool m_bAttackOrDefense;
	bool m_bEscape;
	bool m_bDashAttack;

	bool m_attackHitImmediatelyAfter; // 攻撃くらった直後
	int m_grassSuccessionDelayCnt;
	int m_enemyAirborneTimetoBeCnt;
	Math::Vector3 m_attackMoveDir; // 攻撃した時に移動する方向
	float         m_attackMoveSpd; // 攻撃した時に移動する方向
	int m_invincibilityTimeCnt;  // 無敵時間

	bool m_bRushAttackPossible = false;

	int m_disturbanceCnt = 0;

	bool m_bMantisAttack = false; // マンティスアタックをしてほしい時にtrue	初期値はfalse

	int m_stepCnt;
	Math::Vector3 m_stepDashDir;
	bool m_bMantisPossAng; // マンティスがあたる角度 true
	int m_hasDeTime;

	KdPolygon m_enduranceBarPoly;
	KdPolygon m_endurancePoly;

	bool m_bEnemyLose;
	bool m_bEnemyDeath;
	int m_overStageTime;

	bool          m_bEnemyBetweenPlayer;
	Math::Vector3 m_enemyBetweenPlayerHitPos;

	int m_enemyNumber;

	bool m_bBlowingAwayHitB; // 後ろから吹き飛ばされた

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

	Math::Vector3 m_hitpos;

	int m_enemyAttackTotal;
	int m_enemyAttackMaxTotal;

	bool m_bAttackEnd; // 攻撃が終了した時 : true

	bool  m_bRangedAttackCapableOfFiring; // 遠距離攻撃を打てるか : true;

	int m_weaknesSsuccessionHitCnt;             // 連続で弱点に当たった回数
	int m_addWeaknesSsuccessionHitCntTime;      // 弱点Hitから何秒経過したか計る

	int m_addRotationAttackDistToPlayerTime;	    // 回転攻撃範囲内にPlayerがいる時間を計る
	int m_rotationAttackDistToPlayerTimeInitTime;	// 回転攻撃範囲内にPlayerがいない時間を計る

	int m_attackDelayTime;
};