#pragma once
#define ENEMYAIRBORNETIMETOBECNTVAL 180

class WeaponBase;
class Player;

enum EnemyState
{
	eIdle = 1 << 0,
	eJump = 1 << 1,
	eFall = 1 << 2,
	eRun  = 1 << 3,
	eGrassHopperDashF = 1 << 4,
	eGrassHopperDashB = 1 << 5,
	eGrassHopperDashR = 1 << 6,
	eGrassHopperDashL = 1 << 7,
	eGrassHopperDash  = eGrassHopperDashF | eGrassHopperDashB | eGrassHopperDashR | eGrassHopperDashL,
	eGrassHopperDashUp = 1 << 8,
	eLAttackOne = 1 << 9,
	eLAttackTwo = 1 << 10,
	eLAttackThree = 1 << 11,
	eLAttack = eLAttackOne | eLAttackTwo | eLAttackThree,
	eRAttackOne = 1 << 12,
	eRAttackTwo = 1 << 13,
	eRAttackThree = 1 << 14,
	eRAttack = eRAttackOne | eRAttackTwo | eRAttackThree,
	eDefense = 1 << 15,
	eMantis = 1 << 16,
	eHasDefense = 1 << 17,
	eBlowingAwayHit = 1 << 18,
	eIaiKiriHit = 1 << 19,
	eNomalHit = 1 << 20,
	eCutRaiseHit = 1 << 21,
	eHit = eNomalHit | eIaiKiriHit | eBlowingAwayHit | eCutRaiseHit,
	eStepF = 1 << 22,
	eStepR = 1 << 23,
	eStepL = 1 << 24,
	eStepB = 1 << 25,
	eStep  = eStepF | eStepB | eStepR | eStepL,
	eBlowingAwayRise = 1 << 26,
	eIaiKiriRise = 1 << 27,
	eRise = eIaiKiriRise | eBlowingAwayRise,
	eRlAttackOne = 1 << 28,
	eRlAttackTwo = 1 << 29,
	eRlAttackThree = 1 << 30,
	eRlAttack = eRlAttackOne | eRlAttackTwo | eRlAttackThree,
	eRlAttackRush = 1 << 31,
};

enum eWeaponType
{
	eScorpion = 1 << 0,
	eGrassHopper = 1 << 1,

	eLScorpion = 1 << 4,
	eLGrassHopper = 1 << 5,
};

class Enemy : public KdGameObject
{
public:

	enum WantToMoveState
	{
		none             = 1 << 0,
		attack           = 1 << 1,
		escape           = 1 << 2,
		defense          = 1 << 3,
		dashAttack       = 1 << 4,
		run              = 1 << 5,
		disturbance      = 1 << 6,
		step             = 1 << 7,
		grassDash        = 1 << 8,
		avoidance        = 1 << 9,
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
		striker,
		defender,
		speedSter,
		allRounder
	};

	Enemy() {}
	~Enemy() {}

	void DrawLit()override;
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
	void SetPos(Math::Vector3& a_pos) { m_pos = a_pos;}
	//void Release() {};

	const UINT& GetEnemyState() { return m_EnemyState; }
	const UINT& GetWeaponType() { return m_weaponType; }

	const int& GetLGrassHopperTime() { return m_lGrassHopperTime; }
	const int& GetRGrassHopperTime() { return m_rGrassHopperTime; }

	const int& GetLGrassHopperPauCnt() { return m_lGrassHopperPauCnt; }
	const int& GetRGrassHopperPauCnt() { return m_rGrassHopperPauCnt; }


	const float& GetGravity() { return m_gravity; }
	const float& GetDashSpd() { return m_dashSpd;}
	const int GetInvincibilityTimeCnt() { return m_invincibilityTimeCnt; }
	const std::shared_ptr<KdModelWork>& GetModel() { return m_model; }
	const bool GetBRushAttackPossible() { return m_bRushAttackPossible; }
	void SetBRushAttackPossible(bool a_bRushAttackPossible) { m_bRushAttackPossible = a_bRushAttackPossible; }

