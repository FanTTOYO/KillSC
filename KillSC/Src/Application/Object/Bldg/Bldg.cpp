#include "Bldg.h"

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
	m_bBldg = true;
}

void Bldg::Update()
{
#ifdef _DEBUG
	m_pDebugWire->AddDebugSphere
	(
		m_mWorld.Translation(),
		6.5f,
		{ 0,0,1,1 }
	);

	m_pDebugWire->AddDebugSphere
	(
		m_mWorld.Translation() + Math::Vector3(-5.0f, 0.0f, 5.0f),
		2.60f,
		{ 0,0,1,1 }
	);

	m_pDebugWire->AddDebugSphere
	(
		m_mWorld.Translation() + Math::Vector3(5.0f, 0.0f, 5.0f),
		2.60f,
		{ 0,0,1,1 }
	);

	m_pDebugWire->AddDebugSphere
	(
		m_mWorld.Translation() + Math::Vector3(-5.0f, 0.0f, -5.0f),
		2.60f,
		{ 0,0,1,1 }
	);

	m_pDebugWire->AddDebugSphere
	(
		m_mWorld.Translation() + Math::Vector3(5.0f, 0.0f, -5.0f),
		2.60f,
		{ 0,0,1,1 }
	);

	m_pDebugWire->AddDebugSphere
	(
		m_mWorld.Translation() + Math::Vector3(-6.5f, 0.0f, 6.5f),
		1.05f,
		{ 0,0,1,1 }
	);

	m_pDebugWire->AddDebugSphere
	(
		m_mWorld.Translation() + Math::Vector3(6.5f, 0.0f, 6.5f),
		1.05f,
		{ 0,0,1,1 }
	);

	m_pDebugWire->AddDebugSphere
	(
		m_mWorld.Translation() + Math::Vector3(-6.5f, 0.0f, -6.5f),
		1.05f,
		{ 0,0,1,1 }
	);

	m_pDebugWire->AddDebugSphere
	(
		m_mWorld.Translation() + Math::Vector3(6.5f, 0.0f, -6.5f),
		1.05f,
		{ 0,0,1,1 }
	);
#endif
}

void Bldg::CreateBldg(std::string a_CollName, Math::Vector3 a_pos, std::string a_fileName, Math::Vector3 a_scale)
{
	m_pModel = std::make_shared<KdModelWork>();
	m_pModel->SetModelData
	(KdAssets::Instance().m_modeldatas.GetData
	(a_fileName));

	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape
	(a_CollName, m_pModel, KdCollider::TypeGround | KdCollider::TypeSpeedDec);

	Math::Matrix transMat = Math::Matrix::CreateTranslation(a_pos);

	Math::Matrix scaleMat = Math::Matrix::CreateScale(a_scale);
	m_mWorld = scaleMat * transMat;

	m_pCollider->RegisterCollisionShape
	(
		a_CollName + "col1",
		Math::Vector3(0.0f,0.25f,0.0f),
		6.5f,
		KdCollider::TypeBuried
	);

	m_pCollider->RegisterCollisionShape
	(
		a_CollName + "col2",
		Math::Vector3(-5.0f, 0.0f, 5.0f),
		2.55f,
		KdCollider::TypeBuried
	);

	m_pCollider->RegisterCollisionShape
	(
		a_CollName + "col3",
		Math::Vector3(5.0f, 0.0f, 5.0f),
		2.5f,
		KdCollider::TypeBuried
	);

	m_pCollider->RegisterCollisionShape
	(
		a_CollName + "col4",
		Math::Vector3(-5.0f, 0.0f, -5.0f),
		2.55f,
		KdCollider::TypeBuried
	);

	m_pCollider->RegisterCollisionShape
	(
		a_CollName + "col5",
		Math::Vector3(5.0f, 0.0f, -5.0f),
		2.55f,
		KdCollider::TypeBuried
	);

	m_pCollider->RegisterCollisionShape
	(
		a_CollName + "col6",
		Math::Vector3(-6.5f, 0.0f, 6.5f),
		1.05f,
		KdCollider::TypeBuried
	);

	m_pCollider->RegisterCollisionShape
	(
		a_CollName + "col7",
		Math::Vector3(6.5f, 0.0f, 6.5f),
		1.05f,
		KdCollider::TypeBuried
	);

	m_pCollider->RegisterCollisionShape
	(
		a_CollName + "col8",
		Math::Vector3(-6.5f, 0.0f, -6.5f),
		1.05f,
		KdCollider::TypeBuried
	);

	m_pCollider->RegisterCollisionShape
	(
		a_CollName + "col9",
		Math::Vector3(6.5f, 0.0f, -6.5f),
		1.05f,
		KdCollider::TypeBuried
	);

	m_pDebugWire = std::make_unique<KdDebugWireFrame>();
}
