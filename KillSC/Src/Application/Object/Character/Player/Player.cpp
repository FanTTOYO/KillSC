#include "Player.h"
#include "../../../Camera/GameCamera/GameCamera.h"
#include "../../../Scene/SceneManager.h"
#include "../../Weapon/Hopper/Hopper.h"
#include "../../Weapon/Scopion/Scopion.h"
#include "../Enemy/Enemy.h"
#include "../../Ui/Ui.h"

void Player::Init()
{
	// 座標行列
	Math::Matrix transMat;
	transMat = Math::Matrix::CreateTranslation(0, 0, -INITIALPOSZ);
	m_pos = transMat.Translation();

	// 拡縮行列
	Math::Matrix scaleMat;
	scaleMat = Math::Matrix::CreateScale(Math::Vector3::One);

	// 行列合成
	m_mWorld = scaleMat * transMat;

	m_model = std::make_shared<KdModelWork>();
	m_model->SetModelData
	(KdAssets::Instance().m_modeldatas.GetData
	("Asset/Models/Player/Player.gltf"));
	// 当たり判定初期化
	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape
	("PlayerModel", m_model, KdCollider::TypeBump | KdCollider::TypeDamage);

	m_pDebugWire = std::make_unique<KdDebugWireFrame>();

	m_playerState = idle;

	m_weaponType |= scorpion;
	m_weaponType |= lScorpion;

	m_rightWeaponNumber = 1;
	m_leftWeaponNumber = 1;

	m_torion = 300.0f;
	m_endurance = 400;
	m_graduallyTorionDecVal = 0.0f;
	m_bAttackAnimeCnt = true;
	m_bRushAttackPossible = false;
	m_invincibilityTimeCnt = 0;

	m_animator = std::make_shared<KdAnimator>();
	m_animator->SetAnimation(m_model->GetAnimation("IdleA"), false);


	std::shared_ptr<Scopion> sco;
	sco = std::make_shared<Scopion>();
	sco->SetArrmType(rArrm);
	sco->SetbPlayerWeapon();
	m_weaponList.push_back(sco);

	std::shared_ptr<Scopion> sco2;
	sco2 = std::make_shared<Scopion>();
	sco2->SetArrmType(lArrm);
	sco2->SetbPlayerWeapon();
	m_weaponList.push_back(sco2);

	for (auto& enemyList : m_enemyList)
	{
		sco->AddTarget(enemyList.lock());
		sco2->AddTarget(enemyList.lock());
	}
	std::shared_ptr<Hopper> hopper;
	hopper = std::make_shared<Hopper>();
	hopper->SetArrmType(rArrm);
	hopper->SetbPlayerWeapon();
	m_weaponList.push_back(hopper);

	std::shared_ptr<Hopper> hopper2;
	hopper2 = std::make_shared<Hopper>();
	hopper2->SetArrmType(lArrm);
	hopper2->SetbPlayerWeapon();
	m_weaponList.push_back(hopper2);
	m_mWorldRot = {};
	m_bFirstUpdate = true;
	m_bEwaponDrawPosible = false;
	m_overStageTime = 0;
	m_bPlayerDeath = false;
	m_bPlayerLose = false;

	m_rocKOnPolygon = std::make_shared<KdSquarePolygon>();
	m_rocKOnPolygon->SetMaterial("Asset/Textures/Ui/Game/ターゲット N 白.png");

	m_hitColorChangeTimeCnt = 0;

	m_bRushRp = false;
}

void Player::AddWeaponToEnemy(std::shared_ptr<Enemy> a_enemy)
{
	const std::shared_ptr<Scopion> gScopion1 = std::dynamic_pointer_cast<Scopion>(m_weaponList[0]);
	const std::shared_ptr<Scopion> gScopion2 = std::dynamic_pointer_cast<Scopion>(m_weaponList[1]);

	gScopion1->AddTarget(a_enemy);
	gScopion2->AddTarget(a_enemy);
}

