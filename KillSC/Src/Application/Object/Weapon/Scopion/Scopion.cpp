#include "Scopion.h"
#include "../../Character/Player/Player.h"
#include "../../Character/Enemy/Enemy.h"

void Scopion::Update()
{
	if (m_bPlayerWeapon)
	{
		if (m_bMantis && m_arrmType == lArrm)return;

		const std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(m_owner.lock());
		if (!(player->GetWeaponType() & (scorpion | lScorpion)))return;

		const KdModelWork::Node* node = nullptr;

		if (m_arrmType == rArrm && player->GetWeaponType() & scorpion)
		{
			node = player->GetModel()->FindNode("WeaponRPoint");
		}

		if (m_arrmType == lArrm && player->GetWeaponType() & lScorpion)
		{
			node = player->GetModel()->FindNode("WeaponLPoint");
		}


		if (node)
		{
			if (!m_bMantis)
			{
				m_mWorld = node->m_worldTransform * player->GetMatrix();
				m_mWorld._42 += 0.7f;
			}
			else
			{
				node = player->GetModel()->FindNode("MantisPoint");
				m_mWorld =/* m_mantisRot * */node->m_worldTransform * player->GetMatrix();
				m_mWorld._42 += 0.7f;
			}
		}

#ifdef _DEBUG
		// ÉKÅ[ÉhÇÃìñÇΩÇËîªíËÇâ¬éãâª
		node = m_model->FindNode("AttackPointTwo");
		Math::Matrix mat = node->m_worldTransform * m_mWorld;
		m_pDebugWire->AddDebugSphere
		(
			mat.Translation(),
			0.45f,
			{ 1,0,0,1 }
		);

		node = m_model->FindNode("AttackPointTwo");
		mat = node->m_worldTransform * m_mWorld;
		m_pDebugWire->AddDebugSphere
		(
			mat.Translation(),
			0.45f,
			{ 1,0,0,1 }
		);

		node = m_model->FindNode("AttackPointThree");
		mat = node->m_worldTransform * m_mWorld;
		m_pDebugWire->AddDebugSphere
		(
			mat.Translation(),
			0.45f,
			{ 1,0,0,1 }
		);
#endif

		if (!m_bMantis)
		{
			PlayerHitAttackChaeck();
		}
		else
		{
			PlayerManAttackChaeck();
		}
	}

	if (m_bEnemyWeapon)
	{
		const std::shared_ptr<Enemy> enemy = std::dynamic_pointer_cast<Enemy>(m_owner.lock());
		if (!(enemy->GetWeaponType() & (scorpion | lScorpion)))return;

		const KdModelWork::Node* node = nullptr;

		if (m_arrmType == rArrm && enemy->GetWeaponType() & scorpion)
		{
			node = enemy->GetModel()->FindNode("WeaponRPoint");
		}

		if (m_arrmType == lArrm && enemy->GetWeaponType() & lScorpion)
		{
			node = enemy->GetModel()->FindNode("WeaponLPoint");
		}

		if (node)
		{
			if (!m_bMantis)
			{
				m_mWorld = node->m_worldTransform * enemy->GetMatrix();
				m_mWorld._42 += 0.7f;
			}
			else
			{
				m_mWorld = m_mantisRot * node->m_worldTransform * enemy->GetMatrix();
				m_mWorld._42 += 0.7f;
			}
		}

#ifdef _DEBUG
		// ÉKÅ[ÉhÇÃìñÇΩÇËîªíËÇâ¬éãâª
		node = m_model->FindNode("AttackPointTwo");
		Math::Matrix mat = node->m_worldTransform * m_mWorld;
		m_pDebugWire->AddDebugSphere
		(
			mat.Translation(),
			0.8f,
			{ 1,0,0,1 }
		);

		node = m_model->FindNode("AttackPointTwo");
		mat = node->m_worldTransform * m_mWorld;
		m_pDebugWire->AddDebugSphere
		(
			mat.Translation(),
			0.8f,
			{ 1,0,0,1 }
		);

		node = m_model->FindNode("AttackPointThree");
		mat = node->m_worldTransform * m_mWorld;
		m_pDebugWire->AddDebugSphere
		(
			mat.Translation(),
			0.8f,
			{ 1,0,0,1 }
		);
#endif

		if (!m_bMantis)
		{
			EnemyHitAttackChaeck();
		}
		else
		{
			EnemyManAttackChaeck();
		}
	}

	if (m_bMantis)
	{
		m_animator->AdvanceTime(m_manModel->WorkNodes());
		if (m_animator->IsAnimationEnd())
		{
			m_bMantis = false;
		}
	}
}

void Scopion::DrawBright()
{
	/*const std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(m_owner.lock());
	if (!(player->GetWeaponType() & scorpion))return;

	KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_model, m_mWorld);*/
}

void Scopion::DrawLit()
{
	if (m_bPlayerWeapon)
	{
		if (!m_bMantis)
		{
			const std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(m_owner.lock());
			if (!(player->GetWeaponType() & (scorpion | lScorpion)))return;

			if (m_arrmType == rArrm && player->GetWeaponType() & scorpion)
			{
				KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_model, m_mWorld);
			}

			if (m_arrmType == lArrm && player->GetWeaponType() & lScorpion)
			{
				KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_model, m_mWorld);
			}
		}
		else
		{
			if (m_arrmType == rArrm)
			{
				KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_manModel, m_mWorld);
			}
		}
	}

	if (m_bEnemyWeapon)
	{
		if (!m_bMantis)
		{
			const std::shared_ptr<Enemy> enemy = std::dynamic_pointer_cast<Enemy>(m_owner.lock());
			if (!(enemy->GetWeaponType() & (scorpion | lScorpion)))return;

			if (m_arrmType == rArrm && enemy->GetWeaponType() & scorpion)
			{
				KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_model, m_mWorld);
			}

			if (m_arrmType == lArrm && enemy->GetWeaponType() & lScorpion)
			{
				KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_model, m_mWorld);
			}
		}
		else
		{
			if (m_arrmType == rArrm)
			{
				KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_manModel, m_mWorld);
			}
		}
	}
}

void Scopion::GenerateDepthMapFromLight()
{
	if (m_bPlayerWeapon)
	{
		if (!m_bMantis)
		{
			const std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(m_owner.lock());
			if (!(player->GetWeaponType() & (scorpion | lScorpion)))return;

			if (m_arrmType == rArrm && player->GetWeaponType() & scorpion)
			{
				KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_model, m_mWorld);
			}

			if (m_arrmType == lArrm && player->GetWeaponType() & lScorpion)
			{
				KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_model, m_mWorld);
			}
		}
		else
		{
			if (m_arrmType == rArrm)
			{
				KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_manModel, m_mWorld);
			}
		}
	}

	if (m_bEnemyWeapon)
	{
		if (!m_bMantis)
		{
			const std::shared_ptr<Enemy> enemy = std::dynamic_pointer_cast<Enemy>(m_owner.lock());
			if (!(enemy->GetWeaponType() & (scorpion | lScorpion)))return;

			if (m_arrmType == rArrm && enemy->GetWeaponType() & scorpion)
			{
				KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_model, m_mWorld);
			}

			if (m_arrmType == lArrm && enemy->GetWeaponType() & lScorpion)
			{
				KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_model, m_mWorld);
			}
		}
		else
		{
			if (m_arrmType == rArrm)
			{
				KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_manModel, m_mWorld);
			}
		}
	}
}

void Scopion::SetMantis(Math::Matrix a_mat, bool a_bMantis)
{
	m_mantisRot = a_mat;
	m_bMantis = a_bMantis;
	m_animator->SetAnimation(m_manModel->GetAnimation("mantis"), false);
}

void Scopion::Init()
{
	m_bMantis = false;
	m_mantisRot = Math::Matrix::Identity;
	m_model = std::make_shared<KdModelWork>();
	m_model->SetModelData
	(KdAssets::Instance().m_modeldatas.GetData
	("Asset/Models/Weapon/Scopion/scopion.gltf"));

	m_manModel = std::make_unique<KdModelWork>();
	m_manModel->SetModelData
	(KdAssets::Instance().m_modeldatas.GetData
	("Asset/Models/Weapon/Scopion/mantis.gltf"));

	m_pDebugWire = std::make_unique<KdDebugWireFrame>();
	m_animator = std::make_shared<KdAnimator>();
	// ìñÇΩÇËîªíËèâä˙âª
	m_pCollider = std::make_unique<KdCollider>();


	/*m_pCollider->RegisterCollisionShape
	("ScopionModel", m_model, KdCollider::TypeDamage);*/
	const KdModelWork::Node* node = nullptr;
	node = m_model->FindNode("AttackPointOne");
	Math::Matrix mat = node->m_worldTransform * m_mWorld;
	m_pCollider->RegisterCollisionShape
	("ScopionModelGardCol", mat.Translation(), 0.45f, KdCollider::TypeGard);

	node = m_model->FindNode("AttackPointTwo");
	mat = node->m_worldTransform * m_mWorld;
	m_pCollider->RegisterCollisionShape
	("ScopionModelGardCol", mat.Translation(), 0.45f, KdCollider::TypeGard);

	node = m_model->FindNode("AttackPointThree");
	mat = node->m_worldTransform * m_mWorld;
	m_pCollider->RegisterCollisionShape
	("ScopionModelGardCol", mat.Translation(), 0.45f, KdCollider::TypeGard);
}

