#include "Ground.h"

void Ground::DrawLit()
{
	if (m_groundType != street)return;
	KdShaderManager::Instance().m_HD2DShader.SetUVTiling({ 5,5 });
	KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_model,m_mWorld);
}

void Ground::DrawUnLit()
{
	if (m_groundType != trainingRoom)return;
	KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_model, m_mWorld);
}

void Ground::GenerateDepthMapFromLight()
{
	KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_model, m_mWorld);
}

void Ground::DrawBright()
{
	//KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_model, m_mWorld);
}

void Ground::Init(groundType a_groundType)
{
	m_groundType = a_groundType;
	// 座標行列
	Math::Matrix transMat;

	// 拡縮行列
	Math::Matrix scaleMat;
	if (m_groundType == street)
	{
		transMat = Math::Matrix::CreateTranslation(0, 0, 0);
		scaleMat = Math::Matrix::CreateScale(1.0f,1.0f,1.0f);

		m_model = std::make_shared<KdModelData>();
		m_model->Load("Asset/Models/Ground/ground.gltf");
	}

	if (m_groundType == trainingRoom)
	{
		transMat = Math::Matrix::CreateTranslation(0, -5, 0);
		scaleMat = Math::Matrix::CreateScale(25.0f, 25.0f, 25.0f);

		m_model = std::make_shared<KdModelData>();
		m_model->Load("Asset/Models/Ground/TrainingRoomGround.gltf");
	}

	// 行列合成
	m_mWorld = scaleMat * transMat;
	// 当たり判定初期化
	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape
	("StageModel" ,m_model,KdCollider::TypeGround);
}
