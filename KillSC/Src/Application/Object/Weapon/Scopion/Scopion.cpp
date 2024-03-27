#include "Scopion.h"
#include "../../../Scene/SceneManager.h"
#include "../../Character/Player/Player.h"
#include "../../Character/Enemy/Enemy.h"

void Scopion::Update()
{
	if (m_bPlayerWeapon)
	{
		if (m_bMantis && m_arrmType == lArrm)return;

		const std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(m_owner.lock());
		if (!(player->GetWeaponType() & (Player::WeaponType::scorpion | Player::WeaponType::lScorpion)))return;

		const KdModelWork::Node* node = nullptr;

		if (m_arrmType == rArrm && player->GetWeaponType() & Player::WeaponType::scorpion)
		{
			node = player->GetModel()->FindNode("WeaponRPoint");
		}

		if (m_arrmType == lArrm && player->GetWeaponType() & Player::WeaponType::lScorpion)
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
				m_mWorld = node->m_worldTransform * player->GetMatrix();
				m_mWorld._42 += 0.7f;
			}
		}

#ifdef _DEBUG
		// ガードの当たり判定を可視化
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

		if (m_bMantis)
		{
			PlayerManAttackChaeck();
		}
		else if (!(player->GetCharaState() & CharacterBase::rlAttackRush && player->GetAnimationCnt() >= 107))
		{
			PlayerHitAttackChaeck();
		}
	}

	if (m_bEnemyWeapon)
	{
		const std::shared_ptr<Enemy> enemy = std::dynamic_pointer_cast<Enemy>(m_owner.lock());
		if (!(enemy->GetWeaponType() & (Enemy::WeaponType::scorpion | Enemy::WeaponType::lScorpion)))return;

		const KdModelWork::Node* node = nullptr;

		if (m_arrmType == rArrm && enemy->GetWeaponType() & Enemy::WeaponType::scorpion)
		{
			node = enemy->GetModel()->FindNode("WeaponRPoint");
		}

		if (m_arrmType == lArrm && enemy->GetWeaponType() & Enemy::WeaponType::lScorpion)
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
				node = enemy->GetModel()->FindNode("MantisPoint");
				m_mWorld = node->m_worldTransform * enemy->GetMatrix();
				m_mWorld._42 += 0.7f;
			}
		}

#ifdef _DEBUG
		// ガードの当たり判定を可視化
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

		if (m_bMantis)
		{
			EnemyManAttackChaeck();

			m_animator->AdvanceTime(m_manModel->WorkNodes());
			if (m_animator->IsAnimationEnd())
			{
				m_bMantis = false;
			}
		}
		else if (!(enemy->GetCharaState() & CharacterBase::rlAttackRush && enemy->GetAnimationCnt() >= 107))
		{
			EnemyHitAttackChaeck();
		}
	}
}

void Scopion::PostUpdate()
{
	auto it = m_pTargetList.begin();
	while (it != m_pTargetList.end()) // 数が変動するため範囲ベースForが使えない
	{
		// 不要になったオブジェクトを消す
		if ((*it).expired())
		{
			// 消す
			it = m_pTargetList.erase(it); // 戻り値で次の場所を返してくれる
		}
		else
		{
			++it; // 次へ
		}
	}
}

