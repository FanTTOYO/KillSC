#include "BaseScene.h"
#include "../../Scene/SceneManager.h"

void BaseScene::PreUpdate()
{
	// �I�u�W�F�N�g���X�g�̐���
	// �s�v�ȃI�u�W�F�N�g���폜
	auto it = m_objList.begin();
	while (it != m_objList.end()) // �����ϓ����邽�ߔ͈̓x�[�XFor���g���Ȃ�
	{
		// �s�v�ɂȂ����I�u�W�F�N�g������
		if ((*it)->IsExpired())
		{
			// ����
			it = m_objList.erase(it); // �߂�l�Ŏ��̏ꏊ��Ԃ��Ă����
		}
		else
		{
			++it; // ����
		}
	}
}

void BaseScene::Update()
{
	if (SceneManager::Instance().GetUpdateStopCnt() == 0)
	{
		if (!m_bCountDown)
		{
			for (auto& obj : m_objList)
			{
				obj->Update();
			}
		}

		Event();
	}
	else
	{
		SceneManager::Instance().SetUpdateStopCnt(SceneManager::Instance().GetUpdateStopCnt() - 1);
	}
}

void BaseScene::PostUpdate()
{
	if (SceneManager::Instance().GetUpdateStopCnt() == 0)
	{
		if (!m_bCountDown)
		{
			for (auto& obj : m_objList)
			{
				obj->PostUpdate();
			}
		}
	}
}

void BaseScene::PreDraw()
{
	for (auto& obj : m_objList)
	{
		obj->PreDraw();
	}
}

void BaseScene::Draw()
{
	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// �����Ղ�I�u�W�F�N�g(�s�����ȕ��̂�2D�L����)��Begin��End�̊Ԃɂ܂Ƃ߂�Draw����
	KdShaderManager::Instance().m_HD2DShader.BeginGenerateDepthMapFromLight();
	{
		for (auto& obj : m_objList)
		{
			obj->GenerateDepthMapFromLight();
		}
	}
	KdShaderManager::Instance().m_HD2DShader.EndGenerateDepthMapFromLight();


	// �X�L�����b�V���������Ղ�I�u�W�F�N�g(�s�����ȕ��̂�2D�L����)��Begin��End�̊Ԃɂ܂Ƃ߂�Draw����
	KdShaderManager::Instance().m_HD2DShader.BeginGenerateDepthMapFromLight_SkinMesh();
	{
		for (auto& obj : m_objList)
		{
			obj->GenerateDepthMapFromLight_SkinMesh();
		}
	}
	KdShaderManager::Instance().m_HD2DShader.EndGenerateDepthMapFromLight_SkinMesh();

	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// �A�e�̂���I�u�W�F�N�g(�s�����ȕ��̂�2D�L����)��Begin��End�̊Ԃɂ܂Ƃ߂�Draw����
	KdShaderManager::Instance().m_HD2DShader.BeginLit();
	{
		for (auto& obj : m_objList)
		{
			obj->DrawLit();
		}
	}
	KdShaderManager::Instance().m_HD2DShader.EndLit();

	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// �X�L�����b�V�����A�e�̂���I�u�W�F�N�g(�s�����ȕ��̂�2D�L����)��Begin��End�̊Ԃɂ܂Ƃ߂�Draw����
	KdShaderManager::Instance().m_HD2DShader.BeginLit_SkinMesh();
	{
		for (auto& obj : m_objList)
		{
			obj->DrawLit_SkinMesh();
		}
	}
	KdShaderManager::Instance().m_HD2DShader.EndLit_SkinMesh();


	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// �A�e�̂Ȃ��I�u�W�F�N�g(�����ȕ������܂ޕ��̂�G�t�F�N�g)��Begin��End�̊Ԃɂ܂Ƃ߂�Draw����
	KdShaderManager::Instance().m_HD2DShader.BeginUnLit();
	{
		for (auto& obj : m_objList)
		{
			obj->DrawUnLit();
		}
	}
	KdShaderManager::Instance().m_HD2DShader.EndUnLit();


	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// �����I�u�W�F�N�g(�������I�u�W�F�N�g��G�t�F�N�g)��Begin��End�̊Ԃɂ܂Ƃ߂�Draw����
	KdShaderManager::Instance().m_postProcessShader.BeginBright();
	{
		for (auto& obj : m_objList)
		{
			obj->DrawBright();
		}
	}
	KdShaderManager::Instance().m_postProcessShader.EndBright();
}

void BaseScene::DrawSprite()
{
	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// 2D�̕`��͂��̊Ԃōs��
	KdShaderManager::Instance().m_spriteShader.Begin();
	{
		for (auto& obj : m_objList)
		{
			obj->DrawSprite();
		}
	}
	KdShaderManager::Instance().m_spriteShader.End();
}

void BaseScene::DrawDebug()
{
#ifdef _DEBUG
	KdShaderManager::Instance().m_HD2DShader.BeginUnLit();
	{
		for (auto& obj : m_objList)
		{
			obj->DrawDebug();
		}
	}
	KdShaderManager::Instance().m_HD2DShader.EndUnLit();
#endif
}

void BaseScene::Event()
{

}

void BaseScene::Init()
{

}

//void BaseScene::Release()
//{
//
//}
