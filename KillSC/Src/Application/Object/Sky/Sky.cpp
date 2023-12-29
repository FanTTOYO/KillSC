﻿#include "Sky.h"

void Sky::Update()
{

}

void Sky::Init()
{
	// 座標行列
	Math::Matrix transMat;
	transMat = Math::Matrix::CreateTranslation(0, 0, 0);

	// 拡縮行列
	Math::Matrix scaleMat;
	scaleMat = Math::Matrix::CreateScale(6.5, 10.0f, 6.5);
//	scaleMat = Math::Matrix::CreateScale(15, 10.0f, 15);

	// 行列合成
	m_mWorld = scaleMat * transMat;

	m_model = std::make_shared<KdModelWork>();
	m_model->SetModelData
	(KdAssets::Instance().m_modeldatas.GetData
	("Asset/Models/Sky/Sky.gltf"));
}

void Sky::DrawUnLit()
{
	KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_model, m_mWorld);
}

void Sky::Release()
{

}
