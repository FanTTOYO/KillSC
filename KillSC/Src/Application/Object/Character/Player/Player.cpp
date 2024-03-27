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

	m_weaponType |= scorpion;
	m_weaponType |= lScorpion;

	m_rightWeaponNumber = object["RightWeaponScopionNum"].int_value();
	m_leftWeaponNumber = object["LeftWeaponScopionNum"].int_value();

	m_vForce = (float)object["Vforce"].number_value();
	m_endurance = (float)object["Endurance"].number_value();
	m_damageAmount = (float)object["Endurance"].number_value();
	m_notUnderAttackTime = 0;

	std::shared_ptr<Scopion> rightWeaponOne;
	rightWeaponOne = std::make_shared<Scopion>();
	rightWeaponOne->SetArrmType(rArrm);
	rightWeaponOne->SetbPlayerWeapon();
	m_weaponList.push_back(rightWeaponOne);

	std::shared_ptr<Scopion> leftWeaponOne;
	leftWeaponOne = std::make_shared<Scopion>();
	leftWeaponOne->SetArrmType(lArrm);
	leftWeaponOne->SetbPlayerWeapon();
	m_weaponList.push_back(leftWeaponOne);

	for (auto& enemy : m_enemyList)
	{
		rightWeaponOne->AddTarget(enemy.lock());
		leftWeaponOne->AddTarget(enemy.lock());
	}

	std::shared_ptr<Hopper> rightWeaponTwo;
	rightWeaponTwo = std::make_shared<Hopper>();
	rightWeaponTwo->SetArrmType(rArrm);
	rightWeaponTwo->SetbPlayerWeapon();
	m_weaponList.push_back(rightWeaponTwo);

	std::shared_ptr<Hopper> leftWeaponTwo;
	leftWeaponTwo = std::make_shared<Hopper>();
	leftWeaponTwo->SetArrmType(lArrm);
	leftWeaponTwo->SetbPlayerWeapon();
	m_weaponList.push_back(leftWeaponTwo);

	m_rocKOnPolygon.SetMaterial("Asset/Textures/Ui/Game/lockonMark.png");

	m_hitColorChangeTimeCnt = 0;

	m_bRushRp = false;
	m_bAtttackMoveSpeedDec = false;
	m_attackHit = false;
	m_mpObj = object;
	m_wpJsonObj = a_wpJsonObj;
	m_tuGardTime = 0;

	m_addCenterVal = { (float)(m_mpObj["AddCenterVal"][0].number_value()),
					   (float)(m_mpObj["AddCenterVal"][1].number_value()),
					   (float)(m_mpObj["AddCenterVal"][2].number_value()) };

	m_addGrassDashEffectPosVal = { (float)(m_mpObj["AddGrassDashEffectPosVal"][0].number_value()),
								   (float)(m_mpObj["AddGrassDashEffectPosVal"][1].number_value()),
								   (float)(m_mpObj["AddGrassDashEffectPosVal"][2].number_value()) };
	m_maxWeaponType = 2;

	m_vForceDownValue = (float)(m_mpObj["VforceDownValue"].number_value());
	m_moveSpeedDecelerationamount = (float)(*m_wpJsonObj.lock())["MoveSpeedDecelerationamount"].number_value();

	for (auto& WeaList : m_weaponList)
	{
		WeaList->SetOwner(shared_from_this());
	}

	CharacterBase::Init();
}

// 武器に敵情報を渡す
void Player::AddWeaponToEnemy(std::shared_ptr<Enemy> a_spEnemy)
{
	m_weaponList[0]->AddTarget(a_spEnemy);
	m_weaponList[1]->AddTarget(a_spEnemy);
}

