#include "Enemy.h"
#include "../../../Camera/GameCamera/GameCamera.h"
#include "../../../Scene/SceneManager.h"
#include "../../Weapon/Hopper/Hopper.h"
#include "../../Weapon/Scopion/Scopion.h"
#include "../Player/Player.h"

void Enemy::Init(std::weak_ptr<json11::Json> a_wpJsonObj)
{
	// 行列合成
	m_mWorld = Math::Matrix::Identity;
	m_mWorldRot.y = 0;

	m_model = std::make_shared<KdModelWork>();
	m_model->SetModelData
	(KdAssets::Instance().m_modeldatas.GetData
	("Asset/Models/Enemy/BossEnemy.gltf"));
	/// 当たり判定初期化
	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape
	("EnemyModel", m_model, KdCollider::TypeBump | KdCollider::TypeDamage | KdCollider::TypeAttackDec);

	m_enduranceBarPoly.SetMaterial("Asset/Textures/Ui/Game/enduranceBar.png");
	m_endurancePoly.SetMaterial("Asset/Textures/Ui/Game/endurance.png");

	m_pDebugWire = std::make_unique<KdDebugWireFrame>();

	m_EnemyState = idle;

	m_weaponType |= eScorpion;
	m_weaponType |= eLScorpion;
	m_wantToMoveState = wNone;
	m_attackHitImmediatelyAfter = false;
	m_rightWeaponNumber = 1;
	m_leftWeaponNumber = 1;

	m_hitStopCnt = 0;
	m_hitColorChangeTimeCnt = 0;
	m_hitMoveSpd = 0.0f;
	m_gardMoveSpd = 0.0f;

	m_invincibilityTimeCnt = 0;
	m_bTough = false;
	m_bFirstUpdate = true;
	m_bRushAttackPossible = false;
	m_bEnemyLose = false;
	m_graduallyTorionDecVal = 0;
	m_enemyAirborneTimetoBeCnt = ENEMYAIRBORNETIMETOBECNTVAL;

	m_animator = std::make_shared<KdAnimator>();
	m_animator->SetAnimation(m_model->GetAnimation("IdleA"));
	m_grassSuccessionDelayCnt = 0;

	m_bMantisAttack = false;
	m_bEnemyDeath = false;

	m_bMantisPossAng = false;
	m_bEnemyLose = false;
	m_hasDeTime = 0;
	m_overStageTime = 0;

	m_vForce = 1.0f;
	m_endurance = 1.0f;
	m_enemyType = Enemy::EnemyType::allRounder;

	m_bEnemyBetweenPlayer = false;

	m_enemyNumber = 0;
	m_coarseFishEnemyAttackDelayCnt = 0;
	m_delayTurnAroundTime = 0;

	m_idleCnt = 0;

	m_bShotBeam = false;
	m_bShotEnergyBullet = false;
	m_bBeamHitStart = false;
	m_bEnergyBulletHitStart = false;
	m_bRangedAttack = false;

	m_enemyAttackTotal = 0;
	m_enemyAttackMaxTotal = 0;

	m_bRangedAttackCapableOfFiring = false;
	m_rangedAttackAnimeCnt = 0;

	m_weaknesSsuccessionHitCnt = 0;
	m_addWeaknesSsuccessionHitCntTime = 0;

	m_addRotationAttackDistToPlayerTime = 0;
	m_notHumanoidEnemyState = stand;
	m_attackAnimeCnt = 0;
	m_bBlowingAwayHitB = false;

	m_attackDelayTime = 0;
	m_bEnemy = true;
	m_bLethalDamageToKickOrPantciOrDashAttackHit = false;

	m_wpJsonObj = a_wpJsonObj;
}

void Enemy::Update()
{
	float lowestYPos;
	if (SceneManager::Instance().GetSceneType() == SceneManager::SceneType::tutorial)
	{
		lowestYPos = static_cast<float>((*m_wpJsonObj.lock())["TutorialMinimumYPos"].number_value());
		m_mpObj = (*m_wpJsonObj.lock())["TutorialEnemy"].object_items();
		m_mWorldRot.y = (float)m_mpObj["InitWorldRotationY"].number_value();

		if (m_bFirstUpdate)
		{
			m_weaponList.clear();
		}
	}
	else
	{
		if (SceneManager::Instance().GetSceneType() == SceneManager::SceneType::training)
		{
			lowestYPos = static_cast<float>((*m_wpJsonObj.lock())["TutorialMinimumYPos"].number_value());
		}
		else
		{
			lowestYPos = static_cast<float>((*m_wpJsonObj.lock())["MinimumYPos"].number_value());
		}
		
		if (m_bEnemyLose)
		{
			return;
		}

#ifdef _DEBUG
		// debugキー
		if (GetAsyncKeyState('P') & 0x8000)
		{
			m_pos = Math::Vector3::Zero;
		}

		// debugキー
		if (GetAsyncKeyState('K') & 0x8000)
		{
			m_vForce = 0;
		}
#endif
		

		m_vForce -= m_graduallyTorionDecVal;

		if (!m_bLethalDamageToKickOrPantciOrDashAttackHit)
		{
			if (m_vForce <= 0)
			{
				m_vForce = 0;
				if (!m_bEnemyDeath)
				{
					m_bEnemyDeath = true;
					m_bMove = true;
					m_EnemyState = idle;
					m_wantToMoveState = wNone;
					m_animator->SetAnimation(m_model->GetAnimation("Death"), false);

				}
			}

			if (m_endurance <= 0)
			{
				m_endurance = 0;
				if (!m_bEnemyDeath)
				{
					m_bEnemyDeath = true;
					m_bMove = true;
					m_EnemyState = idle;
					m_wantToMoveState = wNone;
					m_animator->SetAnimation(m_model->GetAnimation("Death"), false);
				}
			}
		}

		if (m_bBoss)
		{
			if (m_enemyType & bossEnemyTypeOne)
			{
				BossEnemyTyepOneUpdate();
			}
			else
			{
				BossUpdate();
			}
		}
		else if(m_enemyType & EnemyType::coarseFishEnemy)
		{
			CoarseFishEnemyUpdate();
		}
		else if (m_enemyType & EnemyType::wimpEnemyTypeOne)
		{
			WimpEnemyTypeOneUpdate();
		}
	}

	if (m_bFirstUpdate)
	{
		m_bFirstUpdate = false;
		return;
	}


	if (m_pos.x >= static_cast<float>((*m_wpJsonObj.lock())["HightXPos"].number_value())   ||
		m_pos.x <= static_cast<float>((*m_wpJsonObj.lock())["MinimumXPos"].number_value()) ||
		m_pos.z >= static_cast<float>((*m_wpJsonObj.lock())["HightZPos"].number_value())   ||
		m_pos.z <= static_cast<float>((*m_wpJsonObj.lock())["MinimumZPos"].number_value()) ||
		m_pos.y <= lowestYPos															   ||
		m_pos.y >= static_cast<float>((*m_wpJsonObj.lock())["HightYPos"].number_value()))
	{
		m_overStageTime++;
		if (m_overStageTime >= (*m_wpJsonObj.lock())["MaxOverStageTime"].int_value())
		{
			m_pos = Math::Vector3::Zero;
			m_overStageTime = 0;
		}
	}
	else
	{
		if (m_overStageTime >= (*m_wpJsonObj.lock())["MaxOverStageTime"].int_value())
		{
			m_pos = Math::Vector3::Zero;
			m_overStageTime = 0;
		}

		Math::Vector3 addCenterVal = { static_cast<float>(m_mpObj["AddCenterVal"][0].number_value()),
										  static_cast<float>(m_mpObj["AddCenterVal"][1].number_value()),
										  static_cast<float>(m_mpObj["AddCenterVal"][2].number_value()) };
		KdCollider::SphereInfo sphereInfo;
		// 球の中心位置を設定
		sphereInfo.m_sphere.Center = m_pos + addCenterVal;
		// 球の半径を設定

		sphereInfo.m_sphere.Radius = static_cast<float>(m_mpObj["TypeBuriedToHitSphereRadius"].number_value());

		// 当たり判定をしたいタイプを設定
		sphereInfo.m_type = KdCollider::TypeBuried;
#ifdef _DEBUG
		// デバック用
		m_pDebugWire->AddDebugSphere
		(
			sphereInfo.m_sphere.Center,
			sphereInfo.m_sphere.Radius
		);
#endif
		// 球の当たったオブジェクト情報
		std::list<KdCollider::CollisionResult> retSphereList;

		// 球と当たり判定 
		for (auto& obj : SceneManager::Instance().GetObjList())
		{
			obj->Intersects
			(
				sphereInfo,
				&retSphereList
			);
		}

		// 球に当たったリスト情報から一番近いオブジェクトを検出
		float maxOverLap = 0;
		bool hit = false;
		for (auto& ret : retSphereList)
		{
			// 一番近くで当たったものを探す
			if (maxOverLap < ret.m_overlapDistance)
			{
				maxOverLap = ret.m_overlapDistance;
				hit = true;
			}
		}

		if (hit)
		{
			++m_overStageTime;
		}
	}

	if (!m_bEwaponDrawPosible)
	{
		m_bEwaponDrawPosible = true;
	}

	if (m_invincibilityTimeCnt > 0)
	{
		--m_invincibilityTimeCnt;
	}

	if (m_hitColorChangeTimeCnt > 0)
	{
		--m_hitColorChangeTimeCnt;
	}

	
	if (m_EnemyState & hit)
	{
		m_pos.y -= m_gravity;

		if (m_EnemyState & cutRaiseHit)
		{
			m_gravity += static_cast<float>((*m_wpJsonObj.lock())["CutRaiseHitGravityAcceleration"].number_value());
		}
		else
		{
			m_gravity += static_cast<float>((*m_wpJsonObj.lock())["GravityAcceleration"].number_value());
		}

		if (m_hitStopCnt > 0)
		{
			--m_hitStopCnt;
			if (m_EnemyState & blowingAwayHit)
			{
				if (m_hitStopCnt < 10)
				{
					m_hitMoveSpd = 0;
				}

				m_hitMoveSpd *= static_cast<float>((*m_wpJsonObj.lock())["MoveSpeedDecelerationamount"].number_value());
			}
			else
			{
				m_hitMoveSpd *= static_cast<float>((*m_wpJsonObj.lock())["MoveSpeedDecelerationamount"].number_value());
			}

			m_pos += m_knockBackVec * m_hitMoveSpd;
		}

		if (m_hitStopCnt == 0)
		{
			if (m_EnemyState & (nomalHit | cutRaiseHit))
			{
				if (!(m_EnemyState & idle))
				{
					m_animator->SetAnimation(m_model->GetAnimation("IdleA"), false);
				}
				m_EnemyState = idle;
				m_wantToMoveState = wNone;
				if (SceneManager::Instance().GetSceneType() != SceneManager::SceneType::tutorial)
				{
					Brain();
				}
			}
			else if (m_EnemyState & blowingAwayHit)
			{
				if (!m_bLethalDamageToKickOrPantciOrDashAttackHit)
				{
					if (!(m_EnemyState & idle))
					{
						if (!m_bBlowingAwayHitB)
						{
							m_animator->SetAnimation(m_model->GetAnimation("BlowingAwayRise"), false);
						}
						else
						{
							m_bBlowingAwayHitB = false;
							m_animator->SetAnimation(m_model->GetAnimation("IaiKiriRise"), false);
						}
					}
					m_EnemyState = blowingAwayRise;
				}
				else
				{
					m_bEnemyDeath = true;
					m_bMove = true;
					m_EnemyState = idle;
					m_wantToMoveState = wNone;
				}
			}
			else if (m_EnemyState & iaiKiriHit)
			{
				if (!(m_EnemyState & idle))
				{
					m_animator->SetAnimation(m_model->GetAnimation("IaiKiriRise"), false);
				}
				m_EnemyState = iaiKiriRise;
			}
			m_hitStopCnt = 0;
			m_attackHitImmediatelyAfter = true;
			m_hitMoveSpd = 0.0f;
		}
	}
	else
	{
		m_bMove = true;
		if (m_animator->IsAnimationEnd())
		{
			if (!(m_EnemyState & idle))
			{
				m_animator->SetAnimation(m_model->GetAnimation("IdleA"), false);
			}
			m_EnemyState = idle;
			m_wantToMoveState = wNone;
		}
	}

	if (!(m_enemyType & wimpEnemyTypeOne))
	{
		if (!(m_EnemyState & (grassHopperDash | grassHopperDashUp | hit)))
		{
			m_pos.y -= m_gravity;
			m_gravity += static_cast<float>((*m_wpJsonObj.lock())["GravityAcceleration"].number_value());
		}
	}
	else
	{
		m_pos.y -= m_gravity;
		m_gravity += static_cast<float>((*m_wpJsonObj.lock())["GravityAcceleration"].number_value());
	}

	//if (m_pos.y <= -1.05f)//今だけ 地面に着いたらカメラのXAngが180以下かつ0以上補正する
	// ========================================
	// 当たり判定
	// ========================================
	CollisionUpdate();

	if ((m_EnemyState & rlAttackRush) && m_attackAnimeCnt >= m_mpObj["RushLastAttackPointTime"].int_value())
	{
		EnemyKickHitAttackChaeck();
	}

	if (m_bShotBeam || m_bShotEnergyBullet)
	{
		if (m_bBeamHitStart)
		{
			m_beamRange += 2.25f;

			if (m_beamRange > 45)
			{
				m_beamRange = 45;
			}

			EnemyBeamHitChaeck();
		}
		else if (m_bEnergyBulletHitStart)
		{
			m_beamCollisionPos += m_energyBulletDir * static_cast<float>(m_mpObj["EnergyBulletSpeed"].number_value());
			EnemyEnergyBulletHitChaeck();
		}
	}


#ifdef _DEBUG

	if (m_bBoss && !(m_enemyType & bossEnemyTypeOne))
	{
		const KdModelWork::Node* node = nullptr;
		Math::Matrix mat = Math::Matrix::Identity;

		node = m_model->FindNode("LegAttackPoint");
		mat = node->m_worldTransform * m_mWorld;
		mat._42 += 0.7f;
		m_pDebugWire->AddDebugSphere
		(
			mat.Translation(),
			0.3f,
			{ 0,0,1,1 }
		);

		node = m_model->FindNode("LegAttackHitPoint");
		mat = node->m_worldTransform * m_mWorld;
		mat._42 += 0.7f;
		m_pDebugWire->AddDebugSphere
		(
			mat.Translation(),
			0.3f,
			{ 0,0,1,1 }
		);

		node = m_model->FindNode("LegAttackHitPointTwo");
		mat = node->m_worldTransform * m_mWorld;
		mat._42 += 0.7f;
		m_pDebugWire->AddDebugSphere
		(
			mat.Translation(),
			0.3f,
			{ 0,0,1,1 }
		);
	}

#endif

	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_pos);
	Math::Matrix rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y));
	m_mWorld = rotMat * transMat;

	for (auto& WeaList : m_weaponList)
	{
		WeaList->Update();
	}
}

