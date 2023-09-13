#include "GameCamera.h"
#include "../../Object/Character/Player/Player.h"
#include "../../Object/Character/Enemy/Enemy.h"

void GameCamera::Init()
{
	// 注視点
	m_LocalPos = Math::Matrix::CreateTranslation(0, CAMERAY, CAMERAZ);

	// ↓画面中央
	m_FixMousePos.x = 640;
	m_FixMousePos.y = 360;

	m_bRotateEnemy = false;
	SetCursorPos(m_FixMousePos.x, m_FixMousePos.y);
	m_cameracChasePower = 0.0f;
	CameraBase::Init();
}

void GameCamera::Update()
{
	Math::Matrix targetMat = Math::Matrix::Identity;
	if (!m_wpPlayer.expired())
	{
		if (!m_wpTarget.expired())
		{
			const std::shared_ptr<const KdGameObject> spTarget = m_wpTarget.lock();
			if (spTarget)
			{
				// Player基準
				if (m_stepOnPlayerPos.y + 0.2f < spTarget->GetPos().y)
				{
					if (m_wpPlayer.lock()->GetGravity() > 0.0f)
					{
						m_cameracChasePower -= (1.0f / 60.0f);
						if (m_cameracChasePower <= 0.0f)
						{
							m_cameracChasePower = 0.0f;
						}
					}
					else
					{
						m_cameracChasePower += (1.0f / 60.0f);
						if (m_cameracChasePower >= 1.0f)
						{
							m_cameracChasePower = 1.0f;
						}
					}
				}
				else
				{
					m_cameracChasePower = 0;
				}
				targetMat = Math::Matrix::CreateTranslation(spTarget->GetPos().x, spTarget->GetPos().y, spTarget->GetPos().z);
				

				//// 地面基準 未完
				//if (spTarget->GetPos().y < m_stepOnPlayerPos.y + 5 && spTarget->GetPos().y > m_stepOnPlayerPos.y - 5 && !m_bCameraDown)
				//{
				//	m_cameracChasePower = 0;
				//	targetMat = Math::Matrix::CreateTranslation(spTarget->GetPos().x, m_stepOnPlayerPos.y, spTarget->GetPos().z);
				//}
				//else if (spTarget->GetPos().y >= m_stepOnPlayerPos.y + 5 && spTarget->GetPos().y > m_stepOnPlayerPos.y + m_cameracChasePower)
				//{
				//	float b = spTarget->GetPos().y - m_stepOnPlayerPos.y + m_cameracChasePower;
				//	if (b >= 2.0f)
				//	{
				//		m_cameracChasePower += m_wpPlayer.lock()->GetDashSpd() * 1.10f;
				//	}

				//	targetMat = Math::Matrix::CreateTranslation(spTarget->GetPos().x, m_stepOnPlayerPos.y + m_cameracChasePower, spTarget->GetPos().z);
				//}
				//else if(m_wpPlayer.lock()->GetGravity() < 0)
				//{
				//	m_cameracChasePower = m_wpPlayer.lock()->GetGravity() * 0.95f;
				//	targetMat = Math::Matrix::CreateTranslation(spTarget->GetPos().x, m_stepOnPlayerPos.y + m_cameracChasePower, spTarget->GetPos().z);
				//}
				//else
				//{
				//	float b = 0.0f;
				//	b = spTarget->GetPos().y - m_stepOnPlayerPos.y;
				//	m_bCameraDown = true;
				//	if (m_cameracChasePower <= 0.0f)
				//	{
				//		m_bCameraDown = false;
				//	}

				//	if (m_wpPlayer.lock()->GetGravity() > 0)
				//	{
				//		m_cameracChasePower -= m_wpPlayer.lock()->GetGravity();
				//	}

				//	targetMat = Math::Matrix::CreateTranslation(spTarget->GetPos().x, m_stepOnPlayerPos.y + m_cameracChasePower, spTarget->GetPos().z);
				//}

				//if (m_cameracChasePower > 0.0f && m_wpPlayer.lock()->GetGravity() == 0 && !(m_wpPlayer.lock()->GetPlayerState() & (Player::PlayerState::grassHopperDash | Player::PlayerState::grassHopperDashUp)))
				//{
				//	m_cameracChasePower -= 0.15f;
				//	m_bCameraDown = true;
				//	if (m_cameracChasePower <= 0.0f)
				//	{
				//		m_bCameraDown = false;
				//	}
				//	targetMat = Math::Matrix::CreateTranslation(spTarget->GetPos().x, m_stepOnPlayerPos.y + m_cameracChasePower, spTarget->GetPos().z);
				//}

				//if (spTarget->GetPos().y <= m_stepOnPlayerPos.y)
				//{
				//	if (m_wpPlayer.lock()->GetGravity() > 0)
				//	{
				//		m_cameracChasePower -= m_wpPlayer.lock()->GetGravity();
				//	}
				//	else
				//	{
				//		m_cameracChasePower = 0;
				//	}

				//	targetMat = Math::Matrix::CreateTranslation(spTarget->GetPos().x, m_stepOnPlayerPos.y + m_cameracChasePower, spTarget->GetPos().z);
				//}
			}
		}



		if (m_wpPlayer.lock()->GetLGrassHopperTime() <= 80 && m_wpPlayer.lock()->GetLGrassHopperTime() > 75 || m_wpPlayer.lock()->GetRGrassHopperTime() <= 80 && m_wpPlayer.lock()->GetRGrassHopperTime() > 75)
		{
			if (m_wpPlayer.lock()->GetLGrassHopperTime() <= 80 && m_wpPlayer.lock()->GetLGrassHopperTime() > 79 || m_wpPlayer.lock()->GetRGrassHopperTime() <= 80 && m_wpPlayer.lock()->GetRGrassHopperTime() > 79)
			{
				m_LocalPos = Math::Matrix::CreateTranslation(0.0, CAMERAY + 0.1f, CAMERAZ - m_cameracChasePower);
			}
			else if (m_wpPlayer.lock()->GetLGrassHopperTime() <= 79 && m_wpPlayer.lock()->GetLGrassHopperTime() > 78 || m_wpPlayer.lock()->GetRGrassHopperTime() <= 79 && m_wpPlayer.lock()->GetRGrassHopperTime() > 78)
			{
				m_LocalPos = Math::Matrix::CreateTranslation(0.0, CAMERAY - 0.1f, CAMERAZ - m_cameracChasePower);
			}
			else if (m_wpPlayer.lock()->GetLGrassHopperTime() <= 78 && m_wpPlayer.lock()->GetLGrassHopperTime() > 77 || m_wpPlayer.lock()->GetRGrassHopperTime() <= 78 && m_wpPlayer.lock()->GetRGrassHopperTime() > 77)
			{
				m_LocalPos = Math::Matrix::CreateTranslation(0.0, CAMERAY + 0.1f, CAMERAZ - m_cameracChasePower);
			}
			else if (m_wpPlayer.lock()->GetLGrassHopperTime() <= 77 && m_wpPlayer.lock()->GetLGrassHopperTime() >= 76 || m_wpPlayer.lock()->GetRGrassHopperTime() <= 77 && m_wpPlayer.lock()->GetRGrassHopperTime() >= 76)
			{
				m_LocalPos = Math::Matrix::CreateTranslation(0.0, CAMERAY - 0.1f, CAMERAZ - m_cameracChasePower);
			}
		}
		else
		{
			m_LocalPos = Math::Matrix::CreateTranslation(0, CAMERAY, CAMERAZ - m_cameracChasePower);
		}
	}

	//float z = m_wpEnemy.lock()->GetPos().z - m_wpPlayer.lock()->GetPos().z;
	/*if (z >= 0)
	{
		m_bRotateEnemy = true;
	}
	else
	{
		m_bRotateEnemy = false;
	}*/

	Math::Matrix rotMatX;
	if (!m_bRotateEnemy)
	{
		// カメラの回転
		UpdateRotateByMouse();
	}
	else
	{
		UpdateRotateByEnemy();
		SetCursorPos(m_FixMousePos.x, m_FixMousePos.y);
		rotMatX = Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(10));
	}
		m_Rotation = GetRotationMatrix();
	
	if (m_bCameraSet)
	{
		CameraSetUpdate();
	}

	m_mWorld = m_LocalPos * rotMatX * m_Rotation * targetMat;

	CameraBase::Update();
}

