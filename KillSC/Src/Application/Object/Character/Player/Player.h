#pragma once
#define GRASS_HOPPER_PAUCNT 30  // �O���X�z�b�p�[���g���Ȃ�����

class CameraBase;
class WeaponBase;
class Enemy;
class Ui;

class Player : public KdGameObject
{
public:
	enum WeaponType
	{
		scorpion = 1 << 0,				// �E�Z������
		grassHopper = 1 << 1,				// �E�z�b�p�[����

		lGrassHopper = 1 << 5,				// ���Z������
		lScorpion = 1 << 4,				// ���z�b�p�[����
	};

	enum PlayerState
	{
		idle = 1 << 0,																							// �ʏ���
		jump = 1 << 1,																							// �W�����v
		fall = 1 << 2,																							// �~��
		run  = 1 << 3,																							// ����
		grassHopperDashF = 1 << 4,																				// �z�b�p�[�ł̍����ړ��i�O�j
		grassHopperDashB = 1 << 5,																				// �z�b�p�[�ł̍����ړ��i���j
		grassHopperDashR = 1 << 6,																				// �z�b�p�[�ł̍����ړ��i�E�j
		grassHopperDashL = 1 << 7,																				// �z�b�p�[�ł̍����ړ��i���j
		grassHopperDash = grassHopperDashF | grassHopperDashB | grassHopperDashR | grassHopperDashL,			// �z�b�p�[�ł̍����ړ�
		grassHopperDashUp = 1 << 8,																				// �z�b�p�[�ł̍����ړ��i��j
		lAttackOne = 1 << 9,																					// ���U��1�i��
		lAttackTwo = 1 << 10,																					// ���U��2�i��
		lAttackThree = 1 << 11,																					// ���U��3�i��
		lAttack = lAttackOne | lAttackTwo | lAttackThree,														// ���U��
		rAttackOne = 1 << 12,																					// �E�U��1�i��
		rAttackTwo = 1 << 13,																					// �E�U��2�i��
		rAttackThree = 1 << 14,																					// �E�U��3�i��
		rAttack = rAttackOne | rAttackTwo | rAttackThree,														// �E�U��
		defense = 1 << 15,																						// �h��
		mantis = 1 << 16,																						// �h��s�U��
		hasDefense = 1 << 17,																					// �U����h���ꂽ
		blowingAwayHit = 1 << 18,																				// ������΂��U�����󂯂�
		iaiKiriHit = 1 << 19,																					// �����؂�����ꂽ
		nomalHit = 1 << 20,																						// �ʏ�̍U�����󂯂�
		cutRaiseHit = 1 << 21,																					// �؂�グ�U�������ꂽ
		hit = nomalHit | iaiKiriHit | blowingAwayHit | cutRaiseHit,												// �U�����󂯂�
		stepF = 1 << 22,																						// �X�e�b�v�i�O�j
		stepR = 1 << 23,																						// �X�e�b�v�i�E�j
		stepL = 1 << 24,																						// �X�e�b�v�i���j
		stepB = 1 << 25,																						// �X�e�b�v�i���j
		step  = stepF | stepB | stepR | stepL,																	// �X�e�b�v
		blowingAwayRise  = 1 << 26,																				// ������΂��ꂽ���Ƃ̋N���オ��
		iaiKiriRise      = 1 << 27,																				// �����؂肳�ꂽ���Ƃ̋N���オ��
		rise             = iaiKiriRise | blowingAwayRise,														// �N���オ��
		rlAttackOne      = 1 << 28,																				// ����U��1�i��
		rlAttackTwo      = 1 << 29,																				// ����U��2�i��
		rlAttackThree    = 1 << 30,																				// ����U��3�i��
		rlAttack         = rlAttackOne | rlAttackTwo | rlAttackThree,											// ����U��
		rlAttackRush     = 1 << 31,																				// ���b�V��
	};

	Player() {}
	~Player() {}																								
																												
