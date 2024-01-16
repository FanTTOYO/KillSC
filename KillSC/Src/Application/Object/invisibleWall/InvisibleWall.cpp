#include "InvisibleWall.h"
#include "../../Scene/SceneManager.h"

void InvisibleWall::Init()
{
	// 座標行列
	Math::Matrix transMat;
	transMat = Math::Matrix::CreateTranslation(0,-5,0);

	// 拡縮行列
	Math::Matrix scaleMat;
	scaleMat = Math::Matrix::CreateScale(25.0f,25.0f,25.0f);

	// 行列合成
	m_mWorld = scaleMat * transMat;

	m_model = std::make_shared<KdModelData>();
	m_model->Load("Asset/Models/Wall/Wall.gltf");
	// 当たり判定初期化
	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape
	("StageModel" ,m_model,KdCollider::TypeGround | KdCollider::TypeAttackDec | KdCollider::TypeSpeedDec);
}

void InvisibleWall::DrawUnLit()
{
	if (SceneManager::Instance().GetSceneType() == SceneManager::SceneType::tutorial || SceneManager::Instance().GetSceneType() == SceneManager::SceneType::training)
	{
		Math::Matrix transMat;
		transMat = Math::Matrix::CreateTranslation(0, -5, 0);

		// 拡縮行列
		Math::Matrix scaleMat;
		scaleMat = Math::Matrix::CreateScale(25.0f, 25.0f, 25.0f);

		// 行列合成
		m_mWorld = scaleMat * transMat;
		KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_model, m_mWorld);
	}
}
