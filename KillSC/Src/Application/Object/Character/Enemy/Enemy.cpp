#include "Enemy.h"
#include "../../../Camera/GameCamera/GameCamera.h"
#include "../../../Scene/SceneManager.h"
#include "../../Weapon/Hopper/Hopper.h"
#include "../../Weapon/Scopion/Scopion.h"
#include "../Player/Player.h"

// 初期化
void Enemy::Init(std::weak_ptr<json11::Json> a_wpJsonObj)
{
	// 行列合成
	m_mWorld = Math::Matrix::Identity;

	m_spModel = std::make_shared<KdModelWork>();
	m_spModel->SetModelData
	(KdAssets::Instance().m_modeldatas.GetData
	("Asset/Models/Enemy/BossEnemy.gltf"));
	/// 当たり判定初期化
	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape
	("EnemyModel", m_spModel, KdCollider::TypeBump | KdCollider::TypeDamage | KdCollider::TypeAttackDec);

	m_pDebugWire = std::make_unique<KdDebugWireFrame>();

	m_weaponType |= scorpion;
	m_weaponType |= lScorpion;
	m_wantToMoveState = wNone;
	m_attackHitImmediatelyAfter = false;
	m_rightWeaponNumber = 1;
	m_leftWeaponNumber = 1;

	m_hitStopCnt = 0;
	m_hitColorChangeTimeCnt = 0;
	m_hitMoveSpd = 0.0f;

	m_enemyAirborneTimetoBeCnt = ENEMY_AIRBORNE_TIME_TO_BE_CNT_VAL;

	m_grassSuccessionDelayCnt = 0;

	m_bMantisAttack = false;

	m_bMantisPossAng = false;
	m_bLose = false;
	m_hasDeTime = 0;

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

	m_weaknesSuccessionHitCnt = 0;
	m_addWeaknesSuccessionHitCntTime = 0;

	m_addRotationAttackDistToPlayerTime = 0;
	m_notHumanoidEnemyState = stand;
	m_attackAnimeCnt = 0;

	m_attackDelayTime = 0;
	m_bEnemy = true;
	m_bLethalDamageToKickOrPantciOrDashAttackHit = false;
	m_addCenterVal = {};

	m_wpJsonObj = a_wpJsonObj;

	CharacterBase::Init();
}

