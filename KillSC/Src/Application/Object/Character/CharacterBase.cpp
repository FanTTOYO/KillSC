#include "CharacterBase.h"
#include "../Weapon/Hopper/Hopper.h"
#include "../Weapon/Scopion/Scopion.h"
#include "../../Scene/SceneManager.h"
#include "Player/Player.h"
#include "Enemy/Enemy.h"


void CharacterBase::Init()
{
	m_bDeath = false;
	m_bLose = false;
	m_charaState = idle;
	m_spAnimator = std::make_shared<KdAnimator>();
	m_spAnimator->SetAnimation(m_spModel->GetAnimation("IdleA"), false);
	m_graduallyTorionDecVal = 0.0f;
	m_bRushAttackPossible = false;
	m_invincibilityTimeCnt = 0;
	m_mWorldRot = {};
	m_bFirstUpdate = true;
	m_bWaponDrawPosible = false;
	m_overStageTime = 0;
	m_bBlowingAwayHitB = false;
}

// キックが当たってるかどうかの判定処理
void CharacterBase::KickHitAttackChaeck(std::shared_ptr<CharacterBase> a_target, CharacterBase* a_pAttackedCharacter)
{
	const KdModelWork::Node* node = nullptr;
	Math::Matrix mat = Math::Matrix::Identity;

	if (!a_target)return;

	if (!a_target->GetAttackHit() && !a_target->GetDefenseSuc() && a_target->GetInvincibilityTimeCnt() == 0 && !a_target->GetBDeath()) // ここにはなくてもいいかも
	{
		node = m_spModel->FindNode("LegAttackPoint");
		KdCollider::SphereInfo sphereInfo;
		mat = node->m_worldTransform * m_mWorld;
		mat._42 += static_cast<float>(m_mpObj["AddBottomYVal"].number_value());
		sphereInfo.m_sphere.Center = mat.Translation();
		sphereInfo.m_sphere.Radius = 0.30f;
		sphereInfo.m_type = KdCollider::TypeDamage;

#ifdef _DEBUG
		m_pDebugWire->AddDebugSphere
		(
			sphereInfo.m_sphere.Center,
			sphereInfo.m_sphere.Radius,
			{ 0,0,0,1 }
		);
#endif

		std::list<KdCollider::CollisionResult> retSphereList;

		a_target->Intersects
		(
			sphereInfo,
			&retSphereList
		);

		Math::Vector3 hitDir = {};
		bool hit = false;
		Math::Vector3 hitPos = {};
		for (auto& ret : retSphereList)
		{
			hit = true;
			hitDir = ret.m_hitDir;
			hitPos = ret.m_hitPos;
		}

		if (hit)
		{
			a_target->BlowingAwayAttackOnHit(m_mWorld.Backward(), a_pAttackedCharacter);
			KdAudioManager::Instance().Play("Asset/Audio/SE/KickAttackHit.wav");
			hitPos.y += 0.35f;
			KdEffekseerManager::GetInstance().Play("Hit3.efk", hitPos);
			KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("Hit3.efk"); // これでループしない
			Math::Matrix efcMat = Math::Matrix::CreateScale(0.5f) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)) * Math::Matrix::CreateTranslation(hitPos);
			KdEffekseerManager::GetInstance().SetWorldMatrix("Hit3.efk", efcMat);
		}
		else
		{
			node = m_spModel->FindNode("LegAttackHitPoint");
			sphereInfo;
			mat = node->m_worldTransform * m_mWorld;
			mat._42 += static_cast<float>(m_mpObj["AddBottomYVal"].number_value());
			sphereInfo.m_sphere.Center = mat.Translation();
			sphereInfo.m_sphere.Radius = 0.30f;
			sphereInfo.m_type = KdCollider::TypeDamage;

#ifdef _DEBUG
			m_pDebugWire->AddDebugSphere
			(
				sphereInfo.m_sphere.Center,
				sphereInfo.m_sphere.Radius,
				{ 0,0,0,1 }
			);
#endif

			retSphereList.clear();

			a_target->Intersects
			(
				sphereInfo,
				&retSphereList
			);

			hitDir = {};
			hit = false;
			for (auto& ret : retSphereList)
			{
				hit = true;
				hitDir = ret.m_hitDir;
				hitPos = ret.m_hitPos;
			}

			if (hit)
			{
				a_target->BlowingAwayAttackOnHit(m_mWorld.Backward(), a_pAttackedCharacter);
				KdAudioManager::Instance().Play("Asset/Audio/SE/KickAttackHit.wav");
				hitPos.y += 0.35f;
				KdEffekseerManager::GetInstance().Play("Hit3.efk", hitPos);
				KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("Hit3.efk"); // これでループしない
				Math::Matrix efcMat = Math::Matrix::CreateScale(0.5f) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)) * Math::Matrix::CreateTranslation(hitPos);
				KdEffekseerManager::GetInstance().SetWorldMatrix("Hit3.efk", efcMat);

			}
			else
			{
				node = m_spModel->FindNode("LegAttackHitPointTwo");
				sphereInfo;
				mat = node->m_worldTransform * m_mWorld;
				mat._42 += static_cast<float>(m_mpObj["AddBottomYVal"].number_value());
				sphereInfo.m_sphere.Center = mat.Translation();
				sphereInfo.m_sphere.Radius = 0.30f;
				sphereInfo.m_type = KdCollider::TypeDamage;

#ifdef _DEBUG
				m_pDebugWire->AddDebugSphere
				(
					sphereInfo.m_sphere.Center,
					sphereInfo.m_sphere.Radius,
					{ 0,0,0,1 }
				);
#endif

				retSphereList.clear();

				a_target->Intersects
				(
					sphereInfo,
					&retSphereList
				);

				hitDir = {};
				hit = false;
				for (auto& ret : retSphereList)
				{
					hit = true;
					hitDir = ret.m_hitDir;
					hitPos = ret.m_hitPos;
				}

				if (hit)
				{
					a_target->BlowingAwayAttackOnHit(m_mWorld.Backward(), a_pAttackedCharacter);
					KdAudioManager::Instance().Play("Asset/Audio/SE/KickAttackHit.wav");

					hitPos.y += 0.35f;
					KdEffekseerManager::GetInstance().Play("Hit3.efk", hitPos);
					KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("Hit3.efk"); // これでループしない
					Math::Matrix efcMat = Math::Matrix::CreateScale(0.5f) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)) * Math::Matrix::CreateTranslation(hitPos);
					KdEffekseerManager::GetInstance().SetWorldMatrix("Hit3.efk", efcMat);
				}
			}
		}
	}
}

