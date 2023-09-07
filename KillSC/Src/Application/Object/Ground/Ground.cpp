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
	// 座標行列
	Math::Matrix transMat;
	transMat = Math::Matrix::CreateTranslation(0,0,0);

	// 拡縮行列
	Math::Matrix scaleMat;
	scaleMat = Math::Matrix::CreateScale(1.0f,1.0f,1.0f);

	// 行列合成
	m_mWorld = scaleMat * transMat;

	m_model = std::make_shared<KdModelWork>();
	m_model->SetModelData
	(KdAssets::Instance().m_modeldatas.GetData
	("Asset/Models/Ground/ground.gltf"));
	// 当たり判定初期化
	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape
	("StageModel" ,m_model,KdCollider::TypeGround);
}