void Enemy::CollisionUpdate()
{
	if (m_enemyType & (wimpEnemyTypeOne | bossEnemyTypeOne))
	{
		// レイ判定用に変数を作成
		KdCollider::RayInfo rayInfo;
		rayInfo.m_pos = m_pos;
		rayInfo.m_dir = { 0,-1,0 };
		float enableStepHight = 0.2f;
		rayInfo.m_pos.y += enableStepHight;
		rayInfo.m_range = m_gravity + enableStepHight;

		rayInfo.m_type = KdCollider::TypeGround;
#ifdef _DEBUG
		m_pDebugWire->AddDebugLine(rayInfo.m_pos, rayInfo.m_dir, rayInfo.m_range, { 1,1,1,1 });
#endif
		std::list<KdCollider::CollisionResult> retRayList;

		for (auto& obj : SceneManager::Instance().GetObjList())
		{
			obj->Intersects
			(
				rayInfo,
				&retRayList
			);
		}

		// レイに当たったリストから一番近いオブジェクトを検出
		float maxOverLap = 0;
		Math::Vector3 groundPos = {};
		bool hit = false;
		for (auto& ret : retRayList)
		{
			// レイを遮断しオーバーした長さ
			// 一番長いものを探す
			if (maxOverLap < ret.m_overlapDistance)
			{
				maxOverLap = ret.m_overlapDistance;
				groundPos = ret.m_hitPos;
				hit = true;
			}
		}

		if (hit)
		{
			//m_pos = groundPos;
			m_pos = groundPos;
			m_gravity = 0;
		}

		KdCollider::SphereInfo sphereInfo;
		// 球の中心位置を設定
		if (m_enemyType & bossEnemyTypeOne)
		{
			sphereInfo.m_sphere.Center = m_pos + Math::Vector3(0, 25, 0);
			sphereInfo.m_sphere.Radius = 15;
		}
		else
		{
			sphereInfo.m_sphere.Center = m_pos + Math::Vector3(0, 3, 0);
			sphereInfo.m_sphere.Radius = 2.0f;
		}

		// 当たり判定をしたいタイプを設定
		sphereInfo.m_type = KdCollider::TypeGround /*| KdCollider::TypeBump*/;

#ifdef _DEBUG
		// デバック用
		m_pDebugWire->AddDebugSphere
		(
			sphereInfo.m_sphere.Center,
			sphereInfo.m_sphere.Radius
		);
#endif

		// 球の当たったオブジェクト情報
		std::list<KdCollider::CollisionResult> retSphereList;

		// 球と当たり判定 
		for (auto& obj : SceneManager::Instance().GetObjList())
		{
			obj->Intersects
			(
				sphereInfo,
				&retSphereList
			);
		}

		// 球に当たったリスト情報から一番近いオブジェクトを検出
		maxOverLap = 0;
		hit = false;
		Math::Vector3 hitDir = {}; // 当たった方向
		for (auto& ret : retSphereList)
		{
			// 一番近くで当たったものを探す
			if (maxOverLap < ret.m_overlapDistance)
			{
				maxOverLap = ret.m_overlapDistance;
				hit = true;
				hitDir = ret.m_hitDir;
			}
		}

		if (hit)
		{
			// 球とモデルが当たっている
			m_pos += (hitDir * maxOverLap);
		}

		if (!m_target.expired() && m_target.lock()->GetPlayerState() & (Player::PlayerState::defense) && !(m_enemyType & bossEnemyTypeOne))
		{
			sphereInfo.m_sphere.Radius = 0.6f;

			// 当たり判定をしたいタイプを設定
			sphereInfo.m_type = KdCollider::TypeGard /*| KdCollider::TypeBump*/;

#ifdef _DEBUG
			// デバック用
			m_pDebugWire->AddDebugSphere
			(
				sphereInfo.m_sphere.Center,
				sphereInfo.m_sphere.Radius
			);
#endif

			// 球の当たったオブジェクト情報
			retSphereList.clear();

			// 球と当たり判定 
			for (auto& obj : m_target.lock()->GetWeaponList())
			{
				obj->Intersects
				(
					sphereInfo,
					&retSphereList
				);
			}

			// 球に当たったリスト情報から一番近いオブジェクトを検出
			maxOverLap = 0;
			hit = false;
			hitDir = {}; // 当たった方向
			for (auto& ret : retSphereList)
			{
				// 一番近くで当たったものを探す
				if (maxOverLap < ret.m_overlapDistance)
				{
					maxOverLap = ret.m_overlapDistance;
					hit = true;
					hitDir = ret.m_hitDir;
				}
			}

			if (hit)
			{
				hitDir.y = 0.0f;
				hitDir.Normalize();
				// 球とモデルが当たっている
				m_pos += (hitDir * maxOverLap);
			}
		}

		sphereInfo.m_sphere.Center = m_pos + Math::Vector3(0, 2.5f, 0);
		sphereInfo.m_sphere.Radius = 2.0f;
		// 当たり判定をしたいタイプを設定
		sphereInfo.m_type = KdCollider::TypeBump;

#ifdef _DEBUG
		// デバック用
		m_pDebugWire->AddDebugSphere
		(
			sphereInfo.m_sphere.Center,
			sphereInfo.m_sphere.Radius
		);
#endif

		// 球の当たったオブジェクト情報
		retSphereList.clear();

		// 球と当たり判定 

		if (!m_target.expired() && !(m_enemyType & bossEnemyTypeOne))
		{
			if (!m_target.lock()->GetBPlayerLose())
			{
				m_target.lock()->Intersects
				(
					sphereInfo,
					&retSphereList
				);
			}


			// 球に当たったリスト情報から一番近いオブジェクトを検出
			maxOverLap = 0;
			hit = false;
			hitDir = {}; // 当たった方向
			for (auto& ret : retSphereList)
			{
				// 一番近くで当たったものを探す
				if (maxOverLap < ret.m_overlapDistance)
				{
					maxOverLap = ret.m_overlapDistance;
					hit = true;
					hitDir = ret.m_hitDir;
				}
			}

			if (hit)
			{
				hitDir.y = 0.0f;
				hitDir.Normalize();
				// 球とモデルが当たっている
				m_pos += (hitDir * maxOverLap);
			}
		}

		if (m_EnemyState & (EnemyState::hit))
		{
			sphereInfo;
			// 球の中心位置を設定
			sphereInfo.m_sphere.Center = m_pos + Math::Vector3(0, 1.5f, 0);
			// 球の半径を設定

			sphereInfo.m_sphere.Radius = 0.4f;

			// 当たり判定をしたいタイプを設定
			sphereInfo.m_type = KdCollider::TypeBuried;
#ifdef _DEBUG
			// デバック用
			m_pDebugWire->AddDebugSphere
			(
				sphereInfo.m_sphere.Center,
				sphereInfo.m_sphere.Radius
			);
#endif
			// 球の当たったオブジェクト情報
			retSphereList.clear();

			// 球と当たり判定 
			for (auto& obj : SceneManager::Instance().GetObjList())
			{
				obj->Intersects
				(
					sphereInfo,
					&retSphereList
				);
			}

			// 球に当たったリスト情報から一番近いオブジェクトを検出
			maxOverLap = 0;
			hit = false;
			hitDir = {}; // 当たった方向
			for (auto& ret : retSphereList)
			{
				// 一番近くで当たったものを探す
				if (maxOverLap < ret.m_overlapDistance)
				{
					maxOverLap = ret.m_overlapDistance;
					hit = true;
					hitDir = ret.m_hitDir;
				}
			}

			if (hit)
			{
				hitDir.y = 0.0f;
				hitDir.Normalize();
				// 球とモデルが当たっている
				m_pos += (hitDir * maxOverLap);

				m_attackMoveSpd = 0.0f;
				m_hitMoveSpd = 0.0f;
			}
		}

		if (!m_target.expired())
		{
			rayInfo.m_pos = m_pos + Math::Vector3(0, 0.5f, 0);
			rayInfo.m_dir = (m_target.lock()->GetPos() + Math::Vector3(0, 1.2f, 0)) - (m_pos + Math::Vector3(0, 0.5f, 0));
			rayInfo.m_range = rayInfo.m_dir.Length();
			rayInfo.m_dir.Normalize();
			/*rayInfo.m_range = 0.5f;
			rayInfo.m_dir = m_grassHopperDashDir;*/
			rayInfo.m_type = KdCollider::Type::TypeGround;

			retRayList.clear();

			for (auto& obj : SceneManager::Instance().GetObjList())
			{
				obj->Intersects(rayInfo, &retRayList);
			}

			maxOverLap = 0;
			hit = false;
			Math::Vector3 hitPos;
			for (auto& ret : retRayList)
			{
				if (maxOverLap < ret.m_overlapDistance)
				{
					maxOverLap = ret.m_overlapDistance;
					hitPos = ret.m_hitPos;
					hit = true;
				}
			}

			if (hit)
			{
				m_bEnemyBetweenPlayer = true;
				m_enemyBetweenPlayerHitPos = hitPos;
			}
		}


		if (!(m_EnemyState & cutRaiseHit) && !(m_enemyType & bossEnemyTypeOne))
		{
			// 仲間のリストをつくって仲間に重ならないようにする
			sphereInfo.m_sphere.Center = m_pos + Math::Vector3(0, 2.5f, 0);
			sphereInfo.m_sphere.Radius = 0.65f;
			// 当たり判定をしたいタイプを設定
			sphereInfo.m_type = KdCollider::TypeBump;

#ifdef _DEBUG
			// デバック用
			m_pDebugWire->AddDebugSphere
			(
				sphereInfo.m_sphere.Center,
				sphereInfo.m_sphere.Radius
			);
#endif

			// 球の当たったオブジェクト情報
			retSphereList.clear();

			// 球と当たり判定 	
			for (auto& obj : SceneManager::Instance().GetObjList())
			{
				if (!obj->GetBEnemy())continue;
				obj->Intersects
				(
					sphereInfo,
					&retSphereList
				);
			}

			// 球に当たったリスト情報から一番近いオブジェクトを検出
			maxOverLap = 0;
			hit = false;
			hitDir = {}; // 当たった方向
			for (auto& ret : retSphereList)
			{
				// 一番近くで当たったものを探す
				if (maxOverLap < ret.m_overlapDistance)
				{
					maxOverLap = ret.m_overlapDistance;
					if (maxOverLap <= 0.1f)
					{
						hit = true;
						hitDir = ret.m_hitDir;
					}
				}
			}

			if (hit)
			{
				hitDir.y = 0.0f;
				hitDir.Normalize();
				// 球とモデルが当たっている
				m_pos += (hitDir * maxOverLap);
			}
		}

		if (m_enemyType & (bossEnemyTypeOne) && !(m_notHumanoidEnemyState & rotationAttack))
		{
			sphereInfo.m_sphere.Center = m_pos + Math::Vector3(0, 12, 0);
			sphereInfo.m_sphere.Radius = m_addRotationAttackDistToPlayerRadius;
			// 当たり判定をしたいタイプを設定
			sphereInfo.m_type = KdCollider::TypeDamage;

#ifdef _DEBUG
			// デバック用
			m_pDebugWire->AddDebugSphere
			(
				sphereInfo.m_sphere.Center,
				sphereInfo.m_sphere.Radius,
				{0,0,0,1}
			);
#endif

			// 球の当たったオブジェクト情報
			retSphereList.clear();

			// 球と当たり判定 

			if (!m_target.expired())
			{
				if (!m_target.lock()->GetBPlayerLose())
				{
					m_target.lock()->Intersects
					(
						sphereInfo,
						&retSphereList
					);
				}


				// 球に当たったリスト情報から一番近いオブジェクトを検出
				maxOverLap = 0;
				hit = false;
				hitDir = {}; // 当たった方向
				for (auto& ret : retSphereList)
				{
					// 一番近くで当たったものを探す
					if (maxOverLap < ret.m_overlapDistance)
					{
						maxOverLap = ret.m_overlapDistance;
						hit = true;
						hitDir = ret.m_hitDir;
					}
				}

				if (hit)
				{
					if (!m_bEnemyDeath && !(m_notHumanoidEnemyState & weaknessHit))
					{
						m_rotationAttackDistToPlayerTimeInitTime = 0;
						m_addRotationAttackDistToPlayerTime++;
						if (m_addRotationAttackDistToPlayerTime > ADDROTAYIONATTACKDISTTOPLAYERTIME)
						{
							m_addRotationAttackDistToPlayerTime = ADDROTAYIONATTACKDISTTOPLAYERTIME;
						}
					}
				}
				else
				{
					++m_rotationAttackDistToPlayerTimeInitTime;
					if (m_rotationAttackDistToPlayerTimeInitTime > ROTAYIONATTACKDISTTOPLAYERINITTIME)
					{
						m_rotationAttackDistToPlayerTimeInitTime = 0;
						m_addRotationAttackDistToPlayerTime = 0;
					}
				}
			}
		}
	}
	else
	{
		// レイ判定用に変数を作成
		KdCollider::RayInfo rayInfo;
		//rayInfo.m_pos = m_pos;
		rayInfo.m_pos = m_pos;
		rayInfo.m_pos.y += static_cast<float>(m_mpObj["AddBottomYVal"].number_value());
		if (!(m_EnemyState & (grassHopperDash | grassHopperDashUp)))
		{
			rayInfo.m_dir = { 0,-1,0 };
			static float enableStepHight = static_cast<float>(m_mpObj["EnableStepHight"].number_value());
			rayInfo.m_pos.y += enableStepHight;
			rayInfo.m_range = m_gravity + enableStepHight;
		}
		else
		{
			rayInfo.m_dir = m_grassHopperDashDir;
		}

		rayInfo.m_type = KdCollider::TypeGround;
#ifdef _DEBUG
		m_pDebugWire->AddDebugLine(rayInfo.m_pos, rayInfo.m_dir, rayInfo.m_range, { 1,1,1,1 });
#endif
		std::list<KdCollider::CollisionResult> retRayList;

		for (auto& obj : SceneManager::Instance().GetObjList())
		{
			obj->Intersects
			(
				rayInfo,
				&retRayList
			);
		}

		// レイに当たったリストから一番近いオブジェクトを検出
		float maxOverLap = 0;
		Math::Vector3 groundPos = {};
		bool hit = false;
		for (auto& ret : retRayList)
		{
			// レイを遮断しオーバーした長さ
			// 一番長いものを探す
			if (maxOverLap < ret.m_overlapDistance)
			{
				maxOverLap = ret.m_overlapDistance;
				groundPos = ret.m_hitPos;
				hit = true;
			}
		}

		if (hit)
		{
			//m_pos = groundPos;
			if (!(m_EnemyState & (grassHopperDash | grassHopperDashUp)))
			{
				//m_pos = groundPos;
				m_pos = groundPos + Math::Vector3(0, -static_cast<float>(m_mpObj["AddBottomYVal"].number_value()), 0);
				m_gravity = 0;

				if (m_EnemyState & (fall | jump) && !m_bEnemyDeath)
				{
					m_bMove = false;
					if (!(m_EnemyState & idle))
					{
						m_animator->SetAnimation(m_model->GetAnimation("IdleA"), false);
					}
					m_EnemyState = idle;
					m_wantToMoveState = wNone;
				}
			}
			else
			{
				m_gravity = 0;
				m_EnemyState = fall;
				m_rGrassHopperTime = 0;
				m_lGrassHopperTime = 0;
				m_bMove = false;
				m_grassHopperDashDir = {};
				m_dashSpd = 0.0f;
			}
		}

		Math::Vector3 addCenterVal = { static_cast<float>(m_mpObj["AddCenterVal"][0].number_value()),
									   static_cast<float>(m_mpObj["AddCenterVal"][1].number_value()),
									   static_cast<float>(m_mpObj["AddCenterVal"][2].number_value()) };
		KdCollider::SphereInfo sphereInfo;
		// 球の中心位置を設定
		sphereInfo.m_sphere.Center = m_pos + addCenterVal;
		if (!(m_EnemyState & (grassHopperDash | grassHopperDashUp | step)))
		{
			sphereInfo.m_sphere.Radius = static_cast<float>(m_mpObj["TypeGroundToHitSphereRadius"].number_value());
		}
		else
		{
			sphereInfo.m_sphere.Radius = static_cast<float>(m_mpObj["TypeGroundToHitSphereRadiusIsGrassDash"].number_value());
		}

		// 当たり判定をしたいタイプを設定
		sphereInfo.m_type = KdCollider::TypeGround /*| KdCollider::TypeBump*/;

#ifdef _DEBUG
		// デバック用
		m_pDebugWire->AddDebugSphere
		(
			sphereInfo.m_sphere.Center,
			sphereInfo.m_sphere.Radius
		);
#endif

		// 球の当たったオブジェクト情報
		std::list<KdCollider::CollisionResult> retSphereList;

		// 球と当たり判定 
		for (auto& obj : SceneManager::Instance().GetObjList())
		{
			obj->Intersects
			(
				sphereInfo,
				&retSphereList
			);
		}

		// 球に当たったリスト情報から一番近いオブジェクトを検出
		maxOverLap = 0;
		hit = false;
		Math::Vector3 hitDir = {}; // 当たった方向
		for (auto& ret : retSphereList)
		{
			// 一番近くで当たったものを探す
			if (maxOverLap < ret.m_overlapDistance)
			{
				maxOverLap = ret.m_overlapDistance;
				hit = true;
				hitDir = ret.m_hitDir;
			}
		}

		if (hit)
		{
			// 球とモデルが当たっている
			m_pos += (hitDir * maxOverLap);
		}

		if (!m_target.expired() && m_target.lock()->GetPlayerState() & (Player::PlayerState::defense))
		{
			if (!(m_EnemyState & (grassHopperDash | grassHopperDashUp | step)) || !(m_wantToMoveState & (wEscape | wDashAttack | wGrassDash | wAvoidance)))
			{
				sphereInfo.m_sphere.Radius = static_cast<float>(m_mpObj["TypeGardToHitSphereRadius"].number_value());
			}
			else
			{
				sphereInfo.m_sphere.Radius = static_cast<float>(m_mpObj["TypeGardToHitSphereRadiusIsGrassDash"].number_value());
			}

			// 当たり判定をしたいタイプを設定
			sphereInfo.m_type = KdCollider::TypeGard /*| KdCollider::TypeBump*/;

#ifdef _DEBUG
			// デバック用
			m_pDebugWire->AddDebugSphere
			(
				sphereInfo.m_sphere.Center,
				sphereInfo.m_sphere.Radius
			);
#endif

			// 球の当たったオブジェクト情報
			retSphereList.clear();

			// 球と当たり判定 
			for (auto& obj : m_target.lock()->GetWeaponList())
			{
				obj->Intersects
				(
					sphereInfo,
					&retSphereList
				);
			}

			// 球に当たったリスト情報から一番近いオブジェクトを検出
			maxOverLap = 0;
			hit = false;
			hitDir = {}; // 当たった方向
			for (auto& ret : retSphereList)
			{
				// 一番近くで当たったものを探す
				if (maxOverLap < ret.m_overlapDistance)
				{
					maxOverLap = ret.m_overlapDistance;
					hit = true;
					hitDir = ret.m_hitDir;
				}
			}

			if (hit)
			{
				hitDir.y = 0.0f;
				hitDir.Normalize();
				// 球とモデルが当たっている
				m_pos += (hitDir * maxOverLap);
			}
		}

		// 当たり判定をしたいタイプを設定
		sphereInfo.m_type = KdCollider::TypeBump;

#ifdef _DEBUG
		// デバック用
		m_pDebugWire->AddDebugSphere
		(
			sphereInfo.m_sphere.Center,
			sphereInfo.m_sphere.Radius
		);
#endif

		// 球の当たったオブジェクト情報
		retSphereList.clear();
		sphereInfo.m_sphere.Radius = static_cast<float>(m_mpObj["TypeBumpToHitSphereRadius"].number_value());
		// 球と当たり判定 

		if (!m_target.expired())
		{
			if (!m_target.lock()->GetBPlayerLose() && !(m_target.lock()->GetPlayerState() & Player::PlayerState::cutRaiseHit))
			{
				m_target.lock()->Intersects
				(
					sphereInfo,
					&retSphereList
				);
			}


			// 球に当たったリスト情報から一番近いオブジェクトを検出
			maxOverLap = 0;
			hit = false;
			hitDir = {}; // 当たった方向
			for (auto& ret : retSphereList)
			{
				// 一番近くで当たったものを探す
				if (maxOverLap < ret.m_overlapDistance)
				{
					maxOverLap = ret.m_overlapDistance;
					hit = true;
					hitDir = ret.m_hitDir;
				}
			}

			if (hit)
			{
				hitDir.y = 0.0f;
				hitDir.Normalize();
				// 球とモデルが当たっている
				m_pos += (hitDir * maxOverLap);
			}
		}

		if (!(m_EnemyState & cutRaiseHit))
		{
			// 仲間のリストをつくって仲間に重ならないようにする
			sphereInfo.m_sphere.Center = m_pos + addCenterVal;
			sphereInfo.m_sphere.Radius = static_cast<float>(m_mpObj["TypeBumpToHitSphereRadius"].number_value());
			// 当たり判定をしたいタイプを設定
			sphereInfo.m_type = KdCollider::TypeBump;

#ifdef _DEBUG
			// デバック用
			m_pDebugWire->AddDebugSphere
			(
				sphereInfo.m_sphere.Center,
				sphereInfo.m_sphere.Radius
			);
#endif

			// 球の当たったオブジェクト情報
			retSphereList.clear();

			// 球と当たり判定 	
			for (auto& obj : SceneManager::Instance().GetObjList())
			{
				if (!obj->GetBEnemy())continue;
				obj->Intersects
				(
					sphereInfo,
					&retSphereList
				);
			}

			// 球に当たったリスト情報から一番近いオブジェクトを検出
			maxOverLap = 0;
			hit = false;
			hitDir = {}; // 当たった方向
			for (auto& ret : retSphereList)
			{
				// 一番近くで当たったものを探す
				if (maxOverLap < ret.m_overlapDistance)
				{
					maxOverLap = ret.m_overlapDistance;
					if (maxOverLap <= 0.1f)
					{
						hit = true;
						hitDir = ret.m_hitDir;
					}
				}
			}

			if (hit)
			{
				hitDir.y = 0.0f;
				hitDir.Normalize();
				// 球とモデルが当たっている
				m_pos += (hitDir * maxOverLap);
			}
		}

		if (m_EnemyState & (rlAttack | rlAttackRush | EnemyState::hit))
		{
			sphereInfo;
			// 球の中心位置を設定
			sphereInfo.m_sphere.Center = m_pos + addCenterVal;
			// 球の半径を設定

			sphereInfo.m_sphere.Radius = static_cast<float>(m_mpObj["TypeBuriedToHitSphereRadius"].number_value());

			// 当たり判定をしたいタイプを設定
			sphereInfo.m_type = KdCollider::TypeBuried;
#ifdef _DEBUG
			// デバック用
			m_pDebugWire->AddDebugSphere
			(
				sphereInfo.m_sphere.Center,
				sphereInfo.m_sphere.Radius
			);
#endif
			// 球の当たったオブジェクト情報
			retSphereList.clear();

			// 球と当たり判定 
			for (auto& obj : SceneManager::Instance().GetObjList())
			{
				obj->Intersects
				(
					sphereInfo,
					&retSphereList
				);
			}

			// 球に当たったリスト情報から一番近いオブジェクトを検出
			maxOverLap = 0;
			hit = false;
			hitDir = {}; // 当たった方向
			for (auto& ret : retSphereList)
			{
				// 一番近くで当たったものを探す
				if (maxOverLap < ret.m_overlapDistance)
				{
					maxOverLap = ret.m_overlapDistance;
					hit = true;
					hitDir = ret.m_hitDir;
				}
			}

			if (hit)
			{
				hitDir.y = 0.0f;
				hitDir.Normalize();
				// 球とモデルが当たっている
				m_pos += (hitDir * maxOverLap);

				m_attackMoveSpd = 0.0f;
				m_hitMoveSpd = 0.0f;
			}
		}

		if (!m_target.expired())
		{
			rayInfo.m_pos = m_pos + addCenterVal;
			rayInfo.m_dir = (m_target.lock()->GetPos() + Math::Vector3(0, 1.5f, 0)) - (m_pos + addCenterVal);
			rayInfo.m_range = rayInfo.m_dir.Length();
			rayInfo.m_dir.Normalize();
			/*rayInfo.m_range = 0.5f;
			rayInfo.m_dir = m_grassHopperDashDir;*/
			rayInfo.m_type = KdCollider::Type::TypeGround;

			retRayList.clear();

			for (auto& obj : SceneManager::Instance().GetObjList())
			{
				obj->Intersects(rayInfo, &retRayList);
			}

			maxOverLap = 0;
			hit = false;
			Math::Vector3 hitPos;
			for (auto& ret : retRayList)
			{
				if (maxOverLap < ret.m_overlapDistance)
				{
					maxOverLap = ret.m_overlapDistance;
					hitPos = ret.m_hitPos;
					hit = true;
				}
			}

			if (hit /*&& rayInfo.m_range <= 30.0f*/)
			{
				m_bEnemyBetweenPlayer = true;
				m_enemyBetweenPlayerHitPos = hitPos;
			}
			else
			{
				m_bEnemyBetweenPlayer = false;

				if (m_EnemyState & (grassHopperDashL | grassHopperDashR))
				{
					m_wantToMoveState = Enemy::WantToMoveState::wDashAttack;
					m_dashSpd = 0.0f;
					m_grassHopperDashDir = {};
					m_gravity = 0;
					m_rGrassHopperPauCnt = 0;
					m_lGrassHopperPauCnt = 0;
					GrassMoveVecDecision();
				}
			}

		}
	}
}

void Enemy::BossUpdate()
{
	if (m_bFirstUpdate)
	{
		m_mpObj = (*m_wpJsonObj.lock())["BossEnemy"].object_items();

		std::shared_ptr<Scopion> sco;
		sco = std::make_shared<Scopion>();
		sco->SetArrmType(rArrm);
		sco->SetbEnemyWeapon();
		sco->SetTarget(m_target.lock());
		m_weaponList.push_back(sco);

		sco = std::make_shared<Scopion>();
		sco->SetArrmType(lArrm);
		sco->SetbEnemyWeapon();
		sco->SetTarget(m_target.lock());
		m_weaponList.push_back(sco);

		std::shared_ptr<Hopper> hopper;
		hopper = std::make_shared<Hopper>();
		hopper->SetArrmType(rArrm);
		hopper->SetbEnemyWeapon();
		m_weaponList.push_back(hopper);

		hopper = std::make_shared<Hopper>();
		hopper->SetArrmType(lArrm);
		hopper->SetbEnemyWeapon();
		m_weaponList.push_back(hopper);

		for (auto& WeaList : m_weaponList)
		{
			WeaList->SetOwner(shared_from_this());
		}

		if (SceneManager::Instance().GetSceneType() == SceneManager::SceneType::challenge)
		{
			m_vForce = (float)m_mpObj["ChallengeVforce"].number_value();
			m_endurance = (float)m_mpObj["ChallengeEndurance"].number_value();

			std::random_device rnd;
			std::mt19937 mt(rnd());
			std::uniform_int_distribution<int> intRand(0, 999);
			int randNum[4] = {};
			int rand = intRand(mt);
			randNum[0] = 150;
			randNum[1] = 300;
			randNum[2] = 300;
			randNum[3] = 250;

			for (int i = 0; i < 4; i++)
			{
				rand -= randNum[i];
				if (rand < 0)
				{
					switch (i)
					{
					case 0:
						m_enemyType = Enemy::EnemyType::allRounder;
						break;
					case 1:
						m_enemyType = Enemy::EnemyType::striker;
						break;
					case 2:
						m_enemyType = Enemy::EnemyType::defender;
						break;
					case 3:
						m_enemyType = Enemy::EnemyType::speedSter;
						break;
					}
					break;
				}
			}
		}
		else
		{
			m_vForce = (float)m_mpObj["Vforce"].number_value();
			m_endurance = (float)m_mpObj["Endurance"].number_value();
			m_enemyType = Enemy::EnemyType::allRounder;
		}
		return;
	}

	if (!(m_EnemyState & (hit | rise)))
	{
		if (SceneManager::Instance().GetSceneType() != SceneManager::SceneType::tutorial)
		{
			std::shared_ptr<Player> spTarget = m_target.lock();
			Math::Vector3 vTarget = Math::Vector3::Zero;
			if (spTarget)
			{
				vTarget = spTarget->GetPos() - m_pos;
				UpdateRotate(vTarget);
			}


			if (!(m_EnemyState & (fall | jump)) && !m_bEnemyDeath)
			{
				m_bMove = false;
			}

			if (m_wantToMoveState & wNone && !m_bEnemyDeath)
			{
				Brain();
			}
			else if (!m_bEnemyDeath)
			{
				if (vTarget.Length() >= static_cast<float>(m_mpObj["PlayerToEnemyDistBrain"].number_value()) && !(m_EnemyState & (grassHopperDash & ~grassHopperDashB) | grassHopperDashUp))
				{
					Brain();
				}
			}

			if (m_mpObj["RightWeaponScopionNum"].int_value() == m_rightWeaponNumber)
			{
				m_weaponType |= eScorpion;
				m_weaponType &= ~eGrassHopper;
			}
			else if (m_mpObj["RightWeaponGrassHopperNum"].int_value() == m_rightWeaponNumber)
			{
				m_weaponType |= eGrassHopper;
				m_weaponType &= ~eScorpion;
			}

			if (m_mpObj["LeftWeaponScopionNum"].int_value() == m_leftWeaponNumber)
			{
				m_weaponType |= eLScorpion;
				m_weaponType &= ~eLGrassHopper;
			}
			else if (m_mpObj["LeftWeaponGrassHopperNum"].int_value() == m_leftWeaponNumber)
			{
				m_weaponType |= eLGrassHopper;
				m_weaponType &= ~eLScorpion;
			}

			if (!(m_wantToMoveState & WantToMoveState::wNone))
			{
				if (!(m_EnemyState & hasDefense))
				{
					switch (m_wantToMoveState)
					{
					case WantToMoveState::wAttack:
						ScorpionAttackDecision();
						break;
					case WantToMoveState::wEscape:
						GrassMoveVecDecision();
						break;
					case WantToMoveState::wDefense:
						if (m_EnemyState & defense)
						{
							ScorpionDefenseMove();
						}
						else
						{
							ScorpionDefenseDecision();
						}

						break;
					case WantToMoveState::wDashAttack:
						if (m_lGrassHopperTime <= m_mpObj["DashAttackPossibleTime"].int_value() ||
							m_rGrassHopperTime <= m_mpObj["DashAttackPossibleTime"].int_value())
						{
							if (m_dashSpd == 1.2f)
							{
								if (vTarget.Length() <= 15)
								{
									if (m_weaponType & eGrassHopper)
									{
										if (m_EnemyState & grassHopperDashF)
										{
											ScorpionAttackDecision();
										}
										else
										{
											GrassMoveVecDecision();
										}
									}
									else if (m_weaponType & eLGrassHopper)
									{
										if (m_EnemyState & grassHopperDashF)
										{
											ScorpionAttackDecision();
										}
										else
										{
											GrassMoveVecDecision();
										}
									}
								}
								else
								{
									GrassMoveVecDecision();
								}
							}
							else if (m_dashSpd == 0.8f)
							{
								if (vTarget.Length() <= 9.25f)
								{
									if (m_weaponType & eGrassHopper)
									{
										if (m_EnemyState & grassHopperDashF)
										{
											ScorpionAttackDecision();
										}
										else
										{
											GrassMoveVecDecision();
										}
									}
									else if (m_weaponType & eLGrassHopper)
									{
										if (m_EnemyState & grassHopperDashF)
										{
											ScorpionAttackDecision();
										}
										else
										{
											GrassMoveVecDecision();
										}
									}
								}
								else
								{
									GrassMoveVecDecision();
								}
							}
							else if (m_dashSpd == 0.5f)
							{
								if (vTarget.Length() <= 8.0f)
								{
									if (m_weaponType & eGrassHopper)
									{
										if (m_EnemyState & grassHopperDashF)
										{
											ScorpionAttackDecision();
										}
										else
										{
											GrassMoveVecDecision();
										}
									}
									else if (m_weaponType & eLGrassHopper)
									{
										if (m_EnemyState & grassHopperDashF)
										{
											ScorpionAttackDecision();
										}
										else
										{
											GrassMoveVecDecision();
										}
									}
								}
								else
								{
									GrassMoveVecDecision();
								}
							}
							else if (m_dashSpd == 0.4f || m_dashSpd == 0.35f)
							{
								if (vTarget.Length() <= 4.0f)
								{
									if (m_weaponType & eGrassHopper)
									{
										if ((m_EnemyState & grassHopperDashF) && (m_rGrassHopperTime <= 80))
										{
											ScorpionAttackDecision();
										}
										else
										{
											GrassMoveVecDecision();
										}
									}
									else if (m_weaponType & eLGrassHopper)
									{
										if ((m_EnemyState & grassHopperDashF) && (m_lGrassHopperTime <= 80))
										{
											ScorpionAttackDecision();
										}
										else
										{
											GrassMoveVecDecision();
										}
									}
								}
								else
								{
									GrassMoveVecDecision();
								}
							}
							else if (m_dashSpd <= 0.25f)
							{
								if (vTarget.Length() <= 2.0f)
								{
									if (m_weaponType & eGrassHopper)
									{
										if (m_EnemyState & grassHopperDashF)
										{
											ScorpionAttackDecision();
										}
										else
										{
											GrassMoveVecDecision();
										}
									}
									else if (m_weaponType & eLGrassHopper)
									{
										if (m_EnemyState & grassHopperDashF)
										{
											ScorpionAttackDecision();
										}
										else
										{
											GrassMoveVecDecision();
										}
									}
								}
								else
								{
									GrassMoveVecDecision();
								}
							}
						}
						else
						{
							GrassMoveVecDecision();
						}
						break;
					case WantToMoveState::wRun:
						NormalMoveVecDecision();
						break;
					case WantToMoveState::wStep:
						if (!(m_EnemyState & step))
						{
							StepVecDecision();
						}
						break;
					case WantToMoveState::wGrassDash:
						GrassMoveVecDecision();
						break;
					case WantToMoveState::wAvoidance:
						GrassMoveVecDecision();
						break;
					}
				}

				if (!(m_EnemyState & (grassHopperDash | grassHopperDashUp | step)))
				{
					if (m_EnemyState & (lAttack | rAttack | rlAttack | rlAttackRush | mantis))
					{
						ScorpionAttackMove();
					}
					else if (m_EnemyState & run | m_EnemyState & jump)
					{
						NormalMove();
					}
					else if (m_EnemyState & hasDefense)
					{
						HasDefenseMove();
					}
				}
				else if (m_EnemyState & (grassHopperDash | grassHopperDashUp))
				{
					GrassMove();
				}
				else if (m_EnemyState & step)
				{
					StepMove();
				}
			}

			if (!m_bMove)
			{
				++m_idleCnt;

				if (m_idleCnt >= 30)
				{
					Brain();
				}

				if (!(m_EnemyState & (fall | jump)))
				{
					m_EnemyState = idle;
					m_animator->SetAnimation(m_model->GetAnimation("IdleA"));
				}
			}
		}
	}
}

