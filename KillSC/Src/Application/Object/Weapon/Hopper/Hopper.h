#pragma once
#include "../WeaponBase.h"

class Player;

class Hopper : public WeaponBase
{
public:
	Hopper() { Init(); }
	~Hopper(){}

	void Update()override;
	void DrawBright()override;
	void GenerateDepthMapFromLight()override;

	void StartAnime()override;

	void SetbPlayerWeapon() { m_bPlayerWeapon = true; }
	void SetbEnemyWeapon() { m_bEnemyWeapon = true; }
private:
	void Init()override;

	std::unique_ptr<KdModelWork> m_hopperModel;
	Math::Matrix m_hopperMat;
	std::shared_ptr<KdAnimator> m_animator;
	bool m_bAddanimeTime;

	bool m_bPlayerWeapon = false;
	bool m_bEnemyWeapon = false;

};