// Grassダッシュに必要な情報をセット
void CharacterBase::SetGrassDashInfo(Math::Vector3 a_dashDir, CharaState a_charaState, std::shared_ptr<KdAnimationData> a_animationInfo, float a_hopperBaseAng,
	int a_weaponNumber, Math::Vector3 a_vec, float a_grassDashAng, float a_grassDashAddAng, std::string a_legPoint, std::string a_effectName)
{
	m_grassHopperDashDir = a_dashDir;
	m_bMove = true;
	m_charaState |= a_charaState;
	m_charaState &= a_charaState;
	m_gravity = 0;
	m_vForce -= m_vForceDownValue;
	m_spAnimator->SetAnimation(a_animationInfo, false);

	const KdModelWork::Node* node = nullptr;
	Math::Matrix mat = Math::Matrix::Identity;

	node = m_spModel->FindNode(a_legPoint);
	mat = node->m_worldTransform * m_mWorld;
	mat._42 += static_cast<float>(m_mpObj["AddBottomYVal"].number_value());
	m_weaponList[a_weaponNumber]->GrassHopper({ mat._41,mat._42,mat._43 }, a_hopperBaseAng);

	KdEffekseerManager::GetInstance().
		Play(a_effectName, { m_pos.x + m_addGrassDashEffectPosVal.x * a_vec.x,m_pos.y + m_addGrassDashEffectPosVal.y,m_pos.z + m_addGrassDashEffectPosVal.z * a_vec.z });
	KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect(a_effectName); // これでループしない
	Math::Matrix efcMat = Math::Matrix::CreateScale(1) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(a_grassDashAng + a_grassDashAddAng)) * Math::Matrix::CreateTranslation({ m_pos.x + m_addGrassDashEffectPosVal.x * a_vec.x,m_pos.y + m_addGrassDashEffectPosVal.y,m_pos.z + m_addGrassDashEffectPosVal.z * a_vec.z });
	KdEffekseerManager::GetInstance().SetWorldMatrix(a_effectName, efcMat);
}

