#include "Enemy.h"
#include "../../../Camera/GameCamera/GameCamera.h"
#include "../../../Scene/SceneManager.h"
#include "../../Weapon/Hopper/Hopper.h"
#include "../../Weapon/Scopion/Scopion.h"
#include "../Player/Player.h"

void Enemy::Init()
{
	// 座標行列
	Math::Matrix transMat;
	transMat = Math::Matrix::CreateTranslation(0, 0, INITIALPOSZ);
	m_pos = transMat.Translation();

	// 拡縮行列
	Math::Matrix scaleMat;
	scaleMat = Math::Matrix::CreateScale(Math::Vector3::One);

	// 回転行列
	Math::Matrix rotMat;
	rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(INITIALROTATIONY));
	m_mWorldRot.y = 180;

	// 行列合成
	m_mWorld = rotMat * scaleMat * transMat;

	m_model = std::make_shared<KdModelWork>();
	m_model->SetModelData
	(KdAssets::Instance().m_modeldatas.GetData
	("Asset/Models/Player/Player.gltf"));
	/// 当たり判定初期化
	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape
	("EnemyModel", m_model, KdCollider::TypeBump | KdCollider::TypeDamage);

	m_enduranceBarPoly.SetMaterial("Asset/Textures/Ui/Game/enduranceBar.png");
	m_endurancePoly.SetMaterial("Asset/Textures/Ui/Game/endurance.png");

	m_pDebugWire = std::make_unique<KdDebugWireFrame>();

	m_EnemyState = eIdle;

	m_weaponType |= eScorpion;
	m_weaponType |= eLScorpion;
	m_wantToMoveState = none;
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
	m_animator->SetAnimation(m_model->GetAnimation("Idle"));

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

	//m_thinkActionDelayTimeVal = 5;
	m_thinkActionDelayTimeVal = 1;
	//m_actionDelayTimeVal = 10;

	m_thinkActionDelayTime = m_thinkActionDelayTimeVal;
	//m_actionDelayTime = m_actionDelayTimeVal;
	m_grassSuccessionDelayCnt = 0;

	m_bMantisAttack = false;
	m_bEnemyDeath = false;

	m_bMantisPossAng = false;
	m_bEnemyLose = false;
	m_hasDeTime = 0;
	m_overStageTime = 0;

	m_torion = 300.0f;
	m_endurance = 400.0f;
	m_enemyType = Enemy::EnemyType::allRounder;

	m_bEnemyBetweenPlayer = false;

	m_enemyNumber = 0;
	m_coarseFishEnemyAttackDelayCnt = 0;
}

void Enemy::Update()
{
	float lowestYPos;
	if (SceneManager::Instance().GetSceneType() == SceneManager::SceneType::tutorial)
	{
		lowestYPos = -10.0f;

		if (m_bFirstUpdate)
		{
			m_weaponList.clear();
		}
	}
	else
	{
		lowestYPos = -1.0f;
		if (m_bEnemyLose)
		{
			return;
		}

		// debugキー
		if (GetAsyncKeyState('P') & 0x8000)
		{
			m_pos = Math::Vector3::Zero;
		}

		// debugキー
		if (GetAsyncKeyState('K') & 0x8000)
		{
			m_torion = 0;
			//m_endurance -= 5;
		}

		m_torion -= m_graduallyTorionDecVal;
		if (m_torion <= 0)
		{
			m_torion = 0;
			if (!m_bEnemyDeath)
			{
				m_bEnemyDeath = true;
				m_bMove = true;
				m_EnemyState = eIdle;
				m_wantToMoveState = none;
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
				m_EnemyState = eIdle;
				m_wantToMoveState = none;
				m_animator->SetAnimation(m_model->GetAnimation("Death"), false);
			}
		}

		if (m_bBoss)
		{
			BossUpdate();
		}
		else
		{
			CoarseFishEnemyUpdate();
		}

		for (auto& WeaList : m_weaponList)
		{
			WeaList->Update();
		}
	}

	if (m_bFirstUpdate)
	{
		m_bFirstUpdate = false;
		return;
	}


	if (m_pos.x >= 62.5 ||
		m_pos.x <= -62.5 ||
		m_pos.z >= 62.5 ||
		m_pos.z <= -62.5 ||
		m_pos.y <= lowestYPos ||
		m_pos.y >= 125)
	{
		m_overStageTime++;
		if (m_overStageTime >= 90)
		{
			m_pos = { 0,0,0 };
			m_overStageTime = 0;
		}
	}
	else
	{
		if (m_overStageTime >= 90)
		{
			m_pos = { 0,0,0 };
			m_overStageTime = 0;
		}

		KdCollider::SphereInfo sphereInfo;
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

	
	if (m_EnemyState & eHit)
	{
		m_pos.y -= m_gravity;

		if (m_EnemyState & eCutRaiseHit)
		{
			m_gravity += 0.0075f;
		}
		else
		{
			m_gravity += 0.01f;
		}

		if (m_hitStopCnt > 0)
		{
			--m_hitStopCnt;
			if (m_EnemyState & eBlowingAwayHit)
			{
				if (m_hitStopCnt < 10)
				{
					m_hitMoveSpd = 0;
				}

				m_hitMoveSpd *= 0.95f;
			}
			else
			{
				m_hitMoveSpd *= 0.95f;
			}

			m_pos += m_knockBackVec * m_hitMoveSpd;
		}

		if (m_hitStopCnt == 0)
		{
			if (m_EnemyState & (eNomalHit | eCutRaiseHit))
			{
				if (!(m_EnemyState & eIdle))
				{
					m_animator->SetAnimation(m_model->GetAnimation("IdleA"), false);
				}
				m_EnemyState = eIdle;
				m_wantToMoveState = none;
				if (SceneManager::Instance().GetSceneType() != SceneManager::SceneType::tutorial)
				{
					Brain();
				}
			}
			else if (m_EnemyState & eBlowingAwayHit)
			{
				if (!(m_EnemyState & eIdle))
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
				m_EnemyState = eBlowingAwayRise;
			}
			else if (m_EnemyState & eIaiKiriHit)
			{
				if (!(m_EnemyState & eIdle))
				{
					m_animator->SetAnimation(m_model->GetAnimation("IaiKiriRise"), false);
				}
				m_EnemyState = eIaiKiriRise;
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
			if (!(m_EnemyState & eIdle))
			{
				m_animator->SetAnimation(m_model->GetAnimation("IdleA"), false);
			}
			m_EnemyState = eIdle;
			m_thinkActionDelayTime = m_thinkActionDelayTimeVal;
			m_wantToMoveState = none;
		}
	}

	if (!(m_EnemyState & (eGrassHopperDash | eGrassHopperDashUp | eHit)))
	{
		m_pos.y -= m_gravity;
		m_gravity += 0.01f;
	}

	//if (m_pos.y <= -1.05f)//今だけ 地面に着いたらカメラのXAngが180以下かつ0以上補正する
	// ========================================
	// 当たり判定
	// ========================================

	// レイ判定用に変数を作成
	KdCollider::RayInfo rayInfo;
	//rayInfo.m_pos = m_pos;
	rayInfo.m_pos = m_pos;
	rayInfo.m_pos.y += 0.7f;
	if (!(m_EnemyState & (eGrassHopperDash | eGrassHopperDashUp)))
	{
		rayInfo.m_dir = { 0,-1,0 };
		/*rayInfo.m_pos.y += 0.1f;*/
		static float enableStepHight = 0.2f;
		rayInfo.m_pos.y += enableStepHight;
		rayInfo.m_range = m_gravity + enableStepHight;
	}
	else
	{
		//rayInfo.m_pos += Math::Vector3(0, 0.5f, 0);
		rayInfo.m_dir = m_grassHopperDashDir;
		//rayInfo.m_range = 1.25f;
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
		if (!(m_EnemyState & (eGrassHopperDash | eGrassHopperDashUp)))
		{
			//m_pos = groundPos;
			m_pos = groundPos + Math::Vector3(0, -0.7f, 0);
			m_gravity = 0;

			if (m_EnemyState & (eFall | eJump) && !m_bEnemyDeath)
			{
				m_bMove = false;
				if (!(m_EnemyState & eIdle))
				{
					m_animator->SetAnimation(m_model->GetAnimation("IdleA"), false);
				}
				m_EnemyState = eIdle;
				m_wantToMoveState = none;
			}
		}
		else
		{
			m_gravity = 0;
			m_EnemyState = eFall;
			m_rGrassHopperTime = 0;
			m_lGrassHopperTime = 0;
			m_bMove = false;
			m_grassHopperDashDir = {};
			m_dashSpd = 0.0f;
		}
	}
	else
	{
		/*	if (m_EnemyState & (eGrassHopperDash | eGrassHopperDashUp))
			{
				--m_enemyAirborneTimetoBeCnt;
				if (m_enemyAirborneTimetoBeCnt <= 0)
				{
					m_enemyAirborneTimetoBeCnt = 0;
					m_EnemyState = eFall;
					m_wantToMoveState = none;
				}
			}*/
	}

	KdCollider::SphereInfo sphereInfo;
	// 球の中心位置を設定
	sphereInfo.m_sphere.Center = m_pos + Math::Vector3(0, 1.5f, 0);
	if (!(m_EnemyState & (eGrassHopperDash | eGrassHopperDashUp | eStep)))
	{
		sphereInfo.m_sphere.Radius = 0.3f;
	}
	else
	{
		sphereInfo.m_sphere.Radius = 1.2f;
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

	if (m_target.lock()->GetPlayerState() & (Player::PlayerState::defense))
	{
		if (!(m_EnemyState & (eGrassHopperDash | eGrassHopperDashUp | eStep)) || !(m_wantToMoveState & (escape | dashAttack | grassDash | avoidance)))
		{
			sphereInfo.m_sphere.Radius = 0.3f;
		}
		else
		{
			sphereInfo.m_sphere.Radius = 0.6f;
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


	sphereInfo.m_sphere.Radius = 0.3f;


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
			hitDir.y = 0.0f;
			hitDir.Normalize();
			// 球とモデルが当たっている
			m_pos += (hitDir * maxOverLap);
		}
	}

	if (m_EnemyState & (eRlAttack | eRlAttackRush | EnemyState::eHit))
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
		rayInfo.m_pos = m_pos + Math::Vector3(0, 1.2f, 0);
		rayInfo.m_dir = (m_target.lock()->GetPos() + Math::Vector3(0, 1.2f, 0)) - (m_pos + Math::Vector3(0, 1.2f, 0));
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

			if (m_EnemyState & (eGrassHopperDashL | eGrassHopperDashR))
			{
				m_wantToMoveState = Enemy::WantToMoveState::dashAttack;
				m_dashSpd = 0.0f;
				m_grassHopperDashDir = {};
				m_gravity = 0;
				m_rGrassHopperPauCnt = 0;
				m_lGrassHopperPauCnt = 0;
				GrassMoveVecDecision();
			}
		}

	}

	if ((m_EnemyState & eRlAttackRush) && m_attackAnimeCnt >= 107)
	{
		EnemyKickHitAttackChaeck();
	}

#ifdef _DEBUG
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
#endif

	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_pos);
	Math::Matrix RotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y));
	m_mWorld = RotMat * transMat;
}

