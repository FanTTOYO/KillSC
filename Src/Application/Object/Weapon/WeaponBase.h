#pragma once

class Player;
class Enemy;

enum ArrmType
{
	rArrm,
	lArrm,
};

class WeaponBase : public KdGameObject
{
public:
	WeaponBase() {}
	~WeaponBase() {}

	void SetOwner(const std::shared_ptr<KdGameObject>& a_owner) { m_owner = a_owner; }
	void SetArrmType(ArrmType a_arrmType) { m_arrmType = a_arrmType; }


	void GrassHopper(Math::Vector3 a_pos, float a_yAng);
	void GrassHopper(Math::Vector3 a_pos) { m_pos = a_pos; }

	virtual void StartAnime(){}

	void SetTarget(std::shared_ptr<Enemy> a_enemy) { m_pTarget = a_enemy; }
	void SetTarget(std::shared_ptr<Player> a_player) { m_eTarget = a_player; }

	virtual void SetMantis(Math::Matrix a_mat, bool a_bMantis){}
	virtual void SetBMantis(bool a_bMantis){}
protected:

	//std::shared_ptr<KdGameObject> m_owner;
	std::weak_ptr<KdGameObject> m_owner;

	std::unique_ptr<KdModelWork> m_model;
	
	ArrmType m_arrmType;

	Math::Vector3 m_pos = {};
	float m_yAng = 0;

	std::weak_ptr<Enemy> m_pTarget; // Playerのターゲット
	std::weak_ptr<Player>  m_eTarget; // Enemyのターゲット
};