void Player::Update()
{
	if (m_bPlayerLose)return;

	/*if (SceneManager::Instance().GetSceneType() != SceneManager::SceneType::tutorial)
	{*/
		float lowestYPos;
		if (SceneManager::Instance().GetSceneType() == SceneManager::SceneType::tutorial)
		{
			lowestYPos = -10.0f;
		}
		else
		{
			lowestYPos = -1.0f;
		}

		//if (m_pos.x > 62.5 || m_pos.x < -62.5 || m_pos.z > 62.5 || m_pos.z < -62.5 || m_pos.y < lowestYPos)
		if(0)
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
	//}

	if (m_bFirstUpdate)
	{
		m_bFirstUpdate = false;
		for (auto& WeaList : m_weaponList)
		{
			WeaList->SetOwner(shared_from_this());
		}

		return;
	}

	if (!m_bEwaponDrawPosible)
	{
		m_bEwaponDrawPosible = true;
	}

	if (m_hitColorChangeTimeCnt > 0)
	{
		--m_hitColorChangeTimeCnt;
	}

	m_torion -= m_graduallyTorionDecVal;
	if (m_torion <= 0)
	{
		m_torion = 0;
		if (!m_bPlayerDeath)
		{
			const std::shared_ptr<GameCamera> gCamera = std::dynamic_pointer_cast<GameCamera>(m_wpCamera.lock());
			m_bPlayerDeath = true;
			m_animator->SetAnimation(m_model->GetAnimation("Death"), false);
			m_playerState = idle;
			gCamera->SetBRotateEnemy(false);
			m_enemy.reset();
		}
	}

	if (m_endurance <= 0)
	{
		m_endurance = 0;
		if (!m_bPlayerDeath)
		{
			const std::shared_ptr<GameCamera> gCamera = std::dynamic_pointer_cast<GameCamera>(m_wpCamera.lock());
			m_bPlayerDeath = true;
			m_animator->SetAnimation(m_model->GetAnimation("Death"), false);
			m_playerState = idle;
			gCamera->SetBRotateEnemy(false);
			m_enemy.reset();
		}
	}

	if (m_invincibilityTimeCnt > 0)
	{
		--m_invincibilityTimeCnt;
	}

	if (SceneManager::Instance().GetSceneType() == SceneManager::SceneType::tutorial)
	{
		TutorialUpdate();
	}
	else
	{
		// debugキー
		if (GetAsyncKeyState('L') & 0x8000)
		{
			m_torion = 0;
		}
	}

	if (!(m_playerState & (fall | jump)))
	{
		m_bMove = false;
	}

	if (!(m_playerState & (mantis | hasDefense)))
	{
		if (GetAsyncKeyState('Q') & 0x8000)
		{
			if (!m_bLShiftKeyState)
			{
				m_bLShiftKeyState = true;
				m_leftWeaponNumber++;
				if (m_leftWeaponNumber > MAXWEAPONTYPE)
				{
					m_leftWeaponNumber = FIRSTWEAPONTYPENUMBER;
				}

				if (SceneManager::Instance().GetSceneType() == SceneManager::SceneType::tutorial)
				{
					switch (m_wpUi.lock()->GetTutorialType())
					{

					case Ui::TutorialType::bukiTu:
						m_wpUi.lock()->AddTutorialCnt();
						break;
					}
				}
			}
		}
		else
		{
			m_bLShiftKeyState = false;
		}

		if (GetAsyncKeyState('E') & 0x8000)
		{
			if (!m_bFKeyState)
			{
				m_bFKeyState = true;
				m_rightWeaponNumber++;
				if (m_rightWeaponNumber > MAXWEAPONTYPE)
				{
					m_rightWeaponNumber = FIRSTWEAPONTYPENUMBER;
				}

				if (SceneManager::Instance().GetSceneType() == SceneManager::SceneType::tutorial)
				{
					switch (m_wpUi.lock()->GetTutorialType())
					{

					case Ui::TutorialType::bukiTu:
						m_wpUi.lock()->AddTutorialCnt();
						break;
					}
				}
			}
		}
		else
		{
			m_bFKeyState = false;
		}
	}

	switch (m_rightWeaponNumber) // 後に番号を自由に選べるようになるかも
	{
	case 1:
		m_weaponType |= scorpion;
		m_weaponType &= ~grassHopper;
		break;
	case 2:
		m_weaponType |= grassHopper;
		m_weaponType &= ~scorpion;
		break;
	}

	switch (m_leftWeaponNumber) // 後に番号を自由に選べるようになるかも
	{
	case 1:
		m_weaponType |= lScorpion;
		m_weaponType &= ~lGrassHopper;
		break;
	case 2:
		m_weaponType |= lGrassHopper;
		m_weaponType &= ~lScorpion;
		break;
	}

	if (!(m_playerState & (hit | rise)) && !m_bPlayerDeath)
	{
		if (GetAsyncKeyState(VK_MBUTTON) & 0x80000)
		{
			if (!m_bMButtonState)
			{
				const std::shared_ptr<GameCamera> gCamera = std::dynamic_pointer_cast<GameCamera>(m_wpCamera.lock());
				if (gCamera->GetBRotateEnemy())
				{
					gCamera->SetBRotateEnemy(false);
					m_enemy.reset();
				}
				else
				{
					float smallAng = 0;
					int i = 0;
					for (auto& enemyList : m_enemyList)
					{
						if (enemyList.expired())
						{
							continue;
						}

						if (enemyList.lock()->GetBEnemyDeath())
						{
							continue;
						}

						Math::Vector3 nowVec = gCamera->GetMatrix().Backward();
						nowVec.y = 0.0f;
						nowVec.Normalize();

						// 向きたい方向
						Math::Vector3 toVec = enemyList.lock()->GetPos() - GetPos();
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
						if (i == 0)
						{
							smallAng = ang;
							++i;
							gCamera->SetEnemy(enemyList.lock());
							m_enemy = enemyList.lock();
							continue;
						}

						if (smallAng > ang)
						{
							smallAng = ang;
							gCamera->SetEnemy(enemyList.lock());
							m_enemy = enemyList.lock();
							continue;
						}
						else if (smallAng == ang)
						{
							Math::Vector3 cross = DirectX::XMVector3Cross(nowVec, toVec);
							if (cross.y >= 0)
							{
								smallAng = ang;
								++i;
								gCamera->SetEnemy(enemyList.lock());
								m_enemy = enemyList.lock();
								continue;
							}
						}
					}
				}
				m_bMButtonState = true;
			}
		}
		else
		{
			m_bMButtonState = false;
		}

		if (!(m_playerState & hasDefense))
		{
			if (m_weaponType & (lGrassHopper | grassHopper))
			{
				GrassMoveVecDecision();
			}

			if (m_weaponType & (lScorpion | scorpion))
			{
				ScorpionActionDecision();
			}
		}

		if (!(m_playerState & (grassHopperDash | grassHopperDashUp)))
		{
			if (m_playerState & (lAttack | rAttack | mantis | rlAttack | rlAttackRush))
			{
				ScorpionAttackMove();
			}

			if (m_playerState & (defense))
			{
				ScorpionDefenseMove();
			}

			if (m_playerState & step)
			{
				StepMove();
			}
			else if (!(m_playerState & hasDefense))
			{
				NormalMove();
			}
			else
			{
				HasDefenseMove();
			}

		}
		else
		{
			GrassMove();
		}
	}
	else if (m_playerState & hit)
	{
		--m_hitStopCnt;
		m_bMove = true;
		m_pos.y -= m_gravity;
		if (m_playerState & cutRaiseHit)
		{
			m_gravity += 0.0075f;
		}
		else
		{
			m_gravity += 0.01f;
		}

		if (m_hitStopCnt <= 0)
		{
			m_hitStopCnt = 0;

			if (m_playerState & nomalHit)
			{
				if (!(m_playerState & idle))
				{
					m_animator->SetAnimation(m_model->GetAnimation("IdleA"), false);
				}
				m_playerState = idle;
			}
			else if (m_playerState & blowingAwayHit)
			{
				if (!(m_playerState & blowingAwayRise))
				{
					m_animator->SetAnimation(m_model->GetAnimation("BlowingAwayRise"), false);
				}
				m_playerState = blowingAwayRise;
			}
			else if (m_playerState & iaiKiriHit)
			{
				if (!(m_playerState & iaiKiriRise))
				{
					m_animator->SetAnimation(m_model->GetAnimation("IaiKiriRise"), false);
				}
				m_playerState = iaiKiriRise;
			}
			m_hitMoveSpd = false;
		}

		if (m_playerState & blowingAwayHit)
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
	else if (m_playerState & rise)
	{
		m_bMove = true;
		if (m_animator->IsAnimationEnd())
		{
			if (!(m_playerState & idle))
			{
				m_animator->SetAnimation(m_model->GetAnimation("IdleA"), false);
			}
			m_playerState = idle;
		}
	}
	else
	{
		m_pos.y -= m_gravity;
		m_gravity += 0.01f;
	}

	if (!m_bMove && !m_bPlayerDeath)
	{
		if (!(m_playerState & (fall | jump)))
		{
			if (!(m_playerState & idle))
			{
				m_animator->SetAnimation(m_model->GetAnimation("IdleA"), false);
			}
			m_playerState = idle;
		}
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
	if (!(m_playerState & (grassHopperDash | grassHopperDashUp)))
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
		//rayInfo.m_range = 0.25f;
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
		if (!(m_playerState & (grassHopperDash | grassHopperDashUp)))
		{
			//m_pos = groundPos;
			m_pos = groundPos + Math::Vector3(0, -0.7f, 0);
			m_gravity = 0;
			if (m_playerState & (fall | jump) && !m_bPlayerDeath)
			{
				m_bMove = false;
				if (!(m_playerState & idle))
				{
					m_animator->SetAnimation(m_model->GetAnimation("IdleA"), false);
				}
				m_playerState = idle;
			}

			m_wpCamera.lock()->SetStepOnPlayerPos(groundPos);
		}
		else
		{
			//m_pos = groundPos /*+ Math::Vector3(0,-0.1,0)*/;
			m_gravity = 0;
			//m_playerState = fall;
			m_rGrassHopperTime = 0;
			m_lGrassHopperTime = 0;
			//m_bMove = false;
			m_grassHopperDashDir = {};
			m_dashSpd = 0.0f;
		}
	}

	KdCollider::SphereInfo sphereInfo;
	// 球の中心位置を設定
	sphereInfo.m_sphere.Center = m_pos + Math::Vector3(0, 1.5f, 0);
	// 球の半径を設定

	if (!(m_playerState & (grassHopperDash | grassHopperDashUp | step)))
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

	for (auto& enemyList : m_enemyList)
	{
		if (enemyList.expired())continue;

		if (enemyList.lock()->GetEnemyState() & eDefense)
		{
			if (!(m_playerState & (grassHopperDash | grassHopperDashUp | step)))
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
			for (auto& obj : enemyList.lock()->GetWeaponList())
			{
				obj->Intersects
				(
					sphereInfo,
					&retSphereList
				);


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
		}
	}

	if (m_playerState & (rlAttack | rlAttackRush | PlayerState::hit))
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

	for (auto& enemyList : m_enemyList)
	{
		if (enemyList.expired())continue;

		enemyList.lock()->Intersects
		(
			sphereInfo,
			&retSphereList
		);

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

	if (!m_bRushRp)
	{
		if ((m_playerState & Player::PlayerState::rlAttackRush) && m_attackAnimeCnt >= 107)
		{
			PlayerKickHitAttackChaeck();
		}
	}
	else
	{
		if ((m_playerState & Player::PlayerState::rlAttackRush) && m_attackAnimeCnt >= 115)
		{
			PlayerPanchiHitAttackChaeck();
		}
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

	for (auto& WeaList : m_weaponList)
	{
		WeaList->Update();
	}

	if (m_enemy.expired())return;

	Math::Matrix mat = Math::Matrix::CreateTranslation(m_enemy.lock()->GetMatrix().Translation());
	m_rockOnPolyMat = mat;
	m_rockOnPolyMat._42 += 1.75f;
	m_rockOnPolyMat._41 += 0.5f * m_wpCamera.lock()->WorkCamera()->GetCameraMatrix().Forward().x;
	m_rockOnPolyMat._43 += 0.5f * m_wpCamera.lock()->WorkCamera()->GetCameraMatrix().Forward().z;
	m_rockOnPolyMat.Backward(m_wpCamera.lock()->WorkCamera()->GetCameraMatrix().Backward());
	m_rockOnPolyMat.Up(m_wpCamera.lock()->WorkCamera()->GetCameraMatrix().Up());
	m_rockOnPolyMat.Right(m_wpCamera.lock()->WorkCamera()->GetCameraMatrix().Right());
}

void Player::PlayerKickHitAttackChaeck()
{
	const KdModelWork::Node* node = nullptr;
	Math::Matrix mat = Math::Matrix::Identity;

	for (auto& enemyList : m_enemyList)
	{
		if (enemyList.expired())continue;

		if (!enemyList.lock()->GetAttackHit() && !enemyList.lock()->GetDefenseSuc() && enemyList.lock()->GetInvincibilityTimeCnt() == 0 && !enemyList.lock()->GetBEnemyDeath()) // ここにはなくてもいいかも
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

			enemyList.lock()->Intersects
			(
				sphereInfo,
				&retSphereList
			);

			Math::Vector3 hitDir = {};
			bool hit = false;
			Math::Vector3 hitPos = {};
			for (auto& ret : retSphereList)
			{
				hit = true;
				hitDir = ret.m_hitDir;
				hitPos = ret.m_hitPos;
			}

			if (hit)
			{
				enemyList.lock()->BlowingAwayAttackOnHit(m_mWorld.Backward());
				KdAudioManager::Instance().Play("Asset/Audio/SE/KickAttackHit.wav");
				hitPos.y += 0.35f;
				KdEffekseerManager::GetInstance().
					Play("Hit3.efk", hitPos);
				KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("Hit3.efk"); // これでループしない
				//KdEffekseerManager::GetInstance().SetRotation("Hit3.efk", m_mWorld.Backward(), DirectX::XMConvertToRadians(0));
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

				enemyList.lock()->Intersects
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
					enemyList.lock()->BlowingAwayAttackOnHit(m_mWorld.Backward());
					KdAudioManager::Instance().Play("Asset/Audio/SE/KickAttackHit.wav");
					hitPos.y += 0.35f;
					KdEffekseerManager::GetInstance().
						Play("Hit3.efk", hitPos);
					KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("Hit3.efk"); // これでループしない
					//KdEffekseerManager::GetInstance().SetRotation("Hit3.efk", m_mWorld.Backward(), DirectX::XMConvertToRadians(0));
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

					enemyList.lock()->Intersects
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
						enemyList.lock()->BlowingAwayAttackOnHit(m_mWorld.Backward());
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
}

void Player::PlayerPanchiHitAttackChaeck()
{
	const KdModelWork::Node* node = nullptr;
	Math::Matrix mat = Math::Matrix::Identity;

	for (auto& enemyList : m_enemyList)
	{
		if (enemyList.expired())continue;

		if (!enemyList.lock()->GetAttackHit() && !enemyList.lock()->GetDefenseSuc() && enemyList.lock()->GetInvincibilityTimeCnt() == 0 && !enemyList.lock()->GetBEnemyDeath()) // ここにはなくてもいいかも
		{
			/*if (player->GetPlayerState() & Player::PlayerState::rAttack && m_arrmType == lArrm)return;
			if (player->GetPlayerState() & Player::PlayerState::lAttack && m_arrmType == rArrm)return;*/

			node = m_model->FindNode("ArrmAttackPoint");
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

			enemyList.lock()->Intersects
			(
				sphereInfo,
				&retSphereList
			);

			Math::Vector3 hitDir = {};
			bool hit = false;
			Math::Vector3 hitPos = {};
			for (auto& ret : retSphereList)
			{
				hit = true;
				hitDir = ret.m_hitDir;
				hitPos = ret.m_hitPos;
			}

			if (hit)
			{
				enemyList.lock()->BlowingAwayAttackOnHit(m_mWorld.Backward());
				KdAudioManager::Instance().Play("Asset/Audio/SE/KickAttackHit.wav");
				hitPos.y += 0.35f;
				KdEffekseerManager::GetInstance().
					Play("Hit3.efk", hitPos);
				KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("Hit3.efk"); // これでループしない
				//KdEffekseerManager::GetInstance().SetRotation("Hit3.efk", m_mWorld.Backward(), DirectX::XMConvertToRadians(0));
				Math::Matrix efcMat = Math::Matrix::CreateScale(0.5f) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)) * Math::Matrix::CreateTranslation(hitPos);
				KdEffekseerManager::GetInstance().SetWorldMatrix("Hit3.efk", efcMat);
			}
			else
			{
				node = m_model->FindNode("ArrmAttackHitPoint");
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

				enemyList.lock()->Intersects
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
					enemyList.lock()->BlowingAwayAttackOnHit(m_mWorld.Backward());
					KdAudioManager::Instance().Play("Asset/Audio/SE/KickAttackHit.wav");
					hitPos.y += 0.35f;
					KdEffekseerManager::GetInstance().
						Play("Hit3.efk", hitPos);
					KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("Hit3.efk"); // これでループしない
					//KdEffekseerManager::GetInstance().SetRotation("Hit3.efk", m_mWorld.Backward(), DirectX::XMConvertToRadians(0));
					Math::Matrix efcMat = Math::Matrix::CreateScale(0.5f) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)) * Math::Matrix::CreateTranslation(hitPos);
					KdEffekseerManager::GetInstance().SetWorldMatrix("Hit3.efk", efcMat);

				}
				else
				{
					node = m_model->FindNode("ArrmAttackHitPointTwo");
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

					enemyList.lock()->Intersects
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
						enemyList.lock()->BlowingAwayAttackOnHit(m_mWorld.Backward());
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
}

void Player::PostUpdate()
{
	if (!m_bPlayerDeath)
	{
		auto it = m_enemyList.begin();
		while (it != m_enemyList.end()) // 数が変動するため範囲ベースForが使えない
		{
			// 不要になったオブジェクトを消す
			if ((*it).expired())
			{
				// 消す
				it = m_enemyList.erase(it); // 戻り値で次の場所を返してくれる
			}
			else
			{
				++it; // 次へ
			}
		}

		if (m_gravity > 0)
		{
			if (!(m_playerState & hit))
			{
				if (!(m_playerState & fall))
				{
					m_animator->SetAnimation(m_model->GetAnimation("FallA"), false);
				}

				m_playerState = fall;
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

		if (m_bAttackAnimeDelay)
		{
			m_attackAnimeDelayCnt--;
			if (m_attackAnimeDelayCnt <= 0)
			{
				m_bAttackAnimeDelay = false;
				m_attackAnimeDelayCnt = 0;
			}
		}

		if (!m_animator) return;

		if (!(m_playerState & (lAttack | rAttack | rlAttack | rlAttackRush)))
		{
			m_animator->AdvanceTime(m_model->WorkNodes());
			if (m_playerState & run)
			{
				++m_runAnimeCnt;
				if (m_runAnimeCnt == 16)
				{
					KdAudioManager::Instance().Play("Asset/Audio/SE/FootstepsConcrete2.wav");
				}
				else if (m_runAnimeCnt == 31)
				{
					KdAudioManager::Instance().Play("Asset/Audio/SE/FootstepsConcrete2.wav");
				}

				if (m_runAnimeCnt == 40)
				{
					m_runAnimeCnt = 0;
				}
			}
		}
		else if (m_playerState & (lAttack | rAttack) && !m_bAttackAnimeDelay)
		{
			if (m_bAttackAnimeCnt)
			{
				m_attackAnimeCnt++;
				if (m_playerState & (lAttackOne | lAttackTwo | rAttackOne | rAttackTwo))
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
				if (m_playerState & (lAttackThree | rAttackThree))
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
		else if (m_playerState & (rlAttack | rlAttackRush))
		{
			if (m_bAttackAnimeCnt)
			{
				m_attackAnimeCnt++;
				if (m_playerState & rlAttackOne)
				{
					if (m_attackAnimeCnt == 13 || m_attackAnimeCnt == 17)
					{
						KdAudioManager::Instance().Play("Asset/Audio/SE/Swishes - SWSH 40, Swish, Combat, Weapon, Light.wav");
					}
				}
				else if (m_playerState & rlAttackTwo)
				{
					if (m_attackAnimeCnt == 13 || m_attackAnimeCnt == 16)
					{
						KdAudioManager::Instance().Play("Asset/Audio/SE/Swishes - SWSH 40, Swish, Combat, Weapon, Light.wav");
					}
				}
				else if (m_playerState & rlAttackThree)
				{
					if (m_attackAnimeCnt == 15 || m_attackAnimeCnt == 23)
					{
						KdAudioManager::Instance().Play("Asset/Audio/SE/Swishes - SWSH 40, Swish, Combat, Weapon, Light.wav");
					}
				}
				else if (m_playerState & rlAttackRush)
				{
					if (!m_bRushRp)
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
					else if (m_attackAnimeCnt == 22 ||
						     m_attackAnimeCnt == 33 ||
						     m_attackAnimeCnt == 43 ||
						     m_attackAnimeCnt == 53 ||
						     m_attackAnimeCnt == 63 ||
						     m_attackAnimeCnt == 73 ||
						     m_attackAnimeCnt == 83 ||
						     m_attackAnimeCnt == 93
						     )
					{
						KdAudioManager::Instance().Play("Asset/Audio/SE/Swishes - SWSH 40, Swish, Combat, Weapon, Light.wav");
					}
				}


				if (m_playerState & rlAttackOne)
				{
					if (m_attackAnimeCnt == 20)
					{
						m_bAttackAnimeDelay = true;
						m_bAttackAnimeCnt = false;
						m_attackAnimeCnt = 0;
						m_attackAnimeDelayCnt = 10;
					}
				}
				else if (m_playerState & rlAttackTwo)
				{
					if (m_attackAnimeCnt == 20)
					{
						m_bAttackAnimeDelay = true;
						m_bAttackAnimeCnt = false;
						m_attackAnimeCnt = 0;
						m_attackAnimeDelayCnt = 10;
					}
				}
				else if (m_playerState & rlAttackThree)
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

		for (auto& WeaList : m_weaponList)
		{
			WeaList->PostUpdate();
		}
	}
	else
	{
		m_animator->AdvanceTime(m_model->WorkNodes());
		m_model->CalcNodeMatrices();

		if (!KdEffekseerManager::GetInstance().IsPlaying("BailOutPlayer.efk"))
		{
			if (m_bPlayerLose)
			{
				SceneManager::Instance().SetBAddOrSubVal(false);
				if (SceneManager::Instance().GetSceneType() == SceneManager::SceneType::challenge)
				{
					SceneManager::Instance().SetPointAddOrSubVal(0);
				}
				else
				{
					SceneManager::Instance().SetPointAddOrSubVal(500);
				}

				SceneManager::Instance().SetNextScene(SceneManager::SceneType::result);
			}
		}

		if (m_animator->IsAnimationEnd())
		{
			if (!m_bPlayerLose)
			{
				KdEffekseerManager::GetInstance().
					Play("BailOutPlayer.efk", { m_pos.x,m_pos.y + 0.3f,m_pos.z });
				KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("BailOutPlayer.efk"); // これでループしない
				m_bPlayerLose = true;
			}

		}
	}
}

void Player::OnHit(Math::Vector3 a_KnocBackvec)
{
	if (m_bRushRp)
	{
		m_bRushRp = false;
	}

	if (m_playerState & mantis)
	{
		const std::shared_ptr<Scopion> scopion = std::dynamic_pointer_cast<Scopion>(m_weaponList[1]);
		scopion->SetBMantis(false);
		const std::shared_ptr<Scopion> scopion2 = std::dynamic_pointer_cast<Scopion>(m_weaponList[0]);
		scopion2->SetBMantis(false);
	}

	m_playerState = nomalHit;
	for (auto& enemyList : m_enemyList)
	{
		if (enemyList.expired())continue;
		if (enemyList.lock()->GetEnemyState() & eRlAttackThree)
		{
			m_hitStopCnt = 60;
		}
		else
		{
			m_hitStopCnt = 40;
		}
	}

	m_hitMoveSpd = 0.05f;
	m_hitColorChangeTimeCnt = 15;
	m_knockBackVec = a_KnocBackvec;
	m_endurance -= 15.0f;
	m_attackHit = true;

	for (auto& enemyList : m_enemyList)
	{
		if (enemyList.expired())continue;

		if (enemyList.lock()->GetEnemyState() & (eRAttackOne | eRlAttackOne |
			eRlAttackThree))
		{
			m_animator->SetAnimation(m_model->GetAnimation("RHit1"), false);
			if (SceneManager::Instance().GetUpdateStopCnt() == 0)
			{
				SceneManager::Instance().SetUpdateStopCnt(5); // これでアップデートを一時止める
			}
		}
		else if (enemyList.lock()->GetEnemyState() & (eRAttackTwo | eRlAttackTwo))
		{
			m_animator->SetAnimation(m_model->GetAnimation("RHit2"), false);
			if (SceneManager::Instance().GetUpdateStopCnt() == 0)
			{
				SceneManager::Instance().SetUpdateStopCnt(5); // これでアップデートを一時止める
			}
		}

		if (enemyList.lock()->GetEnemyState() & eRlAttackRush && enemyList.lock()->GetAnimationCnt() < 8 ||
			(enemyList.lock()->GetAnimationCnt() >= 21 && enemyList.lock()->GetAnimationCnt() < 41))
		{
			m_animator->SetAnimation(m_model->GetAnimation("RHit1"), false);

			if (SceneManager::Instance().GetUpdateStopCnt() == 0)
			{
				SceneManager::Instance().SetUpdateStopCnt(2); // これでアップデートを一時止める
			}
		}
		else if (enemyList.lock()->GetEnemyState() & eRlAttackRush && (enemyList.lock()->GetAnimationCnt() >= 8 && enemyList.lock()->GetAnimationCnt() < 21))
		{
			m_animator->SetAnimation(m_model->GetAnimation("RHit2"), false);

			if (SceneManager::Instance().GetUpdateStopCnt() == 0)
			{
				SceneManager::Instance().SetUpdateStopCnt(2); // これでアップデートを一時止める
			}
		}
	}

	if (m_endurance < 0)
	{
		m_endurance = 0;
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

void Player::BlowingAwayAttackOnHit(Math::Vector3 a_KnocBackvec)
{
	if (m_bRushRp)
	{
		m_bRushRp = false;
	}

	if (m_playerState & mantis)
	{
		const std::shared_ptr<Scopion> scopion = std::dynamic_pointer_cast<Scopion>(m_weaponList[1]);
		scopion->SetBMantis(false);
		const std::shared_ptr<Scopion> scopion2 = std::dynamic_pointer_cast<Scopion>(m_weaponList[0]);
		scopion2->SetBMantis(false);
	}

	int cnt = 0;
	for (auto& enemyList : m_enemyList)
	{
		if (enemyList.expired())continue;

		if (enemyList.lock()->GetEnemyState() & eRlAttackRush && enemyList.lock()->GetAnimationCnt() >= 107)
		{
			m_hitMoveSpd = 0.95f;
			cnt++;
			if (SceneManager::Instance().GetUpdateStopCnt() == 0)
			{
				SceneManager::Instance().SetUpdateStopCnt(15); // これでアップデートを一時止める
			}
			break;
		}
		else if (enemyList.lock()->GetEnemyState() & (eRAttackOne | eLAttackOne) && enemyList.lock()->GetEnemyState() & (eGrassHopperDashF))
		{
			m_hitMoveSpd = 0.35f;
			cnt++;
			if (SceneManager::Instance().GetUpdateStopCnt() == 0)
			{
				SceneManager::Instance().SetUpdateStopCnt(15); // これでアップデートを一時止める
			}
		}
		else
		{
			if (SceneManager::Instance().GetUpdateStopCnt() == 0)
			{
				SceneManager::Instance().SetUpdateStopCnt(8); // これでアップデートを一時止める
			}
		}
	}

	if (cnt == 0)
	{
		m_hitMoveSpd = 0.3f;
	}

	m_playerState = blowingAwayHit;
	m_hitStopCnt = 40;
	m_hitColorChangeTimeCnt = 15;
	m_knockBackVec = a_KnocBackvec;
	m_endurance -= 30.0f;
	m_attackHit = true;
	m_animator->SetAnimation(m_model->GetAnimation(" BlowingAwayHitB"), false);
	m_invincibilityTimeCnt = 100;
	if (m_endurance < 0)
	{
		m_endurance = 0;
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

void Player::IaiKiriAttackOnHit(Math::Vector3 a_KnocBackvec)
{
	if (m_bRushRp)
	{
		m_bRushRp = false;
	}

	if (m_playerState & mantis)
	{
		const std::shared_ptr<Scopion> scopion = std::dynamic_pointer_cast<Scopion>(m_weaponList[1]);
		scopion->SetBMantis(false);
		const std::shared_ptr<Scopion> scopion2 = std::dynamic_pointer_cast<Scopion>(m_weaponList[0]);
		scopion2->SetBMantis(false);
	}

	m_playerState = iaiKiriHit;
	m_hitStopCnt = 40;
	m_hitColorChangeTimeCnt = 15;
	m_hitMoveSpd = 0.0f;
	m_knockBackVec = a_KnocBackvec;
	m_endurance -= 50.0f;
	m_attackHit = true;
	m_animator->SetAnimation(m_model->GetAnimation("IaiKiriAttackHitB"), false);
	m_invincibilityTimeCnt = 100;
	if (SceneManager::Instance().GetUpdateStopCnt() == 0)
	{
		SceneManager::Instance().SetUpdateStopCnt(8); // これでアップデートを一時止める
	}

	if (m_endurance < 0)
	{
		m_endurance = 0;
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

void Player::CutRaiseOnHit(Math::Vector3 a_KnocBackvec)
{
	m_playerState = cutRaiseHit;
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
	m_attackHit = true;
	m_animator->SetAnimation(m_model->GetAnimation("CutRaiseHit"), false);
	if (SceneManager::Instance().GetUpdateStopCnt() == 0)
	{
		SceneManager::Instance().SetUpdateStopCnt(8); // これでアップデートを一時止める
	}

	if (m_endurance <= 0)
	{
		m_endurance = 0;
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

void Player::HasDefense()
{
	if (m_playerState & (rAttack | rlAttack))
	{
		m_animator->SetAnimation(m_model->GetAnimation("RHasDefense"), false);
	}
	else if (m_playerState & lAttack)
	{
		m_animator->SetAnimation(m_model->GetAnimation("LHasDefense"), false);
	}

	if (m_bRushRp)
	{
		m_bRushRp = false;
	}

	m_playerState = hasDefense;
}

void Player::DrawSprite()
{

}

void Player::GenerateDepthMapFromLight_SkinMesh()
{
	if (m_bPlayerLose)return;
	if (!m_model) return;

	KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_model, m_mWorld);
}

void Player::DrawLit_SkinMesh()
{
	if (m_bPlayerLose)return;
	if (!m_model) return;

	if (m_invincibilityTimeCnt <= 90 && m_invincibilityTimeCnt > 80 ||
		m_invincibilityTimeCnt <= 70 && m_invincibilityTimeCnt > 60 ||
		m_invincibilityTimeCnt <= 50 && m_invincibilityTimeCnt > 40 ||
		m_invincibilityTimeCnt <= 30 && m_invincibilityTimeCnt > 20 ||
		m_invincibilityTimeCnt <= 15 && m_invincibilityTimeCnt > 10 ||
		m_invincibilityTimeCnt <= 5  && m_invincibilityTimeCnt >  3 ||
		m_invincibilityTimeCnt == 1
		)return;

	KdShaderManager::Instance().m_HD2DShader.SetOutLineColor({ 0,0,1 });
	if (m_hitColorChangeTimeCnt == 0)
	{
		KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_model, m_mWorld,true);
	}
	else
	{
		Math::Color color = { 1,0,0,1 };
		KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_model, m_mWorld, true,color);
	}
}

void Player::DrawDebug()
{
#ifdef _DEBUG
	if (!m_pDebugWire)return;
	m_pDebugWire->Draw();

	for (auto& WeaList : m_weaponList)
	{
		WeaList->DrawDebug();
	}
#endif
}


void Player::DrawLit()
{
	if (m_bPlayerLose)return;
	if (!m_bEwaponDrawPosible)return;

	for (auto& WeaList : m_weaponList)
	{
		WeaList->DrawLit();
	}
}

void Player::DrawUnLit()
{
	if (m_bPlayerLose)return;
	if (m_enemy.expired())return;
	if (m_enemy.lock()->GetBEnemyDeath())return;
	KdShaderManager::Instance().m_HD2DShader.DrawPolygon(*m_rocKOnPolygon, m_rockOnPolyMat);
}

void Player::DrawBright()
{
	if (m_bPlayerLose)return;
	if (!m_bEwaponDrawPosible)return;

	for (auto& WeaList : m_weaponList)
	{
		WeaList->DrawBright();
	}
}

void Player::GenerateDepthMapFromLight()
{
	if (!m_bEwaponDrawPosible)return;
	if (m_bPlayerLose)return;

	for (auto& WeaList : m_weaponList)
	{
		WeaList->GenerateDepthMapFromLight();
	}
}

void Player::UpdateRotate(Math::Vector3& a_srcMoveVec)
{
	if (a_srcMoveVec.Length() == 0)return;

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
	m_mWorldRot.y += rotateAng;
}

void Player::GrassMoveVecDecision()
{
	std::shared_ptr<CameraBase> spCamera = m_wpCamera.lock();
	if (!spCamera)return;
	Math::Vector3 CameraVec = spCamera->GetMatrix().Forward();
	CameraVec.y = 0;
	CameraVec.Normalize();

	const KdModelWork::Node* node = nullptr;
	Math::Matrix mat = Math::Matrix::Identity;

	//std::shared_ptr<GameCamera> spCamera = std::dynamic_pointer_cast<GameCamera>(m_wpCamera.lock());
	if (m_rGrassHopperPauCnt == 0)
	{
		if (m_weaponType & grassHopper)
		{
			Math::Vector3 vec = m_mWorld.Backward();
			if (KdInputManager::Instance().GetButtonState("rAttack"))
			{
				m_playerState &= ~lAttack;
				m_playerState &= ~rAttack;
				if (KdInputManager::Instance().GetButtonState("forward"))
				{
					m_grassHopperDashDir = Math::Vector3::TransformNormal(Math::Vector3(0, 0, 1), m_wpCamera.lock()->GetRotationYMatrix());
					m_bMove = true;
					m_playerState &= ~grassHopperDash;
					m_playerState &= ~run;
					m_playerState |= grassHopperDashF;
					m_playerState &= ~grassHopperDashUp;
					m_grasRotYMat = m_wpCamera.lock()->GetRotationYMatrix();
					m_rGrassHopperPauCnt = GRASSHOPPERPAUCNT;
					m_rGrassHopperTime = 90;
					m_lGrassHopperTime = 0;
					m_gravity = 0;
					m_torion -= TORIONDOWNVALUE;
					m_animator->SetAnimation(m_model->GetAnimation("GrassDashFB"), false);

					node = m_model->FindNode("GrassHopperLegRPoint");
					mat = node->m_worldTransform * m_mWorld;
					mat._42 += 0.7f;
					m_weaponList[2]->GrassHopper({ mat._41,mat._42,mat._43 }, spCamera->GetYAng());

					KdEffekseerManager::GetInstance().
						Play("GrassDashBlurPlayer.efk", {m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z});
					KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("GrassDashBlurPlayer.efk"); // これでループしない
					//KdEffekseerManager::GetInstance().SetRotation("Hit3.efk", m_mWorld.Backward(), DirectX::XMConvertToRadians(0));
					Math::Matrix efcMat = Math::Matrix::CreateScale(1)* Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y))* Math::Matrix::CreateTranslation({ m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z });
					KdEffekseerManager::GetInstance().SetWorldMatrix("GrassDashBlurPlayer.efk", efcMat);
				}
				else if (KdInputManager::Instance().GetButtonState("left"))
				{
					m_grassHopperDashDir = Math::Vector3::TransformNormal(Math::Vector3(-1, 0, 0), m_wpCamera.lock()->GetRotationYMatrix());
					m_bMove = true;
					m_playerState &= ~grassHopperDash;
					m_playerState &= ~run;
					m_playerState |= grassHopperDashL;
					m_playerState &= ~grassHopperDashUp;
					m_grasRotYMat = m_wpCamera.lock()->GetRotationYMatrix();
					m_rGrassHopperPauCnt = GRASSHOPPERPAUCNT;
					m_rGrassHopperTime = 90;
					m_lGrassHopperTime = 0;
					m_gravity = 0;
					m_torion -= TORIONDOWNVALUE;
					m_animator->SetAnimation(m_model->GetAnimation("GrassDashLB"), false);

					node = m_model->FindNode("GrassHopperLegRPoint");
					mat = node->m_worldTransform * m_mWorld;
					mat._42 += 0.7f;
					m_weaponList[2]->GrassHopper({ mat._41,mat._42,mat._43 }, spCamera->GetYAng());

					vec = m_mWorld.Left();
					KdEffekseerManager::GetInstance().
						Play("GrassDashBlurPlayer.efk", { m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z });
					KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("GrassDashBlurPlayer.efk"); // これでループしない
					//KdEffekseerManager::GetInstance().SetRotation("Hit3.efk", m_mWorld.Backward(), DirectX::XMConvertToRadians(0));
					Math::Matrix efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y + 90.0f)) * Math::Matrix::CreateTranslation({ m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z });
					KdEffekseerManager::GetInstance().SetWorldMatrix("GrassDashBlurPlayer.efk", efcMat);
				}

				else if (KdInputManager::Instance().GetButtonState("backward"))
				{
					m_grassHopperDashDir = Math::Vector3::TransformNormal(Math::Vector3(0, 0, -1), m_wpCamera.lock()->GetRotationYMatrix());
					m_bMove = true;
					m_playerState &= ~grassHopperDash;
					m_playerState &= ~run;
					m_playerState |= grassHopperDashB;
					m_playerState &= ~grassHopperDashUp;
					m_grasRotYMat = m_wpCamera.lock()->GetRotationYMatrix();
					m_rGrassHopperPauCnt = GRASSHOPPERPAUCNT;
					m_rGrassHopperTime = 90;
					m_lGrassHopperTime = 0;
					m_gravity = 0;
					m_torion -= TORIONDOWNVALUE;
					m_animator->SetAnimation(m_model->GetAnimation("GrassDashBB"), false);

					node = m_model->FindNode("GrassHopperLegRPoint");
					mat = node->m_worldTransform * m_mWorld;
					mat._42 += 0.7f;
					m_weaponList[2]->GrassHopper({ mat._41,mat._42,mat._43 }, spCamera->GetYAng());

					vec = m_mWorld.Forward();
					KdEffekseerManager::GetInstance().
						Play("GrassDashBlurPlayer.efk", { m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z });
					KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("GrassDashBlurPlayer.efk"); // これでループしない
					//KdEffekseerManager::GetInstance().SetRotation("Hit3.efk", m_mWorld.Backward(), DirectX::XMConvertToRadians(0));
					Math::Matrix efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y + 180.0f)) * Math::Matrix::CreateTranslation({ m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z });
					KdEffekseerManager::GetInstance().SetWorldMatrix("GrassDashBlurPlayer.efk", efcMat);
				}
				else if (KdInputManager::Instance().GetButtonState("right"))
				{
					m_grassHopperDashDir = Math::Vector3::TransformNormal(Math::Vector3(1, 0, 0), m_wpCamera.lock()->GetRotationYMatrix());
					m_bMove = true;
					m_playerState &= ~grassHopperDash;
					m_playerState &= ~run;
					m_playerState |= grassHopperDashR;
					m_playerState &= ~grassHopperDashUp;
					m_grasRotYMat = m_wpCamera.lock()->GetRotationYMatrix();
					m_rGrassHopperPauCnt = GRASSHOPPERPAUCNT;
					m_rGrassHopperTime = 90;
					m_lGrassHopperTime = 0;
					m_gravity = 0;
					m_torion -= TORIONDOWNVALUE;
					m_animator->SetAnimation(m_model->GetAnimation("GrassDashRB"), false);

					node = m_model->FindNode("GrassHopperLegLPoint");
					mat = node->m_worldTransform * m_mWorld;
					mat._42 += 0.7f;
					m_weaponList[2]->GrassHopper({ mat._41,mat._42,mat._43 }, spCamera->GetYAng());

					vec = m_mWorld.Right();
					KdEffekseerManager::GetInstance().
						Play("GrassDashBlurPlayer.efk", { m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z });
					KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("GrassDashBlurPlayer.efk"); // これでループしない
					//KdEffekseerManager::GetInstance().SetRotation("Hit3.efk", m_mWorld.Backward(), DirectX::XMConvertToRadians(0));
					Math::Matrix efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y + 270.0f)) * Math::Matrix::CreateTranslation({ m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z });
					KdEffekseerManager::GetInstance().SetWorldMatrix("GrassDashBlurPlayer.efk", efcMat);
				}
				else if (KdInputManager::Instance().GetButtonState("jump"))
				{
					m_grassHopperDashDir = { 0, 1, 0 };
					m_grassHopperDashDir.Normalize();
					m_bMove = true;
					m_playerState |= grassHopperDashUp;
					m_playerState &= ~grassHopperDash;
					m_playerState &= ~run;
					m_grasRotYMat = m_wpCamera.lock()->GetRotationYMatrix();
					m_rGrassHopperPauCnt = GRASSHOPPERPAUCNT;
					m_rGrassHopperTime = 90;
					m_lGrassHopperTime = 0;
					m_gravity = 0;
					m_torion -= TORIONDOWNVALUE;
					m_animator->SetAnimation(m_model->GetAnimation("GrassDashJB"), false);

					node = m_model->FindNode("GrassHopperLegRPoint");
					mat = node->m_worldTransform * m_mWorld;
					mat._42 += 0.7f;
					m_weaponList[2]->GrassHopper({ mat._41,mat._42,mat._43 }, spCamera->GetYAng());

					KdEffekseerManager::GetInstance().
						Play("GrassDashBlurPlayer.efk", { m_pos.x,m_pos.y + 2.9f,m_pos.z});
					KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("GrassDashBlurPlayer.efk"); // これでループしない
					//KdEffekseerManager::GetInstance().SetRotation("Hit3.efk", m_mWorld.Backward(), DirectX::XMConvertToRadians(0));
					Math::Matrix efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(270.0f)) * Math::Matrix::CreateTranslation({ m_pos.x,m_pos.y + 2.9f,m_pos.z });
					KdEffekseerManager::GetInstance().SetWorldMatrix("GrassDashBlurPlayer.efk", efcMat);
				}
				else
				{
					m_grassHopperDashDir = { 0, 1, 0 };
					m_grassHopperDashDir.Normalize();
					m_bMove = true;
					m_playerState |= grassHopperDashUp;
					m_playerState &= ~grassHopperDash;
					m_playerState &= ~run;
					m_grasRotYMat = m_wpCamera.lock()->GetRotationYMatrix();
					m_rGrassHopperPauCnt = GRASSHOPPERPAUCNT;
					m_rGrassHopperTime = 90;
					m_lGrassHopperTime = 0;
					m_gravity = 0;
					m_torion -= TORIONDOWNVALUE;
					m_animator->SetAnimation(m_model->GetAnimation("GrassDashJB"), false);

					node = m_model->FindNode("GrassHopperLegRPoint");
					mat = node->m_worldTransform * m_mWorld;
					mat._42 += 0.7f;
					m_weaponList[2]->GrassHopper({ mat._41,mat._42,mat._43 }, spCamera->GetYAng());

					KdEffekseerManager::GetInstance().
						Play("GrassDashBlurPlayer.efk", { m_pos.x,m_pos.y + 2.9f,m_pos.z });
					KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("GrassDashBlurPlayer.efk"); // これでループしない
					//KdEffekseerManager::GetInstance().SetRotation("Hit3.efk", m_mWorld.Backward(), DirectX::XMConvertToRadians(0));
					Math::Matrix efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(270.0f)) * Math::Matrix::CreateTranslation({ m_pos.x,m_pos.y + 2.9f,m_pos.z });
					KdEffekseerManager::GetInstance().SetWorldMatrix("GrassDashBlurPlayer.efk", efcMat);

					//KdEffekseerManager::GetInstance().
					//	Play("GrassUp.efk", { mat._41,mat._42,mat._43 });
					//KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("GrassUp.efk"); // これでループしない
					////KdEffekseerManager::GetInstance().SetRotation("Hit3.efk", m_mWorld.Backward(), DirectX::XMConvertToRadians(0));
					//efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(270.0f)) * Math::Matrix::CreateTranslation({ mat._41,mat._42,mat._43 });
					//KdEffekseerManager::GetInstance().SetWorldMatrix("GrassUp.efk", efcMat);
				}
			}
		}
	}

	if (m_lGrassHopperPauCnt == 0)
	{
		if (m_weaponType & lGrassHopper)
		{
			Math::Vector3 vec = m_mWorld.Backward();
			if (KdInputManager::Instance().GetButtonState("lAttack"))
			{
				m_playerState &= ~lAttack;
				m_playerState &= ~rAttack;

				if (KdInputManager::Instance().GetButtonState("forward"))
				{
					m_grassHopperDashDir = Math::Vector3::TransformNormal(Math::Vector3(0, 0, 1), m_wpCamera.lock()->GetRotationYMatrix());
					m_bMove = true;
					m_playerState &= ~grassHopperDash;
					m_playerState &= ~run;
					m_playerState |= grassHopperDashF;
					m_playerState &= ~grassHopperDashUp;
					m_grasRotYMat = m_wpCamera.lock()->GetRotationYMatrix();
					m_lGrassHopperPauCnt = GRASSHOPPERPAUCNT;
					m_lGrassHopperTime = 90;
					m_rGrassHopperTime = 0;
					m_gravity = 0;
					m_torion -= TORIONDOWNVALUE;
					m_animator->SetAnimation(m_model->GetAnimation("GrassDashFB"), false);

					node = m_model->FindNode("GrassHopperLegRPoint");
					mat = node->m_worldTransform * m_mWorld;
					mat._42 += 0.7f;
					m_weaponList[3]->GrassHopper({ mat._41,mat._42,mat._43 }, spCamera->GetYAng());

					KdEffekseerManager::GetInstance().
						Play("GrassDashBlurPlayer.efk", { m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z });
					KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("GrassDashBlurPlayer.efk"); // これでループしない
					//KdEffekseerManager::GetInstance().SetRotation("Hit3.efk", m_mWorld.Backward(), DirectX::XMConvertToRadians(0));
					Math::Matrix efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)) * Math::Matrix::CreateTranslation({ m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z });
					KdEffekseerManager::GetInstance().SetWorldMatrix("GrassDashBlurPlayer.efk", efcMat);
				}
				else if (KdInputManager::Instance().GetButtonState("left"))
				{
					m_grassHopperDashDir = Math::Vector3::TransformNormal(Math::Vector3(-1, 0, 0), m_wpCamera.lock()->GetRotationYMatrix());
					m_bMove = true;
					m_playerState &= ~grassHopperDash;
					m_playerState &= ~run;
					m_playerState |= grassHopperDashL;
					m_playerState &= ~grassHopperDashUp;
					m_grasRotYMat = m_wpCamera.lock()->GetRotationYMatrix();
					m_lGrassHopperPauCnt = GRASSHOPPERPAUCNT;
					m_lGrassHopperTime = 90;
					m_rGrassHopperTime = 0;
					m_gravity = 0;
					m_torion -= TORIONDOWNVALUE;
					m_animator->SetAnimation(m_model->GetAnimation("GrassDashLB"), false);

					node = m_model->FindNode("GrassHopperLegRPoint");
					mat = node->m_worldTransform * m_mWorld;
					mat._42 += 0.7f;
					m_weaponList[3]->GrassHopper({ mat._41,mat._42,mat._43 }, spCamera->GetYAng());

					vec = m_mWorld.Left();
					KdEffekseerManager::GetInstance().
						Play("GrassDashBlurPlayer.efk", { m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z });
					KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("GrassDashBlurPlayer.efk"); // これでループしない
					//KdEffekseerManager::GetInstance().SetRotation("Hit3.efk", m_mWorld.Backward(), DirectX::XMConvertToRadians(0));
					Math::Matrix efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y + 90.0f)) * Math::Matrix::CreateTranslation({ m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z });
					KdEffekseerManager::GetInstance().SetWorldMatrix("GrassDashBlurPlayer.efk", efcMat);
				}

				else if (KdInputManager::Instance().GetButtonState("backward"))
				{
					m_grassHopperDashDir = Math::Vector3::TransformNormal(Math::Vector3(0, 0, -1), m_wpCamera.lock()->GetRotationYMatrix());
					m_bMove = true;
					m_playerState &= ~grassHopperDash;
					m_playerState &= ~run;
					m_playerState |= grassHopperDashB;
					m_playerState &= ~grassHopperDashUp;
					m_grasRotYMat = m_wpCamera.lock()->GetRotationYMatrix();
					m_lGrassHopperPauCnt = GRASSHOPPERPAUCNT;
					m_lGrassHopperTime = 90;
					m_rGrassHopperTime = 0;
					m_gravity = 0;
					m_torion -= TORIONDOWNVALUE;
					m_animator->SetAnimation(m_model->GetAnimation("GrassDashBB"), false);

					node = m_model->FindNode("GrassHopperLegRPoint");
					mat = node->m_worldTransform * m_mWorld;
					mat._42 += 0.7f;
					m_weaponList[3]->GrassHopper({ mat._41,mat._42,mat._43 }, spCamera->GetYAng());

					vec = m_mWorld.Forward();
					KdEffekseerManager::GetInstance().
						Play("GrassDashBlurPlayer.efk", { m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z });
					KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("GrassDashBlurPlayer.efk"); // これでループしない
					//KdEffekseerManager::GetInstance().SetRotation("Hit3.efk", m_mWorld.Backward(), DirectX::XMConvertToRadians(0));
					Math::Matrix efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y + 180.0f)) * Math::Matrix::CreateTranslation({ m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z });
					KdEffekseerManager::GetInstance().SetWorldMatrix("GrassDashBlurPlayer.efk", efcMat);
				}
				else if (KdInputManager::Instance().GetButtonState("right"))
				{
					m_grassHopperDashDir = Math::Vector3::TransformNormal(Math::Vector3(1, 0, 0), m_wpCamera.lock()->GetRotationYMatrix());
					m_bMove = true;
					m_playerState &= ~grassHopperDash;
					m_playerState &= ~run;
					m_playerState |= grassHopperDashR;
					m_playerState &= ~grassHopperDashUp;
					m_grasRotYMat = m_wpCamera.lock()->GetRotationYMatrix();
					m_lGrassHopperPauCnt = GRASSHOPPERPAUCNT;
					m_lGrassHopperTime = 90;
					m_rGrassHopperTime = 0;
					m_gravity = 0;
					m_torion -= TORIONDOWNVALUE;
					m_animator->SetAnimation(m_model->GetAnimation("GrassDashRB"), false);

					node = m_model->FindNode("GrassHopperLegLPoint");
					mat = node->m_worldTransform * m_mWorld;
					mat._42 += 0.7f;
					m_weaponList[3]->GrassHopper({ mat._41,mat._42,mat._43 }, spCamera->GetYAng());

					vec = m_mWorld.Right();
					KdEffekseerManager::GetInstance().
						Play("GrassDashBlurPlayer.efk", { m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z });
					KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("GrassDashBlurPlayer.efk"); // これでループしない
					//KdEffekseerManager::GetInstance().SetRotation("Hit3.efk", m_mWorld.Backward(), DirectX::XMConvertToRadians(0));
					Math::Matrix efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y + 270.0f)) * Math::Matrix::CreateTranslation({ m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z });
					KdEffekseerManager::GetInstance().SetWorldMatrix("GrassDashBlurPlayer.efk", efcMat);
				}
				else if (KdInputManager::Instance().GetButtonState("jump"))
				{
					m_grassHopperDashDir = { 0, 1, 0 };
					m_grassHopperDashDir.Normalize();
					m_bMove = true;
					m_playerState &= ~grassHopperDash;
					m_playerState &= ~run;
					m_playerState |= grassHopperDashUp;
					m_grasRotYMat = m_wpCamera.lock()->GetRotationYMatrix();
					m_lGrassHopperPauCnt = GRASSHOPPERPAUCNT;
					m_lGrassHopperTime = 90;
					m_rGrassHopperTime = 0;
					m_gravity = 0;
					m_torion -= TORIONDOWNVALUE;
					m_animator->SetAnimation(m_model->GetAnimation("GrassDashJB"), false);

					node = m_model->FindNode("GrassHopperLegRPoint");
					mat = node->m_worldTransform * m_mWorld;
					mat._42 += 0.7f;
					m_weaponList[3]->GrassHopper({ mat._41,mat._42,mat._43 }, spCamera->GetYAng());

					KdEffekseerManager::GetInstance().
						Play("GrassDashBlurPlayer.efk", { m_pos.x,m_pos.y + 2.9f,m_pos.z });
					KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("GrassDashBlurPlayer.efk"); // これでループしない
					//KdEffekseerManager::GetInstance().SetRotation("Hit3.efk", m_mWorld.Backward(), DirectX::XMConvertToRadians(0));
					Math::Matrix efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(270.0f)) * Math::Matrix::CreateTranslation({ m_pos.x,m_pos.y + 2.9f,m_pos.z });
					KdEffekseerManager::GetInstance().SetWorldMatrix("GrassDashBlurPlayer.efk", efcMat);
				}
				else
				{
					m_grassHopperDashDir = { 0, 1, 0 };
					m_grassHopperDashDir.Normalize();
					m_bMove = true;
					m_playerState &= ~grassHopperDash;
					m_playerState &= ~run;
					m_playerState |= grassHopperDashUp;
					m_grasRotYMat = m_wpCamera.lock()->GetRotationYMatrix();
					m_lGrassHopperPauCnt = GRASSHOPPERPAUCNT;
					m_lGrassHopperTime = 90;
					m_rGrassHopperTime = 0;
					m_gravity = 0;
					m_torion -= TORIONDOWNVALUE;
					m_animator->SetAnimation(m_model->GetAnimation("GrassDashJB"), false);

					node = m_model->FindNode("GrassHopperLegRPoint");
					mat = node->m_worldTransform * m_mWorld;
					mat._42 += 0.7f;
					m_weaponList[3]->GrassHopper({ mat._41,mat._42,mat._43 }, spCamera->GetYAng());

					KdEffekseerManager::GetInstance().
						Play("GrassDashBlurPlayer.efk", { m_pos.x,m_pos.y + 2.9f,m_pos.z });
					KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("GrassDashBlurPlayer.efk"); // これでループしない
					//KdEffekseerManager::GetInstance().SetRotation("Hit3.efk", m_mWorld.Backward(), DirectX::XMConvertToRadians(0));
					Math::Matrix efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(270.0f)) * Math::Matrix::CreateTranslation({ m_pos.x,m_pos.y + 2.9f,m_pos.z });
					KdEffekseerManager::GetInstance().SetWorldMatrix("GrassDashBlurPlayer.efk", efcMat);
				}
			}
		}
	}
}

