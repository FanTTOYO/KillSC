#pragma once
#define GRASS_HOPPER_PAUCNT 30  // グラスホッパーが使えない時間
#include "../CharacterBase.h"

class CameraBase;
class WeaponBase;
class Enemy;
class Ui;

class Player : public CharacterBase
{
public:
	Player() {}
	~Player() {}

	void Update()								override;
	void PostUpdate()							override;
	void DrawUnLit()							override;
	void DrawLit_SkinMesh()						override;
	void Init(std::weak_ptr<json11::Json> a_wpJsonObj);

	void SetCamera(std::shared_ptr<CameraBase> a_spCcamera) { m_wpCamera = a_spCcamera; }
	void AddEnemy(std::shared_ptr<Enemy> a_spEnemy) { m_enemyList.push_back(a_spEnemy); }
	void AddWeaponToEnemy(std::shared_ptr<Enemy> a_spEnemy);
	void SetUi(std::shared_ptr<Ui> a_spUi) { m_wpUi = a_spUi; }

	void OnHit(Math::Vector3 a_KnocBackvec, CharacterBase* a_pAttackedCharacter)override;																	// 通常攻撃が当たった
	void BlowingAwayAttackOnHit(Math::Vector3 a_KnocBackvec, CharacterBase* a_pAttackedCharacter)override;													// 吹き飛ばし攻撃が当たった
	void IaiKiriAttackOnHit(Math::Vector3 a_KnocBackvec)override;														// 居合切り系統の攻撃が当たった
	void CutRaiseOnHit(Math::Vector3 a_KnocBackvec)override;															// 切り上げ攻撃が当たった
	void HasDefense()override;																							// 攻撃が防御された																													

	const bool GetBRushRp() { return m_bRushRp; }

private:

	void WeaponChangeProcess();																						// 武器切替の処理
	void AnimationUpdate();																							// アニメーションの更新
	void PlayerPanchiHitAttackChaeck();																				// プレイヤーのパンチ当たり判定
	void UpdateRotate(Math::Vector3& a_srcMoveVec);																	// プレイヤーの回転を更新
	void GrassMoveVecDecision();																					// ホッパーで進む方向を決める
	void ScorpionActionDecision();																					// 短剣で何をするかを決める
	void GrassMove();																								// 高速移動時の動きを扱う
	void StepMove();																								// ステップ時の動きを扱う			
	bool StepMoveVecDecision();																				    	// ステップで進む方向を決める
	Math::Vector3 RunMoveVecDecsion();		                                                                        // RUNで進むする方向を決める
	void NormalMove();																								// 歩きなどの動きを扱う
	void ScorpionAttackMove();																						// 短剣での攻撃を扱う
	void ScorpionAttackEndProcess();																				// 短剣での攻撃終了処理
	void ScorpionAttackAnimationMoveProcess();																		// 短剣での攻撃アニメーション中の処理
	void ScorpionDefenseMove();																						// 短剣での防御を扱う
	void HasDefenseMove();																							// 防御された時の動きを扱う
	void TutorialUpdate();																							// チュートリアル時のみ行う処理
	void EnemyRockOn();																								// 敵をロックオンする処理
	void HitStateUpdate();																							// Hit状態の時の処理
	void SpeedyMoveWallHitChack(float& a_moveSpd, Math::Vector3 moveVec);											// 速い動きの時の壁との当たり判定
	void RayCollisionUpdate();																						// レイ判定処理
	void SphereCollisionUpdate();																					// 球形判定処理
	void SetDefenseSucAndAttackHit();
	void CntDown();
	void FirstAttackDecision(std::shared_ptr<CameraBase> a_spCamera);
	void FirstAfterAttackDecision(std::shared_ptr<CameraBase> a_spCamera);
	void EnduranceAndVForthManagement();

	int m_maxWeaponType = 2;																						// 武器の最大のナンバー
	const int FIRST_WEAPON_TYPE_NUMBER = 1;																			// 武器の最初のナンバー

	std::weak_ptr<CameraBase> m_wpCamera;																			// カメラのアドレスを保持

	Math::Matrix m_grasRotYMat;																						// 高速移動する方向

	std::weak_ptr<Enemy> m_wpEnemy;																					// ロックオンしてる敵
	std::list<std::weak_ptr<Enemy>> m_enemyList;																	// 敵のリスト

	std::weak_ptr<Ui> m_wpUi;																						// UIクラスの情報を持ってる
	int m_tuGardTime;																								// チュートリアルの際の防御してる時間

	KdSquarePolygon m_rocKOnPolygon;																				// ロックオン用ポリゴン
	Math::Matrix m_rockOnPolyMat;																					// ロックオン用行列

	bool m_bRushRp;																									// 回転ラッシュ攻撃 true	

	bool m_bAtttackMoveSpeedDec;																					// 減速させる : true
};