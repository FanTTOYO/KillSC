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
		idle = 1 << 0,																							// �ʏ���
		jump = 1 << 1,																							// �W�����v
		fall = 1 << 2,																							// �~��
		run = 1 << 3,																							// ����
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
		step = stepF | stepR | stepL | stepB,																	// �X�e�b�v
		blowingAwayRise = 1 << 26,																				// ������΂��ꂽ���Ƃ̋N���オ��
		iaiKiriRise = 1 << 27,																					// �����؂肳�ꂽ���Ƃ̋N���オ��
		rise = iaiKiriRise | blowingAwayRise,																	// �N���オ��
		rlAttackOne = 1 << 28,																					// ����U��1�i��
		rlAttackTwo = 1 << 29,																					// ����U��2�i��
		rlAttackThree = 1 << 30,																				// ����U��3�i��
		rlAttack = rlAttackOne | rlAttackTwo | rlAttackThree,													// ����U��
		rlAttackRush = 1 << 31,																					// ���b�V��
	};

	// �`��֘A
	void DrawLit()								override;
	void GenerateDepthMapFromLight_SkinMesh()	override;
	void GenerateDepthMapFromLight()			override;
	void DrawBright()							override;
	void DrawDebug()							override;

	// �U��Hit���ɌĂяo��
	virtual void OnHit(Math::Vector3 a_KnocBackvec,CharacterBase* a_pAttackedCharacter) = 0;																	// �ʏ�U������������
	virtual void BlowingAwayAttackOnHit(Math::Vector3 a_KnocBackvec, CharacterBase* a_pAttackedCharacter) = 0;													// ������΂��U������������
	virtual void IaiKiriAttackOnHit(Math::Vector3 a_KnocBackvec) = 0;														// �����؂�n���̍U������������
	virtual void CutRaiseOnHit(Math::Vector3 a_KnocBackvec) = 0;															// �؂�グ�U������������
	virtual void HasDefense() {};																							// �U�����h�䂳�ꂽ					


	// �Z�b�^�[
	void SetPos(Math::Vector3 a_pos) { m_pos = a_pos; }
	void SetAttackHit(bool a_attackHit) { m_attackHit = a_attackHit; }
	void SetDefenseSuc(bool a_defenseSuc) { m_bDefenseSuc = a_defenseSuc; }
	void SetBRushAttackPossible(bool a_bRushAttackPossible) { m_bRushAttackPossible = a_bRushAttackPossible; }
	void SetWorldRotationY(float a_mWorldRotY) { m_mWorldRot.y = a_mWorldRotY; }

	// �Q�b�^�[
	// ��{����Ԃ�
	const UINT& GetCharaState() { return m_charaState; }
	const float GetVForce() { return m_vForce; }
	const float GetEndurance() { return m_endurance; }
	const float GetDamageAmount() { return m_damageAmount; }
	const float GetGravity() { return m_gravity; }
	const std::shared_ptr<KdModelWork>& GetModel() { return m_spModel; }
	const Math::Vector3& GetAddCenterVal() { return m_addCenterVal; }

	// �p�x��Ԃ�
	const float GetAngleY() { return m_mWorldRot.y; }

	// GrassDash�֌W�̏���Ԃ�
	const int GetLGrassHopperTime() { return m_lGrassHopperTime; }
	const int GetRGrassHopperTime() { return m_rGrassHopperTime; }
	const int GetLGrassHopperPauCnt() { return m_lGrassHopperPauCnt; }
	const int GetRGrassHopperPauCnt() { return m_rGrassHopperPauCnt; }
	const float GetDashSpd() { return m_dashSpd; }

	// ���S�E���������ǂ�����Ԃ�
	const bool GetBDeath() { return m_bDeath; }
	const bool GetBLose() { return m_bLose; }

	// �U���E�_���[�W�֌W��Ԃ�
	const int  GetAnimationCnt() { return m_attackAnimeCnt; }
	const bool GetBRushAttackPossible() { return m_bRushAttackPossible; }
	const bool GetAttackHit() { return m_attackHit; }
	const int  GetInvincibilityTimeCnt() { return m_invincibilityTimeCnt; }
	const bool GetDefenseSuc() { return m_bDefenseSuc; }

	const UINT& GetNotHumanoidEnemyState() { return m_notHumanoidEnemyState; }

	// ����֌W
	std::vector<std::shared_ptr<WeaponBase>> GetWeaponList() { return m_weaponList; }
	const UINT& GetWeaponType() { return m_weaponType; }

