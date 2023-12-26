#pragma once
#define ENEMYAIRBORNETIMETOBECNTVAL			180
#define INITIALPOSZ							 10
#define INITIALROTATIONY					180
#define ADDWEAKNESSEUCCESSIONHITCNTTIMELIMIT 90 // �A���U���ƔF�߂��鎞��
#define ADDROTAYIONATTACKDISTTOPLAYERTIME   120 // �U���J�n���鎞��
#define ROTAYIONATTACKDISTTOPLAYERINITTIME   60 // �U���J�n���Ԃ����������鎞��
#define ATTACKDELAYTIME						120 // �U���ł��Ȃ�����

class WeaponBase;
class Player;

class Enemy : public KdGameObject
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
		humanBossEnemy = allRounder | speedSter | defender | striker,
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
	void GenerateDepthMapFromLight_SkinMesh()	override;
	void DrawLit_SkinMesh()						override;
	void Init(std::weak_ptr<json11::Json> a_wpJsonObj);

	void Update()override;
	void PostUpdate()override;
	void DrawBright()override;
	void DrawSprite()override;
	void DrawDebug()override;

	void GenerateDepthMapFromLight()override;
	void SetTarget(std::shared_ptr<Player> a_target) { m_target = a_target; }
	void SetWeaponToTarget(std::shared_ptr<Player> a_spPlayer);
	void SetPos(Math::Vector3 a_pos) { m_pos = a_pos;}
	void SetMatrix();
	void SetWorldRotationY(float a_mWorldRotY) { m_mWorldRot.y = a_mWorldRotY; }
	void SetModelAndType(EnemyType a_enemyType); // ���̓G���G���G�i�l�^�j���G���G�i�����X�^�[�j���������@�{�X�̏ꍇ�͂���Ȃ�
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

	const Math::Vector3& GetAddCenterVal(){return m_addCenterVal;};

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

	std::map<std::string, json11::Json>  m_mpObj;																	// Json����Enemy�I�u�W�F�N�g�̏������炤�Ƃ��Ɏg�p
	std::weak_ptr<json11::Json> m_wpJsonObj;																		// Json�t�@�C��

	std::shared_ptr<KdModelWork> m_model;

	const int MAXWEAPONTYPE = 2;
	const int FIRSTWEAPONTYPENUMBER = 1;

	Math::Vector3 m_addCenterVal;
	Math::Vector3 m_addGrassDashEffectPosVal;

	float m_gravity = 0;
	int m_toleranceComboTime = 0; // �R���{���e����
	int m_m_toleranceAttackTime = 0; // �čU���Ԋu
	UINT m_EnemyState;
	UINT m_weaponType;
	UINT m_wantToMoveState;
	UINT m_wantToMoveCategory;
	UINT m_enemyType;
	UINT m_notHumanoidEnemyState;

	int m_rightWeaponNumber = 0;
	int m_leftWeaponNumber = 0;

	bool m_bMove = false;

	Math::Vector3    m_pos = {};
	Math::Vector3    m_move = {};
	Math::Vector3    m_scale = {};

	Math::Vector3	 m_mWorldRot;

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
	int   m_hitColorChangeTimeCnt = 0;
	float m_hitMoveSpd = 0.0f;
	float m_gardMoveSpd;           // �U����h�䂵�����̃m�b�N�o�b�N���x
	bool  m_bTough;                // �����U���͂̍U�������炤OR�K�[�h�������ǂ���
	Math::Vector3 m_knockBackVec;  // �m�b�N�o�b�N�������
	float m_vForce = 0.0f;
	float m_endurance = 0.0f;    // �g���I���̑ϋv��
	float m_graduallyTorionDecVal; // ���X�Ɍ����Ă��g���I����
	int m_delayTurnAroundTime; // �U��Ԃ��x�点�鎞��

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

	KdPolygon m_enduranceBarPoly;
	KdPolygon m_endurancePoly;

	bool m_bEnemyLose;
	bool m_bEnemyDeath;
	int m_overStageTime;

	bool          m_bEnemyBetweenPlayer;
	Math::Vector3 m_enemyBetweenPlayerHitPos;

	int m_enemyNumber;

	bool m_bBlowingAwayHitB; // ��납�琁����΂��ꂽ

	bool m_bBoss = false; // true �{�X�@false �G���G

	int m_coarseFishEnemyAttackDelayCnt;

	int m_idleCnt; // �Ȃɂ����ĂȎ��Ԃ��v��

	bool  m_bShotBeam;			// �ł����� : true
	bool  m_bShotEnergyBullet;	// �ł����� : true
	float m_beamRange;			// �r�[���̓����蔻��̒���
	Math::Vector3 m_beamCollisionPos = {}; // �G�l���M�[�e�������蔻�肷��ꏊ
	Math::Vector3 m_energyBulletDir = {}; // �G�l���M�[�e�����ł�����

	Math::Vector3 m_rangedAttackDir = {}; // �������U�������ł�������
	//Math::Vector3 m_rangedAttackDir = {}; // �������U�������ł�������

	int   m_rangedAttackAnimeCnt; // �������U���n�̃G�t�F�N�g�̃A�j���[�V�����J�E���g
	bool  m_bRangedAttack; // �������U�������Ă� true;
	Math::Vector3 m_rangedAttackTargetPos; // �������U���n�̑_���Ă���ꏊ
	Math::Vector3 m_rangedAttackShotPos; // �������U���n�U���𔭎˂����n�_


	bool  m_bBeamHitStart;	        // �����蔻�肵�Ă鎞 : true
	bool  m_bEnergyBulletHitStart;	// �����蔻�肵�Ă鎞 : true

	Math::Vector3 m_hitpos;

	int m_enemyAttackTotal;
	int m_enemyAttackMaxTotal;

	bool m_bAttackEnd; // �U�����I�������� : true

	bool  m_bRangedAttackCapableOfFiring; // �������U����łĂ邩 : true;

	int m_weaknesSsuccessionHitCnt;             // �A���Ŏ�_�ɓ���������
	int m_addWeaknesSsuccessionHitCntTime;      // ��_Hit���牽�b�o�߂������v��

	int m_addRotationAttackDistToPlayerTime;	    // ��]�U���͈͓���Player�����鎞�Ԃ��v��
	int m_rotationAttackDistToPlayerTimeInitTime;	// ��]�U���͈͓���Player�����Ȃ����Ԃ��v��
	float m_addRotationAttackDistToPlayerRadius;    // ��]�U���͈͂̔��a

	int m_attackDelayTime;

	bool m_bLethalDamageToKickOrPantciOrDashAttackHit; // �v���I�ȃL�b�N���p���`���_�b�V���A�^�b�N�������������@true
};