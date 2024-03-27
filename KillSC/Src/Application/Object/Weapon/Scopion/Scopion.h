#pragma once
#include "../WeaponBase.h"

class Player;

class Scopion : public WeaponBase
{
public:
	Scopion() { Init(); }
	~Scopion() {}

	void Update()override;
	void PostUpdate()override;
	void DrawLit()override;
	void GenerateDepthMapFromLight()override;

	void SetbPlayerWeapon() { m_bPlayerWeapon = true; }
	void SetbEnemyWeapon() { m_bEnemyWeapon = true; }

	void SetMantis(Math::Matrix a_mat, bool a_bMantis);
	void SetBMantis(bool a_bMantis) { m_bMantis = a_bMantis; }

private:
	void Init()override;
	void PlayerHitAttackChaeck();
	void PlayerManAttackChaeck();
	void PlayerAttackHit(std::shared_ptr<Player> a_player, std::shared_ptr<Enemy> a_enemy, Math::Vector3 a_hitPos);
	Math::Matrix PlayerHitEffectMat(Math::Vector3 a_hitPos, std::weak_ptr<Enemy> a_enemy);


	void EnemyHitAttackChaeck();
	void EnemyManAttackChaeck();
	void EnemyAttackHit(std::shared_ptr<Enemy> a_enemy, Math::Vector3 a_hitPos);
	Math::Matrix EnemyHitEffectMat(Math::Vector3 a_hitPos, std::weak_ptr<Player> a_player);

	bool m_bPlayerWeapon = false;
	bool m_bEnemyWeapon = false;
	bool m_bMantis = false;

	Math::Matrix m_mantisRot = Math::Matrix::Identity;
	std::shared_ptr<KdModelWork> m_model;
	std::unique_ptr<KdModelWork> m_manModel;
	std::shared_ptr<KdAnimator> m_animator;
};