void Scopion::DrawLit()
{
	if (m_bPlayerWeapon)
	{
		if (!m_bMantis)
		{
			const std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(m_owner.lock());
			if (!(player->GetWeaponType() & (Player::WeaponType::scorpion | Player::WeaponType::lScorpion)))return;

			if (m_arrmType == rArrm && player->GetWeaponType() & Player::WeaponType::scorpion)
			{
				KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_model, m_mWorld);
			}

			if (m_arrmType == lArrm && player->GetWeaponType() & Player::WeaponType::lScorpion)
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
			if (!(enemy->GetWeaponType() & (Enemy::WeaponType::scorpion | Enemy::WeaponType::lScorpion)))return;

			if (m_arrmType == rArrm && enemy->GetWeaponType() & Enemy::WeaponType::scorpion)
			{
				KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_model, m_mWorld);
			}

			if (m_arrmType == lArrm && enemy->GetWeaponType() & Enemy::WeaponType::lScorpion)
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
			if (!(player->GetWeaponType() & (Player::WeaponType::scorpion | Player::WeaponType::lScorpion)))return;

			if (m_arrmType == rArrm && player->GetWeaponType() & Player::WeaponType::scorpion)
			{
				KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_model, m_mWorld);
			}

			if (m_arrmType == lArrm && player->GetWeaponType() & Player::WeaponType::lScorpion)
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
			if (!(enemy->GetWeaponType() & (Enemy::WeaponType::scorpion | Enemy::WeaponType::lScorpion)))return;

			if (m_arrmType == rArrm && enemy->GetWeaponType() & Enemy::WeaponType::scorpion)
			{
				KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_model, m_mWorld);
			}

			if (m_arrmType == lArrm && enemy->GetWeaponType() & Enemy::WeaponType::lScorpion)
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
	// 当たり判定初期化
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
	const KdModelWork::Node* node = nullptr;
	Math::Matrix mat = Math::Matrix::Identity;

	if ((player->GetCharaState() & CharacterBase::rAttack) && m_arrmType == lArrm || m_arrmType == rArrm && !(player->GetWeaponType() & Player::WeaponType::scorpion))return;
	if ((player->GetCharaState() & CharacterBase::lAttack) && m_arrmType == rArrm || m_arrmType == lArrm && !(player->GetWeaponType() & Player::WeaponType::lScorpion))return;

	for (auto& pTarget : m_pTargetList)
	{
		if (pTarget.expired())continue;

		if (pTarget.lock()->IsExpired())continue;
		// 攻撃の当たり判定
		for (auto& wepLis : pTarget.lock()->GetWeaponList())
		{
			// 武器との当たり判定＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
			if (pTarget.lock()->GetCharaState() & CharacterBase::defense && !pTarget.lock()->GetAttackHit() && !pTarget.lock()->GetBDeath() && !pTarget.lock()->GetDefenseSuc() && player->GetCharaState() & (CharacterBase::rAttack | CharacterBase::lAttack | CharacterBase::rlAttack | CharacterBase::rlAttackRush))
			{
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
						Math::Vector3 nowDir = pTarget.lock()->GetMatrix().Backward();
						Math::Vector3 targetDir = m_owner.lock()->GetPos() - pTarget.lock()->GetPos();
						float betweenAng = DotProductCalculation(nowDir, targetDir);
						betweenAng = DirectX::XMConvertToDegrees(betweenAng);
						if (betweenAng >= 0 && betweenAng <= 45)
						{
							hit = true;
						}
					}
				}

				if (hit)
				{
					pTarget.lock()->SetDefenseSuc(true);
					player->HasDefense();
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
							Math::Vector3 nowDir = pTarget.lock()->GetMatrix().Backward();
							Math::Vector3 targetDir = m_owner.lock()->GetPos() - pTarget.lock()->GetPos();
							float betweenAng = DotProductCalculation(nowDir, targetDir);
							betweenAng = DirectX::XMConvertToDegrees(betweenAng);
							if (betweenAng >= 0 && betweenAng <= 45)
							{
								hit = true;
							}
						}
					}

					if (hit)
					{
						pTarget.lock()->SetDefenseSuc(true);
						player->HasDefense();
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
								Math::Vector3 nowDir = pTarget.lock()->GetMatrix().Backward();
								Math::Vector3 targetDir = m_owner.lock()->GetPos() - pTarget.lock()->GetPos();
								float betweenAng = DotProductCalculation(nowDir, targetDir);
								betweenAng = DirectX::XMConvertToDegrees(betweenAng);
								if (betweenAng >= 0 && betweenAng <= 45)
								{
									hit = true;
								}
							}
						}

						if (hit)
						{
							pTarget.lock()->SetDefenseSuc(true);
							player->HasDefense();
							KdAudioManager::Instance().Play("Asset/Audio/SE/grad1.wav");
						}
					}
				}
			}
		}

		// 敵との当たり判定＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
		if (!pTarget.lock()->GetAttackHit() && !pTarget.lock()->GetDefenseSuc() && player->GetCharaState() & (CharacterBase::rAttack | CharacterBase::lAttack | CharacterBase::rlAttack | CharacterBase::rlAttackRush) && pTarget.lock()->GetInvincibilityTimeCnt() == 0 && !pTarget.lock()->GetBDeath())
		{
			if (player->GetCharaState() & CharacterBase::rAttack && m_arrmType == lArrm)return;
			if (player->GetCharaState() & CharacterBase::lAttack && m_arrmType == rArrm)return;
			node = m_model->FindNode("AttackPointOne");
			KdCollider::SphereInfo sphereInfo;
			mat = node->m_worldTransform * m_mWorld;
			sphereInfo.m_sphere.Center = mat.Translation();
			sphereInfo.m_sphere.Radius = 0.20f;
			sphereInfo.m_type = KdCollider::TypeWeakness;

#ifdef _DEBUG
			m_pDebugWire->AddDebugSphere
			(
				sphereInfo.m_sphere.Center,
				sphereInfo.m_sphere.Radius,
				{ 0,0,0,1 }
			);
#endif

			std::list<KdCollider::CollisionResult> retSphereList;

			pTarget.lock()->Intersects
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
				KdAudioManager::Instance().Play("Asset/Audio/SE/WeaknessHit2.wav");

				pTarget.lock()->WeaknessOnHit();

				KdEffekseerManager::GetInstance().
					Play("SwordHit.efk", hitPos);
				KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("SwordHit.efk"); // これでループしない
				Math::Matrix efcMat = PlayerHitEffectMat(hitPos, pTarget);
				KdEffekseerManager::GetInstance().SetWorldMatrix("SwordHit.efk", efcMat);
			}
			else
			{
				node = m_model->FindNode("AttackPointTwo");
				sphereInfo;
				mat = node->m_worldTransform * m_mWorld;
				sphereInfo.m_sphere.Center = mat.Translation();
				sphereInfo.m_sphere.Radius = 0.20f;
				sphereInfo.m_type = KdCollider::TypeWeakness;

#ifdef _DEBUG
				m_pDebugWire->AddDebugSphere
				(
					sphereInfo.m_sphere.Center,
					sphereInfo.m_sphere.Radius,
					{ 0,0,0,1 }
				);
#endif

				retSphereList.clear();

				pTarget.lock()->Intersects
				(
					sphereInfo,
					&retSphereList
				);

				hit = false;
				for (auto& ret : retSphereList)
				{
					hit = true;
					hitDir = ret.m_hitDir;
					hitPos = ret.m_hitPos;
				}

				if (hit)
				{
					KdAudioManager::Instance().Play("Asset/Audio/SE/WeaknessHit2.wav");

					pTarget.lock()->WeaknessOnHit();

					KdEffekseerManager::GetInstance().
						Play("SwordHit.efk", hitPos);
					KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("SwordHit.efk"); // これでループしない
					Math::Matrix efcMat = PlayerHitEffectMat(hitPos, pTarget);
					KdEffekseerManager::GetInstance().SetWorldMatrix("SwordHit.efk", efcMat);
				}
				else
				{
					node = m_model->FindNode("AttackPointThree");
					sphereInfo;
					mat = node->m_worldTransform * m_mWorld;
					sphereInfo.m_sphere.Center = mat.Translation();
					sphereInfo.m_sphere.Radius = 0.20f;
					sphereInfo.m_type = KdCollider::TypeWeakness;

#ifdef _DEBUG
					m_pDebugWire->AddDebugSphere
					(
						sphereInfo.m_sphere.Center,
						sphereInfo.m_sphere.Radius,
						{ 0,0,0,1 }
					);
#endif

					retSphereList.clear();

					pTarget.lock()->Intersects
					(
						sphereInfo,
						&retSphereList
					);

					hit = false;
					for (auto& ret : retSphereList)
					{
						hit = true;
						hitDir = ret.m_hitDir;
						hitPos = ret.m_hitPos;
					}

					if (hit)
					{
						KdAudioManager::Instance().Play("Asset/Audio/SE/WeaknessHit2.wav");

						pTarget.lock()->WeaknessOnHit();

						KdEffekseerManager::GetInstance().
							Play("SwordHit.efk", hitPos);
						KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("SwordHit.efk"); // これでループしない
						Math::Matrix efcMat = PlayerHitEffectMat(hitPos, pTarget);
						KdEffekseerManager::GetInstance().SetWorldMatrix("SwordHit.efk", efcMat);
					}
					else
					{
						node = m_model->FindNode("AttackPointOne");
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

						retSphereList;

						pTarget.lock()->Intersects
						(
							sphereInfo,
							&retSphereList
						);

						hitDir = {};
						hit = false;
						hitPos;
						for (auto& ret : retSphereList)
						{
							hit = true;
							hitDir = ret.m_hitDir;
							hitPos = ret.m_hitPos;
						}

						if (hit)
						{
							PlayerAttackHit(player, pTarget.lock(), hitPos);
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

							pTarget.lock()->Intersects
							(
								sphereInfo,
								&retSphereList
							);

							hit = false;
							for (auto& ret : retSphereList)
							{
								hit = true;
								hitDir = ret.m_hitDir;
								hitPos = ret.m_hitPos;
							}

							if (hit)
							{
								PlayerAttackHit(player, pTarget.lock(), hitPos);
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

								pTarget.lock()->Intersects
								(
									sphereInfo,
									&retSphereList
								);

								hit = false;
								for (auto& ret : retSphereList)
								{
									hit = true;
									hitDir = ret.m_hitDir;
									hitPos = ret.m_hitPos;
								}

								if (hit)
								{
									PlayerAttackHit(player, pTarget.lock(), hitPos);
								}
							}
						}
					}
				}
			}
		}
	}
}

