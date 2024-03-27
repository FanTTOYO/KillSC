#include "GameCamera.h"
#include "../../main.h"
#include "../../Object/Character/Player/Player.h"
#include "../../Object/Character/Enemy/Enemy.h"
#include "../../Scene/SceneManager.h"

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
	m_startPos[0] = {};
	m_startPos[1] = {};
	m_startPos[2] = {};
	m_startPos[3] = {};
	m_startPos[4] = {};
	m_startDelayFive = true;
	CameraBase::Init();
}

void GameCamera::Update()
{
	m_FixMousePos.x = 640;
	m_FixMousePos.y = 360;
	ClientToScreen(Application::Instance().GetWindowHandle(), &m_FixMousePos);

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

				//===========================================================
				// 座標の補間

				Math::Vector3 endPos;
				Math::Vector3 nowPos;

				if (m_wpPlayer.lock()->GetCharaState() & (CharacterBase::grassHopperDash | CharacterBase::grassHopperDashUp) && m_wpPlayer.lock()->GetLGrassHopperTime() <= 75 || m_wpPlayer.lock()->GetRGrassHopperTime() <= 75)
				{
					m_startPos[0] = m_startPos[1];
					m_startPos[1] = m_startPos[2];
					m_startPos[2] = m_startPos[3];
					m_startPos[3] = m_startPos[4];

					m_startPos[4] = spTarget->GetMatrix().Translation();
					if (m_startDelayFive)
					{
						endPos = spTarget->GetMatrix().Translation();

						// 中間を求める
						// 線形補間
						nowPos = Math::Vector3::Lerp(m_startPos[0], endPos, 0.5f);
					}
					else
					{
						endPos = spTarget->GetMatrix().Translation();

						// 中間を求める
						// 線形補間
						nowPos = Math::Vector3::Lerp(m_startPos[0], endPos, 0.65f);
					}

					targetMat = Math::Matrix::CreateTranslation(nowPos);
				}
				else if (m_wpPlayer.lock()->GetCharaState() & (CharacterBase::grassHopperDash | CharacterBase::grassHopperDashUp) && m_wpPlayer.lock()->GetLGrassHopperTime() > 75 || m_wpPlayer.lock()->GetRGrassHopperTime() > 75)
				{
					if (!m_bRotateEnemy)
					{
						m_startDelayFive = true;
					}
					else
					{
						m_startDelayFive = false;
					}

					//endPos = spTarget->GetMatrix().Translation();

					//// 中間を求める
					//// 線形補間
					//nowPos = Math::Vector3::Lerp(m_startPos[0], endPos, 0.5f);
					targetMat = Math::Matrix::CreateTranslation(spTarget->GetPos().x, spTarget->GetPos().y, spTarget->GetPos().z);
				}

				else
				{
					targetMat = Math::Matrix::CreateTranslation(spTarget->GetPos().x, spTarget->GetPos().y, spTarget->GetPos().z);
				}
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
		else if (SceneManager::Instance().GetScreenVibFrames() > 0 && SceneManager::Instance().GetUpdateStopCnt() == 0)
		{
			int PMVal = 0;
			if (SceneManager::Instance().GetScreenVibFrames() <= 30 && SceneManager::Instance().GetScreenVibFrames() > 28 ||
				SceneManager::Instance().GetScreenVibFrames() <= 26 && SceneManager::Instance().GetScreenVibFrames() > 24 ||
				SceneManager::Instance().GetScreenVibFrames() <= 22 && SceneManager::Instance().GetScreenVibFrames() > 20 ||
				SceneManager::Instance().GetScreenVibFrames() <= 18 && SceneManager::Instance().GetScreenVibFrames() > 16)
			{
				if (SceneManager::Instance().GetScreenVibFrames() == 30 ||
					SceneManager::Instance().GetScreenVibFrames() == 26 ||
					SceneManager::Instance().GetScreenVibFrames() == 22 ||
					SceneManager::Instance().GetScreenVibFrames() == 18)
				{
					PMVal = (rand() % 2) * 2 - 1;
				}
				m_LocalPos = Math::Matrix::CreateTranslation(0.025f * PMVal, CAMERAY + 0.025f * PMVal, CAMERAZ - m_cameracChasePower);
			}
			else if (SceneManager::Instance().GetScreenVibFrames() <= 28 && SceneManager::Instance().GetScreenVibFrames() > 26 ||
				SceneManager::Instance().GetScreenVibFrames() <= 24 && SceneManager::Instance().GetScreenVibFrames() > 22 ||
				SceneManager::Instance().GetScreenVibFrames() <= 20 && SceneManager::Instance().GetScreenVibFrames() > 18 ||
				SceneManager::Instance().GetScreenVibFrames() <= 16 && SceneManager::Instance().GetScreenVibFrames() > 15)
			{
				if (SceneManager::Instance().GetScreenVibFrames() == 28 ||
					SceneManager::Instance().GetScreenVibFrames() == 24 ||
					SceneManager::Instance().GetScreenVibFrames() == 20 ||
					SceneManager::Instance().GetScreenVibFrames() == 16)
				{
					PMVal = (rand() % 2) * 2 - 1;
				}
				m_LocalPos = Math::Matrix::CreateTranslation(0.025f * PMVal, CAMERAY + 0.025f * PMVal, CAMERAZ - m_cameracChasePower);
			}
			else if (SceneManager::Instance().GetScreenVibFrames() <= 15 && SceneManager::Instance().GetScreenVibFrames() > 12 ||
				SceneManager::Instance().GetScreenVibFrames() <= 9 && SceneManager::Instance().GetScreenVibFrames() > 6 ||
				SceneManager::Instance().GetScreenVibFrames() <= 3 && SceneManager::Instance().GetScreenVibFrames() > 0
				)
			{
				if (SceneManager::Instance().GetScreenVibFrames() == 15 ||
					SceneManager::Instance().GetScreenVibFrames() == 9 ||
					SceneManager::Instance().GetScreenVibFrames() == 3
					)
				{
					PMVal = (rand() % 2) * 2 - 1;
				}
				m_LocalPos = Math::Matrix::CreateTranslation(0.0125f * PMVal, CAMERAY + 0.0125f * PMVal, CAMERAZ - m_cameracChasePower);
			}
			else if (SceneManager::Instance().GetScreenVibFrames() <= 12 && SceneManager::Instance().GetScreenVibFrames() > 9 ||
				SceneManager::Instance().GetScreenVibFrames() <= 6 && SceneManager::Instance().GetScreenVibFrames() > 3
				)
			{
				if (SceneManager::Instance().GetScreenVibFrames() == 12 ||
					SceneManager::Instance().GetScreenVibFrames() == 6
					)
				{
					PMVal = (rand() % 2) * 2 - 1;
				}
				m_LocalPos = Math::Matrix::CreateTranslation(0.0125f * PMVal, CAMERAY + 0.0125f * PMVal, CAMERAZ - m_cameracChasePower);
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

	Math::Matrix rotMatX = Math::Matrix::Identity;
	if (!m_bRotateEnemy)
	{
		// カメラの回転
		UpdateRotateByMouse();
	}
	else
	{
		if (m_wpEnemy.expired())
		{
			m_bRotateEnemy = false;
		}
		if (m_wpEnemy.lock()->GetBDeath())
		{
			m_bRotateEnemy = false;
		}
		else
		{
			UpdateRotateByEnemy();
		}

		SetCursorPos(m_FixMousePos.x, m_FixMousePos.y);
		rotMatX = Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(10));
	}
	m_Rotation = GetRotationMatrix();

	if (m_bCameraSet)
	{
		CameraSetUpdate();
	}

	m_mWorld = m_LocalPos * rotMatX * m_Rotation * targetMat;

	UpdateCollision();

	CameraBase::Update();
}

void GameCamera::SetEnemy(std::shared_ptr<Enemy> a_enemy)
{
	m_wpEnemy = a_enemy;
	m_bRotateEnemy = true;
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
	Math::Vector3 nowVec = m_mWorld.Backward();
	nowVec.y = 0.0f;
	nowVec.Normalize();

	// 向きたい方向
	Math::Vector3 toVec = Math::Vector3(m_wpEnemy.lock()->GetPos().x + m_wpEnemy.lock()->GetRockOnPos().x, m_wpEnemy.lock()->GetPos().y + m_wpEnemy.lock()->GetRockOnPos().y, m_wpEnemy.lock()->GetPos().z + m_wpEnemy.lock()->GetRockOnPos().z) -
		Math::Vector3(m_wpPlayer.lock()->GetPos().x + m_wpPlayer.lock()->GetAddCenterVal().x, m_wpPlayer.lock()->GetPos().y + m_wpPlayer.lock()->GetAddCenterVal().y, m_wpPlayer.lock()->GetPos().z + m_wpPlayer.lock()->GetAddCenterVal().z);
	toVec.y = 0.0f;
	toVec.Normalize();

	float ang = DotProductCalculation(nowVec, toVec);

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

	if (m_wpPlayer.lock()->GetCharaState() & CharacterBase::rlAttackRush)return;

	nowVec = m_mWorld.Backward();
	nowVec.x = 0.0f;
	nowVec.Normalize();

	toVec = Math::Vector3(m_wpEnemy.lock()->GetPos().x + m_wpEnemy.lock()->GetRockOnPos().x, m_wpEnemy.lock()->GetPos().y + m_wpEnemy.lock()->GetRockOnPos().y, m_wpEnemy.lock()->GetPos().z + m_wpEnemy.lock()->GetRockOnPos().z) -
		Math::Vector3(m_wpPlayer.lock()->GetPos().x + m_wpPlayer.lock()->GetAddCenterVal().x, m_wpPlayer.lock()->GetPos().y + m_wpPlayer.lock()->GetAddCenterVal().y, m_wpPlayer.lock()->GetPos().z + m_wpPlayer.lock()->GetAddCenterVal().z);
	toVec.x = 0.0f;
	toVec.Normalize();

	ang = DotProductCalculation(nowVec, toVec);

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
	if (y <= 0.5f)
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

void GameCamera::UpdateCollision()
{
	const std::shared_ptr<const KdGameObject>spTarget = m_wpTarget.lock();
	if (!spTarget)return;

	// ①当たり判定(レイ判定)用の情報作成
	KdCollider::RayInfo rayInfo;
	// レイの発射位置を設定
	rayInfo.m_pos = spTarget->GetPos();
	rayInfo.m_pos.y += 1.5f;

	// レイの発射方向を設定
	Math::Vector3 dir = GetPos() - rayInfo.m_pos;
	dir.Normalize();
	rayInfo.m_dir = dir;

	// レイの長さ
	rayInfo.m_range = (GetPos() - rayInfo.m_pos).Length();

	// 当たり判定をしたいタイプを設定
	rayInfo.m_type = KdCollider::TypeGround;

	// ②HIT判定対象オブジェクトに総当たり
	std::list<KdCollider::CollisionResult> retRayList;

	float maxOverLap = 0;
	for (auto& hitObj : m_hitObjList)
	{
		std::shared_ptr<KdGameObject> spGameObj = hitObj.lock();
		if (spGameObj)
		{
			spGameObj->Intersects(rayInfo, &retRayList);

			// ③結果を使って座標を補完する
			// レイに当たったリストから一番近いオブジェクトを検出
			Math::Vector3 hitPos = Math::Vector3::Zero;
			bool hit = false;
			for (auto& ret : retRayList)
			{
				// レイを遮断しオーバーした長さが
				// 一番長いものを探す
				if (maxOverLap < ret.m_overlapDistance)
				{
					maxOverLap = ret.m_overlapDistance;
					hitPos = ret.m_hitPos;
					hit = true;
				}
			}
			// 何かしらの上に乗ってる

			if (hit)
			{
				SetPos(hitPos);
			}
		}
	}
}

//void GameCamera::SetBCameraSet(Math::Vector3 _mobeVec)
//{
//	m_bCameraSet = true;
//	m_cameraSetVec = _mobeVec;
//}