void Scopion::PlayerHitAttackChaeck()
{
	const std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(m_owner.lock());
	if (!(player->GetWeaponType() & (scorpion | lScorpion)))return;

	const KdModelWork::Node* node = nullptr;
	Math::Matrix mat = Math::Matrix::Identity;

	/*for (auto& obj : m_pTargetList)
	{*/
	// çUåÇÇÃìñÇΩÇËîªíË
	for (auto& wepLis : m_pTarget.lock()->GetWeaponList())
	{
		if (m_pTarget.lock()->GetEnemyState() & eDefense && !m_pTarget.lock()->GetAttackHit() && !m_pTarget.lock()->GetDefenseSuc() && player->GetPlayerState() & (Player::PlayerState::rAttack | Player::PlayerState::lAttack | Player::PlayerState::rlAttack))
		{
			if ((player->GetPlayerState() & Player::PlayerState::rAttack) && m_arrmType == lArrm)break;
			if ((player->GetPlayerState() & Player::PlayerState::lAttack) && m_arrmType == rArrm)break;

			node = m_model->FindNode("AttackPointOne");
			KdCollider::SphereInfo sphereInfo;
			mat = node->m_worldTransform * m_mWorld;
			sphereInfo.m_sphere.Center = mat.Translation();
			sphereInfo.m_sphere.Radius = 0.25f;
			sphereInfo.m_type = KdCollider::TypeGard;

#ifdef _DEBUG
			m_pDebugWire->AddDebugSphere
			(
				sphereInfo.m_sphere.Center,
				sphereInfo.m_sphere.Radius,
				{ 0,0,0,1 }
			);
#endif

			std::list<KdCollider::CollisionResult> retSphereList;

			wepLis->Intersects
			(
				sphereInfo,
				&retSphereList
			);

			Math::Vector3 hitDir = {};
			bool hit = false;
			for (auto& ret : retSphereList)
			{
				if (ret.m_overlapDistance > 0)
				{
					Math::Vector3 nowDir = m_pTarget.lock()->GetMatrix().Backward();
					Math::Vector3 targetDir = m_owner.lock()->GetPos() - m_pTarget.lock()->GetPos();
					float dot = nowDir.Dot(targetDir);
					dot = std::clamp(dot, -1.0f, 1.0f);

					float betweenAng = acos(dot);
					betweenAng = DirectX::XMConvertToDegrees(betweenAng);
					if (betweenAng >= 0 && betweenAng <= 45)
					{
						hit = true;
					}
				}
			}

			if (hit)
			{
				m_pTarget.lock()->SetDefenseSuc(true);
				m_owner.lock()->HasDefense();
				KdAudioManager::Instance().Play("Asset/Audio/SE/grad1.wav");
			}
			else
			{
				node = m_model->FindNode("AttackPointTwo");
				sphereInfo;
				mat = node->m_worldTransform * m_mWorld;
				sphereInfo.m_sphere.Center = mat.Translation();
				sphereInfo.m_sphere.Radius = 0.25f;
				sphereInfo.m_type = KdCollider::TypeGard;

#ifdef _DEBUG
				m_pDebugWire->AddDebugSphere
				(
					sphereInfo.m_sphere.Center,
					sphereInfo.m_sphere.Radius,
					{ 0,0,0,1 }
				);
#endif

				retSphereList.clear();

				wepLis->Intersects
				(
					sphereInfo,
					&retSphereList
				);

				hit = false;
				for (auto& ret : retSphereList)
				{
					if (ret.m_overlapDistance > 0)
					{
						Math::Vector3 nowDir = m_pTarget.lock()->GetMatrix().Backward();
						Math::Vector3 targetDir = m_owner.lock()->GetPos() - m_pTarget.lock()->GetPos();
						float dot = nowDir.Dot(targetDir);
						dot = std::clamp(dot, -1.0f, 1.0f);

						float betweenAng = acos(dot);
						betweenAng = DirectX::XMConvertToDegrees(betweenAng);
						if (betweenAng >= 0 && betweenAng <= 45)
						{
							hit = true;
						}
					}
				}

				if (hit)
				{
					m_pTarget.lock()->SetDefenseSuc(true);
					m_owner.lock()->HasDefense();
					KdAudioManager::Instance().Play("Asset/Audio/SE/grad1.wav");
				}
				else
				{
					node = m_model->FindNode("AttackPointThree");
					sphereInfo;
					mat = node->m_worldTransform * m_mWorld;
					sphereInfo.m_sphere.Center = mat.Translation();
					sphereInfo.m_sphere.Radius = 0.25f;
					sphereInfo.m_type = KdCollider::TypeGard;

#ifdef _DEBUG
					m_pDebugWire->AddDebugSphere
					(
						sphereInfo.m_sphere.Center,
						sphereInfo.m_sphere.Radius,
						{ 0,0,0,1 }
					);
#endif

					retSphereList.clear();

					wepLis->Intersects
					(
						sphereInfo,
						&retSphereList
					);

					hit = false;
					for (auto& ret : retSphereList)
					{
						if (ret.m_overlapDistance > 0)
						{
							Math::Vector3 nowDir = m_pTarget.lock()->GetMatrix().Backward();
							Math::Vector3 targetDir = m_owner.lock()->GetPos() - m_pTarget.lock()->GetPos();
							float dot = nowDir.Dot(targetDir);
							dot = std::clamp(dot, -1.0f, 1.0f);

							float betweenAng = acos(dot);
							betweenAng = DirectX::XMConvertToDegrees(betweenAng);
							if (betweenAng >= 0 && betweenAng <= 45)
							{
								hit = true;
							}
						}
					}

					if (hit)
					{
						m_pTarget.lock()->SetDefenseSuc(true);
						m_owner.lock()->HasDefense();
						KdAudioManager::Instance().Play("Asset/Audio/SE/grad1.wav");
					}
				}
			}
		}
	}
	//}

	if (!m_pTarget.lock()->GetAttackHit() && !m_pTarget.lock()->GetDefenseSuc() && player->GetPlayerState() & (Player::PlayerState::rAttack | Player::PlayerState::lAttack | Player::PlayerState::rlAttack) && m_pTarget.lock()->GetInvincibilityTimeCnt() == 0)
	{
		if (player->GetPlayerState() & Player::PlayerState::rAttack && m_arrmType == lArrm)return;
		if (player->GetPlayerState() & Player::PlayerState::lAttack && m_arrmType == rArrm)return;
		node = m_model->FindNode("AttackPointOne");
		KdCollider::SphereInfo sphereInfo;
		mat = node->m_worldTransform * m_mWorld;
		sphereInfo.m_sphere.Center = mat.Translation();
		sphereInfo.m_sphere.Radius = 0.20f;
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
		m_pTarget.lock()->Intersects
		(
			sphereInfo,
			&retSphereList
		);

		Math::Vector3 hitDir = {};
		bool hit = false;
		for (auto& ret : retSphereList)
		{
			hit = true;
			hitDir = ret.m_hitDir;
		}

		if (hit)
		{
			if (player->GetPlayerState() & (Player::PlayerState::rAttackOne | Player::PlayerState::lAttackOne) && player->GetPlayerState() & (Player::PlayerState::grassHopperDashF))
			{
				m_pTarget.lock()->IaiKiriAttackOnHit(player->GetMatrix().Backward());
			}
			else if (player->GetPlayerState() & (Player::PlayerState::rAttackOne | Player::PlayerState::rAttackTwo |
				                                 Player::PlayerState::lAttackOne | Player::PlayerState::lAttackTwo | 
				                                 Player::PlayerState::rlAttack))
			{
				m_pTarget.lock()->OnHit(player->GetMatrix().Backward());
			}
			else if (player->GetPlayerState() & (Player::PlayerState::lAttackThree | Player::PlayerState::rAttackThree))
			{
				m_pTarget.lock()->BlowingAwayAttackOnHit(player->GetMatrix().Backward());
			}
			KdAudioManager::Instance().Play("Asset/Audio/SE/AttackHitOverlapping.wav");
		}
		else
		{
			node = m_model->FindNode("AttackPointTwo");
			sphereInfo;
			mat = node->m_worldTransform * m_mWorld;
			sphereInfo.m_sphere.Center = mat.Translation();
			sphereInfo.m_sphere.Radius = 0.20f;
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

			m_pTarget.lock()->Intersects
			(
				sphereInfo,
				&retSphereList
			);

			hit = false;
			for (auto& ret : retSphereList)
			{
				hit = true;
				hitDir = ret.m_hitDir;
			}

			if (hit)
			{
				if (player->GetPlayerState() & (Player::PlayerState::rAttackOne | Player::PlayerState::lAttackOne) && player->GetPlayerState() & (Player::PlayerState::grassHopperDashF))
				{
					m_pTarget.lock()->IaiKiriAttackOnHit(player->GetMatrix().Backward());
				}
				else if (player->GetPlayerState() & (Player::PlayerState::rAttackOne | Player::PlayerState::rAttackTwo |
					                                 Player::PlayerState::lAttackOne | Player::PlayerState::lAttackTwo |
					                                 Player::PlayerState::rlAttack))
				{
					m_pTarget.lock()->OnHit(player->GetMatrix().Backward());
				}
				else if (player->GetPlayerState() & (Player::PlayerState::lAttackThree | Player::PlayerState::rAttackThree))
				{
					m_pTarget.lock()->BlowingAwayAttackOnHit(player->GetMatrix().Backward());
				}
				KdAudioManager::Instance().Play("Asset/Audio/SE/AttackHitOverlapping.wav");
			}
			else
			{
				node = m_model->FindNode("AttackPointThree");
				sphereInfo;
				mat = node->m_worldTransform * m_mWorld;
				sphereInfo.m_sphere.Center = mat.Translation();
				sphereInfo.m_sphere.Radius = 0.20f;
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

				m_pTarget.lock()->Intersects
				(
					sphereInfo,
					&retSphereList
				);

				hit = false;
				for (auto& ret : retSphereList)
				{
					hit = true;
					hitDir = ret.m_hitDir;
				}

				if (hit)
				{
					if (player->GetPlayerState() & (Player::PlayerState::rAttackOne | Player::PlayerState::lAttackOne) && player->GetPlayerState() & (Player::PlayerState::grassHopperDashF))
					{
						m_pTarget.lock()->IaiKiriAttackOnHit(player->GetMatrix().Backward());
					}
					else if (player->GetPlayerState() & (Player::PlayerState::rAttackOne | Player::PlayerState::rAttackTwo |
						                                 Player::PlayerState::lAttackOne | Player::PlayerState::lAttackTwo |
						                                 Player::PlayerState::rlAttack))
					{
						m_pTarget.lock()->OnHit(player->GetMatrix().Backward());
					}
					else if (player->GetPlayerState() & (Player::PlayerState::lAttackThree | Player::PlayerState::rAttackThree))
					{
						m_pTarget.lock()->BlowingAwayAttackOnHit(player->GetMatrix().Backward());
					}
					KdAudioManager::Instance().Play("Asset/Audio/SE/AttackHitOverlapping.wav");
				}
			}
		}
		//}
	}
}

