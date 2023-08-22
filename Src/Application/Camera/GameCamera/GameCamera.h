#pragma once
#include "../CameraBase.h"
class Player;

class GameCamera : public CameraBase
{
public:
	GameCamera() { Init(); }
	virtual ~GameCamera()	override {}

	void Update()				override;

	void SetPlayer(std::shared_ptr<Player> a_player) { m_wpPlayer = a_player; }

private:
	void Init()					override;

	void UpdateRotateByMouse();

	void CameraSetUpdate();

	//void SetBCameraSet(Math::Vector3 _mobeVec)override;

	POINT m_FixMousePos;

	bool m_bCameraSet;
	Math::Vector3 m_cameraSetVec;
	std::weak_ptr<Player> m_wpPlayer;

	float m_cameracChasePower = 0;
	bool m_bCameraDown;
};