void Player::GrassMove()
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
		if (m_playerState & grassHopperDashF)
		{
			Math::Vector3 vec = m_mWorld.Backward();
			Math::Matrix efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)) * Math::Matrix::CreateTranslation({ m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z });
			KdEffekseerManager::GetInstance().SetWorldMatrix("GrassDashBlurPlayer.efk", efcMat);
		}
		else if (m_playerState & grassHopperDashB)
		{
			Math::Vector3 vec = m_mWorld.Forward();
			Math::Matrix efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y + 180.0f)) * Math::Matrix::CreateTranslation({ m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z });
			KdEffekseerManager::GetInstance().SetWorldMatrix("GrassDashBlurPlayer.efk", efcMat);
		}
		else if (m_playerState & grassHopperDashR)
		{
			Math::Vector3 vec = m_mWorld.Right();
			Math::Matrix efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y + 90.0f)) * Math::Matrix::CreateTranslation({ m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z });
			KdEffekseerManager::GetInstance().SetWorldMatrix("GrassDashBlurPlayer.efk", efcMat);
		}
		else if (m_playerState & grassHopperDashL)
		{
			Math::Vector3 vec = m_mWorld.Left();
			Math::Matrix efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y + 270.0f)) * Math::Matrix::CreateTranslation({ m_pos.x + 0.8f * vec.x,m_pos.y + 1.2f,m_pos.z + 0.8f * vec.z });
			KdEffekseerManager::GetInstance().SetWorldMatrix("GrassDashBlurPlayer.efk", efcMat);
		}
		else if (m_playerState & grassHopperDashUp)
		{
			Math::Matrix efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(270.0f)) * Math::Matrix::CreateTranslation({ m_pos.x,m_pos.y + 2.9f,m_pos.z });
			KdEffekseerManager::GetInstance().SetWorldMatrix("GrassDashBlurPlayer.efk", efcMat);
		}
		
		m_bMove = true;
		if (m_lGrassHopperTime <= 90 && m_lGrassHopperTime > 75 || m_rGrassHopperTime <= 90 && m_rGrassHopperTime > 75)
		{
			m_dashSpd = 0.0f;
		}
		else if (m_lGrassHopperTime <= 75 && m_lGrassHopperTime > 30 || m_rGrassHopperTime <= 75 && m_rGrassHopperTime > 30)
		{
			m_dashSpd = 0.8f;
		}
		else if (m_lGrassHopperTime <= 30 && m_lGrassHopperTime > 25 || m_rGrassHopperTime <= 30 && m_rGrassHopperTime > 25)
		{
			m_dashSpd = 0.5f;
		}
		else if (m_lGrassHopperTime <= 25 && m_lGrassHopperTime > 20 || m_rGrassHopperTime <= 25 && m_rGrassHopperTime > 20)
		{
			m_dashSpd = 0.40f;
		}
		else if (m_lGrassHopperTime <= 20 && m_lGrassHopperTime > 15 || m_rGrassHopperTime <= 20 && m_rGrassHopperTime > 15)
		{
			m_dashSpd = 0.35f;
		}
		else if (m_lGrassHopperTime <= 15 && m_lGrassHopperTime > 10 || m_rGrassHopperTime <= 15 && m_rGrassHopperTime > 10)
		{
			m_dashSpd = 0.30f;
		}
		else if (m_lGrassHopperTime <= 10 && m_lGrassHopperTime > 5 || m_rGrassHopperTime <= 10 && m_rGrassHopperTime > 5)
		{
			m_dashSpd = 0.25f;
		}
		else if (m_lGrassHopperTime <= 5 && m_lGrassHopperTime > 0 || m_rGrassHopperTime <= 5 && m_rGrassHopperTime > 0)
		{
			m_dashSpd = 0.20f;
		}

		if (m_lGrassHopperTime == 80)
		{
			m_weaponList[3]->StartAnime();
			if (m_playerState & grassHopperDashF)
			{
				m_animator->SetAnimation(m_model->GetAnimation("GrassDashFA"), true);
			}
			else if (m_playerState & grassHopperDashR)
			{
				m_animator->SetAnimation(m_model->GetAnimation("GrassDashRA"), true);
			}
			else if (m_playerState & grassHopperDashL)
			{
				m_animator->SetAnimation(m_model->GetAnimation("GrassDashLA"), true);
			}
			else if (m_playerState & grassHopperDashB)
			{
				m_animator->SetAnimation(m_model->GetAnimation("GrassDashBA"), true);
			}
			else if (m_playerState & grassHopperDashUp)
			{
				m_animator->SetAnimation(m_model->GetAnimation("GrassDashJA"), true);
			}
		}

		if (m_rGrassHopperTime == 80)
		{
			m_weaponList[2]->StartAnime();
			if (m_playerState & grassHopperDashF)
			{
				m_animator->SetAnimation(m_model->GetAnimation("GrassDashFA"), true);
			}
			else if (m_playerState & grassHopperDashR)
			{
				m_animator->SetAnimation(m_model->GetAnimation("GrassDashRA"), true);
			}
			else if (m_playerState & grassHopperDashL)
			{
				m_animator->SetAnimation(m_model->GetAnimation("GrassDashLA"), true);
			}
			else if (m_playerState & grassHopperDashB)
			{
				m_animator->SetAnimation(m_model->GetAnimation("GrassDashBA"), true);
			}
			else if (m_playerState & grassHopperDashUp)
			{
				m_animator->SetAnimation(m_model->GetAnimation("GrassDashJA"), true);
			}
		}
	}
	else
	{
		m_bMove = false;
		m_dashSpd = 0.0f;
		m_grassHopperDashDir = {};
		m_gravity = 0;
		m_playerState = idle;
	}

	m_pos += m_grassHopperDashDir * m_dashSpd;

	if (!m_wpCamera.expired())
	{
		Math::Vector3 vec = Math::Vector3::TransformNormal(Math::Vector3(0, 0, 1), m_grasRotYMat);
		UpdateRotate(vec);
	}

}