void GameCamera::SetEnemy(std::shared_ptr<Enemy> a_enemy)
{
	m_wpEnemy = a_enemy;
	m_bRotateEnemy = !m_bRotateEnemy;
}

void GameCamera::UpdateRotateByMouse()
{
	// マウスでカメラを回転させる処理
	POINT nowPos;
	GetCursorPos(&nowPos);

	POINT mouseMove;
	mouseMove.x = nowPos.x - m_FixMousePos.x;
	mouseMove.y = nowPos.y - m_FixMousePos.y;

	SetCursorPos(m_FixMousePos.x, m_FixMousePos.y);
	// マウス感度
// カメラを回転させる処理  // ↓
	m_DegAng.x += mouseMove.y * 0.15f;// マウスを上下した時回したい軸はX
	m_DegAng.y += mouseMove.x * 0.15f;// マウスを左右にした時回したい軸はY

	// 回転制御
	m_DegAng.x = std::clamp(m_DegAng.x, -FLT_MAX, FLT_MAX);
}

void GameCamera::UpdateRotateByEnemy()
{
	//Math::Matrix mInvertRideObject;
	//m_mWorld.Invert(mInvertRideObject);
	//enemyMat.CreatePerspectiveFieldOfView(); // これでいけるかも
	//Math::Matrix enemyMat = m_wpEnemy.lock()->GetMatrix() * mInvertRideObject;
	//Math::Vector3 enemyPos = enemyMat.Translation();
	//enemyPos.x = enemyPos.x - m_FixMousePos.x;
	//enemyPos.y = (enemyPos.y + 0.5f) - m_FixMousePos.y;

	Math::Vector3 nowVec = m_mWorld.Backward();
	nowVec.y = 0.0f;
	nowVec.Normalize();

	// 向きたい方向
	Math::Vector3 toVec = m_wpEnemy.lock()->GetPos() - GetPos();
	toVec.y = 0.0f;
	toVec.Normalize();

	Math::Vector3 dot = DirectX::XMVector3Dot(nowVec, toVec);
	if (dot.x > 1)
	{
		dot.x = 1;
	}
	if (dot.x < -1)
	{
		dot.x = -1;
	}

	// 角度を取得
	float ang = DirectX::XMConvertToDegrees(acos(dot.x));

	//// 角度制限
	//if (ang > 5)
	//{
	//	ang = 5.0f;
	//}
	//if (ang < -5)
	//{
	//	ang = -5.0f;
	//}

	Math::Vector3 cross = DirectX::XMVector3Cross(nowVec, toVec);
	if (cross.y >= 0)
	{
		m_DegAng.y += ang;
	}
	
	if (cross.y < 0)
	{
		m_DegAng.y -= ang;
	}

	//nowVec = Math::Vector3::TransformNormal(nowVec,Math::Matrix::CreateRotationY(m_DegAng.y));
	
	if (m_wpPlayer.lock()->GetPlayerState() & Player::PlayerState::rlAttackRush)return;

	nowVec   = m_mWorld.Backward();
	nowVec.x = 0.0f;
	nowVec.Normalize();

	toVec   = Math::Vector3(m_wpEnemy.lock()->GetPos().x, m_wpEnemy.lock()->GetPos().y + 0.5f, m_wpEnemy.lock()->GetPos().z) - Math::Vector3(m_wpPlayer.lock()->GetPos().x, m_wpPlayer.lock()->GetPos().y + 0.5f, m_wpPlayer.lock()->GetPos().z);
	toVec.x = 0.0f;
	toVec.Normalize();

	dot = DirectX::XMVector3Dot(nowVec, toVec);
	
	if (dot.x > 1)
	{
		dot.x = 1;
	}
	if (dot.x < -1)
	{
		dot.x = -1;
	}

	// 角度を取得
	ang = DirectX::XMConvertToDegrees(acos(dot.x));

	if (ang >= 15)
	{
		if (ang > 1)
		{
			ang = 1;
		}

		if (ang < -1)
		{
			ang = -1;
		}

		/*Math::Vector3 crossX = DirectX::XMVector3Cross(nowVec, toVec);*/
		float y = toVec.y - nowVec.y;
		if (y >= 0)
		{
			m_DegAng.x -= ang;
		}
		if (y < 0)
		{
			m_DegAng.x += ang;
		}

		if (m_DegAng.x > 80)
		{
			m_DegAng.x = 80;
			if (cross.y >= 0)
			{
				m_DegAng.y += 20;
			}

			if (cross.y < 0)
			{
				m_DegAng.y -= 20;
			}
		}

		if (m_DegAng.x < -80)
		{
			m_DegAng.x = -80;
			if (cross.y >= 0)
			{
				m_DegAng.y += 20;
			}

			if (cross.y < 0)
			{
				m_DegAng.y -= 20;
			}
		}
	}

	float y = m_wpEnemy.lock()->GetPos().y - m_wpPlayer.lock()->GetPos().y;
	if (y <= 0)
	{
		if (m_wpPlayer.lock()->GetPos().y <= m_stepOnPlayerPos.y)
		{
			if (m_DegAng.x < 0)
			{
				m_DegAng.x += 10;
				if (m_DegAng.x >= 0)
				{
					m_DegAng.x = 0;
				}
			}
			else
			{
				m_DegAng.x -= 10;
				if (m_DegAng.x <= 0)
				{
					m_DegAng.x = 0;
				}
			}
		}
	}
}

void GameCamera::CameraSetUpdate()
{
	// 移動方向のベクトル
	Math::Vector3 targetDir = m_cameraSetVec;

	targetDir.Normalize();

	float targetAng = atan2(targetDir.x, targetDir.z);
	targetAng = DirectX::XMConvertToDegrees(targetAng);

	// 角度の差分
	float betweenAng = targetAng - m_DegAng.y;
	if (betweenAng > 180)
	{
		betweenAng -= 360;
	}
	else if (betweenAng < -180)
	{
		betweenAng += 360;
	}

	if (betweenAng != 0.0f)
	{
		float rotateAng = std::clamp(betweenAng, -15.0f, 15.0f);
		m_DegAng.y += rotateAng;
	}
	else
	{
		m_bCameraSet = false;
	}
}

//void GameCamera::SetBCameraSet(Math::Vector3 _mobeVec)
//{
//	m_bCameraSet = true;
//	m_cameraSetVec = _mobeVec;
//}
