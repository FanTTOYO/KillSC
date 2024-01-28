#include "Player.h"
#include "../../../Camera/GameCamera/GameCamera.h"
#include "../../../Scene/SceneManager.h"
#include "../../Weapon/Hopper/Hopper.h"
#include "../../Weapon/Scopion/Scopion.h"
#include "../Enemy/Enemy.h"
#include "../../Ui/Ui.h"

// 初期化
void Player::Init(std::weak_ptr<json11::Json> a_wpJsonObj)
{
	auto& object = (*a_wpJsonObj.lock())["Player"].object_items();

	// 座標行列
	Math::Matrix transMat;
	transMat = Math::Matrix::CreateTranslation((float)object["Position"][0].number_value(), 
											   (float)object["Position"][1].number_value(), 
											   (float)object["Position"][2].number_value());
	m_pos = transMat.Translation();

	// 行列合成
	m_mWorld = transMat;

	m_spModel = std::make_shared<KdModelWork>();
	m_spModel->SetModelData
	(KdAssets::Instance().m_modeldatas.GetData
	("Asset/Models/Player/Player.gltf"));
	// 当たり判定初期化
	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape
	("PlayerModel", m_spModel, KdCollider::TypeBump | KdCollider::TypeDamage);

	m_pDebugWire = std::make_unique<KdDebugWireFrame>();

	m_playerState = idle;

	m_weaponType |= scorpion;
	m_weaponType |= lScorpion;

	m_rightWeaponNumber = object["RightWeaponScopionNum"].int_value();
	m_leftWeaponNumber  = object["LeftWeaponScopionNum"].int_value();

	m_vForce = (float)object["Vforce"].number_value();
	m_endurance = (float)object["Endurance"].number_value();

	m_graduallyTorionDecVal = 0.0f;
	m_bAttackAnimeCnt = true;
	m_bRushAttackPossible = false;
	m_invincibilityTimeCnt = 0;



	m_spAnimator = std::make_shared<KdAnimator>();
	m_spAnimator->SetAnimation(m_spModel->GetAnimation("IdleA"), false);


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

	for (auto& enemy : m_enemyList)
	{
		sco->AddTarget(enemy.lock());
		sco2->AddTarget(enemy.lock());
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

	m_rocKOnPolygon.SetMaterial("Asset/Textures/Ui/Game/ターゲット N 白.png");

	m_hitColorChangeTimeCnt = 0;

	m_bRushRp = false;
	m_bBlowingAwayHitB = false;
	m_bAtttackMoveSpeedDec = false;
	m_attackHit = false;
	m_mpObj = object;
	m_wpJsonObj = a_wpJsonObj;
	m_tuGardTime = 0;

	Math::Vector3 addCenterVal = { static_cast<float>(m_mpObj["AddCenterVal"][0].number_value()),
							       static_cast<float>(m_mpObj["AddCenterVal"][1].number_value()),
								   static_cast<float>(m_mpObj["AddCenterVal"][2].number_value()) };
	m_addCenterVal = addCenterVal;

	Math::Vector3 AddGrassDashEffectPosVal = { static_cast<float>(m_mpObj["AddGrassDashEffectPosVal"][0].number_value()),
											   static_cast<float>(m_mpObj["AddGrassDashEffectPosVal"][1].number_value()),
											   static_cast<float>(m_mpObj["AddGrassDashEffectPosVal"][2].number_value())};
	m_addGrassDashEffectPosVal = AddGrassDashEffectPosVal;
	m_maxWeaponType = 2; // ここはJSONからもらってくるようにする　＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
}

// 武器に敵情報を渡す
void Player::AddWeaponToEnemy(std::shared_ptr<Enemy> a_spEnemy)
{
	m_weaponList[0]->AddTarget(a_spEnemy);
	m_weaponList[1]->AddTarget(a_spEnemy);
}

// 更新＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
// 本更新前の更新
void Player::PreUpdate()
{
	m_vForce -= m_graduallyTorionDecVal;
	if (m_vForce <= 0)
	{
		m_vForce = 0;
		if (!m_bPlayerDeath)
		{
			const std::shared_ptr<GameCamera> gCamera = std::dynamic_pointer_cast<GameCamera>(m_wpCamera.lock());
			m_bPlayerDeath = true;
			m_spAnimator->SetAnimation(m_spModel->GetAnimation("Death"), false);
			m_playerState = idle;
			gCamera->SetBRotateEnemy(false);
			m_wpEnemy.reset();
		}
	}

	if (m_endurance <= 0)
	{
		m_endurance = 0;
		if (!m_bPlayerDeath)
		{
			const std::shared_ptr<GameCamera> gCamera = std::dynamic_pointer_cast<GameCamera>(m_wpCamera.lock());
			m_bPlayerDeath = true;
			m_spAnimator->SetAnimation(m_spModel->GetAnimation("Death"), false);
			m_playerState = idle;
			gCamera->SetBRotateEnemy(false);
			m_wpEnemy.reset();
		}
	}
}

// 武器を切り替える処理
void Player::WeaponChangeProcess()
{
	if (!(m_playerState & (mantis | hasDefense | rlAttackRush)))
	{
		if (KdInputManager::Instance().IsPress("lWeaponChange"))
		{
			m_leftWeaponNumber++;
			if (m_leftWeaponNumber > m_maxWeaponType)
			{
				m_leftWeaponNumber = FIRST_WEAPON_TYPE_NUMBER;
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

		if (KdInputManager::Instance().IsPress("rWeaponChange"))
		{
			m_rightWeaponNumber++;
			if (m_rightWeaponNumber > m_maxWeaponType)
			{
				m_rightWeaponNumber = FIRST_WEAPON_TYPE_NUMBER;
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
}

// 更新処理
void Player::Update()
{
	if (m_bPlayerLose)return;

	OverStageChaeck();

	if (m_bFirstUpdate)
	{
		m_vForceDownValue = static_cast<float>(m_mpObj["VforceDownValue"].number_value());
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

#ifdef _DEBUG
 // _DEBUG

		// debugキー
		if (GetAsyncKeyState('L') & 0x8000)
		{
			m_vForce = 0;
		}
#endif

	}

	if (!(m_playerState & (fall | jump)))
	{
		m_bMove = false;
	}

	WeaponChangeProcess();

	if (!(m_playerState & (hit | rise)) && !m_bPlayerDeath)
	{
		EnemyRockOn();

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
		HitStateUpdate();
	}
	else if (m_playerState & rise)
	{
		m_bMove = true;
		if (m_spAnimator->IsAnimationEnd())
		{
			if (!(m_playerState & idle))
			{
				m_spAnimator->SetAnimation(m_spModel->GetAnimation("IdleA"), false);
			}
			m_playerState = idle;
		}
	}

	if (!(m_playerState & (grassHopperDash | grassHopperDashUp | hit | step)))
	{
		m_pos.y -= m_gravity;
		m_gravity += static_cast<float>((*m_wpJsonObj.lock())["GravityAcceleration"].number_value());
	}

	if (!m_bMove && !m_bPlayerDeath)
	{
		if (!(m_playerState & (fall | jump)))
		{
			if (!(m_playerState & idle))
			{
				m_spAnimator->SetAnimation(m_spModel->GetAnimation("IdleA"), false);
			}
			m_playerState = idle;
		}
	}

	// ========================================
	// 当たり判定
	// ========================================
	RayCollisionUpdate();
	SphereCollisionUpdate();

	if (!m_bRushRp)
	{
		if ((m_playerState & Player::PlayerState::rlAttackRush) && m_attackAnimeCnt >= m_mpObj["RushLastAttackPointTime"].int_value())
		{
			PlayerKickHitAttackChaeck();
		}
	}
	else
	{
		if ((m_playerState & Player::PlayerState::rlAttackRush) && m_attackAnimeCnt >= m_mpObj["RotationRushLastAttackPointTime"].int_value())
		{
			PlayerPanchiHitAttackChaeck();
		}
	}


	Math::Matrix mat;
#ifdef _DEBUG
	const KdModelWork::Node* node = nullptr;
	mat = Math::Matrix::Identity;

	node = m_spModel->FindNode("LegAttackPoint");
	mat = node->m_worldTransform * m_mWorld;
	mat._42 += static_cast<float>(m_mpObj["AddBottomYVal"].number_value());
	m_pDebugWire->AddDebugSphere
	(
		mat.Translation(),
		0.3f,
		kBlueColor
	);

	node = m_spModel->FindNode("LegAttackHitPoint");
	mat = node->m_worldTransform * m_mWorld;
	mat._42 += static_cast<float>(m_mpObj["AddBottomYVal"].number_value());
	m_pDebugWire->AddDebugSphere
	(
		mat.Translation(),
		0.3f,
		kBlueColor
	);

	node = m_spModel->FindNode("LegAttackHitPointTwo");
	mat = node->m_worldTransform * m_mWorld;
	mat._42 += static_cast<float>(m_mpObj["AddBottomYVal"].number_value());
	m_pDebugWire->AddDebugSphere
	(
		mat.Translation(),
		0.3f,
		kBlueColor
	);
#endif

	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_pos);
	Math::Matrix RotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y));
	m_mWorld = RotMat * transMat;

	for (auto& WeaList : m_weaponList)
	{
		WeaList->Update();
	}

	if (m_wpEnemy.expired())return;

	mat = Math::Matrix::CreateTranslation(m_wpEnemy.lock()->GetMatrix().Translation());
	if (m_wpEnemy.lock()->GetEnemyType() & (Enemy::EnemyType::wimpEnemyTypeOne | Enemy::EnemyType::bossEnemyTypeOne))
	{
		mat._41 += m_wpEnemy.lock()->GetRockOnPos().x;
		mat._42 += m_wpEnemy.lock()->GetRockOnPos().y;
		mat._43 += m_wpEnemy.lock()->GetRockOnPos().z;
	} 
	m_rockOnPolyMat = mat;
	auto& object = (*m_wpJsonObj.lock())["RockOn"].object_items();
	m_rockOnPolyMat._42 += static_cast<float>(object["AddValRockOnPosY"].number_value());
	m_rockOnPolyMat._41 += static_cast<float>(object["AddValRockOnPosX"].number_value()) * m_wpCamera.lock()->WorkCamera()->GetCameraMatrix().Forward().x;
	m_rockOnPolyMat._43 += static_cast<float>(object["AddValRockOnPosZ"].number_value()) * m_wpCamera.lock()->WorkCamera()->GetCameraMatrix().Forward().z;
	m_rockOnPolyMat.Backward(m_wpCamera.lock()->WorkCamera()->GetCameraMatrix().Backward());
	m_rockOnPolyMat.Up(m_wpCamera.lock()->WorkCamera()->GetCameraMatrix().Up());
	m_rockOnPolyMat.Right(m_wpCamera.lock()->WorkCamera()->GetCameraMatrix().Right());
}

// プレイヤーの向きの更新処理
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

// チュートリアル時限定の更新処理
void Player::TutorialUpdate()
{
	switch (m_wpUi.lock()->GetTutorialType())
	{
	case Ui::TutorialType::kihonTu:
		if (m_runAnimeCnt >= m_mpObj["RunTutorialAddCntMoment"].int_value())
		{
			m_wpUi.lock()->AddTutorialCnt();
		}

		if (m_playerState & jump && m_spAnimator->IsAnimationEnd())
		{
			m_wpUi.lock()->AddTutorialCnt();
		}
		break;
	case Ui::TutorialType::bukiTu:
		break;
	case Ui::TutorialType::sukoADMoveTu:
		if (m_playerState & (rlAttackThree | rlAttackRush | rAttackThree | lAttackThree | mantis) && m_spAnimator->IsAnimationEnd())
		{
			m_wpUi.lock()->AddTutorialCnt();
		}

		else if (m_playerState & defense)
		{
			m_tuGardTime++;
			if (m_tuGardTime >= m_mpObj["GardTutorialAddCntMoment"].int_value())
			{
				m_tuGardTime = 0;
				m_wpUi.lock()->AddTutorialCnt();
			}
		}

		break;
	case Ui::TutorialType::hopperTu:
		//                                                                                                                                                                                                       
		if (m_playerState & (grassHopperDash | grassHopperDashUp) && m_rGrassHopperTime == (*m_wpJsonObj.lock())["GrassDashSteppingAwayMoment"].int_value() || m_playerState & (grassHopperDash | grassHopperDashUp) && m_lGrassHopperTime == (*m_wpJsonObj.lock())["GrassDashSteppingAwayMoment"].int_value())
		{
			m_wpUi.lock()->AddTutorialCnt();
		}
		break;
	}
}

// PlayerStateがHit関係の時の更新処理
void Player::HitStateUpdate()
{
	--m_hitStopCnt;
	m_bMove = true;
	m_pos.y -= m_gravity;
	if (m_playerState & cutRaiseHit)
	{
		m_gravity += static_cast<float>((*m_wpJsonObj.lock())["CutRaiseHitGravityAcceleration"].number_value());
	}
	else
	{
		m_gravity += static_cast<float>((*m_wpJsonObj.lock())["GravityAcceleration"].number_value());
	}

	if (m_hitStopCnt <= 0)
	{
		m_hitStopCnt = 0;

		if (m_playerState & (nomalHit | cutRaiseHit))
		{
			if (!(m_playerState & idle))
			{
				m_spAnimator->SetAnimation(m_spModel->GetAnimation("IdleA"), false);
			}
			m_playerState = idle;
		}
		else if (m_playerState & blowingAwayHit)
		{
			if (!m_bBlowingAwayHitB)
			{
				if (!(m_playerState & blowingAwayRise))
				{
					m_spAnimator->SetAnimation(m_spModel->GetAnimation("BlowingAwayRise"), false);
				}
				m_playerState = blowingAwayRise;
			}
			else
			{
				m_bBlowingAwayHitB = false;
				if (!(m_playerState & iaiKiriRise))
				{
					m_spAnimator->SetAnimation(m_spModel->GetAnimation("IaiKiriRise"), false);
				}
				m_playerState = iaiKiriRise;
			}
		}
		else if (m_playerState & iaiKiriHit)
		{
			if (!(m_playerState & iaiKiriRise))
			{
				m_spAnimator->SetAnimation(m_spModel->GetAnimation("IaiKiriRise"), false);
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

		m_hitMoveSpd *= static_cast<float>((*m_wpJsonObj.lock())["MoveSpeedDecelerationamount"].number_value());
	}
	else
	{
		m_hitMoveSpd *= static_cast<float>((*m_wpJsonObj.lock())["MoveSpeedDecelerationamount"].number_value());
	}

	SpeedyMoveWallHitChack(m_hitMoveSpd, m_knockBackVec);
	m_pos += m_knockBackVec * m_hitMoveSpd;
}

// 更新後の処理
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
					m_spAnimator->SetAnimation(m_spModel->GetAnimation("FallA"), false);
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

		AnimationUpdate();

		for (auto& WeaList : m_weaponList)
		{
			WeaList->PostUpdate();
		}
	}
	else
	{
		m_spAnimator->AdvanceTime(m_spModel->WorkNodes());
		m_spModel->CalcNodeMatrices();

		if (!KdEffekseerManager::GetInstance().IsPlaying("BailOutPlayer.efk"))
		{
			if (m_bPlayerLose)
			{
				if (SceneManager::Instance().GetSceneType() != SceneManager::SceneType::training)
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
				else
				{
					m_isExpired = true;
				}
			}
		}

		if (m_spAnimator->IsAnimationEnd())
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

// アニメーションの更新処理
void Player::AnimationUpdate()
{
	if (m_bAttackAnimeDelay)
	{
		m_attackAnimeDelayCnt--;
		if (m_attackAnimeDelayCnt <= 0)
		{
			m_bAttackAnimeDelay = false;
			m_attackAnimeDelayCnt = 0;
		}
	}

	if (!m_spAnimator) return;
	if (!(m_playerState & (lAttack | rAttack | rlAttack | rlAttackRush)))
	{
		m_spAnimator->AdvanceTime(m_spModel->WorkNodes());
		m_spModel->CalcNodeMatrices();
		if (m_playerState & run)
		{
			++m_runAnimeCnt;
			if (m_runAnimeCnt == m_mpObj["FootfallPointMoment"][0].int_value())
			{
				KdAudioManager::Instance().Play("Asset/Audio/SE/FootstepsConcrete2.wav");
			}
			else if (m_runAnimeCnt == m_mpObj["FootfallPointMoment"][1].int_value())
			{
				KdAudioManager::Instance().Play("Asset/Audio/SE/FootstepsConcrete2.wav");
			}

			if (m_runAnimeCnt == m_mpObj["LastRunAnimationTime"].int_value())
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
				if (m_attackAnimeCnt == m_mpObj["AttackOneOrTwoShakenMoment"].int_value())
				{
					KdAudioManager::Instance().Play("Asset/Audio/SE/Swishes - SWSH 40, Swish, Combat, Weapon, Light.wav");
				}
			}

			if (m_attackAnimeCnt == m_mpObj["LastAttackAnimationMoment"].int_value())
			{
				m_bAttackAnimeDelay = true;
				m_bAttackAnimeCnt = false;
				m_attackAnimeCnt = 0;
				m_attackAnimeDelayCnt = m_mpObj["MaxAttackAnimeDelayCnt"].int_value();
			}
		}
		else
		{
			m_attackAnimeCnt++;
			if (m_playerState & (lAttackThree | rAttackThree))
			{
				if (m_attackAnimeCnt == m_mpObj["AttackThreeShakenMoment"].int_value())
				{
					KdAudioManager::Instance().Play("Asset/Audio/SE/Swishes - SWSH 40, Swish, Combat, Weapon, Light.wav");
				}
			}
		}

		m_spAnimator->AdvanceTime(m_spModel->WorkNodes());
		m_spModel->CalcNodeMatrices();
	}
	else if (m_playerState & (rlAttack | rlAttackRush))
	{
		if (m_bAttackAnimeCnt)
		{
			m_attackAnimeCnt++;
			if (m_playerState & rlAttackOne)
			{
				if (m_attackAnimeCnt == m_mpObj["RLAttackOneShakenFirstMoment"].int_value() || m_attackAnimeCnt == m_mpObj["RLAttackOneShakenSecondMoment"].int_value())
				{
					KdAudioManager::Instance().Play("Asset/Audio/SE/Swishes - SWSH 40, Swish, Combat, Weapon, Light.wav");
				}
			}
			else if (m_playerState & rlAttackTwo)
			{
				if (m_attackAnimeCnt == m_mpObj["RLAttackTwoShakenFirstMoment"].int_value() || m_attackAnimeCnt == m_mpObj["RLAttackTwoShakenSecondMoment"].int_value())
				{
					KdAudioManager::Instance().Play("Asset/Audio/SE/Swishes - SWSH 40, Swish, Combat, Weapon, Light.wav");
				}
			}
			else if (m_playerState & rlAttackThree)
			{
				if (m_attackAnimeCnt == m_mpObj["RLAttackThreeShakenFirstMoment"].int_value() || m_attackAnimeCnt == m_mpObj["RLAttackThreeShakenSecondMoment"].int_value())
				{
					KdAudioManager::Instance().Play("Asset/Audio/SE/Swishes - SWSH 40, Swish, Combat, Weapon, Light.wav");
				}
			}
			else if (m_playerState & rlAttackRush)
			{
				if (!m_bRushRp)
				{
					if (m_attackAnimeCnt == m_mpObj["RLAttackRushShakenMoment"][0].int_value() ||
						m_attackAnimeCnt == m_mpObj["RLAttackRushShakenMoment"][1].int_value() ||
						m_attackAnimeCnt == m_mpObj["RLAttackRushShakenMoment"][2].int_value() ||
						m_attackAnimeCnt == m_mpObj["RLAttackRushShakenMoment"][3].int_value() ||
						m_attackAnimeCnt == m_mpObj["RLAttackRushShakenMoment"][4].int_value() ||
						m_attackAnimeCnt == m_mpObj["RLAttackRushShakenMoment"][5].int_value() ||
						m_attackAnimeCnt == m_mpObj["RLAttackRushShakenMoment"][6].int_value())
					{
						KdAudioManager::Instance().Play("Asset/Audio/SE/Swishes - SWSH 40, Swish, Combat, Weapon, Light.wav");
					}
				}
				else if (m_attackAnimeCnt == m_mpObj["RLAttackRotationRushShakenMoment"][0].int_value() ||
					m_attackAnimeCnt == m_mpObj["RLAttackRotationRushShakenMoment"][1].int_value() ||
					m_attackAnimeCnt == m_mpObj["RLAttackRotationRushShakenMoment"][2].int_value() ||
					m_attackAnimeCnt == m_mpObj["RLAttackRotationRushShakenMoment"][3].int_value() ||
					m_attackAnimeCnt == m_mpObj["RLAttackRotationRushShakenMoment"][4].int_value() ||
					m_attackAnimeCnt == m_mpObj["RLAttackRotationRushShakenMoment"][5].int_value() ||
					m_attackAnimeCnt == m_mpObj["RLAttackRotationRushShakenMoment"][6].int_value() ||
					m_attackAnimeCnt == m_mpObj["RLAttackRotationRushShakenMoment"][7].int_value()
					)
				{
					KdAudioManager::Instance().Play("Asset/Audio/SE/Swishes - SWSH 40, Swish, Combat, Weapon, Light.wav");
				}
			}


			if (m_playerState & rlAttackOne)
			{
				if (m_attackAnimeCnt == m_mpObj["LastRLAttackAnimationdMoment"].int_value())
				{
					m_bAttackAnimeDelay = true;
					m_bAttackAnimeCnt = false;
					m_attackAnimeCnt = 0;
					m_attackAnimeDelayCnt = 10;
				}
			}
			else if (m_playerState & rlAttackTwo)
			{
				if (m_attackAnimeCnt == m_mpObj["LastRLAttackAnimationdMoment"].int_value())
				{
					m_bAttackAnimeDelay = true;
					m_bAttackAnimeCnt = false;
					m_attackAnimeCnt = 0;
					m_attackAnimeDelayCnt = 10;
				}
			}
			else if (m_playerState & rlAttackThree)
			{
				if (m_attackAnimeCnt == m_mpObj["LastRLAttackThreeAnimationMoment"].int_value())
				{
					m_bAttackAnimeDelay = true;
					m_bAttackAnimeCnt = false;
					m_attackAnimeCnt = 0;
					m_attackAnimeDelayCnt = 10;
				}
			}
		}

		m_spAnimator->AdvanceTime(m_spModel->WorkNodes());
		m_spModel->CalcNodeMatrices();
	}

}
//＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝

// 当たり判定＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
// レイ判定
void Player::RayCollisionUpdate()
{
	// レイ判定用に変数を作成
	KdCollider::RayInfo rayInfo;
	//rayInfo.m_pos = m_pos;
	rayInfo.m_pos = m_pos;
	rayInfo.m_pos.y += static_cast<float>(m_mpObj["AddBottomYVal"].number_value());
	if (!(m_playerState & (grassHopperDash | grassHopperDashUp)))
	{
		rayInfo.m_dir = { 0,-1,0 };
		/*rayInfo.m_pos.y += 0.1f;*/
		static float enableStepHight = static_cast<float>(m_mpObj["EnableStepHight"].number_value());
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
			m_pos = groundPos + Math::Vector3(0, -static_cast<float>(m_mpObj["AddBottomYVal"].number_value()), 0);
			m_gravity = 0;
			if (m_playerState & (fall | jump) && !m_bPlayerDeath)
			{
				m_bMove = false;
				if (!(m_playerState & idle))
				{
					m_spAnimator->SetAnimation(m_spModel->GetAnimation("IdleA"), false);
				}
				m_playerState = idle;
			}

			m_wpCamera.lock()->SetStepOnPlayerPos(groundPos);
		}
		else
		{
			m_gravity = 0;
			m_rGrassHopperTime = 0;
			m_lGrassHopperTime = 0;
			m_grassHopperDashDir = {};
			m_dashSpd = 0.0f;
		}
	}

	rayInfo.m_type = KdCollider::TypeRideEnemy;

#ifdef _DEBUG
	m_pDebugWire->AddDebugLine(rayInfo.m_pos, rayInfo.m_dir, rayInfo.m_range, { 1,1,1,1 });
#endif
	retRayList.clear();

	for (auto& enemy : m_enemyList)
	{
		if (enemy.expired())continue;
		if (enemy.lock()->GetBEnemyDeath())continue;

		enemy.lock()->Intersects
		(
			rayInfo,
			&retRayList
		);
	}

	// レイに当たったリストから一番近いオブジェクトを検出
	maxOverLap = 0;
	groundPos = {};
	hit = false;
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
			m_pos = groundPos + Math::Vector3(0, -static_cast<float>(m_mpObj["AddBottomYVal"].number_value()), 0);
			m_gravity = 0;
			if (m_playerState & (fall | jump) && !m_bPlayerDeath)
			{
				m_bMove = false;
				if (!(m_playerState & idle))
				{
					m_spAnimator->SetAnimation(m_spModel->GetAnimation("IdleA"), false);
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
}

// 球形当たり判定
void Player::SphereCollisionUpdate()
{
	KdCollider::SphereInfo sphereInfo;
	// 球の中心位置を設定
	sphereInfo.m_sphere.Center = m_pos + m_addCenterVal;
	// 球の半径を設定

	if (!(m_playerState & (grassHopperDash | grassHopperDashUp | step)))
	{
		sphereInfo.m_sphere.Radius = static_cast<float>(m_mpObj["TypeGroundToHitSphereRadius"].number_value());
	}
	else
	{
		sphereInfo.m_sphere.Radius = static_cast<float>(m_mpObj["TypeGroundToHitSphereRadiusIsGrassDash"].number_value());
	}

	// 当たり判定をしたいタイプを設定
	sphereInfo.m_type = KdCollider::TypeGround;
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

	for (auto& enemy : m_enemyList)
	{
		if (enemy.expired())continue;

		if (enemy.lock()->GetEnemyState() & Enemy::EnemyState::defense)
		{
			if (!(m_playerState & (grassHopperDash | grassHopperDashUp | step)))
			{
				sphereInfo.m_sphere.Radius = static_cast<float>(m_mpObj["TypeGardToHitSphereRadius"].number_value());
			}
			else
			{
				sphereInfo.m_sphere.Radius = static_cast<float>(m_mpObj["TypeGardToHitSphereRadiusIsGrassDash"].number_value());
			}

			// 当たり判定をしたいタイプを設定
			sphereInfo.m_type = KdCollider::TypeGard;
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
			for (auto& obj : enemy.lock()->GetWeaponList())
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
		sphereInfo.m_sphere.Center = m_pos + m_addCenterVal;
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

	for (auto& enemy : m_enemyList)
	{
		if (enemy.expired())continue;
		if (enemy.lock()->GetBEnemyDeath())continue;
		if (enemy.lock()->GetEnemyType() & Enemy::EnemyType::coarseFishEnemy)continue;

		if (enemy.lock()->GetEnemyType() & Enemy::EnemyType::bossEnemyTypeOne)
		{
			sphereInfo.m_sphere.Radius = static_cast<float>(m_mpObj["TypeBumpToHitSphereRadiusIsBossEnemyTypeOne"].number_value());
		}
		else
		{
			sphereInfo.m_sphere.Radius = static_cast<float>(m_mpObj["TypeBumpToHitSphereRadius"].number_value());
		}

		enemy.lock()->Intersects
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
			if (!(enemy.lock()->GetEnemyType() & Enemy::EnemyType::bossEnemyTypeOne) || m_playerState & (grassHopperDash | grassHopperDashUp | step))
			{
				hitDir.y = 0.0f;
			}

			hitDir.Normalize();
			// 球とモデルが当たっている
			m_pos += (hitDir * maxOverLap);
		}
	}

	sphereInfo.m_sphere.Radius = static_cast<float>(m_mpObj["TypeAttackDecSphereRadius"].number_value());

	// 当たり判定をしたいタイプを設定
	sphereInfo.m_type = KdCollider::TypeAttackDec;
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

	if (!m_wpEnemy.expired())
	{
		m_wpEnemy.lock()->Intersects
		(
			sphereInfo,
			&retSphereList
		);

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
			m_bAtttackMoveSpeedDec = true;
		}
		else
		{
			for (auto& enemy : m_enemyList)
			{
				if (enemy.expired())continue;
				if (enemy.lock()->GetBEnemyDeath())continue;

				enemy.lock()->Intersects
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
				m_bAtttackMoveSpeedDec = true;
			}
			else
			{
				m_bAtttackMoveSpeedDec = false;
			}
		}
	}
	else
	{
		for (auto& enemy : m_enemyList)
		{
			if (enemy.expired())continue;
			if (enemy.lock()->GetBEnemyDeath())continue;

			enemy.lock()->Intersects
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
			m_bAtttackMoveSpeedDec = true;
		}
		else
		{
			m_bAtttackMoveSpeedDec = false;
		}
	}
}

// 一定速度以上の時の当たり判定
void Player::SpeedyMoveWallHitChack(float& a_moveSpd, Math::Vector3 moveVec)
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

// プレイヤーのキックが当たってるかどうかの判定処理
void Player::PlayerKickHitAttackChaeck()
{
	const KdModelWork::Node* node = nullptr;
	Math::Matrix mat = Math::Matrix::Identity;

	for (auto& enemy : m_enemyList)
	{
		if (enemy.expired())continue;

		if (!enemy.lock()->GetAttackHit() && !enemy.lock()->GetDefenseSuc() && enemy.lock()->GetInvincibilityTimeCnt() == 0 && !enemy.lock()->GetBEnemyDeath()) // ここにはなくてもいいかも
		{
			/*if (player->GetPlayerState() & Player::PlayerState::rAttack && m_arrmType == lArrm)return;
			if (player->GetPlayerState() & Player::PlayerState::lAttack && m_arrmType == rArrm)return;*/

			node = m_spModel->FindNode("LegAttackPoint");
			KdCollider::SphereInfo sphereInfo;
			mat = node->m_worldTransform * m_mWorld;
			mat._42 += static_cast<float>(m_mpObj["AddBottomYVal"].number_value());
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

			enemy.lock()->Intersects
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
				enemy.lock()->BlowingAwayAttackOnHit(m_mWorld.Backward());
				KdAudioManager::Instance().Play("Asset/Audio/SE/KickAttackHit.wav");
				hitPos.y += 0.35f;
				KdEffekseerManager::GetInstance().Play("Hit3.efk", hitPos);
				KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("Hit3.efk"); // これでループしない
				Math::Matrix efcMat = Math::Matrix::CreateScale(0.5f) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)) * Math::Matrix::CreateTranslation(hitPos);
				KdEffekseerManager::GetInstance().SetWorldMatrix("Hit3.efk", efcMat);
			}
			else
			{
				node = m_spModel->FindNode("LegAttackHitPoint");
				sphereInfo;
				mat = node->m_worldTransform * m_mWorld;
				mat._42 += static_cast<float>(m_mpObj["AddBottomYVal"].number_value());
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

				enemy.lock()->Intersects
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
					enemy.lock()->BlowingAwayAttackOnHit(m_mWorld.Backward());
					KdAudioManager::Instance().Play("Asset/Audio/SE/KickAttackHit.wav");
					hitPos.y += 0.35f;
					KdEffekseerManager::GetInstance().Play("Hit3.efk", hitPos);
					KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("Hit3.efk"); // これでループしない
					Math::Matrix efcMat = Math::Matrix::CreateScale(0.5f) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)) * Math::Matrix::CreateTranslation(hitPos);
					KdEffekseerManager::GetInstance().SetWorldMatrix("Hit3.efk", efcMat);

				}
				else
				{
					node = m_spModel->FindNode("LegAttackHitPointTwo");
					sphereInfo;
					mat = node->m_worldTransform * m_mWorld;
					mat._42 += static_cast<float>(m_mpObj["AddBottomYVal"].number_value());
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

					enemy.lock()->Intersects
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
						enemy.lock()->BlowingAwayAttackOnHit(m_mWorld.Backward());
						KdAudioManager::Instance().Play("Asset/Audio/SE/KickAttackHit.wav");

						hitPos.y += 0.35f;
						KdEffekseerManager::GetInstance().Play("Hit3.efk", hitPos);
						KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("Hit3.efk"); // これでループしない
						Math::Matrix efcMat = Math::Matrix::CreateScale(0.5f) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)) * Math::Matrix::CreateTranslation(hitPos);
						KdEffekseerManager::GetInstance().SetWorldMatrix("Hit3.efk", efcMat);
					}
				}
			}
		}
	}
}

