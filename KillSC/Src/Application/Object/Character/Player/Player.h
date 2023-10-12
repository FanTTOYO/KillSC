#pragma once
#define TORIONDOWNVALUE 1.0f
#define  GRASSHOPPERPAUCNT 30

class CameraBase;
class WeaponBase;
class Enemy;
class Ui;

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
		run  = 1 << 3,
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
		step  = stepF | stepB | stepR | stepL,
		blowingAwayRise  = 1 << 26,
		iaiKiriRise      = 1 << 27,
		rise             = iaiKiriRise | blowingAwayRise,
		rlAttackOne      = 1 << 28,
		rlAttackTwo      = 1 << 29,
		rlAttackThree    = 1 << 30,
		rlAttack         = rlAttackOne | rlAttackTwo | rlAttackThree,
		/*rlAttackRushOne     = 10000000 << 0,
		rlAttackRushTwo     = 10000000 << 1,
		rlAttackRushThree   = 10000000 << 2,
		rlAttackRushFour    = 10000000 << 3,
		rlAttackRushFive    = 10000000 << 4,
		rlAttackRushSix     = 10000000 << 5,
		rlAttackRushSeven   = 10000000 << 6,
		rlAttackRusEight    = 10000000 << 7,*/
		rlAttackRush        = 1 << 31  ,

	};

	Player() {}
	~Player() {}

	void Update()								override;
	void PostUpdate()							override;
	void DrawLit()								override;
	void DrawUnLit()							override;
	void DrawBright()							override;
	void DrawSprite()							override;
	void GenerateDepthMapFromLight()			override;
	void GenerateDepthMapFromLight_SkinMesh()	override;
	void DrawLit_SkinMesh()						override;
	void DrawDebug()override;
	void Init()									override;
	void Release() {};

	void SetCamera(std::shared_ptr<CameraBase> a_camera) { m_wpCamera = a_camera; }
	// void SetEnemy(std::shared_ptr<Enemy> a_enemy) { m_enemy = a_enemy; }
	void AddEnemy(std::shared_ptr<Enemy> a_enemy) { m_enemyList.push_back(a_enemy); }
	void AddWeaponToEnemy(std::shared_ptr<Enemy> a_enemy);
	void SetUi(std::shared_ptr<Ui> a_ui) { m_wpUi = a_ui; }

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
	const bool GetBRushAttackPossible() { return m_bRushAttackPossible; }
	void SetBRushAttackPossible(bool a_bRushAttackPossible) { m_bRushAttackPossible = a_bRushAttackPossible; }

	const int GetAnimationCnt() { return m_attackAnimeCnt; }
	std::vector<std::shared_ptr<WeaponBase>> GetWeaponList() { return m_weaponList; }

	void OnHit(Math::Vector3 a_KnocBackvec)override;
	void BlowingAwayAttackOnHit(Math::Vector3 a_KnocBackvec)override;
	void IaiKiriAttackOnHit(Math::Vector3 a_KnocBackvec)override;
	void CutRaiseOnHit(Math::Vector3 a_KnocBackvec)override;
	void HasDefense()override;

	const bool GetBPlayerDeath() { return m_bPlayerDeath; }

private:

	void PlayerKickHitAttackChaeck();
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
	void TutorialUpdate();

	std::shared_ptr<KdModelWork> m_model;

	const int MAXWEAPONTYPE = 2;
	const int FIRSTWEAPONTYPENUMBER = 1;

	float m_gravity = 0;
	int m_toleranceComboTime = 0; // ƒRƒ“ƒ{‹–—eŠÔ
	int m_m_toleranceAttackTime = 0; // ÄUŒ‚ŠÔŠu
	UINT m_playerState;
	UINT m_weaponType;

	int m_rightWeaponNumber = 0;
	int m_leftWeaponNumber = 0;

	bool m_bMove = false;
	bool m_bStateUnChange = false; // ó‘Ô‚ğ•ÏX‚Å‚«‚È‚¢‚Æ‚«‚Étrue  ‚à‚µ‚©‚µ‚½‚ç‚±‚±‚ç•ÓƒrƒbƒgŠÇ—‚É‚·‚é‚©‚à
	bool m_bUnMove = false; // “®‚¯‚È‚¢‚Æ‚«‚Étrue
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
	int m_lGrassHopperPauCnt = 0; // ‹x~‚·‚éŠÔ
	int m_rGrassHopperPauCnt = 0; // ‹x~‚·‚éŠÔ

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
	std::list<std::weak_ptr<Enemy>> m_enemyList;

	int m_attackAnimeCnt = 0;
	int m_runAnimeCnt = 0;
	int m_attackAnimeDelayCnt = 0;
	bool m_bAttackAnimeDelay = false;
	bool m_bAttackAnimeCnt = false;

	int   m_hitStopCnt = 0;
	int   m_defenseKnockbackCnt = 0;
	float m_hitMoveSpd = 0.0f;
	float m_gardMoveSpd;           // UŒ‚‚ğ–hŒä‚µ‚½‚ÌƒmƒbƒNƒoƒbƒN‘¬“x
	bool  m_bTough;                // ‚‚¢UŒ‚—Í‚ÌUŒ‚‚ğ‚­‚ç‚¤ORƒK[ƒh‚µ‚½‚©‚Ç‚¤‚©
	Math::Vector3 m_knockBackVec;  // ƒmƒbƒNƒoƒbƒN‚·‚é•ûŒü
	float m_torion    = 0.0f;    // ƒgƒŠƒIƒ“c—Ê
	float m_endurance = 0.0f;    // ƒgƒŠƒIƒ“‘Ì‘Ï‹v—Í
	float m_graduallyTorionDecVal; // ™X‚ÉŒ¸‚Á‚Ä‚­ƒgƒŠƒIƒ“—Ê
	int m_delayTurnAroundTime; // U‚è•Ô‚è‚ğ’x‚ç‚¹‚éŠÔ

	int m_stepCnt;
	Math::Vector3 m_stepDashDir;

	Math::Vector3 m_attackMoveDir; // UŒ‚‚µ‚½‚ÉˆÚ“®‚·‚é•ûŒü
	float         m_attackMoveSpd; // UŒ‚‚µ‚½‚ÉˆÚ“®‚·‚é•ûŒü

	int m_invincibilityTimeCnt;  // –³“GŠÔ

	bool m_bRushAttackPossible = false;
	std::weak_ptr<Ui> m_wpUi;
	int m_tuGardTime;

	int m_overStageTime;

	bool m_bPlayerDeath;

	std::shared_ptr<KdSquarePolygon> m_rocKOnPolygon;

	Math::Matrix m_rockOnPolyMat;
};