// ステップに必要な情報をセットする
bool CharacterBase::SetStepDashInfo(Math::Vector3 a_dashDir, CharaState a_charaState, std::shared_ptr<KdAnimationData> a_animationInfo)
{
	if (!(m_charaState & (jump | fall)))
	{
		m_stepDashDir = a_dashDir;
		m_bMove = true;
		m_stepCnt = m_mpObj["StepMaxTime"].int_value();
		m_charaState = a_charaState;
		m_spAnimator->SetAnimation(a_animationInfo, false);
		return true;
	}

	return false;
}

void CharacterBase::SetHitMoveInfo(CharaState a_charaState, Math::Vector3 a_knockBackVec, float a_hitMoveSpd, float a_damgage, int a_hitStopCnt, int a_UpdateStopCnt, std::shared_ptr<KdAnimationData> a_animationInfo, float a_firstGraduallyTorionDecVal, float a_graduallyTorionDecVal,int a_invincibilityTimeCnt)
{
	if (m_charaState & mantis)
	{
		WeaponMantisReset();
	}

	m_notUnderAttackTime = m_mpObj["NotUnderAttackMaxTime"].int_value();
	m_charaState = a_charaState;
	m_hitStopCnt = a_hitStopCnt;
	m_hitColorChangeTimeCnt = (*m_wpJsonObj.lock())["HitColorChangeTimeCnt"].int_value();
	m_hitMoveSpd = a_hitMoveSpd;
	m_knockBackVec = a_knockBackVec;
	m_endurance -= a_damgage;
	m_attackHit = true;
	m_spAnimator->SetAnimation(a_animationInfo, false);
	m_invincibilityTimeCnt = a_invincibilityTimeCnt;
	if (SceneManager::Instance().GetUpdateStopCnt() == 0)
	{
		SceneManager::Instance().SetUpdateStopCnt(a_UpdateStopCnt); // これでアップデートを一時止める
	}

	if (m_endurance < 0)
	{
		m_endurance = 0;
	}

	if (m_graduallyTorionDecVal == 0)
	{
		m_graduallyTorionDecVal = a_firstGraduallyTorionDecVal;
	}
	else
	{
		m_graduallyTorionDecVal *= a_graduallyTorionDecVal;
	}
}

// スキンメッシュアニメーションするモデルの影描画
void CharacterBase::GenerateDepthMapFromLight_SkinMesh()
{
	if (m_bLose)return;
	if (!m_spModel)return;

	KdShaderManager::Instance().m_HD2DShader.DrawModel(*m_spModel, m_mWorld);
}

// 自己発光させるモデルの描画
void CharacterBase::DrawBright()
{
	if (m_bLose)return;
	if (!m_bWaponDrawPosible)return;

	for (auto& WeaList : m_weaponList)
	{
		WeaList->DrawBright();
	}
}

// デバックラインの描画
void CharacterBase::DrawDebug()
{
#ifdef _DEBUG
	if (!m_pDebugWire)return;
	m_pDebugWire->Draw();

	for (auto& WeaList : m_weaponList)
	{
		WeaList->DrawDebug();
	}
#endif
}

// 光の影響を受けるモデルの描画
void CharacterBase::DrawLit()
{
	if (m_bLose)return;
	if (!m_bWaponDrawPosible)return;

	for (auto& WeaList : m_weaponList)
	{
		WeaList->DrawLit();
	}
}

// スキンメッシュしないモデルの影描画
void CharacterBase::GenerateDepthMapFromLight()
{
	if (!m_bWaponDrawPosible)return;
	if (m_bLose)return;

	for (auto& WeaList : m_weaponList)
	{
		WeaList->GenerateDepthMapFromLight();
	}
}