// プレイヤーのパンチが当たってるかどうかの判定処理
void Player::PlayerPanchiHitAttackChaeck()
{
	const KdModelWork::Node* node = nullptr;
	Math::Matrix mat = Math::Matrix::Identity;

	for (auto& enemy : m_enemyList)
	{
		if (enemy.expired())continue;

		if (!enemy.lock()->GetAttackHit() && !enemy.lock()->GetDefenseSuc() && enemy.lock()->GetInvincibilityTimeCnt() == 0 && !enemy.lock()->GetBEnemyDeath()) // ここにはなくてもいいかも
		{
			/*if (player->GetPlayerState() & Player::PlayerState::rAttack && m_arrmType == lArrm)return;
			if (player->GetPlayerState() & Player::PlayerState::lAttack && m_arrmType == rArrm)return;*/

			node = m_spModel->FindNode("ArrmAttackPoint");
			KdCollider::SphereInfo sphereInfo;
			mat = node->m_worldTransform * m_mWorld;
			mat._42 += static_cast<float>(m_mpObj["AddBottomYVal"].number_value());
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

			enemy.lock()->Intersects
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
				enemy.lock()->BlowingAwayAttackOnHit(m_mWorld.Backward());
				KdAudioManager::Instance().Play("Asset/Audio/SE/KickAttackHit.wav");
				hitPos.y += 0.35f;
				KdEffekseerManager::GetInstance().
					Play("Hit3.efk", hitPos);
				KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("Hit3.efk"); // これでループしない
				Math::Matrix efcMat = Math::Matrix::CreateScale(0.5f) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)) * Math::Matrix::CreateTranslation(hitPos);
				KdEffekseerManager::GetInstance().SetWorldMatrix("Hit3.efk", efcMat);
			}
			else
			{
				node = m_spModel->FindNode("ArrmAttackHitPoint");
				sphereInfo;
				mat = node->m_worldTransform * m_mWorld;
				mat._42 += static_cast<float>(m_mpObj["AddBottomYVal"].number_value());
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

				enemy.lock()->Intersects
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
					enemy.lock()->BlowingAwayAttackOnHit(m_mWorld.Backward());
					KdAudioManager::Instance().Play("Asset/Audio/SE/KickAttackHit.wav");
					hitPos.y += 0.35f;
					KdEffekseerManager::GetInstance().
						Play("Hit3.efk", hitPos);
					KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("Hit3.efk"); // これでループしない
					Math::Matrix efcMat = Math::Matrix::CreateScale(0.5f) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)) * Math::Matrix::CreateTranslation(hitPos);
					KdEffekseerManager::GetInstance().SetWorldMatrix("Hit3.efk", efcMat);

				}
				else
				{
					node = m_spModel->FindNode("ArrmAttackHitPointTwo");
					sphereInfo;
					mat = node->m_worldTransform * m_mWorld;
					mat._42 += static_cast<float>(m_mpObj["AddBottomYVal"].number_value());
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

					enemy.lock()->Intersects
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
						enemy.lock()->BlowingAwayAttackOnHit(m_mWorld.Backward());
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
//＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
 
// 攻撃が当たった時の処理＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
// 通常の攻撃が当たった時の処理
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
	for (auto& enemy : m_enemyList)
	{
		if (enemy.expired())continue;
		if (enemy.lock()->GetEnemyState() & Enemy::EnemyState::rlAttackThree)
		{
			m_hitStopCnt = 60;
		}
		else
		{
			m_hitStopCnt = 40;
		}
	}

	m_hitMoveSpd = 0.05f;
	m_hitColorChangeTimeCnt = (*m_wpJsonObj.lock())["HitColorChangeTimeCnt"].int_value();
	m_knockBackVec = a_KnocBackvec;
	m_endurance -= (*m_wpJsonObj.lock())["OnHitDamage"].int_value();
	m_attackHit = true;

	for (auto& enemy : m_enemyList)
	{
		if (enemy.expired())continue;

		if (enemy.lock()->GetEnemyState() & (Enemy::EnemyState::rAttackOne | Enemy::EnemyState::rlAttackOne |
			Enemy::EnemyState::rlAttackThree))
		{
			m_spAnimator->SetAnimation(m_spModel->GetAnimation("RHit1"), false);
			if (SceneManager::Instance().GetUpdateStopCnt() == 0)
			{
				SceneManager::Instance().SetUpdateStopCnt(5); // これでアップデートを一時止める
			}
		}
		else if (enemy.lock()->GetEnemyState() & (Enemy::EnemyState::rAttackTwo | Enemy::EnemyState::rlAttackTwo))
		{
			m_spAnimator->SetAnimation(m_spModel->GetAnimation("RHit2"), false);
			if (SceneManager::Instance().GetUpdateStopCnt() == 0)
			{
				SceneManager::Instance().SetUpdateStopCnt(5); // これでアップデートを一時止める
			}
		}

		if (enemy.lock()->GetEnemyState() & Enemy::EnemyState::rlAttackRush && enemy.lock()->GetAnimationCnt() < 8 ||
			(enemy.lock()->GetAnimationCnt() >= 21 && enemy.lock()->GetAnimationCnt() < 41))
		{
			m_spAnimator->SetAnimation(m_spModel->GetAnimation("RHit1"), false);

			if (SceneManager::Instance().GetUpdateStopCnt() == 0)
			{
				SceneManager::Instance().SetUpdateStopCnt(2); // これでアップデートを一時止める
			}
		}
		else if (enemy.lock()->GetEnemyState() & Enemy::EnemyState::rlAttackRush &&
			(enemy.lock()->GetAnimationCnt() >= 8 && enemy.lock()->GetAnimationCnt() < 21))
		{
			m_spAnimator->SetAnimation(m_spModel->GetAnimation("RHit2"), false);

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

// 吹き飛ばし攻撃が当たった時の処理
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
	for (auto& enemy : m_enemyList)
	{
		if (enemy.expired())continue;

		if (enemy.lock()->GetEnemyState() & Enemy::EnemyState::rlAttackRush && enemy.lock()->GetAnimationCnt() >= 107 || enemy.lock()->GetNotHumanoidEnemyState() & Enemy::NotHumanoidEnemyState::rotationAttack)
		{
			m_hitMoveSpd = 0.95f;
			cnt++;
			if (SceneManager::Instance().GetUpdateStopCnt() == 0)
			{
				SceneManager::Instance().SetUpdateStopCnt(15); // これでアップデートを一時止める
			}
			break;
		}
		else if (enemy.lock()->GetEnemyState() & (Enemy::EnemyState::rAttackOne | Enemy::EnemyState::lAttackOne) && enemy.lock()->GetEnemyState() & (Enemy::EnemyState::grassHopperDashF))
		{
			m_hitMoveSpd = 0.35f;
			cnt++;
			if (SceneManager::Instance().GetUpdateStopCnt() == 0)
			{
				SceneManager::Instance().SetUpdateStopCnt(15); // これでアップデートを一時止める
			}
			break;
		}
		else
		{
			if (SceneManager::Instance().GetUpdateStopCnt() == 0)
			{
				SceneManager::Instance().SetUpdateStopCnt(8); // これでアップデートを一時止める
			}
			break;
		}
	}

	if (cnt == 0)
	{
		m_hitMoveSpd = 0.3f;
	}

	m_playerState = blowingAwayHit;
	m_hitStopCnt = 40;
	m_hitColorChangeTimeCnt = (*m_wpJsonObj.lock())["HitColorChangeTimeCnt"].int_value();
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
		m_spAnimator->SetAnimation(m_spModel->GetAnimation(" BlowingAwayHitB"), false);
	}
	else
	{
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("BlowingAwayHitBB"), false);
		m_bBlowingAwayHitB = true;
	}

	m_invincibilityTimeCnt = (*m_wpJsonObj.lock())["InvincibilityMaxTimeCnt"].int_value();
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

// 居合切り系の攻撃が当たった時の処理
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
	m_hitColorChangeTimeCnt = (*m_wpJsonObj.lock())["HitColorChangeTimeCnt"].int_value();
	m_hitMoveSpd = 0.0f;
	m_knockBackVec = a_KnocBackvec;
	m_endurance -= 50.0f;
	m_attackHit = true;
	m_spAnimator->SetAnimation(m_spModel->GetAnimation("IaiKiriAttackHitB"), false);
	m_invincibilityTimeCnt = (*m_wpJsonObj.lock())["InvincibilityMaxTimeCnt"].int_value();
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

// 切り上げ攻撃が当たった時の処理
void Player::CutRaiseOnHit(Math::Vector3 a_KnocBackvec)
{
	if (m_playerState & mantis)
	{
		const std::shared_ptr<Scopion> scopion = std::dynamic_pointer_cast<Scopion>(m_weaponList[1]);
		scopion->SetBMantis(false);
		const std::shared_ptr<Scopion> scopion2 = std::dynamic_pointer_cast<Scopion>(m_weaponList[0]);
		scopion2->SetBMantis(false);
	}

	m_playerState = cutRaiseHit;
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
	m_attackHit = true;
	m_spAnimator->SetAnimation(m_spModel->GetAnimation("CutRaiseHit"), false);
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
//＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝

// 攻撃が防がれた時の処理
void Player::HasDefense()
{
	if (m_playerState & (rAttack | rlAttack))
	{
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("RHasDefense"), false);
	}
	else if (m_playerState & lAttack)
	{
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("LHasDefense"), false);
	}

	if (m_bRushRp)
	{
		m_bRushRp = false;
	}

	m_playerState = hasDefense;
}