void Scopion::PlayerManAttackChaeck()
{
	const std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(m_owner.lock());
	if (!(player->GetWeaponType() & (scorpion | lScorpion)))return;

	const KdModelWork::Node* node = nullptr;
	Math::Matrix mat = Math::Matrix::Identity;

	/*for (auto& obj : m_pTargetList)
	{*/
	// çUåÇÇÃìñÇΩÇËîªíË
//	for (auto& wepLis : m_pTarget.lock()->GetWeaponList())
//	{
//		if (m_pTarget.lock()->GetEnemyState() & eDefense && !m_pTarget.lock()->GetAttackHit() && !m_pTarget.lock()->GetDefenseSuc() && player->GetPlayerState() & Player::PlayerState::mantis)
//		{
//			if ((player->GetPlayerState() & Player::PlayerState::rAttack) && m_arrmType == lArrm)break;
//			if ((player->GetPlayerState() & Player::PlayerState::lAttack) && m_arrmType == rArrm)break;
//
//			node = m_manModel->FindNode("AttackPointOne");
//			KdCollider::SphereInfo sphereInfo;
//			mat = node->m_worldTransform * m_mWorld;
//			sphereInfo.m_sphere.Center = mat.Translation();
//			sphereInfo.m_sphere.Radius = 0.25f;
//			sphereInfo.m_type = KdCollider::TypeGard;
//
//#ifdef _DEBUG
//			m_pDebugWire->AddDebugSphere
//			(
//				sphereInfo.m_sphere.Center,
//				sphereInfo.m_sphere.Radius,
//				{ 0,0,0,1 }
//			);
//#endif
//
//			std::list<KdCollider::CollisionResult> retSphereList;
//
//			wepLis->Intersects
//			(
//				sphereInfo,
//				&retSphereList
//			);
//
//			Math::Vector3 hitDir = {};
//			bool hit = false;
//			for (auto& ret : retSphereList)
//			{
//				if (ret.m_overlapDistance > 0)
//				{
//					Math::Vector3 nowDir = m_pTarget.lock()->GetMatrix().Backward();
//					Math::Vector3 targetDir = m_owner.lock()->GetPos() - m_pTarget.lock()->GetPos();
//					float dot = nowDir.Dot(targetDir);
//					dot = std::clamp(dot, -1.0f, 1.0f);
//
//					float betweenAng = acos(dot);
//					betweenAng = DirectX::XMConvertToDegrees(betweenAng);
//					if (betweenAng >= 0 && betweenAng <= 45)
//					{
//						hit = true;
//					}
//				}
//			}
//
//			if (hit)
//			{
//				m_pTarget.lock()->SetDefenseSuc(true);
//				m_owner.lock()->HasDefense();
//				KdAudioManager::Instance().Play("Asset/Audio/SE/grad1.wav");
//			}
//			else
//			{
//				node = m_manModel->FindNode("AttackPointTwo");
//				sphereInfo;
//				mat = node->m_worldTransform * m_mWorld;
//				sphereInfo.m_sphere.Center = mat.Translation();
//				sphereInfo.m_sphere.Radius = 0.25f;
//				sphereInfo.m_type = KdCollider::TypeGard;
//
//#ifdef _DEBUG
//				m_pDebugWire->AddDebugSphere
//				(
//					sphereInfo.m_sphere.Center,
//					sphereInfo.m_sphere.Radius,
//					{ 0,0,0,1 }
//				);
//#endif
//
//				retSphereList.clear();
//
//				wepLis->Intersects
//				(
//					sphereInfo,
//					&retSphereList
//				);
//
//				hit = false;
//				for (auto& ret : retSphereList)
//				{
//					if (ret.m_overlapDistance > 0)
//					{
//						Math::Vector3 nowDir = m_pTarget.lock()->GetMatrix().Backward();
//						Math::Vector3 targetDir = m_owner.lock()->GetPos() - m_pTarget.lock()->GetPos();
//						float dot = nowDir.Dot(targetDir);
//						dot = std::clamp(dot, -1.0f, 1.0f);
//
//						float betweenAng = acos(dot);
//						betweenAng = DirectX::XMConvertToDegrees(betweenAng);
//						if (betweenAng >= 0 && betweenAng <= 45)
//						{
//							hit = true;
//						}
//					}
//				}
//
//				if (hit)
//				{
//					m_pTarget.lock()->SetDefenseSuc(true);
//					m_owner.lock()->HasDefense();
//					KdAudioManager::Instance().Play("Asset/Audio/SE/grad1.wav");
//				}
//				else
//				{
//					node = m_manModel->FindNode("AttackPointThree");
//					sphereInfo;
//					mat = node->m_worldTransform * m_mWorld;
//					sphereInfo.m_sphere.Center = mat.Translation();
//					sphereInfo.m_sphere.Radius = 0.25f;
//					sphereInfo.m_type = KdCollider::TypeGard;
//
//#ifdef _DEBUG
//					m_pDebugWire->AddDebugSphere
//					(
//						sphereInfo.m_sphere.Center,
//						sphereInfo.m_sphere.Radius,
//						{ 0,0,0,1 }
//					);
//#endif
//
//					retSphereList.clear();
//
//					wepLis->Intersects
//					(
//						sphereInfo,
//						&retSphereList
//					);
//
//					hit = false;
//					for (auto& ret : retSphereList)
//					{
//						if (ret.m_overlapDistance > 0)
//						{
//							Math::Vector3 nowDir = m_pTarget.lock()->GetMatrix().Backward();
//							Math::Vector3 targetDir = m_owner.lock()->GetPos() - m_pTarget.lock()->GetPos();
//							float dot = nowDir.Dot(targetDir);
//							dot = std::clamp(dot, -1.0f, 1.0f);
//
//							float betweenAng = acos(dot);
//							betweenAng = DirectX::XMConvertToDegrees(betweenAng);
//							if (betweenAng >= 0 && betweenAng <= 45)
//							{
//								hit = true;
//							}
//						}
//					}
//
//					if (hit)
//					{
//						m_pTarget.lock()->SetDefenseSuc(true);
//						m_owner.lock()->HasDefense();
//						KdAudioManager::Instance().Play("Asset/Audio/SE/grad1.wav");
//					}
//					else
//					{
//						node = m_manModel->FindNode("AttackPointFour");
//						sphereInfo;
//						mat = node->m_worldTransform * m_mWorld;
//						sphereInfo.m_sphere.Center = mat.Translation();
//						sphereInfo.m_sphere.Radius = 0.25f;
//						sphereInfo.m_type = KdCollider::TypeGard;
//
//#ifdef _DEBUG
//						m_pDebugWire->AddDebugSphere
//						(
//							sphereInfo.m_sphere.Center,
//							sphereInfo.m_sphere.Radius,
//							{ 0,0,0,1 }
//						);
//#endif
//
//						retSphereList.clear();
//
//						wepLis->Intersects
//						(
//							sphereInfo,
//							&retSphereList
//						);
//
//						hit = false;
//						for (auto& ret : retSphereList)
//						{
//							if (ret.m_overlapDistance > 0)
//							{
//								Math::Vector3 nowDir = m_pTarget.lock()->GetMatrix().Backward();
//								Math::Vector3 targetDir = m_owner.lock()->GetPos() - m_pTarget.lock()->GetPos();
//								float dot = nowDir.Dot(targetDir);
//								dot = std::clamp(dot, -1.0f, 1.0f);
//
//								float betweenAng = acos(dot);
//								betweenAng = DirectX::XMConvertToDegrees(betweenAng);
//								if (betweenAng >= 0 && betweenAng <= 45)
//								{
//									hit = true;
//								}
//							}
//						}
//
//						if (hit)
//						{
//							m_pTarget.lock()->SetDefenseSuc(true);
//							m_owner.lock()->HasDefense();
//							KdAudioManager::Instance().Play("Asset/Audio/SE/grad1.wav");
//						}
//						else
//						{
//							node = m_manModel->FindNode("AttackPointFive");
//							sphereInfo;
//							mat = node->m_worldTransform * m_mWorld;
//							sphereInfo.m_sphere.Center = mat.Translation();
//							sphereInfo.m_sphere.Radius = 0.25f;
//							sphereInfo.m_type = KdCollider::TypeGard;
//
//#ifdef _DEBUG
//							m_pDebugWire->AddDebugSphere
//							(
//								sphereInfo.m_sphere.Center,
//								sphereInfo.m_sphere.Radius,
//								{ 0,0,0,1 }
//							);
//#endif
//
//							retSphereList.clear();
//
//							wepLis->Intersects
//							(
//								sphereInfo,
//								&retSphereList
//							);
//
//							hit = false;
//							for (auto& ret : retSphereList)
//							{
//								if (ret.m_overlapDistance > 0)
//								{
//									Math::Vector3 nowDir = m_pTarget.lock()->GetMatrix().Backward();
//									Math::Vector3 targetDir = m_owner.lock()->GetPos() - m_pTarget.lock()->GetPos();
//									float dot = nowDir.Dot(targetDir);
//									dot = std::clamp(dot, -1.0f, 1.0f);
//
//									float betweenAng = acos(dot);
//									betweenAng = DirectX::XMConvertToDegrees(betweenAng);
//									if (betweenAng >= 0 && betweenAng <= 45)
//									{
//										hit = true;
//									}
//								}
//							}
//
//							if (hit)
//							{
//								m_pTarget.lock()->SetDefenseSuc(true);
//								m_owner.lock()->HasDefense();
//								KdAudioManager::Instance().Play("Asset/Audio/SE/grad1.wav");
//							}
//							else
//							{
//								node = m_manModel->FindNode("AttackPointSix");
//								sphereInfo;
//								mat = node->m_worldTransform * m_mWorld;
//								sphereInfo.m_sphere.Center = mat.Translation();
//								sphereInfo.m_sphere.Radius = 0.25f;
//								sphereInfo.m_type = KdCollider::TypeGard;
//
//#ifdef _DEBUG
//								m_pDebugWire->AddDebugSphere
//								(
//									sphereInfo.m_sphere.Center,
//									sphereInfo.m_sphere.Radius,
//									{ 0,0,0,1 }
//								);
//#endif
//
//								retSphereList.clear();
//
//								wepLis->Intersects
//								(
//									sphereInfo,
//									&retSphereList
//								);
//
//								hit = false;
//								for (auto& ret : retSphereList)
//								{
//									if (ret.m_overlapDistance > 0)
//									{
//										Math::Vector3 nowDir = m_pTarget.lock()->GetMatrix().Backward();
//										Math::Vector3 targetDir = m_owner.lock()->GetPos() - m_pTarget.lock()->GetPos();
//										float dot = nowDir.Dot(targetDir);
//										dot = std::clamp(dot, -1.0f, 1.0f);
//
//										float betweenAng = acos(dot);
//										betweenAng = DirectX::XMConvertToDegrees(betweenAng);
//										if (betweenAng >= 0 && betweenAng <= 45)
//										{
//											hit = true;
//										}
//									}
//								}
//
//								if (hit)
//								{
//									m_pTarget.lock()->SetDefenseSuc(true);
//									m_owner.lock()->HasDefense();
//									KdAudioManager::Instance().Play("Asset/Audio/SE/grad1.wav");
//								}
//								else
//								{
//									node = m_manModel->FindNode("AttackPointSeven");
//									sphereInfo;
//									mat = node->m_worldTransform * m_mWorld;
//									sphereInfo.m_sphere.Center = mat.Translation();
//									sphereInfo.m_sphere.Radius = 0.25f;
//									sphereInfo.m_type = KdCollider::TypeGard;
//
//#ifdef _DEBUG
//									m_pDebugWire->AddDebugSphere
//									(
//										sphereInfo.m_sphere.Center,
//										sphereInfo.m_sphere.Radius,
//										{ 0,0,0,1 }
//									);
//#endif
//
//									retSphereList.clear();
//
//									wepLis->Intersects
//									(
//										sphereInfo,
//										&retSphereList
//									);
//
//									hit = false;
//									for (auto& ret : retSphereList)
//									{
//										if (ret.m_overlapDistance > 0)
//										{
//											Math::Vector3 nowDir = m_pTarget.lock()->GetMatrix().Backward();
//											Math::Vector3 targetDir = m_owner.lock()->GetPos() - m_pTarget.lock()->GetPos();
//											float dot = nowDir.Dot(targetDir);
//											dot = std::clamp(dot, -1.0f, 1.0f);
//
//											float betweenAng = acos(dot);
//											betweenAng = DirectX::XMConvertToDegrees(betweenAng);
//											if (betweenAng >= 0 && betweenAng <= 45)
//											{
//												hit = true;
//											}
//										}
//									}
//
//									if (hit)
//									{
//										m_pTarget.lock()->SetDefenseSuc(true);
//										m_owner.lock()->HasDefense();
//										KdAudioManager::Instance().Play("Asset/Audio/SE/grad1.wav");
//									}
//								}
//							}
//						}
//					}
//				}
//			}
//		}
//	}
//	//}

	if (!m_pTarget.lock()->GetAttackHit() && !m_pTarget.lock()->GetDefenseSuc() && player->GetPlayerState() & Player::PlayerState::mantis && m_pTarget.lock()->GetInvincibilityTimeCnt() == 0)
	{
		if (player->GetPlayerState() & Player::PlayerState::rAttack && m_arrmType == lArrm)return;
		if (player->GetPlayerState() & Player::PlayerState::lAttack && m_arrmType == rArrm)return;
		node = m_manModel->FindNode("AttackPointOne");
		KdCollider::SphereInfo sphereInfo;
		mat = node->m_worldTransform * m_mWorld;
		sphereInfo.m_sphere.Center = mat.Translation();
		sphereInfo.m_sphere.Radius = 0.20f;
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
		m_pTarget.lock()->Intersects
		(
			sphereInfo,
			&retSphereList
		);

		Math::Vector3 hitDir = {};
		bool hit = false;
		for (auto& ret : retSphereList)
		{
			hit = true;
			hitDir = ret.m_hitDir;
		}

		if (hit)
		{
			m_pTarget.lock()->BlowingAwayAttackOnHit(player->GetMatrix().Backward());
			KdAudioManager::Instance().Play("Asset/Audio/SE/AttackHitOverlapping.wav");
		}
		else
		{
			node = m_manModel->FindNode("AttackPointTwo");
			sphereInfo;
			mat = node->m_worldTransform * m_mWorld;
			sphereInfo.m_sphere.Center = mat.Translation();
			sphereInfo.m_sphere.Radius = 0.20f;
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

			m_pTarget.lock()->Intersects
			(
				sphereInfo,
				&retSphereList
			);

			hit = false;
			for (auto& ret : retSphereList)
			{
				hit = true;
				hitDir = ret.m_hitDir;
			}

			if (hit)
			{
				m_pTarget.lock()->BlowingAwayAttackOnHit(player->GetMatrix().Backward());
				(player->GetMatrix().Backward());
				KdAudioManager::Instance().Play("Asset/Audio/SE/AttackHitOverlapping.wav");
			}
			else
			{
				node = m_manModel->FindNode("AttackPointThree");
				sphereInfo;
				mat = node->m_worldTransform * m_mWorld;
				sphereInfo.m_sphere.Center = mat.Translation();
				sphereInfo.m_sphere.Radius = 0.20f;
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

				m_pTarget.lock()->Intersects
				(
					sphereInfo,
					&retSphereList
				);

				hit = false;
				for (auto& ret : retSphereList)
				{
					hit = true;
					hitDir = ret.m_hitDir;
				}

				if (hit)
				{
					m_pTarget.lock()->BlowingAwayAttackOnHit(player->GetMatrix().Backward());
					KdAudioManager::Instance().Play("Asset/Audio/SE/AttackHitOverlapping.wav");
				}
				else
				{
					node = m_manModel->FindNode("AttackPointFour");
					sphereInfo;
					mat = node->m_worldTransform * m_mWorld;
					sphereInfo.m_sphere.Center = mat.Translation();
					sphereInfo.m_sphere.Radius = 0.20f;
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

					m_pTarget.lock()->Intersects
					(
						sphereInfo,
						&retSphereList
					);

					hit = false;
					for (auto& ret : retSphereList)
					{
						hit = true;
						hitDir = ret.m_hitDir;
					}

					if (hit)
					{
						m_pTarget.lock()->BlowingAwayAttackOnHit(player->GetMatrix().Backward());
						KdAudioManager::Instance().Play("Asset/Audio/SE/AttackHitOverlapping.wav");
					}
					else
					{
						node = m_manModel->FindNode("AttackPointFive");
						sphereInfo;
						mat = node->m_worldTransform * m_mWorld;
						sphereInfo.m_sphere.Center = mat.Translation();
						sphereInfo.m_sphere.Radius = 0.20f;
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

						m_pTarget.lock()->Intersects
						(
							sphereInfo,
							&retSphereList
						);

						hit = false;
						for (auto& ret : retSphereList)
						{
							hit = true;
							hitDir = ret.m_hitDir;
						}

						if (hit)
						{
							m_pTarget.lock()->BlowingAwayAttackOnHit(player->GetMatrix().Backward());
							KdAudioManager::Instance().Play("Asset/Audio/SE/AttackHitOverlapping.wav");
						}
						else
						{
							node = m_manModel->FindNode("AttackPointSix");
							sphereInfo;
							mat = node->m_worldTransform * m_mWorld;
							sphereInfo.m_sphere.Center = mat.Translation();
							sphereInfo.m_sphere.Radius = 0.20f;
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

							m_pTarget.lock()->Intersects
							(
								sphereInfo,
								&retSphereList
							);

							hit = false;
							for (auto& ret : retSphereList)
							{
								hit = true;
								hitDir = ret.m_hitDir;
							}

							if (hit)
							{
								m_pTarget.lock()->BlowingAwayAttackOnHit(player->GetMatrix().Backward());
								KdAudioManager::Instance().Play("Asset/Audio/SE/AttackHitOverlapping.wav");
							}
							else
							{
								node = m_manModel->FindNode("AttackPointSeven");
								sphereInfo;
								mat = node->m_worldTransform * m_mWorld;
								sphereInfo.m_sphere.Center = mat.Translation();
								sphereInfo.m_sphere.Radius = 0.20f;
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

								m_pTarget.lock()->Intersects
								(
									sphereInfo,
									&retSphereList
								);

								hit = false;
								for (auto& ret : retSphereList)
								{
									hit = true;
									hitDir = ret.m_hitDir;
								}

								if (hit)
								{
									m_pTarget.lock()->BlowingAwayAttackOnHit(player->GetMatrix().Backward());
									KdAudioManager::Instance().Play("Asset/Audio/SE/AttackHitOverlapping.wav");
								}
							}
						}
					}
				}
			}
		}
		//}
	}
}