// 更新＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
// 更新処理
void Enemy::Update()
{
	if (SceneManager::Instance().GetSceneType() == SceneManager::SceneType::tutorial)
	{
		if (m_bFirstUpdate)
		{
			m_mpObj = (*m_wpJsonObj.lock())["TutorialEnemy"].object_items();
			m_mWorldRot.y = static_cast<float>(m_mpObj["InitWorldRotationY"].number_value());
			m_addCenterVal = { static_cast<float>(m_mpObj["AddCenterVal"][0].number_value()),
							   static_cast<float>(m_mpObj["AddCenterVal"][1].number_value()),
							   static_cast<float>(m_mpObj["AddCenterVal"][2].number_value()) };

			m_weaponList.clear();
		}

		TutorialUpdate();
	}
	else
	{
		if (m_bLose)
		{
			return;
		}

#ifdef _DEBUG
		// debugキー
		if (GetAsyncKeyState('P') & 0x8000)
		{
			m_pos = Math::Vector3::Zero;
		}
#endif
		// debugキー
		if (GetAsyncKeyState('K') & 0x8000)
		{
			m_vForce = 0;
		}

		m_vForce -= m_graduallyTorionDecVal;

		if (!m_bLethalDamageToKickOrPantciOrDashAttackHit)
		{
			if (m_vForce <= 0)
			{
				m_vForce = 0;
				if (!m_bDeath)
				{
					m_bDeath = true;
					m_bMove = true;
					m_charaState = idle;
					m_wantToMoveState = wNone;
					m_spAnimator->SetAnimation(m_spModel->GetAnimation("Death"), false);

				}
			}

			if (m_endurance <= 0)
			{
				m_endurance = 0;
				if (!m_bDeath)
				{
					m_bDeath = true;
					m_bMove = true;
					m_charaState = idle;
					m_wantToMoveState = wNone;
					m_spAnimator->SetAnimation(m_spModel->GetAnimation("Death"), false);
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
		else if (m_enemyType & EnemyType::coarseFishEnemy)
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

	OverStageChaeck();

	if (!m_bWaponDrawPosible)
	{
		if (SceneManager::Instance().GetSceneType() != SceneManager::SceneType::tutorial)
		{
			m_bWaponDrawPosible = true;
		}
	}

	if (m_invincibilityTimeCnt > 0)
	{
		--m_invincibilityTimeCnt;
	}

	if (m_hitColorChangeTimeCnt > 0)
	{
		--m_hitColorChangeTimeCnt;
	}


	if (m_charaState & hit)
	{
		m_pos.y -= m_gravity;

		if (m_charaState & cutRaiseHit)
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
			if (m_charaState & blowingAwayHit)
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

			SpeedyMoveWallHitChack(m_hitMoveSpd, m_knockBackVec);
			m_pos += m_knockBackVec * m_hitMoveSpd;
		}

		if (m_hitStopCnt == 0)
		{
			if (m_charaState & (nomalHit | cutRaiseHit))
			{
				if (!(m_charaState & idle))
				{
					m_spAnimator->SetAnimation(m_spModel->GetAnimation("IdleA"), false);
				}
				m_charaState = idle;
				m_wantToMoveState = wNone;
				if (SceneManager::Instance().GetSceneType() != SceneManager::SceneType::tutorial)
				{
					Brain();
				}
			}
			else if (m_charaState & blowingAwayHit)
			{
				if (!m_bLethalDamageToKickOrPantciOrDashAttackHit)
				{
					if (!m_bBlowingAwayHitB)
					{
						m_spAnimator->SetAnimation(m_spModel->GetAnimation("BlowingAwayRise"), false);
					}
					else
					{
						m_bBlowingAwayHitB = false;
						m_spAnimator->SetAnimation(m_spModel->GetAnimation("IaiKiriRise"), false);
					}
					m_charaState = blowingAwayRise;
				}
				else
				{
					m_bDeath = true;
					m_bMove = true;
					m_charaState = idle;
					m_wantToMoveState = wNone;
				}
			}
			else if (m_charaState & iaiKiriHit)
			{
				if (!(m_charaState & idle))
				{
					m_spAnimator->SetAnimation(m_spModel->GetAnimation("IaiKiriRise"), false);
				}
				m_charaState = iaiKiriRise;
			}
			m_hitStopCnt = 0;
			m_attackHitImmediatelyAfter = true;
			m_hitMoveSpd = 0.0f;
		}
	}
	else if (m_charaState & rise)
	{
		m_bMove = true;
		if (m_spAnimator->IsAnimationEnd())
		{
			if (!(m_charaState & idle))
			{
				m_spAnimator->SetAnimation(m_spModel->GetAnimation("IdleA"), false);
			}
			m_charaState = idle;
			m_wantToMoveState = wNone;
		}
	}

	if (!(m_enemyType & wimpEnemyTypeOne))
	{
		if (!(m_charaState & (grassHopperDash | grassHopperDashUp | hit | step)))
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

	// ========================================
	// 当たり判定
	// ========================================
	CollisionUpdate();

	if ((m_charaState & rlAttackRush) && m_attackAnimeCnt >= m_mpObj["RushLastAttackPointTime"].int_value())
	{
		KickHitAttackChaeck(m_target.lock(), this);
	}

	if (m_bShotBeam || m_bShotEnergyBullet)
	{
		if (m_bBeamHitStart)
		{
			m_beamRange += static_cast<float>(m_mpObj["EnergyBeemSpeed"].number_value());

			if (m_beamRange > static_cast<float>(m_mpObj["EnergyBeemLongestVal"].int_value()))
			{
				m_beamRange = static_cast<float>(m_mpObj["EnergyBeemLongestVal"].number_value());
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

		node = m_spModel->FindNode("LegAttackPoint");
		mat = node->m_worldTransform * m_mWorld;
		mat._42 += 0.7f;
		m_pDebugWire->AddDebugSphere
		(
			mat.Translation(),
			0.3f,
			{ 0,0,1,1 }
		);

		node = m_spModel->FindNode("LegAttackHitPoint");
		mat = node->m_worldTransform * m_mWorld;
		mat._42 += 0.7f;
		m_pDebugWire->AddDebugSphere
		(
			mat.Translation(),
			0.3f,
			{ 0,0,1,1 }
		);

		node = m_spModel->FindNode("LegAttackHitPointTwo");
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

// humanBossEnemyの更新処理
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

		Math::Vector3 addCenterVal = { static_cast<float>(m_mpObj["AddCenterVal"][0].number_value()),
										  static_cast<float>(m_mpObj["AddCenterVal"][1].number_value()),
										  static_cast<float>(m_mpObj["AddCenterVal"][2].number_value()) };
		m_addCenterVal = addCenterVal;
		m_rockOnPos = addCenterVal;

		Math::Vector3 AddGrassDashEffectPosVal = { static_cast<float>(m_mpObj["AddGrassDashEffectPosVal"][0].number_value()),
												   static_cast<float>(m_mpObj["AddGrassDashEffectPosVal"][1].number_value()),
												   static_cast<float>(m_mpObj["AddGrassDashEffectPosVal"][2].number_value()) };
		m_addGrassDashEffectPosVal = AddGrassDashEffectPosVal;

		m_addHpPosY = static_cast<float>(m_mpObj["AddHpPosY"].number_value());

		for (auto& WeaList : m_weaponList)
		{
			WeaList->SetOwner(shared_from_this());
		}

		if (SceneManager::Instance().GetSceneType() == SceneManager::SceneType::challenge)
		{
			m_vForce = (float)m_mpObj["ChallengeVforce"].number_value();
			m_endurance = (float)m_mpObj["ChallengeEndurance"].number_value();
			m_maxEndurance = m_endurance;
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

	if (!(m_charaState & (hit | rise)))
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


			if (!(m_charaState & (fall | jump)) && !m_bDeath)
			{
				m_bMove = false;
			}

			if (m_wantToMoveState & wNone && !m_bDeath)
			{
				Brain();
			}
			else if (!m_bDeath)
			{
				if (vTarget.Length() >= static_cast<float>(m_mpObj["PlayerToEnemyDistBrain"].number_value()) && !(m_charaState & (grassHopperDash & ~grassHopperDashB) | grassHopperDashUp | step | hasDefense))
				{
					Brain();
				}
			}

			if (m_mpObj["RightWeaponScopionNum"].int_value() == m_rightWeaponNumber)
			{
				m_weaponType |= scorpion;
				m_weaponType &= ~grassHopper;
			}
			else if (m_mpObj["RightWeaponGrassHopperNum"].int_value() == m_rightWeaponNumber)
			{
				m_weaponType |= grassHopper;
				m_weaponType &= ~scorpion;
			}

			if (m_mpObj["LeftWeaponScopionNum"].int_value() == m_leftWeaponNumber)
			{
				m_weaponType |= lScorpion;
				m_weaponType &= ~lGrassHopper;
			}
			else if (m_mpObj["LeftWeaponGrassHopperNum"].int_value() == m_leftWeaponNumber)
			{
				m_weaponType |= lGrassHopper;
				m_weaponType &= ~lScorpion;
			}

			if (!(m_wantToMoveState & WantToMoveState::wNone))
			{
				if (!(m_charaState & hasDefense))
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
						if (m_charaState & defense)
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
									if (m_weaponType & grassHopper)
									{
										if (m_charaState & grassHopperDashF)
										{
											ScorpionAttackDecision();
										}
										else
										{
											GrassMoveVecDecision();
										}
									}
									else if (m_weaponType & lGrassHopper)
									{
										if (m_charaState & grassHopperDashF)
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
									if (m_weaponType & grassHopper)
									{
										if (m_charaState & grassHopperDashF)
										{
											ScorpionAttackDecision();
										}
										else
										{
											GrassMoveVecDecision();
										}
									}
									else if (m_weaponType & lGrassHopper)
									{
										if (m_charaState & grassHopperDashF)
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
									if (m_weaponType & grassHopper)
									{
										if (m_charaState & grassHopperDashF)
										{
											ScorpionAttackDecision();
										}
										else
										{
											GrassMoveVecDecision();
										}
									}
									else if (m_weaponType & lGrassHopper)
									{
										if (m_charaState & grassHopperDashF)
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
									if (m_weaponType & grassHopper)
									{
										if ((m_charaState & grassHopperDashF) && (m_rGrassHopperTime <= 80))
										{
											ScorpionAttackDecision();
										}
										else
										{
											GrassMoveVecDecision();
										}
									}
									else if (m_weaponType & lGrassHopper)
									{
										if ((m_charaState & grassHopperDashF) && (m_lGrassHopperTime <= 80))
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
									if (m_weaponType & grassHopper)
									{
										if (m_charaState & grassHopperDashF)
										{
											ScorpionAttackDecision();
										}
										else
										{
											GrassMoveVecDecision();
										}
									}
									else if (m_weaponType & lGrassHopper)
									{
										if (m_charaState & grassHopperDashF)
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
						if (!(m_charaState & step))
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

				if (!(m_charaState & (grassHopperDash | grassHopperDashUp | step)))
				{
					if (m_charaState & (lAttack | rAttack | rlAttack | rlAttackRush | mantis))
					{
						ScorpionAttackMove();
					}
					else if (m_charaState & run | m_charaState & jump)
					{
						NormalMove();
					}
					else if (m_charaState & hasDefense)
					{
						HasDefenseMove();
					}
				}
				else if (m_charaState & (grassHopperDash | grassHopperDashUp))
				{
					GrassMove();
				}
				else if (m_charaState & step)
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

				if (!(m_charaState & (fall | jump)))
				{
					m_charaState = idle;
					m_spAnimator->SetAnimation(m_spModel->GetAnimation("IdleA"));
				}
			}
		}
	}
}

// BossEnemyTypeOneの更新処理
void Enemy::BossEnemyTyepOneUpdate()
{
	if (m_bFirstUpdate)
	{
		m_mpObj = (*m_wpJsonObj.lock())["BossEnemyTypeOne"].object_items();
		m_weaponList.clear();
		m_weaponType |= lWeaponNone | weaponNone;
		m_weaponType &= lWeaponNone | weaponNone;

		m_leftWeaponNumber = 0;
		m_rightWeaponNumber = 0;

		m_vForce = static_cast<float>(m_mpObj["Vforce"].number_value());
		m_endurance = static_cast<float>(m_mpObj["Endurance"].number_value());
		m_maxEndurance = m_endurance;
		if (!(m_charaState & run))
		{
			m_spAnimator->SetAnimation(m_spModel->GetAnimation("RUN"));
		}
		m_charaState = run;
		m_addRotationAttackDistToPlayerRadius = 42.5f;
		Math::Vector3 addCenterVal = { static_cast<float>(m_mpObj["AddCenterVal"][0].number_value()),
										  static_cast<float>(m_mpObj["AddCenterVal"][1].number_value()),
										  static_cast<float>(m_mpObj["AddCenterVal"][2].number_value()) };
		m_addCenterVal = addCenterVal;
		m_addHpPosY = static_cast<float>(m_mpObj["AddHpPosY"].number_value());
		return;
	}

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
		if (m_addWeaknesSuccessionHitCntTime > 0)
		{
			--m_addWeaknesSuccessionHitCntTime;
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
				if (!m_bDeath)
				{
					m_addRotationAttackDistToPlayerTime = 0;
					m_notHumanoidEnemyState = rotationAttack;
					m_spAnimator->SetAnimation(m_spModel->GetAnimation("RotationAttack"), false);
				}
			}


			if (m_wantToMoveState & wNone && !m_bDeath && !m_bShotEnergyBullet && !m_bShotBeam)
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

				if (m_charaState & run | m_charaState & jump)
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
	const KdModelWork::Node* node = nullptr;
	Math::Matrix mat = Math::Matrix::Identity;

	node = m_spModel->FindNode("HitPoint");
	mat = node->m_localTransform;
	Math::Vector3 addPos = Math::Vector3::Transform({ 0,0,1 }, Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
	addPos *= static_cast<float>(m_mpObj["AddRockOnPos"].number_value());
	m_rockOnPos = { mat._41 + addPos.x,mat._42 + static_cast<float>(m_mpObj["AddRockOnPosY"].number_value()),mat._43 + addPos.z };

#ifdef _DEBUG
	m_pDebugWire->AddDebugSphere
	(
		mat.Translation(),
		6.0f,
		{ 0,0,0,1 }
	);
#endif

}

// 人間型の弱い敵の更新処理
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

		m_weaponType |= lWeaponNone | scorpion;
		m_weaponType &= lWeaponNone | scorpion;

		m_leftWeaponNumber = 0;
		m_rightWeaponNumber = 1;

		for (auto& WeaList : m_weaponList)
		{
			WeaList->SetOwner(shared_from_this());
		}

		m_vForce = (float)m_mpObj["Vforce"].number_value();
		m_endurance = (float)m_mpObj["Endurance"].number_value();
		m_maxEndurance = m_endurance;

		m_enemyType = Enemy::EnemyType::coarseFishEnemy;

		Math::Vector3 addCenterVal = { static_cast<float>(m_mpObj["AddCenterVal"][0].number_value()),
										  static_cast<float>(m_mpObj["AddCenterVal"][1].number_value()),
										  static_cast<float>(m_mpObj["AddCenterVal"][2].number_value()) };
		m_addCenterVal = addCenterVal;
		m_rockOnPos = addCenterVal;
		m_addHpPosY = static_cast<float>(m_mpObj["AddHpPosY"].number_value());
		return;
	}

	if (m_coarseFishEnemyAttackDelayCnt > 0)
	{
		--m_coarseFishEnemyAttackDelayCnt;
	}

	if (!(m_charaState & (hit | rise)))
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


			if (!(m_charaState & (fall | jump)) && !m_bDeath)
			{
				m_bMove = false;
			}

			if (m_wantToMoveState & wNone && !m_bDeath)
			{
				Brain();
			}
			else if (!m_bDeath)
			{
				if (vTarget.Length() >= 20.0f && !(m_charaState & (grassHopperDash & ~grassHopperDashB) | grassHopperDashUp))
				{
					Brain();
				}
			}

			if (!(m_wantToMoveState & WantToMoveState::wNone))
			{
				if (!(m_charaState & hasDefense))
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

				if (m_charaState & (lAttack | rAttack | rlAttack | rlAttackRush | mantis))
				{
					ScorpionAttackMove();
				}
				else if (m_charaState & run | m_charaState & jump)
				{
					NormalMove();
				}
			}

			if (!m_bMove)
			{
				if (!(m_charaState & (fall | jump)))
				{
					m_charaState = idle;
					m_spAnimator->SetAnimation(m_spModel->GetAnimation("IdleA"));
				}
			}
		}
	}
}

// モンスター型の弱い敵の更新処理
void Enemy::WimpEnemyTypeOneUpdate()
{
	if (m_bFirstUpdate)
	{
		m_mpObj = (*m_wpJsonObj.lock())["WimpEnemyTypeOne"].object_items();

		m_weaponList.clear();
		m_weaponType |= lWeaponNone | weaponNone;
		m_weaponType &= lWeaponNone | weaponNone;

		m_leftWeaponNumber = 0;
		m_rightWeaponNumber = 0;

		m_vForce = (float)m_mpObj["Vforce"].number_value();
		m_endurance = (float)m_mpObj["Endurance"].number_value();
		m_maxEndurance = m_endurance;

		m_spAnimator->SetAnimation(m_spModel->GetAnimation("IdleA"));
		m_charaState = idle;

		Math::Vector3 addCenterVal = { static_cast<float>(m_mpObj["AddCenterVal"][0].number_value()),
										  static_cast<float>(m_mpObj["AddCenterVal"][1].number_value()),
										  static_cast<float>(m_mpObj["AddCenterVal"][2].number_value()) };
		m_addCenterVal = addCenterVal;
		m_addHpPosY = static_cast<float>(m_mpObj["AddHpPosY"].number_value());
		return;
	}


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

	if (m_wantToMoveState & wNone && !m_bDeath && !m_bShotEnergyBullet && !m_bShotBeam)
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

		if (m_charaState & run | m_charaState & jump)
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

	const KdModelWork::Node* node = nullptr;
	Math::Matrix mat = Math::Matrix::Identity;

	node = m_spModel->FindNode("HitPoint");
	mat = node->m_localTransform;
	Math::Vector3 addPos = Math::Vector3::Transform({ 0,0,1 }, Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
	addPos *= static_cast<float>(m_mpObj["AddRockOnPos"].number_value());
	m_rockOnPos = { mat._41 + addPos.x,mat._42 - static_cast<float>(m_mpObj["SubRockOnPosY"].number_value()),mat._43 + addPos.z };

#ifdef _DEBUG
	m_pDebugWire->AddDebugSphere
	(
		{ mat._41, mat._42 , mat._43 },
		0.325f,
		{ 0,0,0,1 }
	);
#endif

}

// 回転を更新する処理
void Enemy::UpdateRotate(Math::Vector3& a_srcMoveVec)
{
	if (m_bDeath)return;
	// 今向いてる方向のベクトル
	Math::Vector3 nowDir = m_mWorld.Backward();

	// 移動方向のベクトル
	Math::Vector3 targetDir = a_srcMoveVec;

	nowDir.Normalize();
	targetDir.Normalize();

	float ang = DotProductCalculation(nowDir, targetDir);

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

			node = m_spModel->FindNode("HitPoint");
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

//　チュートリアル時の更新処理
void Enemy::TutorialUpdate()
{
	if (KdInputManager::Instance().IsPress("tutorialEnemyPosInit"))
	{
		m_pos = { Math::Vector3::Zero };
	}
}

// 更新後の更新処理
void Enemy::PostUpdate()
{
	if (!m_spAnimator) return;

	if (!m_bDeath)
	{
		if (m_gravity > 0)
		{
			if (!(m_charaState & (hit | hasDefense)))
			{
				if (!(m_charaState & fall))
				{
					m_spAnimator->SetAnimation(m_spModel->GetAnimation("FallA"), false);
				}
				m_charaState = fall;
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

		if (!(m_charaState & (lAttack | rAttack | rlAttack | rlAttackRush)) && !(m_notHumanoidEnemyState & rotationAttack))
		{
			m_spAnimator->AdvanceTime(m_spModel->WorkNodes());
			m_spModel->CalcNodeMatrices();

			if (m_charaState & run)
			{
				++m_runAnimeCnt;
				if (m_runAnimeCnt == m_mpObj["FootfallPointMoment"][0].int_value())
				{
					KdAudioManager::Instance().Play3D("Asset/Audio/SE/FootstepsConcrete2.wav", { m_pos.x,m_pos.y + static_cast<float>(m_mpObj["AddBottomYVal"].number_value()),m_pos.z });
				}
				else if (m_runAnimeCnt == m_mpObj["FootfallPointMoment"][1].int_value())
				{
					KdAudioManager::Instance().Play3D("Asset/Audio/SE/FootstepsConcrete2.wav", { m_pos.x,m_pos.y + static_cast<float>(m_mpObj["AddBottomYVal"].number_value()),m_pos.z });
				}

				if (m_runAnimeCnt == m_mpObj["LastRunAnimationTime"].int_value())
				{
					m_runAnimeCnt = 0;
				}
			}
		}
		else if (m_charaState & (lAttack | rAttack))
		{
			if (!m_bAttackAnimeDelay)
			{
				m_attackAnimeCnt++;
				if (m_charaState & (lAttackOne | lAttackTwo | rAttackOne | rAttackTwo))
				{
					if (m_attackAnimeCnt == m_mpObj["AttackOneOrTwoShakenMoment"].int_value())
					{
						KdAudioManager::Instance().Play3D("Asset/Audio/SE/Swishes - SWSH 40, Swish, Combat, Weapon, Light.wav", m_pos + m_addCenterVal);
					}

					if (m_attackAnimeCnt == m_mpObj["LastAttackAnimationMoment"].int_value())
					{
						m_bAttackAnimeDelay = true;
						m_attackAnimeDelayCnt = m_mpObj["MaxAttackAnimeDelayCnt"].int_value();
					}
				}

				if (m_charaState & (lAttackThree | rAttackThree))
				{
					if (m_attackAnimeCnt == m_mpObj["AttackThreeShakenMoment"].int_value())
					{
						KdAudioManager::Instance().Play3D("Asset/Audio/SE/Swishes - SWSH 40, Swish, Combat, Weapon, Light.wav", m_pos + m_addCenterVal);
					}
				}
			}

			m_spAnimator->AdvanceTime(m_spModel->WorkNodes());
			m_spModel->CalcNodeMatrices();
		}
		else if (m_charaState & (rlAttack | rlAttackRush))
		{
			if (!m_bAttackAnimeDelay)
			{
				m_attackAnimeCnt++;
				if (m_charaState & rlAttackOne)
				{
					if (m_attackAnimeCnt == m_mpObj["RLAttackOneShakenFirstMoment"].int_value() || m_attackAnimeCnt == m_mpObj["RLAttackOneShakenSecondMoment"].int_value())
					{
						KdAudioManager::Instance().Play3D("Asset/Audio/SE/Swishes - SWSH 40, Swish, Combat, Weapon, Light.wav", m_pos + m_addCenterVal);
					}
				}
				else if (m_charaState & rlAttackTwo)
				{
					if (m_attackAnimeCnt == m_mpObj["RLAttackTwoShakenFirstMoment"].int_value() || m_attackAnimeCnt == m_mpObj["RLAttackTwoShakenSecondMoment"].int_value())
					{
						KdAudioManager::Instance().Play3D("Asset/Audio/SE/Swishes - SWSH 40, Swish, Combat, Weapon, Light.wav", m_pos + m_addCenterVal);
					}
				}
				else if (m_charaState & rlAttackThree)
				{
					if (m_attackAnimeCnt == m_mpObj["RLAttackThreeShakenFirstMoment"].int_value() || m_attackAnimeCnt == m_mpObj["RLAttackThreeShakenSecondMoment"].int_value())
					{
						KdAudioManager::Instance().Play3D("Asset/Audio/SE/Swishes - SWSH 40, Swish, Combat, Weapon, Light.wav", m_pos + m_addCenterVal);
					}
				}
				else if (m_charaState & rlAttackRush)
				{
					for (int i = 0; i < 7; ++i)
					{
						if (m_attackAnimeCnt == m_mpObj["RLAttackRushShakenMoment"][i].int_value())
						{
							KdAudioManager::Instance().Play("Asset/Audio/SE/Swishes - SWSH 40, Swish, Combat, Weapon, Light.wav");
							break;
						}
					}
				}


				if (m_charaState & rlAttackOne)
				{
					if (m_attackAnimeCnt == 20)
					{
						m_bAttackAnimeDelay = true;
						m_attackAnimeDelayCnt = 10;
					}
				}
				else if (m_charaState & rlAttackTwo)
				{
					if (m_attackAnimeCnt == 20)
					{
						m_bAttackAnimeDelay = true;
						m_attackAnimeDelayCnt = 10;
					}
				}
				else if (m_charaState & rlAttackThree)
				{
					if (m_attackAnimeCnt == 40)
					{
						m_bAttackAnimeDelay = true;
						m_attackAnimeDelayCnt = 10;
					}
				}
			}

			m_spAnimator->AdvanceTime(m_spModel->WorkNodes());
			m_spModel->CalcNodeMatrices();
		}
		else if (m_notHumanoidEnemyState & rotationAttack)
		{
			++m_attackAnimeCnt;
			m_spAnimator->AdvanceTime(m_spModel->WorkNodes());
			m_spModel->CalcNodeMatrices();
		}
	}
	else
	{
		m_spAnimator->AdvanceTime(m_spModel->WorkNodes());
		m_spModel->CalcNodeMatrices();
		m_bMove = true;

		if (!KdEffekseerManager::GetInstance().IsPlaying("BailOutEnemy.efk"))
		{
			if (m_bLose)
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

		if (m_spAnimator->IsAnimationEnd())
		{
			if (!m_bLose)
			{
				m_bLose = true;
				KdEffekseerManager::GetInstance().
					Play("BailOutEnemy.efk", { m_pos.x,m_pos.y + m_addCenterVal.y,m_pos.z });
				KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("BailOutEnemy.efk"); // これでループしない
				Math::Matrix efcMat = Math::Matrix::CreateScale(static_cast<float>(m_mpObj["BailOutEffectScale"].number_value())) * Math::Matrix::CreateTranslation({ m_pos.x,m_pos.y + 0.9f,m_pos.z });
				KdEffekseerManager::GetInstance().SetWorldMatrix("BailOutEnemy.efk", efcMat);

				if (m_enemyType & bossEnemyTypeOne)
				{
					KdAudioManager::Instance().Play("Asset/Audio/SE/BossEnemyTypeOneDeath.wav");
				}
				else if (m_enemyType & humanBossEnemy)
				{
					KdAudioManager::Instance().Play("Asset/Audio/SE/HumanCharacterDeath.wav");
				}
				else
				{
					KdAudioManager::Instance().Play3D("Asset/Audio/SE/CoarseFishEnemyAndWinpEnemyDeath.wav", { m_pos.x,m_pos.y + m_addCenterVal.y,m_pos.z });
				}
			}
		}
	}
}
// ＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝

// 当たり判定＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
// オブジェクトとの当たり判定
void Enemy::CollisionUpdate()
{
	if (m_enemyType & (wimpEnemyTypeOne | bossEnemyTypeOne))
	{
		MonsterTypeEnemyRayCollisionUpdate();
		MonsterTypeEnemySphereCollisionUpdate();
	}
	else
	{
		HumanTypeEnemyRayCollisionUpdate();
		HumanTypeEnemySphereCollisionUpdate();
	}
}

void Enemy::HumanTypeEnemyRayCollisionUpdate()
{
	// レイ判定用に変数を作成
	KdCollider::RayInfo rayInfo;
	HitInfo hitInfo;
	float addBottmoY = static_cast<float>(m_mpObj["AddBottomYVal"].number_value());
	if (!(m_charaState & (grassHopperDash | grassHopperDashUp)))
	{
		float enableStepHight = static_cast<float>(m_mpObj["EnableStepHight"].number_value());
		hitInfo = RayColisionCheck(Math::Vector3(m_pos.x, m_pos.y + addBottmoY + enableStepHight, m_pos.z), { 0,-1,0 }, m_gravity + enableStepHight, KdCollider::TypeGround);
	}
	else
	{
		rayInfo.m_dir = m_grassHopperDashDir;
		hitInfo = RayColisionCheck(Math::Vector3(m_pos.x, m_pos.y + addBottmoY, m_pos.z), m_grassHopperDashDir, 0.0f, KdCollider::TypeGround);
	}

	if (hitInfo.hit)
	{
		if (!(m_charaState & (grassHopperDash | grassHopperDashUp)))
		{
			m_pos = hitInfo.groundPos + Math::Vector3(0, -static_cast<float>(m_mpObj["AddBottomYVal"].number_value()), 0);
			m_gravity = 0;

			if (m_charaState & (fall | jump) && !m_bDeath)
			{
				m_bMove = false;
				if (!(m_charaState & idle))
				{
					m_spAnimator->SetAnimation(m_spModel->GetAnimation("IdleA"), false);
				}
				m_charaState = idle;
				m_wantToMoveState = wNone;
			}
		}
		else
		{
			m_gravity = 0;
			m_charaState = fall;
			m_rGrassHopperTime = 0;
			m_lGrassHopperTime = 0;
			m_bMove = false;
			m_grassHopperDashDir = {};
			m_dashSpd = 0.0f;
		}
	}

	if (!m_target.expired())
	{
		Math::Vector3 dir = (m_target.lock()->GetPos() + Math::Vector3(0, 1.5f, 0)) - (m_pos + m_addCenterVal);
		dir.Normalize();
		hitInfo = RayColisionCheck(m_pos + m_addCenterVal, dir, dir.Length(), KdCollider::TypeGround);
		if (hitInfo.hit)
		{
			m_bEnemyBetweenPlayer = true;
			m_enemyBetweenPlayerHitPos = hitInfo.groundPos;
		}
		else
		{
			m_bEnemyBetweenPlayer = false;

			if (m_charaState & (grassHopperDashL | grassHopperDashR))
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

void Enemy::HumanTypeEnemySphereCollisionUpdate()
{
	KdCollider::SphereInfo sphereInfo;
	// 球の中心位置を設定
	HitInfo hitInfo;
	sphereInfo.m_sphere.Center = m_pos + m_addCenterVal;
	if (!(m_charaState & (grassHopperDash | grassHopperDashUp | step)))
	{
		hitInfo = SphereColisionCheck(m_pos + m_addCenterVal, static_cast<float>(m_mpObj["TypeGroundToHitSphereRadius"].number_value()), KdCollider::TypeGround);
	}
	else
	{
		hitInfo = SphereColisionCheck(m_pos + m_addCenterVal, static_cast<float>(m_mpObj["TypeGroundToHitSphereRadiusIsGrassDash"].number_value()), KdCollider::TypeGround);
	}

	if (hitInfo.hit)
	{
		// 球とモデルが当たっている
		m_pos += (hitInfo.hitDir * hitInfo.maxOverLap);
	}

	if (!m_target.expired() && m_target.lock()->GetCharaState() & (defense))
	{
		HitInfo hitInfoComparison;
		// 球と当たり判定 
		for (auto& obj : m_target.lock()->GetWeaponList())
		{
			if (!(m_charaState & (grassHopperDash | grassHopperDashUp | step)) || !(m_wantToMoveState & (wEscape | wDashAttack | wGrassDash | wAvoidance)))
			{
				hitInfo = SphereColisionCheck(m_pos + m_addCenterVal, static_cast<float>(m_mpObj["TypeGardToHitSphereRadius"].number_value()), KdCollider::TypeGard, obj);
			}
			else
			{
				hitInfo = SphereColisionCheck(m_pos + m_addCenterVal, static_cast<float>(m_mpObj["TypeGardToHitSphereRadiusIsGrassDash"].number_value()), KdCollider::TypeGard, obj);
			}

			if (hitInfoComparison.maxOverLap < hitInfo.maxOverLap)
			{
				hitInfoComparison.maxOverLap = hitInfo.maxOverLap;
				hitInfoComparison.hit = true;
				hitInfoComparison.hitDir = hitInfo.hitDir;
			}

		}

		if (hitInfoComparison.hit)
		{
			hitInfoComparison.hitDir.y = 0.0f;
			hitInfoComparison.hitDir.Normalize();
			// 球とモデルが当たっている
			m_pos += (hitInfoComparison.hitDir * hitInfoComparison.maxOverLap);
		}
	}

	if (!m_target.expired())
	{
		if (!m_target.lock()->GetBLose() && !(m_target.lock()->GetCharaState() & cutRaiseHit))
		{
			hitInfo = SphereColisionCheck(m_pos + m_addCenterVal, static_cast<float>(m_mpObj["TypeBumpToHitSphereRadius"].number_value()), KdCollider::TypeBump, m_target.lock());

			if (hitInfo.hit)
			{
				hitInfo.hitDir.y = 0.0f;
				hitInfo.hitDir.Normalize();
				// 球とモデルが当たっている
				m_pos += (hitInfo.hitDir * hitInfo.maxOverLap);
			}
		}
	}

	if (!(m_charaState & cutRaiseHit))
	{
		HitInfo hitInfoComparison;
		for (auto& obj : SceneManager::Instance().GetObjList())
		{
			if (!obj->GetBEnemy())continue;
			hitInfo = SphereColisionCheck(m_pos + m_addCenterVal, static_cast<float>(m_mpObj["TypeBumpToHitSphereRadius"].number_value()), KdCollider::TypeBump, obj);

			// 一番近くで当たったものを探す
			if (hitInfoComparison.maxOverLap < hitInfo.maxOverLap)
			{
				hitInfoComparison.maxOverLap = hitInfo.maxOverLap;
				if (hitInfoComparison.maxOverLap <= 0.1f)
				{
					hitInfoComparison.hit = true;
					hitInfoComparison.hitDir = hitInfo.hitDir;
				}
			}
		}

		if (hitInfoComparison.hit)
		{
			hitInfoComparison.hitDir.y = 0.0f;
			hitInfoComparison.hitDir.Normalize();
			// 球とモデルが当たっている
			m_pos += (hitInfoComparison.hitDir * hitInfoComparison.maxOverLap);
		}
	}

	if (m_charaState & (rlAttack | rlAttackRush | CharaState::hit))
	{
		hitInfo = SphereColisionCheck(m_pos + m_addCenterVal, static_cast<float>(m_mpObj["TypeBuriedToHitSphereRadius"].number_value()), KdCollider::TypeBuried);
		if (hitInfo.hit)
		{
			hitInfo.hitDir.y = 0.0f;
			hitInfo.hitDir.Normalize();
			// 球とモデルが当たっている
			m_pos += (hitInfo.hitDir * hitInfo.maxOverLap);
			m_attackMoveSpd = 0.0f;
			m_hitMoveSpd = 0.0f;
		}
	}
}

void Enemy::MonsterTypeEnemyRayCollisionUpdate()
{
	// レイ判定用に変数を作成
	HitInfo hitInfo;
	float enableStepHight = 0.2f;
	hitInfo = RayColisionCheck(Math::Vector3(m_pos.x, m_pos.y + enableStepHight, m_pos.z), { 0,-1,0 }, m_gravity + enableStepHight, KdCollider::TypeGround);

	if (hitInfo.hit)
	{
		//m_pos = groundPos;
		m_pos = hitInfo.groundPos;
		m_gravity = 0;
	}

	hitInfo.Init();
	if (!m_target.expired())
	{
		Math::Vector3 dir = (m_target.lock()->GetPos() + Math::Vector3(0, 1.2f, 0)) - (m_pos + Math::Vector3(0, 0.5f, 0));
		dir.Normalize();
		hitInfo = RayColisionCheck(m_pos + Math::Vector3(0, 0.5f, 0), dir, dir.Length(), KdCollider::TypeGround);
		if (hitInfo.hit)
		{
			m_bEnemyBetweenPlayer = true;
			m_enemyBetweenPlayerHitPos = hitInfo.groundPos;
		}
	}
}

void Enemy::MonsterTypeEnemySphereCollisionUpdate()
{
	KdCollider::SphereInfo sphereInfo;

	HitInfo hitInfo;
	// 球の中心位置を設定
	if (m_enemyType & bossEnemyTypeOne)
	{
		hitInfo = SphereColisionCheck(m_pos + Math::Vector3(0, 25, 0), 15.0f, KdCollider::TypeGround);
	}
	else
	{
		hitInfo = SphereColisionCheck(m_pos + Math::Vector3(0, 3, 0), 2.0f, KdCollider::TypeGround);
	}

	if (hitInfo.hit)
	{
		// 球とモデルが当たっている
		m_pos += (hitInfo.hitDir * hitInfo.maxOverLap);
	}

	if (!m_target.expired() && m_target.lock()->GetCharaState() & (defense) && !(m_enemyType & bossEnemyTypeOne))
	{
		HitInfo hitInfoComparison;

		for (auto& obj : m_target.lock()->GetWeaponList())
		{
			hitInfo = SphereColisionCheck(m_pos + Math::Vector3(0, 3, 0), 0.6f, KdCollider::TypeGard, obj);
			if (hitInfoComparison.maxOverLap < hitInfo.maxOverLap)
			{
				hitInfoComparison.maxOverLap = hitInfo.maxOverLap;
				hitInfoComparison.hit = true;
				hitInfoComparison.hitDir = hitInfo.hitDir;
			}

		}

		if (hitInfoComparison.hit)
		{
			hitInfoComparison.hitDir.y = 0.0f;
			hitInfoComparison.hitDir.Normalize();
			// 球とモデルが当たっている
			m_pos += (hitInfoComparison.hitDir * hitInfoComparison.maxOverLap);
		}
	}

	if (!m_target.expired() && !(m_enemyType & bossEnemyTypeOne))
	{
		if (!m_target.lock()->GetBLose())
		{
			hitInfo = SphereColisionCheck(m_pos + Math::Vector3(0, 2.5f, 0), 2.0f, KdCollider::TypeBump, m_target.lock());
			if (hitInfo.hit)
			{
				hitInfo.hitDir.y = 0.0f;
				hitInfo.hitDir.Normalize();
				// 球とモデルが当たっている
				m_pos += (hitInfo.hitDir * hitInfo.maxOverLap);
			}
		}
	}

	if (m_charaState & (CharaState::hit))
	{
		hitInfo = SphereColisionCheck(m_pos + Math::Vector3(0, 1.5f, 0), 0.4f, KdCollider::TypeBuried);
		if (hitInfo.hit)
		{
			hitInfo.hitDir.y = 0.0f;
			hitInfo.hitDir.Normalize();
			// 球とモデルが当たっている
			m_pos += (hitInfo.hitDir * hitInfo.maxOverLap);
			m_attackMoveSpd = 0.0f;
			m_hitMoveSpd = 0.0f;
		}
	}

	if (!(m_charaState & cutRaiseHit) && !(m_enemyType & bossEnemyTypeOne))
	{
		HitInfo hitInfoComparison;
		// 球と当たり判定 	
		for (auto& obj : SceneManager::Instance().GetObjList())
		{
			hitInfo = SphereColisionCheck(m_pos + Math::Vector3(0, 2.5f, 0), 0.65f, KdCollider::TypeBump, obj);
			// 一番近くで当たったものを探す
			if (hitInfoComparison.maxOverLap < hitInfo.maxOverLap)
			{
				hitInfoComparison.maxOverLap = hitInfo.maxOverLap;
				if (hitInfoComparison.maxOverLap <= 0.1f)
				{
					hitInfoComparison.hit = true;
					hitInfoComparison.hitDir = hitInfo.hitDir;
				}
			}
		}

		if (hitInfoComparison.hit)
		{
			hitInfoComparison.hitDir.y = 0.0f;
			hitInfoComparison.hitDir.Normalize();
			// 球とモデルが当たっている
			//m_pos += (hitInfoComparison.hitDir * hitInfoComparison.maxOverLap);
		}
	}

	if (m_enemyType & (bossEnemyTypeOne) && !(m_notHumanoidEnemyState & rotationAttack))
	{
		if (!m_target.expired())
		{
			if (!m_target.lock()->GetBLose())
			{
				hitInfo = SphereColisionCheck(m_pos + Math::Vector3(0, 12, 0), m_addRotationAttackDistToPlayerRadius, KdCollider::TypeDamage, m_target.lock());
			}

			if (hitInfo.hit)
			{
				if (!m_bDeath && !(m_notHumanoidEnemyState & weaknessHit))
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
// 一定速度以上の時にする壁との当たり判定
void Enemy::SpeedyMoveWallHitChack(float& a_moveSpd, Math::Vector3 moveVec)
{
	KdCollider::RayInfo rayInfo;
	rayInfo.m_pos = m_pos + m_addCenterVal;
	rayInfo.m_dir = moveVec;
	static float enableStepHight = static_cast<float>(m_mpObj["EnableStepHight"].number_value());
	rayInfo.m_pos.y += enableStepHight;
	rayInfo.m_range = a_moveSpd;

	rayInfo.m_type = KdCollider::TypeSpeedDec;

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
		if (maxOverLap <= 0.2)
		{
			a_moveSpd -= maxOverLap;
			if (a_moveSpd < 0)
			{
				a_moveSpd = 0;
			}
		}
		else
		{
			a_moveSpd = 0;
		}
	}
}

// ビームの当たり判定
void Enemy::EnemyBeamHitChaeck()
{
	if (m_target.expired())return;
	if (!m_target.lock()->GetAttackHit() && !m_target.lock()->GetDefenseSuc() && m_target.lock()->GetInvincibilityTimeCnt() == 0 && !m_target.lock()->GetBDeath()) // ここになくていいかも
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
			m_target.lock()->BlowingAwayAttackOnHit(m_mWorld.Backward(), this);
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
			rayInfo.m_dir = Math::Vector3::Transform(m_rangedAttackDir, Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(1)));
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
				m_target.lock()->BlowingAwayAttackOnHit(m_mWorld.Backward(), this);
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
					m_target.lock()->BlowingAwayAttackOnHit(m_mWorld.Backward(), this);
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

// エネルギー弾の当たり判定
void Enemy::EnemyEnergyBulletHitChaeck()
{
	if (m_target.expired())return;
	if (!m_target.lock()->GetAttackHit() && m_target.lock()->GetInvincibilityTimeCnt() == 0 && !m_target.lock()->GetBDeath())
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
			m_target.lock()->BlowingAwayAttackOnHit(m_mWorld.Backward(), this);
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

// 回転攻撃の当たり判定
void Enemy::RotationAttackChaeck()
{
	KdCollider::SphereInfo sphereInfo;

	// 球の当たったオブジェクト情報
	std::list<KdCollider::CollisionResult> retSphereList;

	sphereInfo.m_sphere.Center = m_pos + m_addCenterVal;
	sphereInfo.m_sphere.Radius = static_cast<float>(m_mpObj["RotationAttackHitChaeckSphereRadius"].number_value());
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
		if (!m_target.lock()->GetAttackHit() && !m_target.lock()->GetDefenseSuc() && m_target.lock()->GetInvincibilityTimeCnt() == 0 && !m_target.lock()->GetBDeath())
		{
			if (!m_target.lock()->GetBLose())
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
				m_target.lock()->BlowingAwayAttackOnHit(m_mWorld.Backward(), this);
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
// ＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝

// 攻撃を受けた時の処理＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
// 通常攻撃を受けた時の処理
void Enemy::OnHit(Math::Vector3 a_KnocBackvec, CharacterBase* a_pAttackedCharacter)
{
	if (!a_pAttackedCharacter)return;

	m_charaState = nomalHit;
	if (m_target.lock()->GetCharaState() & rlAttackThree)
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
	if (a_pAttackedCharacter->GetCharaState() & (rAttackOne | rlAttackOne))
	{
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("RHit1"), false);
		if (SceneManager::Instance().GetUpdateStopCnt() == 0)
		{
			SceneManager::Instance().SetUpdateStopCnt(5); // これでアップデートを一時止める
		}
	}
	else if (a_pAttackedCharacter->GetCharaState() & (rAttackTwo | rlAttackTwo))
	{
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("RHit2"), false);
		if (SceneManager::Instance().GetUpdateStopCnt() == 0)
		{
			SceneManager::Instance().SetUpdateStopCnt(5); // これでアップデートを一時止める
		}
	}
	else if (a_pAttackedCharacter->GetCharaState() & (lAttackOne | rlAttackThree))
	{
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("LHit1"), false);
		if (SceneManager::Instance().GetUpdateStopCnt() == 0)
		{
			SceneManager::Instance().SetUpdateStopCnt(5); // これでアップデートを一時止める
		}
	}
	else if (a_pAttackedCharacter->GetCharaState() & (lAttackTwo))
	{
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("LHit2"), false);
		if (SceneManager::Instance().GetUpdateStopCnt() == 0)
		{
			SceneManager::Instance().SetUpdateStopCnt(5); // これでアップデートを一時止める
		}
	}

	if (!m_target.lock()->GetBRushRp())
	{
		if (a_pAttackedCharacter->GetCharaState() & rlAttackRush && a_pAttackedCharacter->GetAnimationCnt() < 8)
		{
			m_spAnimator->SetAnimation(m_spModel->GetAnimation("LHit1"), false);
			if (SceneManager::Instance().GetUpdateStopCnt() == 0)
			{
				SceneManager::Instance().SetUpdateStopCnt(2); // これでアップデートを一時止める
			}
		}
		else if (a_pAttackedCharacter->GetCharaState() & rlAttackRush && (a_pAttackedCharacter->GetAnimationCnt() >= 21 && a_pAttackedCharacter->GetAnimationCnt() < 31))
		{
			m_spAnimator->SetAnimation(m_spModel->GetAnimation("LHit2"), false);
			if (SceneManager::Instance().GetUpdateStopCnt() == 0)
			{
				SceneManager::Instance().SetUpdateStopCnt(2); // これでアップデートを一時止める
			}
		}
		else if (a_pAttackedCharacter->GetCharaState() & rlAttackRush && (a_pAttackedCharacter->GetAnimationCnt() >= 31 && a_pAttackedCharacter->GetAnimationCnt() < 41))
		{
			m_spAnimator->SetAnimation(m_spModel->GetAnimation("RHit1"), false);
			if (SceneManager::Instance().GetUpdateStopCnt() == 0)
			{
				SceneManager::Instance().SetUpdateStopCnt(2); // これでアップデートを一時止める
			}
		}
		else if (a_pAttackedCharacter->GetCharaState() & rlAttackRush && (a_pAttackedCharacter->GetAnimationCnt() >= 8 && a_pAttackedCharacter->GetAnimationCnt() < 21))
		{
			m_spAnimator->SetAnimation(m_spModel->GetAnimation("RHit2"), false);
			if (SceneManager::Instance().GetUpdateStopCnt() == 0)
			{
				SceneManager::Instance().SetUpdateStopCnt(2); // これでアップデートを一時止める
			}
		}
	}
	else
	{
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("RHit1"), false);
		if (a_pAttackedCharacter->GetCharaState() & rlAttackRush && a_pAttackedCharacter->GetAnimationCnt() < 93)
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

// 吹き飛ばし攻撃を受けたときの処理
void Enemy::BlowingAwayAttackOnHit(Math::Vector3 a_KnocBackvec, CharacterBase* a_pAttackedCharacter)
{
	if (!a_pAttackedCharacter)return;

	//SceneManager::Instance().SetUpdateStopCnt(10);   // 画面を揺らす時のHtiストップフレーム数
	//SceneManager::Instance().SetScreenVibFrames(15); // 画面を揺らすフレーム数

	m_charaState = blowingAwayHit;
	m_hitStopCnt = 40;
	m_hitColorChangeTimeCnt = (*m_wpJsonObj.lock())["HitColorChangeTimeCnt"].int_value();
	if (a_pAttackedCharacter->GetCharaState() & rlAttackRush && a_pAttackedCharacter->GetAnimationCnt() >= 107)
	{
		m_hitMoveSpd = 0.95f;
		if (SceneManager::Instance().GetUpdateStopCnt() == 0)
		{
			SceneManager::Instance().SetUpdateStopCnt(25); // これでアップデートを一時止める
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
	else if (a_pAttackedCharacter->GetCharaState() & (rAttackOne | lAttackOne) && a_pAttackedCharacter->GetCharaState() & (grassHopperDashF))
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
	else if (a_pAttackedCharacter->GetCharaState() & (rAttackThree | lAttackThree))
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
		float ang = DotProductCalculation(nowVec, a_KnocBackvec);

		if (ang > 90)
		{
			m_spAnimator->SetAnimation(m_spModel->GetAnimation(" BlowingAwayHitB"), false);
		}
		else
		{
			m_spAnimator->SetAnimation(m_spModel->GetAnimation("BlowingAwayHitBB"), false);
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

// 居合切り系の攻撃を受けたときの処理
void Enemy::IaiKiriAttackOnHit(Math::Vector3 a_KnocBackvec)
{
	m_charaState = iaiKiriHit;
	m_hitStopCnt = 40;
	m_hitColorChangeTimeCnt = (*m_wpJsonObj.lock())["HitColorChangeTimeCnt"].int_value();
	m_hitMoveSpd = 0.0f;
	m_invincibilityTimeCnt = (*m_wpJsonObj.lock())["InvincibilityMaxTimeCnt"].int_value();
	m_knockBackVec = a_KnocBackvec;
	m_endurance -= 50.0f;
	m_attackHit = true;
	m_spAnimator->SetAnimation(m_spModel->GetAnimation("IaiKiriAttackHitB"), false);
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

// 切り上げ攻撃を受けたときの処理
void Enemy::CutRaiseOnHit(Math::Vector3 a_KnocBackvec)
{
	m_charaState = cutRaiseHit;
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
	m_spAnimator->SetAnimation(m_spModel->GetAnimation("CutRaiseHit"), false);

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

// 弱点に攻撃されたときの処理
void Enemy::WeaknessOnHit()
{
	SceneManager::Instance().SetUpdateStopCnt(15); // これでアップデートを一時止める

	if (m_enemyType & bossEnemyTypeOne)
	{
		if (m_addWeaknesSuccessionHitCntTime == 0)
		{
			m_weaknesSuccessionHitCnt = 1;
		}
		else
		{
			++m_weaknesSuccessionHitCnt;
		}

		m_addWeaknesSuccessionHitCntTime = ADD_WEAKNES_SEUCCESSION_HIT_CNT_TIME_LIMIT;

		if (m_weaknesSuccessionHitCnt >= m_mpObj["MaxWeaknesSuccessionHitCnt"].int_value())
		{
			m_weaknesSuccessionHitCnt = 0;
			m_addWeaknesSuccessionHitCntTime = 0;
			m_invincibilityTimeCnt = (*m_wpJsonObj.lock())["InvincibilityMaxTimeCnt"].int_value();
			m_hitStopCnt = m_mpObj["MaxWeaknesSuccessionHitStopCnt"].int_value();
			m_spAnimator->SetAnimation(m_spModel->GetAnimation("SuccessionWeaknessOnHit"), false);
		}

		m_hitColorChangeTimeCnt = (*m_wpJsonObj.lock())["HitColorChangeTimeCnt"].int_value();
	}
	else
	{
		m_endurance = 0.0f;
	}
	m_attackHit = true;

	if (m_graduallyTorionDecVal == 0)
	{
		m_graduallyTorionDecVal = static_cast<float>((*m_wpJsonObj.lock())["FristWeaknessHitGraduallyTorionDecVal"].number_value());
	}
	else
	{
		m_graduallyTorionDecVal *= static_cast<float>((*m_wpJsonObj.lock())["AddWeaknessHitGraduallyTorionDecVal"].number_value());
	}
}
// ＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝

// 攻撃が防がれた時の処理＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
void Enemy::HasDefense()
{
	if (m_charaState & (rAttack | rlAttack))
	{
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("RHasDefense"), false);
	}
	else if (m_charaState & lAttack)
	{
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("LHasDefense"), false);
	}

	m_charaState |= hasDefense;
	m_charaState &= hasDefense;
	m_hasDeTime = 30;
	m_bMove = true;
}
// ＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝

// 描画処理＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
// 光の影響を受けるオブジェクトの描画（スキンメッシュ対応）
void Enemy::DrawLit_SkinMesh()
{
	if (m_bLose)return;
	if (!m_spModel) return;

	if (m_invincibilityTimeCnt <= (*m_wpJsonObj.lock())["InvincibilityProgramTimeCnt"][0].int_value() && m_invincibilityTimeCnt > (*m_wpJsonObj.lock())["InvincibilityProgramTimeCnt"][1].int_value() ||
		m_invincibilityTimeCnt <= (*m_wpJsonObj.lock())["InvincibilityProgramTimeCnt"][2].int_value() && m_invincibilityTimeCnt > (*m_wpJsonObj.lock())["InvincibilityProgramTimeCnt"][3].int_value() ||
		m_invincibilityTimeCnt <= (*m_wpJsonObj.lock())["InvincibilityProgramTimeCnt"][4].int_value() && m_invincibilityTimeCnt > (*m_wpJsonObj.lock())["InvincibilityProgramTimeCnt"][5].int_value() ||
		m_invincibilityTimeCnt <= (*m_wpJsonObj.lock())["InvincibilityProgramTimeCnt"][6].int_value() && m_invincibilityTimeCnt > (*m_wpJsonObj.lock())["InvincibilityProgramTimeCnt"][7].int_value() ||
		m_invincibilityTimeCnt <= (*m_wpJsonObj.lock())["InvincibilityProgramTimeCnt"][8].int_value() && m_invincibilityTimeCnt > (*m_wpJsonObj.lock())["InvincibilityProgramTimeCnt"][9].int_value() ||
		m_invincibilityTimeCnt <= (*m_wpJsonObj.lock())["InvincibilityProgramTimeCnt"][10].int_value() && m_invincibilityTimeCnt > (*m_wpJsonObj.lock())["InvincibilityProgramTimeCnt"][11].int_value() ||
		m_invincibilityTimeCnt == (*m_wpJsonObj.lock())["InvincibilityProgramTimeCnt"][12].int_value()
		)return;


	if (m_charaState & defense)
	{
		KdShaderManager::Instance().m_HD2DShader.SetLimLightEnable(true);
		KdShaderManager::Instance().m_HD2DShader.SetLimLight({ static_cast<float>(m_mpObj["StateDefenseMomentLightColor"][0].number_value()),
															   static_cast<float>(m_mpObj["StateDefenseMomentLightColor"][1].number_value()),
															   static_cast<float>(m_mpObj["StateDefenseMomentLightColor"][2].number_value()) },
			static_cast<float>(m_mpObj["StateDefenseMomentLightLevel"].number_value()));
	}

	KdShaderManager::Instance().m_HD2DShader.SetOutLineColor({ 1,0,0 });
	if (m_hitColorChangeTimeCnt == 0)
	{
		KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_spModel, m_mWorld, true);
	}
	else
	{
		Math::Color color = kRedColor;
		KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_spModel, m_mWorld, true, color);
	}
}
// ＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝

// 武器にプレイヤーの情報を渡す処理
void Enemy::SetWeaponToTarget(std::shared_ptr<Player> a_spPlayer)
{
	m_weaponList[0]->SetTarget(a_spPlayer);
	m_weaponList[1]->SetTarget(a_spPlayer);
}

// 行列をセットする処理
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

// モデルとタイプをセットする処理
void Enemy::SetModelAndType(EnemyType a_enemyType)
{
	m_enemyType = a_enemyType;
	const KdModelWork::Node* node = nullptr;
	Math::Matrix mat = Math::Matrix::Identity;

	switch (m_enemyType)
	{
	case coarseFishEnemy:
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData
		(KdAssets::Instance().m_modeldatas.GetData
		("Asset/Models/Enemy/CoarseFishEnemy.gltf"));
		/// 当たり判定初期化
		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape
		("EnemyModel", m_spModel, KdCollider::TypeBump | KdCollider::TypeDamage | KdCollider::TypeAttackDec);
		m_enemyAttackMaxTotal = 2;
		break;
	case wimpEnemyTypeOne:
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData
		(KdAssets::Instance().m_modeldatas.GetData
		("Asset/Models/Enemy/WimpEnemyTypeOne.gltf"));
		/// 当たり判定初期化
		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape
		("EnemyModel", m_spModel, KdCollider::TypeBump | KdCollider::TypeDamage | KdCollider::TypeRideEnemy | KdCollider::TypeAttackDec);

		node = m_spModel->FindNode("HitPoint");
		mat = node->m_worldTransform * Math::Matrix::Identity;
		m_pCollider->RegisterCollisionShape
		("EnemyModelWeakness", { mat._41,mat._42,mat._43 }, 0.325f, KdCollider::TypeWeakness);
		m_enemyAttackMaxTotal = 4;
		m_rockOnPos = { mat._41,mat._42,mat._43 };
		break;
	case bossEnemyTypeOne:
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData
		(KdAssets::Instance().m_modeldatas.GetData
		("Asset/Models/Enemy/BossEnemyTypeOne.gltf"));
		/// 当たり判定初期化
		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape
		("EnemyModel", m_spModel, KdCollider::TypeBump | KdCollider::TypeDamage | KdCollider::TypeRideEnemy | KdCollider::TypeAttackDec);

		node = m_spModel->FindNode("HitPoint");
		mat = node->m_worldTransform * Math::Matrix::Identity;
		m_pCollider->RegisterCollisionShape
		("EnemyModelWeakness", { mat._41,mat._42,mat._43 }, 3.00f, KdCollider::TypeWeakness);
		m_rockOnPos = { mat._41,mat._42,mat._43 };
		break;
	}
}

// 行動選択＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
// ホッパー装備時の動きを決める処理
void Enemy::GrassMoveVecDecision()
{
	Math::Matrix mat = Math::Matrix::Identity;
	Math::Vector3 src = Math::Vector3::Zero;

	if (!m_target.expired())
	{
		src = m_target.lock()->GetPos() - m_pos;
	}

	if (m_rGrassHopperPauCnt == 0)
	{
		if (m_weaponType & grassHopper)
		{
			int calculationResults = 0;
			float y = 0.0f;

			if (!m_target.expired())
			{
				y = m_target.lock()->GetPos().y - m_pos.y;

			}

			switch (m_wantToMoveState)
			{
			case WantToMoveState::wEscape:
				if (m_grassSuccessionDelayCnt != 0)return;
				{	int randNum[5] = { 0,550,150,150,150 };
				calculationResults = SetRandInfo(5, randNum);
				}
				break;
			case WantToMoveState::wDashAttack:
				if (src.Length() <= 2.0f)
				{
					int randNum[5] = { 0,1000,0,0,0 };
					calculationResults = SetRandInfo(5, randNum);
				}
				else
				{
					int randNum[5] = { 1000,0,0,0,0 };
					calculationResults = SetRandInfo(5, randNum);
				}
				break;
			case WantToMoveState::wGrassDash:
				if (m_grassSuccessionDelayCnt != 0)return;
				if (y >= 1.0f)
				{
					int randNum[5] = { 0,0,0,0,1000 };
					calculationResults = SetRandInfo(5, randNum);
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
						float ang = DotProductCalculation(nowVec, toVec);

						if (ang <= 0.1 || ang > 150 && ang <= 180)
						{
							int randNum[5] = { 0,0,1000,0,0 };
							calculationResults = SetRandInfo(5, randNum);
						}
						else if (ang > 0.1 && ang <= 150)
						{

							int randNum[5] = { 1000,0,0,0,0 };
							calculationResults = SetRandInfo(5, randNum);
						}
					}
					else
					{
						int randNum[5] = { 350,150,200,200,100 };
						calculationResults = SetRandInfo(5, randNum);
					}
				}
				break;
			case WantToMoveState::wAvoidance:
				if (m_grassSuccessionDelayCnt != 0)return;
				{
					int randNum[5] = { 50,50,350,350,200 };
					calculationResults = SetRandInfo(5, randNum);
				}
				break;
			}

			Math::Vector3 vec = m_mWorld.Backward();
			Math::Matrix efcMat;

			m_lGrassHopperPauCnt = 120;
			m_lGrassHopperTime = 90;
			m_rGrassHopperTime = 0;
			Math::Vector3 dir;
			switch (calculationResults)
			{
			case 0:
				vec = m_mWorld.Backward();
				dir = Math::Vector3::TransformNormal(Math::Vector3(0, 0, 1), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
				SetGrassDashInfo(dir, grassHopperDashF, m_spModel->GetAnimation("GrassDashFB"), m_mWorldRot.y, 2, vec, m_mWorldRot.y, 0.0f, "GrassHopperLegRPoint", SetGrassDashEffectName(m_enemyNumber));
				break;
			case 1:
				dir = Math::Vector3::TransformNormal(Math::Vector3(0, 0, -1), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
				vec = m_mWorld.Forward();
				SetGrassDashInfo(dir, grassHopperDashB, m_spModel->GetAnimation("GrassDashBB"), m_mWorldRot.y, 2, vec, m_mWorldRot.y, 180.0f, "GrassHopperLegRPoint", SetGrassDashEffectName(m_enemyNumber));
				break;
			case 2:
				dir = Math::Vector3::TransformNormal(Math::Vector3(1, 0, 0), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
				vec = m_mWorld.Right();
				SetGrassDashInfo(dir, grassHopperDashR, m_spModel->GetAnimation("GrassDashRB"), m_mWorldRot.y, 2, vec, m_mWorldRot.y, 90.0f, "GrassHopperLegLPoint", SetGrassDashEffectName(m_enemyNumber));
				break;
			case 3:
				dir = Math::Vector3::TransformNormal(Math::Vector3(-1, 0, 0), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
				vec = m_mWorld.Left();
				SetGrassDashInfo(dir, grassHopperDashL, m_spModel->GetAnimation("GrassDashLB"), m_mWorldRot.y, 2, vec, m_mWorldRot.y, 270.0f, "GrassHopperLegRPoint", SetGrassDashEffectName(m_enemyNumber));
				break;
			case 4:
				dir = Math::Vector3(0, 1, 0);
				SetGrassDashInfo(dir, grassHopperDashUp, m_spModel->GetAnimation("GrassDashJB"), m_mWorldRot.y, 2, Math::Vector3::Zero, 270.0f, 0.0f, "GrassHopperLegRPoint", SetGrassDashEffectName(m_enemyNumber));
				break;
			}
			m_grassSuccessionDelayCnt = 40;
			return;
		}
	}

	float y = 0;
	if (!m_target.expired())
	{
		y = m_target.lock()->GetPos().y - m_pos.y;
	}

	if (m_lGrassHopperPauCnt == 0)
	{
		if (m_weaponType & lGrassHopper)
		{
			int calculationResults = 0;
			switch (m_wantToMoveState)
			{
			case WantToMoveState::wEscape:
				if (m_grassSuccessionDelayCnt != 0)return;
				{
					int randNum[5] = { 0,550,150,150,150 };
					calculationResults = SetRandInfo(5, randNum);
				}
				break;
			case WantToMoveState::wDashAttack:
				if (y >= 1.0f)
				{
					int randNum[5] = { 0,0,0,0,1000 };
					calculationResults = SetRandInfo(5, randNum);
				}
				else
				{
					if (src.Length() <= 2.0f)
					{
						int randNum[5] = { 0,1000,0,0,0 };
						calculationResults = SetRandInfo(5, randNum);
					}
					else
					{
						int randNum[5] = { 1000,0,0,0,0 };
						calculationResults = SetRandInfo(5, randNum);
					}
				}
				break;
			case WantToMoveState::wGrassDash:
				if (m_grassSuccessionDelayCnt != 0)return;
				if (y >= 1.0f)
				{
					int randNum[5] = { 0,0,0,0,1000 };
					calculationResults = SetRandInfo(5, randNum);
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
						float ang = DotProductCalculation(nowVec, toVec);

						if (ang < 0.1 || ang > 150 && ang <= 180)
						{
							int randNum[5] = { 0,0,1000,0,0 };
							calculationResults = SetRandInfo(5, randNum);
						}
						else if (ang >= 0.1 && ang <= 150)
						{

							int randNum[5] = { 1000,0,0,0,0 };
							calculationResults = SetRandInfo(5, randNum);
						}
					}
					else
					{
						int randNum[5] = { 350,150,200,200,100 };
						calculationResults = SetRandInfo(5, randNum);
					}
				}
				break;
			case WantToMoveState::wAvoidance:
				if (m_grassSuccessionDelayCnt != 0)return;
				int randNum[5] = { 50,50,350,350,200 };
				calculationResults = SetRandInfo(5, randNum);
				break;
			}

			Math::Vector3 vec = m_mWorld.Backward();
			Math::Matrix efcMat;

			m_lGrassHopperPauCnt = 120;
			m_lGrassHopperTime = 90;
			m_rGrassHopperTime = 0;
			Math::Vector3 dir;

			switch (calculationResults)
			{
			case 0:
				dir = Math::Vector3::TransformNormal(Math::Vector3(0, 0, 1), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
				SetGrassDashInfo(dir, grassHopperDashF, m_spModel->GetAnimation("GrassDashFB"), m_mWorldRot.y, 3, vec, m_mWorldRot.y, 0.0f, "GrassHopperLegRPoint", SetGrassDashEffectName(m_enemyNumber));
				break;
			case 1:
				dir = Math::Vector3::TransformNormal(Math::Vector3(0, 0, -1), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
				vec = m_mWorld.Forward();
				SetGrassDashInfo(dir, grassHopperDashB, m_spModel->GetAnimation("GrassDashBB"), m_mWorldRot.y, 3, vec, m_mWorldRot.y, 180.0f, "GrassHopperLegRPoint", SetGrassDashEffectName(m_enemyNumber));
				break;
			case 2:
				dir = Math::Vector3::TransformNormal(Math::Vector3(1, 0, 0), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
				vec = m_mWorld.Right();
				SetGrassDashInfo(dir, grassHopperDashR, m_spModel->GetAnimation("GrassDashRB"), m_mWorldRot.y, 3, vec, m_mWorldRot.y, 90.0f, "GrassHopperLegLPoint", SetGrassDashEffectName(m_enemyNumber));
				break;
			case 3:
				dir = Math::Vector3::TransformNormal(Math::Vector3(-1, 0, 0), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
				vec = m_mWorld.Left();
				SetGrassDashInfo(dir, grassHopperDashL, m_spModel->GetAnimation("GrassDashLB"), m_mWorldRot.y, 3, vec, m_mWorldRot.y, 270.0f, "GrassHopperLegRPoint", SetGrassDashEffectName(m_enemyNumber));
				break;
			case 4:
				dir = Math::Vector3(0, 1, 0);
				SetGrassDashInfo(dir, grassHopperDashUp, m_spModel->GetAnimation("GrassDashJB"), m_mWorldRot.y, 3, Math::Vector3::Zero, 270.0f, 0.0f, "GrassHopperLegRPoint", SetGrassDashEffectName(m_enemyNumber));
				break;
			}
			m_grassSuccessionDelayCnt = 40;
		}
	}
}

std::string Enemy::SetGrassDashEffectName(int a_efectNumber)
{
	switch (a_efectNumber)
	{
	case 1:
		return "GrassDashBlurEnemyOne.efk";
		break;
	case 2:
		return "GrassDashBlurEnemyTwo.efk";
		break;
	case 3:
		return "GrassDashBlurEnemyThree.efk";
		break;
	case 4:
		return "GrassDashBlurEnemyFour.efk";
		break;
	case 5:
		return "GrassDashBlurEnemyFive.efk";
		break;
	default:
		return "GrassDashBlurEnemyOne.efk";
		break;
	}
}

// 短剣装備時の攻撃関係の動きを決める処理
void Enemy::ScorpionAttackDecision()
{
	if (!(m_charaState & (rAttack | lAttack | mantis | rlAttack | rlAttackRush)))
	{
		if ((m_weaponType & scorpion) && (m_weaponType & lScorpion))
		{
			if (m_bMantisAttack && m_bMantisPossAng)
			{
				SetAttackInfo(mantis, mantis, Math::Vector3::TransformNormal(Math::Vector3(0, 0, 1), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y))), m_spModel->GetAnimation("Mantis"), 0.0f);

				const std::shared_ptr<Scopion> scopion = std::dynamic_pointer_cast<Scopion>(m_weaponList[0]);
				scopion->SetMantis(Math::Matrix::CreateRotationY(m_mWorldRot.y), true);
				const std::shared_ptr<Scopion> scopion2 = std::dynamic_pointer_cast<Scopion>(m_weaponList[1]);
				scopion2->SetBMantis(true);
				m_spAnimator->SetAnimation(m_spModel->GetAnimation("Mantis"), false);
				m_bMantisAttack = false;
			}
			else
			{
				m_bRushAttackPossible = false;
				SetAttackInfo(rlAttackOne, rlAttackOne, Math::Vector3::TransformNormal(Math::Vector3(0, 0, 1), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y))), m_spModel->GetAnimation("RLAttackOne"), static_cast<float>(m_mpObj["AttackMoveSpeed"].number_value()));
			}

			if (!m_target.expired())
			{
				m_target.lock()->SetAttackHit(false);
				m_target.lock()->SetDefenseSuc(false);
			}
		}

	}
	else
	{
		if (m_bAttackAnimeDelay)
		{
			if (m_charaState & rAttackOne)
			{
				SetAttackInfo(rAttackTwo, rAttackTwo, Math::Vector3::TransformNormal(Math::Vector3(0, 0, 1), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y))), m_spModel->GetAnimation("RAttack2"), static_cast<float>(m_mpObj["AttackMoveSpeed"].number_value()));
			}
			else if (m_charaState & rAttackTwo)
			{
				SetAttackInfo(rAttackThree, rAttackThree, Math::Vector3::TransformNormal(Math::Vector3(0, 0, 1), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y))), m_spModel->GetAnimation("RAttack3"), static_cast<float>(m_mpObj["AttackMoveSpeed"].number_value()));
			}

			if (m_charaState & lAttackOne)
			{
				SetAttackInfo(lAttackTwo, lAttackTwo, Math::Vector3::TransformNormal(Math::Vector3(0, 0, 1), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y))), m_spModel->GetAnimation("LAttack2"), static_cast<float>(m_mpObj["AttackMoveSpeed"].number_value()));
			}
			else if (m_charaState & lAttackTwo)
			{
				SetAttackInfo(lAttackThree, lAttackThree, Math::Vector3::TransformNormal(Math::Vector3(0, 0, 1), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y))), m_spModel->GetAnimation("LAttack3"), static_cast<float>(m_mpObj["AttackMoveSpeed"].number_value()));
			}

			if (m_charaState & rlAttackOne)
			{
				SetAttackInfo(rlAttackTwo, rlAttackTwo, Math::Vector3::TransformNormal(Math::Vector3(0, 0, 1), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y))), m_spModel->GetAnimation("RLAttackTwo"), static_cast<float>(m_mpObj["AttackMoveSpeed"].number_value()));
			}
			else if (m_charaState & rlAttackTwo)
			{
				SetAttackInfo(rlAttackThree, rlAttackThree, Math::Vector3::TransformNormal(Math::Vector3(0, 0, 1), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y))), m_spModel->GetAnimation("RLAttackThree"), static_cast<float>(m_mpObj["AttackMoveSpeed"].number_value()));
			}
			else if (m_charaState & rlAttackThree && m_bRushAttackPossible)
			{
				SetAttackInfo(rlAttackRush, rlAttackRush, Math::Vector3::TransformNormal(Math::Vector3(0, 0, 1), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y))), m_spModel->GetAnimation("RLAttackRush"), static_cast<float>(m_mpObj["AttackMoveSpeed"].number_value()));
			}

			if (!m_target.expired())
			{
				m_target.lock()->SetAttackHit(false);
				m_target.lock()->SetDefenseSuc(false);
			}
		}
	}

	if (!(m_charaState & (rAttack | lAttack | mantis | rlAttack | rlAttackRush)))
	{
		if (m_weaponType & scorpion)
		{
			if (m_charaState & grassHopperDash && m_bBoss)
			{
				m_spAnimator->SetAnimation(m_spModel->GetAnimation("GrassDashRAttack"), false);
				SetAttackInfo(rAttackOne, static_cast<CharaState>((~lAttack & ~run)), Math::Vector3::TransformNormal(Math::Vector3(0, 0, 1), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y))), m_spModel->GetAnimation("GrassDashRAttack"), 0.0f);

			}
			else
			{
				SetAttackInfo(rAttackOne, static_cast<CharaState>((~lAttack & ~run)), Math::Vector3::TransformNormal(Math::Vector3(0, 0, 1), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y))), m_spModel->GetAnimation("RAttack1"), static_cast<float>(m_mpObj["AttackMoveSpeed"].number_value()));
			}
		}
		else if (m_weaponType & lScorpion)
		{
			if (m_charaState & grassHopperDash && m_bBoss)
			{
				SetAttackInfo(lAttackOne, static_cast<CharaState>((~rAttack & ~run)), Math::Vector3::TransformNormal(Math::Vector3(0, 0, 1), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y))), m_spModel->GetAnimation("GrassDashLAttack"), 0.0f);
			}
			else
			{
				SetAttackInfo(lAttackOne, static_cast<CharaState>((~rAttack & ~run)), Math::Vector3::TransformNormal(Math::Vector3(0, 0, 1), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y))), m_spModel->GetAnimation("LAttack1"), static_cast<float>(m_mpObj["AttackMoveSpeed"].number_value()));
			}
		}

		if (!m_target.expired())
		{
			m_target.lock()->SetAttackHit(false);
			m_target.lock()->SetDefenseSuc(false);
		}
	}
}

// 短剣装備時の防御関係の動きを決める処理
void Enemy::ScorpionDefenseDecision()
{
	if (!(m_charaState & (rAttack | lAttack | defense | mantis | rlAttack | rlAttackRush)))
	{
		m_charaState = defense;
		m_charaState &= defense;
		m_bMove = true;
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Defense"), true);
	}
}

// ステップの方向を決める処理
void Enemy::StepVecDecision()
{
	int randNum[2] = { 500,500 };
	Math::Vector3 dir;

	switch (SetRandInfo(2, randNum))
	{
	case 0:
		dir = Math::Vector3::TransformNormal(Math::Vector3(1, 0, 0), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
		SetStepDashInfo(dir, stepR, m_spModel->GetAnimation("GrassDashRB"));
		break;
	case 1:
		dir = Math::Vector3::TransformNormal(Math::Vector3(-1, 0, 0), Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
		SetStepDashInfo(dir, stepL, m_spModel->GetAnimation("GrassDashLB"));
		break;
	}
}

// 歩く方向を決める処理
void Enemy::NormalMoveVecDecision()
{
	if (!(m_charaState & run))
	{
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("RUN"));
	}
	m_charaState = run;
}
// ＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝

// 各Stateの動き処理＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
// Hopper行動関連の処理
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
		if (m_charaState & grassHopperDashF)
		{
			Math::Vector3 vec = m_mWorld.Backward();
			EffectTrackingUpdate(SetGrassDashEffectName(m_enemyNumber), 1, m_mWorldRot.y, 0.0f, m_pos, m_addGrassDashEffectPosVal, vec);
		}
		else if (m_charaState & grassHopperDashB)
		{
			Math::Vector3 vec = m_mWorld.Forward();
			EffectTrackingUpdate(SetGrassDashEffectName(m_enemyNumber), 1, m_mWorldRot.y + 180.0f, 0.0f, m_pos, m_addGrassDashEffectPosVal, vec);
		}
		else if (m_charaState & grassHopperDashR)
		{
			Math::Vector3 vec = m_mWorld.Right();
			EffectTrackingUpdate(SetGrassDashEffectName(m_enemyNumber), 1, m_mWorldRot.y + 90.0f, 0.0f, m_pos, m_addGrassDashEffectPosVal, vec);
		}
		else if (m_charaState & grassHopperDashL)
		{
			Math::Vector3 vec = m_mWorld.Left();
			EffectTrackingUpdate(SetGrassDashEffectName(m_enemyNumber), 1, m_mWorldRot.y + 270.0f, 0.0f, m_pos, m_addGrassDashEffectPosVal, vec);
		}
		else if (m_charaState & grassHopperDashUp)
		{
			Math::Matrix efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(270.0f)) * Math::Matrix::CreateTranslation({ m_pos.x,m_pos.y + static_cast<float>(m_mpObj["AddGrassDashUpEffectPosYVal"].number_value()),m_pos.z });
			EffectTrackingUpdate(SetGrassDashEffectName(m_enemyNumber), 1, 0.0f, 270.0f, m_pos, Math::Vector3(0, static_cast<float>(m_mpObj["AddGrassDashUpEffectPosYVal"].number_value()), 0));
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
				if (m_charaState & grassHopperDashUp)
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
			GrassDashAfterAnimationChange();
		}

		if (m_rGrassHopperTime == (*m_wpJsonObj.lock())["GrassDashSteppingAwayJustBefore"].int_value())
		{
			m_weaponList[2]->StartAnime();
			GrassDashAfterAnimationChange();
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
				if (!(m_charaState & (rAttackOne | rAttackTwo)))
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
			if (m_charaState & grassHopperDashUp)
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
		m_charaState = fall;
		Brain();
	}


	m_pos += m_grassHopperDashDir * m_dashSpd;

	if (m_charaState & grassHopperDashB | grassHopperDashF)
	{
		UpdateRotate(m_grassHopperDashDir);
	}
	else if (m_charaState & grassHopperDashR | grassHopperDashL)
	{
		std::shared_ptr<Player> spTarget = m_target.lock();
		if (spTarget)
		{
			Math::Vector3 vTarget = spTarget->GetPos() - m_pos;
			UpdateRotate(vTarget);
		}
	}
	else if (m_charaState & grassHopperDashUp)
	{
		Math::Matrix rotYMat = Math::Matrix::CreateRotationY(m_mWorldRot.y);
		Math::Vector3 forwardVec = Math::Vector3(0, 0, 1);
		Math::Vector3 toVec = Math::Vector3::TransformNormal(forwardVec, rotYMat);
		UpdateRotate(toVec);
	}
}