void CharacterBase::OverStageChaeck()
{
	float lowestYPos;
	if (SceneManager::Instance().GetSceneType() == SceneManager::SceneType::tutorial || SceneManager::Instance().GetSceneType() == SceneManager::SceneType::training)
	{
		m_vForceDownValue = 0;
		lowestYPos = static_cast<float>((*m_wpJsonObj.lock())["TutorialMinimumYPos"].number_value());
	}
	else
	{
		lowestYPos = static_cast<float>((*m_wpJsonObj.lock())["MinimumYPos"].number_value());
	}

	if (m_pos.x > static_cast<float>((*m_wpJsonObj.lock())["HightXPos"].number_value()) ||
		m_pos.x < static_cast<float>((*m_wpJsonObj.lock())["MinimumXPos"].number_value()) ||
		m_pos.z > static_cast<float>((*m_wpJsonObj.lock())["HightZPos"].number_value()) ||
		m_pos.z < static_cast<float>((*m_wpJsonObj.lock())["MinimumZPos"].number_value()) ||
		m_pos.y < lowestYPos)
	{
		m_overStageTime++;
		int maxOverStageTime = (*m_wpJsonObj.lock())["MaxOverStageTime"].int_value();
		if (m_overStageTime >= maxOverStageTime)
		{
			m_pos = Math::Vector3::Zero;
			m_overStageTime = 0;
		}
	}
	else
	{
		int maxOverStageTime = (*m_wpJsonObj.lock())["MaxOverStageTime"].int_value();
		if (m_overStageTime >= maxOverStageTime)
		{
			m_pos = Math::Vector3::Zero;
			m_overStageTime = 0;
		}

		KdCollider::SphereInfo sphereInfo;
		// 球の中心位置を設定
		sphereInfo.m_sphere.Center = m_pos + m_addCenterVal;
		// 球の半径を設定

		sphereInfo.m_sphere.Radius = static_cast<float>(m_mpObj["DeterminationInsideTheBuildingSphereRadius"].number_value());

		// 当たり判定をしたいタイプを設定
		sphereInfo.m_type = KdCollider::TypeBuried;
#ifdef _DEBUG
		// デバック用
		m_pDebugWire->AddDebugSphere
		(
			sphereInfo.m_sphere.Center,
			sphereInfo.m_sphere.Radius
		);
#endif
		// 球の当たったオブジェクト情報
		std::list<KdCollider::CollisionResult> retSphereList;

		// 球と当たり判定 
		for (auto& obj : SceneManager::Instance().GetObjList())
		{
			obj->Intersects
			(
				sphereInfo,
				&retSphereList
			);
		}

		// 球に当たったリスト情報から一番近いオブジェクトを検出
		float maxOverLap = 0;
		bool hit = false;
		for (auto& ret : retSphereList)
		{
			// 一番近くで当たったものを探す
			if (maxOverLap < ret.m_overlapDistance)
			{
				maxOverLap = ret.m_overlapDistance;
				hit = true;
			}
		}

		if (hit)
		{
			++m_overStageTime;
		}
	}
}

CharacterBase::HitInfo CharacterBase::SphereColisionCheck(Math::Vector3 a_center, float a_radius, KdCollider::Type a_type, std::shared_ptr<WeaponBase> a_weapon)
{
	return SphereColisionCheck(a_center, a_radius, a_type, a_weapon, nullptr, nullptr, nullptr);
}

CharacterBase::HitInfo CharacterBase::SphereColisionCheck(Math::Vector3 a_center, float a_radius, KdCollider::Type a_type, std::shared_ptr<Enemy> a_enemy)
{
	return SphereColisionCheck(a_center, a_radius, a_type, nullptr, a_enemy, nullptr, nullptr);
}

CharacterBase::HitInfo CharacterBase::SphereColisionCheck(Math::Vector3 a_center, float a_radius, KdCollider::Type a_type, std::shared_ptr<Player> a_player)
{
	return SphereColisionCheck(a_center, a_radius, a_type, nullptr, nullptr, a_player, nullptr);
}