void Scopion::EnemyHitAttackChaeck()
{
	const std::shared_ptr<Enemy> enemy = std::dynamic_pointer_cast<Enemy>(m_owner.lock());
	if (!(enemy->GetWeaponType() & (scorpion | lScorpion)))return;

	const KdModelWork::Node* node = nullptr;
	Math::Matrix mat = Math::Matrix::Identity;

	/*for (auto& obj : m_pTargetList)
{*/
// çUåÇÇÃìñÇΩÇËîªíË
	for (auto& wepLis : m_eTarget.lock()->GetWeaponList())
	{
		if (m_eTarget.lock()->GetPlayerState() & Player::PlayerState::defense && !m_eTarget.lock()->GetAttackHit() && !m_eTarget.lock()->GetDefenseSuc() && enemy->GetEnemyState() & (eRAttack | eLAttack))
		{
			if ((enemy->GetEnemyState() & eRAttack) && m_arrmType == lArrm)break;
			if ((enemy->GetEnemyState() & eLAttack) && m_arrmType == rArrm)break;

			node = m_model->FindNode("AttackPointOne");
			KdCollider::SphereInfo sphereInfo;
			mat = node->m_worldTransform * m_mWorld;
			sphereInfo.m_sphere.Center = mat.Translation();
			sphereInfo.m_sphere.Radius = 0.25f;
			sphereInfo.m_type = KdCollider::TypeGard;

#ifdef _DEBUG
			m_pDebugWire->AddDebugSphere
			(
				sphereInfo.m_sphere.Center,
				sphereInfo.m_sphere.Radius,
				{ 0,0,0,1 }
			);
#endif

			std::list<KdCollider::CollisionResult> retSphereList;

			wepLis->Intersects
			(
				sphereInfo,
				&retSphereList
			);

			Math::Vector3 hitDir = {};
			bool hit = false;
			for (auto& ret : retSphereList)
			{
				if (ret.m_overlapDistance > 0)
				{
					Math::Vector3 nowDir = m_eTarget.lock()->GetMatrix().Backward();
					Math::Vector3 targetDir = m_owner.lock()->GetPos() - m_eTarget.lock()->GetPos();
					float dot = nowDir.Dot(targetDir);
					dot = std::clamp(dot, -1.0f, 1.0f);

					float betweenAng = acos(dot);
					betweenAng = DirectX::XMConvertToDegrees(betweenAng);
					if (betweenAng >= 0 && betweenAng <= 45)
					{
						hit = true;
					}
				}
			}

			if (hit)
			{
				m_eTarget.lock()->SetDefenseSuc(true);
				m_owner.lock()->HasDefense();
				KdAudioManager::Instance().Play("Asset/Audio/SE/grad1.wav");
			}
			else
			{
				node = m_model->FindNode("AttackPointTwo");
				sphereInfo;
				mat = node->m_worldTransform * m_mWorld;
				sphereInfo.m_sphere.Center = mat.Translation();
				sphereInfo.m_sphere.Radius = 0.25f;
				sphereInfo.m_type = KdCollider::TypeGard;

#ifdef _DEBUG
				m_pDebugWire->AddDebugSphere
				(
					sphereInfo.m_sphere.Center,
					sphereInfo.m_sphere.Radius,
					{ 0,0,0,1 }
				);
#endif

				retSphereList.clear();

				wepLis->Intersects
				(
					sphereInfo,
					&retSphereList
				);

				hit = false;
				for (auto& ret : retSphereList)
				{
					if (ret.m_overlapDistance > 0)
					{
						Math::Vector3 nowDir = m_eTarget.lock()->GetMatrix().Backward();
						Math::Vector3 targetDir = m_owner.lock()->GetPos() - m_eTarget.lock()->GetPos();
						float dot = nowDir.Dot(targetDir);
						dot = std::clamp(dot, -1.0f, 1.0f);

						float betweenAng = acos(dot);
						betweenAng = DirectX::XMConvertToDegrees(betweenAng);
						if (betweenAng >= 0 && betweenAng <= 45)
						{
							hit = true;
						}
					}
				}

				if (hit)
				{
					m_eTarget.lock()->SetDefenseSuc(true);
					m_owner.lock()->HasDefense();
					KdAudioManager::Instance().Play("Asset/Audio/SE/grad1.wav");
				}
				else
				{
					node = m_model->FindNode("AttackPointThree");
					sphereInfo;
					mat = node->m_worldTransform * m_mWorld;
					sphereInfo.m_sphere.Center = mat.Translation();
					sphereInfo.m_sphere.Radius = 0.25f;
					sphereInfo.m_type = KdCollider::TypeGard;

#ifdef _DEBUG
					m_pDebugWire->AddDebugSphere
					(
						sphereInfo.m_sphere.Center,
						sphereInfo.m_sphere.Radius,
						{ 0,0,0,1 }
					);
#endif

					retSphereList.clear();

					wepLis->Intersects
					(
						sphereInfo,
						&retSphereList
					);

					hit = false;
					for (auto& ret : retSphereList)
					{
						if (ret.m_overlapDistance > 0)
						{
							Math::Vector3 nowDir = m_eTarget.lock()->GetMatrix().Backward();
							Math::Vector3 targetDir = m_owner.lock()->GetPos() - m_eTarget.lock()->GetPos();
							float dot = nowDir.Dot(targetDir);
							dot = std::clamp(dot, -1.0f, 1.0f);

							float betweenAng = acos(dot);
							betweenAng = DirectX::XMConvertToDegrees(betweenAng);
							if (betweenAng >= 0 && betweenAng <= 45)
							{
								hit = true;
							}
						}
					}

					if (hit)
					{
						m_eTarget.lock()->SetDefenseSuc(true);
						m_owner.lock()->HasDefense();
						KdAudioManager::Instance().Play("Asset/Audio/SE/grad1.wav");
					}
				}
			}
		}
	}
	//}

	if (!m_eTarget.lock()->GetAttackHit() && !m_eTarget.lock()->GetDefenseSuc() && enemy->GetEnemyState() & (eRAttack | eLAttack) && m_eTarget.lock()->GetInvincibilityTimeCnt() == 0)
	{
		if (enemy->GetEnemyState() & eRAttack && m_arrmType == lArrm)return;
		if (enemy->GetEnemyState() & eLAttack && m_arrmType == rArrm)return;
		node = m_model->FindNode("AttackPointOne");
		KdCollider::SphereInfo sphereInfo;
		mat = node->m_worldTransform * m_mWorld;
		sphereInfo.m_sphere.Center = mat.Translation();
		sphereInfo.m_sphere.Radius = 0.20f;
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
		m_eTarget.lock()->Intersects
		(
			sphereInfo,
			&retSphereList
		);

		Math::Vector3 hitDir = {};
		bool hit = false;
		for (auto& ret : retSphereList)
		{
			hit = true;
			hitDir = ret.m_hitDir;
		}

		if (hit)
		{
			if(enemy->GetEnemyState() & (eRAttackOne | eLAttackOne) && enemy->GetEnemyState() & (eGrassHopperDashF))
			{
				m_eTarget.lock()->IaiKiriAttackOnHit(enemy->GetMatrix().Backward());
			}
			else if (enemy->GetEnemyState() & (eRAttackOne | eRAttackTwo |
				                          eLAttackOne | eRAttackTwo))
			{
				m_eTarget.lock()->OnHit(enemy->GetMatrix().Backward());
			}
			else if(enemy->GetEnemyState() & (eRAttackThree | eLAttackThree))
			{
				m_eTarget.lock()->BlowingAwayAttackOnHit(enemy->GetMatrix().Backward());
			}
			KdAudioManager::Instance().Play("Asset/Audio/SE/AttackHitOverlapping.wav");
		}
		else
		{
			node = m_model->FindNode("AttackPointTwo");
			sphereInfo;
			mat = node->m_worldTransform * m_mWorld;
			sphereInfo.m_sphere.Center = mat.Translation();
			sphereInfo.m_sphere.Radius = 0.20f;
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

			m_eTarget.lock()->Intersects
			(
				sphereInfo,
				&retSphereList
			);

			hit = false;
			for (auto& ret : retSphereList)
			{
				hit = true;
				hitDir = ret.m_hitDir;
			}

			if (hit)
			{
				if (enemy->GetEnemyState() & (eRAttackOne | eLAttackOne) && enemy->GetEnemyState() & (eGrassHopperDashF))
				{
					m_eTarget.lock()->IaiKiriAttackOnHit(enemy->GetMatrix().Backward());
				}
				else if (enemy->GetEnemyState() & (eRAttackOne | eRAttackTwo |
					eLAttackOne | eRAttackTwo))
				{
					m_eTarget.lock()->OnHit(enemy->GetMatrix().Backward());
				}
				else if (enemy->GetEnemyState() & (eRAttackThree | eLAttackThree))
				{
					m_eTarget.lock()->BlowingAwayAttackOnHit(enemy->GetMatrix().Backward());
				}
				KdAudioManager::Instance().Play("Asset/Audio/SE/AttackHitOverlapping.wav");
			}
			else
			{
				node = m_model->FindNode("AttackPointThree");
				sphereInfo;
				mat = node->m_worldTransform * m_mWorld;
				sphereInfo.m_sphere.Center = mat.Translation();
				sphereInfo.m_sphere.Radius = 0.20f;
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

				m_eTarget.lock()->Intersects
				(
					sphereInfo,
					&retSphereList
				);

				hit = false;
				for (auto& ret : retSphereList)
				{
					hit = true;
					hitDir = ret.m_hitDir;
				}

				if (hit)
				{
					if (enemy->GetEnemyState() & (eRAttackOne | eLAttackOne) && enemy->GetEnemyState() & (eGrassHopperDashF))
					{
						m_eTarget.lock()->IaiKiriAttackOnHit(enemy->GetMatrix().Backward());
					}
					else if (enemy->GetEnemyState() & (eRAttackOne | eRAttackTwo |
						eLAttackOne | eRAttackTwo))
					{
						m_eTarget.lock()->OnHit(enemy->GetMatrix().Backward());
					}
					else if (enemy->GetEnemyState() & (eRAttackThree | eLAttackThree))
					{
						m_eTarget.lock()->BlowingAwayAttackOnHit(enemy->GetMatrix().Backward());
					}
					KdAudioManager::Instance().Play("Asset/Audio/SE/AttackHitOverlapping.wav");
				}
			}
		}
		//}
	}
}

