#pragma once
#include "../CameraBase.h"
#define CAMERAZ -3.65f 
#define CAMERAY  2.65f

class Player;
class Enemy;

class GameCamera : public CameraBase
{
public:
	GameCamera() { Init(); }
	virtual ~GameCamera()	override {}

	void Update()				override;

	void SetPlayer(std::shared_ptr<Player> a_player) { m_wpPlayer = a_player; }
	void SetEnemy(std::shared_ptr<Enemy> a_enemy);
	void SetBRotateEnemy(bool a_bRotateEnemy) { m_bRotateEnemy = a_bRotateEnemy; }
	bool GetBRotateEnemy() { return m_bRotateEnemy; }
	void SetHitObj(std::shared_ptr<KdGameObject> a_hitObj) { m_hitObjList.push_back(a_hitObj); }

	const std::weak_ptr<Enemy> GetEnemy() {return m_wpEnemy;}

private:
	void Init()					override;

	void UpdateRotateByMouse();
	void UpdateRotateByEnemy();

	void CameraSetUpdate();
	void UpdateCollision();

	//void SetBCameraSet(Math::Vector3 _mobeVec)override;

	POINT m_FixMousePos;

	bool m_bCameraSet;
	Math::Vector3 m_cameraSetVec;
	std::weak_ptr<Player> m_wpPlayer;
	std::weak_ptr<Enemy> m_wpEnemy;

	float m_cameracChasePower = 0;
	bool m_bCameraDown;
	bool m_bRotateEnemy; // エネミーによってカメラの回転がかわる
	Math::Vector3 m_startPos[5] = {};
	bool m_startDelayFive = false; // 行列を5個使う true その他 false
	std::list<std::weak_ptr<KdGameObject>> m_hitObjList; //	当たり判定するオブジェクト
};