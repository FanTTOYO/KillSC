#include "BaseScene.h"
#include "../../Scene/SceneManager.h"

void BaseScene::PreUpdate()
{
	// オブジェクトリストの整理
	// 不要なオブジェクトを削除
	auto it = m_objList.begin();
	while (it != m_objList.end()) // 数が変動するため範囲ベースForが使えない
	{
		// 不要になったオブジェクトを消す
		if ((*it)->IsExpired())
		{
			// 消す
			it = m_objList.erase(it); // 戻り値で次の場所を返してくれる
		}
		else
		{
			++it; // 次へ
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
	// 光を遮るオブジェクト(不透明な物体や2Dキャラ)はBeginとEndの間にまとめてDrawする
	KdShaderManager::Instance().m_HD2DShader.BeginGenerateDepthMapFromLight();
	{
		for (auto& obj : m_objList)
		{
			obj->GenerateDepthMapFromLight();
		}
	}
	KdShaderManager::Instance().m_HD2DShader.EndGenerateDepthMapFromLight();


	// スキンメッシュかつ光を遮るオブジェクト(不透明な物体や2Dキャラ)はBeginとEndの間にまとめてDrawする
	KdShaderManager::Instance().m_HD2DShader.BeginGenerateDepthMapFromLight_SkinMesh();
	{
		for (auto& obj : m_objList)
		{
			obj->GenerateDepthMapFromLight_SkinMesh();
		}
	}
	KdShaderManager::Instance().m_HD2DShader.EndGenerateDepthMapFromLight_SkinMesh();

	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// 陰影のあるオブジェクト(不透明な物体や2Dキャラ)はBeginとEndの間にまとめてDrawする
	KdShaderManager::Instance().m_HD2DShader.BeginLit();
	{
		for (auto& obj : m_objList)
		{
			obj->DrawLit();
		}
	}
	KdShaderManager::Instance().m_HD2DShader.EndLit();

	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// スキンメッシュかつ陰影のあるオブジェクト(不透明な物体や2Dキャラ)はBeginとEndの間にまとめてDrawする
	KdShaderManager::Instance().m_HD2DShader.BeginLit_SkinMesh();
	{
		for (auto& obj : m_objList)
		{
			obj->DrawLit_SkinMesh();
		}
	}
	KdShaderManager::Instance().m_HD2DShader.EndLit_SkinMesh();


	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// 陰影のないオブジェクト(透明な部分を含む物体やエフェクト)はBeginとEndの間にまとめてDrawする
	KdShaderManager::Instance().m_HD2DShader.BeginUnLit();
	{
		for (auto& obj : m_objList)
		{
			obj->DrawUnLit();
		}
	}
	KdShaderManager::Instance().m_HD2DShader.EndUnLit();


	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// 光源オブジェクト(自ら光るオブジェクトやエフェクト)はBeginとEndの間にまとめてDrawする
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
	// 2Dの描画はこの間で行う
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