// ステップ行動関連の処理
void Enemy::StepMove()
{
	if (m_stepCnt != 0)
	{
		--m_stepCnt;
		m_bMove = true;
		for (int i = 0; i < 5; ++i)
		{
			if (m_stepCnt > m_mpObj["StepTransitionTime"][i].int_value())
			{
				m_dashSpd = static_cast<float>(m_mpObj["StepSpeed"][i].number_value());
				break;
			}
		}
	}
	else
	{
		m_bMove = false;
		m_dashSpd = 0.0f;
		m_stepDashDir = {};
		m_gravity = 0;
		m_charaState = idle;
		Brain();
	}

	m_pos += m_stepDashDir * m_dashSpd;
}

// 歩き行動関連の処理
void Enemy::NormalMove()
{
	Math::Vector3 moveVec = {};

	if (m_charaState & run)
	{
		if (m_enemyType == wimpEnemyTypeOne)
		{
			Math::Vector3 src;
			std::shared_ptr<Player> spTarget = m_target.lock();
			if (spTarget)
			{
				src = spTarget->GetPos() - m_pos;
			}

			if (src.Length() <= static_cast<float>(m_mpObj["PlayerIsDistToBackMove"].number_value()))
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

	if (m_charaState & jump)
	{
		if (!(m_charaState & (jump | fall)))
		{
			m_gravity = -0.2f;
			m_charaState |= jump;
			m_bMove = true;
		}
	}

	if (!(m_charaState & (grassHopperDash | grassHopperDashUp)))
	{
		m_pos += moveVec * static_cast<float>(m_mpObj["MoveSpeed"].number_value());
	}

	Brain();
}

// 短剣で攻撃した際の行動処理
void Enemy::ScorpionAttackMove()
{
	if (m_spAnimator->IsAnimationEnd())
	{
		if (m_charaState & lAttack)
		{
			m_charaState &= ~lAttack;
			if (m_charaState & grassHopperDashF | step && m_bBoss)
			{
				m_spAnimator->SetAnimation(m_spModel->GetAnimation("GrassDashFA"), true);
			}
			else
			{
				if (!m_bBoss)
				{
					m_coarseFishEnemyAttackDelayCnt = m_mpObj["MaxCoarseFishEnemyAttackDelayCnt"].int_value();
				}

				Brain();
			}
		}

		if (m_charaState & rAttack)
		{
			m_charaState &= ~rAttack;
			if (m_charaState & grassHopperDashF | step && m_bBoss)
			{
				m_spAnimator->SetAnimation(m_spModel->GetAnimation("GrassDashFA"), true);
			}
			else
			{
				if (!m_bBoss)
				{
					m_coarseFishEnemyAttackDelayCnt = m_mpObj["MaxCoarseFishEnemyAttackDelayCnt"].int_value();
				}
				Brain();
			}
		}

		if (m_charaState & (rlAttack | rlAttackRush | mantis))
		{
			m_charaState = idle;
			Brain();
		}
	}
	else
	{
		if (!(m_charaState & (rlAttack | rlAttackRush)))
		{
			m_attackMoveSpd *= static_cast<float>((*m_wpJsonObj.lock())["MoveSpeedDecelerationamount"].number_value());

			if (m_charaState & (rAttackTwo | lAttackTwo))
			{
				if (m_attackAnimeCnt == m_mpObj["AttackPointMoment"].int_value())
				{
					if (!m_target.expired())
					{
						m_target.lock()->SetAttackHit(false);
						m_target.lock()->SetDefenseSuc(false);
					}
				}
			}
			else if (m_charaState & (rAttackThree | lAttackThree))
			{
				if (m_attackAnimeCnt == m_mpObj["AttackPointMoment"].int_value())
				{
					if (!m_target.expired())
					{
						m_target.lock()->SetAttackHit(false);
						m_target.lock()->SetDefenseSuc(false);
					}
				}
			}
		}
		if (!(m_charaState & rlAttackRush))
		{
			if (m_attackAnimeCnt >= m_mpObj["AttackPointMoment"].int_value())
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
					m_leftWeaponNumber = m_mpObj["LeftWeaponScopionNum"].int_value();
					m_rightWeaponNumber = m_mpObj["RightWeaponScopionNum"].int_value();
					break;
				}
			}
		}

		if (!(m_charaState & (rlAttack | rlAttackRush)))
		{
			m_attackMoveSpd *= static_cast<float>((*m_wpJsonObj.lock())["MoveSpeedDecelerationamount"].number_value());
		}
		else
		{
			if (m_charaState & rlAttackOne)
			{
				m_attackMoveSpd *= static_cast<float>((*m_wpJsonObj.lock())["MoveSpeedDecelerationamount"].number_value());
				if (m_attackAnimeCnt == m_mpObj["RLAttackOneShakenSecondMoment"].int_value())
				{
					if (!m_target.expired())
					{
						m_target.lock()->SetAttackHit(false);
						m_target.lock()->SetDefenseSuc(false);
					}
				}
			}
			else if (m_charaState & rlAttackTwo)
			{
				m_attackMoveSpd *= static_cast<float>((*m_wpJsonObj.lock())["MoveSpeedDecelerationamount"].number_value());
				if (m_attackAnimeCnt == m_mpObj["RLAttackTwoShakenSecondMoment"].int_value())
				{
					if (!m_target.expired())
					{
						m_target.lock()->SetAttackHit(false);
						m_target.lock()->SetDefenseSuc(false);
					}
				}
			}
			else if (m_charaState & rlAttackThree)
			{
				m_attackMoveSpd *= static_cast<float>((*m_wpJsonObj.lock())["MoveSpeedDecelerationamount"].number_value());
				if (m_attackAnimeCnt == m_mpObj["RLAttackThreeShakenSecondMoment"].int_value())
				{
					if (!m_target.expired())
					{
						m_target.lock()->SetAttackHit(false);
						m_target.lock()->SetDefenseSuc(false);
					}
				}
			}
			else if (m_charaState & rlAttackRush)
			{
				m_attackMoveSpd *= static_cast<float>((*m_wpJsonObj.lock())["RushAttackMoveSpeedDecelerationamount"].number_value());
				for (int i = 1; i < 7; ++i)
				{
					if (m_attackAnimeCnt == m_mpObj["RushLastAttackPointTime"].int_value())
					{
						if (!m_target.expired())
						{
							m_target.lock()->SetAttackHit(false);
							m_target.lock()->SetDefenseSuc(false);
						}
						m_attackMoveSpd = static_cast<float>(m_mpObj["RushLastAttackMoveSpeed"].number_value());
						break;
					}

					if (m_attackAnimeCnt == m_mpObj["RLAttackRushShakenMoment"][i].int_value())
					{
						if (!m_target.expired())
						{
							m_target.lock()->SetAttackHit(false);
							m_target.lock()->SetDefenseSuc(false);
						}
						m_attackMoveSpd = static_cast<float>(m_mpObj["RLAttackRushMoveSpeed"][i - 1].number_value());
						break;
					}
				}
			}
		}

		SpeedyMoveWallHitChack(m_attackMoveSpd, m_attackMoveDir);
		m_pos += m_attackMoveDir * m_attackMoveSpd;
	}
}

