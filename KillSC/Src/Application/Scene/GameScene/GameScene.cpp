#include "GameScene.h"
#include "../../Object/Character/Player/Player.h"
#include "../../Object/Character/Enemy/Enemy.h"
#include "../../Object/invisibleWall/InvisibleWall.h"
#include "../../Object/Sky/Sky.h"
#include "../../Object/Ground/Ground.h"
#include "../../Object/Bldg/Bldg.h"
#include "../../Camera/GameCamera/GameCamera.h"
#include "../SceneManager.h"
#include "../../Object/Ui/Ui.h"

void GameScene::Event()
{
	/*if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		SceneManager::Instance().SetNextScene
		(
			SceneManager::SceneType::result
		);
	}*/

	if (m_wpUi.lock()->GetTime() >= 0 && m_wpUi.lock()->GetTime() < MAX_STATE_TIME_CNT || m_wpUi.lock()->GetBOption())
	{
		if (!m_bCountDown)
		{
			m_bCountDown = true;
		}
	}
	else
	{
		m_bCountDown = false;
	}

	if (m_bCountDown)
	{
		m_wpUi.lock()->Update();
	}

	if (m_wpUi.lock()->GetGameTimeM10() == 0 &&
		m_wpUi.lock()->GetGameTimeM1() == 0 &&
		m_wpUi.lock()->GetGameTimeS10() == 0 &&
		m_wpUi.lock()->GetGameTimeS1() == 0)
	{
		if (SceneManager::Instance().GetSceneType() == SceneManager::SceneType::challenge)
		{
			SceneManager::Instance().SetBAddOrSubVal(false);
			SceneManager::Instance().SetPointAddOrSubVal(500);
		}
		else
		{
			if (SceneManager::Instance().GetEnemyIeftover() < SceneManager::Instance().GetEnemyTotal())
			{
				SceneManager::Instance().SetBAddOrSubVal(true);
				SceneManager::Instance().SetBPlayerWin();
			}
			else
			{
				SceneManager::Instance().SetBAddOrSubVal(false);
				SceneManager::Instance().SetPointAddOrSubVal(500);
			}
		}

		SceneManager::Instance().SetNextScene(SceneManager::SceneType::result);
	}

	if (SceneManager::Instance().GetEnemyIeftover() == 0)
	{
		SceneManager::Instance().SetBAddOrSubVal(true);
		SceneManager::Instance().SetBPlayerWin();
		SceneManager::Instance().SetNextScene(SceneManager::SceneType::result);


		if (SceneManager::Instance().GetEnemyTotal() == CHARENGE_ONE_HUMDRED_ENEMY_CNT)
		{
			SceneManager::Instance().SetPointAddOrSubVal(6000);
		}
		else if (SceneManager::Instance().GetEnemyTotal() == CHARENGE_FIFTY_ENEMY_CNT)
		{
			SceneManager::Instance().SetPointAddOrSubVal(3000);
		}
		else if (SceneManager::Instance().GetEnemyTotal() == MIN_BOSS_ENEMY_CNT)
		{
			if (SceneManager::Instance().GetBHumanoidEnemy())
			{
				SceneManager::Instance().SetPointAddOrSubVal(1000);
			}
			else
			{
				SceneManager::Instance().SetPointAddOrSubVal(1500);
			}
		}
		else if (SceneManager::Instance().GetEnemyTotal() == MAX_BOSS_ENEMY_CNT)
		{
			SceneManager::Instance().SetPointAddOrSubVal(2000);
		}
	}

	if (SceneManager::Instance().GetScreenVibFrames() > 0)
	{
		SceneManager::Instance().SubScreenVibFrames();
	}

	if (SceneManager::Instance().GetSceneType() == SceneManager::SceneType::challenge)
	{
		if (SceneManager::Instance().GetEnemyIeftover() >= MAX_ENEMY_DRAW_TOTAL)
		{
			if (SceneManager::Instance().GetEnemyDrawTotal() == 0)
			{
				for (int i = 0; i < MAX_ENEMY_DRAW_TOTAL; ++i)
				{
					std::shared_ptr<Enemy> enemy;
					SceneManager::Instance().AddEnemyDrawTotal();
					enemy = std::make_shared<Enemy>();
					enemy->SetTarget(m_wpPlayer.lock());
					m_wpPlayer.lock()->AddEnemy(enemy);
					m_wpPlayer.lock()->AddWeaponToEnemy(enemy);
					m_wpUi.lock()->AddEnemy(enemy);
					enemy->Init(m_spJsonObj);
					enemy->SetEnemyNumber(i + 1);
					enemy->SetBBoss(true);
					enemy->SetPos(Math::Vector3(-10.0f + 5.0f * i, 0.0f, 20.0f));
					enemy->SetWorldRotationY(180);
					enemy->SetMatrix();

					m_objList.push_back(enemy);
					m_wpEnemyList.push_back(enemy);
				}
			}
			if (SceneManager::Instance().GetEnemyDrawTotal() < MAX_ENEMY_DRAW_TOTAL)
			{
				std::shared_ptr<Enemy> enemy;
				enemy = std::make_shared<Enemy>();
				enemy->SetTarget(m_wpPlayer.lock());
				m_wpPlayer.lock()->AddEnemy(enemy);
				m_wpPlayer.lock()->AddWeaponToEnemy(enemy);
				enemy->Init(m_spJsonObj);
				enemy->SetBBoss(true);
				m_wpUi.lock()->AddEnemy(enemy);
				m_objList.push_back(enemy);
				m_wpEnemyList.push_back(enemy);
				SceneManager::Instance().AddEnemyDrawTotal();
			}
		}
	}
	else if (SceneManager::Instance().GetSceneType() == SceneManager::SceneType::battle && !m_bCountDown)
	{
		int attackCnt = 0;
		for (auto& wpEnemy : m_wpEnemyList)
		{
			if (wpEnemy.expired())continue;
			if (wpEnemy.lock()->GetBBoss())continue;
			if (wpEnemy.lock()->GetEnemyAttackTotal() >= MAX_ENEMY_ATTACK_CNT)break;

			
			if (wpEnemy.lock()->GetEnemyState() & (Enemy::EnemyState::rAttack | Enemy::EnemyState::lAttack | Enemy::EnemyState::rlAttack | Enemy::EnemyState::rlAttackRush | Enemy::EnemyState::mantis) || wpEnemy.lock()->GetBRangedAttack())
			{
				++attackCnt;
				for (auto& enemy : m_wpEnemyList)
				{
					if (enemy.expired())continue;
					if (enemy.lock()->GetBBoss())continue;

					enemy.lock()->AddEnemyAttackTotal();
				}
			}
		}

		if (attackCnt == 0)
		{
			for (auto& wpEnemy : m_wpEnemyList)
			{
				if (wpEnemy.expired())continue;
				if (wpEnemy.lock()->GetBBoss())continue;
				wpEnemy.lock()->SetEnemyAttackTotal(0);
			}
		}

		if (!KdEffekseerManager::GetInstance().IsPlaying("EnemyAppearance.efk"))
		{
			m_appearanceEffectCnt = 0;
			m_bAppearanceEffect = false;
		}

		if (SceneManager::Instance().GetEnemyDrawTotal() == 0 && m_waveCnt < MAX_WAVE_CNT)
		{
			Math::Vector3 pos;
			if (!m_bAppearanceEffect)
			{
				m_appearanceEffectCnt = 0;
				m_bAppearanceEffect = true;

				pos = Math::Vector3((float)(*m_spJsonObj)["EnemyAppearanceF"][0].number_value(),
				   				    (float)(*m_spJsonObj)["EnemyAppearanceF"][1].number_value(),
								    (float)(*m_spJsonObj)["EnemyAppearanceF"][2].number_value());
				KdEffekseerManager::GetInstance().
					Play("EnemyAppearance.efk", pos);
				KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("EnemyAppearance.efk"); // これでループしない

				pos = Math::Vector3((float)(*m_spJsonObj)["EnemyAppearanceB"][0].number_value(),
					                (float)(*m_spJsonObj)["EnemyAppearanceB"][1].number_value(),
					                (float)(*m_spJsonObj)["EnemyAppearanceB"][2].number_value());
				KdEffekseerManager::GetInstance().
					Play("EnemyAppearance.efk", pos);
				KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("EnemyAppearance.efk"); // これでループしない

				pos = Math::Vector3((float)(*m_spJsonObj)["EnemyAppearanceR"][0].number_value(),
					                (float)(*m_spJsonObj)["EnemyAppearanceR"][1].number_value(),
					                (float)(*m_spJsonObj)["EnemyAppearanceR"][2].number_value());
				KdEffekseerManager::GetInstance().
					Play("EnemyAppearance.efk", pos);
				KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("EnemyAppearance.efk"); // これでループしない

				pos = Math::Vector3((float)(*m_spJsonObj)["EnemyAppearanceL"][0].number_value(),
					                (float)(*m_spJsonObj)["EnemyAppearanceL"][1].number_value(),
					                (float)(*m_spJsonObj)["EnemyAppearanceL"][2].number_value());
				KdEffekseerManager::GetInstance().
					Play("EnemyAppearance.efk", pos);
				KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("EnemyAppearance.efk"); // これでループしない

			}

			if (m_bAppearanceEffect)
			{
				++m_appearanceEffectCnt;
			}

			if (m_appearanceEffectCnt == ENEMY_APPEARANCE_MOMENT)
			{
				std::shared_ptr<Enemy> enemy;
				int total = 2;
				m_waveCnt++;
				m_wpUi.lock()->SetWaveCnt(m_waveCnt);

				if (m_waveCnt >= MAX_WAVE_CNT)
				{
					m_wpUi.lock()->SetBWaveChange();
				}

				auto& enemySharedObj = (*m_spJsonObj)["EnemyShared"].object_items();

				if (SceneManager::Instance().GetBHumanoidEnemy())
				{
					auto& coarseFishEnemyObj = (*m_spJsonObj)["CoarseFishEnemy"].object_items();

					if (SceneManager::Instance().GetEnemyTotal() == MAX_BOSS_ENEMY_CNT)
					{
						auto& enemyTotalTwoWimpEnemyObj = (*m_spJsonObj)["EnemyTotalTwoWimpEnemy"].object_items();
						// モンスター型Enemyインスタンス化==========================================================================================================
						SceneManager::Instance().AddEnemyDrawTotal();
						enemy = std::make_shared<Enemy>();
						enemy->SetTarget(m_wpPlayer.lock());
						m_wpPlayer.lock()->AddEnemy(enemy);
						m_wpPlayer.lock()->AddWeaponToEnemy(enemy);
						m_wpUi.lock()->AddEnemy(enemy);
						enemy->Init(m_spJsonObj);
						enemy->SetEnemyNumber(0);
						enemy->SetBBoss(false);
						pos = Math::Vector3((float)enemyTotalTwoWimpEnemyObj["EnemyTotalTwoWimpEnemyPosF"][0].number_value(),
							                (float)enemyTotalTwoWimpEnemyObj["EnemyTotalTwoWimpEnemyPosF"][1].number_value(),
							                (float)enemyTotalTwoWimpEnemyObj["EnemyTotalTwoWimpEnemyPosF"][2].number_value());
						enemy->SetPos(pos);
						enemy->SetWorldRotationY((float)enemySharedObj["EnemyAppearanceFWorldRotationY"].number_value());
						enemy->SetMatrix();
						enemy->SetModelAndType(Enemy::EnemyType::wimpEnemyTypeOne);
						m_objList.push_back(enemy);
						m_wpEnemyList.push_back(enemy);

						SceneManager::Instance().AddEnemyDrawTotal();
						enemy = std::make_shared<Enemy>();
						enemy->SetTarget(m_wpPlayer.lock());
						m_wpPlayer.lock()->AddEnemy(enemy);
						m_wpPlayer.lock()->AddWeaponToEnemy(enemy);
						m_wpUi.lock()->AddEnemy(enemy);
						enemy->Init(m_spJsonObj);
						enemy->SetEnemyNumber(0);
						enemy->SetBBoss(false);
						pos = Math::Vector3((float)enemyTotalTwoWimpEnemyObj["EnemyTotalTwoWimpEnemyPosB"][0].number_value(),
							                (float)enemyTotalTwoWimpEnemyObj["EnemyTotalTwoWimpEnemyPosB"][1].number_value(),
							                (float)enemyTotalTwoWimpEnemyObj["EnemyTotalTwoWimpEnemyPosB"][2].number_value());
						enemy->SetPos(pos);
						enemy->SetWorldRotationY((float)enemySharedObj["EnemyAppearanceBWorldRotationY"].number_value());
						enemy->SetMatrix();
						enemy->SetModelAndType(Enemy::EnemyType::wimpEnemyTypeOne);
						m_objList.push_back(enemy);
						m_wpEnemyList.push_back(enemy);

						SceneManager::Instance().AddEnemyDrawTotal();
						enemy = std::make_shared<Enemy>();
						enemy->SetTarget(m_wpPlayer.lock());
						m_wpPlayer.lock()->AddEnemy(enemy);
						m_wpPlayer.lock()->AddWeaponToEnemy(enemy);
						m_wpUi.lock()->AddEnemy(enemy);
						enemy->Init(m_spJsonObj);
						enemy->SetEnemyNumber(0);
						enemy->SetBBoss(false);
						pos = Math::Vector3((float)enemyTotalTwoWimpEnemyObj["EnemyTotalTwoWimpEnemyPosR"][0].number_value(),
							                (float)enemyTotalTwoWimpEnemyObj["EnemyTotalTwoWimpEnemyPosR"][1].number_value(),
							                (float)enemyTotalTwoWimpEnemyObj["EnemyTotalTwoWimpEnemyPosR"][2].number_value());
						enemy->SetPos(pos);
						enemy->SetWorldRotationY((float)enemySharedObj["EnemyAppearanceRWorldRotationY"].number_value());
						enemy->SetMatrix();
						enemy->SetModelAndType(Enemy::EnemyType::wimpEnemyTypeOne);
						m_objList.push_back(enemy);
						m_wpEnemyList.push_back(enemy);

						SceneManager::Instance().AddEnemyDrawTotal();
						enemy = std::make_shared<Enemy>();
						enemy->SetTarget(m_wpPlayer.lock());
						m_wpPlayer.lock()->AddEnemy(enemy);
						m_wpPlayer.lock()->AddWeaponToEnemy(enemy);
						m_wpUi.lock()->AddEnemy(enemy);
						enemy->Init(m_spJsonObj);
						enemy->SetEnemyNumber(0);
						enemy->SetBBoss(false);
						pos = Math::Vector3((float)enemyTotalTwoWimpEnemyObj["EnemyTotalTwoWimpEnemyPosL"][0].number_value(),
							                (float)enemyTotalTwoWimpEnemyObj["EnemyTotalTwoWimpEnemyPosL"][1].number_value(),
							                (float)enemyTotalTwoWimpEnemyObj["EnemyTotalTwoWimpEnemyPosL"][2].number_value());
						enemy->SetPos(pos);
						enemy->SetWorldRotationY((float)enemySharedObj["EnemyAppearanceLWorldRotationY"].number_value());
						enemy->SetMatrix();
						enemy->SetModelAndType(Enemy::EnemyType::wimpEnemyTypeOne);
						m_objList.push_back(enemy);
						m_wpEnemyList.push_back(enemy);
						// ===============================================================================================================================================
					}

					// 人型Enemyインスタンス化========================================================================================================================
					// 前方向
					for (int i = 0; i < total; ++i)
					{
						SceneManager::Instance().AddEnemyDrawTotal();
						enemy = std::make_shared<Enemy>();
						enemy->SetTarget(m_wpPlayer.lock());
						m_wpPlayer.lock()->AddEnemy(enemy);
						m_wpPlayer.lock()->AddWeaponToEnemy(enemy);
						m_wpUi.lock()->AddEnemy(enemy);
						enemy->Init(m_spJsonObj);
						enemy->SetEnemyNumber(i + 1);
						enemy->SetBBoss(false);
						pos = Math::Vector3((float)coarseFishEnemyObj["CoarseFishEnemyPosF"][0].number_value(),
							                (float)coarseFishEnemyObj["CoarseFishEnemyPosF"][1].number_value(),
							                (float)coarseFishEnemyObj["CoarseFishEnemyPosF"][2].number_value());
						enemy->SetPos(Math::Vector3(pos.x + 1.0f * i, pos.y, pos.z));
						enemy->SetWorldRotationY((float)enemySharedObj["EnemyAppearanceFWorldRotationY"].number_value());
						enemy->SetMatrix();
						enemy->SetModelAndType(Enemy::EnemyType::coarseFishEnemy);

						m_objList.push_back(enemy);
						m_wpEnemyList.push_back(enemy);
					}

					// 後ろ方向
					for (int i = 0; i < total; ++i)
					{
						SceneManager::Instance().AddEnemyDrawTotal();
						enemy = std::make_shared<Enemy>();
						enemy->SetTarget(m_wpPlayer.lock());
						m_wpPlayer.lock()->AddEnemy(enemy);
						m_wpPlayer.lock()->AddWeaponToEnemy(enemy);
						m_wpUi.lock()->AddEnemy(enemy);
						enemy->Init(m_spJsonObj);
						enemy->SetEnemyNumber(i + 1);
						enemy->SetBBoss(false);
						pos = Math::Vector3((float)coarseFishEnemyObj["CoarseFishEnemyPosB"][0].number_value(),
							                (float)coarseFishEnemyObj["CoarseFishEnemyPosB"][1].number_value(),
							                (float)coarseFishEnemyObj["CoarseFishEnemyPosB"][2].number_value());
						enemy->SetPos(Math::Vector3(pos.x + 1.0f * i, pos.y, pos.z));
						enemy->SetWorldRotationY((float)enemySharedObj["EnemyAppearanceBWorldRotationY"].number_value());
						enemy->SetMatrix();
						enemy->SetModelAndType(Enemy::EnemyType::coarseFishEnemy);


						m_objList.push_back(enemy);
						m_wpEnemyList.push_back(enemy);
					}

					// 右方向
					for (int i = 0; i < total; ++i)
					{
						SceneManager::Instance().AddEnemyDrawTotal();
						enemy = std::make_shared<Enemy>();
						enemy->SetTarget(m_wpPlayer.lock());
						m_wpPlayer.lock()->AddEnemy(enemy);
						m_wpPlayer.lock()->AddWeaponToEnemy(enemy);
						m_wpUi.lock()->AddEnemy(enemy);
						enemy->Init(m_spJsonObj);
						enemy->SetEnemyNumber(i + 1);
						enemy->SetBBoss(false);
						pos = Math::Vector3((float)coarseFishEnemyObj["CoarseFishEnemyPosR"][0].number_value(),
							                (float)coarseFishEnemyObj["CoarseFishEnemyPosR"][1].number_value(),
							                (float)coarseFishEnemyObj["CoarseFishEnemyPosR"][2].number_value());
						enemy->SetPos(Math::Vector3(pos.x, pos.y, pos.z + 1.0f * i));
						enemy->SetWorldRotationY((float)enemySharedObj["EnemyAppearanceRWorldRotationY"].number_value());
						enemy->SetMatrix();
						enemy->SetModelAndType(Enemy::EnemyType::coarseFishEnemy);

						m_objList.push_back(enemy);
						m_wpEnemyList.push_back(enemy);
					}

					// 左方向
					for (int i = 0; i < total; ++i)
					{
						SceneManager::Instance().AddEnemyDrawTotal();
						enemy = std::make_shared<Enemy>();
						enemy->SetTarget(m_wpPlayer.lock());
						m_wpPlayer.lock()->AddEnemy(enemy);
						m_wpPlayer.lock()->AddWeaponToEnemy(enemy);
						m_wpUi.lock()->AddEnemy(enemy);
						enemy->Init(m_spJsonObj);
						enemy->SetEnemyNumber(i + 1);
						enemy->SetBBoss(false);

						pos = Math::Vector3((float)coarseFishEnemyObj["CoarseFishEnemyPosL"][0].number_value(),
							                (float)coarseFishEnemyObj["CoarseFishEnemyPosL"][1].number_value(),
							                (float)coarseFishEnemyObj["CoarseFishEnemyPosL"][2].number_value());
						enemy->SetPos(Math::Vector3(pos.x, pos.y, pos.z + 1.0f * i));

						enemy->SetWorldRotationY((float)enemySharedObj["EnemyAppearanceLWorldRotationY"].number_value());
						enemy->SetMatrix();
						enemy->SetModelAndType(Enemy::EnemyType::coarseFishEnemy);

						m_objList.push_back(enemy);
						m_wpEnemyList.push_back(enemy);
					}
					//========================================================================================================================================================
				}   
				else // モンスター型Enemyインスタンス化==========================================================================================================
				{
					auto& wimpEnemyTypeOne = (*m_spJsonObj)["WimpEnemyTypeOne"].object_items();

					// 前方向
					SceneManager::Instance().AddEnemyDrawTotal();
					enemy = std::make_shared<Enemy>();
					enemy->SetTarget(m_wpPlayer.lock());
					m_wpPlayer.lock()->AddEnemy(enemy);
					m_wpPlayer.lock()->AddWeaponToEnemy(enemy);
					m_wpUi.lock()->AddEnemy(enemy);
					enemy->Init(m_spJsonObj);
					enemy->SetEnemyNumber(1);
					enemy->SetBBoss(false);
					pos = Math::Vector3((float)wimpEnemyTypeOne["WimpEnemyTypeOnePosF"][0].number_value(),
						                (float)wimpEnemyTypeOne["WimpEnemyTypeOnePosF"][1].number_value(),
						                (float)wimpEnemyTypeOne["WimpEnemyTypeOnePosF"][2].number_value());

					enemy->SetPos(Math::Vector3(pos.x, pos.y, pos.z));
					enemy->SetWorldRotationY((float)enemySharedObj["EnemyAppearanceFWorldRotationY"].number_value());
					enemy->SetMatrix();
					enemy->SetModelAndType(Enemy::EnemyType::wimpEnemyTypeOne);

					m_objList.push_back(enemy);
					m_wpEnemyList.push_back(enemy);
					
					SceneManager::Instance().AddEnemyDrawTotal();
					enemy = std::make_shared<Enemy>();
					enemy->SetTarget(m_wpPlayer.lock());
					m_wpPlayer.lock()->AddEnemy(enemy);
					m_wpPlayer.lock()->AddWeaponToEnemy(enemy);
					m_wpUi.lock()->AddEnemy(enemy);
					enemy->Init(m_spJsonObj);
					enemy->SetEnemyNumber(2);
					enemy->SetBBoss(false);
					pos = Math::Vector3((float)wimpEnemyTypeOne["WimpEnemyTypeOnePosB"][0].number_value(),
						                (float)wimpEnemyTypeOne["WimpEnemyTypeOnePosB"][1].number_value(),
						                (float)wimpEnemyTypeOne["WimpEnemyTypeOnePosB"][2].number_value());
					enemy->SetPos(Math::Vector3(pos.x, pos.y, pos.z));
					enemy->SetWorldRotationY((float)enemySharedObj["EnemyAppearanceBWorldRotationY"].number_value());
					enemy->SetMatrix();
					enemy->SetModelAndType(Enemy::EnemyType::wimpEnemyTypeOne);
					m_objList.push_back(enemy);
					m_wpEnemyList.push_back(enemy);

					// 右方向
					SceneManager::Instance().AddEnemyDrawTotal();
					enemy = std::make_shared<Enemy>();
					enemy->SetTarget(m_wpPlayer.lock());
					m_wpPlayer.lock()->AddEnemy(enemy);
					m_wpPlayer.lock()->AddWeaponToEnemy(enemy);
					m_wpUi.lock()->AddEnemy(enemy);
					enemy->Init(m_spJsonObj);
					enemy->SetEnemyNumber(3);
					enemy->SetBBoss(false);
					pos = Math::Vector3((float)wimpEnemyTypeOne["WimpEnemyTypeOnePosR"][0].number_value(),
						                (float)wimpEnemyTypeOne["WimpEnemyTypeOnePosR"][1].number_value(),
						                (float)wimpEnemyTypeOne["WimpEnemyTypeOnePosR"][2].number_value());
					enemy->SetPos(Math::Vector3(pos.x, pos.y, pos.z));
					enemy->SetWorldRotationY((float)enemySharedObj["EnemyAppearanceRWorldRotationY"].number_value());
					enemy->SetMatrix();
					enemy->SetModelAndType(Enemy::EnemyType::wimpEnemyTypeOne);
					m_objList.push_back(enemy);
					m_wpEnemyList.push_back(enemy);

					SceneManager::Instance().AddEnemyDrawTotal();
					enemy = std::make_shared<Enemy>();
					enemy->SetTarget(m_wpPlayer.lock());
					m_wpPlayer.lock()->AddEnemy(enemy);
					m_wpPlayer.lock()->AddWeaponToEnemy(enemy);
					m_wpUi.lock()->AddEnemy(enemy);
					enemy->Init(m_spJsonObj);
					enemy->SetEnemyNumber(4);
					enemy->SetBBoss(false);
					pos = Math::Vector3((float)wimpEnemyTypeOne["WimpEnemyTypeOnePosL"][0].number_value(),
						                (float)wimpEnemyTypeOne["WimpEnemyTypeOnePosL"][1].number_value(),
						                (float)wimpEnemyTypeOne["WimpEnemyTypeOnePosL"][2].number_value());
					enemy->SetPos(Math::Vector3(pos.x, pos.y, pos.z));
					enemy->SetWorldRotationY((float)enemySharedObj["EnemyAppearanceLWorldRotationY"].number_value());
					enemy->SetMatrix();
					enemy->SetModelAndType(Enemy::EnemyType::wimpEnemyTypeOne);

					m_objList.push_back(enemy);
					m_wpEnemyList.push_back(enemy);
				} // ==================================================================================================================================
			}
		}
		else if (SceneManager::Instance().GetEnemyDrawTotal() == 0 && m_waveCnt >= MAX_WAVE_CNT && m_bossAppearanceCnt < SceneManager::Instance().GetEnemyTotal())
		{
			auto& enemySharedObj = (*m_spJsonObj)["EnemyShared"].object_items();
			auto& bossEnemyObj   = (*m_spJsonObj)["BossEnemy"].object_items();
			auto& bossEnemyTypeOneObj = (*m_spJsonObj)["BossEnemyTypeOne"].object_items();
			Math::Vector3 pos = Math::Vector3((float)enemySharedObj["BossEnemyAppearanceEffectPos"][0].number_value(),
				                              (float)enemySharedObj["BossEnemyAppearanceEffectPos"][1].number_value(),
				                              (float)enemySharedObj["BossEnemyAppearanceEffectPos"][2].number_value());
			if (!m_bAppearanceEffect)
			{
				m_appearanceEffectCnt = 0;
				m_bAppearanceEffect = true;

				if (!SceneManager::Instance().GetBHumanoidEnemy())
				{
					// モンスター型ボス出現ゲートエフェクト==================================================================================================
					KdEffekseerManager::GetInstance().
						Play("EnemyAppearance.efk", pos);
					KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("EnemyAppearance.efk"); // これでループしない

					Math::Matrix efcMat = Math::Matrix::CreateScale((float)bossEnemyTypeOneObj["BossEnemyTypeOneAppearanceEffectScale"].number_value()) * Math::Matrix::CreateTranslation(pos);
					KdEffekseerManager::GetInstance().SetWorldMatrix("EnemyAppearance.efk", efcMat);
					//=======================================================================================================================================
				}
				else
				{
					// 人型ボス出現ゲートエフェクト========================================================================================================
					KdEffekseerManager::GetInstance().
						Play("EnemyAppearance.efk", pos);
					KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("EnemyAppearance.efk"); // これでループしない

					Math::Matrix efcMat = Math::Matrix::CreateScale((float)bossEnemyObj["BossEnemyAppearanceEffectScale"].number_value()) * Math::Matrix::CreateTranslation(pos);
					KdEffekseerManager::GetInstance().SetWorldMatrix("EnemyAppearance.efk", efcMat);
					//=======================================================================================================================================
				}

			}

			if (m_bAppearanceEffect)
			{
				++m_appearanceEffectCnt;
			}

			// ボスEnemyのインスタンス化================================================================================================================
			if (m_appearanceEffectCnt == ENEMY_APPEARANCE_MOMENT)
			{
				int total = SceneManager::Instance().GetEnemyTotal();
				std::shared_ptr<Enemy> enemy;
				// 前方向
				for (int i = 0; i < total; ++i)
				{
					m_bossAppearanceCnt++;
					SceneManager::Instance().AddEnemyDrawTotal();
					enemy = std::make_shared<Enemy>();
					enemy->SetTarget(m_wpPlayer.lock());
					m_wpPlayer.lock()->AddEnemy(enemy);
					m_wpPlayer.lock()->AddWeaponToEnemy(enemy);
					m_wpUi.lock()->AddEnemy(enemy);
					enemy->Init(m_spJsonObj);
					enemy->SetEnemyNumber(1);
					enemy->SetBBoss(true);

					switch (total)
					{
					case 1:
						enemy->SetPos(pos);
						enemy->SetWorldRotationY((float)enemySharedObj["EnemyAppearanceFWorldRotationY"].number_value());
						enemy->SetMatrix();
						break;
					case 2:
						enemy->SetPos(Math::Vector3(-3.0f + 6.0f * i, 10.0f, 30.0f));
						enemy->SetWorldRotationY((float)enemySharedObj["EnemyAppearanceFWorldRotationY"].number_value());
						enemy->SetMatrix();
						break;
					}

					if (!SceneManager::Instance().GetBHumanoidEnemy())
					{
						enemy->SetModelAndType(Enemy::EnemyType::bossEnemyTypeOne); // モンスター型のボスをここでセット
						m_wpGameCamera.lock()->SetHitObj(enemy);
						auto it = m_objList.begin();
						while (it != m_objList.end())
						{
							// 不要になったオブジェクトを消す
							if ((*it)->GetBBldg())
							{
								// 消す
								it = m_objList.erase(it); // 戻り値で次の場所を返してくれる
							}
							else
							{
								++it; // 次へ
							}
						}

						std::shared_ptr<Bldg> bldg;
						std::ifstream ifs("Asset/Data/BldgInfoBackUpBossOne.csv");
						std::string str, filed;
						while (getline(ifs, str))
						{
							bldg = std::make_shared<Bldg>();
							std::istringstream ss(str);
							int j = 0;
							std::string aStr[8];

							while (getline(ss, filed, ','))
							{
								aStr[j] = filed;
								j++;
							}

							if (j >= 7)
							{
								bldg->CreateBldg(aStr[0], Math::Vector3(stof(aStr[1]), stof(aStr[2]), stof(aStr[3])), aStr[4], Math::Vector3(stof(aStr[5]), stof(aStr[6]), stof(aStr[7])));
							}
							else
							{
								bldg->CreateBldg(aStr[0], Math::Vector3(stof(aStr[1]), stof(aStr[2]), stof(aStr[3])), aStr[4]);

							}
							m_objList.push_back(bldg);

							m_wpGameCamera.lock()->SetHitObj(bldg);
						}
					}
					m_objList.push_back(enemy);
					m_wpEnemyList.push_back(enemy);

				}
			}
			//===============================================================================================================
		}
	}
}