void Scopion::PlayerAttackHit(std::shared_ptr<Player> a_player, std::shared_ptr<Enemy> a_enemy, Math::Vector3 a_hitPos)
{
	if (a_player->GetCharaState() & (CharacterBase::rAttackOne | CharacterBase::lAttackOne) && a_player->GetCharaState() & (CharacterBase::grassHopperDashF))
	{
		a_enemy->BlowingAwayAttackOnHit(a_player->GetMatrix().Backward(), a_player.get());
		KdAudioManager::Instance().Play("Asset/Audio/SE/AttackHitOverlapping.wav");
	}
	else if (a_player->GetCharaState() & (CharacterBase::lAttackThree | CharacterBase::rAttackThree))
	{
		a_enemy->BlowingAwayAttackOnHit(a_player->GetMatrix().Backward(), a_player.get());
		KdAudioManager::Instance().Play("Asset/Audio/SE/AttackHitOverlapping.wav");
	}
	else if (a_player->GetCharaState() & (CharacterBase::rAttackOne | CharacterBase::rAttackTwo |
		CharacterBase::lAttackOne | CharacterBase::lAttackTwo |
		CharacterBase::rlAttack))
	{
		if (a_player->GetCharaState() & CharacterBase::rlAttackThree)
		{
			a_player->SetBRushAttackPossible(true);
		}

		a_enemy->OnHit(a_player->GetMatrix().Backward(), a_player.get());

		KdAudioManager::Instance().Play("Asset/Audio/SE/AttackHitOverlapping.wav");
	}

	if (!a_player->GetBRushRp())
	{
		if (a_player->GetCharaState() & CharacterBase::rlAttackRush && a_player->GetAnimationCnt() < 42)
		{
			a_enemy->OnHit(a_player->GetMatrix().Backward(), a_player.get());
			KdAudioManager::Instance().Play("Asset/Audio/SE/AttackHitOverlapping.wav");
		}
		else if (a_player->GetCharaState() & CharacterBase::rlAttackRush &&
			(a_player->GetAnimationCnt() >= 49 && a_player->GetAnimationCnt() < 57) ||
			(a_player->GetAnimationCnt() >= 58 && a_player->GetAnimationCnt() < 68) ||
			(a_player->GetAnimationCnt() >= 74 && a_player->GetAnimationCnt() < 89) ||
			(a_player->GetAnimationCnt() >= 89 && a_player->GetAnimationCnt() < 107))
		{
			a_enemy->CutRaiseOnHit(a_player->GetMatrix().Backward());
			KdAudioManager::Instance().Play("Asset/Audio/SE/AttackHitOverlapping.wav");
		}
	}
	else
	{
		if (a_player->GetCharaState() & CharacterBase::rlAttackRush && a_player->GetAnimationCnt() < 115)
		{
			a_enemy->OnHit(a_player->GetMatrix().Backward(), a_player.get());
			KdAudioManager::Instance().Play("Asset/Audio/SE/AttackHitOverlapping.wav");
		}
	}

	KdEffekseerManager::GetInstance().
		Play("SwordHit.efk", a_hitPos);
	KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("SwordHit.efk"); // これでループしない
	Math::Matrix efcMat = PlayerHitEffectMat(a_hitPos, a_enemy);
	KdEffekseerManager::GetInstance().SetWorldMatrix("SwordHit.efk", efcMat);
}