void Player::StepMove()
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
		/*else if (m_stepCnt <= 15 && m_stepCnt > 10)
		{
			m_dashSpd = 0.30f;
		}
		else if (m_stepCnt <= 10 && m_stepCnt > 5)
		{
			m_dashSpd = 0.25f;
		}
		else if (m_stepCnt <= 5 && m_stepCnt > 0)
		{
			m_dashSpd = 0.20f;
		}*/

		if (m_stepCnt == 60)
		{
			if (m_playerState & stepF)
			{
				m_animator->SetAnimation(m_model->GetAnimation("GrassDashFA"), true);
			}
			else if (m_playerState & stepR)
			{
				m_animator->SetAnimation(m_model->GetAnimation("GrassDashRA"), true);
			}
			else if (m_playerState & stepL)
			{
				m_animator->SetAnimation(m_model->GetAnimation("GrassDashLA"), true);
			}
			else if (m_playerState & stepB)
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

		/*if (!(m_playerState & idle))
		{
			m_animator->SetAnimation(m_model->GetAnimation("IdleA"));
		}
		m_playerState = idle;*/
	}

	m_pos += m_stepDashDir * m_dashSpd;

	if (!m_wpCamera.expired())
	{
		Math::Vector3 vec = Math::Vector3::TransformNormal(Math::Vector3(0, 0, 1), m_grasRotYMat);
		UpdateRotate(vec);
	}

}