	void Update()								override;														
	void PostUpdate()							override;														
	void DrawLit()								override;														
	void DrawUnLit()							override;														
	void DrawBright()							override;																										
	void GenerateDepthMapFromLight()			override;														
	void GenerateDepthMapFromLight_SkinMesh()	override;														
	void DrawLit_SkinMesh()						override;
	void DrawDebug()override;
	void Init(std::weak_ptr<json11::Json> a_wpJsonObj);
	void Release() {};
																												
	void SetCamera(std::shared_ptr<CameraBase> a_spCcamera) { m_wpCamera = a_spCcamera; }						// �J�����N���X�̂����炤
	void AddEnemy(std::shared_ptr<Enemy> a_spEnemy) { m_enemyList.push_back(a_spEnemy); }						// �G�𑝂₷
	void AddWeaponToEnemy(std::shared_ptr<Enemy> a_spEnemy);													// ����ɖ��o�^�̓G�̏��������
	void SetUi(std::shared_ptr<Ui> a_spUi) { m_wpUi = a_spUi; }													// UI�N���X�����炤
																												
	const UINT& GetPlayerState() { return m_playerState; }														// �v���C���[�̏�Ԃ�n��
	const UINT& GetWeaponType() { return m_weaponType; }														// ���������Ă��鑕���̏���n��
																												
	const int& GetLGrassHopperTime() { return m_lGrassHopperTime; }												// ���̃z�b�p�[�̎g�p���Ԃ�n��
	const int& GetRGrassHopperTime() { return m_rGrassHopperTime; }												// �E�̃z�b�p�[�̎g�p���Ԃ�n��
																												
	const int& GetLGrassHopperPauCnt() { return m_lGrassHopperPauCnt; }											// ���̃z�b�p�[�̎g�p�s���Ԃ�n��
	const int& GetRGrassHopperPauCnt() { return m_rGrassHopperPauCnt; }											// �E�̃z�b�p�[�̎g�p�s���Ԃ�n��
																												 
																												 
	const float& GetGravity() { return m_gravity; }																// �������Ă�d�͂�n��
	const float& GetDashSpd() { return m_dashSpd;}																// �����ړ��̑�����n��
	const float& GetVForce()  { return m_vForce; }																// ���݂̃G�l���M�[�ʂ�n��
	const float& GetEndurance()  { return m_endurance; }														// �퓬�̂̑ϋv�͂�n��
	const std::shared_ptr<KdModelWork>& GetModel() { return m_spModel; }										// ���f���̏���n��
	const int GetInvincibilityTimeCnt() { return m_invincibilityTimeCnt; }										// ���݂̖��G���Ԃ�n��
	const bool GetBRushAttackPossible() { return m_bRushAttackPossible; }										// ���b�V���A�^�b�N���\���ǂ�����n��
	void SetBRushAttackPossible(bool a_bRushAttackPossible) { m_bRushAttackPossible = a_bRushAttackPossible; }	// ���b�V���A�^�b�N���\���ǂ������n�����
																												
	const int GetAnimationCnt() { return m_attackAnimeCnt; }													// �U���̃A�j���[�V�����t���[�����n��
	std::vector<std::shared_ptr<WeaponBase>> GetWeaponList() { return m_weaponList; }							// ����̃��X�g��n��
																												
	void OnHit(Math::Vector3 a_KnocBackvec);																	// �ʏ�U������������
	void BlowingAwayAttackOnHit(Math::Vector3 a_KnocBackvec);													// ������΂��U������������
	void IaiKiriAttackOnHit(Math::Vector3 a_KnocBackvec);														// �����؂�n���̍U������������
	void CutRaiseOnHit(Math::Vector3 a_KnocBackvec);															// �؂�グ�U������������
	void HasDefense();																							// �U�����h�䂳�ꂽ
																												
	const bool GetBPlayerDeath() { return m_bPlayerDeath; }														// �v���C���[��Death�������ǂ�����n��
	const bool GetBPlayerLose() { return m_bPlayerLose; }														// �v���C���[�����������ǂ�����n��
																												