Math::Matrix Scopion::PlayerHitEffectMat(Math::Vector3 a_hitPos, std::weak_ptr<Enemy> a_enemy)
{
	Math::Matrix mat;

	const std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(m_owner.lock());

	Math::Matrix scaleMat = Math::Matrix::Identity;
	if (a_enemy.lock()->GetEnemyType() & Enemy::EnemyType::bossEnemyTypeOne)
	{
		scaleMat = Math::Matrix::CreateScale(1.05f);
	}
	else if (a_enemy.lock()->GetEnemyType() & Enemy::EnemyType::wimpEnemyTypeOne)
	{
		scaleMat = Math::Matrix::CreateScale(0.85f);
	}
	else
	{
		scaleMat = Math::Matrix::CreateScale(0.45f);
	}



	Math::Matrix RotYMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(player->GetAngleY()));

	if (player->GetCharaState() & CharacterBase::rAttackOne)
	{
		if (player->GetCharaState() & CharacterBase::grassHopperDash)
		{
			mat = scaleMat * Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(315)) *
				RotYMat * Math::Matrix::CreateTranslation(a_hitPos);
		}
		else
		{
			mat = scaleMat * Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(225)) *
				RotYMat * Math::Matrix::CreateTranslation(a_hitPos);
		}
	}
	else if (player->GetCharaState() & CharacterBase::rAttackTwo)
	{
		mat = scaleMat * Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(0)) *
			RotYMat * Math::Matrix::CreateTranslation(a_hitPos);
	}
	else if (player->GetCharaState() & CharacterBase::rAttackThree)
	{
		mat = scaleMat * Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(225)) *
			RotYMat * Math::Matrix::CreateTranslation(a_hitPos);
	}
	else if (player->GetCharaState() & CharacterBase::lAttackOne)
	{
		if (player->GetCharaState() & CharacterBase::grassHopperDash)
		{
			mat = scaleMat * Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(225)) *
				RotYMat * Math::Matrix::CreateTranslation(a_hitPos);
		}
		else
		{
			mat = scaleMat * Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(315)) *
				RotYMat * Math::Matrix::CreateTranslation(a_hitPos);
		}
	}
	else if (player->GetCharaState() & CharacterBase::lAttackTwo)
	{
		mat = scaleMat * Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(180)) *
			RotYMat * Math::Matrix::CreateTranslation(a_hitPos);
	}
	else if (player->GetCharaState() & CharacterBase::lAttackThree)
	{
		mat = scaleMat * Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(315)) *
			RotYMat * Math::Matrix::CreateTranslation(a_hitPos);
	}
	else if (player->GetCharaState() & CharacterBase::rlAttackOne)
	{
		mat = scaleMat * Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(225)) *
			RotYMat * Math::Matrix::CreateTranslation(a_hitPos);
	}
	else if (player->GetCharaState() & CharacterBase::rlAttackTwo)
	{
		mat = scaleMat * Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(0)) *
			RotYMat * Math::Matrix::CreateTranslation(a_hitPos);
	}
	else if (player->GetCharaState() & CharacterBase::rlAttackThree)
	{
		mat = scaleMat * Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(315)) *
			RotYMat * Math::Matrix::CreateTranslation(a_hitPos);
	}
	else if (player->GetCharaState() & CharacterBase::rlAttackRush)
	{
		if (!player->GetBRushRp())
		{
			if (player->GetAnimationCnt() < 21)
			{
				mat = scaleMat * Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(315)) *
					RotYMat * Math::Matrix::CreateTranslation(a_hitPos);
			}
			else if (player->GetAnimationCnt() >= 21 && player->GetAnimationCnt() < 31)
			{
				mat = scaleMat * Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(180)) *
					RotYMat * Math::Matrix::CreateTranslation(a_hitPos);
			}
			else if (player->GetAnimationCnt() >= 31 && player->GetAnimationCnt() < 49)
			{
				mat = scaleMat * Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(270)) *
					RotYMat * Math::Matrix::CreateTranslation(a_hitPos);
			}
			else if (player->GetAnimationCnt() >= 49 && player->GetAnimationCnt() < 107)
			{
				mat = scaleMat * Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(45)) *
					RotYMat * Math::Matrix::CreateTranslation({ a_hitPos.x,a_hitPos.y + 0.35f ,a_hitPos.z });
			}
		}
		else
		{
			if (player->GetAnimationCnt() < 115)
			{
				mat = scaleMat * Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(270)) *
					RotYMat * Math::Matrix::CreateTranslation(a_hitPos);
			}
		}
	}

	return mat;
}

