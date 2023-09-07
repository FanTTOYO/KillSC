#include "Bldg.h"
#include "../Ground/Ground.h"

void Bldg::DrawLit()
{
	KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_pModel, m_mWorld);	
}

void Bldg::GenerateDepthMapFromLight()
{
	KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_pModel, m_mWorld);
}

void Bldg::Init()
{
	
}

void Bldg::CreateBldg(std::string a_CollName, Math::Vector3 a_pos, std::string a_fileName, Math::Vector3 a_scale)
{
	m_pModel = std::make_shared<KdModelWork>();
	m_pModel->SetModelData
	(KdAssets::Instance().m_modeldatas.GetData
	(a_fileName));

	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape
	(a_CollName, m_pModel, KdCollider::TypeGround);

	Math::Matrix transMat = Math::Matrix::CreateTranslation(a_pos);

	Math::Matrix scaleMat = Math::Matrix::CreateScale(a_scale);
	m_mWorld = scaleMat * transMat;
}