void Player::NormalMove()
{
	Math::Vector3 moveVec = {};
	float moveSpd = 0.25f;

	if (!(m_playerState & (lAttack | rAttack | defense | mantis | rlAttack | rlAttackRush)))
	{
		if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		{
			if (KdInputManager::Instance().GetButtonState("forward"))
			{
				if (!(m_playerState & (jump | fall)))
				{
					m_stepDashDir = Math::Vector3::TransformNormal(Math::Vector3(0, 0, 1), m_wpCamera.lock()->GetRotationYMatrix());
					m_bMove = true;
					m_stepCnt = 60;
					m_grasRotYMat = m_wpCamera.lock()->GetRotationYMatrix();
					/*if (!(m_playerState & run))
					{
						m_animator->SetAnimation(m_model->GetAnimation("RUN"));
						m_runAnimeCnt = 0;
					}*/
					m_playerState = stepF;
					m_animator->SetAnimation(m_model->GetAnimation("GrassDashFB"), false);
				}
			}

			if (KdInputManager::Instance().GetButtonState("left"))
			{
				if (!(m_playerState & (jump | fall)))
				{
					m_stepDashDir = Math::Vector3::TransformNormal(Math::Vector3(-1, 0, 0), m_wpCamera.lock()->GetRotationYMatrix());
					m_bMove = true;
					m_stepCnt = 60;
					m_grasRotYMat = m_wpCamera.lock()->GetRotationYMatrix();
					/*if (!(m_playerState & run))
					{
						m_animator->SetAnimation(m_model->GetAnimation("RUN"));
						m_runAnimeCnt = 0;
					}*/
					m_playerState = stepL;
					m_animator->SetAnimation(m_model->GetAnimation("GrassDashLB"), false);
				}
			}

			if (KdInputManager::Instance().GetButtonState("backward"))
			{
				if (!(m_playerState & (jump | fall)))
				{
					m_stepDashDir = Math::Vector3::TransformNormal(Math::Vector3(0, 0, -1), m_wpCamera.lock()->GetRotationYMatrix());
					m_bMove = true;
					m_stepCnt = 60;
					m_grasRotYMat = m_wpCamera.lock()->GetRotationYMatrix();
					/*if (!(m_playerState & run))
					{
						m_animator->SetAnimation(m_model->GetAnimation("RUN"));
						m_runAnimeCnt = 0;
					}*/
					m_playerState = stepB;
					m_animator->SetAnimation(m_model->GetAnimation("GrassDashBB"), false);
				}
			}

			if (KdInputManager::Instance().GetButtonState("right"))
			{
				if (!(m_playerState & (jump | fall)))
				{
					m_stepDashDir = Math::Vector3::TransformNormal(Math::Vector3(1, 0, 0), m_wpCamera.lock()->GetRotationYMatrix());
					m_bMove = true;
					m_stepCnt = 60;
					m_grasRotYMat = m_wpCamera.lock()->GetRotationYMatrix();
					/*if (!(m_playerState & run))
					{
						m_animator->SetAnimation(m_model->GetAnimation("RUN"));
						m_runAnimeCnt = 0;
					}*/
					m_playerState = stepR;
					m_animator->SetAnimation(m_model->GetAnimation("GrassDashRB"), false);
				}
			}
		}
		else
		{
			if (KdInputManager::Instance().GetButtonState("forward"))
			{
				moveVec += {0, 0, 1};
				m_bMove = true;
				if (!(m_playerState & (jump | fall)))
				{
					if (!(m_playerState & run))
					{
						m_animator->SetAnimation(m_model->GetAnimation("RUN"));
						m_runAnimeCnt = 0;
					}
					else if(m_runAnimeCnt == 0)
					{
						m_animator->SetAnimation(m_model->GetAnimation("RUN"));
					}
					m_playerState = run;
				}
			}

			if (KdInputManager::Instance().GetButtonState("left"))
			{
				moveVec += {-1, 0, 0};
				m_bMove = true;
				if (!(m_playerState & (jump | fall)))
				{
					if (!(m_playerState & run))
					{
						m_animator->SetAnimation(m_model->GetAnimation("RUN"));
						m_runAnimeCnt = 0;
					}
					else if (m_runAnimeCnt == 0)
					{
						m_animator->SetAnimation(m_model->GetAnimation("RUN"));
					}
					m_playerState = run;
				}
			}

			if (KdInputManager::Instance().GetButtonState("backward"))
			{
				moveVec += {0, 0, -1};
				m_bMove = true;
				if (!(m_playerState & (jump | fall)))
				{
					if (!(m_playerState & run))
					{
						m_animator->SetAnimation(m_model->GetAnimation("RUN"));
						m_runAnimeCnt = 0;
					}
					else if (m_runAnimeCnt == 0)
					{
						m_animator->SetAnimation(m_model->GetAnimation("RUN"));
					}
					m_playerState = run;
				}
			}

			if (KdInputManager::Instance().GetButtonState("right"))
			{
				moveVec += {1, 0, 0};
				m_bMove = true;
				if (!(m_playerState & (jump | fall)))
				{
					if (!(m_playerState & run))
					{
						m_animator->SetAnimation(m_model->GetAnimation("RUN"));
						m_runAnimeCnt = 0;
					}
					else if (m_runAnimeCnt == 0)
					{
						m_animator->SetAnimation(m_model->GetAnimation("RUN"));
					}
					m_playerState = run;
				}
			}
		}

		if (KdInputManager::Instance().GetButtonState("jump"))
		{
			if (!(m_playerState & (jump | fall)))
			{
				m_gravity = -0.2f;
				m_playerState |= jump;
				KdAudioManager::Instance().Play("Asset/Audio/SE/FootstepsConcrete2.wav");
				m_animator->SetAnimation(m_model->GetAnimation("JumpA"), false);
				m_bMove = true;

			}
		}

		std::shared_ptr<CameraBase> spCamera = m_wpCamera.lock();
		if (spCamera)
		{
			//if (GetAsyncKeyState(VK_TAB)) { spCamera->SetBCameraSet(GetMatrix().Backward()); }

			if (!(m_playerState & (grassHopperDash | grassHopperDashUp)))
			{
				moveVec = moveVec.Transform(moveVec, spCamera->GetRotationYMatrix());
			}

			moveVec.Normalize();
		}

		if (!(m_playerState & (grassHopperDash | grassHopperDashUp)))
		{
			m_pos += moveVec * moveSpd;
		}
	}

	if (m_playerState & jump)
	{
		if (m_animator->IsAnimationEnd())
		{
			m_animator->SetAnimation(m_model->GetAnimation("JumpB"), true);
		}
	}

	if (m_playerState & idle)
	{
		if (m_animator->IsAnimationEnd())
		{
			m_animator->SetAnimation(m_model->GetAnimation("IdleB"), true);
		}
	}

	if (m_playerState & fall)
	{
		if (m_animator->IsAnimationEnd())
		{
			m_animator->SetAnimation(m_model->GetAnimation("FallB"), true);
		}
	}

	m_pos.y -= m_gravity;
	m_gravity += 0.01f;
	UpdateRotate(moveVec);
}