	const float GetAngleY() { return m_mWorldRot.y;}															// �v���C���[�̌����Ă�p�x��n��
																												 
	const bool GetBRushRp() {return m_bRushRp;}																	// ��]���b�V�����ǂ�����n��

	const Math::Vector3& GetAddCenterVal() { return m_addCenterVal; };											// ������character�̐^�񒆂ɂȂ�l 
																												 
private:

	void PlayerKickHitAttackChaeck();																				// �v���C���[�̃L�b�N�����蔻��
	void PlayerPanchiHitAttackChaeck();																				// �v���C���[�̃p���`�����蔻��
	void UpdateRotate(Math::Vector3& a_srcMoveVec);																	// �v���C���[�̉�]���X�V
	void GrassMoveVecDecision();																					// �z�b�p�[�Ői�ޕ��������߂�
	void ScorpionActionDecision();																					// �Z���ŉ������邩�����߂�
	void GrassMove();																								// �����ړ����̓���������
	void StepMove();																								// �X�e�b�v���̓���������																						
	void NormalMove();																								// �����Ȃǂ̓���������
	void ScorpionAttackMove();																						// �Z���ł̍U��������
	void ScorpionDefenseMove();																						// �Z���ł̖h�������
	void HasDefenseMove();																							// �h�䂳�ꂽ���̓���������
	void TutorialUpdate();																							// �`���[�g���A�����̂ݍs������
	void OverStageChaeck();																							// �X�e�[�W����o�����ǂ������m�F
	void EnemyRockOn();																								// �G�����b�N�I�����鏈��
	void HitStateUpdate();																							// Hit��Ԃ̎��̏���
	void CollisionUpdate();																							// �����蔻�菈��

	std::map<std::string,json11::Json>  m_mpObj;																	// Json����Player�I�u�W�F�N�g�̏������炤�Ƃ��Ɏg�p
	std::weak_ptr<json11::Json> m_wpJsonObj;																		// Player�I�u�W�F�N�g�ȊO�̏������炤�Ƃ��Ɏg�p
																													
	std::shared_ptr<KdModelWork> m_spModel;																			// ���f��
																													
	const int MAXWEAPONTYPE = 2;																					// ����̍ő�̃i���o�[
	const int FIRSTWEAPONTYPENUMBER = 1;																			// ����̍ŏ��̃i���o�[

	Math::Vector3 m_addCenterVal;																					// �������璆�S�ɂȂ�l
	Math::Vector3 m_addGrassDashEffectPosVal;																		// �v���C���[�̍��W�ɑ����ƃG�t�F�N�g���o���ʒu�ɂȂ�

	float m_gravity = 0;																							// �d��
	int m_toleranceComboTime = 0;																					// �R���{���e����
	int m_m_toleranceAttackTime = 0;																				// �čU���Ԋu
	UINT m_playerState;																								// �v���C���[�̏��
	UINT m_weaponType;																								// �����ݑ������Ă��镐��

	int m_rightWeaponNumber = 0;                                                                                    // �����݉E�ɑ������Ă��镐��i���o�[
	int m_leftWeaponNumber = 0;																						// �����ݍ��ɑ������Ă��镐��i���o�[

	bool m_bMove = false;																							// �����Ă� true

	bool m_bMButtonState;																							// �z�C�[�����N���b�N���Ă� true

	Math::Vector3    m_pos = {};																					// ���W
	Math::Vector3    m_move = {};																					// ��������
	Math::Vector3    m_scale = {};																					// �f�J��

	std::weak_ptr<CameraBase>        m_wpCamera;
	Math::Vector3					 m_mWorldRot;

	int m_lGrassHopperPauCnt = 0;																					// �x�~���鎞��
	int m_rGrassHopperPauCnt = 0;																					// �x�~���鎞��

