#pragma once
#include "../CameraBase.h"
#define CAMERAZ -3.5f 
#define CAMERAY  2.0f

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

private:
	void Init()					override;

	void UpdateRotateByMouse();
	void UpdateRotateByEnemy();

	void CameraSetUpdate();

	//void SetBCameraSet(Math::Vector3 _mobeVec)override;

	POINT m_FixMousePos;

	bool m_bCameraSet;
	Math::Vector3 m_cameraSetVec;
	std::weak_ptr<Player> m_wpPlayer;
	std::weak_ptr<Enemy> m_wpEnemy;

	float m_cameracChasePower = 0;
	bool m_bCameraDown;
	bool m_bRotateEnemy; // エネミーによってカメラの回転がかわる
};