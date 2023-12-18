#pragma once
#define TORIONDOWNVALUE 1.0f   // V�t�H�[�X�����
#define  GRASSHOPPERPAUCNT 30  // �O���X�z�b�p�[���g���Ȃ�����

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
		rlAttackRush     = 1 << 31,
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
	void Init(std::weak_ptr<json11::Json> a_wpJsonObj);
	void Release() {};

	void SetCamera(std::shared_ptr<CameraBase> a_spCcamera) { m_wpCamera = a_spCcamera; }
	// void SetEnemy(std::shared_ptr<Enemy> a_enemy) { m_enemy = a_enemy; }
	void AddEnemy(std::shared_ptr<Enemy> a_spEnemy) { m_enemyList.push_back(a_spEnemy); }
	void AddWeaponToEnemy(std::shared_ptr<Enemy> a_spEnemy);
	void SetUi(std::shared_ptr<Ui> a_spUi) { m_wpUi = a_spUi; }

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
	const std::shared_ptr<KdModelWork>& GetModel() { return m_spModel; }
	const int GetInvincibilityTimeCnt() { return m_invincibilityTimeCnt; }
	const bool GetBRushAttackPossible() { return m_bRushAttackPossible; }
	void SetBRushAttackPossible(bool a_bRushAttackPossible) { m_bRushAttackPossible = a_bRushAttackPossible; }

	const int GetAnimationCnt() { return m_attackAnimeCnt; }
	std::vector<std::shared_ptr<WeaponBase>> GetWeaponList() { return m_weaponList; }

	void OnHit(Math::Vector3 a_KnocBackvec);
	void BlowingAwayAttackOnHit(Math::Vector3 a_KnocBackvec);
	void IaiKiriAttackOnHit(Math::Vector3 a_KnocBackvec);
	void CutRaiseOnHit(Math::Vector3 a_KnocBackvec);
	void HasDefense();

	const bool GetBPlayerDeath() { return m_bPlayerDeath; }
	const bool GetBPlayerLose() { return m_bPlayerLose; }

	const float GetAngleY() { return m_mWorldRot.y;}

	const bool GetBRushRp() {return m_bRushRp;}