Math::Matrix Scopion::EnemyHitEffectMat(Math::Vector3 a_hitPos, std::weak_ptr<Player> a_player)
{
	Math::Matrix mat;

	const std::shared_ptr<Enemy> enemy = std::dynamic_pointer_cast<Enemy>(m_owner.lock());

	Math::Matrix scaleMat = Math::Matrix::CreateScale(0.45f);
	Math::Matrix RotYMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(enemy->GetAngleY()));

	if (enemy->GetCharaState() & CharacterBase::rAttackOne)
	{
		if (enemy->GetCharaState() & CharacterBase::grassHopperDash)
		{
			mat = scaleMat * Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(315)) *
				RotYMat * Math::Matrix::CreateTranslation(a_hitPos);
		}
		else
		{
			mat = scaleMat * Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(225)) *
				RotYMat * Math::Matrix::CreateTranslation(a_hitPos);
		}
	}
	else if (enemy->GetCharaState() & CharacterBase::rAttackTwo)
	{
		mat = scaleMat * Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(0)) *
			RotYMat * Math::Matrix::CreateTranslation(a_hitPos);
	}
	else if (enemy->GetCharaState() & CharacterBase::rAttackThree)
	{
		mat = scaleMat * Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(225)) *
			RotYMat * Math::Matrix::CreateTranslation(a_hitPos);
	}
	else if (enemy->GetCharaState() & CharacterBase::lAttackOne)
	{
		if (enemy->GetCharaState() & CharacterBase::grassHopperDash)
		{
			mat = scaleMat * Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(225)) *
				RotYMat * Math::Matrix::CreateTranslation(a_hitPos);
		}
		else
		{
			mat = scaleMat * Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(315)) *
				RotYMat * Math::Matrix::CreateTranslation(a_hitPos);
		}
	}
	else if (enemy->GetCharaState() & CharacterBase::lAttackTwo)
	{
		mat = scaleMat * Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(180)) *
			RotYMat * Math::Matrix::CreateTranslation(a_hitPos);
	}
	else if (enemy->GetCharaState() & CharacterBase::lAttackThree)
	{
		mat = scaleMat * Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(315)) *
			RotYMat * Math::Matrix::CreateTranslation(a_hitPos);
	}
	else if (enemy->GetCharaState() & CharacterBase::rlAttackOne)
	{
		mat = scaleMat * Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(225)) *
			RotYMat * Math::Matrix::CreateTranslation(a_hitPos);
	}
	else if (enemy->GetCharaState() & CharacterBase::rlAttackTwo)
	{
		mat = scaleMat * Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(0)) *
			RotYMat * Math::Matrix::CreateTranslation(a_hitPos);
	}
	else if (enemy->GetCharaState() & CharacterBase::rlAttackThree)
	{
		mat = scaleMat * Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(315)) *
			RotYMat * Math::Matrix::CreateTranslation(a_hitPos);
	}
	else if (enemy->GetCharaState() & CharacterBase::rlAttackRush)
	{
		if (enemy->GetAnimationCnt() < 21)
		{
			mat = scaleMat * Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(315)) *
				RotYMat * Math::Matrix::CreateTranslation(a_hitPos);
		}
		else if (enemy->GetAnimationCnt() >= 21 && enemy->GetAnimationCnt() < 31)
		{
			mat = scaleMat * Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(180)) *
				RotYMat * Math::Matrix::CreateTranslation(a_hitPos);
		}
		else if (enemy->GetAnimationCnt() >= 31 && enemy->GetAnimationCnt() < 49)
		{
			mat = scaleMat * Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(270)) *
				RotYMat * Math::Matrix::CreateTranslation(a_hitPos);
		}
		else if (enemy->GetAnimationCnt() >= 49 && enemy->GetAnimationCnt() < 107)
		{
			mat = scaleMat * Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(45)) *
				RotYMat * Math::Matrix::CreateTranslation({ a_hitPos.x,a_hitPos.y + 0.35f,a_hitPos.z });
		}
	}

	return mat;
}

