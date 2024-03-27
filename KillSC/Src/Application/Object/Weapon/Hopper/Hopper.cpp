#include "Hopper.h"
#include "../../Character/Player/Player.h"
#include "../../Character/Enemy/Enemy.h"

void Hopper::Update()
{
	if (m_bPlayerWeapon)
	{
		const std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(m_owner.lock());
		if (!(player->GetWeaponType() & (Player::WeaponType::grassHopper | Player::WeaponType::lGrassHopper)))return;

		const KdModelWork::Node* node = nullptr;
		if (m_arrmType == rArrm && player->GetWeaponType() & Player::WeaponType::grassHopper)
		{
			node = player->GetModel()->FindNode("WeaponRPoint");
		}

		if (m_arrmType == lArrm && player->GetWeaponType() & Player::WeaponType::lGrassHopper)
		{
			node = player->GetModel()->FindNode("WeaponLPoint");
		}

		if (node)
		{
			m_mWorld = node->m_worldTransform * player->GetMatrix();
			m_mWorld._42 += 0.9f;
		}

		if (player->GetCharaState() & CharacterBase::grassHopperDashF)
		{
			m_hopperMat = Math::Matrix::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(m_yAng),
				DirectX::XMConvertToRadians(45),
				DirectX::XMConvertToRadians(0)) * Math::Matrix::CreateTranslation(m_pos);
		}
		else if (player->GetCharaState() & CharacterBase::grassHopperDashB)
		{
			m_hopperMat = Math::Matrix::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(m_yAng),
				DirectX::XMConvertToRadians(315),
				DirectX::XMConvertToRadians(0)) * Math::Matrix::CreateTranslation(m_pos);
		}
		else if (player->GetCharaState() & CharacterBase::grassHopperDashL)
		{
			m_hopperMat = Math::Matrix::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(m_yAng + 90),
				DirectX::XMConvertToRadians(315),
				DirectX::XMConvertToRadians(0)) * Math::Matrix::CreateTranslation(m_pos);
		}
		else if (player->GetCharaState() & CharacterBase::grassHopperDashR)
		{
			m_hopperMat = Math::Matrix::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(m_yAng + 270),
				DirectX::XMConvertToRadians(315),
				DirectX::XMConvertToRadians(0)) * Math::Matrix::CreateTranslation(m_pos);
		}

		if (player->GetCharaState() & CharacterBase::grassHopperDashUp)
		{
			m_hopperMat = Math::Matrix::CreateTranslation(m_pos);
		}

		if (m_bAddanimeTime)
		{
			m_animator->AdvanceTime(m_hopperModel->WorkNodes());
			if (m_animator->IsAnimationEnd())
			{
				m_bAddanimeTime = false;
			}
		}
	}

	if (m_bEnemyWeapon)
	{
		const std::shared_ptr<Enemy> enemy = std::dynamic_pointer_cast<Enemy>(m_owner.lock());
		if (!(enemy->GetWeaponType() & (Enemy::WeaponType::grassHopper | Enemy::WeaponType::lGrassHopper)))return;

		const KdModelWork::Node* node = nullptr;
		if (m_arrmType == rArrm && enemy->GetWeaponType() & Enemy::WeaponType::grassHopper)
		{
			node = enemy->GetModel()->FindNode("WeaponRPoint");
		}

		if (m_arrmType == lArrm && enemy->GetWeaponType() & Enemy::WeaponType::lGrassHopper)
		{
			node = enemy->GetModel()->FindNode("WeaponLPoint");
		}

		if (node)
		{
			m_mWorld = node->m_worldTransform * enemy->GetMatrix();
			m_mWorld._42 += 0.9f;
		}

		if (enemy->GetCharaState() & CharacterBase::grassHopperDashF)
		{
			m_hopperMat = Math::Matrix::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(m_yAng),
				DirectX::XMConvertToRadians(45),
				DirectX::XMConvertToRadians(0)) * Math::Matrix::CreateTranslation(m_pos);
		}
		else if (enemy->GetCharaState() & CharacterBase::grassHopperDashB)
		{
			m_hopperMat = Math::Matrix::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(m_yAng + 180),
				DirectX::XMConvertToRadians(315),
				DirectX::XMConvertToRadians(0)) * Math::Matrix::CreateTranslation(m_pos);
		}
		else if (enemy->GetCharaState() & CharacterBase::grassHopperDashL)
		{
			m_hopperMat = Math::Matrix::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(m_yAng + 90),
				DirectX::XMConvertToRadians(315),
				DirectX::XMConvertToRadians(0)) * Math::Matrix::CreateTranslation(m_pos);
		}
		else if (enemy->GetCharaState() & CharacterBase::grassHopperDashR)
		{
			m_hopperMat = Math::Matrix::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(m_yAng + 270),
				DirectX::XMConvertToRadians(315),
				DirectX::XMConvertToRadians(0)) * Math::Matrix::CreateTranslation(m_pos);
		}

		if (enemy->GetCharaState() & CharacterBase::grassHopperDashUp)
		{
			m_hopperMat = Math::Matrix::CreateTranslation(m_pos);
		}

		if (m_bAddanimeTime)
		{
			m_animator->AdvanceTime(m_hopperModel->WorkNodes());
			if (m_animator->IsAnimationEnd())
			{
				m_bAddanimeTime = false;
			}
		}
	}
}

