#include "Sky.h"

void Sky::Update()
{

}

void Sky::Init()
{
	// ���W�s��
	Math::Matrix transMat;
	transMat = Math::Matrix::CreateTranslation(0, 0, 0);

	// �g�k�s��
	Math::Matrix scaleMat;
	scaleMat = Math::Matrix::CreateScale(12.5f, 18.0f, 12.5f);

	// �s�񍇐�
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