// 更新＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
// 武器を切り替える処理
void Player::WeaponChangeProcess()
{
	if (!(m_charaState & (mantis | hasDefense | rlAttackRush)))
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

void Player::CntDown()
{
	if (m_hitColorChangeTimeCnt > 0)
	{
		--m_hitColorChangeTimeCnt;
	}

	if (m_invincibilityTimeCnt > 0)
	{
		--m_invincibilityTimeCnt;
	}

	if (m_notUnderAttackTime > 0)
	{
		--m_notUnderAttackTime;
	}

	if (m_rGrassHopperPauCnt > 0)
	{
		--m_rGrassHopperPauCnt;
	}

	if (m_lGrassHopperPauCnt > 0)
	{
		--m_lGrassHopperPauCnt;
	}
}

void Player::EnduranceAndVForthManagement()
{
	m_vForce -= m_graduallyTorionDecVal;
	if (m_vForce <= 0)
	{
		m_vForce = 0;
		if (!m_bDeath)
		{
			const std::shared_ptr<GameCamera> gCamera = std::dynamic_pointer_cast<GameCamera>(m_wpCamera.lock());
			m_bDeath = true;
			m_spAnimator->SetAnimation(m_spModel->GetAnimation("Death"), false);
			m_charaState = idle;
			gCamera->SetBRotateEnemy(false);
			m_wpEnemy.reset();
		}
	}

	if (m_endurance <= 0)
	{
		m_endurance = 0;
		if (m_endurance != m_damageAmount)
		{
			m_damageAmount -= 5;
			if (m_damageAmount <= m_endurance)
			{
				m_damageAmount = 0;
			}
		}

		if (!m_bDeath)
		{
			const std::shared_ptr<GameCamera> gCamera = std::dynamic_pointer_cast<GameCamera>(m_wpCamera.lock());
			m_bDeath = true;
			m_spAnimator->SetAnimation(m_spModel->GetAnimation("Death"), false);
			m_charaState = idle;
			gCamera->SetBRotateEnemy(false);
			m_wpEnemy.reset();
		}
	}
	else if (!m_notUnderAttackTime)
	{
		if (m_endurance != m_damageAmount)
		{
			m_damageAmount -= 5;
			if (m_damageAmount <= m_endurance)
			{
				m_damageAmount = m_endurance;
			}
		}
	}
}

// 更新処理
void Player::Update()
{
	if (m_bLose)return;

	EnduranceAndVForthManagement();
	OverStageChaeck();

	if (m_bFirstUpdate)
	{
		m_bFirstUpdate = false;
		return;
	}

	if (!m_bWaponDrawPosible)
	{
		m_bWaponDrawPosible = true;
	}

	CntDown();

	if (SceneManager::Instance().GetSceneType() == SceneManager::SceneType::tutorial)
	{
		TutorialUpdate();
	}
	else
	{
		// debugキー
		if (GetAsyncKeyState('L') & 0x8000)
		{
			m_endurance = 0;
		}

	}

	if (m_bRushRp && !(m_charaState & rlAttackRush))
	{
		m_bRushRp = false;
	}

	if (!(m_charaState & (fall | jump)))
	{
		m_bMove = false;
	}

	WeaponChangeProcess();

	if (!(m_charaState & (rise | blowingAwayHit | cutRaiseHit)) && !m_bDeath && !m_hitColorChangeTimeCnt)
	{
		EnemyRockOn();

		if (!(m_charaState & hasDefense))
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

		if (!(m_charaState & (grassHopperDash | grassHopperDashUp)))
		{
			if (m_charaState & (lAttack | rAttack | mantis | rlAttack | rlAttackRush))
			{
				ScorpionAttackMove();
			}

			if (m_charaState & (defense))
			{
				ScorpionDefenseMove();
			}

			if (m_charaState & step)
			{
				StepMove();
			}
			else if (!(m_charaState & hasDefense))
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

	if (m_charaState & hit)
	{
		HitStateUpdate();
	}

	if (m_charaState & rise)
	{
		m_bMove = true;
		if (m_spAnimator->IsAnimationEnd())
		{
			m_bMove = false;
			m_charaState &= ~rise;
		}
	}

	if (!(m_charaState & (grassHopperDash | grassHopperDashUp | hit | step)))
	{
		m_pos.y -= m_gravity;
		m_gravity += static_cast<float>((*m_wpJsonObj.lock())["GravityAcceleration"].number_value());
	}

	if (!m_bMove && !m_bDeath)
	{
		if (!(m_charaState & (fall | jump)))
		{
			if (!(m_charaState & idle))
			{
				m_spAnimator->SetAnimation(m_spModel->GetAnimation("IdleA"), false);
			}
			m_charaState = idle;
		}
	}

	// ========================================
	// 当たり判定
	// ========================================
	RayCollisionUpdate();
	SphereCollisionUpdate();

	if (!m_bRushRp)
	{
		if ((m_charaState & rlAttackRush) && m_attackAnimeCnt >= m_mpObj["RushLastAttackPointTime"].int_value())
		{
			for (auto& enemy : m_enemyList)
			{
				if (enemy.expired())continue;
				KickHitAttackChaeck(enemy.lock(), this);
			}
		}
	}
	else
	{
		if ((m_charaState & rlAttackRush) && m_attackAnimeCnt >= m_mpObj["RotationRushLastAttackPointTime"].int_value())
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

		if (m_charaState & jump && m_spAnimator->IsAnimationEnd())
		{
			m_wpUi.lock()->AddTutorialCnt();
		}
		break;
	case Ui::TutorialType::bukiTu:
		break;
	case Ui::TutorialType::sukoADMoveTu:
		if (m_charaState & (rlAttackThree | rlAttackRush | rAttackThree | lAttackThree | mantis) && m_spAnimator->IsAnimationEnd())
		{
			m_wpUi.lock()->AddTutorialCnt();
		}

		else if (m_charaState & defense)
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
		if (m_charaState & (grassHopperDash | grassHopperDashUp) && m_rGrassHopperTime == (*m_wpJsonObj.lock())["GrassDashSteppingAwayMoment"].int_value() || m_charaState & (grassHopperDash | grassHopperDashUp) && m_lGrassHopperTime == (*m_wpJsonObj.lock())["GrassDashSteppingAwayMoment"].int_value())
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
	if (m_charaState & cutRaiseHit)
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

		if (m_charaState & (nomalHit | cutRaiseHit))
		{
			if (!(m_charaState & idle))
			{
				m_spAnimator->SetAnimation(m_spModel->GetAnimation("IdleA"), false);
			}
			m_charaState = idle;
		}
		else if (m_charaState & blowingAwayHit)
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
		else if (m_charaState & iaiKiriHit)
		{
			if (!(m_charaState & iaiKiriRise))
			{
				m_spAnimator->SetAnimation(m_spModel->GetAnimation("IaiKiriRise"), false);
			}
			m_charaState = iaiKiriRise;
		}
		m_hitMoveSpd = false;
	}

	if (m_charaState & blowingAwayHit)
	{
		if (m_hitStopCnt < 10)
		{
			m_hitMoveSpd = 0;
		}

		m_hitMoveSpd *= m_moveSpeedDecelerationamount;
	}
	else
	{
		m_hitMoveSpd *= m_moveSpeedDecelerationamount;
	}

	SpeedyMoveWallHitChack(m_hitMoveSpd, m_knockBackVec);
	m_pos += m_knockBackVec * m_hitMoveSpd;
}

// 更新後の処理
void Player::PostUpdate()
{
	if (!m_bDeath)
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
			if (!(m_charaState & hit))
			{
				if (!(m_charaState & fall))
				{
					m_spAnimator->SetAnimation(m_spModel->GetAnimation("FallA"), false);
				}

				m_charaState = fall;
			}
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
			if (m_bLose)
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
			if (!m_bLose)
			{
				KdEffekseerManager::GetInstance().
					Play("BailOutPlayer.efk", { m_pos.x,m_pos.y + 0.3f,m_pos.z });
				KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("BailOutPlayer.efk"); // これでループしない
				KdAudioManager::Instance().Play("Asset/Audio/SE/HumanCharacterDeath.wav");
				m_bLose = true;
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
	if (!(m_charaState & (lAttack | rAttack | rlAttack | rlAttackRush)))
	{
		m_spAnimator->AdvanceTime(m_spModel->WorkNodes());
		m_spModel->CalcNodeMatrices();
		if (m_charaState & run)
		{
			++m_runAnimeCnt;
			for (int i = 0; i < 2; ++i)
			{
				if (m_runAnimeCnt == m_mpObj["FootfallPointMoment"][i].int_value())
				{
					KdAudioManager::Instance().Play("Asset/Audio/SE/FootstepsConcrete2.wav");
					break;
				}
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
					KdAudioManager::Instance().Play("Asset/Audio/SE/Swishes - SWSH 40, Swish, Combat, Weapon, Light.wav");
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
					KdAudioManager::Instance().Play("Asset/Audio/SE/Swishes - SWSH 40, Swish, Combat, Weapon, Light.wav");
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
					KdAudioManager::Instance().Play("Asset/Audio/SE/Swishes - SWSH 40, Swish, Combat, Weapon, Light.wav");
				}
			}
			else if (m_charaState & rlAttackTwo)
			{
				if (m_attackAnimeCnt == m_mpObj["RLAttackTwoShakenFirstMoment"].int_value() || m_attackAnimeCnt == m_mpObj["RLAttackTwoShakenSecondMoment"].int_value())
				{
					KdAudioManager::Instance().Play("Asset/Audio/SE/Swishes - SWSH 40, Swish, Combat, Weapon, Light.wav");
				}
			}
			else if (m_charaState & rlAttackThree)
			{
				if (m_attackAnimeCnt == m_mpObj["RLAttackThreeShakenFirstMoment"].int_value() || m_attackAnimeCnt == m_mpObj["RLAttackThreeShakenSecondMoment"].int_value())
				{
					KdAudioManager::Instance().Play("Asset/Audio/SE/Swishes - SWSH 40, Swish, Combat, Weapon, Light.wav");
				}
			}
			else if (m_charaState & rlAttackRush)
			{
				if (!m_bRushRp)
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
				else
				{
					for (int i = 0; i < 8; ++i)
					{
						if (m_attackAnimeCnt == m_mpObj["RLAttackRotationRushShakenMoment"][i].int_value())
						{
							KdAudioManager::Instance().Play("Asset/Audio/SE/Swishes - SWSH 40, Swish, Combat, Weapon, Light.wav");
							break;
						}
					}
				}
			}


			if (m_charaState & rlAttackOne)
			{
				if (m_attackAnimeCnt == m_mpObj["LastRLAttackAnimationdMoment"].int_value())
				{
					m_bAttackAnimeDelay = true;
					m_attackAnimeDelayCnt = 10;
				}
			}
			else if (m_charaState & rlAttackTwo)
			{
				if (m_attackAnimeCnt == m_mpObj["LastRLAttackAnimationdMoment"].int_value())
				{
					m_bAttackAnimeDelay = true;
					m_attackAnimeDelayCnt = 10;
				}
			}
			else if (m_charaState & rlAttackThree)
			{
				if (m_attackAnimeCnt == m_mpObj["LastRLAttackThreeAnimationMoment"].int_value())
				{
					m_bAttackAnimeDelay = true;
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
	HitInfo hitInfo;
	float addBottomYVal = static_cast<float>(m_mpObj["AddBottomYVal"].number_value());
	Math::Vector3 bottomPos = Math::Vector3(m_pos.x, m_pos.y + addBottomYVal, m_pos.z);
	static float enableStepHight = static_cast<float>(m_mpObj["EnableStepHight"].number_value());

	if (!(m_charaState & (grassHopperDash | grassHopperDashUp)))
	{
		hitInfo = RayColisionCheck({ bottomPos.x,bottomPos.y + enableStepHight,bottomPos.z }, { 0,-1,0 }, m_gravity + enableStepHight, KdCollider::TypeGround);
	}
	else
	{
		hitInfo = RayColisionCheck(bottomPos, m_grassHopperDashDir, 0.0f, KdCollider::TypeGround);
	}

	if (hitInfo.hit)
	{
		if (!(m_charaState & (grassHopperDash | grassHopperDashUp)))
		{
			m_pos = hitInfo.groundPos + Math::Vector3(0, -addBottomYVal, 0);
			m_gravity = 0;
			if (m_charaState & (fall | jump) && !m_bDeath)
			{
				m_bMove = false;
				if (!(m_charaState & idle))
				{
					m_spAnimator->SetAnimation(m_spModel->GetAnimation("IdleA"), false);
				}
				m_charaState = idle;
			}

			m_wpCamera.lock()->SetStepOnPlayerPos(hitInfo.groundPos);
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

	hitInfo.Init();

	for (auto& enemy : m_enemyList)
	{
		if (enemy.expired())continue;
		if (enemy.lock()->GetBDeath())continue;

		if (!(m_charaState & (grassHopperDash | grassHopperDashUp)))
		{
			hitInfo = RayColisionCheck({ bottomPos.x,bottomPos.y + enableStepHight,bottomPos.z }, { 0,-1,0 }, m_gravity + enableStepHight, KdCollider::TypeRideEnemy, enemy.lock());
		}
		else
		{
			hitInfo = RayColisionCheck(bottomPos, m_grassHopperDashDir, 0.0f, KdCollider::TypeRideEnemy, enemy.lock());
		}
	}

	if (hitInfo.hit)
	{
		if (!(m_charaState & (grassHopperDash | grassHopperDashUp)))
		{
			m_pos = hitInfo.groundPos + Math::Vector3(0, -addBottomYVal, 0);
			m_gravity = 0;
			if (m_charaState & (fall | jump) && !m_bDeath)
			{
				m_bMove = false;
				if (!(m_charaState & idle))
				{
					m_spAnimator->SetAnimation(m_spModel->GetAnimation("IdleA"), false);
				}
				m_charaState = idle;
			}

			m_wpCamera.lock()->SetStepOnPlayerPos(hitInfo.groundPos);
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
}

// 球形当たり判定
void Player::SphereCollisionUpdate()
{
	HitInfo hitInfo;
	HitInfo hitInfoComparison;
	if (!(m_charaState & (grassHopperDash | grassHopperDashUp | step)))
	{
		hitInfo = SphereColisionCheck(m_pos + m_addCenterVal, static_cast<float>(m_mpObj["TypeGroundToHitSphereRadius"].number_value()), KdCollider::TypeGround);
		if (hitInfo.hit)
		{
			// 球とモデルが当たっている
			m_pos += (hitInfo.hitDir * hitInfo.maxOverLap);
		}
	}
	else
	{
		hitInfo = SphereColisionCheck(m_pos + m_addCenterVal, static_cast<float>(m_mpObj["TypeGroundToHitSphereRadiusIsGrassDash"].number_value()), KdCollider::TypeGround);
		if (hitInfo.hit)
		{
			// 球とモデルが当たっている
			m_pos += (hitInfo.hitDir * hitInfo.maxOverLap);
		}
	}

	for (auto& enemy : m_enemyList)
	{
		if (enemy.expired())continue;
		if (enemy.lock()->GetCharaState() & defense)
		{
			if (!(m_charaState & (grassHopperDash | grassHopperDashUp | step)))
			{
				hitInfo = SphereColisionCheck(m_pos + m_addCenterVal, static_cast<float>(m_mpObj["TypeGardToHitSphereRadius"].number_value()), KdCollider::TypeGard, enemy.lock());
			}
			else
			{
				hitInfo = SphereColisionCheck(m_pos + m_addCenterVal, static_cast<float>(m_mpObj["TypeGardToHitSphereRadiusIsGrassDash"].number_value()), KdCollider::TypeGard, enemy.lock());
			}

			if (HitOverRapComparison(hitInfoComparison.maxOverLap, hitInfo.maxOverLap, hitInfoComparison.hitDir, hitInfo.hitDir))
			{
				hitInfoComparison.hit = true;
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

	if (m_charaState & (rAttack | lAttack | rlAttack | rlAttackRush | CharaState::hit))
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

	hitInfoComparison.Init();
	for (auto& enemy : m_enemyList)
	{
		if (enemy.expired())continue;
		if (enemy.lock()->GetBDeath())continue;
		if (enemy.lock()->GetEnemyType() & Enemy::EnemyType::coarseFishEnemy)continue;

		if (enemy.lock()->GetEnemyType() & Enemy::EnemyType::bossEnemyTypeOne)
		{
			hitInfo = SphereColisionCheck(m_pos + m_addCenterVal, static_cast<float>(m_mpObj["TypeBumpToHitSphereRadiusIsBossEnemyTypeOne"].number_value()), KdCollider::TypeBump, enemy.lock());
		}
		else
		{
			hitInfo = SphereColisionCheck(m_pos + m_addCenterVal, static_cast<float>(m_mpObj["TypeBumpToHitSphereRadius"].number_value()), KdCollider::TypeBump, enemy.lock());
		}

		if (!(enemy.lock()->GetEnemyType() & Enemy::EnemyType::bossEnemyTypeOne) || m_charaState & (grassHopperDash | grassHopperDashUp | step))
		{
			hitInfo.hitDir.y = 0.0f;
		}

		if (HitOverRapComparison(hitInfoComparison.maxOverLap, hitInfo.maxOverLap, hitInfoComparison.hitDir, hitInfo.hitDir))
		{
			hitInfoComparison.hit = true;
		}
	}

	if (hitInfoComparison.hit)
	{
		hitInfoComparison.hitDir.Normalize();
		// 球とモデルが当たっている
		m_pos += (hitInfoComparison.hitDir * hitInfoComparison.maxOverLap);
	}

	hitInfoComparison.Init();
	if (!m_wpEnemy.expired())
	{
		hitInfo = SphereColisionCheck(m_pos + m_addCenterVal, static_cast<float>(m_mpObj["TypeAttackDecSphereRadius"].number_value()), KdCollider::TypeAttackDec, m_wpEnemy.lock());
		if (hitInfo.hit)
		{
			m_bAtttackMoveSpeedDec = true;
		}
		else
		{
			for (auto& enemy : m_enemyList)
			{
				if (enemy.expired())continue;
				if (enemy.lock()->GetBDeath())continue;
				hitInfo = SphereColisionCheck(m_pos + m_addCenterVal, static_cast<float>(m_mpObj["TypeAttackDecSphereRadius"].number_value()), KdCollider::TypeAttackDec, enemy.lock());

				if (HitOverRapComparison(hitInfoComparison.maxOverLap, hitInfo.maxOverLap, hitInfoComparison.hitDir, hitInfo.hitDir))
				{
					hitInfoComparison.hit = true;
				}
			}

			if (hitInfoComparison.hit)
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
			if (enemy.lock()->GetBDeath())continue;
			hitInfo = SphereColisionCheck(m_pos + m_addCenterVal, static_cast<float>(m_mpObj["TypeAttackDecSphereRadius"].number_value()), KdCollider::TypeAttackDec, enemy.lock());

			if (HitOverRapComparison(hitInfoComparison.maxOverLap, hitInfo.maxOverLap, hitInfoComparison.hitDir, hitInfo.hitDir))
			{
				hitInfoComparison.hit = true;
			}
		}

		if (hitInfoComparison.hit)
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
	HitInfo hitInfo;
	float enableStepHight = static_cast<float>(m_mpObj["EnableStepHight"].number_value());
	Math::Vector3 pos = m_pos + m_addCenterVal;
	hitInfo = RayColisionCheck(Math::Vector3(pos.x, pos.y + enableStepHight, pos.z), moveVec, a_moveSpd, KdCollider::TypeSpeedDec);

	if (hitInfo.hit)
	{
		if (hitInfo.maxOverLap <= 0.2)
		{
			a_moveSpd -= hitInfo.maxOverLap;
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

// プレイヤーのパンチが当たってるかどうかの判定処理
void Player::PlayerPanchiHitAttackChaeck()
{
	for (auto& enemy : m_enemyList)
	{
		if (enemy.expired())continue;
		float addBottomYVal = static_cast<float>(m_mpObj["AddBottomYVal"].number_value());
		if (PanchiAttackHitChaeck(enemy.lock(), "ArrmAttackPoint", addBottomYVal, 0.30f, KdCollider::TypeDamage, this))return;
		if (PanchiAttackHitChaeck(enemy.lock(), "ArrmAttackHitPoint", addBottomYVal, 0.30f, KdCollider::TypeDamage, this))return;
		if (PanchiAttackHitChaeck(enemy.lock(), "ArrmAttackHitPointTwo", addBottomYVal, 0.30f, KdCollider::TypeDamage, this))return;
	}
}
//＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝

// 攻撃が当たった時の処理＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
// 通常の攻撃が当たった時の処理
void Player::OnHit(Math::Vector3 a_KnocBackvec, CharacterBase* a_pAttackedCharacter)
{
	if (!a_pAttackedCharacter)
	{
		return;
	}

	int hitStop;
	if (a_pAttackedCharacter->GetCharaState() & rlAttackThree)
	{
		hitStop = 60;
	}
	else
	{
		hitStop = 20;
	}

	if (a_pAttackedCharacter->GetCharaState() & (rAttackOne | rlAttackOne) || a_pAttackedCharacter->GetCharaState() & rlAttackRush && (a_pAttackedCharacter->GetAnimationCnt() >= 31 && a_pAttackedCharacter->GetAnimationCnt() < 41))
	{
		SetHitMoveInfo(nomalHit, a_KnocBackvec, 0.05f, static_cast<float>((*m_wpJsonObj.lock())["OnHitDamage"].number_value()), hitStop, 5, m_spModel->GetAnimation("RHit1"), 0.01f, 1.25f);
	}
	else if (a_pAttackedCharacter->GetCharaState() & (rAttackTwo | rlAttackTwo))
	{
		SetHitMoveInfo(nomalHit, a_KnocBackvec, 0.05f, static_cast<float>((*m_wpJsonObj.lock())["OnHitDamage"].number_value()), hitStop, 5, m_spModel->GetAnimation("RHit2"), 0.01f, 1.25f);
	}

	if (a_pAttackedCharacter->GetCharaState() & rlAttackRush && a_pAttackedCharacter->GetAnimationCnt() >= 8 && (a_pAttackedCharacter->GetAnimationCnt() < 21) ||
		a_pAttackedCharacter->GetCharaState() & (lAttackOne | rlAttackThree))
	{
		SetHitMoveInfo(nomalHit, a_KnocBackvec, 0.05f, static_cast<float>((*m_wpJsonObj.lock())["OnHitDamage"].number_value()), hitStop, 2, m_spModel->GetAnimation("LHit1"), 0.01f, 1.25f);
	}
	else if (a_pAttackedCharacter->GetCharaState() & rlAttackRush && (a_pAttackedCharacter->GetAnimationCnt() >= 21 && a_pAttackedCharacter->GetAnimationCnt() < 31) ||
		a_pAttackedCharacter->GetCharaState() & lAttackTwo)
	{
		SetHitMoveInfo(nomalHit, a_KnocBackvec, 0.05f, static_cast<float>((*m_wpJsonObj.lock())["OnHitDamage"].number_value()), hitStop, 2, m_spModel->GetAnimation("LHit2"), 0.01f, 1.25f);
	}
}

// 吹き飛ばし攻撃が当たった時の処理
void Player::BlowingAwayAttackOnHit(Math::Vector3 a_KnocBackvec, CharacterBase* a_pAttackedCharacter)
{
	if (!a_pAttackedCharacter)return;

	Math::Vector3 nowVec = m_mWorld.Backward();
	float ang = DotProductCalculation(nowVec, a_KnocBackvec);

	if (a_pAttackedCharacter->GetCharaState() & rlAttackRush && a_pAttackedCharacter->GetAnimationCnt() >= 107 || a_pAttackedCharacter->GetNotHumanoidEnemyState() & Enemy::NotHumanoidEnemyState::rotationAttack)
	{
		if (ang > 90)
		{
			SetHitMoveInfo(blowingAwayHit, a_KnocBackvec, 0.95f, 30.0f, 40, 15, m_spModel->GetAnimation(" BlowingAwayHitB"), 0.01f, 1.45f, (*m_wpJsonObj.lock())["InvincibilityMaxTimeCnt"].int_value());
		}
		else
		{
			SetHitMoveInfo(blowingAwayHit, a_KnocBackvec, 0.95f, 30.0f, 40, 15, m_spModel->GetAnimation("BlowingAwayHitBB"), 0.01f, 1.45f, (*m_wpJsonObj.lock())["InvincibilityMaxTimeCnt"].int_value());
			m_bBlowingAwayHitB = true;
		}
	}
	else if (a_pAttackedCharacter->GetCharaState() & (rAttackOne | lAttackOne) && a_pAttackedCharacter->GetCharaState() & (grassHopperDashF), (*m_wpJsonObj.lock())["InvincibilityMaxTimeCnt"].int_value())
	{
		if (ang > 90)
		{
			SetHitMoveInfo(blowingAwayHit, a_KnocBackvec, 0.35f, 30.0f, 40, 15, m_spModel->GetAnimation(" BlowingAwayHitB"), 0.01f, 1.45f, (*m_wpJsonObj.lock())["InvincibilityMaxTimeCnt"].int_value());
		}
		else
		{
			SetHitMoveInfo(blowingAwayHit, a_KnocBackvec, 0.35f, 30.0f, 40, 15, m_spModel->GetAnimation("BlowingAwayHitBB"), 0.01f, 1.45f, (*m_wpJsonObj.lock())["InvincibilityMaxTimeCnt"].int_value());
			m_bBlowingAwayHitB = true;
		}
	}
	else
	{
		if (ang > 90)
		{
			SetHitMoveInfo(blowingAwayHit, a_KnocBackvec, 0.3f, 15.0f, 40, 8, m_spModel->GetAnimation(" BlowingAwayHitB"), 0.01f, 1.45f, (*m_wpJsonObj.lock())["InvincibilityMaxTimeCnt"].int_value());
		}
		else
		{
			SetHitMoveInfo(blowingAwayHit, a_KnocBackvec, 0.3f, 15.0f, 40, 8, m_spModel->GetAnimation("BlowingAwayHitBB"), 0.01f, 1.45f, (*m_wpJsonObj.lock())["InvincibilityMaxTimeCnt"].int_value());
			m_bBlowingAwayHitB = true;
		}
	}
}

// 居合切り系の攻撃が当たった時の処理
void Player::IaiKiriAttackOnHit(Math::Vector3 a_KnocBackvec)
{
	SetHitMoveInfo(iaiKiriHit, a_KnocBackvec, 0.0f, 50.0f, 40, 8, m_spModel->GetAnimation("IaiKiriAttackHitB"), 0.05f, 1.5f, (*m_wpJsonObj.lock())["InvincibilityMaxTimeCnt"].int_value());
}

// 切り上げ攻撃が当たった時の処理
void Player::CutRaiseOnHit(Math::Vector3 a_KnocBackvec)
{
	SetHitMoveInfo(cutRaiseHit, a_KnocBackvec, 0.0f, 15.0f, 60, 8, m_spModel->GetAnimation("CutRaiseHit"), 0.01f, 1.15f);

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
}
//＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝

// 攻撃が防がれた時の処理
void Player::HasDefense()
{
	if (m_charaState & (rAttack | rlAttack))
	{
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("RHasDefense"), false);
	}
	else if (m_charaState & lAttack)
	{
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("LHasDefense"), false);
	}

	m_charaState = hasDefense;
}

// 描画関係＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
// スキンメッシュアニメーションするモデルの描画
void Player::DrawLit_SkinMesh()
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

	if (m_charaState & defense && m_spModel->GetAnimation("LHasDefense"))
	{
		KdShaderManager::Instance().m_HD2DShader.SetLimLightEnable(true);
		KdShaderManager::Instance().m_HD2DShader.SetLimLight({ static_cast<float>(m_mpObj["StateDefenseMomentLightColor"][0].number_value()),
															   static_cast<float>(m_mpObj["StateDefenseMomentLightColor"][1].number_value()),
															   static_cast<float>(m_mpObj["StateDefenseMomentLightColor"][2].number_value()) },
			static_cast<float>(m_mpObj["StateDefenseMomentLightLevel"].number_value()));
	}

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

// 光の影響を受けないオブジェクトの描画
void Player::DrawUnLit()
{
	if (m_bLose)return;
	if (m_wpEnemy.expired())return;
	if (m_wpEnemy.lock()->GetBDeath())return;
	KdShaderManager::Instance().m_HD2DShader.DrawPolygon(m_rocKOnPolygon, m_rockOnPolyMat);
}
//＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝

// 武器装備時の行動選択＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
// Hoperr装備時の動きを決める処理
void Player::GrassMoveVecDecision()
{
	std::shared_ptr<CameraBase> spCamera = m_wpCamera.lock();
	if (!spCamera)return;

	if (m_rGrassHopperPauCnt == 0 && m_weaponType & grassHopper)
	{
		Math::Vector3 vec = m_mWorld.Backward();
		if (KdInputManager::Instance().GetButtonState("rAttack"))
		{
			m_rGrassHopperPauCnt = GRASS_HOPPER_PAUCNT;
			m_rGrassHopperTime = (*m_wpJsonObj.lock())["GrassDashMaxTime"].int_value();
			m_lGrassHopperTime = 0;
			m_grasRotYMat = spCamera->GetRotationYMatrix();

			if (KdInputManager::Instance().GetButtonState("forward"))
			{
				Math::Vector3 dir = Math::Vector3::TransformNormal(Math::Vector3(0, 0, 1), spCamera->GetRotationYMatrix());
				SetGrassDashInfo(dir, grassHopperDashF, m_spModel->GetAnimation("GrassDashFB"), spCamera->GetYAng(), 2, vec, m_mWorldRot.y);
			}
			else if (KdInputManager::Instance().GetButtonState("left"))
			{
				Math::Vector3 dir = Math::Vector3::TransformNormal(Math::Vector3(-1, 0, 0), spCamera->GetRotationYMatrix());
				vec = m_mWorld.Left();
				SetGrassDashInfo(dir, grassHopperDashL, m_spModel->GetAnimation("GrassDashLB"), spCamera->GetYAng(), 2, vec, m_mWorldRot.y, 90.0f);
			}

			else if (KdInputManager::Instance().GetButtonState("backward"))
			{
				Math::Vector3 dir = Math::Vector3::TransformNormal(Math::Vector3(0, 0, -1), spCamera->GetRotationYMatrix());
				vec = m_mWorld.Forward();
				SetGrassDashInfo(dir, grassHopperDashB, m_spModel->GetAnimation("GrassDashBB"), spCamera->GetYAng(), 2, vec, m_mWorldRot.y, 180.0f);
			}
			else if (KdInputManager::Instance().GetButtonState("right"))
			{
				Math::Vector3 dir = Math::Vector3::TransformNormal(Math::Vector3(1, 0, 0), spCamera->GetRotationYMatrix());
				vec = m_mWorld.Right();
				SetGrassDashInfo(dir, grassHopperDashR, m_spModel->GetAnimation("GrassDashRB"), spCamera->GetYAng(), 2, vec, m_mWorldRot.y, 270.0f, "GrassHopperLegLPoint");
			}
			else if (KdInputManager::Instance().GetButtonState("jump"))
			{
				Math::Vector3 dir = Math::Vector3(0, 1, 0);
				SetGrassDashInfo(dir, grassHopperDashUp, m_spModel->GetAnimation("GrassDashJB"), spCamera->GetYAng(), 2, Math::Vector3::Zero, 270.0f);
			}
			else
			{
				Math::Vector3 dir = Math::Vector3(0, 1, 0);
				SetGrassDashInfo(dir, grassHopperDashUp, m_spModel->GetAnimation("GrassDashJB"), spCamera->GetYAng(), 2, Math::Vector3::Zero, 270.0f);
			}
		}
	}

	if (m_lGrassHopperPauCnt == 0 && m_weaponType & lGrassHopper)
	{
		Math::Vector3 vec = m_mWorld.Backward();
		if (KdInputManager::Instance().GetButtonState("lAttack"))
		{
			m_lGrassHopperPauCnt = GRASS_HOPPER_PAUCNT;
			m_lGrassHopperTime = (*m_wpJsonObj.lock())["GrassDashMaxTime"].int_value();
			m_rGrassHopperTime = 0;
			m_grasRotYMat = spCamera->GetRotationYMatrix();

			if (KdInputManager::Instance().GetButtonState("forward"))
			{
				Math::Vector3 dir = Math::Vector3::TransformNormal(Math::Vector3(0, 0, 1), spCamera->GetRotationYMatrix());
				SetGrassDashInfo(dir, grassHopperDashF, m_spModel->GetAnimation("GrassDashFB"), spCamera->GetYAng(), 3, vec, m_mWorldRot.y);
			}
			else if (KdInputManager::Instance().GetButtonState("left"))
			{
				Math::Vector3 dir = Math::Vector3::TransformNormal(Math::Vector3(-1, 0, 0), spCamera->GetRotationYMatrix());
				vec = m_mWorld.Left();
				SetGrassDashInfo(dir, grassHopperDashL, m_spModel->GetAnimation("GrassDashLB"), spCamera->GetYAng(), 3, vec, m_mWorldRot.y, 90.0f);
			}

			else if (KdInputManager::Instance().GetButtonState("backward"))
			{
				Math::Vector3 dir = Math::Vector3::TransformNormal(Math::Vector3(0, 0, -1), spCamera->GetRotationYMatrix());
				vec = m_mWorld.Forward();
				SetGrassDashInfo(dir, grassHopperDashB, m_spModel->GetAnimation("GrassDashBB"), spCamera->GetYAng(), 3, vec, m_mWorldRot.y, 180.0f);
			}
			else if (KdInputManager::Instance().GetButtonState("right"))
			{
				Math::Vector3 dir = Math::Vector3::TransformNormal(Math::Vector3(1, 0, 0), spCamera->GetRotationYMatrix());
				vec = m_mWorld.Right();
				SetGrassDashInfo(dir, grassHopperDashR, m_spModel->GetAnimation("GrassDashRB"), spCamera->GetYAng(), 3, vec, m_mWorldRot.y, 270.0f, "GrassHopperLegLPoint");
			}
			else if (KdInputManager::Instance().GetButtonState("jump"))
			{
				Math::Vector3 dir = Math::Vector3(0, 1, 0);
				SetGrassDashInfo(dir, grassHopperDashUp, m_spModel->GetAnimation("GrassDashJB"), spCamera->GetYAng(), 3, Math::Vector3::Zero, 270.0f);
			}
			else
			{
				Math::Vector3 dir = Math::Vector3(0, 1, 0);
				SetGrassDashInfo(dir, grassHopperDashUp, m_spModel->GetAnimation("GrassDashJB"), spCamera->GetYAng(), 3, Math::Vector3::Zero, 270.0f);
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
			if (!(m_charaState & idle))
			{
				m_spAnimator->SetAnimation(m_spModel->GetAnimation("IdleA"), false);
			}
			m_charaState = idle;
		}
	}
	else
	{
		m_bMove = false;
		if (!(m_charaState & idle))
		{
			m_spAnimator->SetAnimation(m_spModel->GetAnimation("IdleA"), false);
		}
		m_charaState = idle;
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
				if (!(m_charaState & (rAttack | lAttack | defense | mantis | rlAttack | rlAttackRush)) && m_weaponType & lScorpion)
				{
					m_charaState |= defense;
					m_charaState &= defense;
					m_bMove = true;
					m_spAnimator->SetAnimation(m_spModel->GetAnimation("Defense"), true);
				}
				return;
			}

			if (KdInputManager::Instance().GetButtonState("rAttack"))
			{
				if (!(m_charaState & (rAttack | lAttack | defense | mantis | rlAttack | rlAttackRush)) && m_weaponType & scorpion)
				{
					m_charaState |= defense;
					m_charaState &= defense;
					m_bMove = true;
					m_spAnimator->SetAnimation(m_spModel->GetAnimation("Defense"), true);
				}
				return;
			}
		}
	}
	else
	{
		std::shared_ptr<CameraBase> spCamera = m_wpCamera.lock();
		if (!spCamera)return;

		if (!m_bAttackAnimeDelay)
		{
			if (m_charaState & grassHopperDash && m_lGrassHopperTime > (*m_wpJsonObj.lock())["GrassDashSteppingAwayJustBefore"].int_value() || m_rGrassHopperTime > (*m_wpJsonObj.lock())["GrassDashSteppingAwayJustBefore"].int_value())return;
			if (!(m_charaState & (rAttack | lAttack | mantis | rlAttack | rlAttackRush | defense)))
			{
				FirstAttackDecision(spCamera);
			}
		}
		else
		{
			if (!(m_charaState & mantis))
			{
				FirstAfterAttackDecision(spCamera);
			}
		}
	}
}

void Player::FirstAttackDecision(std::shared_ptr<CameraBase> a_spCamera)
{
	if (m_weaponType & lScorpion && m_weaponType & scorpion)
	{
		if (KdInputManager::Instance().GetButtonState("lAttack"))
		{
			if (KdInputManager::Instance().GetButtonState("rAttack"))
			{
				SetDefenseSucAndAttackHit();
				SetAttackInfo(mantis, mantis, Math::Vector3::Zero, m_spModel->GetAnimation("Mantis"), 0.0f);
				const std::shared_ptr<Scopion> scopion = std::dynamic_pointer_cast<Scopion>(m_weaponList[0]);
				scopion->SetMantis(a_spCamera->GetRotationMatrix(), true);
				const std::shared_ptr<Scopion> scopion2 = std::dynamic_pointer_cast<Scopion>(m_weaponList[1]);
				scopion2->SetBMantis(true);
				return;
			}
			else
			{
				m_bRushAttackPossible = false;
				SetDefenseSucAndAttackHit();
				SetAttackInfo(rlAttackOne, rlAttackOne, a_spCamera->GetMatrix().Backward(), m_spModel->GetAnimation("RLAttackOne"), static_cast<float>(m_mpObj["FirstAttackMoveSpeed"].number_value()));
				return;
			}
		}

		if (KdInputManager::Instance().GetButtonState("rAttack"))
		{
			if (KdInputManager::Instance().GetButtonState("lAttack"))
			{
				SetDefenseSucAndAttackHit();
				SetAttackInfo(mantis, mantis, Math::Vector3::Zero, m_spModel->GetAnimation("Mantis"), 0.0f);
				const std::shared_ptr<Scopion> scopion = std::dynamic_pointer_cast<Scopion>(m_weaponList[0]);
				scopion->SetMantis(a_spCamera->GetRotationMatrix(), true);
				const std::shared_ptr<Scopion> scopion2 = std::dynamic_pointer_cast<Scopion>(m_weaponList[1]);
				scopion2->SetBMantis(true);
				return;
			}
			else
			{
				m_bRushAttackPossible = false;
				SetDefenseSucAndAttackHit();
				SetAttackInfo(rlAttackOne, rlAttackOne, a_spCamera->GetMatrix().Backward(), m_spModel->GetAnimation("RLAttackOne"), static_cast<float>(m_mpObj["FirstAttackMoveSpeed"].number_value()));
				return;
			}
		}
	}


	if (m_weaponType & lScorpion)
	{
		if (KdInputManager::Instance().GetButtonState("lAttack"))
		{
			SetDefenseSucAndAttackHit();
			if (m_charaState & grassHopperDash)
			{
				SetAttackInfo(lAttackOne, static_cast<CharaState>((~rAttack & ~run)), a_spCamera->GetMatrix().Backward(), m_spModel->GetAnimation("GrassDashLAttack"), 0.0f);
			}
			else
			{
				SetAttackInfo(lAttackOne, static_cast<CharaState>((~rAttack & ~run)), a_spCamera->GetMatrix().Backward(), m_spModel->GetAnimation("LAttack1"), static_cast<float>(m_mpObj["FirstAttackMoveSpeed"].number_value()));
			}
			return;
		}
	}

	if (m_weaponType & scorpion)
	{
		if (KdInputManager::Instance().GetButtonState("rAttack"))
		{
			SetDefenseSucAndAttackHit();
			if (m_charaState & (grassHopperDashF | step))
			{
				SetAttackInfo(rAttackOne, static_cast<CharaState>((~lAttack & ~run)), a_spCamera->GetMatrix().Backward(), m_spModel->GetAnimation("GrassDashRAttack"), 0.0f);
			}
			else
			{
				SetAttackInfo(rAttackOne, static_cast<CharaState>((~lAttack & ~run)), a_spCamera->GetMatrix().Backward(), m_spModel->GetAnimation("RAttack1"), static_cast<float>(m_mpObj["FirstAttackMoveSpeed"].number_value()));
			}
			return;
		}
	}
}

void Player::FirstAfterAttackDecision(std::shared_ptr<CameraBase> a_spCamera)
{
	if (m_weaponType & lScorpion && m_weaponType & scorpion)
	{
		if (KdInputManager::Instance().GetButtonState("lAttack") ||
			KdInputManager::Instance().GetButtonState("rAttack"))
		{
			if (m_charaState & rlAttackOne)
			{
				SetDefenseSucAndAttackHit();
				SetAttackInfo(rlAttackTwo, rlAttackTwo, a_spCamera->GetMatrix().Backward(), m_spModel->GetAnimation("RLAttackTwo"), static_cast<float>(m_mpObj["AfterTheSecondAttackMoveSpeed"].number_value()));
			}
			else if (m_charaState & rlAttackTwo)
			{
				SetDefenseSucAndAttackHit();
				SetAttackInfo(rlAttackThree, rlAttackThree, a_spCamera->GetMatrix().Backward(), m_spModel->GetAnimation("RLAttackThree"), static_cast<float>(m_mpObj["RLAttackThreeMoveSpeed"].number_value()));
			}
			else if (m_charaState & rlAttackThree && m_bRushAttackPossible)
			{
				SetDefenseSucAndAttackHit();
				if (KdInputManager::Instance().GetButtonState("lAttack"))
				{
					m_bRushRp = false;
					SetAttackInfo(rlAttackRush, rlAttackRush, a_spCamera->GetMatrix().Backward(), m_spModel->GetAnimation("RLAttackRush"), static_cast<float>(m_mpObj["AfterTheSecondAttackMoveSpeed"].number_value()));
				}
				else if (KdInputManager::Instance().GetButtonState("rAttack"))
				{
					m_bRushRp = true;
					SetAttackInfo(rlAttackRush, rlAttackRush, a_spCamera->GetMatrix().Backward(), m_spModel->GetAnimation("RLAttackRushRP"), static_cast<float>(m_mpObj["RLAttackRotationRushMoveSpeed"].number_value()));
				}
			}
		}
	}
	else if (m_weaponType & lScorpion)
	{
		if (KdInputManager::Instance().GetButtonState("lAttack"))
		{
			if (m_charaState & lAttackOne)
			{
				SetAttackInfo(lAttackTwo, lAttackTwo, a_spCamera->GetMatrix().Backward(), m_spModel->GetAnimation("LAttack2"), static_cast<float>(m_mpObj["AfterTheSecondAttackMoveSpeed"].number_value()));
			}
			else if (m_charaState & lAttackTwo)
			{
				SetAttackInfo(lAttackThree, lAttackThree, a_spCamera->GetMatrix().Backward(), m_spModel->GetAnimation("LAttack3"), static_cast<float>(m_mpObj["AfterTheSecondAttackMoveSpeed"].number_value()));
			}
		}
	}
	else if (m_weaponType & scorpion)
	{
		if (KdInputManager::Instance().GetButtonState("rAttack"))
		{
			if (m_charaState & rAttackOne)
			{
				SetAttackInfo(rAttackTwo, rAttackTwo, a_spCamera->GetMatrix().Backward(), m_spModel->GetAnimation("RAttack2"), static_cast<float>(m_mpObj["AfterTheSecondAttackMoveSpeed"].number_value()));
			}
			else if (m_charaState & rAttackTwo)
			{
				SetAttackInfo(rAttackThree, rAttackThree, a_spCamera->GetMatrix().Backward(), m_spModel->GetAnimation("RAttack3"), static_cast<float>(m_mpObj["AfterTheSecondAttackMoveSpeed"].number_value()));
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
		if (m_charaState & grassHopperDashF)
		{
			EffectTrackingUpdate("GrassDashBlurPlayer.efk", 1, m_mWorldRot.y, 0.0f, m_pos, m_addGrassDashEffectPosVal, m_mWorld.Backward());
		}
		else if (m_charaState & grassHopperDashB)
		{
			EffectTrackingUpdate("GrassDashBlurPlayer.efk", 1, m_mWorldRot.y + 180.0f, 0.0f, m_pos, m_addGrassDashEffectPosVal, m_mWorld.Forward());
		}
		else if (m_charaState & grassHopperDashR)
		{
			EffectTrackingUpdate("GrassDashBlurPlayer.efk", 1, m_mWorldRot.y + 90.0f, 0.0f, m_pos, m_addGrassDashEffectPosVal, m_mWorld.Right());
		}
		else if (m_charaState & grassHopperDashL)
		{
			EffectTrackingUpdate("GrassDashBlurPlayer.efk", 1, m_mWorldRot.y + 270.0f, 0.0f, m_pos, m_addGrassDashEffectPosVal, m_mWorld.Left());
		}
		else if (m_charaState & grassHopperDashUp)
		{
			EffectTrackingUpdate("GrassDashBlurPlayer.efk", 1, 0.0f, 270.0f, m_pos, { 0,static_cast<float>(m_mpObj["AddGrassDashUpEffectPosYVal"].number_value()) ,0 }, Math::Vector3::Zero);
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
		else
		{
			for (int i = 0; i < 6; ++i)
			{
				if (m_lGrassHopperTime == (*m_wpJsonObj.lock())["GrassDashSpeeedDecelerationTime"][i].int_value() ||
					m_rGrassHopperTime == (*m_wpJsonObj.lock())["GrassDashSpeeedDecelerationTime"][i].int_value())
				{
					m_dashSpd = static_cast<float>((*m_wpJsonObj.lock())["GrassDashSpeed"][i + 2].number_value());
				}
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
	}
	else
	{
		m_bMove = false;
		m_dashSpd = 0.0f;
		m_grassHopperDashDir = {};
		m_gravity = 0;
		m_charaState = idle;
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

	if (!(m_charaState & (lAttack | rAttack | defense | mantis | rlAttack | rlAttackRush)))
	{
		if (!StepMoveVecDecision())
		{
			moveVec = RunMoveVecDecsion();
			if (KdInputManager::Instance().GetButtonState("jump"))
			{
				if (!(m_charaState & (jump | fall)))
				{
					m_gravity = static_cast<float>(m_mpObj["JumpPow"].number_value());
					m_charaState |= jump;
					m_charaState &= jump;
					KdAudioManager::Instance().Play("Asset/Audio/SE/FootstepsConcrete2.wav");
					m_spAnimator->SetAnimation(m_spModel->GetAnimation("JumpA"), false);
					m_bMove = true;

				}
			}
		}


		std::shared_ptr<CameraBase> spCamera = m_wpCamera.lock();
		if (spCamera)
		{
			if (!(m_charaState & (grassHopperDash | grassHopperDashUp)))
			{
				moveVec = moveVec.Transform(moveVec, spCamera->GetRotationYMatrix());
			}

			moveVec.Normalize();
		}

		if (!(m_charaState & (grassHopperDash | grassHopperDashUp)))
		{
			m_pos += moveVec * moveSpd;
		}
	}

	if (m_charaState & jump)
	{
		if (m_spAnimator->IsAnimationEnd())
		{
			m_spAnimator->SetAnimation(m_spModel->GetAnimation("JumpB"), true);
		}
	}

	if (m_charaState & idle)
	{
		if (m_spAnimator->IsAnimationEnd())
		{
			m_spAnimator->SetAnimation(m_spModel->GetAnimation("IdleB"), true);
		}
	}

	if (m_charaState & fall)
	{
		if (m_spAnimator->IsAnimationEnd())
		{
			m_spAnimator->SetAnimation(m_spModel->GetAnimation("FallB"), true);
		}
	}

	UpdateRotate(moveVec);
}

bool Player::StepMoveVecDecision()
{
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
	{
		if (KdInputManager::Instance().GetButtonState("forward"))
		{
			if (SetStepDashInfo(Math::Vector3::TransformNormal(Math::Vector3(0, 0, 1), m_wpCamera.lock()->GetRotationYMatrix()), stepF, m_spModel->GetAnimation("GrassDashFB")))
			{
				m_grasRotYMat = m_wpCamera.lock()->GetRotationYMatrix();
				return true;
			}
		}

		if (KdInputManager::Instance().GetButtonState("left"))
		{
			if (SetStepDashInfo(Math::Vector3::TransformNormal(Math::Vector3(-1, 0, 0), m_wpCamera.lock()->GetRotationYMatrix()), stepL, m_spModel->GetAnimation("GrassDashLB")))
			{
				m_grasRotYMat = m_wpCamera.lock()->GetRotationYMatrix();
				return true;
			}
		}

		if (KdInputManager::Instance().GetButtonState("backward"))
		{
			if (SetStepDashInfo(Math::Vector3::TransformNormal(Math::Vector3(0, 0, -1), m_wpCamera.lock()->GetRotationYMatrix()), stepB, m_spModel->GetAnimation("GrassDashBB")))
			{
				m_grasRotYMat = m_wpCamera.lock()->GetRotationYMatrix();
				return true;
			}
		}

		if (KdInputManager::Instance().GetButtonState("right"))
		{
			if (SetStepDashInfo(Math::Vector3::TransformNormal(Math::Vector3(1, 0, 0), m_wpCamera.lock()->GetRotationYMatrix()), stepR, m_spModel->GetAnimation("GrassDashRB")))
			{
				m_grasRotYMat = m_wpCamera.lock()->GetRotationYMatrix();
				return true;
			}
		}
	}

	return false;
}

Math::Vector3 Player::RunMoveVecDecsion()
{
	Math::Vector3 moveVec = {};

	if (KdInputManager::Instance().GetButtonState("forward"))
	{
		moveVec += {0, 0, 1};
		m_bMove = true;
		if (!(m_charaState & (jump | fall)))
		{
			if (!(m_charaState & run))
			{
				m_spAnimator->SetAnimation(m_spModel->GetAnimation("RUN"));
				m_runAnimeCnt = 0;
			}
			m_charaState = run;
		}
	}

	if (KdInputManager::Instance().GetButtonState("left"))
	{
		moveVec += {-1, 0, 0};
		m_bMove = true;
		if (!(m_charaState & (jump | fall)))
		{
			if (!(m_charaState & run))
			{
				m_spAnimator->SetAnimation(m_spModel->GetAnimation("RUN"));
				m_runAnimeCnt = 0;
			}
			m_charaState = run;
		}
	}

	if (KdInputManager::Instance().GetButtonState("backward"))
	{
		moveVec += {0, 0, -1};
		m_bMove = true;
		if (!(m_charaState & (jump | fall)))
		{
			if (!(m_charaState & run))
			{
				m_spAnimator->SetAnimation(m_spModel->GetAnimation("RUN"));
				m_runAnimeCnt = 0;
			}
			m_charaState = run;
		}
	}

	if (KdInputManager::Instance().GetButtonState("right"))
	{
		moveVec += {1, 0, 0};
		m_bMove = true;
		if (!(m_charaState & (jump | fall)))
		{
			if (!(m_charaState & run))
			{
				m_spAnimator->SetAnimation(m_spModel->GetAnimation("RUN"));
				m_runAnimeCnt = 0;
			}
			m_charaState = run;
		}
	}

	return moveVec;
}
// 攻撃が防がれた時の行動処理
void Player::HasDefenseMove()
{
	m_bMove = true;
	if (m_spAnimator->IsAnimationEnd())
	{
		if (!(m_charaState & idle))
		{
			m_spAnimator->SetAnimation(m_spModel->GetAnimation("IdleA"), false);
		}
		m_charaState = idle;
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
	if (m_charaState & lAttack)
	{
		AttackInfoReset(lAttack);
		if (m_charaState & grassHopperDashF)
		{
			m_spAnimator->SetAnimation(m_spModel->GetAnimation("GrassDashFA"), true);
			m_bMove = true;
		}
	}

	if (m_charaState & rAttack)
	{
		AttackInfoReset(rAttack);
		if (m_charaState & grassHopperDashF)
		{
			m_spAnimator->SetAnimation(m_spModel->GetAnimation("GrassDashFA"), true);
			m_bMove = true;
		}
	}

	if (m_charaState & rlAttack)
	{
		AttackInfoReset(rlAttack);
	}

	if (m_charaState & rlAttackRush)
	{
		AttackInfoReset(rlAttackRush);
	}

	if (m_charaState & mantis)
	{
		AttackInfoReset(mantis);
		WeaponMantisReset();
	}
}

// 短剣での攻撃アニメーション中の処理
void Player::ScorpionAttackAnimationMoveProcess()
{
	if (!(m_charaState & rlAttackRush))
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
	if (!(m_charaState & (rlAttack | rlAttackRush)))
	{
		m_attackMoveSpd *= m_moveSpeedDecelerationamount;

		if (m_charaState & (rAttackTwo | lAttackTwo))
		{
			if (m_attackAnimeCnt == m_mpObj["AttackPointMoment"].int_value())
			{
				SetDefenseSucAndAttackHit();
			}
		}
		else if (m_charaState & (rAttackThree | lAttackThree))
		{
			if (m_attackAnimeCnt == m_mpObj["AttackPointMoment"].int_value())
			{
				SetDefenseSucAndAttackHit();
			}
		}
	}
	else
	{
		if (m_charaState & rlAttackOne)
		{
			m_attackMoveSpd *= m_moveSpeedDecelerationamount;
			if (m_attackAnimeCnt == m_mpObj["RLAttackOneShakenSecondMoment"].int_value())
			{
				SetDefenseSucAndAttackHit();
			}
		}
		else if (m_charaState & rlAttackTwo)
		{
			m_attackMoveSpd *= m_moveSpeedDecelerationamount;
			if (m_attackAnimeCnt == m_mpObj["RLAttackTwoShakenSecondMoment"].int_value())
			{
				SetDefenseSucAndAttackHit();
			}
		}
		else if (m_charaState & rlAttackThree)
		{
			m_attackMoveSpd *= m_moveSpeedDecelerationamount;
			if (m_attackAnimeCnt == m_mpObj["RLAttackThreeShakenSecondMoment"].int_value())
			{
				SetDefenseSucAndAttackHit();
			}
		}
		else if (m_charaState & rlAttackRush)
		{
			if (!m_bRushRp)
			{
				m_attackMoveSpd *= static_cast<float>((*m_wpJsonObj.lock())["RushAttackMoveSpeedDecelerationamount"].number_value());

				for (int i = 1; i < 7; ++i)
				{
					if (m_attackAnimeCnt == m_mpObj["RushLastAttackPointTime"].int_value())
					{
						SetDefenseSucAndAttackHit();
						m_attackMoveSpd = static_cast<float>(m_mpObj["RushLastAttackMoveSpeed"].number_value());
						break;
					}

					if (m_attackAnimeCnt == m_mpObj["RLAttackRushShakenMoment"][i].int_value())
					{
						SetDefenseSucAndAttackHit();
						m_attackMoveSpd = static_cast<float>(m_mpObj["RLAttackRushMoveSpeed"][i - 1].number_value());
						break;
					}
				}
			}
			else
			{
				if (m_attackAnimeCnt < m_mpObj["RotationRushLastAttackPointTime"].int_value())
				{
					m_attackMoveSpd *= m_moveSpeedDecelerationamount;
				}
				else
				{
					m_attackMoveSpd *= static_cast<float>(m_mpObj["RotationRushLastAttackMoveSpeedDecelerationamount"].number_value());
				}

				for (int i = 0; i < 8; ++i)
				{
					if (m_attackAnimeCnt == m_mpObj["RLAttackRotationRushShakenMoment"][i].int_value() || m_attackAnimeCnt == m_mpObj["RotationRushLastAttackPointTime"].int_value())
					{
						SetDefenseSucAndAttackHit();

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
						else
						{
							m_attackMoveDir = m_wpCamera.lock()->GetMatrix().Backward();
							m_attackMoveDir.y = 0;
							m_attackMoveDir.Normalize();
							m_attackMoveSpd = static_cast<float>(m_mpObj["RLAttackRotationRushMoveSpeed"].number_value());
						}
						break;
					}
				}
			}
		}
	}

	if (!(m_charaState & mantis))
	{
		UpdateRotate(m_attackMoveDir);
	}

	SpeedyMoveWallHitChack(m_attackMoveSpd, m_attackMoveDir);
	m_pos += m_attackMoveDir * m_attackMoveSpd;
}
//＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝

// 敵をロックオンする処理＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
void Player::EnemyRockOn()
{
	if (KdInputManager::Instance().IsPress("rockOn"))
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
				if (enemy.expired())continue;
				if (enemy.lock()->GetBDeath())continue;

				Math::Vector3 nowVec = gCamera->GetMatrix().Backward();
				nowVec.y = 0.0f;
				nowVec.Normalize();

				// 向きたい方向
				Math::Vector3 toVec = enemy.lock()->GetPos() - GetPos();
				toVec.y = 0.0f;
				toVec.Normalize();

				// 角度を取得
				float ang = DotProductCalculation(nowVec, toVec);
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
	}
}
// ＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝

// その他
void Player::SetDefenseSucAndAttackHit()
{
	for (auto& enemy : m_enemyList)
	{
		if (enemy.expired())continue;
		enemy.lock()->SetAttackHit(false);
		enemy.lock()->SetDefenseSuc(false);
	}
}