	const int GetAnimationCnt() { return m_attackAnimeCnt; }
	std::vector<std::shared_ptr<WeaponBase>> GetWeaponList() { return m_weaponList; }
	void OnHit(Math::Vector3 a_KnocBackvec)override;
	void BlowingAwayAttackOnHit(Math::Vector3 a_KnocBackvec)override;
	void IaiKiriAttackOnHit(Math::Vector3 a_KnocBackvec)override;
	void CutRaiseOnHit(Math::Vector3 a_KnocBackvec)override;
	void HasDefense()override;

private:

	void EnemyKickHitAttackChaeck();
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

	std::shared_ptr<KdModelWork> m_model;

	const int MAXWEAPONTYPE = 2;
	const int FIRSTWEAPONTYPENUMBER = 1;

	float m_gravity = 0;
	int m_toleranceComboTime = 0; // �R���{���e����
	int m_m_toleranceAttackTime = 0; // �čU���Ԋu
	UINT m_EnemyState;
	UINT m_weaponType;
	UINT m_wantToMoveState;
	UINT m_wantToMoveCategory;
	UINT m_enemyType;

	int m_rightWeaponNumber = 0;
	int m_leftWeaponNumber = 0;

	bool m_bMove = false;
	bool m_bStateUnChange = false; // ��Ԃ�ύX�ł��Ȃ��Ƃ���true  �����������炱����Ӄr�b�g�Ǘ��ɂ��邩��
	bool m_bUnMove = false; // �����Ȃ��Ƃ���true
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
	int m_lGrassHopperPauCnt = 0; // �x�~���鎞��
	int m_rGrassHopperPauCnt = 0; // �x�~���鎞��

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
	int   m_defenseKnockbackCnt = 0;
	float m_hitMoveSpd = 0.0f;
	float m_gardMoveSpd;           // �U����h�䂵�����̃m�b�N�o�b�N���x
	bool  m_bTough;                // �����U���͂̍U�������炤OR�K�[�h�������ǂ���
	Math::Vector3 m_knockBackVec;  // �m�b�N�o�b�N�������
	float m_torion = 0.0f;
	float m_endurance = 0.0f;    // �g���I���̑ϋv��
	float m_graduallyTorionDecVal; // ���X�Ɍ����Ă��g���I����
	int m_delayTurnAroundTime; // �U��Ԃ��x�点�鎞��

	//int m_reactionAttackOrDefenseDelayTime;                   //�A�^�b�N���f�B�t�F���X��I�Ԃ̂ɂ�����܂ł̎��Ԃ��L��
	//int m_reactionAttackOrDefenseDelayTimeVal;                //�A�^�b�N���f�B�t�F���X��I�Ԃ̂ɂ�����܂ł̎���							
	//int m_reactionRunDelayTime;					              //����̂ɂ�����܂ł̎��Ԃ��L��
	//int m_reactionRunDelayTimeVal;				              //����̂ɂ�����܂ł̎���
	//int m_reactionSwitchingDelayTime;			              //����̐ؑւɂ�����܂ł̎��Ԃ��L��											
	//int m_reactionSwitchingDelayTimeVal;		              //����̐ؑւɂ�����܂ł̎���

	int m_actionDelayTimeVal;
	int m_actionDelayTime;
	int m_thinkActionDelayTimeVal;
	int m_thinkActionDelayTime;

	bool m_bAttackOrDefense;
	bool m_bEscape;
	bool m_bDashAttack;

	bool m_attackHitImmediatelyAfter; // �U�������������
	int m_grassSuccessionDelayCnt;
	int m_enemyAirborneTimetoBeCnt;
	Math::Vector3 m_attackMoveDir; // �U���������Ɉړ��������
	float         m_attackMoveSpd; // �U���������Ɉړ��������
	int m_invincibilityTimeCnt;  // ���G����

	bool m_bRushAttackPossible = false;

	int m_disturbanceCnt = 0;

	bool m_bMantisAttack = false; // �}���e�B�X�A�^�b�N�����Ăق�������true	�����l��false

	int m_stepCnt;
	Math::Vector3 m_stepDashDir;
	bool m_bMantisPossAng; // �}���e�B�X��������p�x true
	int m_hasDeTime;
};