// 短剣装備時の防御関連の処理
void Enemy::ScorpionDefenseMove()
{
	std::shared_ptr<Player> spTarget = m_target.lock();
	if (spTarget)
	{
		Math::Vector3 vTarget = spTarget->GetPos() - m_pos;
		Math::Vector3 src = spTarget->GetPos() - m_pos;

		if (spTarget->GetCharaState() & (rAttack | lAttack | rlAttack | grassHopperDashF))
		{
			m_bMove = true;
			UpdateRotate(vTarget);
		}
		else
		{
			Brain();
		}

		if (spTarget->GetCharaState() & hasDefense)
		{
			m_wantToMoveState = wAttack;
		}
	}
}

// 攻撃を防がれた時の動き
void Enemy::HasDefenseMove()
{
	m_bMove = true;
	if (m_spAnimator->IsAnimationEnd())
	{
		--m_hasDeTime;
		if (m_hasDeTime <= 0)
		{
			Brain();
			m_charaState = idle;
			m_hasDeTime = 0;
		}
	}
}
// ＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝

// 敵の動きを決める処理＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
// タイプごとの行動を決める処理関数に分岐する処理
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
			if (!(m_charaState & (hit | hasDefense)))
			{
				if (!(m_charaState & fall))
				{
					m_spAnimator->SetAnimation(m_spModel->GetAnimation("FallA"), false);
				}
				m_charaState = fall;
				m_wantToMoveState = wNone;
			}
		}
	}
}