void GameScene::Init()
{
	m_waveCnt = 0;
	m_bossAppearanceCnt = 0;
	m_appearanceEffectCnt = 0;
	m_bAppearanceEffect = false;

	m_spJsonObj = std::make_shared<json11::Json>();
	{
		// jsonファイルを開く
		std::ifstream ifs("Asset/Data/objectVal.json");
		if (ifs.fail()) { assert(0 && "Json ファイルのパスが間違っています！！！"); };

		// 文字列として全読み込み
		std::string strJson((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

		std::string err;
		*m_spJsonObj = json11::Json::parse(strJson, err);

		if (err.size() > 0) { assert(0 && "読み込んだファイルのjson変換に失敗"); };
	}

	KdAudioManager::Instance().StopAllSound();

	std::shared_ptr<Sky> sky;
	sky = std::make_shared<Sky>();
	m_objList.push_back(sky);

	std::shared_ptr<GameCamera> camera = std::make_shared<GameCamera>();
	std::shared_ptr<Ground> ground;
	ground = std::make_shared<Ground>();
	m_objList.push_back(ground);
	camera->SetHitObj(ground);

	std::shared_ptr<Bldg> bldg;
	std::ifstream ifs("Asset/Data/BldgInfoBackUp.csv");
	std::string str, filed;
	while (getline(ifs, str))
	{
		bldg = std::make_shared<Bldg>();
		std::istringstream ss(str);
		int i = 0;
		std::string aStr[8];

		while (getline(ss, filed, ','))
		{
			aStr[i] = filed;
			i++;
		}

		if (i >= 7)
		{
			bldg->CreateBldg(aStr[0], Math::Vector3(stof(aStr[1]), stof(aStr[2]), stof(aStr[3])), aStr[4], Math::Vector3(stof(aStr[5]), stof(aStr[6]), stof(aStr[7])));
		}
		else
		{
			bldg->CreateBldg(aStr[0], Math::Vector3(stof(aStr[1]), stof(aStr[2]), stof(aStr[3])), aStr[4]);
		}
		m_objList.push_back(bldg);

		camera->SetHitObj(bldg);
	}

	std::shared_ptr<InvisibleWall> invisibleWall;
	invisibleWall = std::make_shared<InvisibleWall>();
	m_objList.push_back(invisibleWall);

	std::shared_ptr<Player> player;
	player = std::make_shared<Player>();
	m_objList.push_back(player);
	m_wpPlayer = player;

	std::shared_ptr<Ui> ui = std::make_shared<Ui>();

	ui->SetUiType(Ui::UiType::game);
	ui->Init();
	ui->SetPlayer(player);
	ui->SetWaveCnt(m_waveCnt);
	m_objList.push_back(ui);
	m_wpUi = ui;

	player->Init(m_spJsonObj);


	camera->SetTarget(player);
	camera->SetPlayer(player);
	player->SetCamera(camera);
	ui->SetCamera(camera);
	m_objList.push_back(camera);

	KdEffekseerManager::GetInstance().SetCamera(camera);
	m_wpGameCamera = camera;


	SetCursorPos(640, 360);
	KdAudioManager::Instance().Play("Asset/Audio/SE/CntDwon1.wav");
}