void Enemy::BossEnemyTyepOneUpdate()
{
	if (m_bFirstUpdate)
	{
		m_mpObj = (*m_wpJsonObj.lock())["BossEnemyTypeOne"].object_items();
		m_weaponList.clear();
		m_weaponType |= eLWeaponNone | eWeaponNone;
		m_weaponType &= eLWeaponNone | eWeaponNone;

		m_leftWeaponNumber = 0;
		m_rightWeaponNumber = 0;

		m_vForce    = (float)m_mpObj["Vforce"].number_value();
		m_endurance = (float)m_mpObj["Endurance"].number_value();

		if (!(m_EnemyState & run))
		{
			m_animator->SetAnimation(m_model->GetAnimation("RUN"));
		}
		m_EnemyState = run;
		m_addRotationAttackDistToPlayerRadius = 42.5f;
		return;
	}

#ifdef _DEBUG
	const KdModelWork::Node* node = nullptr;
	Math::Matrix mat = Math::Matrix::Identity;

	node = m_model->FindNode("HitPoint");
	mat = node->m_worldTransform * m_mWorld;
	m_pDebugWire->AddDebugSphere
	(
		mat.Translation(),
		6.0f,
		{ 0,0,0,1 }
	);
#endif

	if (m_attackDelayTime < ATTACKDELAYTIME)
	{
		++m_attackDelayTime;
		return;
	}

	if (m_endurance <= 500 && m_addRotationAttackDistToPlayerRadius == 42.5f)
	{
		m_addRotationAttackDistToPlayerRadius = 90.0f;
	}

	if (m_hitStopCnt > 0)
	{
		--m_hitStopCnt;
	}

	if (m_hitStopCnt == 0)
	{
		if (m_addWeaknesSsuccessionHitCntTime > 0)
		{
			--m_addWeaknesSsuccessionHitCntTime;
		}

		if (m_bShotBeam || m_bShotEnergyBullet)
		{
			m_bMove = true;

			if (m_bShotBeam)
			{
				if (m_rangedAttackAnimeCnt >= 10 && !m_bBeamHitStart)
				{
					m_bBeamHitStart = true;
					KdAudioManager::Instance().Play3D("Asset/Audio/SE/Beem.wav", m_rangedAttackShotPos);
				}
			}

			if (m_bShotEnergyBullet)
			{
				if (m_rangedAttackAnimeCnt >= 26 && !m_bEnergyBulletHitStart)
				{
					m_bEnergyBulletHitStart = true;
					KdAudioManager::Instance().Play3D("Asset/Audio/SE/energyShot.wav", m_rangedAttackShotPos);
				}
			}
		}

		if (!(m_notHumanoidEnemyState & rotationAttack))
		{
			if (m_addRotationAttackDistToPlayerTime == ADDROTAYIONATTACKDISTTOPLAYERTIME)
			{
				if (!m_bEnemyDeath)
				{
					m_addRotationAttackDistToPlayerTime = 0;
					m_notHumanoidEnemyState = rotationAttack;
					m_animator->SetAnimation(m_model->GetAnimation("RotationAttack"), false);
				}
			}


			if (m_wantToMoveState & wNone && !m_bEnemyDeath && !m_bShotEnergyBullet && !m_bShotBeam)
			{
				BossEnemyTypeOneBrain();
			}

			if (!(m_wantToMoveState & WantToMoveState::wNone) && !m_bShotEnergyBullet && !m_bShotBeam)
			{
				switch (m_wantToMoveState)
				{
				case WantToMoveState::wRun:
					NormalMoveVecDecision();
					break;
				}

				if (m_EnemyState & run | m_EnemyState & jump)
				{
					NormalMove();
				}
			}
		}
		else
		{
			RotationAttackMove();
		}

		if (SceneManager::Instance().GetSceneType() != SceneManager::SceneType::tutorial && !m_bShotEnergyBullet && !m_bShotBeam)
		{
			std::shared_ptr<Player> spTarget = m_target.lock();
			if (spTarget)
			{
				Math::Vector3 vTarget = spTarget->GetPos() - m_pos;
				UpdateRotate(vTarget);
			}

		}
	}
}

void Enemy::CoarseFishEnemyUpdate()
{
	if (m_bFirstUpdate)
	{
		m_mpObj = (*m_wpJsonObj.lock())["CoarseFishEnemy"].object_items();
		m_weaponList.clear();
		std::shared_ptr<Scopion> sco;
		sco = std::make_shared<Scopion>();
		sco->SetArrmType(rArrm);
		sco->SetbEnemyWeapon();
		sco->SetTarget(m_target.lock());
		m_weaponList.push_back(sco);

		m_weaponType |= eLWeaponNone | eScorpion;
		m_weaponType &= eLWeaponNone | eScorpion;

		m_leftWeaponNumber = 0;
		m_rightWeaponNumber = 1;

		for (auto& WeaList : m_weaponList)
		{
			WeaList->SetOwner(shared_from_this());
		}

		m_vForce = (float)m_mpObj["Vforce"].number_value();
		m_endurance = (float)m_mpObj["Endurance"].number_value();

		m_enemyType = Enemy::EnemyType::coarseFishEnemy;
		
		return;
	}

	if (m_coarseFishEnemyAttackDelayCnt > 0)
	{
		--m_coarseFishEnemyAttackDelayCnt;
	}

	if (!(m_EnemyState & (hit | rise)))
	{
		if (SceneManager::Instance().GetSceneType() != SceneManager::SceneType::tutorial)
		{
			std::shared_ptr<Player> spTarget = m_target.lock();
			Math::Vector3 vTarget = Math::Vector3::Zero;
			if (spTarget)
			{
				vTarget = spTarget->GetPos() - m_pos;
				UpdateRotate(vTarget);
			}


			if (!(m_EnemyState & (fall | jump)) && !m_bEnemyDeath)
			{
				m_bMove = false;
			}

			if (m_wantToMoveState & wNone && !m_bEnemyDeath)
			{
				Brain();
			}
			else if (!m_bEnemyDeath)
			{
				if (vTarget.Length() >= 20.0f && !(m_EnemyState & (grassHopperDash & ~grassHopperDashB) | grassHopperDashUp))
				{
					Brain();
				}
			}

			if (!(m_wantToMoveState & WantToMoveState::wNone))
			{
				if (!(m_EnemyState & hasDefense))
				{
					switch (m_wantToMoveState)
					{
					case WantToMoveState::wAttack:
						ScorpionAttackDecision();
						break;
					case WantToMoveState::wRun:
						NormalMoveVecDecision();
						break;
					}
				}

				if (m_EnemyState & (lAttack | rAttack | rlAttack | rlAttackRush | mantis))
				{
					ScorpionAttackMove();
				}
				else if (m_EnemyState & run | m_EnemyState & jump)
				{
					NormalMove();
				}
			}

			if (!m_bMove)
			{
				if (!(m_EnemyState & (fall | jump)))
				{
					m_EnemyState = idle;
					m_animator->SetAnimation(m_model->GetAnimation("IdleA"));
				}
			}
		}
	}
}

void Enemy::WimpEnemyTypeOneUpdate()
{
	if (m_bFirstUpdate)
	{
		m_mpObj = (*m_wpJsonObj.lock())["WimpEnemyTypeOne"].object_items();

		m_weaponList.clear();
		m_weaponType |= eLWeaponNone | eWeaponNone;
		m_weaponType &= eLWeaponNone | eWeaponNone;

		m_leftWeaponNumber = 0;
		m_rightWeaponNumber = 0;

		m_vForce = (float)m_mpObj["Vforce"].number_value();
		m_endurance = (float)m_mpObj["Endurance"].number_value();

		m_animator->SetAnimation(m_model->GetAnimation("IdleA"));
		m_EnemyState = idle;

		return;
	}

#ifdef _DEBUG
	const KdModelWork::Node* node = nullptr;
	Math::Matrix mat = Math::Matrix::Identity;
	
	node = m_model->FindNode("HitPoint");
	mat = node->m_worldTransform * m_mWorld;
	m_pDebugWire->AddDebugSphere
	(
		{ mat._41, mat._42 , mat._43},
		0.325f,
		{ 0,0,0,1 }
	);
#endif

	if (m_attackDelayTime < ATTACKDELAYTIME)
	{
		++m_attackDelayTime;
		return;
	}

	if (m_bShotBeam || m_bShotEnergyBullet)
	{
		m_bMove = true;

		if (m_bShotBeam)
		{
			if (m_rangedAttackAnimeCnt >= 10 && !m_bBeamHitStart)
			{
				m_bBeamHitStart = true;
				KdAudioManager::Instance().Play3D("Asset/Audio/SE/Beem.wav", m_rangedAttackShotPos);
			}
		}

		if (m_bShotEnergyBullet)
		{
			if (m_rangedAttackAnimeCnt >= 26 && !m_bEnergyBulletHitStart)
			{
				m_bEnergyBulletHitStart = true;
				KdAudioManager::Instance().Play3D("Asset/Audio/SE/energyShot.wav", m_rangedAttackShotPos);
			}
		}
	}

	if (m_wantToMoveState & wNone && !m_bEnemyDeath && !m_bShotEnergyBullet && !m_bShotBeam)
	{
		Brain();
	}

	if (!(m_wantToMoveState & WantToMoveState::wNone) && !m_bShotEnergyBullet && !m_bShotBeam)
	{
		switch (m_wantToMoveState)
		{
		case WantToMoveState::wRun:
			NormalMoveVecDecision();
			break;
		}

		if (m_EnemyState & run | m_EnemyState & jump)
		{
			NormalMove();
		}
	}

	if (SceneManager::Instance().GetSceneType() != SceneManager::SceneType::tutorial && !m_bShotEnergyBullet && !m_bShotBeam)
	{
		std::shared_ptr<Player> spTarget = m_target.lock();
		if (spTarget)
		{
			Math::Vector3 vTarget = spTarget->GetPos() - m_pos;
			UpdateRotate(vTarget);
		}

	}
}

void Enemy::TutorialUpdate()
{

}

void Enemy::PostUpdate()
{
	if (!m_animator) return;

	if (!m_bEnemyDeath)
	{
		if (m_gravity > 0)
		{
			if (!(m_EnemyState & (hit | hasDefense)))
			{
				if (!(m_EnemyState & fall))
				{
					m_animator->SetAnimation(m_model->GetAnimation("FallA"), false);
				}
				m_EnemyState = fall;
			}
		}

		if (m_bShotBeam || m_bShotEnergyBullet)
		{
			m_rangedAttackAnimeCnt++;
		}
		
		if (m_bShotBeam)
		{
			if (m_rangedAttackAnimeCnt >= 66)
			{
				m_beamRange = 0;
				m_rangedAttackAnimeCnt = 0;
				m_bShotEnergyBullet = false;
				m_bShotBeam = false;
				m_bBeamHitStart = false;
				m_bEnergyBulletHitStart = false;
				m_bMove = false;
				m_bRangedAttack = false;
				m_wantToMoveState = wNone;
			}
		}
		else if (m_bShotEnergyBullet)
		{
			if (m_enemyType & bossEnemyTypeOne)
			{
				if (m_rangedAttackAnimeCnt >= 140)
				{
					m_beamRange = 0;
					m_rangedAttackAnimeCnt = 0;
					m_bShotEnergyBullet = false;
					m_bShotBeam = false;
					m_bBeamHitStart = false;
					m_bEnergyBulletHitStart = false;
					m_bMove = false;
					m_bRangedAttack = false;
					m_wantToMoveState = wNone;
				}
			}
			else
			{
				if (m_rangedAttackAnimeCnt >= 125)
				{
					m_beamRange = 0;
					m_rangedAttackAnimeCnt = 0;
					m_bShotEnergyBullet = false;
					m_bShotBeam = false;
					m_bBeamHitStart = false;
					m_bEnergyBulletHitStart = false;
					m_bMove = false;
					m_bRangedAttack = false;
					m_wantToMoveState = wNone;
				}
			}
		}

		if (m_rGrassHopperPauCnt > 0)
		{
			--m_rGrassHopperPauCnt;
		}

		if (m_lGrassHopperPauCnt > 0)
		{
			--m_lGrassHopperPauCnt;
		}

		if (m_grassSuccessionDelayCnt > 0)
		{
			--m_grassSuccessionDelayCnt;
		}

		if (m_bAttackAnimeDelay)
		{
			m_attackAnimeDelayCnt--;
			if (m_attackAnimeDelayCnt <= 0)
			{
				m_bAttackAnimeDelay = false;
				m_attackAnimeDelayCnt = 0;
			}
		}

		if (!(m_EnemyState & (lAttack | rAttack | rlAttack | rlAttackRush)) && !(m_notHumanoidEnemyState & rotationAttack))
		{
			m_animator->AdvanceTime(m_model->WorkNodes());
			m_model->CalcNodeMatrices();

			if (m_EnemyState & run)
			{
				++m_runAnimeCnt;
				if (m_runAnimeCnt == 16)
				{
					KdAudioManager::Instance().Play3D("Asset/Audio/SE/FootstepsConcrete2.wav", m_pos);
				}
				else if (m_runAnimeCnt == 31)
				{
					KdAudioManager::Instance().Play3D("Asset/Audio/SE/FootstepsConcrete2.wav", m_pos);
				}

				if (m_runAnimeCnt == 40)
				{
					m_runAnimeCnt = 0;
				}
			}
		}
		else if (m_EnemyState & (lAttack | rAttack) && !m_bAttackAnimeDelay)
		{
			if (m_bAttackAnimeCnt)
			{
				m_attackAnimeCnt++;
				if (m_EnemyState & (lAttackOne | lAttackTwo | rAttackOne | rAttackTwo))
				{
					if (m_attackAnimeCnt == 10)
					{
						KdAudioManager::Instance().Play("Asset/Audio/SE/Swishes - SWSH 40, Swish, Combat, Weapon, Light.wav");
					}
				}

				if (m_attackAnimeCnt == 20)
				{
					m_bAttackAnimeDelay = true;
					m_bAttackAnimeCnt = false;
					m_attackAnimeCnt = 0;
					m_attackAnimeDelayCnt = 10;
				}
			}
			else
			{
				m_attackAnimeCnt++;
				if (m_EnemyState & (lAttackThree | rAttackThree))
				{
					if (m_attackAnimeCnt == 15)
					{
						KdAudioManager::Instance().Play("Asset/Audio/SE/Swishes - SWSH 40, Swish, Combat, Weapon, Light.wav");
					}
				}
			}

			m_animator->AdvanceTime(m_model->WorkNodes());
			m_model->CalcNodeMatrices();
		}
		else if (m_EnemyState & (rlAttack | rlAttackRush))
		{
			if (m_bAttackAnimeCnt)
			{
				m_attackAnimeCnt++;
				if (m_EnemyState & rlAttackOne)
				{
					if (m_attackAnimeCnt == 13 || m_attackAnimeCnt == 17)
					{
						KdAudioManager::Instance().Play("Asset/Audio/SE/Swishes - SWSH 40, Swish, Combat, Weapon, Light.wav");
					}
				}
				else if (m_EnemyState & rlAttackTwo)
				{
					if (m_attackAnimeCnt == 13 || m_attackAnimeCnt == 16)
					{
						KdAudioManager::Instance().Play("Asset/Audio/SE/Swishes - SWSH 40, Swish, Combat, Weapon, Light.wav");
					}
				}
				else if (m_EnemyState & rlAttackThree)
				{
					if (m_attackAnimeCnt == 15 || m_attackAnimeCnt == 23)
					{
						KdAudioManager::Instance().Play("Asset/Audio/SE/Swishes - SWSH 40, Swish, Combat, Weapon, Light.wav");
					}
				}
				else if (m_EnemyState & rlAttackRush)
				{
					if (m_attackAnimeCnt == 8 ||
						m_attackAnimeCnt == 21 ||
						m_attackAnimeCnt == 31 ||
						m_attackAnimeCnt == 49 ||
						m_attackAnimeCnt == 57 ||
						m_attackAnimeCnt == 74 ||
						m_attackAnimeCnt == 89)
					{
						KdAudioManager::Instance().Play("Asset/Audio/SE/Swishes - SWSH 40, Swish, Combat, Weapon, Light.wav");
					}
				}


				if (m_EnemyState & rlAttackOne)
				{
					if (m_attackAnimeCnt == 20)
					{
						m_bAttackAnimeDelay = true;
						m_bAttackAnimeCnt = false;
						m_attackAnimeCnt = 0;
						m_attackAnimeDelayCnt = 10;
					}
				}
				else if (m_EnemyState & rlAttackTwo)
				{
					if (m_attackAnimeCnt == 20)
					{
						m_bAttackAnimeDelay = true;
						m_bAttackAnimeCnt = false;
						m_attackAnimeCnt = 0;
						m_attackAnimeDelayCnt = 10;
					}
				}
				else if (m_EnemyState & rlAttackThree)
				{
					if (m_attackAnimeCnt == 40)
					{
						m_bAttackAnimeDelay = true;
						m_bAttackAnimeCnt = false;
						m_attackAnimeCnt = 0;
						m_attackAnimeDelayCnt = 10;
					}
				}
			}

			m_animator->AdvanceTime(m_model->WorkNodes());
			m_model->CalcNodeMatrices();
		}
		else if (m_notHumanoidEnemyState & rotationAttack)
		{
			++m_attackAnimeCnt;
			m_animator->AdvanceTime(m_model->WorkNodes());
			m_model->CalcNodeMatrices();
		}
	}
	else
	{
		m_animator->AdvanceTime(m_model->WorkNodes());
		m_model->CalcNodeMatrices();
		m_bMove = true;

		if (!KdEffekseerManager::GetInstance().IsPlaying("BailOutEnemy.efk"))
		{
			if (m_bEnemyLose)
			{
				m_isExpired = true;

				if (SceneManager::Instance().GetSceneType() == SceneManager::SceneType::challenge)
				{
					SceneManager::Instance().SubEnemyIeftover();
					SceneManager::Instance().SubEnemyDrawTotal();
				}
				else if (SceneManager::Instance().GetSceneType() == SceneManager::SceneType::battle || SceneManager::Instance().GetSceneType() == SceneManager::SceneType::training)
				{
					if (m_bBoss)
					{
						SceneManager::Instance().SubEnemyIeftover();
					}

					SceneManager::Instance().SubEnemyDrawTotal();
				}
			}
		}

		if (m_animator->IsAnimationEnd())
		{
			if (!m_bEnemyLose)
			{
				/*SceneManager::Instance().SubEnemyIeftover();
				if (SceneManager::Instance().GetSceneType() == SceneManager::SceneType::challenge)
				{
					SceneManager::Instance().SubEnemyDrawTotal();
				}*/

				m_bEnemyLose = true;
				KdEffekseerManager::GetInstance().
					Play("BailOutEnemy.efk", { m_pos.x,m_pos.y + 0.3f,m_pos.z });
				KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("BailOutEnemy.efk"); // これでループしない

				if (!m_bBoss)
				{
					if (m_enemyType & coarseFishEnemy)
					{
						Math::Matrix efcMat = Math::Matrix::CreateScale(0.15f) * Math::Matrix::CreateTranslation({ m_pos.x,m_pos.y + 0.9f,m_pos.z });
						KdEffekseerManager::GetInstance().SetWorldMatrix("BailOutEnemy.efk", efcMat);
					}
					else
					{
						Math::Matrix efcMat = Math::Matrix::CreateScale(0.35f) * Math::Matrix::CreateTranslation({ m_pos.x,m_pos.y + 0.3f,m_pos.z });
						KdEffekseerManager::GetInstance().SetWorldMatrix("BailOutEnemy.efk", efcMat);
					}
				}
				//m_isExpired = true;
			}
		}
	}
}

void Enemy::OnHit(Math::Vector3 a_KnocBackvec)
{
	m_EnemyState = nomalHit;
	if (m_target.lock()->GetPlayerState() & Player::PlayerState::rlAttackThree)
	{
		m_hitStopCnt = 60;
	}
	else
	{
		m_hitStopCnt = 40;
	}

	m_hitMoveSpd = 0.05f;
	m_hitColorChangeTimeCnt = (*m_wpJsonObj.lock())["HitColorChangeTimeCnt"].int_value();
	m_knockBackVec = a_KnocBackvec;
	m_endurance -= (*m_wpJsonObj.lock())["OnHitDamage"].int_value();
	m_attackHit = true;
	if (m_target.lock()->GetPlayerState() & (Player::PlayerState::rAttackOne | Player::PlayerState::rlAttackOne))
	{
		m_animator->SetAnimation(m_model->GetAnimation("RHit1"), false);
		if (SceneManager::Instance().GetUpdateStopCnt() == 0)
		{
			SceneManager::Instance().SetUpdateStopCnt(5); // これでアップデートを一時止める
		}
	}
	else if (m_target.lock()->GetPlayerState() & (Player::PlayerState::rAttackTwo | Player::PlayerState::rlAttackTwo))
	{
		m_animator->SetAnimation(m_model->GetAnimation("RHit2"), false);
		if (SceneManager::Instance().GetUpdateStopCnt() == 0)
		{
			SceneManager::Instance().SetUpdateStopCnt(5); // これでアップデートを一時止める
		}
	}
	else if (m_target.lock()->GetPlayerState() & (Player::PlayerState::lAttackOne | Player::PlayerState::rlAttackThree))
	{
		m_animator->SetAnimation(m_model->GetAnimation("LHit1"), false);
		if (SceneManager::Instance().GetUpdateStopCnt() == 0)
		{
			SceneManager::Instance().SetUpdateStopCnt(5); // これでアップデートを一時止める
		}
	}
	else if (m_target.lock()->GetPlayerState() & (Player::PlayerState::lAttackTwo))
	{
		m_animator->SetAnimation(m_model->GetAnimation("LHit2"), false);
		if (SceneManager::Instance().GetUpdateStopCnt() == 0)
		{
			SceneManager::Instance().SetUpdateStopCnt(5); // これでアップデートを一時止める
		}
	}

	if (!m_target.lock()->GetBRushRp())
	{
		if (m_target.lock()->GetPlayerState() & Player::PlayerState::rlAttackRush && m_target.lock()->GetAnimationCnt() < 8)
		{
			m_animator->SetAnimation(m_model->GetAnimation("LHit1"), false);
			if (SceneManager::Instance().GetUpdateStopCnt() == 0)
			{
				SceneManager::Instance().SetUpdateStopCnt(2); // これでアップデートを一時止める
			}
		}
		else if (m_target.lock()->GetPlayerState() & Player::PlayerState::rlAttackRush && (m_target.lock()->GetAnimationCnt() >= 21 && m_target.lock()->GetAnimationCnt() < 31))
		{
			m_animator->SetAnimation(m_model->GetAnimation("LHit2"), false);
			if (SceneManager::Instance().GetUpdateStopCnt() == 0)
			{
				SceneManager::Instance().SetUpdateStopCnt(2); // これでアップデートを一時止める
			}
		}
		else if (m_target.lock()->GetPlayerState() & Player::PlayerState::rlAttackRush && (m_target.lock()->GetAnimationCnt() >= 31 && m_target.lock()->GetAnimationCnt() < 41))
		{
			m_animator->SetAnimation(m_model->GetAnimation("RHit1"), false);
			if (SceneManager::Instance().GetUpdateStopCnt() == 0)
			{
				SceneManager::Instance().SetUpdateStopCnt(2); // これでアップデートを一時止める
			}
		}
		else if (m_target.lock()->GetPlayerState() & Player::PlayerState::rlAttackRush && (m_target.lock()->GetAnimationCnt() >= 8 && m_target.lock()->GetAnimationCnt() < 21))
		{
			m_animator->SetAnimation(m_model->GetAnimation("RHit2"), false);
			if (SceneManager::Instance().GetUpdateStopCnt() == 0)
			{
				SceneManager::Instance().SetUpdateStopCnt(2); // これでアップデートを一時止める
			}
		}
	}
	else
	{
		m_animator->SetAnimation(m_model->GetAnimation("RHit1"), false);
		if (m_target.lock()->GetPlayerState() & Player::PlayerState::rlAttackRush && m_target.lock()->GetAnimationCnt() < 93)
		{
			m_hitMoveSpd = 0.395f;
		}
		else
		{
			m_hitMoveSpd = 0.275f;
		}

		if (SceneManager::Instance().GetUpdateStopCnt() == 0)
		{
			SceneManager::Instance().SetUpdateStopCnt(2); // これでアップデートを一時止める
		}
	}

	if (m_graduallyTorionDecVal == 0)
	{
		m_graduallyTorionDecVal = 0.01f;
	}
	else
	{
		m_graduallyTorionDecVal *= 1.25f;
	}
}