CharacterBase::HitInfo CharacterBase::SphereColisionCheck(Math::Vector3 a_center, float a_radius, KdCollider::Type a_type, std::shared_ptr<KdGameObject> a_obj)
{
	return SphereColisionCheck(a_center, a_radius, a_type, nullptr, nullptr, nullptr, a_obj);
}

CharacterBase::HitInfo CharacterBase::SphereColisionCheck(Math::Vector3 a_center, float a_radius, KdCollider::Type a_type)
{
	return SphereColisionCheck(a_center, a_radius, a_type, nullptr, nullptr, nullptr, nullptr);
}

CharacterBase::HitInfo CharacterBase::SphereColisionCheck(Math::Vector3 a_center, float a_radius, KdCollider::Type a_type, std::shared_ptr<WeaponBase> a_weapon, std::shared_ptr<Enemy> a_enemy, std::shared_ptr<Player> a_player, std::shared_ptr<KdGameObject> a_obj)
{
	KdCollider::SphereInfo sphereInfo;
	// 球の中心位置を設定
	sphereInfo.m_sphere.Center = a_center;
	// 球の半径を設定
	sphereInfo.m_sphere.Radius = a_radius;
	// 当たり判定をしたいタイプを設定
	sphereInfo.m_type = a_type;
#ifdef _DEBUG
	// デバック用
	m_pDebugWire->AddDebugSphere
	(
		sphereInfo.m_sphere.Center,
		sphereInfo.m_sphere.Radius
	);
#endif
	// 球の当たったオブジェクト情報
	std::list<KdCollider::CollisionResult> retSphereList;

	if (a_weapon)
	{
		a_weapon->Intersects
		(
			sphereInfo,
			&retSphereList
		);
	}
	else if (a_enemy)
	{
		a_enemy->Intersects
		(
			sphereInfo,
			&retSphereList
		);
	}
	else if (a_player)
	{
		a_player->Intersects
		(
			sphereInfo,
			&retSphereList
		);
	}
	else if (a_obj)
	{
		a_obj->Intersects
		(
			sphereInfo,
			&retSphereList
		);
	}
	else
	{
		// 球と当たり判定 
		for (auto& obj : SceneManager::Instance().GetObjList())
		{
			obj->Intersects
			(
				sphereInfo,
				&retSphereList
			);
		}
	}

	// 球に当たったリスト情報から一番近いオブジェクトを検出
	HitInfo hitInfo;
	hitInfo.maxOverLap = 0;
	hitInfo.hit = false;
	hitInfo.hitDir = {}; // 当たった方向
	for (auto& ret : retSphereList)
	{
		// 一番近くで当たったものを探す
		if (hitInfo.maxOverLap < ret.m_overlapDistance)
		{
			hitInfo.maxOverLap = ret.m_overlapDistance;
			hitInfo.hit = true;
			hitInfo.hitDir = ret.m_hitDir;
		}
	}

	return hitInfo;
}

CharacterBase::HitInfo CharacterBase::RayColisionCheck(Math::Vector3 a_pos, Math::Vector3 a_dir, float a_range,KdCollider::Type a_type, std::shared_ptr<WeaponBase> a_weapon)
{
	return RayColisionCheck(a_pos, a_dir, a_range,a_type, a_weapon, nullptr, nullptr, nullptr);
}

CharacterBase::HitInfo CharacterBase::RayColisionCheck(Math::Vector3 a_pos, Math::Vector3 a_dir, float a_range, KdCollider::Type a_type, std::shared_ptr<Enemy> a_enemy)
{
	return RayColisionCheck(a_pos, a_dir, a_range,a_type, nullptr, a_enemy, nullptr, nullptr);
}

CharacterBase::HitInfo CharacterBase::RayColisionCheck(Math::Vector3 a_pos, Math::Vector3 a_dir, float a_range,KdCollider::Type a_type, std::shared_ptr<Player> a_player)
{
	return RayColisionCheck(a_pos, a_dir, a_range,a_type, nullptr, nullptr, a_player, nullptr);
}