void Scopion::EnemyManAttackChaeck()
{
	const std::shared_ptr<Enemy> enemy = std::dynamic_pointer_cast<Enemy>(m_owner.lock());
	if (!(enemy->GetWeaponType() & (scorpion | lScorpion)))return;

	const KdModelWork::Node* node = nullptr;
	Math::Matrix mat = Math::Matrix::Identity;

	/*for (auto& obj : m_pTargetList)
{*/
// çUåÇÇÃìñÇΩÇËîªíË
//	for (auto& wepLis : m_eTarget.lock()->GetWeaponList())
//	{
//		if (m_eTarget.lock()->GetPlayerState() & Player::PlayerState::defense && !m_eTarget.lock()->GetAttackHit() && !m_eTarget.lock()->GetDefenseSuc() && enemy->GetEnemyState() & eMantis)
//		{
//			if ((enemy->GetEnemyState() & eRAttack) && m_arrmType == lArrm)break;
//			if ((enemy->GetEnemyState() & eLAttack) && m_arrmType == rArrm)break;
//
//			node = m_manModel->FindNode("AttackPointOne");
//			KdCollider::SphereInfo sphereInfo;
//			mat = node->m_worldTransform * m_mWorld;
//			sphereInfo.m_sphere.Center = mat.Translation();
//			sphereInfo.m_sphere.Radius = 0.25f;
//			sphereInfo.m_type = KdCollider::TypeGard;
//
//#ifdef _DEBUG
//			m_pDebugWire->AddDebugSphere
//			(
//				sphereInfo.m_sphere.Center,
//				sphereInfo.m_sphere.Radius,
//				{ 0,0,0,1 }
//			);
//#endif
//
//			std::list<KdCollider::CollisionResult> retSphereList;
//
//			wepLis->Intersects
//			(
//				sphereInfo,
//				&retSphereList
//			);
//
//			Math::Vector3 hitDir = {};
//			bool hit = false;
//			for (auto& ret : retSphereList)
//			{
//				if (ret.m_overlapDistance > 0)
//				{
//					Math::Vector3 nowDir = m_eTarget.lock()->GetMatrix().Backward();
//					Math::Vector3 targetDir = m_owner.lock()->GetPos() - m_eTarget.lock()->GetPos();
//					float dot = nowDir.Dot(targetDir);
//					dot = std::clamp(dot, -1.0f, 1.0f);
//
//					float betweenAng = acos(dot);
//					betweenAng = DirectX::XMConvertToDegrees(betweenAng);
//					if (betweenAng >= 0 && betweenAng <= 45)
//					{
//						hit = true;
//					}
//				}
//			}
//
//			if (hit)
//			{
//				m_eTarget.lock()->SetDefenseSuc(true);
//				m_owner.lock()->HasDefense();
//				KdAudioManager::Instance().Play("Asset/Audio/SE/grad1.wav");
//			}
//			else
//			{
//				node = m_manModel->FindNode("AttackPointTwo");
//				sphereInfo;
//				mat = node->m_worldTransform * m_mWorld;
//				sphereInfo.m_sphere.Center = mat.Translation();
//				sphereInfo.m_sphere.Radius = 0.25f;
//				sphereInfo.m_type = KdCollider::TypeGard;
//
//#ifdef _DEBUG
//				m_pDebugWire->AddDebugSphere
//				(
//					sphereInfo.m_sphere.Center,
//					sphereInfo.m_sphere.Radius,
//					{ 0,0,0,1 }
//				);
//#endif
//
//				retSphereList.clear();
//
//				wepLis->Intersects
//				(
//					sphereInfo,
//					&retSphereList
//				);
//
//				hit = false;
//				for (auto& ret : retSphereList)
//				{
//					if (ret.m_overlapDistance > 0)
//					{
//						Math::Vector3 nowDir = m_eTarget.lock()->GetMatrix().Backward();
//						Math::Vector3 targetDir = m_owner.lock()->GetPos() - m_eTarget.lock()->GetPos();
//						float dot = nowDir.Dot(targetDir);
//						dot = std::clamp(dot, -1.0f, 1.0f);
//
//						float betweenAng = acos(dot);
//						betweenAng = DirectX::XMConvertToDegrees(betweenAng);
//						if (betweenAng >= 0 && betweenAng <= 45)
//						{
//							hit = true;
//						}
//					}
//				}
//
//				if (hit)
//				{
//					m_eTarget.lock()->SetDefenseSuc(true);
//					m_owner.lock()->HasDefense();
//					KdAudioManager::Instance().Play("Asset/Audio/SE/grad1.wav");
//				}
//				else
//				{
//					node = m_manModel->FindNode("AttackPointThree");
//					sphereInfo;
//					mat = node->m_worldTransform * m_mWorld;
//					sphereInfo.m_sphere.Center = mat.Translation();
//					sphereInfo.m_sphere.Radius = 0.25f;
//					sphereInfo.m_type = KdCollider::TypeGard;
//
//#ifdef _DEBUG
//					m_pDebugWire->AddDebugSphere
//					(
//						sphereInfo.m_sphere.Center,
//						sphereInfo.m_sphere.Radius,
//						{ 0,0,0,1 }
//					);
//#endif
//
//					retSphereList.clear();
//
//					wepLis->Intersects
//					(
//						sphereInfo,
//						&retSphereList
//					);
//
//					hit = false;
//					for (auto& ret : retSphereList)
//					{
//						if (ret.m_overlapDistance > 0)
//						{
//							Math::Vector3 nowDir = m_eTarget.lock()->GetMatrix().Backward();
//							Math::Vector3 targetDir = m_owner.lock()->GetPos() - m_eTarget.lock()->GetPos();
//							float dot = nowDir.Dot(targetDir);
//							dot = std::clamp(dot, -1.0f, 1.0f);
//
//							float betweenAng = acos(dot);
//							betweenAng = DirectX::XMConvertToDegrees(betweenAng);
//							if (betweenAng >= 0 && betweenAng <= 45)
//							{
//								hit = true;
//							}
//						}
//					}
//
//					if (hit)
//					{
//						m_eTarget.lock()->SetDefenseSuc(true);
//						m_owner.lock()->HasDefense();
//						KdAudioManager::Instance().Play("Asset/Audio/SE/grad1.wav");
//					}
//					else
//					{
//						node = m_manModel->FindNode("AttackPointFour");
//						sphereInfo;
//						mat = node->m_worldTransform * m_mWorld;
//						sphereInfo.m_sphere.Center = mat.Translation();
//						sphereInfo.m_sphere.Radius = 0.25f;
//						sphereInfo.m_type = KdCollider::TypeGard;
//
//#ifdef _DEBUG
//						m_pDebugWire->AddDebugSphere
//						(
//							sphereInfo.m_sphere.Center,
//							sphereInfo.m_sphere.Radius,
//							{ 0,0,0,1 }
//						);
//#endif
//
//						retSphereList.clear();
//
//						wepLis->Intersects
//						(
//							sphereInfo,
//							&retSphereList
//						);
//
//						hit = false;
//						for (auto& ret : retSphereList)
//						{
//							if (ret.m_overlapDistance > 0)
//							{
//								Math::Vector3 nowDir = m_eTarget.lock()->GetMatrix().Backward();
//								Math::Vector3 targetDir = m_owner.lock()->GetPos() - m_eTarget.lock()->GetPos();
//								float dot = nowDir.Dot(targetDir);
//								dot = std::clamp(dot, -1.0f, 1.0f);
//
//								float betweenAng = acos(dot);
//								betweenAng = DirectX::XMConvertToDegrees(betweenAng);
//								if (betweenAng >= 0 && betweenAng <= 45)
//								{
//									hit = true;
//								}
//							}
//						}
//
//						if (hit)
//						{
//							m_eTarget.lock()->SetDefenseSuc(true);
//							m_owner.lock()->HasDefense();
//							KdAudioManager::Instance().Play("Asset/Audio/SE/grad1.wav");
//						}
//						else
//						{
//							node = m_manModel->FindNode("AttackPointFive");
//							sphereInfo;
//							mat = node->m_worldTransform * m_mWorld;
//							sphereInfo.m_sphere.Center = mat.Translation();
//							sphereInfo.m_sphere.Radius = 0.25f;
//							sphereInfo.m_type = KdCollider::TypeGard;
//
//#ifdef _DEBUG
//							m_pDebugWire->AddDebugSphere
//							(
//								sphereInfo.m_sphere.Center,
//								sphereInfo.m_sphere.Radius,
//								{ 0,0,0,1 }
//							);
//#endif
//
//							retSphereList.clear();
//
//							wepLis->Intersects
//							(
//								sphereInfo,
//								&retSphereList
//							);
//
//							hit = false;
//							for (auto& ret : retSphereList)
//							{
//								if (ret.m_overlapDistance > 0)
//								{
//									Math::Vector3 nowDir = m_eTarget.lock()->GetMatrix().Backward();
//									Math::Vector3 targetDir = m_owner.lock()->GetPos() - m_eTarget.lock()->GetPos();
//									float dot = nowDir.Dot(targetDir);
//									dot = std::clamp(dot, -1.0f, 1.0f);
//
//									float betweenAng = acos(dot);
//									betweenAng = DirectX::XMConvertToDegrees(betweenAng);
//									if (betweenAng >= 0 && betweenAng <= 45)
//									{
//										hit = true;
//									}
//								}
//							}
//
//							if (hit)
//							{
//								m_eTarget.lock()->SetDefenseSuc(true);
//								m_owner.lock()->HasDefense();
//								KdAudioManager::Instance().Play("Asset/Audio/SE/grad1.wav");
//							}
//							else
//							{
//								node = m_manModel->FindNode("AttackPointSix");
//								sphereInfo;
//								mat = node->m_worldTransform * m_mWorld;
//								sphereInfo.m_sphere.Center = mat.Translation();
//								sphereInfo.m_sphere.Radius = 0.25f;
//								sphereInfo.m_type = KdCollider::TypeGard;
//
//#ifdef _DEBUG
//								m_pDebugWire->AddDebugSphere
//								(
//									sphereInfo.m_sphere.Center,
//									sphereInfo.m_sphere.Radius,
//									{ 0,0,0,1 }
//								);
//#endif
//
//								retSphereList.clear();
//
//								wepLis->Intersects
//								(
//									sphereInfo,
//									&retSphereList
//								);
//
//								hit = false;
//								for (auto& ret : retSphereList)
//								{
//									if (ret.m_overlapDistance > 0)
//									{
//										Math::Vector3 nowDir = m_eTarget.lock()->GetMatrix().Backward();
//										Math::Vector3 targetDir = m_owner.lock()->GetPos() - m_eTarget.lock()->GetPos();
//										float dot = nowDir.Dot(targetDir);
//										dot = std::clamp(dot, -1.0f, 1.0f);
//
//										float betweenAng = acos(dot);
//										betweenAng = DirectX::XMConvertToDegrees(betweenAng);
//										if (betweenAng >= 0 && betweenAng <= 45)
//										{
//											hit = true;
//										}
//									}
//								}
//
//								if (hit)
//								{
//									m_eTarget.lock()->SetDefenseSuc(true);
//									m_owner.lock()->HasDefense();
//									KdAudioManager::Instance().Play("Asset/Audio/SE/grad1.wav");
//								}
//								else
//								{
//									node = m_manModel->FindNode("AttackPointSeven");
//									sphereInfo;
//									mat = node->m_worldTransform * m_mWorld;
//									sphereInfo.m_sphere.Center = mat.Translation();
//									sphereInfo.m_sphere.Radius = 0.25f;
//									sphereInfo.m_type = KdCollider::TypeGard;
//
//#ifdef _DEBUG
//									m_pDebugWire->AddDebugSphere
//									(
//										sphereInfo.m_sphere.Center,
//										sphereInfo.m_sphere.Radius,
//										{ 0,0,0,1 }
//									);
//#endif
//
//									retSphereList.clear();
//
//									wepLis->Intersects
//									(
//										sphereInfo,
//										&retSphereList
//									);
//
//									hit = false;
//									for (auto& ret : retSphereList)
//									{
//										if (ret.m_overlapDistance > 0)
//										{
//											Math::Vector3 nowDir = m_eTarget.lock()->GetMatrix().Backward();
//											Math::Vector3 targetDir = m_owner.lock()->GetPos() - m_eTarget.lock()->GetPos();
//											float dot = nowDir.Dot(targetDir);
//											dot = std::clamp(dot, -1.0f, 1.0f);
//
//											float betweenAng = acos(dot);
//											betweenAng = DirectX::XMConvertToDegrees(betweenAng);
//											if (betweenAng >= 0 && betweenAng <= 45)
//											{
//												hit = true;
//											}
//										}
//									}
//
//									if (hit)
//									{
//										m_eTarget.lock()->SetDefenseSuc(true);
//										m_owner.lock()->HasDefense();
//										KdAudioManager::Instance().Play("Asset/Audio/SE/grad1.wav");
//									}
//								}
//							}
//						}
//					}
//				}
//			}
//		}
//	}
//	//}

	if (!m_eTarget.lock()->GetAttackHit() && !m_eTarget.lock()->GetDefenseSuc() && enemy->GetEnemyState() & eMantis && m_eTarget.lock()->GetInvincibilityTimeCnt() == 0)
	{
		if (enemy->GetEnemyState() & eRAttack && m_arrmType == lArrm)return;
		if (enemy->GetEnemyState() & eLAttack && m_arrmType == rArrm)return;
		node = m_manModel->FindNode("AttackPointOne");
		KdCollider::SphereInfo sphereInfo;
		mat = node->m_worldTransform * m_mWorld;
		sphereInfo.m_sphere.Center = mat.Translation();
		sphereInfo.m_sphere.Radius = 0.20f;
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
		m_eTarget.lock()->Intersects
		(
			sphereInfo,
			&retSphereList
		);

		Math::Vector3 hitDir = {};
		bool hit = false;
		for (auto& ret : retSphereList)
		{
			hit = true;
			hitDir = ret.m_hitDir;
		}

		if (hit)
		{
			m_eTarget.lock()->OnHit(enemy->GetMatrix().Backward());
			KdAudioManager::Instance().Play("Asset/Audio/SE/AttackHitOverlapping.wav");
		}
		else
		{
			node = m_manModel->FindNode("AttackPointTwo");
			sphereInfo;
			mat = node->m_worldTransform * m_mWorld;
			sphereInfo.m_sphere.Center = mat.Translation();
			sphereInfo.m_sphere.Radius = 0.20f;
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

			m_eTarget.lock()->Intersects
			(
				sphereInfo,
				&retSphereList
			);

			hit = false;
			for (auto& ret : retSphereList)
			{
				hit = true;
				hitDir = ret.m_hitDir;
			}

			if (hit)
			{
				m_eTarget.lock()->OnHit(enemy->GetMatrix().Backward());
				KdAudioManager::Instance().Play("Asset/Audio/SE/AttackHitOverlapping.wav");
			}
			else
			{
				node = m_manModel->FindNode("AttackPointThree");
				sphereInfo;
				mat = node->m_worldTransform * m_mWorld;
				sphereInfo.m_sphere.Center = mat.Translation();
				sphereInfo.m_sphere.Radius = 0.20f;
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

				m_eTarget.lock()->Intersects
				(
					sphereInfo,
					&retSphereList
				);

				hit = false;
				for (auto& ret : retSphereList)
				{
					hit = true;
					hitDir = ret.m_hitDir;
				}

				if (hit)
				{
					m_eTarget.lock()->OnHit(enemy->GetMatrix().Backward());
					KdAudioManager::Instance().Play("Asset/Audio/SE/AttackHitOverlapping.wav");
				}
				else
				{
					node = m_manModel->FindNode("AttackPointFour");
					sphereInfo;
					mat = node->m_worldTransform * m_mWorld;
					sphereInfo.m_sphere.Center = mat.Translation();
					sphereInfo.m_sphere.Radius = 0.20f;
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

					m_eTarget.lock()->Intersects
					(
						sphereInfo,
						&retSphereList
					);

					hit = false;
					for (auto& ret : retSphereList)
					{
						hit = true;
						hitDir = ret.m_hitDir;
					}

					if (hit)
					{
						m_eTarget.lock()->OnHit(enemy->GetMatrix().Backward());
						KdAudioManager::Instance().Play("Asset/Audio/SE/AttackHitOverlapping.wav");
					}
					else
					{
						node = m_manModel->FindNode("AttackPointFive");
						sphereInfo;
						mat = node->m_worldTransform * m_mWorld;
						sphereInfo.m_sphere.Center = mat.Translation();
						sphereInfo.m_sphere.Radius = 0.20f;
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

						m_eTarget.lock()->Intersects
						(
							sphereInfo,
							&retSphereList
						);

						hit = false;
						for (auto& ret : retSphereList)
						{
							hit = true;
							hitDir = ret.m_hitDir;
						}

						if (hit)
						{
							m_eTarget.lock()->OnHit(enemy->GetMatrix().Backward());
							KdAudioManager::Instance().Play("Asset/Audio/SE/AttackHitOverlapping.wav");
						}
						else
						{
							node = m_manModel->FindNode("AttackPointSix");
							sphereInfo;
							mat = node->m_worldTransform * m_mWorld;
							sphereInfo.m_sphere.Center = mat.Translation();
							sphereInfo.m_sphere.Radius = 0.20f;
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

							m_eTarget.lock()->Intersects
							(
								sphereInfo,
								&retSphereList
							);

							hit = false;
							for (auto& ret : retSphereList)
							{
								hit = true;
								hitDir = ret.m_hitDir;
							}

							if (hit)
							{
								m_eTarget.lock()->OnHit(enemy->GetMatrix().Backward());
								KdAudioManager::Instance().Play("Asset/Audio/SE/AttackHitOverlapping.wav");
							}
							else
							{
								node = m_manModel->FindNode("AttackPointSeven");
								sphereInfo;
								mat = node->m_worldTransform * m_mWorld;
								sphereInfo.m_sphere.Center = mat.Translation();
								sphereInfo.m_sphere.Radius = 0.20f;
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

								m_eTarget.lock()->Intersects
								(
									sphereInfo,
									&retSphereList
								);

								hit = false;
								for (auto& ret : retSphereList)
								{
									hit = true;
									hitDir = ret.m_hitDir;
								}

								if (hit)
								{
									m_eTarget.lock()->OnHit(enemy->GetMatrix().Backward());
									KdAudioManager::Instance().Play("Asset/Audio/SE/AttackHitOverlapping.wav");
								}
							}
						}
					}
				}
			}
		}
		//}
	}
}