void Enemy::BlowingAwayAttackOnHit(Math::Vector3 a_KnocBackvec)
{
	m_EnemyState = blowingAwayHit;
	m_hitStopCnt = 40;
	m_hitColorChangeTimeCnt = (*m_wpJsonObj.lock())["HitColorChangeTimeCnt"].int_value();
	if (m_target.lock()->GetPlayerState() & Player::PlayerState::rlAttackRush && m_target.lock()->GetAnimationCnt() >= 107)
	{
		m_hitMoveSpd = 0.95f;
		if (SceneManager::Instance().GetUpdateStopCnt() == 0)
		{
			SceneManager::Instance().SetUpdateStopCnt(25); // これでアップデートを一時止める
		}
		//SceneManager::Instance().SetUpdateStopCnt(10);   // 画面を揺らす時のHtiストップフレーム数
		//SceneManager::Instance().SetScreenVibFrames(15); // 画面を揺らすフレーム数

		if (!(m_enemyType & (bossEnemyTypeOne | humanBossEnemy)))
		{
			if (m_enemyType & wimpEnemyTypeOne)
			{
				m_endurance -= 50.0f;
			}
			else
			{
				m_bLethalDamageToKickOrPantciOrDashAttackHit = true;
				m_endurance -= 150.0f;
			}
		}
		else
		{
			m_endurance -= 30.0f;
			if (m_endurance <= 0)
			{
				m_bLethalDamageToKickOrPantciOrDashAttackHit = true;
			}
		}
	}
	else if (m_target.lock()->GetPlayerState() & (Player::PlayerState::rAttackOne | Player::PlayerState::lAttackOne) && m_target.lock()->GetPlayerState() & (Player::PlayerState::grassHopperDashF))
	{
		m_hitMoveSpd = 0.35f;
		if (SceneManager::Instance().GetUpdateStopCnt() == 0)
		{
			SceneManager::Instance().SetUpdateStopCnt(15); // これでアップデートを一時止める
		}

		if (!(m_enemyType & (bossEnemyTypeOne | humanBossEnemy)))
		{
			if (m_enemyType & wimpEnemyTypeOne)
			{
				m_endurance -= 50.0f;
			}
			else
			{
				m_bLethalDamageToKickOrPantciOrDashAttackHit = true;
				m_endurance -= 150.0f;
			}
		}
		else
		{
			m_endurance -= 30.0f;
			if (m_endurance <= 0)
			{
				m_bLethalDamageToKickOrPantciOrDashAttackHit = true;
			}
		}

	}
	else if (m_target.lock()->GetPlayerState() & (Player::PlayerState::rAttackThree | Player::PlayerState::lAttackThree))
	{
		m_hitMoveSpd = 0.3f;
		if (SceneManager::Instance().GetUpdateStopCnt() == 0)
		{
			SceneManager::Instance().SetUpdateStopCnt(8); // これでアップデートを一時止める
		}

		if (!(m_enemyType & (bossEnemyTypeOne | humanBossEnemy)))
		{
			if (m_enemyType & wimpEnemyTypeOne)
			{
				m_endurance -= 20.0f;
			}
			else
			{
				m_endurance -= 30.0f;
			}
		}
		else
		{
			m_endurance -= 15.0f;
		}
	}

	m_knockBackVec = a_KnocBackvec;
	m_attackHit = true;

	if (!(m_enemyType & (bossEnemyTypeOne | wimpEnemyTypeOne)))
	{
		Math::Vector3 nowVec = m_mWorld.Backward();

		Math::Vector3 dot = DirectX::XMVector3Dot(nowVec, a_KnocBackvec);
		if (dot.x > 1)
		{
			dot.x = 1;
		}
		if (dot.x < -1)
		{
			dot.x = -1;
		}

		float ang = DirectX::XMConvertToDegrees(acos(dot.x));

		if (ang > 90)
		{
			m_animator->SetAnimation(m_model->GetAnimation(" BlowingAwayHitB"), false);
		}
		else
		{
			m_animator->SetAnimation(m_model->GetAnimation("BlowingAwayHitBB"), false);
			m_bBlowingAwayHitB = true;
		}
		m_invincibilityTimeCnt = (*m_wpJsonObj.lock())["InvincibilityMaxTimeCnt"].int_value();
	}

	if (m_graduallyTorionDecVal == 0)
	{
		m_graduallyTorionDecVal = 0.01f;
	}
	else
	{
		m_graduallyTorionDecVal *= 1.45f;
	}

	if (SceneManager::Instance().GetSceneType() == SceneManager::SceneType::tutorial)
	{
		if (m_bLethalDamageToKickOrPantciOrDashAttackHit)
		{
			m_bLethalDamageToKickOrPantciOrDashAttackHit = false;
		}
	}
}

void Enemy::IaiKiriAttackOnHit(Math::Vector3 a_KnocBackvec)
{
	m_EnemyState = iaiKiriHit;
	m_hitStopCnt = 40;
	m_hitColorChangeTimeCnt = (*m_wpJsonObj.lock())["HitColorChangeTimeCnt"].int_value();
	m_hitMoveSpd = 0.0f;
	m_invincibilityTimeCnt = (*m_wpJsonObj.lock())["InvincibilityMaxTimeCnt"].int_value();
	m_knockBackVec = a_KnocBackvec;
	m_endurance -= 50.0f;
	m_attackHit = true;
	m_animator->SetAnimation(m_model->GetAnimation("IaiKiriAttackHitB"), false);
	if (SceneManager::Instance().GetUpdateStopCnt() == 0)
	{
		SceneManager::Instance().SetUpdateStopCnt(8); // これでアップデートを一時止める
	}

	if (m_graduallyTorionDecVal == 0)
	{
		m_graduallyTorionDecVal = 0.05f;
	}
	else
	{
		m_graduallyTorionDecVal *= 1.5f;
	}
}

void Enemy::CutRaiseOnHit(Math::Vector3 a_KnocBackvec)
{
	m_EnemyState = cutRaiseHit;
	m_hitStopCnt = 60;
	m_hitColorChangeTimeCnt = (*m_wpJsonObj.lock())["HitColorChangeTimeCnt"].int_value();
	m_hitMoveSpd = 0.0f;

	if (m_gravity == 0)
	{
		m_gravity -= 0.1f;
	}
	else if (m_gravity <= 0.03f)
	{
		m_gravity -= 0.04f;
	}
	else
	{
		m_gravity = 0;
		m_gravity = -0.05f;
	}
	m_knockBackVec = a_KnocBackvec;
	m_endurance -= 15.0f;
	if (m_enemyType & humanBossEnemy)
	{
		if (m_endurance <= 0)
		{
			m_endurance = 0.1f;
		}
	}

	m_attackHit = true;
	m_animator->SetAnimation(m_model->GetAnimation("CutRaiseHit"), false);

	if (SceneManager::Instance().GetUpdateStopCnt() == 0)
	{
		SceneManager::Instance().SetUpdateStopCnt(8); // これでアップデートを一時止める
	}

	if (m_graduallyTorionDecVal == 0)
	{
		m_graduallyTorionDecVal = 0.01f;
	}
	else
	{
		m_graduallyTorionDecVal *= 1.15f;
	}
}

void Enemy::WeaknessOnHit()
{
	SceneManager::Instance().SetUpdateStopCnt(15); // これでアップデートを一時止める

	if (m_enemyType & bossEnemyTypeOne)
	{
		if (m_addWeaknesSsuccessionHitCntTime == 0)
		{
			m_weaknesSsuccessionHitCnt = 1;
		}
		else
		{
			++m_weaknesSsuccessionHitCnt;
		}

		m_addWeaknesSsuccessionHitCntTime = ADDWEAKNESSEUCCESSIONHITCNTTIMELIMIT;
	}
	else
	{
		m_endurance = 0.0f;
	}
	m_attackHit = true;

	if (m_graduallyTorionDecVal == 0)
	{
		m_graduallyTorionDecVal = 0.1f;
	}
	else
	{
		m_graduallyTorionDecVal *= 1.5f;
	}

	m_hitColorChangeTimeCnt = (*m_wpJsonObj.lock())["HitColorChangeTimeCnt"].int_value();

	if (m_weaknesSsuccessionHitCnt >= 5)
	{
		m_weaknesSsuccessionHitCnt = 0;
		m_addWeaknesSsuccessionHitCntTime = 0;
		m_invincibilityTimeCnt = (*m_wpJsonObj.lock())["InvincibilityMaxTimeCnt"].int_value();
		m_hitStopCnt = 80;
		m_animator->SetAnimation(m_model->GetAnimation("SuccessionWeaknessOnHit"), false);
	}
}

void Enemy::HasDefense()
{
	if (m_EnemyState & (rAttack | rlAttack))
	{
		m_animator->SetAnimation(m_model->GetAnimation("RHasDefense"), false);
	}
	else if (m_EnemyState & lAttack)
	{
		m_animator->SetAnimation(m_model->GetAnimation("LHasDefense"), false);
	}

	m_EnemyState |= hasDefense;
	m_EnemyState &= hasDefense;
	m_hasDeTime = 30;
	m_bMove = true;
}

void Enemy::DrawSprite()
{

}

void Enemy::DrawDebug()
{
	if (m_bEnemyLose)
	{
		return;
	}

#ifdef _DEBUG
	if (!m_pDebugWire)return;
	m_pDebugWire->Draw();

	for (auto& WeaList : m_weaponList)
	{
		WeaList->DrawDebug();
	}
#endif
}

void Enemy::GenerateDepthMapFromLight_SkinMesh()
{
	if (m_bEnemyLose)
	{
		return;
	}

	if (!m_model) return;

	KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_model, m_mWorld);
}

void Enemy::DrawLit_SkinMesh()
{
	if (m_bEnemyLose)
	{
		return;
	}

	if (!m_model) return;

	if (m_invincibilityTimeCnt <= (*m_wpJsonObj.lock())["InvincibilityProgramTimeCnt"][0].int_value() && m_invincibilityTimeCnt > (*m_wpJsonObj.lock())["InvincibilityProgramTimeCnt"][1].int_value() ||
		m_invincibilityTimeCnt <= (*m_wpJsonObj.lock())["InvincibilityProgramTimeCnt"][2].int_value() && m_invincibilityTimeCnt > (*m_wpJsonObj.lock())["InvincibilityProgramTimeCnt"][3].int_value() ||
		m_invincibilityTimeCnt <= (*m_wpJsonObj.lock())["InvincibilityProgramTimeCnt"][4].int_value() && m_invincibilityTimeCnt > (*m_wpJsonObj.lock())["InvincibilityProgramTimeCnt"][5].int_value() ||
		m_invincibilityTimeCnt <= (*m_wpJsonObj.lock())["InvincibilityProgramTimeCnt"][6].int_value() && m_invincibilityTimeCnt > (*m_wpJsonObj.lock())["InvincibilityProgramTimeCnt"][7].int_value() ||
		m_invincibilityTimeCnt <= (*m_wpJsonObj.lock())["InvincibilityProgramTimeCnt"][8].int_value() && m_invincibilityTimeCnt > (*m_wpJsonObj.lock())["InvincibilityProgramTimeCnt"][9].int_value() ||
		m_invincibilityTimeCnt <= (*m_wpJsonObj.lock())["InvincibilityProgramTimeCnt"][10].int_value() && m_invincibilityTimeCnt > (*m_wpJsonObj.lock())["InvincibilityProgramTimeCnt"][11].int_value() ||
		m_invincibilityTimeCnt == (*m_wpJsonObj.lock())["InvincibilityProgramTimeCnt"][12].int_value()
		)return;

	KdShaderManager::Instance().m_HD2DShader.SetOutLineColor({ 1,0,0 });
	if (m_hitColorChangeTimeCnt == 0)
	{
		Math::Color color = { 1,1,1,1 };
		KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_model, m_mWorld, true, color);
	}
	else
	{
		Math::Color color = { 1,0,0,1 };
		KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_model, m_mWorld, true, color);
	}
}

void Enemy::DrawLit()
{
	if (m_bEnemyLose)
	{
		return;
	}

	if (!m_bEwaponDrawPosible)return;

	if (SceneManager::Instance().GetSceneType() != SceneManager::SceneType::tutorial)
	{
		for (auto& WeaList : m_weaponList)
		{
			WeaList->DrawLit();
		}
	}
}

void Enemy::DrawUnLit()
{
	/*Math::Matrix mat = Math::Matrix::CreateScale(1.0f) * Math::Matrix::CreateTranslation(m_pos.x, m_pos.y + 3.0f, m_pos.z);

	Math::Color color = { 1, 1, 1, 1 };
	KdShaderManager::Instance().m_HD2DShader.DrawPolygon(m_endurancePoly, mat,color);

	color = { 1, 1, 1, 1 };
	KdShaderManager::Instance().m_HD2DShader.DrawPolygon(m_enduranceBarPoly, mat, color);*/
}

void Enemy::DrawBright()
{
	if (m_bEnemyLose)
	{
		return;
	}

	if (!m_bEwaponDrawPosible)return;

	if (SceneManager::Instance().GetSceneType() != SceneManager::SceneType::tutorial)
	{
		for (auto& WeaList : m_weaponList)
		{
			WeaList->DrawBright();
		}
	}
}

void Enemy::GenerateDepthMapFromLight()
{
	if (m_bEnemyLose)
	{
		return;
	}

	if (!m_bEwaponDrawPosible)return;

	if (SceneManager::Instance().GetSceneType() != SceneManager::SceneType::tutorial)
	{
		for (auto& WeaList : m_weaponList)
		{
			WeaList->GenerateDepthMapFromLight();
		}
	}
}

void Enemy::SetWeaponToTarget(std::shared_ptr<Player> a_spPlayer)
{
	m_weaponList[0]->SetTarget(a_spPlayer);
	m_weaponList[1]->SetTarget(a_spPlayer);
}

void Enemy::SetMatrix()
{
	// 座標行列
	Math::Matrix transMat;
	transMat = Math::Matrix::CreateTranslation(m_pos);

	// 回転行列
	Math::Matrix rotMat;
	rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y));

	// 行列合成
	m_mWorld = rotMat * transMat;
}

void Enemy::SetModelAndType(EnemyType a_enemyType)
{
	m_enemyType = a_enemyType;
	const KdModelWork::Node* node = nullptr;
	Math::Matrix mat = Math::Matrix::Identity;

	switch (m_enemyType)
	{
	case coarseFishEnemy:
		m_model = std::make_shared<KdModelWork>();
		m_model->SetModelData
		(KdAssets::Instance().m_modeldatas.GetData
		("Asset/Models/Enemy/CoarseFishEnemy.gltf"));
		/// 当たり判定初期化
		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape
		("EnemyModel", m_model, KdCollider::TypeBump | KdCollider::TypeDamage  | KdCollider::TypeAttackDec);
		m_enemyAttackMaxTotal = 2;
		break;
	case wimpEnemyTypeOne:
		m_model = std::make_shared<KdModelWork>();
		m_model->SetModelData
		(KdAssets::Instance().m_modeldatas.GetData
		("Asset/Models/Enemy/WimpEnemyTypeOne.gltf"));
		/// 当たり判定初期化
		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape
		("EnemyModel", m_model, KdCollider::TypeBump | KdCollider::TypeDamage | KdCollider::TypeRideEnemy | KdCollider::TypeAttackDec);
		
		node = m_model->FindNode("HitPoint");
		mat = node->m_worldTransform * Math::Matrix::Identity;
		m_pCollider->RegisterCollisionShape
		("EnemyModelWeakness", { mat._41,mat._42,mat._43}, 0.325f, KdCollider::TypeWeakness);
		m_enemyAttackMaxTotal = 4;
		break;
	case bossEnemyTypeOne:
		m_model = std::make_shared<KdModelWork>();
		m_model->SetModelData
		(KdAssets::Instance().m_modeldatas.GetData
		("Asset/Models/Enemy/BossEnemyTypeOne.gltf"));
		/// 当たり判定初期化
		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape
		("EnemyModel", m_model, KdCollider::TypeBump | KdCollider::TypeDamage | KdCollider::TypeRideEnemy | KdCollider::TypeAttackDec);

		node = m_model->FindNode("HitPoint");
		mat = node->m_worldTransform * Math::Matrix::Identity;
		m_pCollider->RegisterCollisionShape
		("EnemyModelWeakness", { mat._41,mat._42,mat._43}, 3.00f, KdCollider::TypeWeakness);
		m_hitpos = { mat._41,mat._42,mat._43};
		break;
	}
}

void Enemy::EnemyKickHitAttackChaeck()
{
	const KdModelWork::Node* node = nullptr;
	Math::Matrix mat = Math::Matrix::Identity;

	if (!m_target.lock()->GetAttackHit() && !m_target.lock()->GetDefenseSuc() && m_target.lock()->GetInvincibilityTimeCnt() == 0 && !m_target.lock()->GetBPlayerDeath()) // ここになくていいかも
	{
		/*if (player->GetPlayerState() & Player::PlayerState::rAttack && m_arrmType == lArrm)return;
		if (player->GetPlayerState() & Player::PlayerState::lAttack && m_arrmType == rArrm)return;*/

		node = m_model->FindNode("LegAttackPoint");
		KdCollider::SphereInfo sphereInfo;
		mat = node->m_worldTransform * m_mWorld;
		mat._42 += 0.7f;
		sphereInfo.m_sphere.Center = mat.Translation();
		sphereInfo.m_sphere.Radius = 0.30f;
		sphereInfo.m_type = KdCollider::TypeDamage;

#ifdef _DEBUG
		m_pDebugWire->AddDebugSphere
		(
			sphereInfo.m_sphere.Center,
			sphereInfo.m_sphere.Radius,
			{ 0,0,0,1 }
		);
#endif

		std::list<KdCollider::CollisionResult> retSphereList;

		/*for (auto& obj : SceneManager::Instance().GetObjList())
		{*/
		m_target.lock()->Intersects
		(
			sphereInfo,
			&retSphereList
		);

		Math::Vector3 hitDir = {};
		bool hit = false;
		Math::Vector3 hitPos;
		for (auto& ret : retSphereList)
		{
			hit = true;
			hitDir = ret.m_hitDir;
			hitPos = ret.m_hitPos;
		}

		if (hit)
		{

			m_target.lock()->BlowingAwayAttackOnHit(m_mWorld.Backward());
			KdAudioManager::Instance().Play("Asset/Audio/SE/KickAttackHit.wav");

			hitPos.y += 0.35f;
			KdEffekseerManager::GetInstance().
				Play("Hit3.efk", hitPos);
			KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("Hit3.efk"); // これでループしない
			//KdEffekseerManager::GetInstance().SetRotation("Hit3.efk", m_mWorld.Backward(),DirectX::XMConvertToRadians(0));
			Math::Matrix efcMat = Math::Matrix::CreateScale(0.5f) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)) * Math::Matrix::CreateTranslation(hitPos);
			KdEffekseerManager::GetInstance().SetWorldMatrix("Hit3.efk", efcMat);
		}
		else
		{
			node = m_model->FindNode("LegAttackHitPoint");
			sphereInfo;
			mat = node->m_worldTransform * m_mWorld;
			mat._42 += 0.7f;
			sphereInfo.m_sphere.Center = mat.Translation();
			sphereInfo.m_sphere.Radius = 0.30f;
			sphereInfo.m_type = KdCollider::TypeDamage;

#ifdef _DEBUG
			m_pDebugWire->AddDebugSphere
			(
				sphereInfo.m_sphere.Center,
				sphereInfo.m_sphere.Radius,
				{ 0,0,0,1 }
			);
#endif

			retSphereList.clear();

			/*for (auto& obj : SceneManager::Instance().GetObjList())
			{*/
			m_target.lock()->Intersects
			(
				sphereInfo,
				&retSphereList
			);

			hitDir = {};
			hit = false;
			for (auto& ret : retSphereList)
			{
				hit = true;
				hitDir = ret.m_hitDir;

				hitPos = ret.m_hitPos;
			}

			if (hit)
			{
				m_target.lock()->BlowingAwayAttackOnHit(m_mWorld.Backward());
				KdAudioManager::Instance().Play("Asset/Audio/SE/KickAttackHit.wav");

				hitPos.y += 0.35f;
				KdEffekseerManager::GetInstance().
					Play("Hit3.efk", hitPos);
				KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("Hit3.efk"); // これでループしない
				//KdEffekseerManager::GetInstance().SetRotation("Hit3.efk", m_mWorld.Backward(),DirectX::XMConvertToRadians(0));
				Math::Matrix efcMat = Math::Matrix::CreateScale(0.5f) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)) * Math::Matrix::CreateTranslation(hitPos);
				KdEffekseerManager::GetInstance().SetWorldMatrix("Hit3.efk", efcMat);
			}
			else
			{
				node = m_model->FindNode("LegAttackHitPointTwo");
				sphereInfo;
				mat = node->m_worldTransform * m_mWorld;
				mat._42 += 0.7f;
				sphereInfo.m_sphere.Center = mat.Translation();
				sphereInfo.m_sphere.Radius = 0.30f;
				sphereInfo.m_type = KdCollider::TypeDamage;

#ifdef _DEBUG
				m_pDebugWire->AddDebugSphere
				(
					sphereInfo.m_sphere.Center,
					sphereInfo.m_sphere.Radius,
					{ 0,0,0,1 }
				);
#endif

				retSphereList.clear();

				/*for (auto& obj : SceneManager::Instance().GetObjList())
				{*/
				m_target.lock()->Intersects
				(
					sphereInfo,
					&retSphereList
				);

				hitDir = {};
				hit = false;
				for (auto& ret : retSphereList)
				{
					hit = true;
					hitDir = ret.m_hitDir;
					hitPos = ret.m_hitPos;
				}

				if (hit)
				{
					m_target.lock()->BlowingAwayAttackOnHit(m_mWorld.Backward());
					KdAudioManager::Instance().Play("Asset/Audio/SE/KickAttackHit.wav");

					hitPos.y += 0.35f;
					KdEffekseerManager::GetInstance().
						Play("Hit3.efk", hitPos);
					KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("Hit3.efk"); // これでループしない
					//KdEffekseerManager::GetInstance().SetRotation("Hit3.efk", m_mWorld.Backward(),DirectX::XMConvertToRadians(0));
					Math::Matrix efcMat = Math::Matrix::CreateScale(0.5f) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)) * Math::Matrix::CreateTranslation(hitPos);
					KdEffekseerManager::GetInstance().SetWorldMatrix("Hit3.efk", efcMat);
				}
			}
		}
	}	//}
}

