#include "GameCamera.h"
#include "../../Object/Character/Player/Player.h"

void GameCamera::Init()
{
	// 注視点
	m_LocalPos = Math::Matrix::CreateTranslation(0, 3.5f, -7.5f);

	// ↓画面中央
	m_FixMousePos.x = 640;
	m_FixMousePos.y = 360;

	SetCursorPos(m_FixMousePos.x, m_FixMousePos.y);

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
				//targetMat = Math::Matrix::CreateTranslation(spTarget->GetPos().x, spTarget->GetPos().y, spTarget->GetPos().z);

				// 地面基準 未完
				if (spTarget->GetPos().y < m_stepOnPlayerPos.y + 5 && spTarget->GetPos().y > m_stepOnPlayerPos.y - 5 && !m_bCameraDown)
				{
					m_cameracChasePower = 0;
					targetMat = Math::Matrix::CreateTranslation(spTarget->GetPos().x, m_stepOnPlayerPos.y, spTarget->GetPos().z);
				}
				else if (spTarget->GetPos().y >= m_stepOnPlayerPos.y + 5 && spTarget->GetPos().y > m_stepOnPlayerPos.y + m_cameracChasePower)
				{
					float b = spTarget->GetPos().y - m_stepOnPlayerPos.y + m_cameracChasePower;
					if (b >= 2.0f)
					{
						m_cameracChasePower += m_wpPlayer.lock()->GetDashSpd();
					}

					targetMat = Math::Matrix::CreateTranslation(spTarget->GetPos().x, m_stepOnPlayerPos.y + m_cameracChasePower, spTarget->GetPos().z);
				}
				else if(m_wpPlayer.lock()->GetGravity() < 0)
				{
					m_cameracChasePower = m_wpPlayer.lock()->GetGravity() * 0.95f;
					targetMat = Math::Matrix::CreateTranslation(spTarget->GetPos().x, m_stepOnPlayerPos.y + m_cameracChasePower, spTarget->GetPos().z);
				}
				else
				{
					float b = 0.0f;
					b = spTarget->GetPos().y - m_stepOnPlayerPos.y;
					m_bCameraDown = true;
					if (m_cameracChasePower <= 0.0f)
					{
						m_bCameraDown = false;
					}

					if (m_wpPlayer.lock()->GetGravity() > 0)
					{
						m_cameracChasePower -= m_wpPlayer.lock()->GetGravity();
					}

					targetMat = Math::Matrix::CreateTranslation(spTarget->GetPos().x, m_stepOnPlayerPos.y + m_cameracChasePower, spTarget->GetPos().z);
				}

				if (m_cameracChasePower > 0.0f && m_wpPlayer.lock()->GetGravity() == 0 && !(m_wpPlayer.lock()->GetPlayerState() & (grassHopperDash | grassHopperDashUp)))
				{
					m_cameracChasePower -= 0.15f;
					m_bCameraDown = true;
					if (m_cameracChasePower <= 0.0f)
					{
						m_bCameraDown = false;
					}
					targetMat = Math::Matrix::CreateTranslation(spTarget->GetPos().x, m_stepOnPlayerPos.y + m_cameracChasePower, spTarget->GetPos().z);
				}
			}
		}


		if (m_wpPlayer.lock()->GetLGrassHopperTime() <= 80 && m_wpPlayer.lock()->GetLGrassHopperTime() > 75 || m_wpPlayer.lock()->GetRGrassHopperTime() <= 80 && m_wpPlayer.lock()->GetRGrassHopperTime() > 75)
		{
			if (m_wpPlayer.lock()->GetLGrassHopperTime() <= 80 && m_wpPlayer.lock()->GetLGrassHopperTime() > 79 || m_wpPlayer.lock()->GetRGrassHopperTime() <= 80 && m_wpPlayer.lock()->GetRGrassHopperTime() > 79)
			{
				m_LocalPos = Math::Matrix::CreateTranslation(0.0, 3.6f, -7.5f);
			}
			else if (m_wpPlayer.lock()->GetLGrassHopperTime() <= 79 && m_wpPlayer.lock()->GetLGrassHopperTime() > 78 || m_wpPlayer.lock()->GetRGrassHopperTime() <= 79 && m_wpPlayer.lock()->GetRGrassHopperTime() > 78)
			{
				m_LocalPos = Math::Matrix::CreateTranslation(0.0, 3.4f, -7.5f);
			}
			else if (m_wpPlayer.lock()->GetLGrassHopperTime() <= 78 && m_wpPlayer.lock()->GetLGrassHopperTime() > 77 || m_wpPlayer.lock()->GetRGrassHopperTime() <= 78 && m_wpPlayer.lock()->GetRGrassHopperTime() > 77)
			{
				m_LocalPos = Math::Matrix::CreateTranslation(0.0, 3.6f, -7.5f);
			}
			else if (m_wpPlayer.lock()->GetLGrassHopperTime() <= 77 && m_wpPlayer.lock()->GetLGrassHopperTime() >= 76 || m_wpPlayer.lock()->GetRGrassHopperTime() <= 77 && m_wpPlayer.lock()->GetRGrassHopperTime() >= 76)
			{
				m_LocalPos = Math::Matrix::CreateTranslation(0.0, 3.4f, -7.5f);
			}
		}
		else
		{
			m_LocalPos = Math::Matrix::CreateTranslation(0, 3.5f, -7.5f);
		}
	}

	// カメラの回転
	UpdateRotateByMouse();
	m_Rotation = GetRotationMatrix();

	if (m_bCameraSet)
	{
		CameraSetUpdate();
	}

	m_mWorld = m_LocalPos * m_Rotation * targetMat;

	CameraBase::Update();
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
