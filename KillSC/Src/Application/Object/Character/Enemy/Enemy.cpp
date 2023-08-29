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
	transMat = Math::Matrix::CreateTranslation(0, 0, 10);
	m_pos = { 0,0,10 };

	// 拡縮行列
	Math::Matrix scaleMat;
	scaleMat = Math::Matrix::CreateScale(1.0f, 1.0f, 1.0f);

	// 回転行列
	Math::Matrix rotMat;
	rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(180));
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

	m_pDebugWire = std::make_unique<KdDebugWireFrame>();

	m_EnemyState = eIdle;

	m_weaponType |= eScorpion;
	m_weaponType |= eLScorpion;
	m_wantToMoveState = none;
	m_attackHitImmediatelyAfter = false;
	m_rightWeaponNumber = 1;
	m_leftWeaponNumber = 1;

	m_hitStopCnt = 0;
	m_hitMoveSpd = 0.0f;
	m_gardMoveSpd = 0.0f;
	m_torion = 300.0f;
	m_bTough = false;
	m_bFirstUpdate = true;


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
}

void Enemy::Update()
{
	m_torion -= m_graduallyTorionDecVal;
	if (m_torion <= 0)
	{
		m_torion = 0;
		SceneManager::Instance().SetBAddOrSubVal(true);
		SceneManager::Instance().SetPointAddOrSubVal(1000);
		SceneManager::Instance().SetBPlayerWin();
		SceneManager::Instance().SetNextScene(SceneManager::SceneType::result);
	}

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

	if (!(m_EnemyState & eHit))
	{
		std::shared_ptr<Player> spTarget = m_target.lock();
		if (spTarget)
		{
			Math::Vector3 vTarget = spTarget->GetPos() - m_pos;
			UpdateRotate(vTarget);
		}


		if (!(m_EnemyState & (eFall | eJump)))
		{
			m_bMove = false;
		}

		std::random_device rnd;
		std::mt19937 mt(rnd());
		std::uniform_int_distribution<int> intRand(0, 999);
		int randNum[9] = {};
		Math::Vector3 src;
		if (m_wantToMoveState & none)
		{
			--m_thinkActionDelayTime;
			m_pos.y -= m_gravity;
			m_gravity += 0.01f;
			if (m_thinkActionDelayTime <= 0)
			{
				int rand = intRand(mt);
				m_thinkActionDelayTime = m_thinkActionDelayTimeVal;
				//m_actionDelayTime = m_actionDelayTimeVal;
				src = spTarget->GetPos() - m_pos;
				if (src.Length() <= 1.2f)
				{
					if (m_attackHitImmediatelyAfter)
					{
						randNum[0] = 100;
						randNum[1] = 150;
						randNum[2] = 150;
						randNum[3] = 100;
						randNum[4] = 100;
						randNum[5] = 100;
						randNum[6] = 100;
						randNum[7] = 100;
						randNum[8] = 100;
					}
					else
					{
						randNum[0] = 200;
						randNum[1] = 50;
						randNum[2] = 10;
						randNum[3] = 150;
						randNum[4] = 100;
						randNum[5] = 140;
						randNum[6] = 100;
						randNum[7] = 100;
						randNum[8] = 100;
					}

					if (m_attackHitImmediatelyAfter)
					{
						for (int i = 0; i < 9; i++)
						{
							rand -= randNum[i];
							if (rand < 0)
							{
								switch (i)
								{
								case 0:
									m_wantToMoveState = WantToMoveState::attack;
									break;
								case 1:
									m_wantToMoveState = WantToMoveState::escape;
									break;
								case 2:
									m_wantToMoveState = WantToMoveState::defense;
									break;
								case 3:
									m_wantToMoveState = WantToMoveState::dashAttack;
									break;
								case 4:
									m_wantToMoveState = WantToMoveState::run;
									break;
								case 5:
									m_wantToMoveState = WantToMoveState::disturbance;
									break;
								case 6:
									m_wantToMoveState = WantToMoveState::step;
									break;
								case 7:
									m_wantToMoveState = WantToMoveState::grassDash;
									break;
								case 8:
									m_wantToMoveState = WantToMoveState::avoidance;
									break;
								}
								break;
							}
						}
					}
					else
					{
						for (int i = 0; i < 9; i++)
						{
							rand -= randNum[i];
							if (rand < 0)
							{
								switch (i)
								{
								case 0:
									m_wantToMoveState = WantToMoveState::attack;
									break;
								case 1:
									m_wantToMoveState = WantToMoveState::escape;
									break;
								case 2:
									m_wantToMoveState = WantToMoveState::defense;
									break;
								case 3:
									m_wantToMoveState = WantToMoveState::dashAttack;
									break;
								case 4:
									m_wantToMoveState = WantToMoveState::run;
									break;
								case 5:
									m_wantToMoveState = WantToMoveState::disturbance;
									break;
								case 6:
									m_wantToMoveState = WantToMoveState::step;
									break;
								case 7:
									m_wantToMoveState = WantToMoveState::grassDash;
									break;
								case 8:
									m_wantToMoveState = WantToMoveState::avoidance;
									break;
								}
								break;
							}
						}
					}
				}
				else
				{
					randNum[0] =   0;
					randNum[1] =  50;
					randNum[2] =   0;
					randNum[3] = 2500;
					randNum[4] = 200;
					randNum[5] = 100;
					randNum[6] = 100;
					randNum[7] = 300;
					randNum[8] =   0;

					for (int i = 0; i < 9; i++)
					{
						rand -= randNum[i];
						if (rand < 0)
						{
							switch (i)
							{
							case 0:
								m_wantToMoveState = WantToMoveState::attack;
								break;
							case 1:
								m_wantToMoveState = WantToMoveState::escape;
								break;
							case 2:
								m_wantToMoveState = WantToMoveState::defense;
								break;
							case 3:
								m_wantToMoveState = WantToMoveState::dashAttack;
								break;
							case 4:
								m_wantToMoveState = WantToMoveState::run;
								break;
							case 5:
								m_wantToMoveState = WantToMoveState::disturbance;
								break;
							case 6:
								m_wantToMoveState = WantToMoveState::step;
								break;
							case 7:
								m_wantToMoveState = WantToMoveState::grassDash;
								break;
							case 8:
								m_wantToMoveState = WantToMoveState::avoidance;
								break;
							}
							break;
						}
					}
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
				case WantToMoveState::disturbance:
					m_leftWeaponNumber = 2;
					m_rightWeaponNumber = 2;
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

		if (!(m_wantToMoveState & WantToMoveState::none))
		{
			//--m_actionDelayTime;
			if (/*m_actionDelayTime <= 0*/1)
			{
				//m_actionDelayTime = 0;
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
						ScorpionDefenseDecision();
						break;
					case WantToMoveState::dashAttack:
						src = spTarget->GetPos() - m_pos;
						if (m_dashSpd == 0.8f || m_dashSpd == 0.5f)
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
							if (src.Length() <= 3.5f)
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
					case WantToMoveState::disturbance:
						GrassMoveVecDecision();
						break;
					case WantToMoveState::step:
						NormalMove();
						break;
					case WantToMoveState::grassDash:
						GrassMoveVecDecision();
						break;
					case WantToMoveState::avoidance:
						GrassMoveVecDecision();
						break;
					}
				}
			}


			if (!(m_EnemyState & (eGrassHopperDash | eGrassHopperDashUp)))
			{
				if (m_EnemyState & (eLAttack | eRAttack))
				{
					ScorpionAttackMove();
				}
				else if (m_EnemyState & eRun)
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

		if (!m_bMove)
		{
			if (!(m_EnemyState & (eFall | eJump)))
			{
				m_EnemyState = eIdle;
				m_animator->SetAnimation(m_model->GetAnimation("Idle"));
			}
		}
	}
	else
	{
		m_pos.y -= m_gravity;
		m_gravity += 0.01f;
		if(m_hitStopCnt > 0)
		{
			--m_hitStopCnt;
			m_pos += m_knockBackVec * m_hitMoveSpd;
			m_hitMoveSpd *= 0.95f;
		}

		if (m_hitStopCnt == 0)
		{
			m_hitStopCnt = 0;
			m_EnemyState = eIdle;
			m_thinkActionDelayTime = m_thinkActionDelayTimeVal;
			//m_actionDelayTime = m_actionDelayTimeVal;
			m_wantToMoveState = none;
			m_attackHitImmediatelyAfter = true;
			//m_EnemyState = eDefense;
			m_hitMoveSpd = 0.0f;
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
		rayInfo.m_dir = {0,0,1};
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
		m_pos = groundPos + Math::Vector3(0, -0.7f, 0);
		m_gravity = 0;
		if (m_EnemyState & (eFall | eJump))
		{
			m_thinkActionDelayTime = m_thinkActionDelayTimeVal;
			//m_actionDelayTime = m_actionDelayTimeVal;
			m_wantToMoveState = none;
		}
		m_enemyAirborneTimetoBeCnt = ENEMYAIRBORNETIMETOBECNTVAL;
		
		//else
		//{
		//	//m_pos = groundPos /*+ Math::Vector3(0,-0.1,0)*/;
		//	m_gravity = 0;
		//	m_EnemyState = eFall;
		//	m_rGrassHopperTime = 0;
		//	m_lGrassHopperTime = 0;
		//	m_bMove = false;
		//	m_grassHopperDashDir = {};
		//	m_dashSpd = 0.0f;
		//	m_thinkActionDelayTime = m_thinkActionDelayTimeVal;
		//	//m_actionDelayTime = m_actionDelayTimeVal;
		//	m_wantToMoveState = none;
		//}
	}
	else
	{
		if (m_EnemyState & (eGrassHopperDash | eGrassHopperDashUp))
		{
			--m_enemyAirborneTimetoBeCnt;
			if (m_enemyAirborneTimetoBeCnt <= 0)
			{
				m_enemyAirborneTimetoBeCnt = 0;
				m_EnemyState = eFall;
				m_thinkActionDelayTime = m_thinkActionDelayTimeVal + 1;
				//m_actionDelayTime = m_actionDelayTimeVal;
				m_wantToMoveState = none;
			}
		}
	}

	KdCollider::SphereInfo sphereInfo;
	// 球の中心位置を設定
	sphereInfo.m_sphere.Center = m_pos + Math::Vector3(0, 1.5f, 0);
	if (!(m_EnemyState & (eGrassHopperDash | eGrassHopperDashUp)))
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
			// 球とモデルが当たっている
			m_pos += (hitDir * maxOverLap);
		}
	}

	if (!(m_EnemyState & eGrassHopperDash) && !(m_target.lock()->GetPlayerState() & Player::PlayerState::grassHopperDash))
	{
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

		m_target.lock()->Intersects
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
			// 球とモデルが当たっている
			m_pos += (hitDir * maxOverLap);
		}
	}

	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_pos);
	Math::Matrix RotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y));
	m_mWorld = RotMat * transMat;

	for (auto& WeaList : m_weaponList)
	{
		WeaList->Update();
	}
}