CharacterBase::HitInfo CharacterBase::RayColisionCheck(Math::Vector3 a_pos, Math::Vector3 a_dir, float a_range, KdCollider::Type a_type, std::shared_ptr<KdGameObject> a_obj)
{
	return RayColisionCheck(a_pos, a_dir, a_range, a_type, nullptr, nullptr, nullptr, a_obj);
}

CharacterBase::HitInfo CharacterBase::RayColisionCheck(Math::Vector3 a_pos, Math::Vector3 a_dir, float a_range, KdCollider::Type a_type)
{
	return RayColisionCheck(a_pos, a_dir, a_range, a_type, nullptr, nullptr, nullptr, nullptr);
}

CharacterBase::HitInfo CharacterBase::RayColisionCheck(Math::Vector3 a_pos, Math::Vector3 a_dir, float a_range,KdCollider::Type a_type, std::shared_ptr<WeaponBase> a_weapon, std::shared_ptr<Enemy> a_enemy, std::shared_ptr<Player> a_player, std::shared_ptr<KdGameObject> a_obj)
{
	HitInfo hitInfo;

	KdCollider::RayInfo rayInfo;
	rayInfo.m_pos = a_pos;
	rayInfo.m_dir = a_dir;
	rayInfo.m_range = a_range;
	rayInfo.m_type = a_type;

#ifdef _DEBUG
	m_pDebugWire->AddDebugLine(rayInfo.m_pos, rayInfo.m_dir, rayInfo.m_range, { 1,1,1,1 });
#endif
	std::list<KdCollider::CollisionResult> retRayList;

	if (a_weapon)
	{
		a_weapon->Intersects
		(
			rayInfo,
			&retRayList
		);
	}
	else if (a_enemy)
	{
		a_enemy->Intersects
		(
			rayInfo,
			&retRayList
		);
	}
	else if (a_player)
	{
		a_player->Intersects
		(
			rayInfo,
			&retRayList
		);
	}
	else if (a_obj)
	{
		a_obj->Intersects
		(
			rayInfo,
			&retRayList
		);
	}
	else
	{
		// 球と当たり判定 
		for (auto& obj : SceneManager::Instance().GetObjList())
		{
			obj->Intersects
			(
				rayInfo,
				&retRayList
			);
		}
	}

	// レイに当たったリストから一番近いオブジェクトを検出
	for (auto& ret : retRayList)
	{
		// レイを遮断しオーバーした長さ
		// 一番長いものを探す
		if (hitInfo.maxOverLap < ret.m_overlapDistance)
		{
			hitInfo.maxOverLap = ret.m_overlapDistance;
			hitInfo.groundPos = ret.m_hitPos;
			hitInfo.hit = true;
		}
	}

	return hitInfo;
}

void CharacterBase::EffectTrackingUpdate(std::string a_efectName, float a_scale, float a_rotY, float a_rotX, Math::Vector3 a_pos, Math::Vector3 a_addGrassDashEffectPosVal, Math::Vector3 vec)
{
	Math::Matrix efcMat = Math::Matrix::CreateScale(a_scale) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(a_rotY)) * Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(a_rotX)) * Math::Matrix::CreateTranslation({ a_pos.x + a_addGrassDashEffectPosVal.x * vec.x,a_pos.y + a_addGrassDashEffectPosVal.y,a_pos.z + a_addGrassDashEffectPosVal.z * vec.z });
	KdEffekseerManager::GetInstance().SetWorldMatrix(a_efectName, efcMat);
}

void CharacterBase::SetAttackInfo(CharaState a_nextState, CharaState a_logicalAndState, Math::Vector3 a_attackMoveDir, std::shared_ptr<KdAnimationData> a_animationInfo, float a_attackMoveSpd)
{
	m_charaState |= a_nextState;
	m_charaState &= a_logicalAndState;
	m_bAttackAnimeDelay = false;
	m_attackAnimeCnt = 0;
	m_attackAnimeDelayCnt = 0;
	m_bMove = true;
	m_attackMoveDir = a_attackMoveDir;

	m_attackMoveDir.y = 0;
	m_attackMoveDir.Normalize();
	m_attackMoveSpd = a_attackMoveSpd;
	m_spAnimator->SetAnimation(a_animationInfo, false);
}