void Enemy::EnemyBeamHitChaeck()
{
	if (m_target.expired())return;
	if (!m_target.lock()->GetAttackHit() && !m_target.lock()->GetDefenseSuc() && m_target.lock()->GetInvincibilityTimeCnt() == 0 && !m_target.lock()->GetBPlayerDeath()) // ここになくていいかも
	{
		KdCollider::RayInfo rayInfo;
		rayInfo.m_pos = m_rangedAttackShotPos;

		rayInfo.m_dir = m_rangedAttackDir;
		rayInfo.m_range = m_beamRange;

		rayInfo.m_type = KdCollider::TypeDamage;
#ifdef _DEBUG
		m_pDebugWire->AddDebugLine(rayInfo.m_pos, rayInfo.m_dir, rayInfo.m_range, { 1,1,1,1 });
#endif
		std::list<KdCollider::CollisionResult> retRayList;

		m_target.lock()->Intersects
		(
			rayInfo,
			&retRayList
		);


		// レイに当たったリストから一番近いオブジェクトを検出
		float maxOverLap = 0;
		Math::Vector3 hitPos = {};
		bool hit = false;
		for (auto& ret : retRayList)
		{
			maxOverLap = ret.m_overlapDistance;
			hitPos = ret.m_hitPos;
			hit = true;
		}

		if (hit)
		{
			m_target.lock()->BlowingAwayAttackOnHit(m_mWorld.Backward());
			KdAudioManager::Instance().Play("Asset/Audio/SE/KickAttackHit.wav");

			hitPos.y += 0.35f;
			KdEffekseerManager::GetInstance().
				Play("Hit3.efk", hitPos);
			KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("Hit3.efk"); // これでループしない
			//KdEffekseerManager::GetInstance().SetRotation("Hit3.efk", m_mWorld.Backward(),DirectX::XMConvertToRadians(0));
			Math::Matrix efcMat = Math::Matrix::CreateScale(0.5f) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)) * Math::Matrix::CreateTranslation(hitPos);
			KdEffekseerManager::GetInstance().SetWorldMatrix("Hit3.efk", efcMat);
		}
		else
		{
			rayInfo.m_dir = Math::Vector3::Transform(m_rangedAttackDir,Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(1)));
			rayInfo.m_range = m_beamRange;
#ifdef _DEBUG
			m_pDebugWire->AddDebugLine(rayInfo.m_pos, rayInfo.m_dir, rayInfo.m_range, { 1,0,0,1 });
#endif
			retRayList.clear();

			m_target.lock()->Intersects
			(
				rayInfo,
				&retRayList
			);


			// レイに当たったリストから一番近いオブジェクトを検出
			maxOverLap = 0;
			hitPos = {};
			hit = false;
			for (auto& ret : retRayList)
			{
				maxOverLap = ret.m_overlapDistance;
				hitPos = ret.m_hitPos;
				hit = true;
			}

			if (hit)
			{
				m_target.lock()->BlowingAwayAttackOnHit(m_mWorld.Backward());
				KdAudioManager::Instance().Play("Asset/Audio/SE/KickAttackHit.wav");

				hitPos.y += 0.35f;
				KdEffekseerManager::GetInstance().
					Play("Hit3.efk", hitPos);
				KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("Hit3.efk"); // これでループしない
				//KdEffekseerManager::GetInstance().SetRotation("Hit3.efk", m_mWorld.Backward(),DirectX::XMConvertToRadians(0));
				Math::Matrix efcMat = Math::Matrix::CreateScale(0.5f) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)) * Math::Matrix::CreateTranslation(hitPos);
				KdEffekseerManager::GetInstance().SetWorldMatrix("Hit3.efk", efcMat);
			}
			else
			{
				rayInfo.m_dir = Math::Vector3::Transform(m_rangedAttackDir, Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(-1)));
				rayInfo.m_range = m_beamRange;
#ifdef _DEBUG
				m_pDebugWire->AddDebugLine(rayInfo.m_pos, rayInfo.m_dir, rayInfo.m_range, { 0,0,1,1 });
#endif
				retRayList.clear();

				m_target.lock()->Intersects
				(
					rayInfo,
					&retRayList
				);


				// レイに当たったリストから一番近いオブジェクトを検出
				maxOverLap = 0;
				hitPos = {};
				hit = false;
				for (auto& ret : retRayList)
				{
					maxOverLap = ret.m_overlapDistance;
					hitPos = ret.m_hitPos;
					hit = true;
				}

				if (hit)
				{
					m_target.lock()->BlowingAwayAttackOnHit(m_mWorld.Backward());
					KdAudioManager::Instance().Play("Asset/Audio/SE/KickAttackHit.wav");

					hitPos.y += 0.35f;
					KdEffekseerManager::GetInstance().
						Play("Hit3.efk", hitPos);
					KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("Hit3.efk"); // これでループしない
					//KdEffekseerManager::GetInstance().SetRotation("Hit3.efk", m_mWorld.Backward(),DirectX::XMConvertToRadians(0));
					Math::Matrix efcMat = Math::Matrix::CreateScale(0.5f) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)) * Math::Matrix::CreateTranslation(hitPos);
					KdEffekseerManager::GetInstance().SetWorldMatrix("Hit3.efk", efcMat);
				}
			}
		}
	}
}

void Enemy::EnemyEnergyBulletHitChaeck()
{
	if (m_target.expired())return;
	if (!m_target.lock()->GetAttackHit() && m_target.lock()->GetInvincibilityTimeCnt() == 0 && !m_target.lock()->GetBPlayerDeath())
	{
		KdCollider::SphereInfo sphereInfo;
		sphereInfo.m_sphere.Center = m_beamCollisionPos;

		if (m_enemyType & bossEnemyTypeOne)
		{
			sphereInfo.m_sphere.Radius = 2.4f;
		}
		else
		{
			sphereInfo.m_sphere.Radius = 0.8f;
		}

		sphereInfo.m_type = KdCollider::TypeDamage;

#ifdef _DEBUG
		m_pDebugWire->AddDebugSphere
		(
			sphereInfo.m_sphere.Center,
			sphereInfo.m_sphere.Radius,
			{ 1,0,0,1 }
		);
#endif
		std::list<KdCollider::CollisionResult> retSphereList;

		m_target.lock()->Intersects
		(
			sphereInfo,
			&retSphereList
		);

		Math::Vector3 hitPos = {};
		Math::Vector3 hitDir = {};
		bool hit = false;
		for (auto& ret : retSphereList)
		{
			hit = true;
			hitDir = ret.m_hitDir;
			hitPos = ret.m_hitPos;
		}

		if (hit)
		{
			m_target.lock()->BlowingAwayAttackOnHit(m_mWorld.Backward());
			//KdAudioManager::Instance().Play("Asset/Audio/SE/KickAttackHit.wav");
			KdAudioManager::Instance().Play("Asset/Audio/SE/EnergyBulletHit.wav");

			KdEffekseerManager::GetInstance().
				Play("Hit3.efk", hitPos);
			KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("Hit3.efk"); // これでループしない
			//KdEffekseerManager::GetInstance().SetRotation("Hit3.efk", m_mWorld.Backward(),DirectX::XMConvertToRadians(0));
			Math::Matrix efcMat = Math::Matrix::CreateScale(0.5f) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)) * Math::Matrix::CreateTranslation(hitPos);
			KdEffekseerManager::GetInstance().SetWorldMatrix("Hit3.efk", efcMat);
		}
	}
}

void Enemy::UpdateRotate(Math::Vector3& a_srcMoveVec)
{
	if (m_bEnemyDeath)return;
	// 今向いてる方向のベクトル
	Math::Vector3 nowDir = m_mWorld.Backward();

	// 移動方向のベクトル
	Math::Vector3 targetDir = a_srcMoveVec;

	nowDir.Normalize();
	targetDir.Normalize();

	float nowAng = atan2(nowDir.x, nowDir.z);
	nowAng = DirectX::XMConvertToDegrees(nowAng);

	float targetAng = atan2(targetDir.x, targetDir.z);
	targetAng = DirectX::XMConvertToDegrees(targetAng);

	Math::Vector3 dot = DirectX::XMVector3Dot(nowDir, targetDir);
	if (dot.x > 1)
	{
		dot.x = 1;
	}
	if (dot.x < -1)
	{
		dot.x = -1;
	}

	float ang = DirectX::XMConvertToDegrees(acos(dot.x));

	if (ang > 1)
	{
		if (m_delayTurnAroundTime > 0)
		{
			m_delayTurnAroundTime--;
		}
	}
	else
	{
		m_delayTurnAroundTime = 10;
	}

	if (ang < 1)
	{
		m_bMantisPossAng = true;
	}
	else
	{
		m_bMantisPossAng = false;
	}

	if (!(m_enemyType & (bossEnemyTypeOne | wimpEnemyTypeOne)))
	{
		if (ang >= 8.0f)
		{
			ang = 8.0f;
		}
	}
	else
	{
		if (m_enemyType & wimpEnemyTypeOne)
		{
			if (ang < 30)
			{
				m_bRangedAttackCapableOfFiring = true;
			}
			else
			{
				m_bRangedAttackCapableOfFiring = false;
			}
		}
		else
		{
			const KdModelWork::Node* node = nullptr;
			Math::Matrix mat = Math::Matrix::Identity;

			node = m_model->FindNode("HitPoint");
			mat = node->m_worldTransform * m_mWorld;

			std::shared_ptr<Player> spTarget = m_target.lock();
			if (spTarget)
			{
				Math::Vector3 std = spTarget->GetPos() - mat.Translation();
				if (spTarget->GetPos().y < mat.Translation().y - 1.65f)
				{
					if (ang < 5)
					{
						m_bRangedAttackCapableOfFiring = true;
					}
					else
					{
						m_bRangedAttackCapableOfFiring = false;
					}
				}
				else
				{
					if (std.Length() <= 100)
					{
						if (ang < 45)
						{
							m_bRangedAttackCapableOfFiring = true;
						}
						else
						{
							m_bRangedAttackCapableOfFiring = false;
						}
					}
					else
					{
						if (ang < 20)
						{
							m_bRangedAttackCapableOfFiring = true;
						}
						else
						{
							m_bRangedAttackCapableOfFiring = false;
						}
					}
				}
			}
		}

		if (ang >= 1.0f)
		{
			ang = 1.0f;
		}
	}

	Math::Vector3 croos = DirectX::XMVector3Cross(nowDir, targetDir);

	if (m_delayTurnAroundTime == 0)
	{
		if (croos.y >= 0)
		{
			m_mWorldRot.y += ang;
		}
		if (croos.y < 0)
		{
			m_mWorldRot.y -= ang;
		}
	}
}

void Enemy::GrassMoveVecDecision()
{
	const KdModelWork::Node* node = nullptr;
	Math::Matrix mat = Math::Matrix::Identity;
	Math::Vector3 src = Math::Vector3::Zero;

	if (!m_target.expired())
	{
		src = m_target.lock()->GetPos() - m_pos;
	}

	if (m_rGrassHopperPauCnt == 0)
	{
		if (m_weaponType & eGrassHopper)
		{
			std::random_device rnd;
			std::mt19937 mt(rnd());
			std::uniform_int_distribution<int> intRand(0, 999);
			int randNum[5] = {};

			int rand = intRand(mt);
			
			float y = 0.0f;

			if (!m_target.expired())
			{
				y = m_target.lock()->GetPos().y - m_pos.y;
				
			}

			switch (m_wantToMoveState)
			{
			case WantToMoveState::wEscape:
				if (m_grassSuccessionDelayCnt != 0)return;
				randNum[0] = 0;
				randNum[1] = 550;
				randNum[2] = 150;
				randNum[3] = 150;
				randNum[4] = 150;
				break;
			case WantToMoveState::wDashAttack:
				if (src.Length() <= 2.0f)
				{
					randNum[0] = 0;    // 前
					randNum[1] = 1000;    // 後
					randNum[2] = 0;    // 右 
					randNum[3] = 0;    // 左
					randNum[4] = 0;    // 上
				}
				else
				{
					randNum[0] = 1000;    // 前
					randNum[1] = 0;    // 後
					randNum[2] = 0;    // 右 
					randNum[3] = 0;    // 左
					randNum[4] = 0;
				}
				break;
			case WantToMoveState::wGrassDash:
				if (m_grassSuccessionDelayCnt != 0)return;
				if (y >= 1.0f)
				{
					randNum[0] = 0;    // 前
					randNum[1] = 0;    // 後
					randNum[2] = 0;    // 右 
					randNum[3] = 0;    // 左
					randNum[4] = 1000;    // 上
				}
				else
				{
					if (m_bEnemyBetweenPlayer)
					{
						Math::Vector3 nowVec = m_mWorld.Backward();
						nowVec.y = 0.0f;
						nowVec.Normalize();

						Math::Vector3 toVec = m_enemyBetweenPlayerHitPos - m_pos;
						toVec.y = 0.0f;
						toVec.Normalize();
						Math::Vector3 dot = DirectX::XMVector3Dot(nowVec, toVec);

						if (dot.x > 1.0f)
						{
							dot.x = 1.0f;
						}
						else if (dot.x < -1.0f)
						{
							dot.x = -1.0f;
						}

						float ang = DirectX::XMConvertToDegrees(acos(dot.x));

						if (ang <= 0.1 || ang > 150 && ang <= 180)
						{
							randNum[0] = 0;
							randNum[1] = 0;
							randNum[2] = 1000;
							randNum[3] = 0;
							randNum[4] = 0;
						}
						else if (ang > 0.1 && ang <= 150)
						{

							randNum[0] = 1000;
							randNum[1] = 0;
							randNum[2] = 0;
							randNum[3] = 0;
							randNum[4] = 0;
						}
					}
					else
					{
						randNum[0] = 350;
						randNum[1] = 150;
						randNum[2] = 200;
						randNum[3] = 200;
						randNum[4] = 100;
					}
				}
				break;
			case WantToMoveState::wAvoidance:
				if (m_grassSuccessionDelayCnt != 0)return;
				randNum[0] = 50;
				randNum[1] = 50;
				randNum[2] = 350;
				randNum[3] = 350;
				randNum[4] = 200;
				break;
			}

			Math::Vector3 vec = m_mWorld.Backward();
			Math::Matrix efcMat;

			std::string fileName;
			switch (m_enemyNumber)
			{
			case 1:
				fileName = "GrassDashBlurEnemyOne.efk";
				break;
			case 2:
				fileName = "GrassDashBlurEnemyTwo.efk";
				break;
			case 3:
				fileName = "GrassDashBlurEnemyThree.efk";
				break;
			case 4:
				fileName = "GrassDashBlurEnemyFour.efk";
				break;
			case 5:
				fileName = "GrassDashBlurEnemyFive.efk";
				break;
			}

			for (int i = 0; i < 5; i++)
			{
				rand -= randNum[i];
				if (rand < 0)
				{
					switch (i)
					{
					case 0:
						m_grassHopperDashDir = Math::Vector3::TransformNormal(Math::Vector3(0, 0, 1), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
						m_grassHopperDashDir.Normalize();
						m_bMove = true;
						m_EnemyState &= ~grassHopperDash;
						m_EnemyState |= grassHopperDashF;
						m_EnemyState &= ~grassHopperDashUp;
						m_lGrassHopperPauCnt = 120;
						m_lGrassHopperTime = 90;
						m_rGrassHopperTime = 0;
						m_gravity = 0;

						m_weaponList[2]->GrassHopper({ m_pos.x + 0.65f * m_mWorldRot.x,m_pos.y + 0.9f,m_pos.z + 0.65f * m_mWorldRot.z }, m_mWorldRot.y);

						KdEffekseerManager::GetInstance().
							Play(fileName, { m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z });
						KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect(fileName); // これでループしない
						efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)) * Math::Matrix::CreateTranslation({ m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z });
						KdEffekseerManager::GetInstance().SetWorldMatrix(fileName, efcMat);
						break;
					case 1:
						m_grassHopperDashDir = Math::Vector3::TransformNormal(Math::Vector3(0, 0, -1), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
						m_grassHopperDashDir.Normalize();
						m_bMove = true;
						m_EnemyState &= ~grassHopperDash;
						m_EnemyState |= grassHopperDashB;
						m_EnemyState &= ~grassHopperDashUp;
						m_lGrassHopperPauCnt = 120;
						m_lGrassHopperTime = 90;
						m_rGrassHopperTime = 0;
						m_gravity = 0;
						node = m_model->FindNode("GrassHopperLegRPoint");
						mat = node->m_worldTransform * m_mWorld;
						mat._42 += 0.7f;
						m_weaponList[2]->GrassHopper({ mat._41,mat._42,mat._43 }, m_mWorldRot.y);

						vec = m_mWorld.Forward();
						KdEffekseerManager::GetInstance().
							Play(fileName, { m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z });
						KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect(fileName); // これでループしない
						efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y + 180.0f)) * Math::Matrix::CreateTranslation({ m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z });
						KdEffekseerManager::GetInstance().SetWorldMatrix(fileName, efcMat);
						break;
					case 2:
						m_grassHopperDashDir = Math::Vector3::TransformNormal(Math::Vector3(1, 0, 0), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
						m_grassHopperDashDir.Normalize();
						m_bMove = true;
						m_EnemyState &= ~grassHopperDash;
						m_EnemyState |= grassHopperDashR;
						m_EnemyState &= ~grassHopperDashUp;
						m_lGrassHopperPauCnt = 120;
						m_lGrassHopperTime = 90;
						m_rGrassHopperTime = 0;
						m_gravity = 0;
						node = m_model->FindNode("GrassHopperLegLPoint");
						mat = node->m_worldTransform * m_mWorld;
						mat._42 += 0.7f;
						m_weaponList[2]->GrassHopper({ mat._41,mat._42,mat._43 }, m_mWorldRot.y);

						vec = m_mWorld.Right();
						KdEffekseerManager::GetInstance().
							Play(fileName, { m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z });
						KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect(fileName); // これでループしない
						efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y + 90.0f)) * Math::Matrix::CreateTranslation({ m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z });
						KdEffekseerManager::GetInstance().SetWorldMatrix(fileName, efcMat);
						break;
					case 3:
						m_grassHopperDashDir = Math::Vector3::TransformNormal(Math::Vector3(-1, 0, 0), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
						m_grassHopperDashDir.Normalize();
						m_bMove = true;
						m_EnemyState &= ~grassHopperDash;
						m_EnemyState |= grassHopperDashL;
						m_EnemyState &= ~grassHopperDashUp;
						m_lGrassHopperPauCnt = 120;
						m_lGrassHopperTime = 90;
						m_rGrassHopperTime = 0;
						m_gravity = 0;
						node = m_model->FindNode("GrassHopperLegRPoint");
						mat = node->m_worldTransform * m_mWorld;
						mat._42 += 0.7f;
						m_weaponList[2]->GrassHopper({ mat._41,mat._42,mat._43 }, m_mWorldRot.y);

						vec = m_mWorld.Left();
						KdEffekseerManager::GetInstance().
							Play(fileName, { m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z });
						KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect(fileName); // これでループしない
						efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y + 270.0f)) * Math::Matrix::CreateTranslation({ m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z });
						KdEffekseerManager::GetInstance().SetWorldMatrix(fileName, efcMat);
						break;
					case 4:
						m_grassHopperDashDir = { 0, 1, 0 };
						m_grassHopperDashDir.Normalize();
						m_bMove = true;
						m_EnemyState &= ~grassHopperDash;
						m_EnemyState |= grassHopperDashUp;
						m_lGrassHopperPauCnt = 120;
						m_lGrassHopperTime = 90;
						m_rGrassHopperTime = 0;
						m_gravity = 0;

						node = m_model->FindNode("GrassHopperLegRPoint");
						mat = node->m_worldTransform * m_mWorld;
						mat._42 += 0.7f;
						m_weaponList[2]->GrassHopper({ mat._41,mat._42,mat._43 }, m_mWorldRot.y);

						KdEffekseerManager::GetInstance().
							Play(fileName, { m_pos.x,m_pos.y + 2.9f,m_pos.z });
						KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect(fileName); // これでループしない
						efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(270.0f)) * Math::Matrix::CreateTranslation({ m_pos.x,m_pos.y + 2.9f,m_pos.z });
						KdEffekseerManager::GetInstance().SetWorldMatrix(fileName, efcMat);
						break;
					}
					break;
				}
			}
			m_grassSuccessionDelayCnt = 40;
			return;
		}
	}

	if (m_lGrassHopperPauCnt == 0)
	{
		if (m_weaponType & eLGrassHopper)
		{
			std::random_device rnd;
			std::mt19937 mt(rnd());
			std::uniform_int_distribution<int> intRand(0, 999);
			int randNum[5] = {};

			int rand = intRand(mt);
			float y = m_target.lock()->GetPos().y - m_pos.y;
			switch (m_wantToMoveState)
			{
			case WantToMoveState::wEscape:
				if (m_grassSuccessionDelayCnt != 0)return;
				randNum[0] = 0;
				randNum[1] = 550;
				randNum[2] = 150;
				randNum[3] = 150;
				randNum[4] = 150;
				break;
			case WantToMoveState::wDashAttack:

				if (y >= 1.0f)
				{
					randNum[0] = 0;    // 前
					randNum[1] = 0;    // 後
					randNum[2] = 0;    // 右 
					randNum[3] = 0;    // 左
					randNum[4] = 1000;    // 上
				}
				else
				{
					if (src.Length() <= 2.0f)
					{
						randNum[0] = 0;    // 前
						randNum[1] = 1000;    // 後
						randNum[2] = 0;    // 右 
						randNum[3] = 0;    // 左
						randNum[4] = 0;    // 上
					}
					else
					{
						randNum[0] = 1000;    // 前
						randNum[1] = 0;    // 後
						randNum[2] = 0;    // 右 
						randNum[3] = 0;    // 左
						randNum[4] = 0;
					}
				}
				break;
			case WantToMoveState::wGrassDash:
				if (m_grassSuccessionDelayCnt != 0)return;
				if (y >= 1.0f)
				{
					randNum[0] = 0;    // 前
					randNum[1] = 0;    // 後
					randNum[2] = 0;    // 右 
					randNum[3] = 0;    // 左
					randNum[4] = 1000;    // 上
				}
				else
				{
					if (m_bEnemyBetweenPlayer)
					{
						Math::Vector3 nowVec = m_mWorld.Backward();
						nowVec.y = 0.0f;
						nowVec.Normalize();

						Math::Vector3 toVec = m_enemyBetweenPlayerHitPos - m_pos;
						toVec.y = 0.0f;
						toVec.Normalize();
						Math::Vector3 dot = DirectX::XMVector3Dot(nowVec, toVec);

						if (dot.x > 1.0f)
						{
							dot.x = 1.0f;
						}
						if (dot.x < -1.0f)
						{
							dot.x = -1.0f;
						}

						float ang = DirectX::XMConvertToDegrees(acos(dot.x));

						if (ang < 0.1 || ang > 150 && ang <= 180)
						{
							randNum[0] = 0;
							randNum[1] = 0;
							randNum[2] = 1000;
							randNum[3] = 0;
							randNum[4] = 0;
						}
						else if (ang >= 0.1 && ang <= 150)
						{

							randNum[0] = 1000;
							randNum[1] = 0;
							randNum[2] = 0;
							randNum[3] = 0;
							randNum[4] = 0;
						}
					}
					else
					{
						randNum[0] = 350;
						randNum[1] = 150;
						randNum[2] = 200;
						randNum[3] = 200;
						randNum[4] = 100;
					}
				}
				break;
			case WantToMoveState::wAvoidance:
				if (m_grassSuccessionDelayCnt != 0)return;
				randNum[0] = 50;
				randNum[1] = 50;
				randNum[2] = 350;
				randNum[3] = 350;
				randNum[4] = 200;
				break;
			}

			Math::Vector3 vec = m_mWorld.Backward();
			Math::Matrix efcMat;

			std::string fileName;
			switch (m_enemyNumber)
			{
			case 1:
				fileName = "GrassDashBlurEnemyOne.efk";
				break;
			case 2:
				fileName = "GrassDashBlurEnemyTwo.efk";
				break;
			case 3:
				fileName = "GrassDashBlurEnemyThree.efk";
				break;
			case 4:
				fileName = "GrassDashBlurEnemyFour.efk";
				break;
			case 5:
				fileName = "GrassDashBlurEnemyFive.efk";
				break;
			}

			for (int i = 0; i < 5; i++)
			{
				rand -= randNum[i];
				if (rand < 0)
				{
					switch (i)
					{
					case 0:
						m_grassHopperDashDir = Math::Vector3::TransformNormal(Math::Vector3(0, 0, 1), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
						m_grassHopperDashDir.Normalize();
						m_bMove = true;
						m_EnemyState &= ~grassHopperDash;
						m_EnemyState |= grassHopperDashF;
						m_EnemyState &= ~grassHopperDashUp;
						m_lGrassHopperPauCnt = 120;
						m_lGrassHopperTime = 90;
						m_rGrassHopperTime = 0;
						m_gravity = 0;
						node = m_model->FindNode("GrassHopperLegRPoint");
						mat = node->m_worldTransform * m_mWorld;
						mat._42 += 0.7f;
						m_weaponList[3]->GrassHopper({ mat._41,mat._42,mat._43 }, m_mWorldRot.y);

						KdEffekseerManager::GetInstance().
							Play(fileName, { m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z });
						KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect(fileName); // これでループしない
						efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)) * Math::Matrix::CreateTranslation({ m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z });
						KdEffekseerManager::GetInstance().SetWorldMatrix(fileName, efcMat);
						break;
					case 1:
						m_grassHopperDashDir = Math::Vector3::TransformNormal(Math::Vector3(0, 0, -1), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
						m_grassHopperDashDir.Normalize();
						m_bMove = true;
						m_EnemyState &= ~grassHopperDash;
						m_EnemyState |= grassHopperDashB;
						m_EnemyState &= ~grassHopperDashUp;
						m_lGrassHopperPauCnt = 120;
						m_lGrassHopperTime = 90;
						m_rGrassHopperTime = 0;
						m_gravity = 0;
						node = m_model->FindNode("GrassHopperLegRPoint");
						mat = node->m_worldTransform * m_mWorld;
						mat._42 += 0.7f;
						m_weaponList[3]->GrassHopper({ mat._41,mat._42,mat._43 }, m_mWorldRot.y);

						vec = m_mWorld.Forward();
						KdEffekseerManager::GetInstance().
							Play(fileName, { m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z });
						KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect(fileName); // これでループしない
						efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y + 180.0f)) * Math::Matrix::CreateTranslation({ m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z });
						KdEffekseerManager::GetInstance().SetWorldMatrix(fileName, efcMat);
						break;
					case 2:
						m_grassHopperDashDir = Math::Vector3::TransformNormal(Math::Vector3(1, 0, 0), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
						m_grassHopperDashDir.Normalize();
						m_bMove = true;
						m_EnemyState &= ~grassHopperDash;
						m_EnemyState |= grassHopperDashR;
						m_EnemyState &= ~grassHopperDashUp;
						m_lGrassHopperPauCnt = 120;
						m_lGrassHopperTime = 90;
						m_rGrassHopperTime = 0;
						m_gravity = 0;
						node = m_model->FindNode("GrassHopperLegLPoint");
						mat = node->m_worldTransform * m_mWorld;
						mat._42 += 0.7f;
						m_weaponList[3]->GrassHopper({ mat._41,mat._42,mat._43 }, m_mWorldRot.y);

						vec = m_mWorld.Right();
						KdEffekseerManager::GetInstance().
							Play(fileName, { m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z });
						KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect(fileName); // これでループしない
						efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y + 270.0f)) * Math::Matrix::CreateTranslation({ m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z });
						KdEffekseerManager::GetInstance().SetWorldMatrix(fileName, efcMat);
						break;
					case 3:
						m_grassHopperDashDir = Math::Vector3::TransformNormal(Math::Vector3(-1, 0, 0), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
						m_grassHopperDashDir.Normalize();
						m_bMove = true;
						m_EnemyState &= ~grassHopperDash;
						m_EnemyState |= grassHopperDashL;
						m_EnemyState &= ~grassHopperDashUp;
						m_lGrassHopperPauCnt = 120;
						m_lGrassHopperTime = 90;
						m_rGrassHopperTime = 0;
						m_gravity = 0;
						node = m_model->FindNode("GrassHopperLegRPoint");
						mat = node->m_worldTransform * m_mWorld;
						mat._42 += 0.7f;
						m_weaponList[3]->GrassHopper({ mat._41,mat._42,mat._43 }, m_mWorldRot.y);

						vec = m_mWorld.Left();
						KdEffekseerManager::GetInstance().
							Play(fileName, { m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z });
						KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect(fileName); // これでループしない
						efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y + 90.0f)) * Math::Matrix::CreateTranslation({ m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z });
						KdEffekseerManager::GetInstance().SetWorldMatrix(fileName, efcMat);
						break;
					case 4:
						m_grassHopperDashDir = { 0, 1, 0 };
						m_grassHopperDashDir.Normalize();
						m_bMove = true;
						m_EnemyState &= ~grassHopperDash;
						m_EnemyState |= grassHopperDashUp;
						m_lGrassHopperPauCnt = 120;
						m_lGrassHopperTime = 90;
						m_rGrassHopperTime = 0;
						m_gravity = 0;

						node = m_model->FindNode("GrassHopperLegRPoint");
						mat = node->m_worldTransform * m_mWorld;
						mat._42 += 0.7f;
						m_weaponList[3]->GrassHopper({ mat._41,mat._42,mat._43 }, m_mWorldRot.y);

						KdEffekseerManager::GetInstance().
							Play(fileName, { m_pos.x,m_pos.y + 2.9f,m_pos.z });
						KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect(fileName); // これでループしない
						efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(270.0f)) * Math::Matrix::CreateTranslation({ m_pos.x,m_pos.y + 2.9f,m_pos.z });
						KdEffekseerManager::GetInstance().SetWorldMatrix(fileName, efcMat);
						break;
					}
					break;

				}
			}
			m_grassSuccessionDelayCnt = 40;
		}
	}
}