// 描画関係＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
// スキンメッシュアニメーションするモデルの影描画
void Player::GenerateDepthMapFromLight_SkinMesh()
{
	if (m_bPlayerLose)return;
	if (!m_spModel) return;

	KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_spModel, m_mWorld);
}

// スキンメッシュアニメーションするモデルの描画
void Player::DrawLit_SkinMesh()
{
	if (m_bPlayerLose)return;
	if (!m_spModel) return;

	if (m_invincibilityTimeCnt <= (*m_wpJsonObj.lock())["InvincibilityProgramTimeCnt"][0].int_value()  && m_invincibilityTimeCnt > (*m_wpJsonObj.lock())["InvincibilityProgramTimeCnt"][1].int_value()  ||
		m_invincibilityTimeCnt <= (*m_wpJsonObj.lock())["InvincibilityProgramTimeCnt"][2].int_value()  && m_invincibilityTimeCnt > (*m_wpJsonObj.lock())["InvincibilityProgramTimeCnt"][3].int_value()  ||
		m_invincibilityTimeCnt <= (*m_wpJsonObj.lock())["InvincibilityProgramTimeCnt"][4].int_value()  && m_invincibilityTimeCnt > (*m_wpJsonObj.lock())["InvincibilityProgramTimeCnt"][5].int_value()  ||
		m_invincibilityTimeCnt <= (*m_wpJsonObj.lock())["InvincibilityProgramTimeCnt"][6].int_value()  && m_invincibilityTimeCnt > (*m_wpJsonObj.lock())["InvincibilityProgramTimeCnt"][7].int_value()  ||
		m_invincibilityTimeCnt <= (*m_wpJsonObj.lock())["InvincibilityProgramTimeCnt"][8].int_value()  && m_invincibilityTimeCnt > (*m_wpJsonObj.lock())["InvincibilityProgramTimeCnt"][9].int_value()  ||
		m_invincibilityTimeCnt <= (*m_wpJsonObj.lock())["InvincibilityProgramTimeCnt"][10].int_value() && m_invincibilityTimeCnt > (*m_wpJsonObj.lock())["InvincibilityProgramTimeCnt"][11].int_value() ||
		m_invincibilityTimeCnt == (*m_wpJsonObj.lock())["InvincibilityProgramTimeCnt"][12].int_value()
		)return;

	KdShaderManager::Instance().m_HD2DShader.SetOutLineColor({ 0,0,1 });
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

// デバックラインの描画
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

// 光の影響を受けるモデルの描画
void Player::DrawLit()
{
	if (m_bPlayerLose)return;
	if (!m_bEwaponDrawPosible)return;

	for (auto& WeaList : m_weaponList)
	{
		WeaList->DrawLit();
	}
}

// 光の影響を受けないオブジェクトの描画
void Player::DrawUnLit()
{
	if (m_bPlayerLose)return;
	if (m_wpEnemy.expired())return;
	if (m_wpEnemy.lock()->GetBEnemyDeath())return;
	KdShaderManager::Instance().m_HD2DShader.DrawPolygon(m_rocKOnPolygon, m_rockOnPolyMat);
}

// 自己発光させるモデルの描画
void Player::DrawBright()
{
	if (m_bPlayerLose)return;
	if (!m_bEwaponDrawPosible)return;

	for (auto& WeaList : m_weaponList)
	{
		WeaList->DrawBright();
	}
}

// スキンメッシュしないモデルの影描画
void Player::GenerateDepthMapFromLight()
{
	if (!m_bEwaponDrawPosible)return;
	if (m_bPlayerLose)return;

	for (auto& WeaList : m_weaponList)
	{
		WeaList->GenerateDepthMapFromLight();
	}
}
//＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝

// 武器装備時の行動選択＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
// Hoperr装備時の動きを決める処理
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
					m_playerState |= grassHopperDashF;
					m_playerState &= grassHopperDashF;
					m_grasRotYMat = m_wpCamera.lock()->GetRotationYMatrix();
					m_rGrassHopperPauCnt = GRASS_HOPPER_PAUCNT;
					m_rGrassHopperTime = (*m_wpJsonObj.lock())["GrassDashMaxTime"].int_value();
					m_lGrassHopperTime = 0;
					m_gravity = 0;
					m_vForce -= m_vForceDownValue;
					m_spAnimator->SetAnimation(m_spModel->GetAnimation("GrassDashFB"), false);

					node = m_spModel->FindNode("GrassHopperLegRPoint");
					mat = node->m_worldTransform * m_mWorld;
					mat._42 += static_cast<float>(m_mpObj["AddBottomYVal"].number_value());
					m_weaponList[2]->GrassHopper({ mat._41,mat._42,mat._43 }, spCamera->GetYAng());

					KdEffekseerManager::GetInstance().
						Play("GrassDashBlurPlayer.efk", { m_pos.x + m_addGrassDashEffectPosVal.x * vec.x,m_pos.y + m_addGrassDashEffectPosVal.y,m_pos.z + m_addGrassDashEffectPosVal.z * vec.z });
					KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("GrassDashBlurPlayer.efk"); // これでループしない
					Math::Matrix efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)) * Math::Matrix::CreateTranslation({ m_pos.x + m_addGrassDashEffectPosVal.x * vec.x,m_pos.y + m_addGrassDashEffectPosVal.y,m_pos.z + m_addGrassDashEffectPosVal.z * vec.z });
					KdEffekseerManager::GetInstance().SetWorldMatrix("GrassDashBlurPlayer.efk", efcMat);
				}
				else if (KdInputManager::Instance().GetButtonState("left"))
				{
					m_grassHopperDashDir = Math::Vector3::TransformNormal(Math::Vector3(-1, 0, 0), m_wpCamera.lock()->GetRotationYMatrix());
					m_bMove = true;
					m_playerState |= grassHopperDashL;
					m_playerState &= grassHopperDashL;
					m_grasRotYMat = m_wpCamera.lock()->GetRotationYMatrix();
					m_rGrassHopperPauCnt = GRASS_HOPPER_PAUCNT;
					m_rGrassHopperTime = (*m_wpJsonObj.lock())["GrassDashMaxTime"].int_value();
					m_lGrassHopperTime = 0;
					m_gravity = 0;
					m_vForce -= m_vForceDownValue;
					m_spAnimator->SetAnimation(m_spModel->GetAnimation("GrassDashLB"), false);

					node = m_spModel->FindNode("GrassHopperLegRPoint");
					mat = node->m_worldTransform * m_mWorld;
					mat._42 += static_cast<float>(m_mpObj["AddBottomYVal"].number_value());
					m_weaponList[2]->GrassHopper({ mat._41,mat._42,mat._43 }, spCamera->GetYAng());

					vec = m_mWorld.Left();
					KdEffekseerManager::GetInstance().
						Play("GrassDashBlurPlayer.efk", { m_pos.x + m_addGrassDashEffectPosVal.x * vec.x,m_pos.y + m_addGrassDashEffectPosVal.y,m_pos.z + m_addGrassDashEffectPosVal.z * vec.z });
					KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("GrassDashBlurPlayer.efk"); // これでループしない
					Math::Matrix efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y + 90.0f)) * Math::Matrix::CreateTranslation({ m_pos.x + m_addGrassDashEffectPosVal.x * vec.x,m_pos.y + m_addGrassDashEffectPosVal.y,m_pos.z + m_addGrassDashEffectPosVal.z * vec.z });
					KdEffekseerManager::GetInstance().SetWorldMatrix("GrassDashBlurPlayer.efk", efcMat);
				}

				else if (KdInputManager::Instance().GetButtonState("backward"))
				{
					m_grassHopperDashDir = Math::Vector3::TransformNormal(Math::Vector3(0, 0, -1), m_wpCamera.lock()->GetRotationYMatrix());
					m_bMove = true;
					m_playerState |= grassHopperDashB;
					m_playerState &= grassHopperDashB;
					m_grasRotYMat = m_wpCamera.lock()->GetRotationYMatrix();
					m_rGrassHopperPauCnt = GRASS_HOPPER_PAUCNT;
					m_rGrassHopperTime = (*m_wpJsonObj.lock())["GrassDashMaxTime"].int_value();
					m_lGrassHopperTime = 0;
					m_gravity = 0;
					m_vForce -= m_vForceDownValue;
					m_spAnimator->SetAnimation(m_spModel->GetAnimation("GrassDashBB"), false);

					node = m_spModel->FindNode("GrassHopperLegRPoint");
					mat = node->m_worldTransform * m_mWorld;
					mat._42 += static_cast<float>(m_mpObj["AddBottomYVal"].number_value());
					m_weaponList[2]->GrassHopper({ mat._41,mat._42,mat._43 }, spCamera->GetYAng());

					vec = m_mWorld.Forward();
					KdEffekseerManager::GetInstance().
						Play("GrassDashBlurPlayer.efk", { m_pos.x + m_addGrassDashEffectPosVal.x * vec.x,m_pos.y + m_addGrassDashEffectPosVal.y,m_pos.z + m_addGrassDashEffectPosVal.z * vec.z });
					KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("GrassDashBlurPlayer.efk"); // これでループしない
					Math::Matrix efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y + 180.0f)) * Math::Matrix::CreateTranslation({ m_pos.x + m_addGrassDashEffectPosVal.x * vec.x,m_pos.y + m_addGrassDashEffectPosVal.y,m_pos.z + m_addGrassDashEffectPosVal.z * vec.z });
					KdEffekseerManager::GetInstance().SetWorldMatrix("GrassDashBlurPlayer.efk", efcMat);
				}
				else if (KdInputManager::Instance().GetButtonState("right"))
				{
					m_grassHopperDashDir = Math::Vector3::TransformNormal(Math::Vector3(1, 0, 0), m_wpCamera.lock()->GetRotationYMatrix());
					m_bMove = true;
					m_playerState |= grassHopperDashR;
					m_playerState &= grassHopperDashR;
					m_grasRotYMat = m_wpCamera.lock()->GetRotationYMatrix();
					m_rGrassHopperPauCnt = GRASS_HOPPER_PAUCNT;
					m_rGrassHopperTime = (*m_wpJsonObj.lock())["GrassDashMaxTime"].int_value();
					m_lGrassHopperTime = 0;
					m_gravity = 0;
					m_vForce -= m_vForceDownValue;
					m_spAnimator->SetAnimation(m_spModel->GetAnimation("GrassDashRB"), false);

					node = m_spModel->FindNode("GrassHopperLegLPoint");
					mat = node->m_worldTransform * m_mWorld;
					mat._42 += static_cast<float>(m_mpObj["AddBottomYVal"].number_value());
					m_weaponList[2]->GrassHopper({ mat._41,mat._42,mat._43 }, spCamera->GetYAng());

					vec = m_mWorld.Right();
					KdEffekseerManager::GetInstance().
						Play("GrassDashBlurPlayer.efk", { m_pos.x + m_addGrassDashEffectPosVal.x * vec.x,m_pos.y + m_addGrassDashEffectPosVal.y,m_pos.z + m_addGrassDashEffectPosVal.z * vec.z });
					KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("GrassDashBlurPlayer.efk"); // これでループしない
					Math::Matrix efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y + 270.0f)) * Math::Matrix::CreateTranslation({ m_pos.x + m_addGrassDashEffectPosVal.x * vec.x,m_pos.y + m_addGrassDashEffectPosVal.y,m_pos.z + m_addGrassDashEffectPosVal.z * vec.z });
					KdEffekseerManager::GetInstance().SetWorldMatrix("GrassDashBlurPlayer.efk", efcMat);
				}
				else if (KdInputManager::Instance().GetButtonState("jump"))
				{
					m_grassHopperDashDir = { 0, 1, 0 };
					m_grassHopperDashDir.Normalize();
					m_bMove = true;
					m_playerState |= grassHopperDashUp;
					m_playerState &= grassHopperDashUp;
					m_grasRotYMat = m_wpCamera.lock()->GetRotationYMatrix();
					m_rGrassHopperPauCnt = GRASS_HOPPER_PAUCNT;
					m_rGrassHopperTime = (*m_wpJsonObj.lock())["GrassDashMaxTime"].int_value();
					m_lGrassHopperTime = 0;
					m_gravity = 0;
					m_vForce -= m_vForceDownValue;
					m_spAnimator->SetAnimation(m_spModel->GetAnimation("GrassDashJB"), false);

					node = m_spModel->FindNode("GrassHopperLegRPoint");
					mat = node->m_worldTransform * m_mWorld;
					mat._42 += static_cast<float>(m_mpObj["AddBottomYVal"].number_value());
					m_weaponList[2]->GrassHopper({ mat._41,mat._42,mat._43 }, spCamera->GetYAng());

					KdEffekseerManager::GetInstance().
						Play("GrassDashBlurPlayer.efk", { m_pos.x,m_pos.y + static_cast<float>(m_mpObj["AddGrassDashUpEffectPosYVal"].number_value()),m_pos.z });
					KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("GrassDashBlurPlayer.efk"); // これでループしない
					Math::Matrix efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(270.0f)) * Math::Matrix::CreateTranslation({ m_pos.x,m_pos.y + static_cast<float>(m_mpObj["AddGrassDashUpEffectPosYVal"].number_value()),m_pos.z });
					KdEffekseerManager::GetInstance().SetWorldMatrix("GrassDashBlurPlayer.efk", efcMat);
				}
				else
				{
					m_grassHopperDashDir = { 0, 1, 0 };
					m_grassHopperDashDir.Normalize();
					m_bMove = true;
					m_playerState |= grassHopperDashUp;
					m_playerState &= grassHopperDashUp;

					m_grasRotYMat = m_wpCamera.lock()->GetRotationYMatrix();
					m_rGrassHopperPauCnt = GRASS_HOPPER_PAUCNT;
					m_rGrassHopperTime = (*m_wpJsonObj.lock())["GrassDashMaxTime"].int_value();
					m_lGrassHopperTime = 0;
					m_gravity = 0;
					m_vForce -= m_vForceDownValue;
					m_spAnimator->SetAnimation(m_spModel->GetAnimation("GrassDashJB"), false);

					node = m_spModel->FindNode("GrassHopperLegRPoint");
					mat = node->m_worldTransform * m_mWorld;
					mat._42 += static_cast<float>(m_mpObj["AddBottomYVal"].number_value());
					m_weaponList[2]->GrassHopper({ mat._41,mat._42,mat._43 }, spCamera->GetYAng());

					KdEffekseerManager::GetInstance().
						Play("GrassDashBlurPlayer.efk", { m_pos.x,m_pos.y + static_cast<float>(m_mpObj["AddGrassDashUpEffectPosYVal"].number_value()),m_pos.z });
					KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("GrassDashBlurPlayer.efk"); // これでループしない
					Math::Matrix efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(270.0f)) * Math::Matrix::CreateTranslation({ m_pos.x,m_pos.y + static_cast<float>(m_mpObj["AddGrassDashUpEffectPosYVal"].number_value()),m_pos.z });
					KdEffekseerManager::GetInstance().SetWorldMatrix("GrassDashBlurPlayer.efk", efcMat);
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
					m_playerState |= grassHopperDashF;
					m_playerState &= grassHopperDashF;
					m_grasRotYMat = m_wpCamera.lock()->GetRotationYMatrix();
					m_lGrassHopperPauCnt = GRASS_HOPPER_PAUCNT;
					m_lGrassHopperTime = (*m_wpJsonObj.lock())["GrassDashMaxTime"].int_value();
					m_rGrassHopperTime = 0;
					m_gravity = 0;
					m_vForce -= m_vForceDownValue;
					m_spAnimator->SetAnimation(m_spModel->GetAnimation("GrassDashFB"), false);

					node = m_spModel->FindNode("GrassHopperLegRPoint");
					mat = node->m_worldTransform * m_mWorld;
					mat._42 += static_cast<float>(m_mpObj["AddBottomYVal"].number_value());
					m_weaponList[3]->GrassHopper({ mat._41,mat._42,mat._43 }, spCamera->GetYAng());

					KdEffekseerManager::GetInstance().
						Play("GrassDashBlurPlayer.efk", { m_pos.x + m_addGrassDashEffectPosVal.x * vec.x,m_pos.y + m_addGrassDashEffectPosVal.y,m_pos.z + m_addGrassDashEffectPosVal.z * vec.z });
					KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("GrassDashBlurPlayer.efk"); // これでループしない
					Math::Matrix efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)) * Math::Matrix::CreateTranslation({ m_pos.x + m_addGrassDashEffectPosVal.x * vec.x,m_pos.y + m_addGrassDashEffectPosVal.y,m_pos.z + m_addGrassDashEffectPosVal.z * vec.z });
					KdEffekseerManager::GetInstance().SetWorldMatrix("GrassDashBlurPlayer.efk", efcMat);
				}
				else if (KdInputManager::Instance().GetButtonState("left"))
				{
					m_grassHopperDashDir = Math::Vector3::TransformNormal(Math::Vector3(-1, 0, 0), m_wpCamera.lock()->GetRotationYMatrix());
					m_bMove = true;
					m_playerState |= grassHopperDashL;
					m_playerState &= grassHopperDashL;
					m_grasRotYMat = m_wpCamera.lock()->GetRotationYMatrix();
					m_lGrassHopperPauCnt = GRASS_HOPPER_PAUCNT;
					m_lGrassHopperTime = (*m_wpJsonObj.lock())["GrassDashMaxTime"].int_value();
					m_rGrassHopperTime = 0;
					m_gravity = 0;
					m_vForce -= m_vForceDownValue;
					m_spAnimator->SetAnimation(m_spModel->GetAnimation("GrassDashLB"), false);

					node = m_spModel->FindNode("GrassHopperLegRPoint");
					mat = node->m_worldTransform * m_mWorld;
					mat._42 += static_cast<float>(m_mpObj["AddBottomYVal"].number_value());
					m_weaponList[3]->GrassHopper({ mat._41,mat._42,mat._43 }, spCamera->GetYAng());

					vec = m_mWorld.Left();
					KdEffekseerManager::GetInstance().
						Play("GrassDashBlurPlayer.efk", { m_pos.x + m_addGrassDashEffectPosVal.x * vec.x,m_pos.y + m_addGrassDashEffectPosVal.y,m_pos.z + m_addGrassDashEffectPosVal.z * vec.z });
					KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("GrassDashBlurPlayer.efk"); // これでループしない
					Math::Matrix efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y + 90.0f)) * Math::Matrix::CreateTranslation({ m_pos.x + m_addGrassDashEffectPosVal.x * vec.x,m_pos.y + m_addGrassDashEffectPosVal.y,m_pos.z + m_addGrassDashEffectPosVal.z * vec.z });
					KdEffekseerManager::GetInstance().SetWorldMatrix("GrassDashBlurPlayer.efk", efcMat);
				}

				else if (KdInputManager::Instance().GetButtonState("backward"))
				{
					m_grassHopperDashDir = Math::Vector3::TransformNormal(Math::Vector3(0, 0, -1), m_wpCamera.lock()->GetRotationYMatrix());
					m_bMove = true;
					m_playerState |= grassHopperDashB;
					m_playerState &= grassHopperDashB;
					m_grasRotYMat = m_wpCamera.lock()->GetRotationYMatrix();
					m_lGrassHopperPauCnt = GRASS_HOPPER_PAUCNT;
					m_lGrassHopperTime = (*m_wpJsonObj.lock())["GrassDashMaxTime"].int_value();
					m_rGrassHopperTime = 0;
					m_gravity = 0;
					m_vForce -= m_vForceDownValue;
					m_spAnimator->SetAnimation(m_spModel->GetAnimation("GrassDashBB"), false);

					node = m_spModel->FindNode("GrassHopperLegRPoint");
					mat = node->m_worldTransform * m_mWorld;
					mat._42 += static_cast<float>(m_mpObj["AddBottomYVal"].number_value());
					m_weaponList[3]->GrassHopper({ mat._41,mat._42,mat._43 }, spCamera->GetYAng());

					vec = m_mWorld.Forward();
					KdEffekseerManager::GetInstance().
						Play("GrassDashBlurPlayer.efk", { m_pos.x + m_addGrassDashEffectPosVal.x * vec.x,m_pos.y + m_addGrassDashEffectPosVal.y,m_pos.z + m_addGrassDashEffectPosVal.z * vec.z });
					KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("GrassDashBlurPlayer.efk"); // これでループしない
					Math::Matrix efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y + 180.0f)) * Math::Matrix::CreateTranslation({ m_pos.x + m_addGrassDashEffectPosVal.x * vec.x,m_pos.y + m_addGrassDashEffectPosVal.y,m_pos.z + m_addGrassDashEffectPosVal.z * vec.z });
					KdEffekseerManager::GetInstance().SetWorldMatrix("GrassDashBlurPlayer.efk", efcMat);
				}
				else if (KdInputManager::Instance().GetButtonState("right"))
				{
					m_grassHopperDashDir = Math::Vector3::TransformNormal(Math::Vector3(1, 0, 0), m_wpCamera.lock()->GetRotationYMatrix());
					m_bMove = true;
					m_playerState |= grassHopperDashR;
					m_playerState &= grassHopperDashR;
					m_grasRotYMat = m_wpCamera.lock()->GetRotationYMatrix();
					m_lGrassHopperPauCnt = GRASS_HOPPER_PAUCNT;
					m_lGrassHopperTime = (*m_wpJsonObj.lock())["GrassDashMaxTime"].int_value();
					m_rGrassHopperTime = 0;
					m_gravity = 0;
					m_vForce -= m_vForceDownValue;
					m_spAnimator->SetAnimation(m_spModel->GetAnimation("GrassDashRB"), false);

					node = m_spModel->FindNode("GrassHopperLegLPoint");
					mat = node->m_worldTransform * m_mWorld;
					mat._42 += static_cast<float>(m_mpObj["AddBottomYVal"].number_value());
					m_weaponList[3]->GrassHopper({ mat._41,mat._42,mat._43 }, spCamera->GetYAng());

					vec = m_mWorld.Right();
					KdEffekseerManager::GetInstance().
						Play("GrassDashBlurPlayer.efk", { m_pos.x + m_addGrassDashEffectPosVal.x * vec.x,m_pos.y + m_addGrassDashEffectPosVal.y,m_pos.z + m_addGrassDashEffectPosVal.z * vec.z });
					KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("GrassDashBlurPlayer.efk"); // これでループしない
					Math::Matrix efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y + 270.0f)) * Math::Matrix::CreateTranslation({ m_pos.x + m_addGrassDashEffectPosVal.x * vec.x,m_pos.y + m_addGrassDashEffectPosVal.y,m_pos.z + m_addGrassDashEffectPosVal.z * vec.z });
					KdEffekseerManager::GetInstance().SetWorldMatrix("GrassDashBlurPlayer.efk", efcMat);
				}
				else if (KdInputManager::Instance().GetButtonState("jump"))
				{
					m_grassHopperDashDir = { 0, 1, 0 };
					m_grassHopperDashDir.Normalize();
					m_bMove = true;
					m_playerState |= grassHopperDashUp;
					m_playerState &= grassHopperDashUp;
					m_grasRotYMat = m_wpCamera.lock()->GetRotationYMatrix();
					m_lGrassHopperPauCnt = GRASS_HOPPER_PAUCNT;
					m_lGrassHopperTime = (*m_wpJsonObj.lock())["GrassDashMaxTime"].int_value();
					m_rGrassHopperTime = 0;
					m_gravity = 0;
					m_vForce -= m_vForceDownValue;
					m_spAnimator->SetAnimation(m_spModel->GetAnimation("GrassDashJB"), false);

					node = m_spModel->FindNode("GrassHopperLegRPoint");
					mat = node->m_worldTransform * m_mWorld;
					mat._42 += static_cast<float>(m_mpObj["AddBottomYVal"].number_value());
					m_weaponList[3]->GrassHopper({ mat._41,mat._42,mat._43 }, spCamera->GetYAng());

					KdEffekseerManager::GetInstance().
						Play("GrassDashBlurPlayer.efk", { m_pos.x,m_pos.y + static_cast<float>(m_mpObj["AddGrassDashUpEffectPosYVal"].number_value()),m_pos.z });
					KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("GrassDashBlurPlayer.efk"); // これでループしない
					Math::Matrix efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(270.0f)) * Math::Matrix::CreateTranslation({ m_pos.x,m_pos.y + static_cast<float>(m_mpObj["AddGrassDashUpEffectPosYVal"].number_value()),m_pos.z });
					KdEffekseerManager::GetInstance().SetWorldMatrix("GrassDashBlurPlayer.efk", efcMat);
				}
				else
				{
					m_grassHopperDashDir = { 0, 1, 0 };
					m_grassHopperDashDir.Normalize();
					m_bMove = true;
					m_playerState |= grassHopperDashUp;
					m_playerState &= grassHopperDashUp;
					m_grasRotYMat = m_wpCamera.lock()->GetRotationYMatrix();
					m_lGrassHopperPauCnt = GRASS_HOPPER_PAUCNT;
					m_lGrassHopperTime = (*m_wpJsonObj.lock())["GrassDashMaxTime"].int_value();
					m_rGrassHopperTime = 0;
					m_gravity = 0;
					m_vForce -= m_vForceDownValue;
					m_spAnimator->SetAnimation(m_spModel->GetAnimation("GrassDashJB"), false);

					node = m_spModel->FindNode("GrassHopperLegRPoint");
					mat = node->m_worldTransform * m_mWorld;
					mat._42 += static_cast<float>(m_mpObj["AddBottomYVal"].number_value());
					m_weaponList[3]->GrassHopper({ mat._41,mat._42,mat._43 }, spCamera->GetYAng());

					KdEffekseerManager::GetInstance().
						Play("GrassDashBlurPlayer.efk", { m_pos.x,m_pos.y + static_cast<float>(m_mpObj["AddGrassDashUpEffectPosYVal"].number_value()),m_pos.z });
					KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("GrassDashBlurPlayer.efk"); // これでループしない
					Math::Matrix efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(270.0f)) * Math::Matrix::CreateTranslation({ m_pos.x,m_pos.y + static_cast<float>(m_mpObj["AddGrassDashUpEffectPosYVal"].number_value()),m_pos.z });
					KdEffekseerManager::GetInstance().SetWorldMatrix("GrassDashBlurPlayer.efk", efcMat);
				}
				// KdAudioManager::Instance().Play("Asset/Audio/SE/GrassHopeer.wav");
			}
		}
	}
}