void Enemy::BossUpdate()
{
	if (m_bFirstUpdate)
	{
		for (auto& WeaList : m_weaponList)
		{
			WeaList->SetOwner(shared_from_this());
		}

		if (SceneManager::Instance().GetSceneType() == SceneManager::SceneType::challenge)
		{
			m_torion = 100.0f;
			m_endurance = 150.0f;

			std::random_device rnd;
			std::mt19937 mt(rnd());
			std::uniform_int_distribution<int> intRand(0, 999);
			int randNum[4] = {};
			int rand = intRand(mt);
			randNum[0] = 150;
			randNum[1] = 300;
			randNum[2] = 300;
			randNum[3] = 250;

			/*randNum[0] = 0;
			randNum[1] = 0;
			randNum[2] = 0;
			randNum[3] = 2500;*/
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
			m_torion = 300.0f;
			m_endurance = 400.0f;
			m_enemyType = Enemy::EnemyType::allRounder;
		}
		return;
	}

	if (!(m_EnemyState & (eHit | eRise)))
	{
		if (SceneManager::Instance().GetSceneType() != SceneManager::SceneType::tutorial)
		{
			std::shared_ptr<Player> spTarget = m_target.lock();
			if (spTarget)
			{
				Math::Vector3 vTarget = spTarget->GetPos() - m_pos;
				UpdateRotate(vTarget);
			}


			if (!(m_EnemyState & (eFall | eJump)) && !m_bEnemyDeath)
			{
				m_bMove = false;
			}

			if (m_wantToMoveState & none && !m_bEnemyDeath)
			{
				Brain();
			}
			else if (!m_bEnemyDeath)
			{
				Math::Vector3 src = spTarget->GetPos() - m_pos;
				if (src.Length() >= 20.0f && !(m_EnemyState & (eGrassHopperDash & ~eGrassHopperDashB) | eGrassHopperDashUp))
				{
					Brain();
				}
			}

			switch (m_rightWeaponNumber) // 後に番号を自由に選べるようになるかも
			{
			case 1:
				m_weaponType |= eScorpion;
				m_weaponType &= ~eGrassHopper;
				break;
			case 2:
				m_weaponType |= eGrassHopper;
				m_weaponType &= ~eScorpion;
				break;
			}

			switch (m_leftWeaponNumber) // 後に番号を自由に選べるようになるかも
			{
			case 1:
				m_weaponType |= eLScorpion;
				m_weaponType &= ~eLGrassHopper;
				break;
			case 2:
				m_weaponType |= eLGrassHopper;
				m_weaponType &= ~eLScorpion;
				break;
			}

			Math::Vector3 src;
			if (!(m_wantToMoveState & WantToMoveState::none))
			{
				if (!(m_EnemyState & eHasDefense))
				{
					switch (m_wantToMoveState)
					{
					case WantToMoveState::attack:
						ScorpionAttackDecision();
						break;
					case WantToMoveState::escape:
						GrassMoveVecDecision();
						break;
					case WantToMoveState::defense:
						if (m_EnemyState & eDefense)
						{
							ScorpionDefenseMove();
						}
						else
						{
							ScorpionDefenseDecision();
						}

						break;
					case WantToMoveState::dashAttack:
						src = spTarget->GetPos() - m_pos;
						if (m_dashSpd == 1.2f)
						{
							if (src.Length() <= 14.5f)
							{
								if (m_weaponType & eGrassHopper)
								{
									if ((m_EnemyState & eGrassHopperDashF) && (m_rGrassHopperTime <= 80))
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
									if ((m_EnemyState & eGrassHopperDashF) && (m_lGrassHopperTime <= 80))
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
						else if (m_dashSpd == 0.8f || m_dashSpd == 0.5f)
						{
							if (src.Length() <= 8.0f)
							{
								if (m_weaponType & eGrassHopper)
								{
									if ((m_EnemyState & eGrassHopperDashF) && (m_rGrassHopperTime <= 80))
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
									if ((m_EnemyState & eGrassHopperDashF) && (m_lGrassHopperTime <= 80))
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
							if (src.Length() <= 4.0f)
							{
								if (m_weaponType & eGrassHopper)
								{
									if ((m_EnemyState & eGrassHopperDashF) && (m_rGrassHopperTime <= 80))
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
									if ((m_EnemyState & eGrassHopperDashF) && (m_lGrassHopperTime <= 80))
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
							if (src.Length() <= 2.0f)
							{
								if (m_weaponType & eGrassHopper)
								{
									if ((m_EnemyState & eGrassHopperDashF) && (m_rGrassHopperTime <= 80))
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
									if ((m_EnemyState & eGrassHopperDashF) && (m_lGrassHopperTime <= 80))
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
						break;
					case WantToMoveState::run:
						NormalMoveVecDecision();
						break;
					case WantToMoveState::step:
						if (!(m_EnemyState & eStep))
						{
							StepVecDecision();
						}
						break;
					case WantToMoveState::grassDash:
						GrassMoveVecDecision();
						break;
					case WantToMoveState::avoidance:
						GrassMoveVecDecision();
						break;
					}
				}

				if (!(m_EnemyState & (eGrassHopperDash | eGrassHopperDashUp | eStep)))
				{
					if (m_EnemyState & (eLAttack | eRAttack | eRlAttack | eRlAttackRush | eMantis))
					{
						ScorpionAttackMove();
					}
					else if (m_EnemyState & eRun | m_EnemyState & eJump)
					{
						NormalMove();
					}
					else if (m_EnemyState & eHasDefense)
					{
						HasDefenseMove();
					}
				}
				else if (m_EnemyState & (eGrassHopperDash | eGrassHopperDashUp))
				{
					GrassMove();
				}
				else if (m_EnemyState & eStep)
				{
					StepMove();
				}
			}

			if (!m_bMove)
			{
				if (!(m_EnemyState & (eFall | eJump)))
				{
					m_EnemyState = eIdle;
					m_animator->SetAnimation(m_model->GetAnimation("IdleA"));
				}
			}
		}
	}
}

void Enemy::CoarseFishEnemyUpdate()
{
	if (m_bFirstUpdate)
	{
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

		if (SceneManager::Instance().GetSceneType() == SceneManager::SceneType::challenge)
		{
			m_torion = 100.0f;
			m_endurance = 150.0f;

			std::random_device rnd;
			std::mt19937 mt(rnd());
			std::uniform_int_distribution<int> intRand(0, 999);
			int randNum[4] = {};
			int rand = intRand(mt);
			randNum[0] = 150;
			randNum[1] = 300;
			randNum[2] = 300;
			randNum[3] = 250;

			/*randNum[0] = 0;
			randNum[1] = 0;
			randNum[2] = 0;
			randNum[3] = 2500;*/
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
			m_torion = 50.0f;
			m_endurance = 100.0f;
			m_enemyType = Enemy::EnemyType::coarseFishEnemy;
		}
		return;
	}

	if (m_coarseFishEnemyAttackDelayCnt > 0)
	{
		--m_coarseFishEnemyAttackDelayCnt;
	}

	if (!(m_EnemyState & (eHit | eRise)))
	{
		if (SceneManager::Instance().GetSceneType() != SceneManager::SceneType::tutorial)
		{
			std::shared_ptr<Player> spTarget = m_target.lock();
			if (spTarget)
			{
				Math::Vector3 vTarget = spTarget->GetPos() - m_pos;
				UpdateRotate(vTarget);
			}


			if (!(m_EnemyState & (eFall | eJump)) && !m_bEnemyDeath)
			{
				m_bMove = false;
			}

			if (m_wantToMoveState & none && !m_bEnemyDeath)
			{
				Brain();
			}
			else if (!m_bEnemyDeath)
			{
				Math::Vector3 src = spTarget->GetPos() - m_pos;
				if (src.Length() >= 20.0f && !(m_EnemyState & (eGrassHopperDash & ~eGrassHopperDashB) | eGrassHopperDashUp))
				{
					Brain();
				}
			}

			Math::Vector3 src;
			if (!(m_wantToMoveState & WantToMoveState::none))
			{
				if (!(m_EnemyState & eHasDefense))
				{
					switch (m_wantToMoveState)
					{
					case WantToMoveState::attack:
						ScorpionAttackDecision();
						break;
					case WantToMoveState::run:
						NormalMoveVecDecision();
						break;
					}
				}

				if (m_EnemyState & (eLAttack | eRAttack | eRlAttack | eRlAttackRush | eMantis))
				{
					ScorpionAttackMove();
				}
				else if (m_EnemyState & eRun | m_EnemyState & eJump)
				{
					NormalMove();
				}
			}

			if (!m_bMove)
			{
				if (!(m_EnemyState & (eFall | eJump)))
				{
					m_EnemyState = eIdle;
					m_animator->SetAnimation(m_model->GetAnimation("IdleA"));
				}
			}
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
			if (!(m_EnemyState & (eHit | eHasDefense)))
			{
				if (!(m_EnemyState & eFall))
				{
					m_animator->SetAnimation(m_model->GetAnimation("FallA"), false);
				}
				m_EnemyState = eFall;
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

		if (!(m_EnemyState & (eLAttack | eRAttack | eRlAttack | eRlAttackRush)))
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
		else if (m_EnemyState & (eLAttack | eRAttack) && !m_bAttackAnimeDelay)
		{
			if (m_bAttackAnimeCnt)
			{
				m_attackAnimeCnt++;
				if (m_EnemyState & (eLAttackOne | eLAttackTwo | eRAttackOne | eRAttackTwo))
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
				if (m_EnemyState & (eLAttackThree | eRAttackThree))
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
		else if (m_EnemyState & (eRlAttack | eRlAttackRush))
		{
			if (m_bAttackAnimeCnt)
			{
				m_attackAnimeCnt++;
				if (m_EnemyState & eRlAttackOne)
				{
					if (m_attackAnimeCnt == 13 || m_attackAnimeCnt == 17)
					{
						KdAudioManager::Instance().Play("Asset/Audio/SE/Swishes - SWSH 40, Swish, Combat, Weapon, Light.wav");
					}
				}
				else if (m_EnemyState & eRlAttackTwo)
				{
					if (m_attackAnimeCnt == 13 || m_attackAnimeCnt == 16)
					{
						KdAudioManager::Instance().Play("Asset/Audio/SE/Swishes - SWSH 40, Swish, Combat, Weapon, Light.wav");
					}
				}
				else if (m_EnemyState & eRlAttackThree)
				{
					if (m_attackAnimeCnt == 15 || m_attackAnimeCnt == 23)
					{
						KdAudioManager::Instance().Play("Asset/Audio/SE/Swishes - SWSH 40, Swish, Combat, Weapon, Light.wav");
					}
				}
				else if (m_EnemyState & eRlAttackRush)
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


				if (m_EnemyState & eRlAttackOne)
				{
					if (m_attackAnimeCnt == 20)
					{
						m_bAttackAnimeDelay = true;
						m_bAttackAnimeCnt = false;
						m_attackAnimeCnt = 0;
						m_attackAnimeDelayCnt = 10;
					}
				}
				else if (m_EnemyState & eRlAttackTwo)
				{
					if (m_attackAnimeCnt == 20)
					{
						m_bAttackAnimeDelay = true;
						m_bAttackAnimeCnt = false;
						m_attackAnimeCnt = 0;
						m_attackAnimeDelayCnt = 10;
					}
				}
				else if (m_EnemyState & eRlAttackThree)
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
				else if (SceneManager::Instance().GetSceneType() == SceneManager::SceneType::battle)
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
					Math::Matrix efcMat = Math::Matrix::CreateScale(0.5f) * Math::Matrix::CreateTranslation({ m_pos.x,m_pos.y + 0.3f,m_pos.z });
					KdEffekseerManager::GetInstance().SetWorldMatrix("BailOutEnemy.efk", efcMat);
				}
				//m_isExpired = true;
			}
		}
	}
}

void Enemy::OnHit(Math::Vector3 a_KnocBackvec)
{
	m_EnemyState = eNomalHit;
	if (m_target.lock()->GetPlayerState() & Player::PlayerState::rlAttackThree)
	{
		m_hitStopCnt = 60;
	}
	else
	{
		m_hitStopCnt = 40;
	}

	m_hitMoveSpd = 0.05f;
	m_hitColorChangeTimeCnt = 15;
	m_knockBackVec = a_KnocBackvec;
	m_endurance -= 15.0f;
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
	m_EnemyState = eBlowingAwayHit;
	m_hitStopCnt = 40;
	m_hitColorChangeTimeCnt = 15;
	if (m_target.lock()->GetPlayerState() & Player::PlayerState::rlAttackRush && m_target.lock()->GetAnimationCnt() >= 107)
	{
		m_hitMoveSpd = 0.95f;
		if (SceneManager::Instance().GetUpdateStopCnt() == 0)
		{
			SceneManager::Instance().SetUpdateStopCnt(25); // これでアップデートを一時止める
		}
		//SceneManager::Instance().SetUpdateStopCnt(10);   // 画面を揺らす時のHtiストップフレーム数
		//SceneManager::Instance().SetScreenVibFrames(15); // 画面を揺らすフレーム数
	}
	else if (m_target.lock()->GetPlayerState() & (Player::PlayerState::rAttackOne | Player::PlayerState::lAttackOne) && m_target.lock()->GetPlayerState() & (Player::PlayerState::grassHopperDashF))
	{
		m_hitMoveSpd = 0.35f;
		if (SceneManager::Instance().GetUpdateStopCnt() == 0)
		{
			SceneManager::Instance().SetUpdateStopCnt(15); // これでアップデートを一時止める
		}
	}
	else if (m_target.lock()->GetPlayerState() & (Player::PlayerState::rAttackThree | Player::PlayerState::lAttackThree))
	{
		m_hitMoveSpd = 0.3f;
		if (SceneManager::Instance().GetUpdateStopCnt() == 0)
		{
			SceneManager::Instance().SetUpdateStopCnt(8); // これでアップデートを一時止める
		}
	}

	m_invincibilityTimeCnt = 100;
	m_knockBackVec = a_KnocBackvec;
	m_endurance -= 30.0f;
	m_attackHit = true;

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

	if (m_graduallyTorionDecVal == 0)
	{
		m_graduallyTorionDecVal = 0.01f;
	}
	else
	{
		m_graduallyTorionDecVal *= 1.45f;
	}
}

void Enemy::IaiKiriAttackOnHit(Math::Vector3 a_KnocBackvec)
{
	m_EnemyState = eIaiKiriHit;
	m_hitStopCnt = 40;
	m_hitColorChangeTimeCnt = 15;
	m_hitMoveSpd = 0.0f;
	m_invincibilityTimeCnt = 100;
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
	m_EnemyState = eCutRaiseHit;
	m_hitStopCnt = 60;
	m_hitColorChangeTimeCnt = 15;
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

	m_endurance -= 15.0f;
	m_knockBackVec = a_KnocBackvec;
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

void Enemy::HasDefense()
{
	if (m_EnemyState & (eRAttack | eRlAttack))
	{
		m_animator->SetAnimation(m_model->GetAnimation("RHasDefense"), false);
	}
	else if (m_EnemyState & eLAttack)
	{
		m_animator->SetAnimation(m_model->GetAnimation("LHasDefense"), false);
	}

	m_EnemyState |= eHasDefense;
	m_EnemyState &= eHasDefense;
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

	if (m_invincibilityTimeCnt <= 90 && m_invincibilityTimeCnt > 80 ||
		m_invincibilityTimeCnt <= 70 && m_invincibilityTimeCnt > 60 ||
		m_invincibilityTimeCnt <= 50 && m_invincibilityTimeCnt > 40 ||
		m_invincibilityTimeCnt <= 30 && m_invincibilityTimeCnt > 20 ||
		m_invincibilityTimeCnt <= 15 && m_invincibilityTimeCnt > 10 ||
		m_invincibilityTimeCnt <= 5 && m_invincibilityTimeCnt > 3 ||
		m_invincibilityTimeCnt == 1
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

	float betweenAng = targetAng - nowAng;

	if (betweenAng > 180)
	{
		betweenAng -= 360;
	}

	if (betweenAng < -180)
	{
		betweenAng += 360;
	}

	float rotateAng = std::clamp(betweenAng, -8.0f, 8.0f);

	if (rotateAng > 1.0f || rotateAng < -1.0f)
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

	if (rotateAng <= 2.0f && rotateAng >= -2.0f)
	{
		m_bMantisPossAng = true;
	}
	else
	{
		m_bMantisPossAng = false;
	}

	if (m_delayTurnAroundTime == 0)
	{
		m_mWorldRot.y += rotateAng;
	}
}

void Enemy::GrassMoveVecDecision()
{
	const KdModelWork::Node* node = nullptr;
	Math::Matrix mat = Math::Matrix::Identity;

	if (m_rGrassHopperPauCnt == 0)
	{
		if (m_weaponType & eGrassHopper)
		{
			std::random_device rnd;
			std::mt19937 mt(rnd());
			std::uniform_int_distribution<int> intRand(0, 999);
			int randNum[5] = {};

			int rand = intRand(mt);
			Math::Vector3 src;
			float y = m_target.lock()->GetPos().y - m_pos.y;

			switch (m_wantToMoveState)
			{
			case WantToMoveState::escape:
				if (m_grassSuccessionDelayCnt != 0)return;
				randNum[0] = 0;
				randNum[1] = 550;
				randNum[2] = 150;
				randNum[3] = 150;
				randNum[4] = 150;
				break;
			case WantToMoveState::dashAttack:
				src = m_target.lock()->GetPos() - m_pos;
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
			case WantToMoveState::grassDash:
				if (m_grassSuccessionDelayCnt != 0)return;
				src = m_target.lock()->GetPos() - m_pos;
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
			case WantToMoveState::avoidance:
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
						m_bMove = true;
						m_EnemyState &= ~eGrassHopperDash;
						m_EnemyState |= eGrassHopperDashF;
						m_EnemyState &= ~eGrassHopperDashUp;
						m_lGrassHopperPauCnt = 120;
						m_lGrassHopperTime = 90;
						m_rGrassHopperTime = 0;
						m_gravity = 0;

						m_weaponList[2]->GrassHopper({ m_pos.x + 0.65f * m_mWorldRot.x,m_pos.y + 0.9f,m_pos.z + 0.65f * m_mWorldRot.z }, m_mWorldRot.y);

						KdEffekseerManager::GetInstance().
							Play(fileName, { m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z });
						KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect(fileName); // これでループしない
						//KdEffekseerManager::GetInstance().SetRotation("Hit3.efk", m_mWorld.Backward(), DirectX::XMConvertToRadians(0));
						efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)) * Math::Matrix::CreateTranslation({ m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z });
						KdEffekseerManager::GetInstance().SetWorldMatrix(fileName, efcMat);
						break;
					case 1:
						m_grassHopperDashDir = Math::Vector3::TransformNormal(Math::Vector3(0, 0, -1), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
						m_bMove = true;
						m_EnemyState &= ~eGrassHopperDash;
						m_EnemyState |= eGrassHopperDashB;
						m_EnemyState &= ~eGrassHopperDashUp;
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
						//KdEffekseerManager::GetInstance().SetRotation("Hit3.efk", m_mWorld.Backward(), DirectX::XMConvertToRadians(0));
						efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y + 180.0f)) * Math::Matrix::CreateTranslation({ m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z });
						KdEffekseerManager::GetInstance().SetWorldMatrix(fileName, efcMat);
						break;
					case 2:
						m_grassHopperDashDir = Math::Vector3::TransformNormal(Math::Vector3(1, 0, 0), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
						m_bMove = true;
						m_EnemyState &= ~eGrassHopperDash;
						m_EnemyState |= eGrassHopperDashR;
						m_EnemyState &= ~eGrassHopperDashUp;
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
						//KdEffekseerManager::GetInstance().SetRotation("Hit3.efk", m_mWorld.Backward(), DirectX::XMConvertToRadians(0));
						efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y + 90.0f)) * Math::Matrix::CreateTranslation({ m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z });
						KdEffekseerManager::GetInstance().SetWorldMatrix(fileName, efcMat);
						break;
					case 3:
						m_grassHopperDashDir = Math::Vector3::TransformNormal(Math::Vector3(-1, 0, 0), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
						m_bMove = true;
						m_EnemyState &= ~eGrassHopperDash;
						m_EnemyState |= eGrassHopperDashL;
						m_EnemyState &= ~eGrassHopperDashUp;
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
						//KdEffekseerManager::GetInstance().SetRotation("Hit3.efk", m_mWorld.Backward(), DirectX::XMConvertToRadians(0));
						efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y + 270.0f)) * Math::Matrix::CreateTranslation({ m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z });
						KdEffekseerManager::GetInstance().SetWorldMatrix(fileName, efcMat);
						break;
					case 4:
						m_grassHopperDashDir = { 0, 1, 0 };
						m_grassHopperDashDir.Normalize();
						m_bMove = true;
						m_EnemyState &= ~eGrassHopperDash;
						m_EnemyState |= eGrassHopperDashUp;
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
						//KdEffekseerManager::GetInstance().SetRotation("Hit3.efk", m_mWorld.Backward(), DirectX::XMConvertToRadians(0));
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
			Math::Vector3 src;
			float y = m_target.lock()->GetPos().y - m_pos.y;
			switch (m_wantToMoveState)
			{
			case WantToMoveState::escape:
				if (m_grassSuccessionDelayCnt != 0)return;
				randNum[0] = 0;
				randNum[1] = 550;
				randNum[2] = 150;
				randNum[3] = 150;
				randNum[4] = 150;
				break;
			case WantToMoveState::dashAttack:
				src = m_target.lock()->GetPos() - m_pos;

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
			case WantToMoveState::grassDash:
				if (m_grassSuccessionDelayCnt != 0)return;
				src = m_target.lock()->GetPos() - m_pos;
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
			case WantToMoveState::avoidance:
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
						m_bMove = true;
						m_EnemyState &= ~eGrassHopperDash;
						m_EnemyState |= eGrassHopperDashF;
						m_EnemyState &= ~eGrassHopperDashUp;
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
						//KdEffekseerManager::GetInstance().SetRotation("Hit3.efk", m_mWorld.Backward(), DirectX::XMConvertToRadians(0));
						efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)) * Math::Matrix::CreateTranslation({ m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z });
						KdEffekseerManager::GetInstance().SetWorldMatrix(fileName, efcMat);
						break;
					case 1:
						m_grassHopperDashDir = Math::Vector3::TransformNormal(Math::Vector3(0, 0, -1), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
						m_bMove = true;
						m_EnemyState &= ~eGrassHopperDash;
						m_EnemyState |= eGrassHopperDashB;
						m_EnemyState &= ~eGrassHopperDashUp;
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
						//KdEffekseerManager::GetInstance().SetRotation("Hit3.efk", m_mWorld.Backward(), DirectX::XMConvertToRadians(0));
						efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y + 180.0f)) * Math::Matrix::CreateTranslation({ m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z });
						KdEffekseerManager::GetInstance().SetWorldMatrix(fileName, efcMat);
						break;
					case 2:
						m_grassHopperDashDir = Math::Vector3::TransformNormal(Math::Vector3(1, 0, 0), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
						m_bMove = true;
						m_EnemyState &= ~eGrassHopperDash;
						m_EnemyState |= eGrassHopperDashR;
						m_EnemyState &= ~eGrassHopperDashUp;
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
						//KdEffekseerManager::GetInstance().SetRotation("Hit3.efk", m_mWorld.Backward(), DirectX::XMConvertToRadians(0));
						efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y + 270.0f)) * Math::Matrix::CreateTranslation({ m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z });
						KdEffekseerManager::GetInstance().SetWorldMatrix(fileName, efcMat);
						break;
					case 3:
						m_grassHopperDashDir = Math::Vector3::TransformNormal(Math::Vector3(-1, 0, 0), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
						m_bMove = true;
						m_EnemyState &= ~eGrassHopperDash;
						m_EnemyState |= eGrassHopperDashL;
						m_EnemyState &= ~eGrassHopperDashUp;
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
						//KdEffekseerManager::GetInstance().SetRotation("Hit3.efk", m_mWorld.Backward(), DirectX::XMConvertToRadians(0));
						efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y + 90.0f)) * Math::Matrix::CreateTranslation({ m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z });
						KdEffekseerManager::GetInstance().SetWorldMatrix(fileName, efcMat);
						break;
					case 4:
						m_grassHopperDashDir = { 0, 1, 0 };
						m_grassHopperDashDir.Normalize();
						m_bMove = true;
						m_EnemyState &= ~eGrassHopperDash;
						m_EnemyState |= eGrassHopperDashUp;
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
						//KdEffekseerManager::GetInstance().SetRotation("Hit3.efk", m_mWorld.Backward(), DirectX::XMConvertToRadians(0));
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
	if (!(m_EnemyState & (eRAttack | eLAttack | eDefense | eMantis | eRlAttack | eRlAttackRush)))
	{
		m_EnemyState = eDefense;
		m_EnemyState &= eDefense;
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

		if (m_EnemyState & eGrassHopperDashF)
		{
			Math::Vector3 vec = m_mWorld.Backward();
			Math::Matrix efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)) * Math::Matrix::CreateTranslation({ m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z });
			KdEffekseerManager::GetInstance().SetWorldMatrix(fileName, efcMat);
		}
		else if (m_EnemyState & eGrassHopperDashB)
		{
			Math::Vector3 vec = m_mWorld.Forward();
			Math::Matrix efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y + 180.0f)) * Math::Matrix::CreateTranslation({ m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z });
			KdEffekseerManager::GetInstance().SetWorldMatrix(fileName, efcMat);
		}
		else if (m_EnemyState & eGrassHopperDashR)
		{
			Math::Vector3 vec = m_mWorld.Right();
			Math::Matrix efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y + 90.0f)) * Math::Matrix::CreateTranslation({ m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z });
			KdEffekseerManager::GetInstance().SetWorldMatrix(fileName, efcMat);
		}
		else if (m_EnemyState & eGrassHopperDashL)
		{
			Math::Vector3 vec = m_mWorld.Left();
			Math::Matrix efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y + 270.0f)) * Math::Matrix::CreateTranslation({ m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z });
			KdEffekseerManager::GetInstance().SetWorldMatrix(fileName, efcMat);
		}
		else if (m_EnemyState & eGrassHopperDashUp)
		{
			Math::Matrix efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(270.0f)) * Math::Matrix::CreateTranslation({ m_pos.x,m_pos.y + 2.9f,m_pos.z });
			KdEffekseerManager::GetInstance().SetWorldMatrix(fileName, efcMat);
		}

		m_bMove = true;
		if (m_lGrassHopperTime <= 90 && m_lGrassHopperTime > 80 || m_rGrassHopperTime <= 90 && m_rGrassHopperTime > 80)
		{
			m_dashSpd = 0.0f;

			float y = m_target.lock()->GetPos().y - m_pos.y;
			if (y >= -0.5f && y <= 0.5f)
			{
				if (m_EnemyState & eGrassHopperDashUp)
				{
					m_wantToMoveState = Enemy::WantToMoveState::dashAttack;
					m_dashSpd = 0.0f;
					m_grassHopperDashDir = {};
					m_gravity = 0;
					m_rGrassHopperPauCnt = 0;
					m_lGrassHopperPauCnt = 0;
					GrassMoveVecDecision();
				}
			}
		}
		else if (m_lGrassHopperTime <= 75 && m_lGrassHopperTime > 30 || m_rGrassHopperTime <= 75 && m_rGrassHopperTime > 30)
		{
			if (m_enemyType & EnemyType::speedSter)
			{
				if (m_lGrassHopperTime <= 75 && m_lGrassHopperTime > 60 || m_rGrassHopperTime <= 75 && m_rGrassHopperTime > 60)
				{
					m_dashSpd = 1.2f;
				}
				else
				{
					m_dashSpd = 0.5f;
				}
			}
			else
			{
				m_dashSpd = 0.8f;
			}
		}
		else if (m_lGrassHopperTime <= 30 && m_lGrassHopperTime > 25 || m_rGrassHopperTime <= 30 && m_rGrassHopperTime > 25)
		{
			if (m_enemyType & EnemyType::speedSter)
			{
				m_dashSpd = 0.4f;
			}
			else
			{
				m_dashSpd = 0.5f;
			}
		}
		else if (m_lGrassHopperTime <= 25 && m_lGrassHopperTime > 20 || m_rGrassHopperTime <= 25 && m_rGrassHopperTime > 20)
		{
			if (m_enemyType & EnemyType::speedSter)
			{
				m_dashSpd = 0.35f;
			}
			else
			{
				m_dashSpd = 0.40f;
			}
		}
		else if (m_lGrassHopperTime <= 20 && m_lGrassHopperTime > 15 || m_rGrassHopperTime <= 20 && m_rGrassHopperTime > 15)
		{
			if (m_enemyType & EnemyType::speedSter)
			{
				m_dashSpd = 0.30f;
			}
			else
			{
				m_dashSpd = 0.35f;
			}
		}
		else if (m_lGrassHopperTime <= 15 && m_lGrassHopperTime > 10 || m_rGrassHopperTime <= 15 && m_rGrassHopperTime > 10)
		{
			if (m_enemyType & EnemyType::speedSter)
			{
				m_dashSpd = 0.25f;
			}
			else
			{
				m_dashSpd = 0.30f;
			}
		}
		else if (m_lGrassHopperTime <= 10 && m_lGrassHopperTime > 5 || m_rGrassHopperTime <= 10 && m_rGrassHopperTime > 5)
		{
			if (m_enemyType & EnemyType::speedSter)
			{
				m_dashSpd = 0.20f;
			}
			else
			{
				m_dashSpd = 0.25f;
			}
		}
		else if (m_lGrassHopperTime <= 5 && m_lGrassHopperTime > 0 || m_rGrassHopperTime <= 5 && m_rGrassHopperTime > 0)
		{
			if (m_enemyType & EnemyType::speedSter)
			{
				m_dashSpd = 0.10f;
			}
			else
			{
				m_dashSpd = 0.20f;
			}
		}

		if (m_lGrassHopperTime == 80)
		{
			m_weaponList[3]->StartAnime();
			m_animator->SetAnimation(m_model->GetAnimation("GrassDashFA"), true);
		}

		if (m_rGrassHopperTime == 80)
		{
			m_weaponList[2]->StartAnime();
			m_animator->SetAnimation(m_model->GetAnimation("GrassDashFA"), true);
		}

		Math::Vector3  src = m_target.lock()->GetPos() - m_pos;
		if (src.Length() <= 1.2f)
		{
			if (m_lGrassHopperTime <= 75 || m_rGrassHopperTime <= 75)
			{
				if (!(m_EnemyState & (eRAttackOne | eRAttackTwo)))
				{
					ScorpionAttackDecision();
				}
			}
		}

		float y = m_target.lock()->GetPos().y - m_pos.y;
		if (y >= -0.35f && y <= 0.35f)
		{
			if (m_EnemyState & eGrassHopperDashUp)
			{
				m_wantToMoveState = Enemy::WantToMoveState::dashAttack;
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
		m_EnemyState = eFall;
		Brain();
	}


	m_pos += m_grassHopperDashDir * m_dashSpd;

	if (m_EnemyState & eGrassHopperDash)
	{
		UpdateRotate(m_grassHopperDashDir);
	}
	else if (m_EnemyState & eGrassHopperDashUp)
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
			if (m_EnemyState & eStepF)
			{
				m_animator->SetAnimation(m_model->GetAnimation("GrassDashFA"), true);
			}
			else if (m_EnemyState & eStepR)
			{
				m_animator->SetAnimation(m_model->GetAnimation("GrassDashRA"), true);
			}
			else if (m_EnemyState & eStepL)
			{
				m_animator->SetAnimation(m_model->GetAnimation("GrassDashLA"), true);
			}
			else if (m_EnemyState & eStepB)
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
		m_EnemyState = eIdle;
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
				m_EnemyState = eStepR;
				m_animator->SetAnimation(m_model->GetAnimation("GrassDashRB"), false);
				break;
			case 1:
				m_stepDashDir = Math::Vector3::TransformNormal(Math::Vector3(0, -1, 0), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
				m_bMove = true;
				m_stepCnt = 60;
				m_EnemyState = eStepL;
				m_animator->SetAnimation(m_model->GetAnimation("GrassDashLB"), false);
				break;
			case 2:
				m_stepDashDir = Math::Vector3::TransformNormal(Math::Vector3(0, 0, 1), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
				m_bMove = true;
				m_stepCnt = 60;
				m_EnemyState = eStepF;
				m_animator->SetAnimation(m_model->GetAnimation("GrassDashFB"), false);
				break;
			case 3:
				m_stepDashDir = Math::Vector3::TransformNormal(Math::Vector3(0, 0, -1), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
				m_bMove = true;
				m_stepCnt = 60;
				m_EnemyState = eStepB;
				m_animator->SetAnimation(m_model->GetAnimation("GrassDashBB"), false);
				break;
			}
		}
	}
}

void Enemy::NormalMoveVecDecision()
{
	std::random_device rnd;
	std::mt19937 mt(rnd());
	std::uniform_int_distribution<int> intRand(0, 999);
	int randNum[2] = {};

	int rand = intRand(mt);
	randNum[0] = 900;
	randNum[1] = 100;

	for (int i = 0; i < 2; i++)
	{
		rand -= randNum[i];
		if (rand < 0)
		{
			switch (i)
			{
			case 0:
				if (!(m_EnemyState & eRun))
				{
					m_animator->SetAnimation(m_model->GetAnimation("RUN"));
				}
				m_EnemyState = eRun;
				break;
			case 1:
				m_EnemyState = eJump;
				break;
			}
			break;
		}
	}
}

void Enemy::NormalMove()
{
	Math::Vector3 moveVec = {};
	float moveSpd = 0.25f;

	if (m_EnemyState & eRun)
	{
		moveVec += Math::Vector3::TransformNormal({ 0, 0, 1 }, Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
		m_bMove = true;
		m_runAnimeCnt = 0;
	}

	if (m_EnemyState & eJump)
	{
		if (!(m_EnemyState & (eJump | eFall)))
		{
			m_gravity = -0.2f;
			m_EnemyState |= eJump;
			m_bMove = true;

		}
	}

	if (!(m_EnemyState & (eGrassHopperDash | eGrassHopperDashUp)))
	{
		m_pos += moveVec * moveSpd;
	}

	m_pos.y -= m_gravity;
	m_gravity += 0.01f;

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
			m_wantToMoveState = attack;
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
			m_EnemyState = eIdle;
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
		AllRounderBrain();

		if (m_bEnemyBetweenPlayer)
		{
			m_wantToMoveState = Enemy::WantToMoveState::grassDash;

			m_leftWeaponNumber = 2;
			m_rightWeaponNumber = 1;
		}
		break;
	case Enemy::EnemyType::coarseFishEnemy:
		CoarseFishEnemyBrain();
		break;
	}

	if (!m_bEnemyBetweenPlayer)
	{
		float y = m_target.lock()->GetPos().y - m_pos.y;
		if (y <= -0.85f)
		{
			if (!(m_EnemyState & (eHit | eHasDefense)))
			{
				if (!(m_EnemyState & eFall))
				{
					m_animator->SetAnimation(m_model->GetAnimation("FallA"), false);
				}
				m_EnemyState = eFall;
				m_wantToMoveState = none;
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
		if (spTarget->GetPlayerState() & (Player::PlayerState::rAttack | Player::PlayerState::lAttack | Player::PlayerState::rlAttack))
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
		if (src.Length() <= 8.0f && spTarget->GetPlayerState() & Player::PlayerState::grassHopperDashF)
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
		else if (src.Length() <= 2.5f && m_target.lock()->GetPlayerState() & Player::PlayerState::defense && !(m_EnemyState & eGrassHopperDash))
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
		m_wantToMoveState = Enemy::WantToMoveState::attack;
		if (m_target.lock()->GetPlayerState() & Player::PlayerState::defense && !(m_EnemyState & eGrassHopperDash))
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
					m_wantToMoveState = Enemy::WantToMoveState::step;
					break;
				case 1:
					m_wantToMoveState = Enemy::WantToMoveState::defense;
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
		m_wantToMoveState = Enemy::WantToMoveState::run;
		break;
	case Enemy::WantToMoveCategory::grassCategory:
		m_wantToMoveState = Enemy::WantToMoveState::dashAttack;
		break;
	}

	switch (m_wantToMoveState)
	{
	case WantToMoveState::attack:
		m_leftWeaponNumber = 1;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::escape:
		m_leftWeaponNumber = 1;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::defense:
		m_leftWeaponNumber = 1;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::dashAttack:
		m_leftWeaponNumber = 2;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::run:
		m_leftWeaponNumber = 1;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::step:
		m_leftWeaponNumber = 1;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::grassDash:
		m_leftWeaponNumber = 1;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::avoidance:
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
		if (spTarget->GetPlayerState() & (Player::PlayerState::rAttack | Player::PlayerState::lAttack | Player::PlayerState::rlAttack))
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
		else if (src.Length() <= 2.5f && m_target.lock()->GetPlayerState() & Player::PlayerState::defense && !(m_EnemyState & eGrassHopperDash))
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
		m_wantToMoveState = Enemy::WantToMoveState::attack;
		if (m_target.lock()->GetPlayerState() & Player::PlayerState::defense && !(m_EnemyState & eGrassHopperDash))
		{
			m_bMantisAttack = true;
		}
		break;
	case Enemy::WantToMoveCategory::defenseCategory:
		m_wantToMoveState = Enemy::WantToMoveState::defense;
		break;
	case Enemy::WantToMoveCategory::approachCategory:
		m_wantToMoveCategory = Enemy::WantToMoveCategory::runCategory;
		break;
	}

	switch (m_wantToMoveCategory)
	{
	case Enemy::WantToMoveCategory::runCategory:
		m_wantToMoveState = Enemy::WantToMoveState::run;
		break;
	}

	switch (m_wantToMoveState)
	{
	case WantToMoveState::attack:
		m_leftWeaponNumber = 1;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::escape:
		m_leftWeaponNumber = 2;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::defense:
		m_leftWeaponNumber = 1;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::dashAttack:
		m_leftWeaponNumber = 2;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::run:
		m_leftWeaponNumber = 1;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::step:
		m_leftWeaponNumber = 1;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::grassDash:
		m_leftWeaponNumber = 2;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::avoidance:
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
		if (spTarget->GetPlayerState() & (Player::PlayerState::rAttack | Player::PlayerState::lAttack | Player::PlayerState::rlAttack))
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
		if (src.Length() <= 5.0f && spTarget->GetPlayerState() & Player::PlayerState::grassHopperDashF)
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
		else if (src.Length() <= 2.5f && m_target.lock()->GetPlayerState() & Player::PlayerState::defense && !(m_EnemyState & eGrassHopperDash))
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
		if (m_target.lock()->GetPlayerState() & Player::PlayerState::defense && !(m_EnemyState & eGrassHopperDash))
		{
			m_wantToMoveState = Enemy::WantToMoveState::attack;
			m_bMantisAttack = true;
		}
		else
		{
			m_wantToMoveState = Enemy::WantToMoveState::dashAttack;
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
					m_wantToMoveState = Enemy::WantToMoveState::avoidance;
					break;
				case 1:
					m_wantToMoveState = Enemy::WantToMoveState::escape;
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
					m_wantToMoveState = Enemy::WantToMoveState::grassDash;
					break;
				case 1:
					m_wantToMoveState = Enemy::WantToMoveState::dashAttack;
					break;
				}
				break;
			}
		}
		break;
	}

	switch (m_wantToMoveState)
	{
	case WantToMoveState::attack:
		m_leftWeaponNumber = 1;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::escape:
		m_leftWeaponNumber = 2;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::defense:
		m_leftWeaponNumber = 1;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::dashAttack:
		m_leftWeaponNumber = 2;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::run:
		m_leftWeaponNumber = 1;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::step:
		m_leftWeaponNumber = 1;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::grassDash:
		m_leftWeaponNumber = 2;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::avoidance:
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

	if (src.Length() <= 8.0f)
	{
		if (spTarget->GetPlayerState() & (Player::PlayerState::rAttack | Player::PlayerState::lAttack | Player::PlayerState::rlAttack))
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
		if (src.Length() <= 5.0f && spTarget->GetPlayerState() & Player::PlayerState::grassHopperDashF)
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
		else if (src.Length() <= 2.5f && m_target.lock()->GetPlayerState() & Player::PlayerState::defense && !(m_EnemyState & eGrassHopperDash))
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
		m_wantToMoveState = Enemy::WantToMoveState::attack;
		if (m_target.lock()->GetPlayerState() & Player::PlayerState::defense && !(m_EnemyState & eGrassHopperDash))
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
					m_wantToMoveState = Enemy::WantToMoveState::defense;
					break;
				case 1:
					m_wantToMoveState = Enemy::WantToMoveState::avoidance;
					break;
				case 2:
					m_wantToMoveState = Enemy::WantToMoveState::escape;
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
		m_wantToMoveState = Enemy::WantToMoveState::run;
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
					m_wantToMoveState = Enemy::WantToMoveState::grassDash;
					break;
				case 1:
					m_wantToMoveState = Enemy::WantToMoveState::dashAttack;
					break;
				}
				break;
			}
		}
		break;
	}

	switch (m_wantToMoveState)
	{
	case WantToMoveState::attack:
		m_leftWeaponNumber = 1;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::escape:
		m_leftWeaponNumber = 2;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::defense:
		m_leftWeaponNumber = 1;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::dashAttack:
		m_leftWeaponNumber = 2;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::run:
		m_leftWeaponNumber = 1;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::step:
		m_leftWeaponNumber = 1;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::grassDash:
		m_leftWeaponNumber = 2;
		m_rightWeaponNumber = 1;
		break;
	case WantToMoveState::avoidance:
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

	if (m_coarseFishEnemyAttackDelayCnt == 0)
	{
		if (src.Length() <= 2.0f)
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
						m_wantToMoveState = Enemy::WantToMoveState::none;
						break;
					case 1:
						m_wantToMoveState = Enemy::WantToMoveState::attack;
						break;
					}
					break;
				}
			}
		}
		else
		{
			m_wantToMoveState = Enemy::WantToMoveState::run;
		}
	}
	else
	{
		m_wantToMoveState = Enemy::WantToMoveState::run;
	}
}