void Hopper::DrawBright()
{
	if (m_bPlayerWeapon)
	{
		const std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(m_owner.lock());
		if (!(player->GetWeaponType() & (Player::WeaponType::grassHopper | Player::WeaponType::lGrassHopper)))return;

		if (m_arrmType == rArrm && player->GetWeaponType() & Player::WeaponType::grassHopper)
		{
			KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_model, m_mWorld);
		}

		if (m_arrmType == lArrm && player->GetWeaponType() & Player::WeaponType::lGrassHopper)
		{
			KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_model, m_mWorld);
		}

		if (m_arrmType == rArrm && player->GetRGrassHopperTime() >= 60 && player->GetCharaState() & (CharacterBase::grassHopperDashUp | CharacterBase::grassHopperDash))
		{
			KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_hopperModel, m_hopperMat);
		}

		if (m_arrmType == lArrm && player->GetLGrassHopperTime() >= 60 && player->GetCharaState() & (CharacterBase::grassHopperDashUp | CharacterBase::grassHopperDash))
		{
			KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_hopperModel, m_hopperMat);
		}
	}

	if (m_bEnemyWeapon)
	{
		const std::shared_ptr<Enemy> enemy = std::dynamic_pointer_cast<Enemy>(m_owner.lock());
		if (!(enemy->GetWeaponType() & (Enemy::WeaponType::grassHopper | Enemy::WeaponType::lGrassHopper)))return;

		if (m_arrmType == rArrm && enemy->GetWeaponType() & Enemy::WeaponType::grassHopper)
		{
			KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_model, m_mWorld);
		}

		if (m_arrmType == lArrm && enemy->GetWeaponType() & Enemy::WeaponType::lGrassHopper)
		{
			KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_model, m_mWorld);
		}

		if (m_arrmType == rArrm && enemy->GetRGrassHopperTime() >= 60 && enemy->GetCharaState() & (CharacterBase::grassHopperDashUp | CharacterBase::grassHopperDash))
		{
			KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_hopperModel, m_hopperMat);
		}

		if (m_arrmType == lArrm && enemy->GetLGrassHopperTime() >= 60 && enemy->GetCharaState() & (CharacterBase::grassHopperDashUp | CharacterBase::grassHopperDash))
		{
			KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_hopperModel, m_hopperMat);
		}
	}
}

void Hopper::GenerateDepthMapFromLight()
{
	if (m_bPlayerWeapon)
	{
		const std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(m_owner.lock());
		if (!(player->GetWeaponType() & (Player::WeaponType::grassHopper | Player::WeaponType::lGrassHopper)))return;

		if (m_arrmType == rArrm && player->GetWeaponType() & Player::WeaponType::grassHopper)
		{
			KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_model, m_mWorld);
		}

		if (m_arrmType == lArrm && player->GetWeaponType() & Player::WeaponType::lGrassHopper)
		{
			KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_model, m_mWorld);
		}
	}

	if (m_bEnemyWeapon)
	{
		const std::shared_ptr<Enemy> enemy = std::dynamic_pointer_cast<Enemy>(m_owner.lock());
		if (!(enemy->GetWeaponType() & (Enemy::WeaponType::grassHopper | Enemy::WeaponType::lGrassHopper)))return;

		if (m_arrmType == rArrm && enemy->GetWeaponType() & Enemy::WeaponType::grassHopper)
		{
			KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_model, m_mWorld);
		}

		if (m_arrmType == lArrm && enemy->GetWeaponType() & Enemy::WeaponType::lGrassHopper)
		{
			KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_model, m_mWorld);
		}
	}
}

void Hopper::StartAnime()
{
	m_bAddanimeTime = true;
	m_animator->SetAnimation(m_hopperModel->GetAnimation("Hopper"), false);
}

void Hopper::Init()
{
	m_model = std::make_unique<KdModelWork>();
	m_model->SetModelData
	(KdAssets::Instance().m_modeldatas.GetData
	("Asset/Models/Weapon/Hopper/grassOutbreakSource.gltf"));

	m_hopperModel = std::make_unique<KdModelWork>();
	m_hopperModel->SetModelData
	(KdAssets::Instance().m_modeldatas.GetData
	("Asset/Models/Weapon/Hopper/Hopper.gltf"));

	m_animator = std::make_shared<KdAnimator>();
}