void Enemy::ScorpionDefenseDecision()
{
	if (!(m_EnemyState & (rAttack | lAttack | defense | mantis | rlAttack | rlAttackRush)))
	{
		m_EnemyState = defense;
		m_EnemyState &= defense;
		m_bMove = true;
		m_animator->SetAnimation(m_model->GetAnimation("Defense"), true);
	}
}

void Enemy::GrassMove()
{
	--m_lGrassHopperTime;
	if (m_lGrassHopperTime <= 0)
	{
		m_lGrassHopperTime = 0;
	}

	--m_rGrassHopperTime;
	if (m_rGrassHopperTime <= 0)
	{
		m_rGrassHopperTime = 0;
	}

	if (m_lGrassHopperTime != 0 || m_rGrassHopperTime != 0)
	{
		std::string fileName;
		switch (m_enemyNumber)
		{
		case 1:
			fileName = "GrassDashBlurEnemyOne.efk";
			break;
		case 2:
			fileName = "GrassDashBlurEnemyTwo.efk";
			break;
		case 3:
			fileName = "GrassDashBlurEnemyThree.efk";
			break;
		case 4:
			fileName = "GrassDashBlurEnemyFour.efk";
			break;
		case 5:
			fileName = "GrassDashBlurEnemyFive.efk";
			break;
		}

		if (m_EnemyState & grassHopperDashF)
		{
			Math::Vector3 vec = m_mWorld.Backward();
			Math::Matrix efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)) * Math::Matrix::CreateTranslation({ m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z });
			KdEffekseerManager::GetInstance().SetWorldMatrix(fileName, efcMat);
		}
		else if (m_EnemyState & grassHopperDashB)
		{
			Math::Vector3 vec = m_mWorld.Forward();
			Math::Matrix efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y + 180.0f)) * Math::Matrix::CreateTranslation({ m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z });
			KdEffekseerManager::GetInstance().SetWorldMatrix(fileName, efcMat);
		}
		else if (m_EnemyState & grassHopperDashR)
		{
			Math::Vector3 vec = m_mWorld.Right();
			Math::Matrix efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y + 90.0f)) * Math::Matrix::CreateTranslation({ m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z });
			KdEffekseerManager::GetInstance().SetWorldMatrix(fileName, efcMat);
		}
		else if (m_EnemyState & grassHopperDashL)
		{
			Math::Vector3 vec = m_mWorld.Left();
			Math::Matrix efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y + 270.0f)) * Math::Matrix::CreateTranslation({ m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z });
			KdEffekseerManager::GetInstance().SetWorldMatrix(fileName, efcMat);
		}
		else if (m_EnemyState & grassHopperDashUp)
		{
			Math::Matrix efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(270.0f)) * Math::Matrix::CreateTranslation({ m_pos.x,m_pos.y + 2.9f,m_pos.z });
			KdEffekseerManager::GetInstance().SetWorldMatrix(fileName, efcMat);
		}

		m_bMove = true;
		if (m_lGrassHopperTime <= (*m_wpJsonObj.lock())["GrassDashMaxTime"].int_value() && m_lGrassHopperTime > (*m_wpJsonObj.lock())["GrassDashSteppingAwayMoment"].int_value() ||
			m_rGrassHopperTime <= (*m_wpJsonObj.lock())["GrassDashMaxTime"].int_value() && m_rGrassHopperTime > (*m_wpJsonObj.lock())["GrassDashSteppingAwayMoment"].int_value())
		{
			m_dashSpd = static_cast<float>((*m_wpJsonObj.lock())["GrassDashSpeed"][0].number_value());
			float y = 0.0f;
			if (!m_target.expired())
			{
				y = m_target.lock()->GetPos().y - m_pos.y;
			}

			if (y >= -0.5f && y <= 0.5f)
			{
				if (m_EnemyState & grassHopperDashUp)
				{
					m_wantToMoveState = Enemy::WantToMoveState::wDashAttack;
					m_dashSpd = static_cast<float>((*m_wpJsonObj.lock())["GrassDashSpeed"][0].number_value());
					m_grassHopperDashDir = {};
					m_gravity = 0;
					m_rGrassHopperPauCnt = 0;
					m_lGrassHopperPauCnt = 0;
					GrassMoveVecDecision();
				}
			}
		}
		else if (m_lGrassHopperTime == (*m_wpJsonObj.lock())["GrassDashSteppingAwayMoment"].int_value() ||
			     m_rGrassHopperTime == (*m_wpJsonObj.lock())["GrassDashSteppingAwayMoment"].int_value())
		{
			KdAudioManager::Instance().Play3D("Asset/Audio/SE/GrassHopeer.wav", { m_pos.x, m_pos.y + 0.3f,m_pos.z });
			if (m_enemyType & EnemyType::speedSter)
			{
				m_dashSpd = static_cast<float>((*m_wpJsonObj.lock())["SpeedSterGrassDashSpeed"][0].number_value());
			}
			else
			{
				m_dashSpd = static_cast<float>((*m_wpJsonObj.lock())["GrassDashSpeed"][1].number_value());
			}
		}
		else if (m_lGrassHopperTime == (*m_wpJsonObj.lock())["GrassDashSpeeedDecelerationTime"][0].int_value() ||
			     m_rGrassHopperTime == (*m_wpJsonObj.lock())["GrassDashSpeeedDecelerationTime"][0].int_value())
		{
			if (m_enemyType & EnemyType::speedSter)
			{
				m_dashSpd = static_cast<float>((*m_wpJsonObj.lock())["SpeedSterGrassDashSpeed"][1].number_value());
			}
			else
			{
				m_dashSpd = static_cast<float>((*m_wpJsonObj.lock())["GrassDashSpeed"][2].number_value());
			}
		}
		else if (m_lGrassHopperTime == (*m_wpJsonObj.lock())["GrassDashSpeeedDecelerationTime"][1].int_value() ||
			     m_rGrassHopperTime == (*m_wpJsonObj.lock())["GrassDashSpeeedDecelerationTime"][1].int_value())
		{
			if (m_enemyType & EnemyType::speedSter)
			{
				m_dashSpd = static_cast<float>((*m_wpJsonObj.lock())["SpeedSterGrassDashSpeed"][2].number_value());
			}
			else
			{
				m_dashSpd = static_cast<float>((*m_wpJsonObj.lock())["GrassDashSpeed"][3].number_value());
			}
		}
		else if (m_lGrassHopperTime == (*m_wpJsonObj.lock())["GrassDashSpeeedDecelerationTime"][2].int_value() ||
			     m_rGrassHopperTime == (*m_wpJsonObj.lock())["GrassDashSpeeedDecelerationTime"][2].int_value())
		{
			if (m_enemyType & EnemyType::speedSter)
			{
				m_dashSpd = static_cast<float>((*m_wpJsonObj.lock())["SpeedSterGrassDashSpeed"][3].number_value());
			}
			else
			{
				m_dashSpd = static_cast<float>((*m_wpJsonObj.lock())["GrassDashSpeed"][4].number_value());
			}
		}
		else if (m_lGrassHopperTime == (*m_wpJsonObj.lock())["GrassDashSpeeedDecelerationTime"][3].int_value() ||
			     m_rGrassHopperTime == (*m_wpJsonObj.lock())["GrassDashSpeeedDecelerationTime"][3].int_value())
		{
			if (m_enemyType & EnemyType::speedSter)
			{
				m_dashSpd = static_cast<float>((*m_wpJsonObj.lock())["SpeedSterGrassDashSpeed"][4].number_value());
			}
			else
			{
				m_dashSpd = static_cast<float>((*m_wpJsonObj.lock())["GrassDashSpeed"][5].number_value());
			}
		}
		else if (m_lGrassHopperTime == (*m_wpJsonObj.lock())["GrassDashSpeeedDecelerationTime"][4].int_value() ||
			     m_rGrassHopperTime == (*m_wpJsonObj.lock())["GrassDashSpeeedDecelerationTime"][4].int_value())
		{
			if (m_enemyType & EnemyType::speedSter)
			{
				m_dashSpd = static_cast<float>((*m_wpJsonObj.lock())["SpeedSterGrassDashSpeed"][5].number_value());
			}
			else
			{
				m_dashSpd = static_cast<float>((*m_wpJsonObj.lock())["GrassDashSpeed"][6].number_value());
			}
		}
		else if (m_lGrassHopperTime == (*m_wpJsonObj.lock())["GrassDashSpeeedDecelerationTime"][5].int_value() ||
			     m_rGrassHopperTime == (*m_wpJsonObj.lock())["GrassDashSpeeedDecelerationTime"][5].int_value())
		{
			if (m_enemyType & EnemyType::speedSter)
			{
				m_dashSpd = static_cast<float>((*m_wpJsonObj.lock())["SpeedSterGrassDashSpeed"][6].number_value());
			}
			else
			{
				m_dashSpd = static_cast<float>((*m_wpJsonObj.lock())["GrassDashSpeed"][7].number_value());
			}
		}

		if (m_lGrassHopperTime == (*m_wpJsonObj.lock())["GrassDashSteppingAwayJustBefore"].int_value())
		{
			m_weaponList[3]->StartAnime();
			m_animator->SetAnimation(m_model->GetAnimation("GrassDashFA"), true);
		}

		if (m_rGrassHopperTime == (*m_wpJsonObj.lock())["GrassDashSteppingAwayJustBefore"].int_value())
		{
			m_weaponList[2]->StartAnime();
			m_animator->SetAnimation(m_model->GetAnimation("GrassDashFA"), true);
		}

		Math::Vector3  src = Math::Vector3::Zero;
		if (!m_target.expired())
		{
			src = m_target.lock()->GetPos() - m_pos;
		}

		if (src.Length() <= 1.2f)
		{
			if (m_lGrassHopperTime <= (*m_wpJsonObj.lock())["GrassDashSteppingAwayMoment"].int_value() || 
				m_rGrassHopperTime <= (*m_wpJsonObj.lock())["GrassDashSteppingAwayMoment"].int_value())
			{
				if (!(m_EnemyState & (rAttackOne | rAttackTwo)))
				{
					ScorpionAttackDecision();
				}
			}
		}

		float y = 0.0f;
		if (!m_target.expired())
		{
			y = m_target.lock()->GetPos().y - m_pos.y;
		}

		if (y >= -0.35f && y <= 0.35f)
		{
			if (m_EnemyState & grassHopperDashUp)
			{
				m_wantToMoveState = Enemy::WantToMoveState::wDashAttack;
				m_dashSpd = 0.0f;
				m_grassHopperDashDir = {};
				m_gravity = 0;
				m_rGrassHopperPauCnt = 0;
				m_lGrassHopperPauCnt = 0;
				GrassMoveVecDecision();
			}
		}
	}
	else
	{
		m_dashSpd = 0.0f;
		m_grassHopperDashDir = {};
		m_gravity = 0;
		m_EnemyState = fall;
		Brain();
	}


	m_pos += m_grassHopperDashDir * m_dashSpd;

	if (m_EnemyState & grassHopperDashB | grassHopperDashF)
	{
		UpdateRotate(m_grassHopperDashDir);
	}
	else if(m_EnemyState & grassHopperDashR | grassHopperDashL)
	{
		std::shared_ptr<Player> spTarget = m_target.lock();
		if (spTarget)
		{
			Math::Vector3 vTarget = spTarget->GetPos() - m_pos;
			UpdateRotate(vTarget);
		}
	}
	else if (m_EnemyState & grassHopperDashUp)
	{
		Math::Matrix rotYMat = Math::Matrix::CreateRotationY(m_mWorldRot.y);
		Math::Vector3 forwardVec = Math::Vector3(0, 0, 1);
		Math::Vector3 toVec = Math::Vector3::TransformNormal(forwardVec, rotYMat);
		UpdateRotate(toVec);
	}
}

void Enemy::StepMove()
{
	--m_stepCnt;
	if (m_stepCnt <= 0)
	{
		m_stepCnt = 0;
	}

	if (m_stepCnt != 0)
	{
		m_bMove = true;
		if (m_stepCnt <= 60 && m_stepCnt > 50)
		{
			m_dashSpd = 0.2f;
		}
		else if (m_stepCnt <= 50 && m_stepCnt > 30)
		{
			m_dashSpd = 0.5f;
		}
		else if (m_stepCnt <= 30 && m_stepCnt > 20)
		{
			m_dashSpd = 0.2f;
		}
		else if (m_stepCnt <= 20 && m_stepCnt > 10)
		{
			m_dashSpd = 0.1f;
		}
		else if (m_stepCnt <= 10 && m_stepCnt > 0)
		{
			m_dashSpd = 0.05f;
		}

		if (m_stepCnt == 60)
		{
			if (m_EnemyState & stepF)
			{
				m_animator->SetAnimation(m_model->GetAnimation("GrassDashFA"), true);
			}
			else if (m_EnemyState & stepR)
			{
				m_animator->SetAnimation(m_model->GetAnimation("GrassDashRA"), true);
			}
			else if (m_EnemyState & stepL)
			{
				m_animator->SetAnimation(m_model->GetAnimation("GrassDashLA"), true);
			}
			else if (m_EnemyState & stepB)
			{
				m_animator->SetAnimation(m_model->GetAnimation("GrassDashBA"), true);
			}
		}
	}
	else
	{
		m_bMove = false;
		m_dashSpd = 0.0f;
		m_stepDashDir = {};
		m_gravity = 0;
		m_EnemyState = idle;
		Brain();
	}

	m_pos += m_stepDashDir * m_dashSpd;
}

