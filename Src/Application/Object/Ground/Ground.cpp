#include "Ground.h"

void Ground::DrawLit()
{
	KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_model,m_mWorld);
}

void Ground::GenerateDepthMapFromLight()
{
	KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_model, m_mWorld);
}

void Ground::DrawBright()
{
	//KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_model, m_mWorld);
}

void Ground::Init()
{
	// ���W�s��
	Math::Matrix transMat;
	transMat = Math::Matrix::CreateTranslation(0,-5,0);

	// �g�k�s��
	Math::Matrix scaleMat;
	scaleMat = Math::Matrix::CreateScale(50.0f,50.0f,50.0f);

	// �s�񍇐�
	m_mWorld = scaleMat * transMat;

	m_model = std::make_shared<KdModelWork>();
	m_model->SetModelData
	(KdAssets::Instance().m_modeldatas.GetData
	("Asset/Models/Stage/Stage.gltf"));
	// �����蔻�菉����
	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape
	("StageModel" ,m_model,KdCollider::TypeGround);
}