protected:

	struct HitInfo
	{
		bool hit = false;
		float maxOverLap = 0;
		Math::Vector3 hitDir = {}; // ������������
		Math::Vector3 groundPos = {};
		void Init() 
		{
			hit = false;
			maxOverLap = 0;
			hitDir = {}; // ������������
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

	void KickHitAttackChaeck(std::shared_ptr<CharacterBase> a_target, CharacterBase* a_pAttakedCharacter);											// �L�b�N�̓����蔻��
	void OverStageChaeck();																						// �X�e�[�W����o�����ǂ������m�F
	void SetGrassDashInfo(Math::Vector3 a_dashDir, CharaState a_charaState, std::shared_ptr<KdAnimationData> a_animationInfo, float a_hopperBaseAng,
		             int a_weaponNumber, Math::Vector3 a_vec, float a_grassDashAng, float a_grassDashAddAng = 0.0f, std::string a_legPoint = "GrassHopperLegRPoint", std::string a_effectName = "GrassDashBlurPlayer.efk"); // �O���X�_�b�V���ɕK�v�ȏ����Z�b�g����
	bool SetStepDashInfo(Math::Vector3 a_dashDir, CharaState a_charaState, std::shared_ptr<KdAnimationData> a_animationInfo); // �X�e�b�v�ɕK�v�ȏ����Z�b�g����
	void SetHitMoveInfo(CharaState a_charaState, Math::Vector3 a_knockBackVec,float a_hitMoveSpd,float a_damgage,int a_hitStopCnt,int a_UpdateStopCnt,std::shared_ptr<KdAnimationData> a_animationInfo,float a_firstGraduallyTorionDecVal,
		                float a_graduallyTorionDecVal,int a_invincibilityTimeCnt = 0);
	// �������蔻��
	HitInfo SphereColisionCheck(Math::Vector3 a_center, float a_radius, KdCollider::Type a_type, std::shared_ptr<WeaponBase> a_weapon);
	HitInfo SphereColisionCheck(Math::Vector3 a_center, float a_radius, KdCollider::Type a_type, std::shared_ptr<Enemy> a_enemy);
	HitInfo SphereColisionCheck(Math::Vector3 a_center, float a_radius, KdCollider::Type a_type, std::shared_ptr<Player> a_player);
	HitInfo SphereColisionCheck(Math::Vector3 a_center, float a_radius, KdCollider::Type a_type, std::shared_ptr<KdGameObject> a_obj);
	HitInfo SphereColisionCheck(Math::Vector3 a_center, float a_radius, KdCollider::Type a_type);

	// ���C�����蔻��
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
	std::map<std::string, json11::Json>  m_mpObj;																// Json����L�������Ƃ̏������炤�Ƃ��Ɏg�p
	std::weak_ptr<json11::Json> m_wpJsonObj;																	// ���ʏ������炤�Ƃ��Ɏg�p

	// ��{���
	UINT             m_charaState;
	Math::Vector3    m_pos = {};																				// ���W
	std::shared_ptr<KdModelWork> m_spModel = nullptr;															// ���f��
	std::shared_ptr<KdAnimator> m_spAnimator;																	// �A�j���[�V����������ׂ̕ϐ�
	float m_vForce = 0.0f;																						// V�t�H�[�X�c��
	float m_endurance = 0.0f;																					// V�t�H�[�X�̑ϋv��
	float m_damageAmount = 0.0f;																				// �_���[�W��
	float m_vForceDownValue = 0.0f;																				// �z�b�p�[�g�p����V�t�H�[�X�����
	float m_graduallyTorionDecVal = 0.0f;																		// ���X�Ɍ����Ă�V�t�H�[�X��
	float m_gravity = 0.0f;																						// �d��
	Math::Vector3 m_mWorldRot = {};																				// �p�x
	int m_invincibilityTimeCnt = 0;																				// ���G����
	Math::Vector3 m_addCenterVal = {};																			// �������璆�S�ɂȂ�l
	bool m_bMove = false;																						// �����Ă� true

	// ����֘A
	std::vector<std::shared_ptr<WeaponBase>> m_weaponList;														// ���탊�X�g
	UINT m_weaponType = 0;																						// �����ݑ������Ă��镐��
	int m_rightWeaponNumber = 0;                                                                                // �����݉E�ɑ������Ă��镐��i���o�[
	int m_leftWeaponNumber = 0;																					// �����ݍ��ɑ������Ă��镐��i���o�[

	// Hoperr���g���Ẵ_�b�V���֘A
	Math::Vector3 m_grassHopperDashDir = {};                                                                    // �����ړ���������x�N�g��
	Math::Vector3 m_addGrassDashEffectPosVal = {};																// �v���C���[�̍��W�ɑ����ƃG�t�F�N�g���o���ʒu�ɂȂ�
	int m_lGrassHopperPauCnt = 0;																				// �x�~���鎞��
	int m_rGrassHopperPauCnt = 0;																				// �x�~���鎞��
	int m_lGrassHopperTime = 0;																					// ���z�b�p�[�ł̍����ړ�����
	int m_rGrassHopperTime = 0;																					// �E�z�b�p�[�ł̍����ړ�����
	float m_dashSpd = 0;                                                                                        // �����ړ��̑���

	// Scopion�֘A
	Math::Vector3 m_attackMoveDir;																				// �U���������Ɉړ��������
	float m_attackMoveSpd;																						// �U���������Ɉړ��������
	int m_attackAnimeCnt = 0;																					// �U���A�j���[�V�����̃t���[����
	int m_attackAnimeDelayCnt = 0;																				// �U���A�j���[�V������x�点�鎞��
	bool m_bAttackAnimeDelay = false;																			// �U���A�j���[�V������x�点�� true
	int m_toleranceComboTime = 0;																				// �R���{���e����
	bool m_bRushAttackPossible = false;																			// ���b�V���A�^�b�N�ł��� true		


	// ����A�j���[�V�����֘A
	int m_runAnimeCnt = 0;																						// ����A�j���[�V�����̃t���[����

	// Hit�֘A
	int   m_hitStopCnt = 0;																						// �U�����󂯂ĂƂ܂鎞��
	int   m_hitColorChangeTimeCnt = 0;																			// �U�����󂯐Ԃ��Ȃ鎞��
	float m_hitMoveSpd = 0.0f;																					// �U�����󂯂��Ƃ��̐�����΂��X�s�[�h
	Math::Vector3 m_knockBackVec;																				// �m�b�N�o�b�N�������
	bool m_bBlowingAwayHitB;																					// ��납�琁����΂��U�����󂯂� true1
	int m_notUnderAttackTime;																					// �U�����󂯂Ă��Ȃ�����

	// Step�֘A
	int m_stepCnt;																								// �X�e�b�v�����鎞��															
	Math::Vector3 m_stepDashDir;																				// �X�e�b�v��������x�N�g��

	// ���̑�
	bool m_bFirstUpdate = false;
	int m_overStageTime = 0;																					// �X�e�[�W�O�ɏo������
	bool m_bDeath = false;																						// ���ꂽ true
	bool m_bLose = false;																						// ������ true
	bool m_attackHit = false;																					// ��̍U���������񓖂���Ȃ����߂̃t���O
	bool m_bDefenseSuc = false;																					// �h��ɐ��������t���O
	bool m_bWaponDrawPosible = false;                                                                           // �����`��ł���@true
	float m_moveSpeedDecelerationamount = 0.0f;																	// �������Ɏg�p

	UINT m_notHumanoidEnemyState;

private:
		HitInfo SphereColisionCheck(Math::Vector3 a_center, float a_radius, KdCollider::Type a_type, std::shared_ptr<WeaponBase> a_weapon, std::shared_ptr<Enemy> a_enemy, std::shared_ptr<Player> a_player, std::shared_ptr<KdGameObject> a_obj);
		HitInfo RayColisionCheck(Math::Vector3 a_pos, Math::Vector3 a_dir, float a_range, KdCollider::Type a_type, std::shared_ptr<WeaponBase> a_weapon, std::shared_ptr<Enemy> a_enemy, std::shared_ptr<Player> a_player, std::shared_ptr<KdGameObject> a_obj);
};