// ストライカータイプの行動を決める処理
void Enemy::StrikerBrain()
{
	Math::Vector3 src;

	std::shared_ptr<Player> spTarget = m_target.lock();
	if (spTarget)
	{
		Math::Vector3 vTarget = spTarget->GetPos() - m_pos;
		src = spTarget->GetPos() - m_pos;
	}

	if (src.Length() <= 1.2f)
	{
		if (spTarget && spTarget->GetCharaState() & (rlAttack))
		{
			int randNum[2] = { 350,650 };
			switch (SetRandInfo(2, randNum))
			{
			case 0:
				m_wantToMoveCategory = Enemy::WantToMoveCategory::defenseCategory;
				break;
			case 1:
				m_wantToMoveCategory = Enemy::WantToMoveCategory::attackCategory;
				break;
			}
		}
		else
		{
			int randNum[2] = { 750,250 };
			switch (SetRandInfo(2, randNum))
			{
			case 0:
				m_wantToMoveCategory = Enemy::WantToMoveCategory::attackCategory;
				break;
			case 1:
				m_wantToMoveCategory = Enemy::WantToMoveCategory::defenseCategory;
				break;
			}
		}

	}
	else
	{
		if (src.Length() <= 2.5f && spTarget && spTarget->GetCharaState() & defense && !(m_charaState & grassHopperDash))
		{
			int randNum[2] = { 600,400 };
			switch (SetRandInfo(2, randNum))
			{
			case 0:
				m_wantToMoveCategory = Enemy::WantToMoveCategory::attackCategory;
				break;
			case 1:
				m_wantToMoveCategory = Enemy::WantToMoveCategory::approachCategory;
				break;
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
		if (spTarget && spTarget->GetCharaState() & defense && !(m_charaState & grassHopperDash))
		{
			m_bMantisAttack = true;
		}
		break;
	case Enemy::WantToMoveCategory::defenseCategory:
	{
		int randNum[2] = { 800,200 };
		switch (SetRandInfo(2, randNum))
		{
		case 0:
			m_wantToMoveState = Enemy::WantToMoveState::wStep;
			break;
		case 1:
			m_wantToMoveState = Enemy::WantToMoveState::wDefense;
			break;
		}
	}
	break;
	case Enemy::WantToMoveCategory::approachCategory:
	{
		int randNum[2] = { 990,10 };
		switch (SetRandInfo(2, randNum))
		{
		case 0:
			m_wantToMoveCategory = Enemy::WantToMoveCategory::runCategory;
			break;
		case 1:
			m_wantToMoveCategory = Enemy::WantToMoveCategory::grassCategory;
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

	DecideWeaponNumber();
}

// ディフェンダータイプの行動を決める処理
void Enemy::DefenderBrain()
{
	Math::Vector3 src;

	std::shared_ptr<Player> spTarget = m_target.lock();
	if (spTarget)
	{
		Math::Vector3 vTarget = spTarget->GetPos() - m_pos;
		src = spTarget->GetPos() - m_pos;
	}

	if (src.Length() <= 1.2f)
	{
		if (spTarget && spTarget->GetCharaState() & rlAttack)
		{
			int randNum[2] = { 950,50 };
			switch (SetRandInfo(2, randNum))
			{
			case 0:
				m_wantToMoveCategory = Enemy::WantToMoveCategory::defenseCategory;
				break;
			case 1:
				m_wantToMoveCategory = Enemy::WantToMoveCategory::attackCategory;
				break;
			}
		}
		else
		{
			int randNum[2] = { 650,350 };
			switch (SetRandInfo(2, randNum))
			{
			case 0:
				m_wantToMoveCategory = Enemy::WantToMoveCategory::attackCategory;
				break;
			case 1:
				m_wantToMoveCategory = Enemy::WantToMoveCategory::defenseCategory;
				break;
			}
		}

	}
	else
	{
		if (src.Length() <= 2.5f && !m_target.expired() && m_target.lock()->GetCharaState() & defense && !(m_charaState & grassHopperDash))
		{
			int randNum[2] = { 600,400 };
			switch (SetRandInfo(2, randNum))
			{
			case 0:
				m_wantToMoveCategory = Enemy::WantToMoveCategory::attackCategory;
				break;
			case 1:
				m_wantToMoveCategory = Enemy::WantToMoveCategory::approachCategory;
				break;
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
		if (!m_target.expired() && m_target.lock()->GetCharaState() & defense && !(m_charaState & grassHopperDash))
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

	DecideWeaponNumber();
}

// スピードスタータイプの行動を決める処理
void Enemy::SpeedSterBrain()
{
	Math::Vector3 src;

	std::shared_ptr<Player> spTarget = m_target.lock();
	if (spTarget)
	{
		Math::Vector3 vTarget = spTarget->GetPos() - m_pos;
		src = spTarget->GetPos() - m_pos;
	}

	if (src.Length() <= 1.2f)
	{
		if (spTarget && spTarget->GetCharaState() & (rlAttack))
		{
			int randNum[2] = { 750,250 };
			switch (SetRandInfo(2, randNum))
			{
			case 0:
				m_wantToMoveCategory = Enemy::WantToMoveCategory::defenseCategory;
				break;
			case 1:
				m_wantToMoveCategory = Enemy::WantToMoveCategory::attackCategory;
				break;
			}
		}
		else
		{
			int randNum[2] = { 750,250 };
			switch (SetRandInfo(2, randNum))
			{
			case 0:
				m_wantToMoveCategory = Enemy::WantToMoveCategory::attackCategory;
				break;
			case 1:
				m_wantToMoveCategory = Enemy::WantToMoveCategory::defenseCategory;
				break;
			}
		}

	}
	else
	{
		if (src.Length() <= 2.5f && !m_target.expired() && m_target.lock()->GetCharaState() & defense && !(m_charaState & grassHopperDash))
		{
			int randNum[2] = { 600,400 };
			switch (SetRandInfo(2, randNum))
			{
			case 0:
				m_wantToMoveCategory = Enemy::WantToMoveCategory::attackCategory;
				break;
			case 1:
				m_wantToMoveCategory = Enemy::WantToMoveCategory::approachCategory;
				break;
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
		if (!m_target.expired() && m_target.lock()->GetCharaState() & defense && !(m_charaState & grassHopperDash))
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
	{
		int randNum[2] = { 500,500 };
		switch (SetRandInfo(2, randNum))
		{
		case 0:
			m_wantToMoveState = Enemy::WantToMoveState::wAvoidance;
			break;
		case 1:
			m_wantToMoveState = Enemy::WantToMoveState::wEscape;
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
	{
		int calculationResults = 0;
		if (src.Length() >= 10.0f)
		{
			int randNum[2] = { 250,850 };
			calculationResults = SetRandInfo(2, randNum);
		}
		else if (src.Length() >= 5.0f)
		{
			int randNum[2] = { 500,500 };
			calculationResults = SetRandInfo(2, randNum);
		}
		else if (src.Length() < 5.0f)
		{
			int randNum[2] = { 300,700 };
			calculationResults = SetRandInfo(2, randNum);
		}

		switch (calculationResults)
		{
		case 0:
			m_wantToMoveState = Enemy::WantToMoveState::wGrassDash;
			break;
		case 1:
			m_wantToMoveState = Enemy::WantToMoveState::wDashAttack;
			break;
		}
	}
	break;
	}

	DecideWeaponNumber();
}

// オールラウンダータイプの行動を決める処理
void Enemy::AllRounderBrain()
{
	Math::Vector3 src;

	std::shared_ptr<Player> spTarget = m_target.lock();
	if (spTarget)
	{
		Math::Vector3 vTarget = spTarget->GetPos() - m_pos;
		src = spTarget->GetPos() - m_pos;
	}

	if (src.Length() <= 8.0f && spTarget)
	{
		if (spTarget->GetCharaState() & (rlAttack))
		{
			int randNum[2] = { 350,650 };
			switch (SetRandInfo(2, randNum))
			{
			case 0:
				m_wantToMoveCategory = Enemy::WantToMoveCategory::defenseCategory;
				break;
			case 1:
				m_wantToMoveState = Enemy::WantToMoveState::wNone;
				break;
			}
		}
		else
		{
			int randNum[2] = { 750,250 };
			switch (SetRandInfo(2, randNum))
			{
			case 0:
				m_wantToMoveCategory = Enemy::WantToMoveCategory::attackCategory;
				break;
			case 1:
				m_wantToMoveState = Enemy::WantToMoveState::wNone;
				break;
			}
		}

	}
	else
	{
		if (src.Length() <= 2.5f && !m_target.expired() && m_target.lock()->GetCharaState() & defense && !(m_charaState & grassHopperDash))
		{
			int randNum[2] = { 600,400 };
			switch (SetRandInfo(2, randNum))
			{
			case 0:
				m_wantToMoveCategory = Enemy::WantToMoveCategory::attackCategory;
				break;
			case 1:
				m_wantToMoveCategory = Enemy::WantToMoveCategory::approachCategory;
				break;
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
		if (!m_target.expired() && m_target.lock()->GetCharaState() & defense && !(m_charaState & grassHopperDash))
		{
			m_bMantisAttack = true;
		}
		break;
	case Enemy::WantToMoveCategory::defenseCategory:
	{
		int randNum[3] = { 600,200,200 };
		switch (SetRandInfo(3, randNum))
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
	}
	break;
	case Enemy::WantToMoveCategory::approachCategory:
		if (src.Length() <= 25.0f)
		{
			int randNum[2] = { 900,100 };
			switch (SetRandInfo(2, randNum))
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
			int randNum[2] = { 900,100 };
			switch (SetRandInfo(2, randNum))
			{
			case 0:
				m_wantToMoveCategory = Enemy::WantToMoveCategory::grassCategory;
				break;
			case 1:
				m_wantToMoveCategory = Enemy::WantToMoveCategory::runCategory;
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
	{
		int calculationResults = 0;
		if (src.Length() >= 10.0f)
		{
			int randNum[2] = { 250,750 };
			calculationResults = SetRandInfo(2, randNum);
		}
		else
		{
			int randNum[2] = { 500,500 };
			calculationResults = SetRandInfo(2, randNum);
		}


		switch (calculationResults)
		{
		case 0:
			m_wantToMoveState = Enemy::WantToMoveState::wGrassDash;
			break;
		case 1:
			m_wantToMoveState = Enemy::WantToMoveState::wDashAttack;
			break;
		}
	}
	break;
	}

	DecideWeaponNumber();
}

// 人型の弱い敵の行動を決める処理
void Enemy::CoarseFishEnemyBrain()
{
	Math::Vector3 src;
	std::shared_ptr<Player> spTarget = m_target.lock();
	if (spTarget)
	{
		Math::Vector3 vTarget = spTarget->GetPos() - m_pos;
		src = spTarget->GetPos() - m_pos;
	}

	if (m_coarseFishEnemyAttackDelayCnt == 0 && m_enemyAttackMaxTotal > m_enemyAttackTotal)
	{
		if (src.Length() <= 2.0f)
		{
			m_wantToMoveState = Enemy::WantToMoveState::wNone;
		}
		else if (src.Length() <= 10.0f)
		{
			int randNum[2] = { 999,1 };
			switch (SetRandInfo(2, randNum))
			{
			case 0:
				m_wantToMoveState = Enemy::WantToMoveState::wRun;
				break;
			case 1:
				m_wantToMoveState = Enemy::WantToMoveState::wAttack;
				break;
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

// モンスター型の弱い敵の行動を決める処理
void Enemy::WimpEnemyBrain()
{
	Math::Vector3 src;
	const KdModelWork::Node* node = nullptr;
	Math::Matrix mat = Math::Matrix::Identity;

	node = m_spModel->FindNode("HitPoint");
	mat = node->m_worldTransform * m_mWorld;

	std::shared_ptr<Player> spTarget = m_target.lock();
	if (spTarget)
	{
		src = spTarget->GetPos() - mat.Translation();
	}

	if (src.Length() <= 110.0f && src.Length() > 10.0f && m_enemyAttackMaxTotal > m_enemyAttackTotal && m_bRangedAttackCapableOfFiring)
	{
		int randNum[2] = { 995,5 };
		switch (SetRandInfo(2, randNum))
		{

		case 0:
			m_wantToMoveState = Enemy::WantToMoveState::wRun;
			break;
		case 1:
			EnergyCharge(false);
			break;
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

// モンスター型のボスの行動を決める処理
void Enemy::BossEnemyTypeOneBrain()
{
	const KdModelWork::Node* node = nullptr;
	Math::Matrix mat = Math::Matrix::Identity;

	node = m_spModel->FindNode("HitPoint");
	mat = node->m_worldTransform * m_mWorld;

	Math::Vector3 src;
	int calculationResults = 0;
	std::shared_ptr<Player> spTarget = m_target.lock();
	if (spTarget)
	{
		src = spTarget->GetPos() - mat.Translation();
	}

	if (m_endurance <= 500)
	{
		int randNum[2] = { 600,400 };
		calculationResults = SetRandInfo(2, randNum);
	}
	else
	{
		int randNum[2] = { 850,150 };
		calculationResults = SetRandInfo(2, randNum);
	}

	if (m_bRangedAttackCapableOfFiring)
	{
		if (src.Length() <= 45.0f)
		{
			switch (calculationResults)
			{
			case 0:
				m_wantToMoveState = Enemy::WantToMoveState::wRun;
				break;
			case 1:
				EnergyCharge(true);
				break;
			}
		}
		else
		{
			switch (calculationResults)
			{
			case 0:
				m_wantToMoveState = Enemy::WantToMoveState::wRun;
				break;
			case 1:
				EnergyCharge(false);
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
// ＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝

// エネルギー系の攻撃を決める処理
void Enemy::EnergyCharge(bool a_bBeem)
{
	if (m_target.expired())return;
	m_target.lock()->SetAttackHit(false);
	m_target.lock()->SetDefenseSuc(false);
	const KdModelWork::Node* node = nullptr;
	Math::Matrix mat = Math::Matrix::Identity;
	node = m_spModel->FindNode("HitPoint");
	mat = node->m_worldTransform * m_mWorld;
	m_rangedAttackShotPos = mat.Translation();
	m_rangedAttackTargetPos = { m_target.lock()->GetPos().x, m_target.lock()->GetPos().y + m_target.lock()->GetAddCenterVal().y, m_target.lock()->GetPos().z };

	Math::Vector3 nowVec = Math::Vector3::Transform({ 0,0,1 }, Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
	nowVec.x = 0;
	nowVec.Normalize();

	Math::Vector3 targetVec = m_rangedAttackTargetPos - m_rangedAttackShotPos;
	targetVec.x = 0;
	targetVec.Normalize();

	float Xang = DotProductCalculation(nowVec, targetVec);

	if (m_enemyType & wimpEnemyTypeOne)
	{
		if (m_rangedAttackShotPos.y <= m_rangedAttackTargetPos.y)
		{
			if (Xang >= 45)
			{
				Xang = 45;
			}

			Xang *= -1;
		}
		else
		{
			if (Xang >= 3)
			{
				Xang = 3;
			}
		}
	}
	else
	{

		if (Xang >= 45)
		{
			Xang = 45;
		}

		if (m_rangedAttackShotPos.y <= m_rangedAttackTargetPos.y)
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
		Math::Matrix efcMat = Math::Matrix::CreateScale(1.0f) * Math::Matrix::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(m_mWorldRot.y + 180), DirectX::XMConvertToRadians(-Xang), 0) * Math::Matrix::CreateTranslation(m_rangedAttackShotPos);
		KdEffekseerManager::GetInstance().SetWorldMatrix("Beem.efk", efcMat);

		m_rangedAttackDir = Math::Vector3::Transform({ 0,0,1 }, Math::Matrix::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(m_mWorldRot.y), DirectX::XMConvertToRadians(Xang), 0));
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
			Math::Matrix efcMat = scaleMat * Math::Matrix::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(m_mWorldRot.y + 180), DirectX::XMConvertToRadians(-Xang), 0) * Math::Matrix::CreateTranslation(m_rangedAttackShotPos);
			KdEffekseerManager::GetInstance().SetWorldMatrix("BossEnergyBullet.efk", efcMat);
		}
		else
		{
			KdEffekseerManager::GetInstance().
				Play("EnergyBullet.efk", m_rangedAttackShotPos);
			KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("EnergyBullet.efk"); // これでループしない
			scaleMat = Math::Matrix::CreateScale(0.5f);
			Math::Matrix efcMat = scaleMat * Math::Matrix::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(m_mWorldRot.y + 180), DirectX::XMConvertToRadians(-Xang), 0) * Math::Matrix::CreateTranslation(m_rangedAttackShotPos);
			KdEffekseerManager::GetInstance().SetWorldMatrix("EnergyBullet.efk", efcMat);
		}

		m_energyBulletDir = Math::Vector3::Transform({ 0,0,1 }, Math::Matrix::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(m_mWorldRot.y), DirectX::XMConvertToRadians(Xang), 0));
		m_energyBulletDir.Normalize();
	}

	m_bRangedAttack = true;
}

// 回転攻撃の処理
void Enemy::RotationAttackMove()
{
	Math::Vector3 moveVec;
	float moveSpd = static_cast<float>(m_mpObj["RotationAttackMoveSpeed"].number_value());
	moveVec += Math::Vector3::TransformNormal({ 0, 0, 1 }, Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)));
	m_pos += moveVec * moveSpd;

	if (m_spAnimator->IsAnimationEnd())
	{
		m_attackAnimeCnt = 0;
		m_notHumanoidEnemyState = stand;
		m_notHumanoidEnemyState &= ~rotationAttack;
		Brain();
	}

	if (!m_target.expired())
	{
		if (m_attackAnimeCnt >= m_mpObj["RotationAttackHitStart"].int_value() &&
			m_attackAnimeCnt <= m_mpObj["RotationAttackHitEnd"].int_value())
		{
			m_target.lock()->SetAttackHit(false);
			m_target.lock()->SetDefenseSuc(false);
			RotationAttackChaeck();
		}
	}
}

int Enemy::SetRandInfo(int a_randMaxNum, int* a_randItemized)
{
	std::random_device rnd;
	std::mt19937 mt(rnd());
	std::uniform_int_distribution<int> intRand(0, 999);

	int rand = intRand(mt);
	std::vector<int> randNum;
	for (int i = 0; i < a_randMaxNum; ++i)
	{
		rand -= a_randItemized[i];
		if (rand < 0)
		{
			return i;
		}
	}

	return a_randMaxNum - 1;
}

void Enemy::DecideWeaponNumber()
{
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