void Enemy::PostUpdate()
{
	if (m_gravity > 0)
	{
		if (!(m_EnemyState & eHit))
		{
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

	if (!m_animator) return;

	if (!(m_EnemyState & (eLAttack | eRAttack)))
	{
		m_animator->AdvanceTime(m_model->WorkNodes());
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
}

void Enemy::OnHit(Math::Vector3 a_KnocBackvec)
{
	m_EnemyState = eHit;
	m_hitStopCnt = 10;
	m_hitMoveSpd = 0.05f;
	m_knockBackVec = a_KnocBackvec;
	m_torion -= 50.0f;
	m_attackHit = true;
	if (m_torion <= 0)
	{
		m_torion = 0;
		SceneManager::Instance().SetBAddOrSubVal(true);
		SceneManager::Instance().SetPointAddOrSubVal(1000);
		SceneManager::Instance().SetBPlayerWin();
		SceneManager::Instance().SetNextScene(SceneManager::SceneType::result);
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

void Enemy::HasDefense()
{
	if (m_EnemyState & eRAttack)
	{
		m_animator->SetAnimation(m_model->GetAnimation("RHasDefense"), false);
	}
	else if (m_EnemyState & eLAttack)
	{
		m_animator->SetAnimation(m_model->GetAnimation("LHasDefense"), false);
	}

	m_thinkActionDelayTime = m_thinkActionDelayTimeVal;
	//m_actionDelayTime = m_actionDelayTimeVal;
	m_wantToMoveState = none;
	m_EnemyState = eHasDefense;
}

void Enemy::DrawSprite()
{

}

void Enemy::DrawDebug()
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

void Enemy::GenerateDepthMapFromLight_SkinMesh()
{
	if (!m_model) return;

	KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_model, m_mWorld);
}

void Enemy::DrawLit_SkinMesh()
{
	if (!m_model) return;

	if (m_hitStopCnt <= 5)
	{
		Math::Color color = { 1,1,1,1 };
		KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_model, m_mWorld,color);
	}
	else if (m_hitStopCnt > 5)
	{
		Math::Color color = { 1,0,0,1 };
		KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_model, m_mWorld, color);
	}
}

void Enemy::DrawLit()
{
	if (!m_bEwaponDrawPosible)return;

	for (auto& WeaList : m_weaponList)
	{
		WeaList->DrawLit();
	}
}

void Enemy::DrawBright()
{
	if (!m_bEwaponDrawPosible)return;

	for (auto& WeaList : m_weaponList)
	{
		WeaList->DrawBright();
	}
}

void Enemy::GenerateDepthMapFromLight()
{
	if (!m_bEwaponDrawPosible)return;

	for (auto& WeaList : m_weaponList)
	{
		WeaList->GenerateDepthMapFromLight();
	}
}

void Enemy::UpdateRotate(Math::Vector3& a_srcMoveVec)
{
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

	if (m_delayTurnAroundTime == 0)
	{
		m_mWorldRot.y += rotateAng;
	}
}

void Enemy::GrassMoveVecDecision()
{
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

			switch (m_wantToMoveState)
			{
			case WantToMoveState::escape:
				if (m_grassSuccessionDelayCnt != 0)return;
				randNum[0] =   0;
				randNum[1] = 550;
				randNum[2] = 150;
				randNum[3] = 150;
				randNum[4] = 150;
				break;
			case WantToMoveState::dashAttack:
				src = m_target.lock()->GetPos() - m_pos;
				if (src.Length() <= 2.0f)
				{
					randNum[0] = 100;    // 前
					randNum[1] = 700;    // 後
					randNum[2] = 100;    // 右 
					randNum[3] = 100;    // 左
					randNum[4] =   0;    // 上
				}
				else
				{
					randNum[0] = 700;    // 前
					randNum[1] = 100;    // 後
					randNum[2] = 100;    // 右 
					randNum[3] = 100;    // 左
					randNum[4] =   0;
				}
				break;
			case WantToMoveState::disturbance:
				if (m_grassSuccessionDelayCnt != 0)return;
				randNum[0] = 200;
				randNum[1] = 200;
				randNum[2] = 200;
				randNum[3] = 200;
				randNum[4] = 200;
				break;
			case WantToMoveState::grassDash:
				if (m_grassSuccessionDelayCnt != 0)return;
				randNum[0] = 450;
				randNum[1] = 250;
				randNum[2] = 150;
				randNum[3] = 150;
				randNum[4] =   0;
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
						m_weaponList[2]->GrassHopper({ m_pos.x - 0.65f * m_mWorldRot.x,m_pos.y + 0.9f,m_pos.z - 0.65f * m_mWorldRot.z }, m_mWorldRot.y);
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
						m_weaponList[2]->GrassHopper({ m_pos.x + 0.65f * m_mWorldRot.x,m_pos.y + 0.9f,m_pos.z + 0.65f * m_mWorldRot.z }, m_mWorldRot.y);
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
						m_weaponList[2]->GrassHopper({ m_pos.x + 0.65f * m_mWorldRot.x,m_pos.y + 0.9f,m_pos.z + 0.65f * m_mWorldRot.z }, m_mWorldRot.y);
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
						
						m_weaponList[2]->GrassHopper({ m_pos.x,m_pos.y + 0.7f,m_pos.z });
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
					randNum[0] = 100;    // 前
					randNum[1] = 700;    // 後
					randNum[2] = 100;    // 右 
					randNum[3] = 100;    // 左
					randNum[4] =   0;    // 上
				}
				else
				{
					randNum[0] = 700;    // 前
					randNum[1] = 100;    // 後
					randNum[2] = 100;    // 右 
					randNum[3] = 100;    // 左
					randNum[4] =   0;
				}
				break;
			case WantToMoveState::disturbance:
				if (m_grassSuccessionDelayCnt != 0)return;
				randNum[0] = 200;
				randNum[1] = 200;
				randNum[2] = 200;
				randNum[3] = 200;
				randNum[4] = 200;
				break;
			case WantToMoveState::grassDash:
				if (m_grassSuccessionDelayCnt != 0)return;
				randNum[0] = 350;
				randNum[1] = 150;
				randNum[2] = 200;
				randNum[3] = 200;
				randNum[4] = 100;
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

			for (int i = 0; i < 9; i++)
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
						m_weaponList[3]->GrassHopper({ m_pos.x + 0.65f * m_mWorldRot.x,m_pos.y + 0.9f,m_pos.z + 0.65f * m_mWorldRot.z }, m_mWorldRot.y);
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
						m_weaponList[3]->GrassHopper({ m_pos.x - 0.65f * m_mWorldRot.x,m_pos.y + 0.9f,m_pos.z - 0.65f * m_mWorldRot.z }, m_mWorldRot.y);
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
						m_weaponList[3]->GrassHopper({ m_pos.x + 0.65f * m_mWorldRot.x,m_pos.y + 0.9f,m_pos.z + 0.65f * m_mWorldRot.z }, m_mWorldRot.y);
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
						m_weaponList[3]->GrassHopper({ m_pos.x + 0.65f * m_mWorldRot.x,m_pos.y + 0.9f,m_pos.z + 0.65f * m_mWorldRot.z }, m_mWorldRot.y);
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
						
						m_weaponList[3]->GrassHopper({ m_pos.x,m_pos.y + 0.7f,m_pos.z });
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

	if (!(m_EnemyState & (eRAttack | eLAttack | eDefense | eMantis)))
	{
		m_EnemyState = eDefense;
		m_EnemyState &= ~eRAttack;
		m_EnemyState &= ~eLAttack;
		m_bMove = true;
		m_animator->SetAnimation(m_model->GetAnimation("Defense"), true);
	}



	if (!(m_EnemyState & (eRAttack | eLAttack | eDefense | eMantis)))
	{
		m_EnemyState = eDefense;
		m_EnemyState &= ~eLAttack;
		m_EnemyState &= ~eRAttack;
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
		m_bMove = true;
		if (m_lGrassHopperTime <= 90 && m_lGrassHopperTime > 80 || m_rGrassHopperTime <= 90 && m_rGrassHopperTime > 80)
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
	}
	else
	{
		m_dashSpd = 0.0f;
		m_grassHopperDashDir = {};
		m_gravity = 0;
		m_EnemyState = eFall;

		std::random_device rnd;
		std::mt19937 mt(rnd());
		std::uniform_int_distribution<int> intRand(0, 999);
		int randNum[9] = {};

		int rand = intRand(mt);
		Math::Vector3 src;

		--m_thinkActionDelayTime;
		if (m_thinkActionDelayTime <= 0)
		{
			m_thinkActionDelayTime = m_thinkActionDelayTimeVal;
			src = m_target.lock()->GetPos() - m_pos;
			if (src.Length() <= 1.2f)
			{
				switch (m_wantToMoveState)
				{
				case WantToMoveState::escape:
					randNum[0] =  50;
					randNum[1] =  50;
					randNum[2] = 200;
					randNum[3] = 200;
					randNum[4] =  50;
					randNum[5] = 100;
					randNum[6] = 100;
					randNum[7] = 100;
					randNum[8] = 100;
					break;
				case WantToMoveState::dashAttack:
					src = m_target.lock()->GetPos() - m_pos;
					randNum[0] =   0;
					randNum[1] = 200;
					randNum[2] = 150;
					randNum[3] = 200;
					randNum[4] =   0;
					randNum[5] =  50;
					randNum[6] = 100;
					randNum[7] = 200;
					randNum[8] = 100;
					break;
				case WantToMoveState::disturbance:
					randNum[0] =   0;
					randNum[1] = 250;
					randNum[2] =   0;
					randNum[3] = 200;
					randNum[4] =   0;
					randNum[5] = 300;
					randNum[6] =   0;
					randNum[7] = 250;
					randNum[8] =   0;
					break;
				case WantToMoveState::grassDash:
					randNum[0] =   0;
					randNum[1] = 150;
					randNum[2] = 100;
					randNum[3] = 200;
					randNum[4] = 100;
					randNum[5] =  50;
					randNum[6] = 100;
					randNum[7] = 200;
					randNum[8] = 100;
					break;
				case WantToMoveState::avoidance:
					randNum[0] =   0;
					randNum[1] = 150;
					randNum[2] = 100;
					randNum[3] = 300;
					randNum[4] = 100;
					randNum[5] = 100;
					randNum[6] =   0;
					randNum[7] = 150;
					randNum[8] = 100;
					break;
				}

				for (int i = 0; i < 9; i++)
				{
					rand -= randNum[i];
					if (rand < 0)
					{
						switch (i)
						{
						case 0:
							m_wantToMoveState = WantToMoveState::attack;
							break;
						case 1:
							m_wantToMoveState = WantToMoveState::escape;
							break;
						case 2:
							m_wantToMoveState = WantToMoveState::defense;
							break;
						case 3:
							m_wantToMoveState = WantToMoveState::dashAttack;
							break;
						case 4:
							m_wantToMoveState = WantToMoveState::run;
							break;
						case 5:
							m_wantToMoveState = WantToMoveState::disturbance;
							break;
						case 6:
							m_wantToMoveState = WantToMoveState::step;
							break;
						case 7:
							m_wantToMoveState = WantToMoveState::grassDash;
							break;
						case 8:
							m_wantToMoveState = WantToMoveState::avoidance;
							break;
						}
						break;
					}
				}

			}
			else
			{
			switch (m_wantToMoveState)
			{
			case WantToMoveState::escape:
				randNum[0] =   0;
				randNum[1] = 100;
				randNum[2] =   0;
				randNum[3] = 350;
				randNum[4] =  50;
				randNum[5] = 100;
				randNum[6] = 100;
				randNum[7] = 150;
				randNum[8] = 100;
				break;
			case WantToMoveState::dashAttack:
				src = m_target.lock()->GetPos() - m_pos;
				randNum[0] =   0;
				randNum[1] = 100;
				randNum[2] =   0;
				randNum[3] = 300;
				randNum[4] = 150;
				randNum[5] =  50;
				randNum[6] = 100;
				randNum[7] = 200;
				randNum[8] = 100;
				break;
			case WantToMoveState::disturbance:
				randNum[0] = 0;
				randNum[1] = 150;
				randNum[2] = 0;
				randNum[3] = 300;
				randNum[4] = 0;
				randNum[5] = 300;
				randNum[6] = 0;
				randNum[7] = 250;
				randNum[8] = 0;
				break;
			case WantToMoveState::grassDash:
				randNum[0] =   0;
				randNum[1] = 100;
				randNum[2] = 100;
				randNum[3] = 200;
				randNum[4] = 150;
				randNum[5] =  50;
				randNum[6] = 100;
				randNum[7] = 200;
				randNum[8] = 100;
				break;
			case WantToMoveState::avoidance:
				randNum[0] = 0;
				randNum[1] = 150;
				randNum[2] = 100;
				randNum[3] = 300;
				randNum[4] = 100;
				randNum[5] = 100;
				randNum[6] =   0;
				randNum[7] = 150;
				randNum[8] = 100;
				break;
			}

				for (int i = 0; i < 9; i++)
				{
					rand -= randNum[i];
					if (rand < 0)
					{
						switch (i)
						{
						case 0:
							m_wantToMoveState = WantToMoveState::attack;
							break;
						case 1:
							m_wantToMoveState = WantToMoveState::escape;
							break;
						case 2:
							m_wantToMoveState = WantToMoveState::defense;
							break;
						case 3:
							m_wantToMoveState = WantToMoveState::dashAttack;
							break;
						case 4:
							m_wantToMoveState = WantToMoveState::run;
							break;
						case 5:
							m_wantToMoveState = WantToMoveState::disturbance;
							break;
						case 6:
							m_wantToMoveState = WantToMoveState::step;
							break;
						case 7:
							m_wantToMoveState = WantToMoveState::grassDash;
							break;
						case 8:
							m_wantToMoveState = WantToMoveState::avoidance;
							break;
						}
						break;
					}
				}
			}

			switch (m_wantToMoveState)
			{
			case WantToMoveState::attack:
				m_leftWeaponNumber = 1;
				m_rightWeaponNumber = 1;
				break;
			case WantToMoveState::escape:
				m_wantToMoveState = WantToMoveState::escape;
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
			case WantToMoveState::disturbance:
				m_leftWeaponNumber = 2;
				m_rightWeaponNumber = 2;
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
				m_wantToMoveState = WantToMoveState::avoidance;
				m_leftWeaponNumber = 2;
				m_rightWeaponNumber = 1;
				break;
			}
		}
	}


	m_pos += m_grassHopperDashDir * m_dashSpd;

	/*if (m_EnemyState & eGrassHopperDash)
	{
		UpdateRotate(m_grassHopperDashDir);
	}
	else if (m_EnemyState & eGrassHopperDashUp)
	{
		UpdateRotate(Math::Vector3::TransformNormal(Math::Vector3(0, 0, 1), Math::Matrix::CreateRotationY(m_mWorldRot.y)));
	}*/
}

void Enemy::NormalMoveVecDecision()
{
	std::random_device rnd;
	std::mt19937 mt(rnd());
	std::uniform_int_distribution<int> intRand(0, 999);
	int randNum[5] = {};

	int rand = intRand(mt);
	randNum[0] = 650;
	randNum[1] = 100;
	randNum[2] = 100;
	randNum[3] = 100;
	randNum[4] = 50;

	for (int i = 0; i < 5; i++)
	{
		rand -= randNum[i];
		if (rand < 0)
		{
			switch (i)
			{
			case 0:
				m_EnemyState = eRunF;
				if (!(m_EnemyState & (eJump | eFall | eLAttack | eRAttack)))
				{
					m_animator->SetAnimation(m_model->GetAnimation("RUN"));
				}
				break;
			case 1:
				m_EnemyState = eRunB;
				if (!(m_EnemyState & (eJump | eFall | eLAttack | eRAttack)))
				{
					m_animator->SetAnimation(m_model->GetAnimation("RUN"));
				}
				break;
			case 2:
				m_EnemyState = eRunR;
				if (!(m_EnemyState & (eJump | eFall | eLAttack | eRAttack)))
				{
					m_animator->SetAnimation(m_model->GetAnimation("RUN"));
				}
				break;
			case 3:
				m_EnemyState = eRunL;
				if (!(m_EnemyState & (eJump | eFall | eLAttack | eRAttack)))
				{
					m_animator->SetAnimation(m_model->GetAnimation("RUN"));
				}
				break;
			case 4:
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

	if (m_EnemyState & eRunF)
	{
		moveVec += Math::Vector3::TransformNormal({ 0, 0, 1 }, Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
		m_bMove = true;
	}
	else if (m_EnemyState & eRunL)
	{
		moveVec += Math::Vector3::TransformNormal({ -1, 0, 0 }, Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
		m_bMove = true;
	}

	else if (m_EnemyState & eRunB)
	{
		moveVec += Math::Vector3::TransformNormal({ 0, 0, -1 }, Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
		m_bMove = true;
	}

	else if (m_EnemyState & eRunR)
	{
		Math::Vector3::TransformNormal({ 1, 0, 0 }, Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
		m_bMove = true;
	}
	else if (m_EnemyState & eJump)
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

	std::random_device rnd;
	std::mt19937 mt(rnd());
	std::uniform_int_distribution<int> intRand(0, 999);
	int randNum[9] = {};

	int rand = intRand(mt);
	Math::Vector3 src;

	--m_thinkActionDelayTime;
	if (m_thinkActionDelayTime <= 0)
	{
		m_thinkActionDelayTime = m_thinkActionDelayTimeVal;
		//m_actionDelayTime = m_actionDelayTimeVal;
		src = m_target.lock()->GetPos() - m_pos;
		if (src.Length() <= 1.2f)
		{
			randNum[0] = 350;
			randNum[1] = 0;
			randNum[2] = 50;
			randNum[3] = 150;
			randNum[4] = 100;
			randNum[5] = 100;
			randNum[6] = 100;
			randNum[7] = 50;
			randNum[8] = 100;

			for (int i = 0; i < 9; i++)
			{
				rand -= randNum[i];
				if (rand < 0)
				{
					switch (i)
					{
					case 0:
						m_wantToMoveState = WantToMoveState::attack;
						break;
					case 1:
						m_wantToMoveState = WantToMoveState::escape;
						break;
					case 2:
						m_wantToMoveState = WantToMoveState::defense;
						break;
					case 3:
						m_wantToMoveState = WantToMoveState::dashAttack;
						break;
					case 4:
						m_wantToMoveState = WantToMoveState::run;
						break;
					case 5:
						m_wantToMoveState = WantToMoveState::disturbance;
						break;
					case 6:
						m_wantToMoveState = WantToMoveState::step;
						break;
					case 7:
						m_wantToMoveState = WantToMoveState::grassDash;
						break;
					case 8:
						m_wantToMoveState = WantToMoveState::avoidance;
						break;
					}
					break;
				}
			}

		}
		else
		{
			randNum[0] = 0;
			randNum[1] = 0;
			randNum[2] = 0;
			randNum[3] = 300;
			randNum[4] = 250;
			randNum[5] = 100;
			randNum[6] = 150;
			randNum[7] = 150;
			randNum[8] = 50;

			for (int i = 0; i < 9; i++)
			{
				rand -= randNum[i];
				if (rand < 0)
				{
					switch (i)
					{
					case 0:
						m_wantToMoveState = WantToMoveState::attack;
						break;
					case 1:
						m_wantToMoveState = WantToMoveState::escape;
						break;
					case 2:
						m_wantToMoveState = WantToMoveState::defense;
						break;
					case 3:
						m_wantToMoveState = WantToMoveState::dashAttack;
						break;
					case 4:
						m_wantToMoveState = WantToMoveState::run;
						break;
					case 5:
						m_wantToMoveState = WantToMoveState::disturbance;
						break;
					case 6:
						m_wantToMoveState = WantToMoveState::step;
						break;
					case 7:
						m_wantToMoveState = WantToMoveState::grassDash;
						break;
					case 8:
						m_wantToMoveState = WantToMoveState::avoidance;
						break;
					}
					break;
				}
			}
		}

		switch (m_wantToMoveState)
		{
		case WantToMoveState::attack:
			m_leftWeaponNumber = 1;
			m_rightWeaponNumber = 1;
			break;
		case WantToMoveState::escape:
			m_wantToMoveState = WantToMoveState::escape;
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
		case WantToMoveState::disturbance:
			m_leftWeaponNumber = 2;
			m_rightWeaponNumber = 2;
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

}

void Enemy::ScorpionDefenseMove()
{
	std::shared_ptr<Player> spTarget = m_target.lock();
	if (spTarget)
	{
		Math::Vector3 vTarget = spTarget->GetPos() - m_pos;
		Math::Vector3 src = spTarget->GetPos() - m_pos;

		if (src.Length() <= 1.2f)
		{
			if (spTarget->GetPlayerState() & (Player::PlayerState::rAttack | Player::PlayerState::lAttack))
			{
				m_bMove = true;
			}
			else
			{
				std::random_device rnd;
				std::mt19937 mt(rnd());
				std::uniform_int_distribution<int> intRand(0, 999);
				int randNum[9] = {};

				int rand = intRand(mt);
				src;

				--m_thinkActionDelayTime;
				if (m_thinkActionDelayTime <= 0)
				{
					m_thinkActionDelayTime = m_thinkActionDelayTimeVal;
					//m_actionDelayTime = m_actionDelayTimeVal;
					src = m_target.lock()->GetPos() - m_pos;
					if (src.Length() <= 1.2f)
					{

						randNum[0] = 450;
						randNum[1] = 0;
						randNum[2] = 50;
						randNum[3] = 250;
						randNum[4] = 0;
						randNum[5] = 100;
						randNum[6] = 100;
						randNum[7] = 50;
						randNum[8] = 0;

						for (int i = 0; i < 9; i++)
						{
							rand -= randNum[i];
							if (rand < 0)
							{
								switch (i)
								{
								case 0:
									m_wantToMoveState = WantToMoveState::attack;
									break;
								case 1:
									m_wantToMoveState = WantToMoveState::escape;
									break;
								case 2:
									m_wantToMoveState = WantToMoveState::defense;
									break;
								case 3:
									m_wantToMoveState = WantToMoveState::dashAttack;
									break;
								case 4:
									m_wantToMoveState = WantToMoveState::run;
									break;
								case 5:
									m_wantToMoveState = WantToMoveState::disturbance;
									break;
								case 6:
									m_wantToMoveState = WantToMoveState::step;
									break;
								case 7:
									m_wantToMoveState = WantToMoveState::grassDash;
									break;
								case 8:
									m_wantToMoveState = WantToMoveState::avoidance;
									break;
								}
								break;
							}
						}

					}

					switch (m_wantToMoveState)
					{
					case WantToMoveState::attack:
						m_leftWeaponNumber = 1;
						m_rightWeaponNumber = 1;
						break;
					case WantToMoveState::escape:
						m_wantToMoveState = WantToMoveState::escape;
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
					case WantToMoveState::disturbance:
						m_leftWeaponNumber = 2;
						m_rightWeaponNumber = 2;
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
						m_wantToMoveState = WantToMoveState::avoidance;
						m_leftWeaponNumber = 2;
						m_rightWeaponNumber = 1;
						break;
					}
				}
			}
		}
		else
		{
			std::random_device rnd;
			std::mt19937 mt(rnd());
			std::uniform_int_distribution<int> intRand(0, 999);
			int randNum[9] = {};

			int rand = intRand(mt);
			src;

			--m_thinkActionDelayTime;
			if (m_thinkActionDelayTime <= 0)
			{
				m_thinkActionDelayTime = m_thinkActionDelayTimeVal;
				//m_actionDelayTime = m_actionDelayTimeVal;


				randNum[0] = 0;
				randNum[1] = 100;
				randNum[2] = 50;
				randNum[3] = 250;
				randNum[4] = 200;
				randNum[5] = 100;
				randNum[6] = 100;
				randNum[7] = 200;
				randNum[8] = 0;

				for (int i = 0; i < 9; i++)
				{
					rand -= randNum[i];
					if (rand < 0)
					{
						switch (i)
						{
						case 0:
							m_wantToMoveState = WantToMoveState::attack;
							break;
						case 1:
							m_wantToMoveState = WantToMoveState::escape;
							break;
						case 2:
							m_wantToMoveState = WantToMoveState::defense;
							break;
						case 3:
							m_wantToMoveState = WantToMoveState::dashAttack;
							break;
						case 4:
							m_wantToMoveState = WantToMoveState::run;
							break;
						case 5:
							m_wantToMoveState = WantToMoveState::disturbance;
							break;
						case 6:
							m_wantToMoveState = WantToMoveState::step;
							break;
						case 7:
							m_wantToMoveState = WantToMoveState::grassDash;
							break;
						case 8:
							m_wantToMoveState = WantToMoveState::avoidance;
							break;
						}
						break;
					}
				}

				switch (m_wantToMoveState)
				{
				case WantToMoveState::attack:
					m_leftWeaponNumber = 1;
					m_rightWeaponNumber = 1;
					break;
				case WantToMoveState::escape:
					m_wantToMoveState = WantToMoveState::escape;
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
				case WantToMoveState::disturbance:
					m_leftWeaponNumber = 2;
					m_rightWeaponNumber = 2;
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
					m_wantToMoveState = WantToMoveState::avoidance;
					m_leftWeaponNumber = 2;
					m_rightWeaponNumber = 1;
					break;
				}
			}
		}
	}
}

void Enemy::HasDefenseMove()
{
	m_pos.y -= m_gravity;
	m_gravity += 0.01f;
	m_bMove = true;
	if (m_animator->IsAnimationEnd())
	{
		//if (!(m_EnemyState & eIdle))
		//{
		//	m_animator->SetAnimation(m_model->GetAnimation("IdleA"), false);
		//}
		//m_EnemyState = eIdle;
		//m_thinkActionDelayTime = m_thinkActionDelayTimeVal;
		////m_actionDelayTime = m_actionDelayTimeVal;
		//m_wantToMoveState = none;

		std::random_device rnd;
		std::mt19937 mt(rnd());
		std::uniform_int_distribution<int> intRand(0, 999);
		int randNum[9] = {};

		int rand = intRand(mt);
		Math::Vector3 src;

		--m_thinkActionDelayTime;
		if (m_thinkActionDelayTime <= 0)
		{
			m_thinkActionDelayTime = m_thinkActionDelayTimeVal;
			//m_actionDelayTime = m_actionDelayTimeVal;
			src = m_target.lock()->GetPos() - m_pos;
			if (src.Length() <= 1.2f)
			{
				randNum[0] = 100;
				randNum[1] = 250;
				randNum[2] = 200;
				randNum[3] = 100;
				randNum[4] = 0;
				randNum[5] = 100;
				randNum[6] = 100;
				randNum[7] = 50;
				randNum[8] = 100;

				for (int i = 0; i < 9; i++)
				{
					rand -= randNum[i];
					if (rand < 0)
					{
						switch (i)
						{
						case 0:
							m_wantToMoveState = WantToMoveState::attack;
							break;
						case 1:
							m_wantToMoveState = WantToMoveState::escape;
							break;
						case 2:
							m_wantToMoveState = WantToMoveState::defense;
							break;
						case 3:
							m_wantToMoveState = WantToMoveState::dashAttack;
							break;
						case 4:
							m_wantToMoveState = WantToMoveState::run;
							break;
						case 5:
							m_wantToMoveState = WantToMoveState::disturbance;
							break;
						case 6:
							m_wantToMoveState = WantToMoveState::step;
							break;
						case 7:
							m_wantToMoveState = WantToMoveState::grassDash;
							break;
						case 8:
							m_wantToMoveState = WantToMoveState::avoidance;
							break;
						}
						break;
					}
				}

			}
			else
			{
				randNum[0] = 0;
				randNum[1] = 0;
				randNum[2] = 0;
				randNum[3] = 300;
				randNum[4] = 150;
				randNum[5] = 100;
				randNum[6] = 150;
				randNum[7] = 250;
				randNum[8] = 50;

				for (int i = 0; i < 9; i++)
				{
					rand -= randNum[i];
					if (rand < 0)
					{
						switch (i)
						{
						case 0:
							m_wantToMoveState = WantToMoveState::attack;
							break;
						case 1:
							m_wantToMoveState = WantToMoveState::escape;
							break;
						case 2:
							m_wantToMoveState = WantToMoveState::defense;
							break;
						case 3:
							m_wantToMoveState = WantToMoveState::dashAttack;
							break;
						case 4:
							m_wantToMoveState = WantToMoveState::run;
							break;
						case 5:
							m_wantToMoveState = WantToMoveState::disturbance;
							break;
						case 6:
							m_wantToMoveState = WantToMoveState::step;
							break;
						case 7:
							m_wantToMoveState = WantToMoveState::grassDash;
							break;
						case 8:
							m_wantToMoveState = WantToMoveState::avoidance;
							break;
						}
						break;
					}
				}
			}

			switch (m_wantToMoveState)
			{
			case WantToMoveState::attack:
				m_leftWeaponNumber = 1;
				m_rightWeaponNumber = 1;
				break;
			case WantToMoveState::escape:
				m_wantToMoveState = WantToMoveState::escape;
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
			case WantToMoveState::disturbance:
				m_leftWeaponNumber = 2;
				m_rightWeaponNumber = 2;
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
				m_wantToMoveState = WantToMoveState::avoidance;
				m_leftWeaponNumber = 2;
				m_rightWeaponNumber = 1;
				break;
			}
		}
	}
}

void Enemy::ScorpionAttackMove()
{
	if (m_animator->IsAnimationEnd())
	{
		if (m_EnemyState & eLAttack)
		{
			m_EnemyState &= ~eLAttack;
			if (m_EnemyState & eGrassHopperDashF | eStep)
			{
				m_animator->SetAnimation(m_model->GetAnimation("GrassDashFA"), true);
			}
			else
			{
				std::random_device rnd;
				std::mt19937 mt(rnd());
				std::uniform_int_distribution<int> intRand(0, 999);
				int randNum[9] = {};

				int rand = intRand(mt);
				Math::Vector3 src;

				--m_thinkActionDelayTime;
				if (m_thinkActionDelayTime <= 0)
				{
					m_thinkActionDelayTime = m_thinkActionDelayTimeVal;
					//m_actionDelayTime = m_actionDelayTimeVal;
					src = m_target.lock()->GetPos() - m_pos;
					if (src.Length() <= 1.2f)
					{
						if (m_target.lock()->GetAttackHit())
						{
							randNum[0] = 450;
							randNum[1] = 0;
							randNum[2] = 50;
							randNum[3] = 150;
							randNum[4] = 0;
							randNum[5] = 100;
							randNum[6] = 100;
							randNum[7] = 50;
							randNum[8] = 100;
						}
						else
						{
							randNum[0] = 100;
							randNum[1] = 150;
							randNum[2] = 200;
							randNum[3] = 100;
							randNum[4] = 100;
							randNum[5] = 100;
							randNum[6] = 100;
							randNum[7] = 50;
							randNum[8] = 100;
						}

						for (int i = 0; i < 9; i++)
						{
							rand -= randNum[i];
							if (rand < 0)
							{
								switch (i)
								{
								case 0:
									m_wantToMoveState = WantToMoveState::attack;
									break;
								case 1:
									m_wantToMoveState = WantToMoveState::escape;
									break;
								case 2:
									m_wantToMoveState = WantToMoveState::defense;
									break;
								case 3:
									m_wantToMoveState = WantToMoveState::dashAttack;
									break;
								case 4:
									m_wantToMoveState = WantToMoveState::run;
									break;
								case 5:
									m_wantToMoveState = WantToMoveState::disturbance;
									break;
								case 6:
									m_wantToMoveState = WantToMoveState::step;
									break;
								case 7:
									m_wantToMoveState = WantToMoveState::grassDash;
									break;
								case 8:
									m_wantToMoveState = WantToMoveState::avoidance;
									break;
								}
								break;
							}
						}

					}
					else
					{
						randNum[0] = 0;
						randNum[1] = 0;
						randNum[2] = 0;
						randNum[3] = 300;
						randNum[4] = 250;
						randNum[5] = 100;
						randNum[6] = 150;
						randNum[7] = 150;
						randNum[8] = 50;

						for (int i = 0; i < 9; i++)
						{
							rand -= randNum[i];
							if (rand < 0)
							{
								switch (i)
								{
								case 0:
									m_wantToMoveState = WantToMoveState::attack;
									break;
								case 1:
									m_wantToMoveState = WantToMoveState::escape;
									break;
								case 2:
									m_wantToMoveState = WantToMoveState::defense;
									break;
								case 3:
									m_wantToMoveState = WantToMoveState::dashAttack;
									break;
								case 4:
									m_wantToMoveState = WantToMoveState::run;
									break;
								case 5:
									m_wantToMoveState = WantToMoveState::disturbance;
									break;
								case 6:
									m_wantToMoveState = WantToMoveState::step;
									break;
								case 7:
									m_wantToMoveState = WantToMoveState::grassDash;
									break;
								case 8:
									m_wantToMoveState = WantToMoveState::avoidance;
									break;
								}
								break;
							}
						}
					}

					switch (m_wantToMoveState)
					{
					case WantToMoveState::attack:
						m_leftWeaponNumber = 1;
						m_rightWeaponNumber = 1;
						break;
					case WantToMoveState::escape:
						m_wantToMoveState = WantToMoveState::escape;
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
					case WantToMoveState::disturbance:
						m_leftWeaponNumber = 2;
						m_rightWeaponNumber = 2;
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
			}
		}

		if (m_EnemyState & eRAttack)
		{
			m_EnemyState &= ~eRAttack;
			if (m_EnemyState & eGrassHopperDashF | eStep)
			{
				m_animator->SetAnimation(m_model->GetAnimation("GrassDashFA"), true);
			}
			else
			{
				std::random_device rnd;
				std::mt19937 mt(rnd());
				std::uniform_int_distribution<int> intRand(0, 999);
				int randNum[9] = {};

				int rand = intRand(mt);
				Math::Vector3 src;

				--m_thinkActionDelayTime;
				if (m_thinkActionDelayTime <= 0)
				{
					m_thinkActionDelayTime = m_thinkActionDelayTimeVal;
					//m_actionDelayTime = m_actionDelayTimeVal;
					src = m_target.lock()->GetPos() - m_pos;
					if (src.Length() <= 1.2f)
					{
						if (m_target.lock()->GetAttackHit())
						{
							randNum[0] = 450;
							randNum[1] = 0;
							randNum[2] = 50;
							randNum[3] = 150;
							randNum[4] = 0;
							randNum[5] = 100;
							randNum[6] = 100;
							randNum[7] = 50;
							randNum[8] = 100;
						}
						else
						{
							randNum[0] = 100;
							randNum[1] = 150;
							randNum[2] = 200;
							randNum[3] = 100;
							randNum[4] = 100;
							randNum[5] = 100;
							randNum[6] = 100;
							randNum[7] = 50;
							randNum[8] = 100;
						}

						for (int i = 0; i < 9; i++)
						{
							rand -= randNum[i];
							if (rand < 0)
							{
								switch (i)
								{
								case 0:
									m_wantToMoveState = WantToMoveState::attack;
									break;
								case 1:
									m_wantToMoveState = WantToMoveState::escape;
									break;
								case 2:
									m_wantToMoveState = WantToMoveState::defense;
									break;
								case 3:
									m_wantToMoveState = WantToMoveState::dashAttack;
									break;
								case 4:
									m_wantToMoveState = WantToMoveState::run;
									break;
								case 5:
									m_wantToMoveState = WantToMoveState::disturbance;
									break;
								case 6:
									m_wantToMoveState = WantToMoveState::step;
									break;
								case 7:
									m_wantToMoveState = WantToMoveState::grassDash;
									break;
								case 8:
									m_wantToMoveState = WantToMoveState::avoidance;
									break;
								}
								break;
							}
						}

					}
					else
					{
						randNum[0] = 0;
						randNum[1] = 0;
						randNum[2] = 0;
						randNum[3] = 300;
						randNum[4] = 250;
						randNum[5] = 100;
						randNum[6] = 150;
						randNum[7] = 150;
						randNum[8] = 50;

						for (int i = 0; i < 9; i++)
						{
							rand -= randNum[i];
							if (rand < 0)
							{
								switch (i)
								{
								case 0:
									m_wantToMoveState = WantToMoveState::attack;
									break;
								case 1:
									m_wantToMoveState = WantToMoveState::escape;
									break;
								case 2:
									m_wantToMoveState = WantToMoveState::defense;
									break;
								case 3:
									m_wantToMoveState = WantToMoveState::dashAttack;
									break;
								case 4:
									m_wantToMoveState = WantToMoveState::run;
									break;
								case 5:
									m_wantToMoveState = WantToMoveState::disturbance;
									break;
								case 6:
									m_wantToMoveState = WantToMoveState::step;
									break;
								case 7:
									m_wantToMoveState = WantToMoveState::grassDash;
									break;
								case 8:
									m_wantToMoveState = WantToMoveState::avoidance;
									break;
								}
								break;
							}
						}
					}

					switch (m_wantToMoveState)
					{
					case WantToMoveState::attack:
						m_leftWeaponNumber = 1;
						m_rightWeaponNumber = 1;
						break;
					case WantToMoveState::escape:
						m_wantToMoveState = WantToMoveState::escape;
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
					case WantToMoveState::disturbance:
						m_leftWeaponNumber = 2;
						m_rightWeaponNumber = 2;
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
			}
		}
	}
	else
	{
		m_bMove = true;
	}
}

void Enemy::ScorpionAttackDecision()
{
	if (!(m_EnemyState & (eRAttack | eLAttack)))
	{
		if (m_EnemyState & eRAttackOne)
		{
			m_EnemyState |= eRAttackTwo;
			m_EnemyState &= ~eRAttackOne;
			m_animator->SetAnimation(m_model->GetAnimation("RAttack2"), false);
		}
		else if (m_EnemyState & eRAttackTwo)
		{
			m_EnemyState |= eRAttackThree;
			m_EnemyState &= ~eRAttackTwo;
			m_animator->SetAnimation(m_model->GetAnimation("RAttack3"), false);
		}
		else
		{
			m_EnemyState |= eRAttackOne;
			if (m_EnemyState & eGrassHopperDashF | eStep)
			{
				m_animator->SetAnimation(m_model->GetAnimation("GrassDashRAttack"), false);
			}
			else
			{
				m_animator->SetAnimation(m_model->GetAnimation("RAttack1"), false);
			}
		}
		m_EnemyState &= ~eLAttack;
		m_bMove = true;
		m_target.lock()->SetAttackHit(false);
		m_target.lock()->SetDefenseSuc(false);
	}

	if (!(m_EnemyState & (eRAttack | eLAttack)))
	{
		m_EnemyState |= eLAttackOne;
		m_EnemyState &= ~eRAttack;
		m_bAttackAnimeDelay = false;
		m_bAttackAnimeCnt = true;
		m_attackAnimeCnt = 0;
		m_attackAnimeDelayCnt = 0;
		m_bMove = true;
		m_target.lock()->SetAttackHit(false);
		m_target.lock()->SetDefenseSuc(false);
		if (m_EnemyState & eGrassHopperDashF | eStep)
		{
			m_animator->SetAnimation(m_model->GetAnimation("LAttack1"), false);
		}
		else
		{
			m_animator->SetAnimation(m_model->GetAnimation("LAttack1"), false);
		}
	}
}