void Scopion::PlayerManAttackChaeck()
{
	const std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(m_owner.lock());
	if (!(player->GetWeaponType() & (Player::WeaponType::scorpion | Player::WeaponType::lScorpion)))return;

	const KdModelWork::Node* node = nullptr;
	Math::Matrix mat = Math::Matrix::Identity;

	for (auto& pTarget : m_pTargetList)
	{
		if (pTarget.expired())continue;

		// 敵との当たり判定＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
		if (!pTarget.lock()->GetAttackHit() && !pTarget.lock()->GetDefenseSuc() && !pTarget.lock()->GetBDeath() && player->GetCharaState() & CharacterBase::mantis && pTarget.lock()->GetInvincibilityTimeCnt() == 0)
		{
			if (player->GetCharaState() & CharacterBase::rAttack && m_arrmType == lArrm)return;
			if (player->GetCharaState() & CharacterBase::lAttack && m_arrmType == rArrm)return;
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
			pTarget.lock()->Intersects
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
				pTarget.lock()->BlowingAwayAttackOnHit(player->GetMatrix().Backward(), player.get());
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

				pTarget.lock()->Intersects
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
					pTarget.lock()->BlowingAwayAttackOnHit(player->GetMatrix().Backward(), player.get());
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

					pTarget.lock()->Intersects
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
						pTarget.lock()->BlowingAwayAttackOnHit(player->GetMatrix().Backward(), player.get());
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

						pTarget.lock()->Intersects
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
							pTarget.lock()->BlowingAwayAttackOnHit(player->GetMatrix().Backward(), player.get());
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

							pTarget.lock()->Intersects
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
								pTarget.lock()->BlowingAwayAttackOnHit(player->GetMatrix().Backward(), player.get());
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

								pTarget.lock()->Intersects
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
									pTarget.lock()->BlowingAwayAttackOnHit(player->GetMatrix().Backward(), player.get());
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

									pTarget.lock()->Intersects
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
										pTarget.lock()->BlowingAwayAttackOnHit(player->GetMatrix().Backward(), player.get());
										KdAudioManager::Instance().Play("Asset/Audio/SE/AttackHitOverlapping.wav");
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

void Scopion::EnemyHitAttackChaeck()
{
	if (m_eTarget.expired())return;
	const std::shared_ptr<Enemy> enemy = std::dynamic_pointer_cast<Enemy>(m_owner.lock());
	if (!(enemy->GetWeaponType() & (Enemy::WeaponType::scorpion | Enemy::WeaponType::lScorpion)))return;

	const KdModelWork::Node* node = nullptr;
	Math::Matrix mat = Math::Matrix::Identity;

	/*for (auto& obj : m_eTargetList)
{*/
// 攻撃の当たり判定
	for (auto& wepLis : m_eTarget.lock()->GetWeaponList())
	{
		// 武器との当たり判定＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
		if (m_eTarget.lock()->GetCharaState() & CharacterBase::defense &&
			!m_eTarget.lock()->GetAttackHit() && !m_eTarget.lock()->GetDefenseSuc() &&
			enemy->GetCharaState() & (CharacterBase::rAttack | CharacterBase::lAttack | CharacterBase::rlAttack) &&
			!m_eTarget.lock()->GetBDeath())
		{
			if ((enemy->GetCharaState() & CharacterBase::rAttack) && m_arrmType == lArrm)break;
			if ((enemy->GetCharaState() & CharacterBase::lAttack) && m_arrmType == rArrm)break;

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
					float betweenAng = DotProductCalculation(nowDir, targetDir);
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
				enemy->HasDefense();
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
						float betweenAng = DotProductCalculation(nowDir, targetDir);
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
					enemy->HasDefense();
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
							float betweenAng = DotProductCalculation(nowDir, targetDir);
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
						enemy->HasDefense();
						KdAudioManager::Instance().Play("Asset/Audio/SE/grad1.wav");
					}
				}
			}
		}
	}
	//}

	// Playerとの当たり判定＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
	if (!m_eTarget.lock()->GetAttackHit() && !m_eTarget.lock()->GetDefenseSuc() &&
		enemy->GetCharaState() & (CharacterBase::rAttack | CharacterBase::lAttack | CharacterBase::rlAttack | CharacterBase::rlAttackRush) &&
		m_eTarget.lock()->GetInvincibilityTimeCnt() == 0 && !m_eTarget.lock()->GetBDeath())
	{
		if (enemy->GetCharaState() & CharacterBase::rAttack && m_arrmType == lArrm)return;
		if (enemy->GetCharaState() & CharacterBase::lAttack && m_arrmType == rArrm)return;
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
		Math::Vector3 hitPos;
		for (auto& ret : retSphereList)
		{
			hit = true;
			hitDir = ret.m_hitDir;
			hitPos = ret.m_hitPos;
		}

		if (hit)
		{
			EnemyAttackHit(enemy, hitPos);
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
				hitPos = ret.m_hitPos;
			}

			if (hit)
			{
				EnemyAttackHit(enemy, hitPos);
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
					hitPos = ret.m_hitPos;
				}

				if (hit)
				{
					EnemyAttackHit(enemy, hitPos);
				}
			}
		}
		//}
	}
}

