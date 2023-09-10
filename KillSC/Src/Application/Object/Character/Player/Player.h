#pragma once
#define TORIONDOWNVALUE 1.0f
#define  GRASSHOPPERPAUCNT 30

class CameraBase;
class WeaponBase;
class Enemy;

enum WeaponType
{
	scorpion    = 1 << 0,
	grassHopper = 1 << 1,

	lScorpion    = 1 << 4,
	lGrassHopper = 1 << 5,
};

class Player : public KdGameObject
{
public:

	enum PlayerState
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
		hit = nomalHit | iaiKiriHit | blowingAwayHit,
		stepF = 1 << 21,
		stepR = 1 << 22,
		stepL = 1 << 23,
		stepB = 1 << 24,
		step  = stepF | stepB | stepR | stepL,
		blowingAwayRise  = 1 << 25,
		iaiKiriRise      = 1 << 26,
		rise             = iaiKiriRise | blowingAwayRise,
	};

	Player() {}
	~Player() {}

	void Update()								override;
	void PostUpdate()							override;
	void DrawLit()								override;
	void DrawBright()							override;
	void DrawSprite()							override;
	void GenerateDepthMapFromLight()			override;
	void GenerateDepthMapFromLight_SkinMesh()	override;
	void DrawLit_SkinMesh()						override;
	void DrawDebug()override;
	void Init()									override;
	void Release() {};

	void SetCamera(std::shared_ptr<CameraBase> a_camera) { m_wpCamera = a_camera; }
	void SetEnemy(std::shared_ptr<Enemy> a_enemy) { m_enemy = a_enemy; }

	const UINT& GetPlayerState() { return m_playerState; }
	const UINT& GetWeaponType() { return m_weaponType; }

	const int& GetLGrassHopperTime() { return m_lGrassHopperTime; }
	const int& GetRGrassHopperTime() { return m_rGrassHopperTime; }

	const int& GetLGrassHopperPauCnt() { return m_lGrassHopperPauCnt; }
	const int& GetRGrassHopperPauCnt() { return m_rGrassHopperPauCnt; }


	const float& GetGravity() { return m_gravity; }
	const float& GetDashSpd() { return m_dashSpd;}
	const float& GetTorion()  { return m_torion; }
	const float& GetEndurance()  { return m_endurance; }
	const std::shared_ptr<KdModelWork>& GetModel() { return m_model; }
	const int GetInvincibilityTimeCnt() { return m_invincibilityTimeCnt; }
	std::vector<std::shared_ptr<WeaponBase>> GetWeaponList() { return m_weaponList; }

	void OnHit(Math::Vector3 a_KnocBackvec)override;
	void BlowingAwayAttackOnHit(Math::Vector3 a_KnocBackvec)override;
	void IaiKiriAttackOnHit(Math::Vector3 a_KnocBackvec)override;
	void HasDefense()override;

private:

	void UpdateRotate(Math::Vector3& a_srcMoveVec);
	void GrassMoveVecDecision();
	void ScorpionActionDecision();
	void GrassMove();
	void StepMove();
	//void GrassAttackMove();
	void NormalMove();
	void ScorpionAttackMove();
	void ScorpionDefenseMove();
	void HasDefenseMove();

	std::shared_ptr<KdModelWork> m_model;

	const int MAXWEAPONTYPE = 2;
	const int FIRSTWEAPONTYPENUMBER = 1;

	float m_gravity = 0;
	int m_toleranceComboTime = 0; // コンボ許容時間
	int m_m_toleranceAttackTime = 0; // 再攻撃間隔
	UINT m_playerState;
	UINT m_weaponType;

	int m_rightWeaponNumber = 0;
	int m_leftWeaponNumber = 0;

	bool m_bMove = false;
	bool m_bStateUnChange = false; // 状態を変更できないときにtrue  もしかしたらここら辺ビット管理にするかも
	bool m_bUnMove = false; // 動けないときにtrue
	bool m_bJump = false;

	bool m_bLeftWallHit = false;
	bool m_bRightWallHit = false;
	bool m_bMButtonState;

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

	std::weak_ptr<CameraBase>        m_wpCamera;
	Math::Vector3					 m_mWorldRot;

	//bool m_bSpaceState;
	int m_lGrassHopperPauCnt = 0; // 休止する時間
	int m_rGrassHopperPauCnt = 0; // 休止する時間

	int m_lGrassHopperTime = 0;
	int m_rGrassHopperTime = 0;
	Math::Matrix m_grasRotYMat;

	bool m_bLShiftKeyState = false;
	bool m_bFKeyState = false;
	Math::Vector3 m_grassHopperDashDir = {};
	float m_dashSpd = 0;

	std::shared_ptr<KdAnimator> m_animator;

	std::vector<std::shared_ptr<WeaponBase>> m_weaponList;

	bool m_bFirstUpdate = false;
	bool m_bEwaponDrawPosible = false;


	std::weak_ptr<Enemy> m_enemy;

	int m_attackAnimeCnt = 0;
	int m_runAnimeCnt = 0;
	int m_attackAnimeDelayCnt = 0;
	bool m_bAttackAnimeDelay = false;
	bool m_bAttackAnimeCnt = false;

	int   m_hitStopCnt = 0;
	int   m_defenseKnockbackCnt = 0;
	float m_hitMoveSpd = 0.0f;
	float m_gardMoveSpd;           // 攻撃を防御した時のノックバック速度
	bool  m_bTough;                // 高い攻撃力の攻撃をくらうORガードしたかどうか
	Math::Vector3 m_knockBackVec;  // ノックバックする方向
	float m_torion    = 0.0f;    // トリオン残量
	float m_endurance = 0.0f;    // トリオン体耐久力
	float m_graduallyTorionDecVal; // 徐々に減ってくトリオン量
	int m_delayTurnAroundTime; // 振り返りを遅らせる時間

	int m_stepCnt;
	Math::Vector3 m_stepDashDir;

	Math::Vector3 m_attackMoveDir; // 攻撃した時に移動する方向
	float         m_attackMoveSpd; // 攻撃した時に移動する方向

	int m_invincibilityTimeCnt;  // 無敵時間
};