	int m_lGrassHopperTime = 0;																						// ���z�b�p�[�ł̍����ړ�����
	int m_rGrassHopperTime = 0;																						// �E�z�b�p�[�ł̍����ړ�����
	Math::Matrix m_grasRotYMat;																						// �����ړ��������

	Math::Vector3 m_grassHopperDashDir = {};                                                                        // �����ړ���������x�N�g��
	float m_dashSpd = 0;                                                                                            // �����ړ��̑���

	std::shared_ptr<KdAnimator> m_spAnimator;																		// �A�j���[�V����������ׂ̕ϐ�

	std::vector<std::shared_ptr<WeaponBase>> m_weaponList;															// ���탊�X�g

	bool m_bFirstUpdate = false;																					// ���߂Ă̍X�V
	bool m_bEwaponDrawPosible = false;                                                                              // �����`��ł���@true


	std::weak_ptr<Enemy> m_wpEnemy;																					// ���b�N�I�����Ă�G
	std::list<std::weak_ptr<Enemy>> m_enemyList;																	// �G�̃��X�g

	int m_attackAnimeCnt = 0;																						// �U���A�j���[�V�����̃t���[����
	int m_runAnimeCnt = 0;																							// ����A�j���[�V�����̃t���[����
	int m_attackAnimeDelayCnt = 0;																					// �U���A�j���[�V������x�点�鎞��
	bool m_bAttackAnimeDelay = false;																				// �U���A�j���[�V������x��Ă� true
	bool m_bAttackAnimeCnt = false;																					// �U���A�j���[�V�����̃t���[���������Z���Ă������@true

	int   m_hitStopCnt = 0;																							// �U�����󂯂ĂƂȂ鎞��
	int   m_hitColorChangeTimeCnt = 0;																				// �U�����󂯐Ԃ��Ȃ鎞��
	float m_hitMoveSpd = 0.0f;																						// �m�b�N�o�b�N���鑬��
	float m_gardMoveSpd;																							// �U����h�䂵�����̃m�b�N�o�b�N���x
	bool  m_bTough;																									// �����U���͂̍U�������炤OR�K�[�h�������ǂ���
	Math::Vector3 m_knockBackVec;																					// �m�b�N�o�b�N�������
	float m_vForce = 0.0f;																							// V�t�H�[�X�c��
	float m_endurance = 0.0f;																						// V�t�H�[�X�̑ϋv��
	float m_vForceDownValue = 0.0f;																					// V�t�H�[�X�����
	float m_graduallyTorionDecVal;																					// ���X�Ɍ����Ă�V�t�H�[�X��
	int m_delayTurnAroundTime;																						// �U��Ԃ��x�点�鎞��

	int m_stepCnt;																									// �X�e�b�v�����鎞��															
	Math::Vector3 m_stepDashDir;																					// �X�e�b�v��������x�N�g��

	Math::Vector3 m_attackMoveDir;																					// �U���������Ɉړ��������
	float         m_attackMoveSpd;																					// �U���������Ɉړ��������

	int m_invincibilityTimeCnt;																						// ���G����

	bool m_bRushAttackPossible = false;																				// ���b�V���A�^�b�N�ł��� true													
	std::weak_ptr<Ui> m_wpUi;																						// UI�N���X�̏��������Ă�
	int m_tuGardTime;																								// �`���[�g���A���̍ۂ̖h�䂵�Ă鎞�Ԃ𐔂��Ă�

	int m_overStageTime;																							// �X�e�[�W�O�ɏo������

	bool m_bPlayerDeath;																							// �v���C���[�����ꂽ true
	bool m_bPlayerLose;																								// �v���C���[�������� true

	KdSquarePolygon m_rocKOnPolygon;																				// ���b�N�I���p�|���S��

	Math::Matrix m_rockOnPolyMat;																					// ���b�N�I���p�s��

	bool m_bRushRp;																									// ��]���b�V���U�� true	
	bool m_bBlowingAwayHitB;																						// ��납�琁����΂��U�����󂯂� true1

	bool m_bAtttackMoveSpeedDec;																					// ���������� : true
};