void Enemy::ScorpionAttackMove()
{
	if (m_animator->IsAnimationEnd())
	{
		if (m_EnemyState & eLAttack)
		{
			m_EnemyState &= ~eLAttack;
			if (m_EnemyState & eGrassHopperDashF | eStep && m_bBoss)
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

		if (m_EnemyState & eRAttack)
		{
			m_EnemyState &= ~eRAttack;
			if (m_EnemyState & eGrassHopperDashF | eStep && m_bBoss)
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

		if (m_EnemyState & (eRlAttack | eRlAttackRush | eMantis))
		{
			m_EnemyState = eIdle;
			Brain();
		}
	}
	else
	{
		if (!(m_EnemyState & (eRlAttack | eRlAttackRush)))
		{
			m_attackMoveSpd *= 0.95f;

			if (m_EnemyState & (eRAttackTwo | eLAttackTwo))
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
			else if (m_EnemyState & (eRAttackThree | eLAttackThree))
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
		if (!(m_EnemyState & eRlAttackRush))
		{
			if (m_attackAnimeCnt >= 15)
			{
				m_attackMoveSpd *= 0.25f;
			}
			else
			{
				if (!m_target.expired())
				{
					Math::Vector3 dis = m_target.lock()->GetPos() - m_pos;
					if (dis.Length() <= 1.25f)
					{
						m_attackMoveSpd *= 0.25f;
					}
				}
				/*else
				{
					for (auto& enemyList : m_enemyList)
					{
						if (enemyList.expired())continue;
						Math::Vector3 dis = enemyList.lock()->GetPos() - m_pos;
						if (dis.Length() <= 1.25f)
						{
							m_attackMoveSpd *= 0.25f;
							break;
						}
					}
				}*/
			}
		}

		m_bMove = true;

		if (m_bAttackAnimeDelay)
		{
			if (m_target.lock()->GetAttackHit())
			{
				m_wantToMoveState = WantToMoveState::attack;
			}

			if (m_bBoss)
			{
				switch (m_wantToMoveState)
				{
				case WantToMoveState::attack:
					m_leftWeaponNumber = 1;
					m_rightWeaponNumber = 1;
					break;
				}
			}
		}

		if (!(m_EnemyState & (eRlAttack | eRlAttackRush)))
		{
			m_attackMoveSpd *= 0.95f;
		}
		else
		{
			if (m_EnemyState & eRlAttackOne)
			{
				m_attackMoveSpd *= 0.95f;
				if (m_attackAnimeCnt == 17)
				{
					m_target.lock()->SetAttackHit(false);
					m_target.lock()->SetDefenseSuc(false);
				}
			}
			else if (m_EnemyState & eRlAttackTwo)
			{
				m_attackMoveSpd *= 0.95f;
				if (m_attackAnimeCnt == 16)
				{
					m_target.lock()->SetAttackHit(false);
					m_target.lock()->SetDefenseSuc(false);
				}
			}
			else if (m_EnemyState & eRlAttackThree)
			{
				m_attackMoveSpd *= 0.95f;
				if (m_attackAnimeCnt == 23)
				{
					m_target.lock()->SetAttackHit(false);
					m_target.lock()->SetDefenseSuc(false);
				}
			}
			else if (m_EnemyState & eRlAttackRush)
			{
				m_attackMoveSpd *= 0.55f;
				if (m_attackAnimeCnt == 21 ||
					m_attackAnimeCnt == 31 ||
					m_attackAnimeCnt == 49 ||
					m_attackAnimeCnt == 57 ||
					m_attackAnimeCnt == 74 ||
					m_attackAnimeCnt == 89 ||
					m_attackAnimeCnt == 107
					)
				{
					m_target.lock()->SetAttackHit(false);
					m_target.lock()->SetDefenseSuc(false);

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
					case 107:
						m_attackMoveSpd = 0.15f;
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
	if (!(m_EnemyState & (eRAttack | eLAttack | eMantis | eRlAttack | eRlAttackRush)))
	{
		if ((m_weaponType & eScorpion) && (m_weaponType & eLScorpion))
		{
			if (m_bMantisAttack && m_bMantisPossAng)
			{
				m_EnemyState |= eMantis;
				m_EnemyState &= eMantis;
				if (m_EnemyState & eGrassHopperDash)
				{
					m_EnemyState &= ~eGrassHopperDash;
				}

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
				m_EnemyState |= eRlAttackOne;
				m_EnemyState &= ~eRAttack;
				m_EnemyState &= ~eLAttack;
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
		m_target.lock()->SetAttackHit(false);
		m_target.lock()->SetDefenseSuc(false);
	}
	else
	{
		if (m_bAttackAnimeDelay)
		{
			if (m_EnemyState & eRAttackOne)
			{
				m_EnemyState |= eRAttackTwo;
				m_EnemyState &= ~eRAttackOne;
				m_bAttackAnimeDelay = false;
				m_bAttackAnimeCnt = true;
				m_animator->SetAnimation(m_model->GetAnimation("RAttack2"), false);
				m_bMove = true;
				m_attackMoveDir = Math::Vector3::TransformNormal(Math::Vector3(0, 0, 1), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
				m_attackMoveDir.y = 0;
				m_attackMoveDir.Normalize();
				m_attackMoveSpd = 0.8f;
			}
			else if (m_EnemyState & eRAttackTwo)
			{
				m_EnemyState |= eRAttackThree;
				m_EnemyState &= ~eRAttackTwo;
				m_bAttackAnimeDelay = false;
				m_bAttackAnimeCnt = false;
				m_animator->SetAnimation(m_model->GetAnimation("RAttack3"), false);
				m_bMove = true;
				m_attackMoveDir = Math::Vector3::TransformNormal(Math::Vector3(0, 0, 1), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
				m_attackMoveDir.y = 0;
				m_attackMoveDir.Normalize();
				m_attackMoveSpd = 0.8f;
			}

			if (m_EnemyState & eLAttackOne)
			{
				m_EnemyState |= eLAttackTwo;
				m_EnemyState &= ~eLAttackOne;
				m_bAttackAnimeDelay = false;
				m_bAttackAnimeCnt = true;
				m_animator->SetAnimation(m_model->GetAnimation("LAttack2"), false);
				m_bMove = true;
				m_attackMoveDir = Math::Vector3::TransformNormal(Math::Vector3(0, 0, 1), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
				m_attackMoveDir.y = 0;
				m_attackMoveDir.Normalize();
				m_attackMoveSpd = 0.8f;
			}
			else if (m_EnemyState & eLAttackTwo)
			{
				m_EnemyState |= eLAttackThree;
				m_EnemyState &= ~eLAttackTwo;
				m_bAttackAnimeDelay = false;
				m_bAttackAnimeCnt = false;
				m_animator->SetAnimation(m_model->GetAnimation("LAttack3"), false);
				m_bMove = true;
				m_attackMoveDir = Math::Vector3::TransformNormal(Math::Vector3(0, 0, 1), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
				m_attackMoveDir.y = 0;
				m_attackMoveDir.Normalize();
				m_attackMoveSpd = 0.8f;
			}

			if (m_EnemyState & eRlAttackOne)
			{
				m_target.lock()->SetAttackHit(false);
				m_target.lock()->SetDefenseSuc(false);

				m_EnemyState |= eRlAttackTwo;
				m_EnemyState &= ~eRlAttackOne;
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
				m_target.lock()->SetAttackHit(false);
				m_target.lock()->SetDefenseSuc(false);
			}
			else if (m_EnemyState & eRlAttackTwo)
			{
				m_target.lock()->SetAttackHit(false);
				m_target.lock()->SetDefenseSuc(false);

				m_EnemyState |= eRlAttackThree;
				m_EnemyState &= ~eRlAttackTwo;
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
				m_target.lock()->SetAttackHit(false);
				m_target.lock()->SetDefenseSuc(false);
			}
			else if (m_EnemyState & eRlAttackThree && m_bRushAttackPossible)
			{
				m_target.lock()->SetAttackHit(false);
				m_target.lock()->SetDefenseSuc(false);

				m_EnemyState |= eRlAttackRush;
				m_EnemyState &= eRlAttackRush;
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
				m_target.lock()->SetAttackHit(false);
				m_target.lock()->SetDefenseSuc(false);
			}
			m_EnemyState &= ~eLAttack;
		}
	}

	if (!(m_EnemyState & (eRAttack | eLAttack | eMantis | eRlAttack | eRlAttackRush)))
	{
		if (m_weaponType & eScorpion)
		{
			m_EnemyState |= eRAttackOne;
			m_EnemyState &= ~eLAttack;
			m_bAttackAnimeDelay = false;
			m_bAttackAnimeCnt = true;
			m_attackAnimeCnt = 0;
			m_attackAnimeDelayCnt = 0;
			m_bMove = true;
			m_attackMoveDir = Math::Vector3::TransformNormal(Math::Vector3(0, 0, 1), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
			m_attackMoveDir.y = 0;
			m_attackMoveDir.Normalize();
			m_target.lock()->SetAttackHit(false);
			m_target.lock()->SetDefenseSuc(false);
			
			m_attackMoveSpd = 0.8f;

			if (m_EnemyState & eGrassHopperDash && m_bBoss)
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
			m_EnemyState |= eLAttackOne;
			m_EnemyState &= ~eRAttack;
			m_bAttackAnimeDelay = false;
			m_bAttackAnimeCnt = true;
			m_attackAnimeCnt = 0;
			m_attackAnimeDelayCnt = 0;
			m_bMove = true;
			m_attackMoveDir = Math::Vector3::TransformNormal(Math::Vector3(0, 0, 1), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
			m_attackMoveDir.y = 0;
			m_attackMoveDir.Normalize();
			m_target.lock()->SetAttackHit(false);
			m_target.lock()->SetDefenseSuc(false);
			
			m_attackMoveSpd = 0.8f;
			if (m_EnemyState & eGrassHopperDash && m_bBoss)
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