private:

	void PlayerKickHitAttackChaeck();
	void PlayerPanchiHitAttackChaeck();
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

	std::map<std::string,json11::Json>  m_mpObj;
	std::shared_ptr<json11::Json> m_spJsonObj;

	std::shared_ptr<KdModelWork> m_spModel;

	const int MAXWEAPONTYPE = 2;
	const int FIRSTWEAPONTYPENUMBER = 1;

	float m_gravity = 0;
	int m_toleranceComboTime = 0; // �R���{���e����
	int m_m_toleranceAttackTime = 0; // �čU���Ԋu
	UINT m_playerState;
	UINT m_weaponType;

	int m_rightWeaponNumber = 0;
	int m_leftWeaponNumber = 0;

	bool m_bMove = false;
	bool m_bStateUnChange = false; // ��Ԃ�ύX�ł��Ȃ��Ƃ���true  �����������炱����Ӄr�b�g�Ǘ��ɂ��邩��
	bool m_bUnMove = false; // �����Ȃ��Ƃ���true
	bool m_bJump = false;

	bool m_bLeftWallHit = false;
	bool m_bRightWallHit = false;
	bool m_bMButtonState;

	Math::Vector3    m_pos = {};
	Math::Vector3    m_move = {};
	Math::Vector3    m_scale = {};
	Math::Vector3    m_addCenterVal = {}; //m_pos�ɑ�������Player�̒��S�ɂȂ�l

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
	int m_lGrassHopperPauCnt = 0; // �x�~���鎞��
	int m_rGrassHopperPauCnt = 0; // �x�~���鎞��

	int m_lGrassHopperTime = 0;
	int m_rGrassHopperTime = 0;
	Math::Matrix m_grasRotYMat;

	bool m_bLShiftKeyState = false;
	bool m_bFKeyState = false;
	Math::Vector3 m_grassHopperDashDir = {};
	float m_dashSpd = 0;

	std::shared_ptr<KdAnimator> m_spAnimator;

	std::vector<std::shared_ptr<WeaponBase>> m_weaponList;

	bool m_bFirstUpdate = false;
	bool m_bEwaponDrawPosible = false;


	std::weak_ptr<Enemy> m_wpEnemy;
	std::list<std::weak_ptr<Enemy>> m_enemyList;

	int m_attackAnimeCnt = 0;
	int m_runAnimeCnt = 0;
	int m_attackAnimeDelayCnt = 0;
	bool m_bAttackAnimeDelay = false;
	bool m_bAttackAnimeCnt = false;

	int   m_hitStopCnt = 0;
	int   m_hitColorChangeTimeCnt = 0;
	int   m_defenseKnockbackCnt = 0;
	float m_hitMoveSpd = 0.0f;
	float m_gardMoveSpd;           // �U����h�䂵�����̃m�b�N�o�b�N���x
	bool  m_bTough;                // �����U���͂̍U�������炤OR�K�[�h�������ǂ���
	Math::Vector3 m_knockBackVec;  // �m�b�N�o�b�N�������
	float m_torion    = 0.0f;    // �g���I���c��
	float m_endurance = 0.0f;    // �g���I���̑ϋv��
	float m_graduallyTorionDecVal; // ���X�Ɍ����Ă��g���I����
	int m_delayTurnAroundTime; // �U��Ԃ��x�点�鎞��

	int m_stepCnt;
	Math::Vector3 m_stepDashDir;

	Math::Vector3 m_attackMoveDir; // �U���������Ɉړ��������
	float         m_attackMoveSpd; // �U���������Ɉړ��������

	int m_invincibilityTimeCnt;  // ���G����

	bool m_bRushAttackPossible = false;
	std::weak_ptr<Ui> m_wpUi;
	int m_tuGardTime;

	int m_overStageTime;

	bool m_bPlayerDeath;
	bool m_bPlayerLose;

	KdSquarePolygon m_rocKOnPolygon;

	Math::Matrix m_rockOnPolyMat;

	bool m_bRushRp;
	bool m_bBlowingAwayHitB;

	bool m_bAtttackMoveSpeedDec; // ���������� : true

	float m_minimumYPos; // Y���W�Œ�l(Game)
	float m_tutorialMinimumYPos; // Y���W�Œ�l(Tutolial)
	
	float m_minimumXPos; // X���W�Œ�l
	float m_hightXPos;   // X���W�ō��l

	float m_minimumZPos; // Z���W�Œ�l
	float m_hightZPos;   // Z���W�ō��l

	int   m_maxOverStageTime; // �X�e�[�W�O�ɏo�Ă�����ő厞��
	float m_cutRaiseHitGravityAcceleration; // �؂�グ����Ă���Ƃ��̏d�͉����x
	float m_gravityAcceleration;            //�ʏ펞�̏d�͉����x
	float m_moveSpeedDecelerationamount;    // �ʏ펞�̓������x�̌����x
	float m_moveSpeedStopsAbruptly;         // �������x���}���ɗ�����Ƃ��̌����x
	float m_rushAttackMoveSpeedDecelerationamount;         // ���b�V���U�����̓������x�̌����x



	int   m_footfallPointMoment[2];         // �������Ȃ�u�� 
	int   m_lastRunAnimationTime;           // ����A�j���[�V�������X�g�̎���
	int   m_attackPointMoment;              // �U������u�� 
	int   m_lastAttackAnimationMoment;      // �U���A�j���[�V�������I������u�� 
	int   m_rushLastAttackPointTime;        // ���b�V���U���̃��X�g�U���̎���
	int   m_rotationRushLastAttackPointTime;        // ��]���b�V���U���̃��X�g�U���̎���
	int   m_maxAttackAnimeDelayCnt;         // �U���̃A�j���[�V������x�点�鎞�̍ő�l
	int   m_attackOneOrTwoSoundMoment;      // �Ў�P�i�ڍU�������Q�i�ڍU�����̉�����������u��
	int   m_attackThreeSoundMoment;         // �Ў�R�i�ڍU�����̉�����������u��
	int   m_rlAttackOneSoundFirstMoment;    // �����i�ڍU�����̉�����������u�Ԉ�i�K��
	int   m_rlAttackOneSoundSecondMoment;   // �����i�ڍU�����̉�����������u�ԓ�i�K��
	int   m_rlAttackTwoSoundFirstMoment;    // �����i�ڍU�����̉�����������u�Ԉ�i�K��
	int   m_rlAttackTwoSoundSecondMoment;   // �����i�ڍU�����̉�����������u�ԓ�i�K��
	int   m_rlAttackThreeSoundFirstMoment;  // ����O�i�ڍU�����̉�����������u�Ԉ�i�K��
	int   m_rlAttackThreeSoundSecondMoment; // ����O�i�ڍU�����̉�����������u�ԓ�i�K��

	int   m_lastRLAttackAnimationdMoment;      // �����i�ڂƓ�i�ڍU���A�j���[�V�������I������u��
	int   m_lastRLAttackThreeAnimationMoment;  // ����O�i�ڍU���A�j���[�V�������I������u��


	int   m_rlAttackRushSoundMoment[7];         // ���b�V���U�����̉�����������u��
	int   m_rlAttackRotationRushSoundMoment[8]; // �����]���b�V���U�����̉�����������u��
};