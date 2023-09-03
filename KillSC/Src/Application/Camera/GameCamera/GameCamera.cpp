#include "GameCamera.h"
#include "../../Object/Character/Player/Player.h"
#include "../../Object/Character/Enemy/Enemy.h"

void GameCamera::Init()
{
	// �����_
	m_LocalPos = Math::Matrix::CreateTranslation(0, CAMERAY, CAMERAZ);

	// ����ʒ���
	m_FixMousePos.x = 640;
	m_FixMousePos.y = 360;

	m_bRotateEnemy = false;
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
				// Player�
				//targetMat = Math::Matrix::CreateTranslation(spTarget->GetPos().x, spTarget->GetPos().y, spTarget->GetPos().z);

				// �n�ʊ ����
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

				if (m_cameracChasePower > 0.0f && m_wpPlayer.lock()->GetGravity() == 0 && !(m_wpPlayer.lock()->GetPlayerState() & (Player::PlayerState::grassHopperDash | Player::PlayerState::grassHopperDashUp)))
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
				m_LocalPos = Math::Matrix::CreateTranslation(0.0, CAMERAY + 0.1f, CAMERAZ);
			}
			else if (m_wpPlayer.lock()->GetLGrassHopperTime() <= 79 && m_wpPlayer.lock()->GetLGrassHopperTime() > 78 || m_wpPlayer.lock()->GetRGrassHopperTime() <= 79 && m_wpPlayer.lock()->GetRGrassHopperTime() > 78)
			{
				m_LocalPos = Math::Matrix::CreateTranslation(0.0, CAMERAY - 0.1f, CAMERAZ);
			}
			else if (m_wpPlayer.lock()->GetLGrassHopperTime() <= 78 && m_wpPlayer.lock()->GetLGrassHopperTime() > 77 || m_wpPlayer.lock()->GetRGrassHopperTime() <= 78 && m_wpPlayer.lock()->GetRGrassHopperTime() > 77)
			{
				m_LocalPos = Math::Matrix::CreateTranslation(0.0, CAMERAY + 0.1f, CAMERAZ);
			}
			else if (m_wpPlayer.lock()->GetLGrassHopperTime() <= 77 && m_wpPlayer.lock()->GetLGrassHopperTime() >= 76 || m_wpPlayer.lock()->GetRGrassHopperTime() <= 77 && m_wpPlayer.lock()->GetRGrassHopperTime() >= 76)
			{
				m_LocalPos = Math::Matrix::CreateTranslation(0.0, CAMERAY - 0.1f, CAMERAZ);
			}
		}
		else
		{
			m_LocalPos = Math::Matrix::CreateTranslation(0, CAMERAY, CAMERAZ);
		}
	}

	if (!m_bRotateEnemy)
	{
		// �J�����̉�]
		UpdateRotateByMouse();
	}
	else
	{
		UpdateRotateByEnemy();
		SetCursorPos(m_FixMousePos.x, m_FixMousePos.y);
	}
		m_Rotation = GetRotationMatrix();
	
	if (m_bCameraSet)
	{
		CameraSetUpdate();
	}

	m_mWorld = m_LocalPos * m_Rotation * targetMat;

	CameraBase::Update();
}

void GameCamera::SetEnemy(std::shared_ptr<Enemy> a_enemy)
{
	m_wpEnemy = a_enemy;
	m_bRotateEnemy = !m_bRotateEnemy;
}

void GameCamera::UpdateRotateByMouse()
{
	// �}�E�X�ŃJ��������]�����鏈��
	POINT nowPos;
	GetCursorPos(&nowPos);

	POINT mouseMove;
	mouseMove.x = nowPos.x - m_FixMousePos.x;
	mouseMove.y = nowPos.y - m_FixMousePos.y;

	SetCursorPos(m_FixMousePos.x, m_FixMousePos.y);
	// �}�E�X���x
// �J��������]�����鏈��  // ��
	m_DegAng.x += mouseMove.y * 0.15f;// �}�E�X���㉺�������񂵂�������X
	m_DegAng.y += mouseMove.x * 0.15f;// �}�E�X�����E�ɂ������񂵂�������Y

	// ��]����
	m_DegAng.x = std::clamp(m_DegAng.x, -FLT_MAX, FLT_MAX);
}

void GameCamera::UpdateRotateByEnemy()
{
	//Math::Matrix mInvertRideObject;
	//m_mWorld.Invert(mInvertRideObject);
	//enemyMat.CreatePerspectiveFieldOfView(); // ����ł����邩��
	//Math::Matrix enemyMat = m_wpEnemy.lock()->GetMatrix() * mInvertRideObject;
	//Math::Vector3 enemyPos = enemyMat.Translation();
	//enemyPos.x = enemyPos.x - m_FixMousePos.x;
	//enemyPos.y = (enemyPos.y + 0.5f) - m_FixMousePos.y;

	Math::Vector3 nowVec = m_mWorld.Backward();

	// ������������
	Math::Vector3 toVec = m_wpEnemy.lock()->GetPos();
	toVec.Normalize();

	// ���ρi��]����p�x�����߂�j
	// �x�N�g��A�ƃx�N�g��B�ƃR�T�C���Ȃ��p
	// A�EB
	Math::Vector3 dot = DirectX::XMVector3Dot(nowVec, toVec);
	// �x�N�g��A�̒����P
	// �x�N�g��B�̒����P
	// �Ȃ̂�dot�̒��ɂ̓R�T�C���Ȃ��p���������Ă���
	// �ۂߌ덷�i�����_�ȉ����ȗ������ۂɐ�����덷�j
	if (dot.x > 1)// .?�͂Ȃ�ł�����
	{
		dot.x = 1;
	}
	if (dot.x < -1)
	{
		dot.x = -1;
	}

	// �p�x���擾
	float ang = DirectX::XMConvertToDegrees(acos(dot.x));

	// �����ł��p�x���ς������
	if (ang >= 10.0f)
	{
		// �p�x����
		if (ang > 5)
		{
			ang = 5.0f;
		}
		if (ang < -5)
		{
			ang = -5.0f;
		}

		// �O�ρi�ǂ����ɉ�]���邩���ׂ�j
		// �x�N�g��A�ƃx�N�g��B�ɐ����ȃx�N�g��
		// A x B
		Math::Vector3 cross = DirectX::XMVector3Cross(nowVec, toVec);
		if (cross.y >= 0)
		{
			m_DegAng.y += ang;
		}

		if (cross.y < 0)
		{
			m_DegAng.y -= ang;
		}
	}
}

void GameCamera::CameraSetUpdate()
{
	// �ړ������̃x�N�g��
	Math::Vector3 targetDir = m_cameraSetVec;

	targetDir.Normalize();

	float targetAng = atan2(targetDir.x, targetDir.z);
	targetAng = DirectX::XMConvertToDegrees(targetAng);

	// �p�x�̍���
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