void Player::ScorpionAttackMove()
{
	if (m_animator->IsAnimationEnd())
	{
		if (m_playerState & lAttack)
		{
			m_playerState &= ~lAttack;
			m_bAttackAnimeDelay = false;
			m_bAttackAnimeCnt = false;
			m_attackAnimeCnt = 0;
			m_attackAnimeDelayCnt = 0;
			if (m_playerState & grassHopperDashF)
			{
				m_animator->SetAnimation(m_model->GetAnimation("GrassDashFA"), true);
			}
			else
			{
				m_bMove = false;
			}
		}

		if (m_playerState & rAttack)
		{
			m_playerState &= ~rAttack;
			m_bAttackAnimeDelay = false;
			m_bAttackAnimeCnt = false;
			m_attackAnimeCnt = 0;
			m_attackAnimeDelayCnt = 0;
			if (m_playerState & grassHopperDashF)
			{
				m_animator->SetAnimation(m_model->GetAnimation("GrassDashFA"), true);
			}
			else
			{
				m_bMove = false;
			}
		}

		if (m_playerState & (rlAttack | rlAttackRush))
		{
			m_playerState &= ~rlAttack;
			m_playerState &= ~rlAttackRush;
			m_bAttackAnimeDelay = false;
			m_bAttackAnimeCnt = false;
			m_bRushRp = false;
			m_attackAnimeCnt = 0;
			m_attackAnimeDelayCnt = 0;
			m_bMove = false;
		}

		if (m_playerState & mantis)
		{
			m_playerState &= ~mantis;
			m_attackAnimeCnt = 0;

			const std::shared_ptr<Scopion> scopion = std::dynamic_pointer_cast<Scopion>(m_weaponList[1]);
			scopion->SetBMantis(false);
			const std::shared_ptr<Scopion> scopion2 = std::dynamic_pointer_cast<Scopion>(m_weaponList[0]);
			scopion2->SetBMantis(false);
			m_bMove = false;
		}
	}
	else
	{
		if (!(m_playerState & rlAttackRush))
		{
			if (m_attackAnimeCnt >= 15)
			{
				m_attackMoveSpd *= 0.25f;
			}
			else
			{
				if (!m_enemy.expired())
				{
					Math::Vector3 dis = m_enemy.lock()->GetPos() - m_pos;
					if (dis.Length() <= 1.15f)
					{
						m_attackMoveSpd *= 0.25f;
					}
				}
				else
				{
					for (auto& enemyList : m_enemyList)
					{
						if (enemyList.expired())continue;
						Math::Vector3 dis = enemyList.lock()->GetPos() - m_pos;
						if (dis.Length() <= 1.15f)
						{
							m_attackMoveSpd *= 0.25f;
							break;
						}
					}
				}
			}
		}

		m_bMove = true;
		if (!(m_playerState & (rlAttack | rlAttackRush)))
		{
			m_attackMoveSpd *= 0.95f;

			if (m_playerState & (rAttackTwo | lAttackTwo))
			{
				if (m_attackAnimeCnt == 15)
				{
					for (auto& enemyList : m_enemyList)
					{
						if (enemyList.expired())continue;
						enemyList.lock()->SetAttackHit(false);
						enemyList.lock()->SetDefenseSuc(false);
					}
				}
			}
			else if (m_playerState & (rAttackThree | lAttackThree))
			{
				if (m_attackAnimeCnt == 15)
				{
					for (auto& enemyList : m_enemyList)
					{
						if (enemyList.expired())continue;
						enemyList.lock()->SetAttackHit(false);
						enemyList.lock()->SetDefenseSuc(false);
					}
				}
			}
		}
		else
		{
			if (m_playerState & rlAttackOne)
			{
				m_attackMoveSpd *= 0.95f;
				if (m_attackAnimeCnt == 17)
				{
					for (auto& enemyList : m_enemyList)
					{
						if (enemyList.expired())continue;
						enemyList.lock()->SetAttackHit(false);
						enemyList.lock()->SetDefenseSuc(false);
					}
				}
			}
			else if (m_playerState & rlAttackTwo)
			{
				m_attackMoveSpd *= 0.95f;
				if (m_attackAnimeCnt == 16)
				{
					for (auto& enemyList : m_enemyList)
					{
						if (enemyList.expired())continue;
						enemyList.lock()->SetAttackHit(false);
						enemyList.lock()->SetDefenseSuc(false);
					}
				}
			}
			else if (m_playerState & rlAttackThree)
			{
				m_attackMoveSpd *= 0.95f;
				if (m_attackAnimeCnt == 23)
				{
					for (auto& enemyList : m_enemyList)
					{
						if (enemyList.expired())continue;
						enemyList.lock()->SetAttackHit(false);
						enemyList.lock()->SetDefenseSuc(false);
					}
				}
			}
			else if (m_playerState & rlAttackRush)
			{
				if (!m_bRushRp)
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
						for (auto& enemyList : m_enemyList)
						{
							if (enemyList.expired())continue;
							enemyList.lock()->SetAttackHit(false);
							enemyList.lock()->SetDefenseSuc(false);
						}

						switch (m_attackAnimeCnt)
						{
						case 21:
						case 31:
							m_attackMoveSpd = 0.2f;
							break;
						case 49:
						case 55:
							m_attackMoveSpd = 0.25f;
							break;
						case 74:
						case 89:
							m_attackMoveSpd = 0.115f;
						case 107:
							m_attackMoveSpd = 0.05f;
							break;
						}
					}
				}
				else
				{
					if (m_attackAnimeCnt < 115)
					{
						m_attackMoveSpd *= 0.95f;
					}
					else
					{
						m_attackMoveSpd *= 0.75f;
					}

					if (m_attackAnimeCnt == 22 ||
						m_attackAnimeCnt == 33 ||
						m_attackAnimeCnt == 43 ||
						m_attackAnimeCnt == 53 ||
						m_attackAnimeCnt == 63 ||
						m_attackAnimeCnt == 73 ||
						m_attackAnimeCnt == 83 ||
						m_attackAnimeCnt == 93 ||
						m_attackAnimeCnt == 115
						)
					{
						for (auto& enemyList : m_enemyList)
						{
							if (enemyList.expired())continue;
							enemyList.lock()->SetAttackHit(false);
							enemyList.lock()->SetDefenseSuc(false);
						}

						switch (m_attackAnimeCnt)
						{
						case 22:
							m_attackMoveDir = m_wpCamera.lock()->GetMatrix().Backward();
							m_attackMoveDir.y = 0;
							m_attackMoveDir.Normalize();
							m_attackMoveSpd = 0.395f;
							break;
						case 33:
						case 43:
						case 53:
						case 63:
						case 73:
						case 83:
						case 93:
							m_attackMoveDir = m_wpCamera.lock()->GetMatrix().Backward();
							m_attackMoveDir.y = 0;
							m_attackMoveDir.Normalize();
							m_attackMoveSpd = 0.35f;
							break;
						case 115:
							if (!m_enemy.expired())
							{
								Math::Vector3 dis = m_enemy.lock()->GetPos() - m_pos;
								if (dis.Length() <= 1.15f)
								{
									m_attackMoveSpd = 0.25f;
								}
								else if (dis.Length() <= 1.35f)
								{
									m_attackMoveSpd = 0.45f;
								}
								else if (dis.Length() <= 1.65f)
								{
									m_attackMoveSpd = 0.65f;
								}
							}
							else
							{
								for (auto& enemyList : m_enemyList)
								{
									if (enemyList.expired())continue;
									Math::Vector3 dis = enemyList.lock()->GetPos() - m_pos;
									if (dis.Length() <= 1.15f)
									{
										m_attackMoveSpd = 0.25f;
										break;
									}
									else if (dis.Length() <= 1.35f)
									{
										m_attackMoveSpd = 0.45f;
										break;
									}
									else if (dis.Length() <= 1.65f)
									{
										m_attackMoveSpd = 0.65f;
										break;
									}
								}
							}
							break;
						}
					}
				}
			}
		}

		if (!(m_playerState & mantis))
		{
			UpdateRotate(m_attackMoveDir);
		}
		m_pos += m_attackMoveDir * m_attackMoveSpd;
	}
}