// 短剣装備時の防御関係の動きを決める処理
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
				m_spAnimator->SetAnimation(m_spModel->GetAnimation("IdleA"), false);
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
			m_spAnimator->SetAnimation(m_spModel->GetAnimation("IdleA"), false);
		}
		m_playerState = idle;
		//m_playerState &= ~defense;
	}
}

// 短剣装備時の攻撃関係の動きを決める処理
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
					m_spAnimator->SetAnimation(m_spModel->GetAnimation("Defense"), true);
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
					m_spAnimator->SetAnimation(m_spModel->GetAnimation("Defense"), true);
				}
				return;
			}
		}
	}
	else
	{
		if (!m_bAttackAnimeDelay)
		{
			if (m_playerState & grassHopperDash && m_lGrassHopperTime > (*m_wpJsonObj.lock())["GrassDashSteppingAwayJustBefore"].int_value() || m_rGrassHopperTime > (*m_wpJsonObj.lock())["GrassDashSteppingAwayJustBefore"].int_value())return;

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
								for (auto& enemy : m_enemyList)
								{
									if (enemy.expired())continue;
									enemy.lock()->SetAttackHit(false);
									enemy.lock()->SetDefenseSuc(false);
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
								m_spAnimator->SetAnimation(m_spModel->GetAnimation("Mantis"), false);
								m_attackMoveSpd = 0;
							}
						}
						else
						{
							m_bRushAttackPossible = false;
							for (auto& enemy : m_enemyList)
							{
								if (enemy.expired())continue;
								enemy.lock()->SetAttackHit(false);
								enemy.lock()->SetDefenseSuc(false);
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
							m_attackMoveSpd = static_cast<float>(m_mpObj["FirstAttackMoveSpeed"].number_value());
							m_spAnimator->SetAnimation(m_spModel->GetAnimation("RLAttackOne"), false);
						}
					}

					if (KdInputManager::Instance().GetButtonState("rAttack"))
					{
						if (KdInputManager::Instance().GetButtonState("lAttack"))
						{
							if (!(m_playerState & (mantis)))
							{
								for (auto& enemy : m_enemyList)
								{
									if (enemy.expired())continue;
									enemy.lock()->SetAttackHit(false);
									enemy.lock()->SetDefenseSuc(false);
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
								m_spAnimator->SetAnimation(m_spModel->GetAnimation("Mantis"), false);
								m_bMove = true;
								m_attackMoveSpd = 0;
							}
						}
						else
						{
							m_bRushAttackPossible = false;
							for (auto& enemy : m_enemyList)
							{
								if (enemy.expired())continue;
								enemy.lock()->SetAttackHit(false);
								enemy.lock()->SetDefenseSuc(false);
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
							m_attackMoveSpd = static_cast<float>(m_mpObj["FirstAttackMoveSpeed"].number_value());
							m_spAnimator->SetAnimation(m_spModel->GetAnimation("RLAttackOne"), false);
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
						for (auto& enemy : m_enemyList)
						{
							if (enemy.expired())continue;
							enemy.lock()->SetAttackHit(false);
							enemy.lock()->SetDefenseSuc(false);
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
						m_attackMoveSpd = static_cast<float>(m_mpObj["FirstAttackMoveSpeed"].number_value());

						if (m_playerState & grassHopperDash)
						{
							m_spAnimator->SetAnimation(m_spModel->GetAnimation("GrassDashLAttack"), false);
						}
						else
						{
							m_spAnimator->SetAnimation(m_spModel->GetAnimation("LAttack1"), false);
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
						for (auto& enemy : m_enemyList)
						{
							if (enemy.expired())continue;
							enemy.lock()->SetAttackHit(false);
							enemy.lock()->SetDefenseSuc(false);
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
						m_attackMoveSpd = static_cast<float>(m_mpObj["FirstAttackMoveSpeed"].number_value());

						if (m_playerState & (grassHopperDashF | step))
						{
							m_spAnimator->SetAnimation(m_spModel->GetAnimation("GrassDashRAttack"), false);
						}
						else
						{
							m_spAnimator->SetAnimation(m_spModel->GetAnimation("RAttack1"), false);
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
							for (auto& enemy : m_enemyList)
							{
								if (enemy.expired())continue;
								enemy.lock()->SetAttackHit(false);
								enemy.lock()->SetDefenseSuc(false);
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
							m_attackMoveSpd = static_cast<float>(m_mpObj["AfterTheSecondAttackMoveSpeed"].number_value());
							m_spAnimator->SetAnimation(m_spModel->GetAnimation("RLAttackTwo"), false);
						}
						else if (m_playerState & rlAttackTwo)
						{
							for (auto& enemy : m_enemyList)
							{
								if (enemy.expired())continue;
								enemy.lock()->SetAttackHit(false);
								enemy.lock()->SetDefenseSuc(false);
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
							m_attackMoveSpd = static_cast<float>(m_mpObj["RLAttackThreeMoveSpeed"].number_value());
							m_spAnimator->SetAnimation(m_spModel->GetAnimation("RLAttackThree"), false);
						}
						else if (m_playerState & rlAttackThree && m_bRushAttackPossible)
						{
							for (auto& enemy : m_enemyList)
							{
								if (enemy.expired())continue;
								enemy.lock()->SetAttackHit(false);
								enemy.lock()->SetDefenseSuc(false);
							}

							if (KdInputManager::Instance().GetButtonState("lAttack"))
							{
								m_bRushRp = false;
								m_spAnimator->SetAnimation(m_spModel->GetAnimation("RLAttackRush"), false);
								m_attackMoveSpd = static_cast<float>(m_mpObj["AfterTheSecondAttackMoveSpeed"].number_value());
							}
							else if (KdInputManager::Instance().GetButtonState("rAttack"))
							{
								m_bRushRp = true;
								m_spAnimator->SetAnimation(m_spModel->GetAnimation("RLAttackRushRP"), false);
								m_attackMoveSpd = static_cast<float>(m_mpObj["RLAttackRotationRushMoveSpeed"].number_value());
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
							m_attackMoveSpd = static_cast<float>(m_mpObj["AfterTheSecondAttackMoveSpeed"].number_value());
							m_spAnimator->SetAnimation(m_spModel->GetAnimation("LAttack2"), false);
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
							m_attackMoveSpd = static_cast<float>(m_mpObj["AfterTheSecondAttackMoveSpeed"].number_value());

							m_spAnimator->SetAnimation(m_spModel->GetAnimation("LAttack3"), false);
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
							m_attackMoveSpd = static_cast<float>(m_mpObj["AfterTheSecondAttackMoveSpeed"].number_value());
							m_spAnimator->SetAnimation(m_spModel->GetAnimation("RAttack2"), false);
						}
						else if (m_playerState & rAttackTwo)
						{
							m_playerState |= rAttackThree;
							m_playerState &= ~rAttackTwo;
							m_bAttackAnimeDelay = false;
							m_bAttackAnimeCnt = false;
							m_attackAnimeCnt = 0;
							m_attackAnimeDelayCnt = 0;

							m_attackMoveDir = m_wpCamera.lock()->GetMatrix().Backward();

							m_attackMoveDir.y = 0;
							m_attackMoveDir.Normalize();
							m_attackMoveSpd = static_cast<float>(m_mpObj["AfterTheSecondAttackMoveSpeed"].number_value());
							m_spAnimator->SetAnimation(m_spModel->GetAnimation("RAttack3"), false);
						}

						m_playerState &= ~lAttack;
						m_bMove = true;
					}
				}
			}
		}

	}
}
//＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝

// 各Stateの動き処理＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
// Hopper行動関連の処理
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
			Math::Matrix efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)) * Math::Matrix::CreateTranslation({ m_pos.x + m_addGrassDashEffectPosVal.x * vec.x,m_pos.y + m_addGrassDashEffectPosVal.y,m_pos.z + m_addGrassDashEffectPosVal.z * vec.z });
			KdEffekseerManager::GetInstance().SetWorldMatrix("GrassDashBlurPlayer.efk", efcMat);
		}
		else if (m_playerState & grassHopperDashB)
		{
			Math::Vector3 vec = m_mWorld.Forward();
			Math::Matrix efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y + 180.0f)) * Math::Matrix::CreateTranslation({ m_pos.x + m_addGrassDashEffectPosVal.x * vec.x,m_pos.y + m_addGrassDashEffectPosVal.y,m_pos.z + m_addGrassDashEffectPosVal.z * vec.z });
			KdEffekseerManager::GetInstance().SetWorldMatrix("GrassDashBlurPlayer.efk", efcMat);
		}
		else if (m_playerState & grassHopperDashR)
		{
			Math::Vector3 vec = m_mWorld.Right();
			Math::Matrix efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y + 90.0f)) * Math::Matrix::CreateTranslation({ m_pos.x + m_addGrassDashEffectPosVal.x * vec.x,m_pos.y + m_addGrassDashEffectPosVal.y,m_pos.z + m_addGrassDashEffectPosVal.z * vec.z });
			KdEffekseerManager::GetInstance().SetWorldMatrix("GrassDashBlurPlayer.efk", efcMat);
		}
		else if (m_playerState & grassHopperDashL)
		{
			Math::Vector3 vec = m_mWorld.Left();
			Math::Matrix efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y + 270.0f)) * Math::Matrix::CreateTranslation({ m_pos.x + m_addGrassDashEffectPosVal.x * vec.x,m_pos.y + m_addGrassDashEffectPosVal.y,m_pos.z + m_addGrassDashEffectPosVal.z * vec.z });
			KdEffekseerManager::GetInstance().SetWorldMatrix("GrassDashBlurPlayer.efk", efcMat);
		}
		else if (m_playerState & grassHopperDashUp)
		{
			Math::Matrix efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(270.0f)) * Math::Matrix::CreateTranslation({ m_pos.x,m_pos.y + static_cast<float>(m_mpObj["AddGrassDashUpEffectPosYVal"].number_value()),m_pos.z });
			KdEffekseerManager::GetInstance().SetWorldMatrix("GrassDashBlurPlayer.efk", efcMat);
		}

		m_bMove = true;
		if (m_lGrassHopperTime <= (*m_wpJsonObj.lock())["GrassDashMaxTime"].int_value() && m_lGrassHopperTime > (*m_wpJsonObj.lock())["GrassDashSteppingAwayMoment"].int_value() ||
			m_rGrassHopperTime <= (*m_wpJsonObj.lock())["GrassDashMaxTime"].int_value() && m_rGrassHopperTime > (*m_wpJsonObj.lock())["GrassDashSteppingAwayMoment"].int_value())
		{
			m_dashSpd = static_cast<float>((*m_wpJsonObj.lock())["GrassDashSpeed"][0].number_value());
		}
		else if (m_lGrassHopperTime == (*m_wpJsonObj.lock())["GrassDashSteppingAwayMoment"].int_value() || 
			     m_rGrassHopperTime == (*m_wpJsonObj.lock())["GrassDashSteppingAwayMoment"].int_value())
		{
			KdAudioManager::Instance().Play("Asset/Audio/SE/GrassHopeer.wav");
			m_dashSpd = static_cast<float>((*m_wpJsonObj.lock())["GrassDashSpeed"][1].number_value());
		}
		else if (m_lGrassHopperTime == (*m_wpJsonObj.lock())["GrassDashSpeeedDecelerationTime"][0].int_value() ||
			     m_rGrassHopperTime == (*m_wpJsonObj.lock())["GrassDashSpeeedDecelerationTime"][0].int_value())
		{
			m_dashSpd = static_cast<float>((*m_wpJsonObj.lock())["GrassDashSpeed"][2].number_value());
		}
		else if (m_lGrassHopperTime == (*m_wpJsonObj.lock())["GrassDashSpeeedDecelerationTime"][1].int_value() ||
			     m_rGrassHopperTime == (*m_wpJsonObj.lock())["GrassDashSpeeedDecelerationTime"][1].int_value())
		{
			m_dashSpd = static_cast<float>((*m_wpJsonObj.lock())["GrassDashSpeed"][3].number_value());
		}
		else if (m_lGrassHopperTime == (*m_wpJsonObj.lock())["GrassDashSpeeedDecelerationTime"][2].int_value() || 
			     m_rGrassHopperTime == (*m_wpJsonObj.lock())["GrassDashSpeeedDecelerationTime"][2].int_value())
		{
			m_dashSpd = static_cast<float>((*m_wpJsonObj.lock())["GrassDashSpeed"][4].number_value());
		}
		else if (m_lGrassHopperTime == (*m_wpJsonObj.lock())["GrassDashSpeeedDecelerationTime"][3].int_value() || 
			     m_rGrassHopperTime == (*m_wpJsonObj.lock())["GrassDashSpeeedDecelerationTime"][3].int_value())
		{
			m_dashSpd = static_cast<float>((*m_wpJsonObj.lock())["GrassDashSpeed"][5].number_value());
		}
		else if (m_lGrassHopperTime == (*m_wpJsonObj.lock())["GrassDashSpeeedDecelerationTime"][4].int_value() || 
			     m_rGrassHopperTime == (*m_wpJsonObj.lock())["GrassDashSpeeedDecelerationTime"][4].int_value())
		{
			m_dashSpd = static_cast<float>((*m_wpJsonObj.lock())["GrassDashSpeed"][6].number_value());
		}
		else if (m_lGrassHopperTime == (*m_wpJsonObj.lock())["GrassDashSpeeedDecelerationTime"][5].int_value() || 
			     m_rGrassHopperTime == (*m_wpJsonObj.lock())["GrassDashSpeeedDecelerationTime"][5].int_value())
		{
			m_dashSpd = static_cast<float>((*m_wpJsonObj.lock())["GrassDashSpeed"][7].number_value());
		}

		if (m_lGrassHopperTime == (*m_wpJsonObj.lock())["GrassDashSteppingAwayJustBefore"].int_value())
		{
			m_weaponList[3]->StartAnime();
			if (m_playerState & grassHopperDashF)
			{
				m_spAnimator->SetAnimation(m_spModel->GetAnimation("GrassDashFA"), true);
			}
			else if (m_playerState & grassHopperDashR)
			{
				m_spAnimator->SetAnimation(m_spModel->GetAnimation("GrassDashRA"), true);
			}
			else if (m_playerState & grassHopperDashL)
			{
				m_spAnimator->SetAnimation(m_spModel->GetAnimation("GrassDashLA"), true);
			}
			else if (m_playerState & grassHopperDashB)
			{
				m_spAnimator->SetAnimation(m_spModel->GetAnimation("GrassDashBA"), true);
			}
			else if (m_playerState & grassHopperDashUp)
			{
				m_spAnimator->SetAnimation(m_spModel->GetAnimation("GrassDashJA"), true);
			}
		}

		if (m_rGrassHopperTime == (*m_wpJsonObj.lock())["GrassDashSteppingAwayJustBefore"].int_value())
		{
			m_weaponList[2]->StartAnime();
			if (m_playerState & grassHopperDashF)
			{
				m_spAnimator->SetAnimation(m_spModel->GetAnimation("GrassDashFA"), true);
			}
			else if (m_playerState & grassHopperDashR)
			{
				m_spAnimator->SetAnimation(m_spModel->GetAnimation("GrassDashRA"), true);
			}
			else if (m_playerState & grassHopperDashL)
			{
				m_spAnimator->SetAnimation(m_spModel->GetAnimation("GrassDashLA"), true);
			}
			else if (m_playerState & grassHopperDashB)
			{
				m_spAnimator->SetAnimation(m_spModel->GetAnimation("GrassDashBA"), true);
			}
			else if (m_playerState & grassHopperDashUp)
			{
				m_spAnimator->SetAnimation(m_spModel->GetAnimation("GrassDashJA"), true);
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

// ステップ行動関連の処理
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
				m_spAnimator->SetAnimation(m_spModel->GetAnimation("GrassDashFA"), true);
			}
			else if (m_playerState & stepR)
			{
				m_spAnimator->SetAnimation(m_spModel->GetAnimation("GrassDashRA"), true);
			}
			else if (m_playerState & stepL)
			{
				m_spAnimator->SetAnimation(m_spModel->GetAnimation("GrassDashLA"), true);
			}
			else if (m_playerState & stepB)
			{
				m_spAnimator->SetAnimation(m_spModel->GetAnimation("GrassDashBA"), true);
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
			m_spAnimator->SetAnimation(m_spModel->GetAnimation("IdleA"));
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

// 通常行動関連の処理
void Player::NormalMove()
{
	Math::Vector3 moveVec = {};
	float moveSpd = static_cast<float>(m_mpObj["RunMoveSpeed"].number_value());

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
					m_stepCnt = m_mpObj["StepMaxTime"].int_value();
					m_grasRotYMat = m_wpCamera.lock()->GetRotationYMatrix();
					m_playerState = stepF;
					m_spAnimator->SetAnimation(m_spModel->GetAnimation("GrassDashFB"), false);
				}
			}

			if (KdInputManager::Instance().GetButtonState("left"))
			{
				if (!(m_playerState & (jump | fall)))
				{
					m_stepDashDir = Math::Vector3::TransformNormal(Math::Vector3(-1, 0, 0), m_wpCamera.lock()->GetRotationYMatrix());
					m_bMove = true;
					m_stepCnt = m_mpObj["StepMaxTime"].int_value();
					m_grasRotYMat = m_wpCamera.lock()->GetRotationYMatrix();
					m_playerState = stepL;
					m_spAnimator->SetAnimation(m_spModel->GetAnimation("GrassDashLB"), false);
				}
			}

			if (KdInputManager::Instance().GetButtonState("backward"))
			{
				if (!(m_playerState & (jump | fall)))
				{
					m_stepDashDir = Math::Vector3::TransformNormal(Math::Vector3(0, 0, -1), m_wpCamera.lock()->GetRotationYMatrix());
					m_bMove = true;
					m_stepCnt = m_mpObj["StepMaxTime"].int_value();
					m_grasRotYMat = m_wpCamera.lock()->GetRotationYMatrix();
					m_playerState = stepB;
					m_spAnimator->SetAnimation(m_spModel->GetAnimation("GrassDashBB"), false);
				}
			}

			if (KdInputManager::Instance().GetButtonState("right"))
			{
				if (!(m_playerState & (jump | fall)))
				{
					m_stepDashDir = Math::Vector3::TransformNormal(Math::Vector3(1, 0, 0), m_wpCamera.lock()->GetRotationYMatrix());
					m_bMove = true;
					m_stepCnt = m_mpObj["StepMaxTime"].int_value();
					m_grasRotYMat = m_wpCamera.lock()->GetRotationYMatrix();
					m_playerState = stepR;
					m_spAnimator->SetAnimation(m_spModel->GetAnimation("GrassDashRB"), false);
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
						m_spAnimator->SetAnimation(m_spModel->GetAnimation("RUN"));
						m_runAnimeCnt = 0;
					}
					else if (m_runAnimeCnt == 0)
					{
						m_spAnimator->SetAnimation(m_spModel->GetAnimation("RUN"));
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
						m_spAnimator->SetAnimation(m_spModel->GetAnimation("RUN"));
						m_runAnimeCnt = 0;
					}
					else if (m_runAnimeCnt == 0)
					{
						m_spAnimator->SetAnimation(m_spModel->GetAnimation("RUN"));
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
						m_spAnimator->SetAnimation(m_spModel->GetAnimation("RUN"));
						m_runAnimeCnt = 0;
					}
					else if (m_runAnimeCnt == 0)
					{
						m_spAnimator->SetAnimation(m_spModel->GetAnimation("RUN"));
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
						m_spAnimator->SetAnimation(m_spModel->GetAnimation("RUN"));
						m_runAnimeCnt = 0;
					}
					else if (m_runAnimeCnt == 0)
					{
						m_spAnimator->SetAnimation(m_spModel->GetAnimation("RUN"));
					}
					m_playerState = run;
				}
			}
		}

		if (KdInputManager::Instance().GetButtonState("jump"))
		{
			if (!(m_playerState & (jump | fall)))
			{
				m_gravity = static_cast<float>(m_mpObj["JumpPow"].number_value());
				m_playerState |= jump;
				KdAudioManager::Instance().Play("Asset/Audio/SE/FootstepsConcrete2.wav");
				m_spAnimator->SetAnimation(m_spModel->GetAnimation("JumpA"), false);
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
		if (m_spAnimator->IsAnimationEnd())
		{
			m_spAnimator->SetAnimation(m_spModel->GetAnimation("JumpB"), true);
		}
	}

	if (m_playerState & idle)
	{
		if (m_spAnimator->IsAnimationEnd())
		{
			m_spAnimator->SetAnimation(m_spModel->GetAnimation("IdleB"), true);
		}
	}

	if (m_playerState & fall)
	{
		if (m_spAnimator->IsAnimationEnd())
		{
			m_spAnimator->SetAnimation(m_spModel->GetAnimation("FallB"), true);
		}
	}

	UpdateRotate(moveVec);
}