void Enemy::StepVecDecision()
{
	std::random_device rnd;
	std::mt19937 mt(rnd());
	std::uniform_int_distribution<int> intRand(0, 999);
	int randNum[4] = {};

	int rand = intRand(mt);
	randNum[0] = 400;
	randNum[1] = 400;
	randNum[2] = 100;
	randNum[3] = 100;

	for (int i = 0; i < 2; i++)
	{
		rand -= randNum[i];
		if (rand < 0)
		{
			switch (i)
			{
			case 0:
				m_stepDashDir = Math::Vector3::TransformNormal(Math::Vector3(0, 1, 0), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
				m_bMove = true;
				m_stepCnt = 60;
				m_EnemyState = stepR;
				m_animator->SetAnimation(m_model->GetAnimation("GrassDashRB"), false);
				break;
			case 1:
				m_stepDashDir = Math::Vector3::TransformNormal(Math::Vector3(0, -1, 0), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
				m_bMove = true;
				m_stepCnt = 60;
				m_EnemyState = stepL;
				m_animator->SetAnimation(m_model->GetAnimation("GrassDashLB"), false);
				break;
			case 2:
				m_stepDashDir = Math::Vector3::TransformNormal(Math::Vector3(0, 0, 1), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
				m_bMove = true;
				m_stepCnt = 60;
				m_EnemyState = stepF;
				m_animator->SetAnimation(m_model->GetAnimation("GrassDashFB"), false);
				break;
			case 3:
				m_stepDashDir = Math::Vector3::TransformNormal(Math::Vector3(0, 0, -1), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
				m_bMove = true;
				m_stepCnt = 60;
				m_EnemyState = stepB;
				m_animator->SetAnimation(m_model->GetAnimation("GrassDashBB"), false);
				break;
			}
		}
	}
}

void Enemy::NormalMoveVecDecision()
{
	if (!(m_EnemyState & run))
	{
		m_animator->SetAnimation(m_model->GetAnimation("RUN"));
	}
	m_EnemyState = run;
}

void Enemy::NormalMove()
{
	Math::Vector3 moveVec = {};

	float moveSpd = 0.0f;
	if (m_bBoss && !(m_enemyType & bossEnemyTypeOne))
	{
		moveSpd = 0.25f;
	}
	else
	{
		switch(m_enemyType)
		{
		case coarseFishEnemy:
			moveSpd = 0.15f;
			break;
		case wimpEnemyTypeOne:
			moveSpd = 0.15f;
			break;
		case bossEnemyTypeOne:
			moveSpd = 0.1f;
			break;
		}
	}


	if (m_EnemyState & run)
	{
		if (m_enemyType & wimpEnemyTypeOne)
		{
			Math::Vector3 src;
			std::shared_ptr<Player> spTarget = m_target.lock();
			if (spTarget)
			{
				src = spTarget->GetPos() - m_pos;
			}

			if (src.Length() <= 20.0f)
			{
				moveVec += Math::Vector3::TransformNormal({ 0, 0, -1 }, Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
			}
			else
			{
				moveVec += Math::Vector3::TransformNormal({ 0, 0, 1 }, Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
			}
		}
		else
		{
			moveVec += Math::Vector3::TransformNormal({ 0, 0, 1 }, Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
		}
		m_bMove = true;
	}

	if (m_EnemyState & jump)
	{
		if (!(m_EnemyState & (jump | fall)))
		{
			m_gravity = -0.2f;
			m_EnemyState |= jump;
			m_bMove = true;

		}
	}

	if (!(m_EnemyState & (grassHopperDash | grassHopperDashUp)))
	{
		m_pos += moveVec * moveSpd;
	}

	Brain();
}

void Enemy::ScorpionDefenseMove()
{
	std::shared_ptr<Player> spTarget = m_target.lock();
	if (spTarget)
	{
		Math::Vector3 vTarget = spTarget->GetPos() - m_pos;
		Math::Vector3 src = spTarget->GetPos() - m_pos;

		if (spTarget->GetPlayerState() & (Player::PlayerState::rAttack | Player::PlayerState::lAttack | Player::PlayerState::rlAttack | Player::PlayerState::grassHopperDashF))
		{
			m_bMove = true;
			UpdateRotate(vTarget);
		}
		else
		{
			Brain();
		}

		if (spTarget->GetPlayerState() & Player::PlayerState::hasDefense)
		{
			m_wantToMoveState = wAttack;
		}
	}
}

void Enemy::HasDefenseMove()
{
	m_bMove = true;
	if (m_animator->IsAnimationEnd())
	{
		--m_hasDeTime;
		if (m_hasDeTime <= 0)
		{
			Brain();
			m_EnemyState = idle;
			m_hasDeTime = 0;
		}
	}
}

void Enemy::Brain()
{
	switch (m_enemyType)
	{
	case Enemy::EnemyType::striker:
		StrikerBrain();
		break;
	case Enemy::EnemyType::defender:
		DefenderBrain();
		break;
	case Enemy::EnemyType::speedSter:
		SpeedSterBrain();
		break;
	case Enemy::EnemyType::allRounder:
		if (m_bEnemyBetweenPlayer)
		{
			m_wantToMoveState = Enemy::WantToMoveState::wGrassDash;

			m_leftWeaponNumber = 2;
			m_rightWeaponNumber = 1;
		}
		else
		{
			AllRounderBrain();
		}

		break;
	case Enemy::EnemyType::coarseFishEnemy:
		CoarseFishEnemyBrain();
		break;
	case Enemy::EnemyType::wimpEnemyTypeOne:
		WimpEnemyBrain();
		break;
	case Enemy::EnemyType::bossEnemyTypeOne:
		BossEnemyTypeOneBrain();
		break;
	}

	if (!m_bEnemyBetweenPlayer)
	{
		float y = 0.0f;
		if (!m_target.expired())
		{
			y = m_target.lock()->GetPos().y - m_pos.y;
		}

		if (y <= -0.85f)
		{
			if (!(m_EnemyState & (hit | hasDefense)))
			{
				if (!(m_EnemyState & fall))
				{
					m_animator->SetAnimation(m_model->GetAnimation("FallA"), false);
				}
				m_EnemyState = fall;
				m_wantToMoveState = wNone;
			}
		}
	}
}

void Enemy::StrikerBrain()
{
	std::random_device rnd;
	std::mt19937 mt(rnd());
	std::uniform_int_distribution<int> intRand(0, 999);
	int randNum[9] = {};
	Math::Vector3 src;

	std::shared_ptr<Player> spTarget = m_target.lock();
	if (spTarget)
	{
		Math::Vector3 vTarget = spTarget->GetPos() - m_pos;
		src = spTarget->GetPos() - m_pos;
	}

	int rand = intRand(mt);

	if (src.Length() <= 1.2f)
	{
		if (spTarget && spTarget->GetPlayerState() & (Player::PlayerState::rAttack | Player::PlayerState::lAttack | Player::PlayerState::rlAttack))
		{
			randNum[0] = 450;
			randNum[1] = 550;
			for (int i = 0; i < 2; i++)
			{
				rand -= randNum[i];
				if (rand < 0)
				{
					switch (i)
					{
					case 0:
						m_wantToMoveCategory = Enemy::WantToMoveCategory::defenseCategory;
						break;
					case 1:
						m_wantToMoveCategory = Enemy::WantToMoveCategory::attackCategory;
						break;
					}
					break;
				}
			}
		}
		else
		{
			rand = intRand(mt);
			randNum[0] = 750;
			randNum[1] = 250;
			for (int i = 0; i < 2; i++)
			{
				rand -= randNum[i];
				if (rand < 0)
				{
					switch (i)
					{
					case 0:
						m_wantToMoveCategory = Enemy::WantToMoveCategory::attackCategory;
						break;
					case 1:
						m_wantToMoveCategory = Enemy::WantToMoveCategory::defenseCategory;
						break;
					}
					break;
				}
			}

		}

	}
	else
	{
		if (src.Length() <= 8.0f && spTarget && spTarget->GetPlayerState() & Player::PlayerState::grassHopperDashF)
		{
			rand = intRand(mt);
			randNum[0] = 650;
			randNum[1] = 350;
			for (int i = 0; i < 2; i++)
			{
				rand -= randNum[i];
				if (rand < 0)
				{
					switch (i)
					{
					case 0:
						m_wantToMoveCategory = Enemy::WantToMoveCategory::attackCategory; rand = intRand(mt);
						randNum[0] = 700;
						randNum[1] = 300;
						for (int j = 0; j < 2; j++)
						{
							rand -= randNum[j];
							if (rand < 0)
							{
								switch (i)
								{
								case 1:
									m_bMantisAttack = true;
									break;
								case 2:
									m_bMantisAttack = false;
									break;
								}
							}
						}
						break;
					case 1:
						m_wantToMoveCategory = Enemy::WantToMoveCategory::approachCategory;
						break;
					}
					break;
				}
			}
		}
		else if (src.Length() <= 2.5f && spTarget && spTarget->GetPlayerState() & Player::PlayerState::defense && !(m_EnemyState & grassHopperDash))
		{
			rand = intRand(mt);
			randNum[0] = 600;
			randNum[1] = 400;
			for (int i = 0; i < 2; i++)
			{
				rand -= randNum[i];
				if (rand < 0)
				{
					switch (i)
					{
					case 0:
						m_wantToMoveCategory = Enemy::WantToMoveCategory::attackCategory;
						break;
					case 1:
						m_wantToMoveCategory = Enemy::WantToMoveCategory::approachCategory;
						break;
					}
					break;
				}
			}
		}
		else
		{
			m_wantToMoveCategory = Enemy::WantToMoveCategory::approachCategory;
		}
	}

	switch (m_wantToMoveCategory)
	{
	case Enemy::WantToMoveCategory::attackCategory:
		m_wantToMoveState = Enemy::WantToMoveState::wAttack;
		if (spTarget && spTarget->GetPlayerState() & Player::PlayerState::defense && !(m_EnemyState & grassHopperDash))
		{
			m_bMantisAttack = true;
		}
		break;
	case Enemy::WantToMoveCategory::defenseCategory:
		rand = intRand(mt);
		randNum[0] = 800;
		randNum[1] = 200;
		for (int i = 0; i < 2; i++)
		{
			rand -= randNum[i];
			if (rand < 0)
			{
				switch (i)
				{
				case 0:
					m_wantToMoveState = Enemy::WantToMoveState::wStep;
					break;
				case 1:
					m_wantToMoveState = Enemy::WantToMoveState::wDefense;
					break;
				}
				break;
			}
		}
		break;
	case Enemy::WantToMoveCategory::approachCategory:
		rand = intRand(mt);
		randNum[0] = 990;
		randNum[1] = 10;

		for (int i = 0; i < 2; i++)
		{
			rand -= randNum[i];
			if (rand < 0)
			{

				switch (i)
				{
				case 0:
					m_wantToMoveCategory = Enemy::WantToMoveCategory::runCategory;
					break;
				case 1:
					m_wantToMoveCategory = Enemy::WantToMoveCategory::grassCategory;
					break;
				}
				break;
			}
		}
		break;
	}

	switch (m_wantToMoveCategory)
	{
	case Enemy::WantToMoveCategory::runCategory:
		m_wantToMoveState = Enemy::WantToMoveState::wRun;
		break;
	case Enemy::WantToMoveCategory::grassCategory:
		m_wantToMoveState = Enemy::WantToMoveState::wDashAttack;
		break;
	}

	switch (m_wantToMoveState)
	{
	case WantToMoveState::wAttack:
		m_leftWeaponNumber = 1;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::wEscape:
		m_leftWeaponNumber = 1;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::wDefense:
		m_leftWeaponNumber = 1;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::wDashAttack:
		m_leftWeaponNumber = 2;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::wRun:
		m_leftWeaponNumber = 1;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::wStep:
		m_leftWeaponNumber = 1;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::wGrassDash:
		m_leftWeaponNumber = 1;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::wAvoidance:
		m_leftWeaponNumber = 1;
		m_rightWeaponNumber = 1;
		break;
	}
}

void Enemy::DefenderBrain()
{
	std::random_device rnd;
	std::mt19937 mt(rnd());
	std::uniform_int_distribution<int> intRand(0, 999);
	int randNum[9] = {};
	Math::Vector3 src;

	std::shared_ptr<Player> spTarget = m_target.lock();
	if (spTarget)
	{
		Math::Vector3 vTarget = spTarget->GetPos() - m_pos;
		src = spTarget->GetPos() - m_pos;
	}

	int rand = intRand(mt);

	if (src.Length() <= 1.2f)
	{
		if (spTarget && spTarget->GetPlayerState() & (Player::PlayerState::rAttack | Player::PlayerState::lAttack | Player::PlayerState::rlAttack))
		{
			randNum[0] = 950;
			randNum[1] = 50;
			for (int i = 0; i < 2; i++)
			{
				rand -= randNum[i];
				if (rand < 0)
				{
					switch (i)
					{
					case 0:
						m_wantToMoveCategory = Enemy::WantToMoveCategory::defenseCategory;
						break;
					case 1:
						m_wantToMoveCategory = Enemy::WantToMoveCategory::attackCategory;
						break;
					}
					break;
				}
			}
		}
		else
		{
			rand = intRand(mt);
			randNum[0] = 650;
			randNum[1] = 350;
			for (int i = 0; i < 2; i++)
			{
				rand -= randNum[i];
				if (rand < 0)
				{
					switch (i)
					{
					case 0:
						m_wantToMoveCategory = Enemy::WantToMoveCategory::attackCategory;
						break;
					case 1:
						m_wantToMoveCategory = Enemy::WantToMoveCategory::defenseCategory;
						break;
					}
					break;
				}
			}

		}

	}
	else
	{
		if (src.Length() <= 5.0f && spTarget->GetPlayerState() & Player::PlayerState::grassHopperDashF)
		{
			randNum[0] = 850;
			randNum[1] = 150;
			for (int i = 0; i < 2; i++)
			{
				rand -= randNum[i];
				if (rand < 0)
				{
					switch (i)
					{
					case 0:
						m_wantToMoveCategory = Enemy::WantToMoveCategory::defenseCategory;
						break;
					case 1:
						m_wantToMoveCategory = Enemy::WantToMoveCategory::approachCategory;
						break;
					}
					break;
				}
			}
		}
		else if (src.Length() <= 2.5f && !m_target.expired() && m_target.lock()->GetPlayerState() & Player::PlayerState::defense && !(m_EnemyState & grassHopperDash))
		{
			randNum[0] = 600;
			randNum[1] = 400;
			for (int i = 0; i < 2; i++)
			{
				rand -= randNum[i];
				if (rand < 0)
				{
					switch (i)
					{
					case 0:
						m_wantToMoveCategory = Enemy::WantToMoveCategory::attackCategory;
						break;
					case 1:
						m_wantToMoveCategory = Enemy::WantToMoveCategory::approachCategory;
						break;
					}
					break;
				}
			}
		}
		else
		{
			m_wantToMoveCategory = Enemy::WantToMoveCategory::approachCategory;
		}
	}

	switch (m_wantToMoveCategory)
	{
	case Enemy::WantToMoveCategory::attackCategory:
		m_wantToMoveState = Enemy::WantToMoveState::wAttack;
		if (!m_target.expired() && m_target.lock()->GetPlayerState() & Player::PlayerState::defense && !(m_EnemyState & grassHopperDash))
		{
			m_bMantisAttack = true;
		}
		break;
	case Enemy::WantToMoveCategory::defenseCategory:
		m_wantToMoveState = Enemy::WantToMoveState::wDefense;
		break;
	case Enemy::WantToMoveCategory::approachCategory:
		m_wantToMoveCategory = Enemy::WantToMoveCategory::runCategory;
		break;
	}

	switch (m_wantToMoveCategory)
	{
	case Enemy::WantToMoveCategory::runCategory:
		m_wantToMoveState = Enemy::WantToMoveState::wRun;
		break;
	}

	switch (m_wantToMoveState)
	{
	case WantToMoveState::wAttack:
		m_leftWeaponNumber = 1;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::wEscape:
		m_leftWeaponNumber = 2;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::wDefense:
		m_leftWeaponNumber = 1;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::wDashAttack:
		m_leftWeaponNumber = 2;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::wRun:
		m_leftWeaponNumber = 1;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::wStep:
		m_leftWeaponNumber = 1;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::wGrassDash:
		m_leftWeaponNumber = 2;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::wAvoidance:
		m_leftWeaponNumber = 2;
		m_rightWeaponNumber = 1;
		break;
	}
}

void Enemy::SpeedSterBrain()
{
	std::random_device rnd;
	std::mt19937 mt(rnd());
	std::uniform_int_distribution<int> intRand(0, 999);
	int randNum[9] = {};
	Math::Vector3 src;

	std::shared_ptr<Player> spTarget = m_target.lock();
	if (spTarget)
	{
		Math::Vector3 vTarget = spTarget->GetPos() - m_pos;
		src = spTarget->GetPos() - m_pos;
	}

	int rand = intRand(mt);

	if (src.Length() <= 1.2f)
	{
		if (spTarget && spTarget->GetPlayerState() & (Player::PlayerState::rAttack | Player::PlayerState::lAttack | Player::PlayerState::rlAttack))
		{
			randNum[0] = 750;
			randNum[1] = 250;
			for (int i = 0; i < 2; i++)
			{
				rand -= randNum[i];
				if (rand < 0)
				{
					switch (i)
					{
					case 0:
						m_wantToMoveCategory = Enemy::WantToMoveCategory::defenseCategory;
						break;
					case 1:
						m_wantToMoveCategory = Enemy::WantToMoveCategory::attackCategory;
						break;
					}
					break;
				}
			}
		}
		else
		{
			rand = intRand(mt);
			randNum[0] = 750;
			randNum[1] = 250;
			for (int i = 0; i < 2; i++)
			{
				rand -= randNum[i];
				if (rand < 0)
				{
					switch (i)
					{
					case 0:
						m_wantToMoveCategory = Enemy::WantToMoveCategory::attackCategory;
						break;
					case 1:
						m_wantToMoveCategory = Enemy::WantToMoveCategory::defenseCategory;
						break;
					}
					break;
				}
			}

		}

	}
	else
	{
		if (src.Length() <= 5.0f && spTarget && spTarget->GetPlayerState() & Player::PlayerState::grassHopperDashF)
		{
			randNum[0] = 150;
			randNum[1] = 850;
			for (int i = 0; i < 2; i++)
			{
				rand -= randNum[i];
				if (rand < 0)
				{
					switch (i)
					{
					case 0:
						m_wantToMoveCategory = Enemy::WantToMoveCategory::defenseCategory;
						break;
					case 1:
						m_wantToMoveCategory = Enemy::WantToMoveCategory::approachCategory;
						break;
					}
					break;
				}
			}
		}
		else if (src.Length() <= 2.5f && !m_target.expired() && m_target.lock()->GetPlayerState() & Player::PlayerState::defense && !(m_EnemyState & grassHopperDash))
		{
			randNum[0] = 600;
			randNum[1] = 400;
			for (int i = 0; i < 2; i++)
			{
				rand -= randNum[i];
				if (rand < 0)
				{
					switch (i)
					{
					case 0:
						m_wantToMoveCategory = Enemy::WantToMoveCategory::attackCategory;
						break;
					case 1:
						m_wantToMoveCategory = Enemy::WantToMoveCategory::approachCategory;
						break;
					}
					break;
				}
			}
		}
		else
		{
			m_wantToMoveCategory = Enemy::WantToMoveCategory::approachCategory;
		}
	}

	switch (m_wantToMoveCategory)
	{
	case Enemy::WantToMoveCategory::attackCategory:
		if (!m_target.expired() && m_target.lock()->GetPlayerState() & Player::PlayerState::defense && !(m_EnemyState & grassHopperDash))
		{
			m_wantToMoveState = Enemy::WantToMoveState::wAttack;
			m_bMantisAttack = true;
		}
		else
		{
			m_wantToMoveState = Enemy::WantToMoveState::wDashAttack;
		}
		break;
	case Enemy::WantToMoveCategory::defenseCategory:
		rand = intRand(mt);
		randNum[0] = 500;
		randNum[1] = 500;
		for (int i = 0; i < 3; i++)
		{
			rand -= randNum[i];
			if (rand < 0)
			{
				switch (i)
				{
				case 0:
					m_wantToMoveState = Enemy::WantToMoveState::wAvoidance;
					break;
				case 1:
					m_wantToMoveState = Enemy::WantToMoveState::wEscape;
					break;
				}
				break;
			}
		}
		break;
	case Enemy::WantToMoveCategory::approachCategory:
		m_wantToMoveCategory = Enemy::WantToMoveCategory::grassCategory;
		break;
	}

	switch (m_wantToMoveCategory)
	{
		break;
	case Enemy::WantToMoveCategory::grassCategory:
		rand = intRand(mt);
		if (src.Length() >= 10.0f)
		{
			randNum[0] = 250;
			randNum[1] = 850;
		}
		else if (src.Length() >= 5.0f)
		{
			randNum[0] = 500;
			randNum[1] = 500;
		}
		else if (src.Length() < 5.0f)
		{
			randNum[0] = 300;
			randNum[1] = 700;
		}

		for (int i = 0; i < 2; i++)
		{
			rand -= randNum[i];
			if (rand < 0)
			{
				switch (i)
				{
				case 0:
					m_wantToMoveState = Enemy::WantToMoveState::wGrassDash;
					break;
				case 1:
					m_wantToMoveState = Enemy::WantToMoveState::wDashAttack;
					break;
				}
				break;
			}
		}
		break;
	}

	switch (m_wantToMoveState)
	{
	case WantToMoveState::wAttack:
		m_leftWeaponNumber = 1;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::wEscape:
		m_leftWeaponNumber = 2;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::wDefense:
		m_leftWeaponNumber = 1;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::wDashAttack:
		m_leftWeaponNumber = 2;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::wRun:
		m_leftWeaponNumber = 1;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::wStep:
		m_leftWeaponNumber = 1;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::wGrassDash:
		m_leftWeaponNumber = 2;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::wAvoidance:
		m_leftWeaponNumber = 2;
		m_rightWeaponNumber = 1;
		break;
	}
}

void Enemy::AllRounderBrain()
{
	std::random_device rnd;
	std::mt19937 mt(rnd());
	std::uniform_int_distribution<int> intRand(0, 999);
	int randNum[9] = {};
	Math::Vector3 src;

	std::shared_ptr<Player> spTarget = m_target.lock();
	if (spTarget)
	{
		Math::Vector3 vTarget = spTarget->GetPos() - m_pos;
		src = spTarget->GetPos() - m_pos;
	}

	int rand = intRand(mt);

	if (src.Length() <= 8.0f && spTarget)
	{
		if (spTarget && spTarget->GetPlayerState() & (Player::PlayerState::rAttack | Player::PlayerState::lAttack | Player::PlayerState::rlAttack))
		{
			randNum[0] = 750;
			randNum[1] = 250;
			for (int i = 0; i < 2; i++)
			{
				rand -= randNum[i];
				if (rand < 0)
				{
					switch (i)
					{
					case 0:
						m_wantToMoveCategory = Enemy::WantToMoveCategory::defenseCategory;
						break;
					case 1:
						m_wantToMoveCategory = Enemy::WantToMoveCategory::attackCategory;
						break;
					}
					break;
				}
			}
		}
		else
		{
			rand = intRand(mt);
			randNum[0] = 750;
			randNum[1] = 250;
			for (int i = 0; i < 2; i++)
			{
				rand -= randNum[i];
				if (rand < 0)
				{
					switch (i)
					{
					case 0:
						m_wantToMoveCategory = Enemy::WantToMoveCategory::attackCategory;
						break;
					case 1:
						m_wantToMoveCategory = Enemy::WantToMoveCategory::defenseCategory;
						break;
					}
					break;
				}
			}

		}

	}
	else
	{
		if (src.Length() <= 5.0f && spTarget && spTarget->GetPlayerState() & Player::PlayerState::grassHopperDashF)
		{
			randNum[0] = 750;
			randNum[1] = 250;
			for (int i = 0; i < 2; i++)
			{
				rand -= randNum[i];
				if (rand < 0)
				{
					switch (i)
					{
					case 0:
						m_wantToMoveCategory = Enemy::WantToMoveCategory::defenseCategory;
						break;
					case 1:
						m_wantToMoveCategory = Enemy::WantToMoveCategory::approachCategory;
						break;
					}
					break;
				}
			}
		}
		else if (src.Length() <= 2.5f && !m_target.expired() && m_target.lock()->GetPlayerState() & Player::PlayerState::defense && !(m_EnemyState & grassHopperDash))
		{
			randNum[0] = 600;
			randNum[1] = 400;
			for (int i = 0; i < 2; i++)
			{
				rand -= randNum[i];
				if (rand < 0)
				{
					switch (i)
					{
					case 0:
						m_wantToMoveCategory = Enemy::WantToMoveCategory::attackCategory;
						break;
					case 1:
						m_wantToMoveCategory = Enemy::WantToMoveCategory::approachCategory;
						break;
					}
					break;
				}
			}
		}
		else
		{
			m_wantToMoveCategory = Enemy::WantToMoveCategory::approachCategory;
		}
	}

	switch (m_wantToMoveCategory)
	{
	case Enemy::WantToMoveCategory::attackCategory:
		m_wantToMoveState = Enemy::WantToMoveState::wAttack;
		if (!m_target.expired() && m_target.lock()->GetPlayerState() & Player::PlayerState::defense && !(m_EnemyState & grassHopperDash))
		{
			m_bMantisAttack = true;
		}
		break;
	case Enemy::WantToMoveCategory::defenseCategory:
		rand = intRand(mt);
		randNum[0] = 600;
		randNum[1] = 200;
		randNum[2] = 200;
		for (int i = 0; i < 3; i++)
		{
			rand -= randNum[i];
			if (rand < 0)
			{
				switch (i)
				{
				case 0:
					m_wantToMoveState = Enemy::WantToMoveState::wDefense;
					break;
				case 1:
					m_wantToMoveState = Enemy::WantToMoveState::wAvoidance;
					break;
				case 2:
					m_wantToMoveState = Enemy::WantToMoveState::wEscape;
					break;
				}
				break;
			}
		}
		break;
	case Enemy::WantToMoveCategory::approachCategory:
		rand = intRand(mt);
		randNum[0] = 900;
		randNum[1] = 100;

		for (int i = 0; i < 3; i++)
		{
			rand -= randNum[i];
			if (rand < 0)
			{
				if (src.Length() <= 25.0f)
				{
					switch (i)
					{
					case 0:
						m_wantToMoveCategory = Enemy::WantToMoveCategory::runCategory;
						break;
					case 1:
						m_wantToMoveCategory = Enemy::WantToMoveCategory::grassCategory;
						break;
					}
					break;
				}
				else
				{
					switch (i)
					{
					case 0:
						m_wantToMoveCategory = Enemy::WantToMoveCategory::grassCategory;
						break;
					case 1:
						m_wantToMoveCategory = Enemy::WantToMoveCategory::runCategory;
						break;
					}
					break;
				}
			}
		}
		break;
	}

	switch (m_wantToMoveCategory)
	{
	case Enemy::WantToMoveCategory::runCategory:
		m_wantToMoveState = Enemy::WantToMoveState::wRun;
		break;
	case Enemy::WantToMoveCategory::grassCategory:
		rand = intRand(mt);
		if (src.Length() >= 10.0f)
		{
			randNum[0] = 250;
			randNum[1] = 850;
		}
		else
		{
			randNum[0] = 500;
			randNum[1] = 500;
		}

		for (int i = 0; i < 2; i++)
		{
			rand -= randNum[i];
			if (rand < 0)
			{
				switch (i)
				{
				case 0:
					m_wantToMoveState = Enemy::WantToMoveState::wGrassDash;
					break;
				case 1:
					m_wantToMoveState = Enemy::WantToMoveState::wDashAttack;
					break;
				}
				break;
			}
		}
		break;
	}

	switch (m_wantToMoveState)
	{
	case WantToMoveState::wAttack:
		m_leftWeaponNumber = 1;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::wEscape:
		m_leftWeaponNumber = 2;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::wDefense:
		m_leftWeaponNumber = 1;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::wDashAttack:
		m_leftWeaponNumber = 2;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::wRun:
		m_leftWeaponNumber = 1;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::wStep:
		m_leftWeaponNumber = 1;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::wGrassDash:
		m_leftWeaponNumber = 2;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::wAvoidance:
		m_leftWeaponNumber = 2;
		m_rightWeaponNumber = 1;
		break;
	}
}

void Enemy::CoarseFishEnemyBrain()
{
	std::random_device rnd;
	std::mt19937 mt(rnd());
	std::uniform_int_distribution<int> intRand(0, 999);
	int randNum[2] = {};
	Math::Vector3 src;

	std::shared_ptr<Player> spTarget = m_target.lock();
	if (spTarget)
	{
		Math::Vector3 vTarget = spTarget->GetPos() - m_pos;
		src = spTarget->GetPos() - m_pos;
	}

	int rand = intRand(mt);

	if (m_coarseFishEnemyAttackDelayCnt == 0 && m_enemyAttackMaxTotal > m_enemyAttackTotal)
	{
		if (src.Length() <= 2.0f)
		{
			m_wantToMoveState = Enemy::WantToMoveState::wNone;
		}
		else if (src.Length() <= 10.0f)
		{
			randNum[0] = 999;
			randNum[1] =   1;

			for (int i = 0; i < 2; i++)
			{
				rand -= randNum[i];
				if (rand < 0)
				{

					switch (i)
					{
					case 0:
						m_wantToMoveState = Enemy::WantToMoveState::wRun;
						break;
					case 1:
						m_wantToMoveState = Enemy::WantToMoveState::wAttack;
						break;
					}
					break;
				}
			}
		}
		else
		{
			m_wantToMoveState = Enemy::WantToMoveState::wRun;
		}
	}
	else
	{
		m_wantToMoveState = Enemy::WantToMoveState::wRun;
	}
}

void Enemy::WimpEnemyBrain()
{
	std::random_device rnd;
	std::mt19937 mt(rnd());
	std::uniform_int_distribution<int> intRand(0, 999);
	int randNum[9] = {};
	Math::Vector3 src;

	const KdModelWork::Node* node = nullptr;
	Math::Matrix mat = Math::Matrix::Identity;

	node = m_model->FindNode("HitPoint");
	mat = node->m_worldTransform * m_mWorld;

	std::shared_ptr<Player> spTarget = m_target.lock();
	if (spTarget)
	{
		src = spTarget->GetPos() - mat.Translation();
	}

	int rand = intRand(mt);
	if (src.Length() <= 110.0f && src.Length() > 10.0f && m_enemyAttackMaxTotal > m_enemyAttackTotal && m_bRangedAttackCapableOfFiring)
	{
		randNum[0] = 995;
		randNum[1] =   5;

		for (int i = 0; i < 2; i++)
		{
			rand -= randNum[i];
			if (rand < 0)
			{
				switch (i)
				{
				case 0:
					m_wantToMoveState = Enemy::WantToMoveState::wRun;
					break;
				case 1:
					EnergyCharge(false);
					break;
				}
				break;
			}

		}
	}
	else
	{
		if (!m_bShotBeam && !m_bShotEnergyBullet)
		{
			m_wantToMoveState = Enemy::WantToMoveState::wRun;
		}
	}
}

void Enemy::BossEnemyTypeOneBrain()
{
	const KdModelWork::Node* node = nullptr;
	Math::Matrix mat = Math::Matrix::Identity;

	node = m_model->FindNode("HitPoint");
	mat = node->m_worldTransform * m_mWorld;

	std::random_device rnd;
	std::mt19937 mt(rnd());
	std::uniform_int_distribution<int> intRand(0, 999);
	int randNum[2] = {};
	Math::Vector3 src;

	std::shared_ptr<Player> spTarget = m_target.lock();
	if (spTarget)
	{
		src = spTarget->GetPos() - mat.Translation();
	}

	int rand = intRand(mt);

	if (m_endurance <= 500)
	{
		randNum[0] = 600;
		randNum[1] = 400;
	}
	else
	{
		randNum[0] = 850;
		randNum[1] = 150;
	}

	if (m_bRangedAttackCapableOfFiring)
	{
		if (src.Length() <= 45.0f)
		{
			for (int i = 0; i < 2; i++)
			{
				rand -= randNum[i];
				if (rand < 0)
				{
					switch (i)
					{
					case 0:
						m_wantToMoveState = Enemy::WantToMoveState::wRun;
						break;
					case 1:
						EnergyCharge(true);
						break;
					}
					break;
				}

			}
		}
		else
		{
			for (int i = 0; i < 2; i++)
			{
				rand -= randNum[i];
				if (rand < 0)
				{
					switch (i)
					{
					case 0:
						m_wantToMoveState = Enemy::WantToMoveState::wRun;
						break;
					case 1:
						EnergyCharge(false);
						break;
					}
					break;
				}

			}
		}
	}
	else
	{
		if (!m_bShotBeam && !m_bShotEnergyBullet)
		{
			m_wantToMoveState = Enemy::WantToMoveState::wRun;
		}
	}
}

void Enemy::EnergyCharge(bool a_bBeem)
{
	if (m_target.expired())return;
	m_target.lock()->SetAttackHit(false);
	m_target.lock()->SetDefenseSuc(false);
	const KdModelWork::Node* node = nullptr;
	Math::Matrix mat = Math::Matrix::Identity;
	node = m_model->FindNode("HitPoint");
	mat = node->m_worldTransform * m_mWorld;
	m_rangedAttackShotPos = mat.Translation();
	m_rangedAttackTargetPos = { m_target.lock()->GetPos().x, m_target.lock()->GetPos().y + 1.2f, m_target.lock()->GetPos().z };

	Math::Vector3 nowVec = Math::Vector3::Transform({ 0,0,1 }, Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
	nowVec.x = 0;

	nowVec.Normalize();
	Math::Vector3 targetVec = m_rangedAttackTargetPos - m_rangedAttackShotPos;
	targetVec.x = 0;

	targetVec.Normalize();
	Math::Vector3 dot = DirectX::XMVector3Dot(nowVec, targetVec);

	if (dot.x > 1)
	{
		dot.x = 1;
	}

	if (dot.x < -1)
	{
		dot.x = -1;
	}

	float Xang = DirectX::XMConvertToDegrees(acos(dot.x));

	Math::Vector3 cross = DirectX::XMVector3Cross(nowVec, targetVec);

	if (m_enemyType & wimpEnemyTypeOne)
	{
		if (m_rangedAttackShotPos.y >= m_rangedAttackTargetPos.y)
		{
			if (Xang >= 3)
			{
				Xang = 3;
			}

			Xang *= -1;
		}
		else
		{
			if (Xang >= 45)
			{
				Xang = 45;
			}
		}
	}
	else
	{

		if (Xang >= 45)
		{
			Xang = 45;
		}

		if (m_rangedAttackShotPos.y >= m_rangedAttackTargetPos.y)
		{
			Xang *= -1;
		}
	}

	if (a_bBeem)
	{
		m_bShotBeam = true;
		KdEffekseerManager::GetInstance().
			Play("Beem.efk", m_rangedAttackShotPos);
		KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("Beem.efk"); // これでループしない
		Math::Matrix efcMat = Math::Matrix::CreateScale(1.0f) * Math::Matrix::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(m_mWorldRot.y + 180), DirectX::XMConvertToRadians(Xang), 0) * Math::Matrix::CreateTranslation(m_rangedAttackShotPos);
		KdEffekseerManager::GetInstance().SetWorldMatrix("Beem.efk", efcMat);

		m_rangedAttackDir = Math::Vector3::Transform({ 0,0,1 }, Math::Matrix::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(m_mWorldRot.y), DirectX::XMConvertToRadians(-Xang), 0));
		m_rangedAttackDir.Normalize();
	}
	else
	{
		m_beamCollisionPos = m_rangedAttackShotPos;
		m_bShotEnergyBullet = true;
		
		Math::Matrix scaleMat = Math::Matrix::Identity;
		if (m_enemyType & bossEnemyTypeOne)
		{
			KdEffekseerManager::GetInstance().
				Play("BossEnergyBullet.efk", m_rangedAttackShotPos);
			KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("BossEnergyBullet.efk"); // これでループしない
			scaleMat = Math::Matrix::CreateScale(2.5f);
			Math::Matrix efcMat = scaleMat * Math::Matrix::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(m_mWorldRot.y + 180), DirectX::XMConvertToRadians(Xang), 0) * Math::Matrix::CreateTranslation(m_rangedAttackShotPos);
			KdEffekseerManager::GetInstance().SetWorldMatrix("BossEnergyBullet.efk", efcMat);
		}
		else
		{
			KdEffekseerManager::GetInstance().
				Play("EnergyBullet.efk", m_rangedAttackShotPos);
			KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("EnergyBullet.efk"); // これでループしない
			scaleMat = Math::Matrix::CreateScale(0.5f);
			Math::Matrix efcMat = scaleMat * Math::Matrix::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(m_mWorldRot.y + 180), DirectX::XMConvertToRadians(Xang), 0) * Math::Matrix::CreateTranslation(m_rangedAttackShotPos);
			KdEffekseerManager::GetInstance().SetWorldMatrix("EnergyBullet.efk", efcMat);
		}

		m_energyBulletDir = Math::Vector3::Transform({ 0,0,1 }, Math::Matrix::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(m_mWorldRot.y), DirectX::XMConvertToRadians(-Xang), 0));
		m_energyBulletDir.Normalize();
	}

	m_bRangedAttack = true;
}

void Enemy::RotationAttackMove()
{
	Math::Vector3 moveVec;
	float moveSpd = 0.25;
	moveVec += Math::Vector3::TransformNormal({ 0, 0, 1 }, Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
	m_pos += moveVec * moveSpd;

	if (m_animator->IsAnimationEnd())
	{
		m_attackAnimeCnt = 0;
		m_notHumanoidEnemyState = stand;
		m_notHumanoidEnemyState &= ~rotationAttack;
		Brain();
	}

	if (!m_target.expired())
	{
		if (m_target.lock()->GetInvincibilityTimeCnt() == 0 && m_attackAnimeCnt >= 17 && m_attackAnimeCnt <= 73)
		{
			m_target.lock()->SetAttackHit(false);
			m_target.lock()->SetDefenseSuc(false);
		}
	}

	RotationAttackChaeck();
}

void Enemy::RotationAttackChaeck()
{
	KdCollider::SphereInfo sphereInfo;

	// 球の当たったオブジェクト情報
	std::list<KdCollider::CollisionResult> retSphereList;

	sphereInfo.m_sphere.Center = m_pos + Math::Vector3(0, 12, 0);
	sphereInfo.m_sphere.Radius = 50.0f;
	// 当たり判定をしたいタイプを設定
	sphereInfo.m_type = KdCollider::TypeDamage;

#ifdef _DEBUG
	// デバック用
	m_pDebugWire->AddDebugSphere
	(
		sphereInfo.m_sphere.Center,
		sphereInfo.m_sphere.Radius,
		{ 0,0,0,1 }
	);
#endif

	// 球の当たったオブジェクト情報
	retSphereList.clear();

	// 球と当たり判定 

	if (!m_target.expired())
	{
		if (!m_target.lock()->GetAttackHit() && !m_target.lock()->GetDefenseSuc() && m_target.lock()->GetInvincibilityTimeCnt() == 0 && !m_target.lock()->GetBPlayerDeath() && m_attackAnimeCnt >= 18 && m_attackAnimeCnt <= 73)
		{
			if (!m_target.lock()->GetBPlayerLose())
			{
				m_target.lock()->Intersects
				(
					sphereInfo,
					&retSphereList
				);
			}


			// 球に当たったリスト情報から一番近いオブジェクトを検出
			float maxOverLap = 0;
			bool hit = false;
			Math::Vector3 hitDir = {}; // 当たった方向
			Math::Vector3 hitPos;
			for (auto& ret : retSphereList)
			{
				// 一番近くで当たったものを探す
				if (maxOverLap < ret.m_overlapDistance)
				{
					maxOverLap = ret.m_overlapDistance;
					hit = true;
					hitDir = ret.m_hitDir;
					hitPos = ret.m_hitPos;
				}
			}

			if (hit)
			{
				m_target.lock()->BlowingAwayAttackOnHit(m_mWorld.Backward());
				KdAudioManager::Instance().Play("Asset/Audio/SE/KickAttackHit.wav");

				hitPos.y += 0.35f;
				KdEffekseerManager::GetInstance().
					Play("Hit3.efk", hitPos);
				KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("Hit3.efk"); // これでループしない
				//KdEffekseerManager::GetInstance().SetRotation("Hit3.efk", m_mWorld.Backward(),DirectX::XMConvertToRadians(0));
				Math::Matrix efcMat = Math::Matrix::CreateScale(0.5f) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)) * Math::Matrix::CreateTranslation(hitPos);
				KdEffekseerManager::GetInstance().SetWorldMatrix("Hit3.efk", efcMat);
			}
		}
	}
}

void Enemy::ScorpionAttackMove()
{
	if (m_animator->IsAnimationEnd())
	{
		if (m_EnemyState & lAttack)
		{
			m_EnemyState &= ~lAttack;
			if (m_EnemyState & grassHopperDashF | step && m_bBoss)
			{
				m_animator->SetAnimation(m_model->GetAnimation("GrassDashFA"), true);
			}
			else
			{
				if (!m_bBoss)
				{
					m_coarseFishEnemyAttackDelayCnt = 120;
				}

				Brain();
			}
		}

		if (m_EnemyState & rAttack)
		{
			m_EnemyState &= ~rAttack;
			if (m_EnemyState & grassHopperDashF | step && m_bBoss)
			{
				m_animator->SetAnimation(m_model->GetAnimation("GrassDashFA"), true);
			}
			else
			{
				if (!m_bBoss)
				{
					m_coarseFishEnemyAttackDelayCnt = 120;
				}
				Brain();
			}
		}

		if (m_EnemyState & (rlAttack | rlAttackRush | mantis))
		{
			m_EnemyState = idle;
			Brain();
		}
	}
	else
	{
		if (!(m_EnemyState & (rlAttack | rlAttackRush)))
		{
			m_attackMoveSpd *= static_cast<float>((*m_wpJsonObj.lock())["MoveSpeedDecelerationamount"].number_value());

			if (m_EnemyState & (rAttackTwo | lAttackTwo))
			{
				if (m_attackAnimeCnt == 15)
				{
					if (!m_target.expired())
					{
						m_target.lock()->SetAttackHit(false);
						m_target.lock()->SetDefenseSuc(false);
					}
				}
			}
			else if (m_EnemyState & (rAttackThree | lAttackThree))
			{
				if (m_attackAnimeCnt == 15)
				{
					if (!m_target.expired())
					{
						m_target.lock()->SetAttackHit(false);
						m_target.lock()->SetDefenseSuc(false);
					}
				}
			}
		}
		if (!(m_EnemyState & rlAttackRush))
		{
			if (m_attackAnimeCnt >= 15)
			{
				m_attackMoveSpd *= static_cast<float>((*m_wpJsonObj.lock())["MoveSpeedStopsAbruptly"].number_value());
			}
			else
			{
				if (!m_target.expired())
				{
					Math::Vector3 dis = m_target.lock()->GetPos() - m_pos;
					if (dis.Length() <= 1.15f)
					{
						m_attackMoveSpd *= static_cast<float>((*m_wpJsonObj.lock())["MoveSpeedStopsAbruptly"].number_value());
					}
				}
			}
		}

		m_bMove = true;

		if (m_bAttackAnimeDelay)
		{
			if (!m_target.expired() && m_target.lock()->GetAttackHit())
			{
				m_wantToMoveState = WantToMoveState::wAttack;
			}

			if (m_bBoss)
			{
				switch (m_wantToMoveState)
				{
				case WantToMoveState::wAttack:
					m_leftWeaponNumber = 1;
					m_rightWeaponNumber = 1;
					break;
				}
			}
		}

		if (!(m_EnemyState & (rlAttack | rlAttackRush)))
		{
			m_attackMoveSpd *= static_cast<float>((*m_wpJsonObj.lock())["MoveSpeedDecelerationamount"].number_value());
		}
		else
		{
			if (m_EnemyState & rlAttackOne)
			{
				m_attackMoveSpd *= static_cast<float>((*m_wpJsonObj.lock())["MoveSpeedDecelerationamount"].number_value());
				if (m_attackAnimeCnt == 17)
				{
					if (!m_target.expired())
					{
						m_target.lock()->SetAttackHit(false);
						m_target.lock()->SetDefenseSuc(false);
					}
				}
			}
			else if (m_EnemyState & rlAttackTwo)
			{
				m_attackMoveSpd *= static_cast<float>((*m_wpJsonObj.lock())["MoveSpeedDecelerationamount"].number_value());
				if (m_attackAnimeCnt == 16)
				{
					if (!m_target.expired())
					{
						m_target.lock()->SetAttackHit(false);
						m_target.lock()->SetDefenseSuc(false);
					}
				}
			}
			else if (m_EnemyState & rlAttackThree)
			{
				m_attackMoveSpd *= static_cast<float>((*m_wpJsonObj.lock())["MoveSpeedDecelerationamount"].number_value());
				if (m_attackAnimeCnt == 23)
				{
					if (!m_target.expired())
					{
						m_target.lock()->SetAttackHit(false);
						m_target.lock()->SetDefenseSuc(false);
					}
				}
			}
			else if (m_EnemyState & rlAttackRush)
			{
				m_attackMoveSpd *= static_cast<float>((*m_wpJsonObj.lock())["RushAttackMoveSpeedDecelerationamount"].number_value());
				if (m_attackAnimeCnt == 21 ||
					m_attackAnimeCnt == 31 ||
					m_attackAnimeCnt == 49 ||
					m_attackAnimeCnt == 57 ||
					m_attackAnimeCnt == 74 ||
					m_attackAnimeCnt == 89 ||
					m_attackAnimeCnt == 107
					)
				{
					if (!m_target.expired())
					{
						m_target.lock()->SetAttackHit(false);
						m_target.lock()->SetDefenseSuc(false);
					}

					switch (m_attackAnimeCnt)
					{
					case 21:
					case 31:
						m_attackMoveSpd = 0.2f;
						break;
					case 49:
					case 57:
						m_attackMoveSpd = 0.25f;
						break;
					case 74:
					case 89:
						m_attackMoveSpd = 0.1f;
						break;
					case 107:
						m_attackMoveSpd = 0.05f;
						break;
					}
				}
			}
		}

		m_pos += m_attackMoveDir * m_attackMoveSpd;
	}
}

void Enemy::ScorpionAttackDecision()
{
	if (!(m_EnemyState & (rAttack | lAttack | mantis | rlAttack | rlAttackRush)))
	{
		if ((m_weaponType & eScorpion) && (m_weaponType & eLScorpion))
		{
			if (m_bMantisAttack && m_bMantisPossAng)
			{
				m_EnemyState |= mantis;
				m_EnemyState &= mantis;
				if (m_EnemyState & grassHopperDash)
				{
					m_EnemyState &= ~grassHopperDash;
				}

				m_attackMoveDir = Math::Vector3::TransformNormal(Math::Vector3(0, 0, 1), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
				m_attackMoveDir.y = 0;
				m_attackMoveDir.Normalize();
				m_attackMoveSpd = 0.1f;

				const std::shared_ptr<Scopion> scopion = std::dynamic_pointer_cast<Scopion>(m_weaponList[0]);
				scopion->SetMantis(Math::Matrix::CreateRotationY(m_mWorldRot.y), true);
				const std::shared_ptr<Scopion> scopion2 = std::dynamic_pointer_cast<Scopion>(m_weaponList[1]);
				scopion2->SetBMantis(true);
				m_bMove = true;
				m_animator->SetAnimation(m_model->GetAnimation("Mantis"), false);
				m_bMantisAttack = false;
			}
			else
			{
				m_bRushAttackPossible = false;
				m_EnemyState |= rlAttackOne;
				m_EnemyState &= ~rAttack;
				m_EnemyState &= ~lAttack;
				m_bAttackAnimeDelay = false;
				m_bAttackAnimeCnt = true;
				m_attackAnimeCnt = 0;
				m_attackAnimeDelayCnt = 0;
				m_bMove = true;

				m_attackMoveDir = Math::Vector3::TransformNormal(Math::Vector3(0, 0, 1), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
				m_attackMoveDir.y = 0;
				m_attackMoveDir.Normalize();
				m_attackMoveSpd = 0.8f;

				m_animator->SetAnimation(m_model->GetAnimation("RLAttackOne"), false);
			}
		}

		m_bMove = true;
		if (!m_target.expired())
		{
			m_target.lock()->SetAttackHit(false);
			m_target.lock()->SetDefenseSuc(false);
		}
	}
	else
	{
		if (m_bAttackAnimeDelay)
		{
			if (m_EnemyState & rAttackOne)
			{
				m_EnemyState |= rAttackTwo;
				m_EnemyState &= ~rAttackOne;
				m_bAttackAnimeDelay = false;
				m_bAttackAnimeCnt = true;
				m_animator->SetAnimation(m_model->GetAnimation("RAttack2"), false);
				m_bMove = true;
				m_attackMoveDir = Math::Vector3::TransformNormal(Math::Vector3(0, 0, 1), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
				m_attackMoveDir.y = 0;
				m_attackMoveDir.Normalize();
				m_attackMoveSpd = 0.8f;
			}
			else if (m_EnemyState & rAttackTwo)
			{
				m_EnemyState |= rAttackThree;
				m_EnemyState &= ~rAttackTwo;
				m_bAttackAnimeDelay = false;
				m_bAttackAnimeCnt = false;
				m_animator->SetAnimation(m_model->GetAnimation("RAttack3"), false);
				m_bMove = true;
				m_attackMoveDir = Math::Vector3::TransformNormal(Math::Vector3(0, 0, 1), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
				m_attackMoveDir.y = 0;
				m_attackMoveDir.Normalize();
				m_attackMoveSpd = 0.8f;
			}

			if (m_EnemyState & lAttackOne)
			{
				m_EnemyState |= lAttackTwo;
				m_EnemyState &= ~lAttackOne;
				m_bAttackAnimeDelay = false;
				m_bAttackAnimeCnt = true;
				m_animator->SetAnimation(m_model->GetAnimation("LAttack2"), false);
				m_bMove = true;
				m_attackMoveDir = Math::Vector3::TransformNormal(Math::Vector3(0, 0, 1), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
				m_attackMoveDir.y = 0;
				m_attackMoveDir.Normalize();
				m_attackMoveSpd = 0.8f;
			}
			else if (m_EnemyState & lAttackTwo)
			{
				m_EnemyState |= lAttackThree;
				m_EnemyState &= ~lAttackTwo;
				m_bAttackAnimeDelay = false;
				m_bAttackAnimeCnt = false;
				m_animator->SetAnimation(m_model->GetAnimation("LAttack3"), false);
				m_bMove = true;
				m_attackMoveDir = Math::Vector3::TransformNormal(Math::Vector3(0, 0, 1), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
				m_attackMoveDir.y = 0;
				m_attackMoveDir.Normalize();
				m_attackMoveSpd = 0.8f;
			}

			if (m_EnemyState & rlAttackOne)
			{
				m_EnemyState |= rlAttackTwo;
				m_EnemyState &= ~rlAttackOne;
				m_bAttackAnimeDelay = false;
				m_bAttackAnimeCnt = true;
				m_attackAnimeCnt = 0;
				m_attackAnimeDelayCnt = 0;

				m_attackMoveDir = Math::Vector3::TransformNormal(Math::Vector3(0, 0, 1), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
				m_attackMoveDir.y = 0;
				m_attackMoveDir.Normalize();
				m_attackMoveSpd = 0.8f;
				m_animator->SetAnimation(m_model->GetAnimation("RLAttackTwo"), false);
				m_bMove = true;
				if (!m_target.expired())
				{
					m_target.lock()->SetAttackHit(false);
					m_target.lock()->SetDefenseSuc(false);
				}
			}
			else if (m_EnemyState & rlAttackTwo)
			{
				m_EnemyState |= rlAttackThree;
				m_EnemyState &= ~rlAttackTwo;
				m_bAttackAnimeDelay = false;
				m_bAttackAnimeCnt = true;
				m_attackAnimeCnt = 0;
				m_attackAnimeDelayCnt = 0;

				m_attackMoveDir = Math::Vector3::TransformNormal(Math::Vector3(0, 0, 1), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
				m_attackMoveDir.y = 0;
				m_attackMoveDir.Normalize();
				m_attackMoveSpd = 0.8f;
				m_animator->SetAnimation(m_model->GetAnimation("RLAttackThree"), false);
				m_bMove = true;
				if (!m_target.expired())
				{
					m_target.lock()->SetAttackHit(false);
					m_target.lock()->SetDefenseSuc(false);
				}
			}
			else if (m_EnemyState & rlAttackThree && m_bRushAttackPossible)
			{
				m_EnemyState |= rlAttackRush;
				m_EnemyState &= rlAttackRush;
				m_bAttackAnimeDelay = false;
				m_bAttackAnimeCnt = true;
				m_attackAnimeCnt = 0;
				m_attackAnimeDelayCnt = 0;

				m_attackMoveDir = Math::Vector3::TransformNormal(Math::Vector3(0, 0, 1), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
				m_attackMoveDir.y = 0;
				m_attackMoveDir.Normalize();
				m_attackMoveSpd = 0.8f;
				m_animator->SetAnimation(m_model->GetAnimation("RLAttackRush"), false);
				m_bMove = true;
				if (!m_target.expired())
				{
					m_target.lock()->SetAttackHit(false);
					m_target.lock()->SetDefenseSuc(false);
				}
			}
			m_EnemyState &= ~lAttack;
		}
	}

	if (!(m_EnemyState & (rAttack | lAttack | mantis | rlAttack | rlAttackRush)))
	{
		if (m_weaponType & eScorpion)
		{
			m_EnemyState |= rAttackOne;
			m_EnemyState &= ~lAttack;
			m_bAttackAnimeDelay = false;
			m_bAttackAnimeCnt = true;
			m_attackAnimeCnt = 0;
			m_attackAnimeDelayCnt = 0;
			m_bMove = true;
			m_attackMoveDir = Math::Vector3::TransformNormal(Math::Vector3(0, 0, 1), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
			m_attackMoveDir.y = 0;
			m_attackMoveDir.Normalize();
			if (!m_target.expired())
			{
				m_target.lock()->SetAttackHit(false);
				m_target.lock()->SetDefenseSuc(false);
			}
			
			m_attackMoveSpd = 0.8f;

			if (m_EnemyState & grassHopperDash && m_bBoss)
			{
				m_animator->SetAnimation(m_model->GetAnimation("GrassDashRAttack"), false);
			}
			else
			{
				m_animator->SetAnimation(m_model->GetAnimation("RAttack1"), false);
			}
		}
		else if (m_weaponType & eLScorpion)
		{
			m_EnemyState |= lAttackOne;
			m_EnemyState &= ~rAttack;
			m_bAttackAnimeDelay = false;
			m_bAttackAnimeCnt = true;
			m_attackAnimeCnt = 0;
			m_attackAnimeDelayCnt = 0;
			m_bMove = true;
			m_attackMoveDir = Math::Vector3::TransformNormal(Math::Vector3(0, 0, 1), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
			m_attackMoveDir.y = 0;
			m_attackMoveDir.Normalize();

			if (!m_target.expired())
			{
				m_target.lock()->SetAttackHit(false);
				m_target.lock()->SetDefenseSuc(false);
			}
			
			m_attackMoveSpd = 0.8f;
			if (m_EnemyState & grassHopperDash && m_bBoss)
			{
				m_animator->SetAnimation(m_model->GetAnimation("GrassDashLAttack"), false);
			}
			else
			{
				m_animator->SetAnimation(m_model->GetAnimation("LAttack1"), false);
			}
		}
	}
}