void Player::ScorpionDefenseMove()
{
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
	{
		if (KdInputManager::Instance().GetButtonState("lAttack"))
		{
			m_bMove = true;
		}
		else if (KdInputManager::Instance().GetButtonState("rAttack"))
		{
			m_bMove = true;
		}
		else
		{
			m_bMove = false;
			if (!(m_playerState & idle))
			{
				m_animator->SetAnimation(m_model->GetAnimation("IdleA"), false);
			}
			m_playerState = idle;
			//m_playerState &= ~defense;
		}
	}
	else
	{
		m_bMove = false;
		if (!(m_playerState & idle))
		{
			m_animator->SetAnimation(m_model->GetAnimation("IdleA"), false);
		}
		m_playerState = idle;
		//m_playerState &= ~defense;
	}
}

void Player::HasDefenseMove()
{
	m_pos.y -= m_gravity;
	m_gravity += 0.01f;
	m_bMove = true;
	if (m_animator->IsAnimationEnd())
	{
		if (!(m_playerState & idle))
		{
			m_animator->SetAnimation(m_model->GetAnimation("IdleA"), false);
		}
		m_playerState = idle;
	}
}

void Player::TutorialUpdate()
{
	switch (m_wpUi.lock()->GetTutorialType())
	{
	case Ui::TutorialType::kihonTu:
		if (m_runAnimeCnt >= 39)
		{
			m_wpUi.lock()->AddTutorialCnt();
		}

		if (m_playerState & jump && m_animator->IsAnimationEnd())
		{
			m_wpUi.lock()->AddTutorialCnt();
		}
		break;
	case Ui::TutorialType::bukiTu:
		break;
	case Ui::TutorialType::sukoADMoveTu:
		if (m_playerState & (rlAttackThree | rlAttackRush | rAttackThree | lAttackThree | mantis) && m_animator->IsAnimationEnd())
		{
			m_wpUi.lock()->AddTutorialCnt();
		}

		else if (m_playerState & defense)
		{
			m_tuGardTime++;
			if (m_tuGardTime >= 60)
			{
				m_tuGardTime = 0;
				m_wpUi.lock()->AddTutorialCnt();
			}
		}

		break;
	case Ui::TutorialType::hopperTu:
		if (m_playerState & (grassHopperDash | grassHopperDashUp) && m_rGrassHopperTime == 75 || m_playerState & (grassHopperDash | grassHopperDashUp) && m_lGrassHopperTime == 75)
		{
			m_wpUi.lock()->AddTutorialCnt();
		}
		break;
	}
}