void Scopion::EnemyAttackHit(std::shared_ptr<Enemy> a_enemy, Math::Vector3 a_hitPos)
{
	if (a_enemy->GetCharaState() & (CharacterBase::rAttackOne | CharacterBase::lAttackOne) &&
		a_enemy->GetCharaState() & (CharacterBase::grassHopperDashF))
	{
		m_eTarget.lock()->BlowingAwayAttackOnHit(a_enemy->GetMatrix().Backward(), a_enemy.get());
		KdAudioManager::Instance().Play("Asset/Audio/SE/AttackHitOverlapping.wav");
	}
	else if (a_enemy->GetCharaState() & (CharacterBase::lAttackThree | CharacterBase::rAttackThree))
	{
		m_eTarget.lock()->BlowingAwayAttackOnHit(a_enemy->GetMatrix().Backward(), a_enemy.get());
		KdAudioManager::Instance().Play("Asset/Audio/SE/AttackHitOverlapping.wav");
	}
	else if (a_enemy->GetCharaState() & (CharacterBase::rAttackOne | CharacterBase::rAttackTwo |
		CharacterBase::lAttackOne | CharacterBase::lAttackTwo |
		CharacterBase::rlAttack) ||
		(CharacterBase::rlAttackRush && a_enemy->GetAnimationCnt() < 41))
	{
		if (a_enemy->GetCharaState() & CharacterBase::rlAttackThree)
		{
			a_enemy->SetBRushAttackPossible(true);
		}

		m_eTarget.lock()->OnHit(a_enemy->GetMatrix().Backward(), a_enemy.get());
		KdAudioManager::Instance().Play("Asset/Audio/SE/AttackHitOverlapping.wav");
	}

	else if (a_enemy->GetCharaState() & CharacterBase::rlAttackRush &&
		(a_enemy->GetAnimationCnt() >= 49 && a_enemy->GetAnimationCnt() < 57) ||
		(a_enemy->GetAnimationCnt() >= 58 && a_enemy->GetAnimationCnt() < 66) ||
		(a_enemy->GetAnimationCnt() >= 74 && a_enemy->GetAnimationCnt() < 89) ||
		(a_enemy->GetAnimationCnt() >= 89 && a_enemy->GetAnimationCnt() < 107))
	{
		m_eTarget.lock()->CutRaiseOnHit(a_enemy->GetMatrix().Backward());
		KdAudioManager::Instance().Play("Asset/Audio/SE/AttackHitOverlapping.wav");
	}

	KdEffekseerManager::GetInstance().
		Play("SwordHit.efk", a_hitPos);
	KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("SwordHit.efk"); // これでループしない
	KdEffekseerManager::GetInstance().SetWorldMatrix("SwordHit.efk", EnemyHitEffectMat(a_hitPos, m_eTarget));
}