// 攻撃が防がれた時の行動処理
void Player::HasDefenseMove()
{
	m_bMove = true;
	if (m_spAnimator->IsAnimationEnd())
	{
		if (!(m_playerState & idle))
		{
			m_spAnimator->SetAnimation(m_spModel->GetAnimation("IdleA"), false);
		}
		m_playerState = idle;
	}
}

// 短剣で攻撃した際の行動処理
void Player::ScorpionAttackMove()
{
	if (m_spAnimator->IsAnimationEnd())
	{
		ScorpionAttackEndProcess();
	}
	else
	{
		ScorpionAttackAnimationMoveProcess();
	}
}

// 短剣での攻撃終了処理
void Player::ScorpionAttackEndProcess()
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
			m_spAnimator->SetAnimation(m_spModel->GetAnimation("GrassDashFA"), true);
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
			m_spAnimator->SetAnimation(m_spModel->GetAnimation("GrassDashFA"), true);
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

// 短剣での攻撃アニメーション中の処理
void Player::ScorpionAttackAnimationMoveProcess()
{
	if (!(m_playerState & rlAttackRush))
	{
		if (m_attackAnimeCnt >= m_mpObj["AttackPointMoment"].int_value())
		{
			m_attackMoveSpd *= static_cast<float>((*m_wpJsonObj.lock())["MoveSpeedStopsAbruptly"].number_value());
		}
		else
		{
			if (m_bAtttackMoveSpeedDec)
			{
				m_attackMoveSpd *= static_cast<float>((*m_wpJsonObj.lock())["MoveSpeedStopsAbruptly"].number_value());
			}
		}
	}

	m_bMove = true;
	if (!(m_playerState & (rlAttack | rlAttackRush)))
	{
		m_attackMoveSpd *= static_cast<float>((*m_wpJsonObj.lock())["MoveSpeedDecelerationamount"].number_value());

		if (m_playerState & (rAttackTwo | lAttackTwo))
		{
			if (m_attackAnimeCnt == m_mpObj["AttackPointMoment"].int_value())
			{
				for (auto& enemy : m_enemyList)
				{
					if (enemy.expired())continue;
					enemy.lock()->SetAttackHit(false);
					enemy.lock()->SetDefenseSuc(false);
				}
			}
		}
		else if (m_playerState & (rAttackThree | lAttackThree))
		{
			if (m_attackAnimeCnt == m_mpObj["AttackPointMoment"].int_value())
			{
				for (auto& enemy : m_enemyList)
				{
					if (enemy.expired())continue;
					enemy.lock()->SetAttackHit(false);
					enemy.lock()->SetDefenseSuc(false);
				}
			}
		}
	}
	else
	{
		if (m_playerState & rlAttackOne)
		{
			m_attackMoveSpd *= static_cast<float>((*m_wpJsonObj.lock())["MoveSpeedDecelerationamount"].number_value());
			if (m_attackAnimeCnt == m_mpObj["RLAttackOneShakenSecondMoment"].int_value())
			{
				for (auto& enemy : m_enemyList)
				{
					if (enemy.expired())continue;
					enemy.lock()->SetAttackHit(false);
					enemy.lock()->SetDefenseSuc(false);
				}
			}
		}
		else if (m_playerState & rlAttackTwo)
		{
			m_attackMoveSpd *= static_cast<float>((*m_wpJsonObj.lock())["MoveSpeedDecelerationamount"].number_value());
			if (m_attackAnimeCnt == m_mpObj["RLAttackTwoShakenSecondMoment"].int_value())
			{
				for (auto& enemy : m_enemyList)
				{
					if (enemy.expired())continue;
					enemy.lock()->SetAttackHit(false);
					enemy.lock()->SetDefenseSuc(false);
				}
			}
		}
		else if (m_playerState & rlAttackThree)
		{
			m_attackMoveSpd *= static_cast<float>((*m_wpJsonObj.lock())["MoveSpeedDecelerationamount"].number_value());
			if (m_attackAnimeCnt == m_mpObj["RLAttackThreeShakenSecondMoment"].int_value())
			{
				for (auto& enemy : m_enemyList)
				{
					if (enemy.expired())continue;
					enemy.lock()->SetAttackHit(false);
					enemy.lock()->SetDefenseSuc(false);
				}
			}
		}
		else if (m_playerState & rlAttackRush)
		{
			if (!m_bRushRp)
			{
				m_attackMoveSpd *= static_cast<float>((*m_wpJsonObj.lock())["RushAttackMoveSpeedDecelerationamount"].number_value());
				if (m_attackAnimeCnt == m_mpObj["RLAttackRushShakenMoment"][1].int_value() ||
					m_attackAnimeCnt == m_mpObj["RLAttackRushShakenMoment"][2].int_value() ||
					m_attackAnimeCnt == m_mpObj["RLAttackRushShakenMoment"][3].int_value() ||
					m_attackAnimeCnt == m_mpObj["RLAttackRushShakenMoment"][4].int_value() ||
					m_attackAnimeCnt == m_mpObj["RLAttackRushShakenMoment"][5].int_value() ||
					m_attackAnimeCnt == m_mpObj["RLAttackRushShakenMoment"][6].int_value() ||
					m_attackAnimeCnt == m_mpObj["RushLastAttackPointTime"].int_value()
					)
				{
					for (auto& enemy : m_enemyList)
					{
						if (enemy.expired())continue;
						enemy.lock()->SetAttackHit(false);
						enemy.lock()->SetDefenseSuc(false);
					}

					if (m_attackAnimeCnt == m_mpObj["RLAttackRushShakenMoment"][1].int_value() ||
						m_attackAnimeCnt == m_mpObj["RLAttackRushShakenMoment"][2].int_value()
						)
					{
						m_attackMoveSpd = static_cast<float>(m_mpObj["RLAttackRushOneAndTwoShakenMomentMoveSpeed"].number_value());
					}

					if (m_attackAnimeCnt == m_mpObj["RLAttackRushShakenMoment"][3].int_value() ||
						m_attackAnimeCnt == m_mpObj["RLAttackRushShakenMoment"][4].int_value()
						)
					{
						m_attackMoveSpd = static_cast<float>(m_mpObj["RLAttackRushThreeAndFourShakenMomentMoveSpeed"].number_value());
					}

					if (m_attackAnimeCnt == m_mpObj["RLAttackRushShakenMoment"][5].int_value() ||
						m_attackAnimeCnt == m_mpObj["RLAttackRushShakenMoment"][6].int_value()
						)
					{
						m_attackMoveSpd = static_cast<float>(m_mpObj["RLAttackRushFiveAndSixShakenMomentMoveSpeed"].number_value());
					}

					if (m_attackAnimeCnt == (*m_wpJsonObj.lock())["RushLastAttackPointTime"].int_value())
					{
						m_attackMoveSpd = static_cast<float>(m_mpObj["RushLastAttackMoveSpeed"].number_value());
					}
				}
			}
			else
			{
				if (m_attackAnimeCnt < m_mpObj["RotationRushLastAttackPointTime"].int_value())
				{
					m_attackMoveSpd *= static_cast<float>((*m_wpJsonObj.lock())["MoveSpeedDecelerationamount"].number_value());
				}
				else
				{
					m_attackMoveSpd *= static_cast<float>(m_mpObj["RotationRushLastAttackMoveSpeedDecelerationamount"].number_value());
				}

				if (m_attackAnimeCnt == m_mpObj["RLAttackRotationRushShakenMoment"][0].int_value() ||
					m_attackAnimeCnt == m_mpObj["RLAttackRotationRushShakenMoment"][1].int_value() ||
					m_attackAnimeCnt == m_mpObj["RLAttackRotationRushShakenMoment"][2].int_value() ||
					m_attackAnimeCnt == m_mpObj["RLAttackRotationRushShakenMoment"][3].int_value() ||
					m_attackAnimeCnt == m_mpObj["RLAttackRotationRushShakenMoment"][4].int_value() ||
					m_attackAnimeCnt == m_mpObj["RLAttackRotationRushShakenMoment"][5].int_value() ||
					m_attackAnimeCnt == m_mpObj["RLAttackRotationRushShakenMoment"][6].int_value() ||
					m_attackAnimeCnt == m_mpObj["RLAttackRotationRushShakenMoment"][7].int_value() ||
					m_attackAnimeCnt == m_mpObj["RotationRushLastAttackPointTime"].int_value()
					)
				{
					for (auto& enemy : m_enemyList)
					{
						if (enemy.expired())continue;
						enemy.lock()->SetAttackHit(false);
						enemy.lock()->SetDefenseSuc(false);
					}

					if (m_attackAnimeCnt == m_mpObj["RLAttackRotationRushShakenMoment"][0].int_value() ||
						m_attackAnimeCnt == m_mpObj["RLAttackRotationRushShakenMoment"][1].int_value() ||
						m_attackAnimeCnt == m_mpObj["RLAttackRotationRushShakenMoment"][2].int_value() ||
						m_attackAnimeCnt == m_mpObj["RLAttackRotationRushShakenMoment"][3].int_value() ||
						m_attackAnimeCnt == m_mpObj["RLAttackRotationRushShakenMoment"][4].int_value() ||
						m_attackAnimeCnt == m_mpObj["RLAttackRotationRushShakenMoment"][5].int_value() ||
						m_attackAnimeCnt == m_mpObj["RLAttackRotationRushShakenMoment"][6].int_value() ||
						m_attackAnimeCnt == m_mpObj["RLAttackRotationRushShakenMoment"][7].int_value())
					{
						m_attackMoveDir = m_wpCamera.lock()->GetMatrix().Backward();
						m_attackMoveDir.y = 0;
						m_attackMoveDir.Normalize();
						m_attackMoveSpd = static_cast<float>(m_mpObj["RLAttackRotationRushMoveSpeed"].number_value());
					}

					if (m_attackAnimeCnt == m_mpObj["RotationRushLastAttackPointTime"].int_value())
					{
						if (!m_wpEnemy.expired())
						{
							Math::Vector3 dis = m_wpEnemy.lock()->GetPos() - m_pos;
							if (dis.Length() <= static_cast<float>(m_mpObj["EnemyToPlayerDistanceIsNearDistance"].number_value()))
							{
								m_attackMoveSpd = static_cast<float>(m_mpObj["EnemyToPlayerDistanceIsNearDistanceSpeed"].number_value());
							}
							else if (dis.Length() <= static_cast<float>(m_mpObj["EnemyToPlayerDistanceIsMediumDistance"].number_value()))
							{
								m_attackMoveSpd = static_cast<float>(m_mpObj["EnemyToPlayerDistanceIsMediumDistanceSpeed"].number_value());
							}
							else
							{
								m_attackMoveSpd = static_cast<float>(m_mpObj["EnemyToPlayerDistanceIsLongDistanceSpeed"].number_value());
							}
						}
						else
						{
							for (auto& enemy : m_enemyList)
							{
								if (enemy.expired())continue;
								Math::Vector3 dis = enemy.lock()->GetPos() - m_pos;
								if (dis.Length() <= static_cast<float>(m_mpObj["EnemyToPlayerDistanceIsNearDistance"].number_value()))
								{
									m_attackMoveSpd = static_cast<float>(m_mpObj["EnemyToPlayerDistanceIsNearDistanceSpeed"].number_value());
									break;
								}
								else if (dis.Length() <= static_cast<float>(m_mpObj["EnemyToPlayerDistanceIsMediumDistance"].number_value()))
								{
									m_attackMoveSpd = static_cast<float>(m_mpObj["EnemyToPlayerDistanceIsMediumDistanceSpeed"].number_value());
									break;
								}
								else
								{
									m_attackMoveSpd = static_cast<float>(m_mpObj["EnemyToPlayerDistanceIsLongDistanceSpeed"].number_value());
									break;
								}
							}
						}
					}
				}
			}
		}
	}

	if (!(m_playerState & mantis))
	{
		UpdateRotate(m_attackMoveDir);
	}

	SpeedyMoveWallHitChack(m_attackMoveSpd, m_attackMoveDir);
	m_pos += m_attackMoveDir * m_attackMoveSpd;
}

//＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝

// Playerがステージの外に出たかどうかを判断する処理＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
void Player::OverStageChaeck()
{
	float lowestYPos;
	if (SceneManager::Instance().GetSceneType() == SceneManager::SceneType::tutorial || SceneManager::Instance().GetSceneType() == SceneManager::SceneType::training)
	{
		m_vForceDownValue = 0;
		lowestYPos = static_cast<float>((*m_wpJsonObj.lock())["TutorialMinimumYPos"].number_value());
	}
	else
	{
		lowestYPos = static_cast<float>((*m_wpJsonObj.lock())["MinimumYPos"].number_value());
	}

	if (m_pos.x > static_cast<float>((*m_wpJsonObj.lock())["HightXPos"].number_value()) ||
		m_pos.x < static_cast<float>((*m_wpJsonObj.lock())["MinimumXPos"].number_value()) ||
		m_pos.z > static_cast<float>((*m_wpJsonObj.lock())["HightZPos"].number_value()) ||
		m_pos.z < static_cast<float>((*m_wpJsonObj.lock())["MinimumZPos"].number_value()) ||
		m_pos.y < lowestYPos)
	{
		m_overStageTime++;
		int maxOverStageTime = (*m_wpJsonObj.lock())["MaxOverStageTime"].int_value();
		if (m_overStageTime >= maxOverStageTime)
		{
			m_pos = Math::Vector3::Zero;
			m_overStageTime = 0;
		}
	}
	else
	{
		int maxOverStageTime = (*m_wpJsonObj.lock())["MaxOverStageTime"].int_value();
		if (m_overStageTime >= maxOverStageTime)
		{
			m_pos = Math::Vector3::Zero;
			m_overStageTime = 0;
		}

		KdCollider::SphereInfo sphereInfo;
		// 球の中心位置を設定
		sphereInfo.m_sphere.Center = m_pos + m_addCenterVal;
		// 球の半径を設定

		sphereInfo.m_sphere.Radius = static_cast<float>(m_mpObj["DeterminationInsideTheBuildingSphereRadius"].number_value());

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
}
//＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝

// 敵をロックオンする処理＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
void Player::EnemyRockOn()
{
	if (GetAsyncKeyState(VK_MBUTTON) & 0x80000)
	{
		if (!m_bMButtonState)
		{
			const std::shared_ptr<GameCamera> gCamera = std::dynamic_pointer_cast<GameCamera>(m_wpCamera.lock());
			if (gCamera->GetBRotateEnemy())
			{
				gCamera->SetBRotateEnemy(false);
				m_wpEnemy.reset();
			}
			else
			{
				float smallAng = 0;
				int i = 0;
				for (auto& enemy : m_enemyList)
				{
					if (enemy.expired())
					{
						continue;
					}

					if (enemy.lock()->GetBEnemyDeath())
					{
						continue;
					}

					Math::Vector3 nowVec = gCamera->GetMatrix().Backward();
					nowVec.y = 0.0f;
					nowVec.Normalize();

					// 向きたい方向
					Math::Vector3 toVec = enemy.lock()->GetPos() - GetPos();
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
						gCamera->SetEnemy(enemy.lock());
						m_wpEnemy = enemy.lock();
						continue;
					}

					if (smallAng > ang)
					{
						smallAng = ang;
						gCamera->SetEnemy(enemy.lock());
						m_wpEnemy = enemy.lock();
						continue;
					}
					else if (smallAng == ang)
					{
						Math::Vector3 cross = DirectX::XMVector3Cross(nowVec, toVec);
						if (cross.y >= 0)
						{
							smallAng = ang;
							++i;
							gCamera->SetEnemy(enemy.lock());
							m_wpEnemy = enemy.lock();
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
}
// ＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