void Player::ScorpionActionDecision()
{
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
	{
		if (m_gravity == 0)
		{
			if (KdInputManager::Instance().GetButtonState("lAttack"))
			{
				if (!(m_playerState & (rAttack | lAttack | defense | mantis | rlAttack | rlAttackRush)))
				{
					m_playerState = defense;
					m_playerState &= ~rAttack;
					m_playerState &= ~lAttack;
					m_playerState &= ~rlAttack;
					m_playerState &= ~rlAttackRush;
					m_bMove = true;
					m_animator->SetAnimation(m_model->GetAnimation("Defense"), true);
				}
				return;
			}

			if (KdInputManager::Instance().GetButtonState("rAttack"))
			{
				if (!(m_playerState & (rAttack | lAttack | defense | mantis | rlAttack | rlAttackRush)))
				{
					m_playerState = defense;
					m_playerState &= ~lAttack;
					m_playerState &= ~rAttack;
					m_playerState &= ~rlAttack;
					m_playerState &= ~rlAttackRush;
					m_bMove = true;
					m_animator->SetAnimation(m_model->GetAnimation("Defense"), true);
				}
				return;
			}
		}
	}
	else
	{
		if (!m_bAttackAnimeDelay)
		{
			if (m_playerState & grassHopperDash && m_lGrassHopperTime > 80 || m_rGrassHopperTime > 80)return;

			if (!(m_playerState & (rAttack | lAttack | mantis | rlAttack | rlAttackRush | defense)))
			{
				if (m_weaponType & lScorpion && m_weaponType & scorpion)
				{
					if (KdInputManager::Instance().GetButtonState("lAttack"))
					{
						if (KdInputManager::Instance().GetButtonState("rAttack"))
						{
							if (!(m_playerState & (mantis)))
							{
								for (auto& enemyList : m_enemyList)
								{
									if (enemyList.expired())continue;
									enemyList.lock()->SetAttackHit(false);
									enemyList.lock()->SetDefenseSuc(false);
								}

								m_playerState |= mantis;
								m_playerState &= ~rAttack;
								m_playerState &= ~lAttack;
								if (m_playerState & grassHopperDash)
								{
									m_playerState &= ~grassHopperDash;
								}
								std::shared_ptr<CameraBase> spCamera = m_wpCamera.lock();
								if (spCamera)
								{
									const std::shared_ptr<Scopion> scopion = std::dynamic_pointer_cast<Scopion>(m_weaponList[0]);
									scopion->SetMantis(spCamera->GetRotationMatrix(), true);
									const std::shared_ptr<Scopion> scopion2 = std::dynamic_pointer_cast<Scopion>(m_weaponList[1]);
									scopion2->SetBMantis(true);
								}
								m_bMove = true;
								m_animator->SetAnimation(m_model->GetAnimation("Mantis"), false);
							}
						}
						else
						{
							m_bRushAttackPossible = false;
							for (auto& enemyList : m_enemyList)
							{
								if (enemyList.expired())continue;
								enemyList.lock()->SetAttackHit(false);
								enemyList.lock()->SetDefenseSuc(false);
							}

							m_playerState |= rlAttackOne;
							m_playerState &= ~rAttack;
							m_playerState &= ~lAttack;
							m_bAttackAnimeDelay = false;
							m_bAttackAnimeCnt = true;
							m_attackAnimeCnt = 0;
							m_attackAnimeDelayCnt = 0;
							m_bMove = true;

							m_attackMoveDir = m_wpCamera.lock()->GetMatrix().Backward();

							m_attackMoveDir.y = 0;
							m_attackMoveDir.Normalize();
							if (KdInputManager::Instance().GetButtonState("forward"))
							{
								m_attackMoveSpd = 1.0f;
							}
							else
							{
								m_attackMoveSpd = 0.8f;
							}

							m_animator->SetAnimation(m_model->GetAnimation("RLAttackOne"), false);
						}
					}

					if (KdInputManager::Instance().GetButtonState("rAttack"))
					{
						if (KdInputManager::Instance().GetButtonState("lAttack"))
						{
							if (!(m_playerState & (mantis)))
							{
								for (auto& enemyList : m_enemyList)
								{
									if (enemyList.expired())continue;
									enemyList.lock()->SetAttackHit(false);
									enemyList.lock()->SetDefenseSuc(false);
								}
								m_playerState |= mantis;
								m_playerState &= ~rAttack;
								m_playerState &= ~lAttack;
								if (m_playerState & grassHopperDash)
								{
									m_playerState &= ~grassHopperDash;
								}
								std::shared_ptr<CameraBase> spCamera = m_wpCamera.lock();
								if (spCamera)
								{
									const std::shared_ptr<Scopion> scopion = std::dynamic_pointer_cast<Scopion>(m_weaponList[0]);
									scopion->SetMantis(spCamera->GetRotationMatrix(), true);
									const std::shared_ptr<Scopion> scopion2 = std::dynamic_pointer_cast<Scopion>(m_weaponList[1]);
									scopion2->SetBMantis(true);
								}
								m_bMove = true;
								//m_animator->SetAnimation(m_model->GetAnimation("mantis"), false);
							}
						}
						else
						{
							m_bRushAttackPossible = false;
							for (auto& enemyList : m_enemyList)
							{
								if (enemyList.expired())continue;
								enemyList.lock()->SetAttackHit(false);
								enemyList.lock()->SetDefenseSuc(false);
							}
							m_playerState |= rlAttackOne;
							m_playerState &= ~rAttack;
							m_playerState &= ~lAttack;
							m_bAttackAnimeDelay = false;
							m_bAttackAnimeCnt = true;
							m_attackAnimeCnt = 0;
							m_attackAnimeDelayCnt = 0;
							m_bMove = true;

							m_attackMoveDir = m_wpCamera.lock()->GetMatrix().Backward();
							m_attackMoveDir.y = 0;
							m_attackMoveDir.Normalize();
							if (KdInputManager::Instance().GetButtonState("forward"))
							{
								m_attackMoveSpd = 1.0f;
							}
							else
							{
								m_attackMoveSpd = 0.8f;
							}

							m_animator->SetAnimation(m_model->GetAnimation("RLAttackOne"), false);
						}
					}
				}
			}

			if (m_weaponType & lScorpion)
			{
				if (KdInputManager::Instance().GetButtonState("lAttack"))
				{
					if (!(m_playerState & (rAttack | lAttack | mantis | rlAttack | rlAttackRush)))
					{
						for (auto& enemyList : m_enemyList)
						{
							if (enemyList.expired())continue;
							enemyList.lock()->SetAttackHit(false);
							enemyList.lock()->SetDefenseSuc(false);
						}
						m_playerState |= lAttackOne;
						m_playerState &= ~rAttack;
						m_bAttackAnimeDelay = false;
						m_bAttackAnimeCnt = true;
						m_attackAnimeCnt = 0;
						m_attackAnimeDelayCnt = 0;
						m_bMove = true;

						m_attackMoveDir = m_wpCamera.lock()->GetMatrix().Backward();
						m_attackMoveDir.y = 0;
						m_attackMoveDir.Normalize();
						if (KdInputManager::Instance().GetButtonState("forward"))
						{
							m_attackMoveSpd = 1.0f;
						}
						else
						{
							m_attackMoveSpd = 0.8f;
						}

						if (m_playerState & grassHopperDash)
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

			if (m_weaponType & scorpion)
			{
				if (KdInputManager::Instance().GetButtonState("rAttack"))
				{
					if (!(m_playerState & (rAttack | lAttack | mantis | rlAttack | rlAttackRush)))
					{
						for (auto& enemyList : m_enemyList)
						{
							if (enemyList.expired())continue;
							enemyList.lock()->SetAttackHit(false);
							enemyList.lock()->SetDefenseSuc(false);
						}
						m_playerState |= rAttackOne;
						m_playerState &= ~lAttack;
						m_bAttackAnimeDelay = false;
						m_bAttackAnimeCnt = true;
						m_attackAnimeCnt = 0;
						m_attackAnimeDelayCnt = 0;
						m_bMove = true;

						m_attackMoveDir = m_wpCamera.lock()->GetMatrix().Backward();
						m_attackMoveDir.y = 0;
						m_attackMoveDir.Normalize();
						if (KdInputManager::Instance().GetButtonState("forward"))
						{
							m_attackMoveSpd = 1.0f;
						}
						else
						{
							m_attackMoveSpd = 0.8f;
						}

						if (m_playerState & (grassHopperDashF | step))
						{
							m_animator->SetAnimation(m_model->GetAnimation("GrassDashRAttack"), false);
						}
						else
						{
							m_animator->SetAnimation(m_model->GetAnimation("RAttack1"), false);
						}
					}
				}
			}
		}
		else
		{
			if (!(m_playerState & mantis))
			{
				if (m_weaponType & lScorpion && m_weaponType & scorpion)
				{
					if (KdInputManager::Instance().GetButtonState("lAttack") ||
						KdInputManager::Instance().GetButtonState("rAttack"))
					{
						if (m_playerState & rlAttackOne)
						{
							for (auto& enemyList : m_enemyList)
							{
								if (enemyList.expired())continue;
								enemyList.lock()->SetAttackHit(false);
								enemyList.lock()->SetDefenseSuc(false);
							}

							m_playerState |= rlAttackTwo;
							m_playerState &= ~rlAttackOne;
							m_bAttackAnimeDelay = false;
							m_bAttackAnimeCnt = true;
							m_attackAnimeCnt = 0;
							m_attackAnimeDelayCnt = 0;

							m_attackMoveDir = m_wpCamera.lock()->GetMatrix().Backward();

							m_attackMoveDir.y = 0;
							m_attackMoveDir.Normalize();
							m_attackMoveSpd = 0.8f;
							m_animator->SetAnimation(m_model->GetAnimation("RLAttackTwo"), false);
						}
						else if (m_playerState & rlAttackTwo)
						{
							for (auto& enemyList : m_enemyList)
							{
								if (enemyList.expired())continue;
								enemyList.lock()->SetAttackHit(false);
								enemyList.lock()->SetDefenseSuc(false);
							}

							m_playerState |= rlAttackThree;
							m_playerState &= ~rlAttackTwo;
							m_bAttackAnimeDelay = false;
							m_bAttackAnimeCnt = true;
							m_attackAnimeCnt = 0;
							m_attackAnimeDelayCnt = 0;

							m_attackMoveDir = m_wpCamera.lock()->GetMatrix().Backward();

							m_attackMoveDir.y = 0;
							m_attackMoveDir.Normalize();
							m_attackMoveSpd = 0.8f;
							m_animator->SetAnimation(m_model->GetAnimation("RLAttackThree"), false);
						}
						else if (m_playerState & rlAttackThree && m_bRushAttackPossible)
						{
							for (auto& enemyList : m_enemyList)
							{
								if (enemyList.expired())continue;
								enemyList.lock()->SetAttackHit(false);
								enemyList.lock()->SetDefenseSuc(false);
							}

							if (KdInputManager::Instance().GetButtonState("lAttack"))
							{
								m_bRushRp = false;
								m_animator->SetAnimation(m_model->GetAnimation("RLAttackRush"), false);
								m_attackMoveSpd = 0.8f;
							}
							else if (KdInputManager::Instance().GetButtonState("rAttack"))
							{
								m_bRushRp = true;
								m_animator->SetAnimation(m_model->GetAnimation("RLAttackRushRP"), false);
								m_attackMoveSpd = 0.25f;
							}

							m_playerState |= rlAttackRush;
							m_playerState &= rlAttackRush;
							m_bAttackAnimeDelay = false;
							m_bAttackAnimeCnt = true;
							m_attackAnimeCnt = 0;
							m_attackAnimeDelayCnt = 0;

							m_attackMoveDir = m_wpCamera.lock()->GetMatrix().Backward();
							m_attackMoveDir.y = 0;
							m_attackMoveDir.Normalize();
						}

						m_bMove = true;
					}
				}
				else if (m_weaponType & lScorpion)
				{
					if (KdInputManager::Instance().GetButtonState("lAttack"))
					{
						if (m_playerState & lAttackOne)
						{
							m_playerState |= lAttackTwo;
							m_playerState &= ~lAttackOne;
							m_bAttackAnimeDelay = false;
							m_bAttackAnimeCnt = true;
							m_attackAnimeCnt = 0;
							m_attackAnimeDelayCnt = 0;

							m_attackMoveDir = m_wpCamera.lock()->GetMatrix().Backward();

							m_attackMoveDir.y = 0;
							m_attackMoveDir.Normalize();
							m_attackMoveSpd = 0.8f;
							m_animator->SetAnimation(m_model->GetAnimation("LAttack2"), false);
						}
						else if (m_playerState & lAttackTwo)
						{
							m_playerState |= lAttackThree;
							m_playerState &= ~lAttackTwo;
							m_bAttackAnimeDelay = false;
							m_bAttackAnimeCnt = true;
							m_attackAnimeCnt = 0;
							m_attackAnimeDelayCnt = 0;

							m_attackMoveDir = m_wpCamera.lock()->GetMatrix().Backward();

							m_attackMoveDir.y = 0;
							m_attackMoveDir.Normalize();
							m_attackMoveSpd = 0.8f;
		
							m_animator->SetAnimation(m_model->GetAnimation("LAttack3"), false);
						}

						m_playerState &= ~rAttack;
						m_bMove = true;
					}
				}
				else if (m_weaponType & scorpion)
				{
					if (KdInputManager::Instance().GetButtonState("rAttack"))
					{
						if (m_playerState & rAttackOne)
						{
							m_playerState |= rAttackTwo;
							m_playerState &= ~rAttackOne;
							m_bAttackAnimeDelay = false;
							m_bAttackAnimeCnt = true;
							m_attackAnimeCnt = 0;
							m_attackAnimeDelayCnt = 0;

							m_attackMoveDir = m_wpCamera.lock()->GetMatrix().Backward();

							m_attackMoveDir.y = 0;
							m_attackMoveDir.Normalize();
							m_attackMoveSpd = 0.8f;
							m_animator->SetAnimation(m_model->GetAnimation("RAttack2"), false);
						}
						else if (m_playerState & rAttackTwo)
						{
							/*for (auto& enemyList : m_enemyList)
							{
								enemyList.lock()->SetAttackHit(false);
								enemyList.lock()->SetDefenseSuc(false);
							}*/

							m_playerState |= rAttackThree;
							m_playerState &= ~rAttackTwo;
							m_bAttackAnimeDelay = false;
							m_bAttackAnimeCnt = false;
							m_attackAnimeCnt = 0;
							m_attackAnimeDelayCnt = 0;

							m_attackMoveDir = m_wpCamera.lock()->GetMatrix().Backward();

							m_attackMoveDir.y = 0;
							m_attackMoveDir.Normalize();
							m_attackMoveSpd = 0.8f;
							m_animator->SetAnimation(m_model->GetAnimation("RAttack3"), false);
						}

						m_playerState &= ~lAttack;
						m_bMove = true;
					}
				}
			}
		}

	}
}