bool CharacterBase::PanchiAttackHitChaeck(std::shared_ptr<CharacterBase> a_target,std::string a_hitPointName, float a_addBottomYVal, float a_radius, KdCollider::Type a_type, CharacterBase* a_pAttackedCharacter)
{
	if (!a_target->GetAttackHit() && !a_target->GetDefenseSuc() && a_target->GetInvincibilityTimeCnt() == 0 && !a_target->GetBDeath()) // ここにはなくてもいいかも
	{
		const KdModelWork::Node* node = nullptr;
		Math::Matrix mat = Math::Matrix::Identity;

		node = m_spModel->FindNode(a_hitPointName);
		KdCollider::SphereInfo sphereInfo;
		mat = node->m_worldTransform * m_mWorld;
		mat._42 += a_addBottomYVal;
		sphereInfo.m_sphere.Center = mat.Translation();
		sphereInfo.m_sphere.Radius = a_radius;
		sphereInfo.m_type = a_type;

#ifdef _DEBUG
		m_pDebugWire->AddDebugSphere
		(
			sphereInfo.m_sphere.Center,
			sphereInfo.m_sphere.Radius,
			{ 0,0,0,1 }
		);
#endif

		std::list<KdCollider::CollisionResult> retSphereList;

		a_target->Intersects
		(
			sphereInfo,
			&retSphereList
		);

		Math::Vector3 hitDir = {};
		bool hit = false;
		Math::Vector3 hitPos = {};
		for (auto& ret : retSphereList)
		{
			hit = true;
			hitDir = ret.m_hitDir;
			hitPos = ret.m_hitPos;
		}

		if (hit)
		{
			a_target->BlowingAwayAttackOnHit(m_mWorld.Backward(), a_pAttackedCharacter);
			KdAudioManager::Instance().Play("Asset/Audio/SE/KickAttackHit.wav");
			hitPos.y += 0.35f;
			KdEffekseerManager::GetInstance().
				Play("Hit3.efk", hitPos);
			KdEffekseerManager::GetInstance().KdEffekseerManager::StopEffect("Hit3.efk"); // これでループしない
			Math::Matrix efcMat = Math::Matrix::CreateScale(0.5f) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_mWorldRot.y)) * Math::Matrix::CreateTranslation(hitPos);
			KdEffekseerManager::GetInstance().SetWorldMatrix("Hit3.efk", efcMat);
			return true;
		}
	}

	return false;
}

void CharacterBase::GrassDashAfterAnimationChange()
{
	if (m_charaState & grassHopperDashF)
	{
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("GrassDashFA"), true);
	}
	else if (m_charaState & grassHopperDashR)
	{
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("GrassDashRA"), true);
	}
	else if (m_charaState & grassHopperDashL)
	{
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("GrassDashLA"), true);
	}
	else if (m_charaState & grassHopperDashB)
	{
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("GrassDashBA"), true);
	}
	else if (m_charaState & grassHopperDashUp)
	{
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("GrassDashJA"), true);
	}
}

void CharacterBase::AttackInfoReset(CharaState a_logicalAndState)
{
	m_charaState &= ~a_logicalAndState;
	m_bAttackAnimeDelay = false;
	m_attackAnimeCnt = 0;
	m_attackAnimeDelayCnt = 0;
	m_bMove = false;
}

void CharacterBase::WeaponMantisReset()
{
	const std::shared_ptr<Scopion> scopion = std::dynamic_pointer_cast<Scopion>(m_weaponList[1]);
	scopion->SetBMantis(false);
	const std::shared_ptr<Scopion> scopion2 = std::dynamic_pointer_cast<Scopion>(m_weaponList[0]);
	scopion2->SetBMantis(false);
}

bool CharacterBase::HitOverRapComparison(float& a_compareFromLen, float a_compareWithLen, Math::Vector3& a_compareFromDir, Math::Vector3 a_compareWithDir)
{
	if (a_compareFromLen < a_compareWithLen)
	{
		a_compareFromLen = a_compareWithLen;
		a_compareFromDir = a_compareWithDir;
		return true;
	}

	return false;
}
