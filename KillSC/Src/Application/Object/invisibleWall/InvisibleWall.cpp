#include "InvisibleWall.h"
#include "../../Scene/SceneManager.h"

void InvisibleWall::Init()
{
	// À•Ws—ñ
	Math::Matrix transMat;
	transMat = Math::Matrix::CreateTranslation(0,-5,0);

	// Šgks—ñ
	Math::Matrix scaleMat;
	scaleMat = Math::Matrix::CreateScale(25.0f,25.0f,25.0f);

	// s—ñ‡¬
	m_mWorld = scaleMat * transMat;

	m_model = std::make_shared<KdModelWork>();
	m_model->SetModelData
	(KdAssets::Instance().m_modeldatas.GetData
	("Asset/Models/Stage/Stage.gltf"));
	// “–‚½‚è”»’è‰Šú‰»
	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape
	("StageModel" ,m_model,KdCollider::TypeGround);
}

void InvisibleWall::DrawUnLit()
{
	if (SceneManager::Instance().GetSceneType() == SceneManager::SceneType::tutorial)
	{
		Math::Matrix transMat;
		transMat = Math::Matrix::CreateTranslation(0, -5, 0);

		// Šgks—ñ
		Math::Matrix scaleMat;
		scaleMat = Math::Matrix::CreateScale(25.0f, 25.0f, 25.0f);

		// s—ñ‡¬
		m_mWorld = scaleMat * transMat;
		KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_model, m_mWorld);
	}
}