void Scopion::EnemyManAttackChaeck()
{
	if (m_eTarget.expired())return;
	const std::shared_ptr<Enemy> enemy = std::dynamic_pointer_cast<Enemy>(m_owner.lock());
	if (!(enemy->GetWeaponType() & (Enemy::WeaponType::scorpion | Enemy::WeaponType::lScorpion)))return;

	const KdModelWork::Node* node = nullptr;
	Math::Matrix mat = Math::Matrix::Identity;

	// Playerとの当たり判定＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
	if (!m_eTarget.lock()->GetAttackHit() && !m_eTarget.lock()->GetDefenseSuc() && enemy->GetCharaState() & CharacterBase::mantis && m_eTarget.lock()->GetInvincibilityTimeCnt() == 0 && !m_eTarget.lock()->GetBDeath())
	{
		if (enemy->GetCharaState() & CharacterBase::rAttack && m_arrmType == lArrm)return;
		if (enemy->GetCharaState() & CharacterBase::lAttack && m_arrmType == rArrm)return;
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
			m_eTarget.lock()->BlowingAwayAttackOnHit(enemy->GetMatrix().Backward(), enemy.get());
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
				m_eTarget.lock()->BlowingAwayAttackOnHit(enemy->GetMatrix().Backward(), enemy.get());
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
					m_eTarget.lock()->BlowingAwayAttackOnHit(enemy->GetMatrix().Backward(), enemy.get());
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
						m_eTarget.lock()->BlowingAwayAttackOnHit(enemy->GetMatrix().Backward(), enemy.get());
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
							m_eTarget.lock()->BlowingAwayAttackOnHit(enemy->GetMatrix().Backward(), enemy.get());
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
								m_eTarget.lock()->BlowingAwayAttackOnHit(enemy->GetMatrix().Backward(), enemy.get());
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
									m_eTarget.lock()->BlowingAwayAttackOnHit(enemy->GetMatrix().Backward(), enemy.get());
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
