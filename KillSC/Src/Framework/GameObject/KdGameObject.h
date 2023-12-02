#pragma once

class KdDebugWireFrame;

// ゲーム上に存在するすべてのオブジェクトの基底となるクラス
class KdGameObject : public std::enable_shared_from_this<KdGameObject>
{
public:

	// どのような描画を行うのかを設定するTypeID：Bitフラグで複数指定可能
	enum
	{
		eDrawTypeLit = 1 << 0,
		eDrawTypeUnLit = 1 << 1,
		eDrawTypeBright = 1 << 2,
		eDrawTypeUI = 1 << 3,
		eDrawTypeDepthOfShadow = 1 << 4,
	};

	KdGameObject() {}
	virtual ~KdGameObject() { Release(); }

	// 生成される全てに共通するパラメータに対する初期化のみ
	virtual void Init() {}

	virtual void PreUpdate() {}
	virtual void Update() {}
	virtual void PostUpdate() {}

	// それぞれの状況で描画する関数
	virtual void PreDraw() {}
	virtual void GenerateDepthMapFromLight() {}
	virtual void GenerateDepthMapFromLight_SkinMesh() {}
	virtual void DrawLit() {}
	virtual void DrawLit_SkinMesh() {}
	virtual void DrawUnLit() {}
	virtual void DrawBright() {}
	virtual void DrawSprite() {}
	virtual void DrawDebug();

	virtual void SetAsset(const std::string&) {}

	virtual void SetPos(const Math::Vector3& pos) { m_mWorld.Translation(pos); }
	virtual Math::Vector3 GetPos() const { return m_mWorld.Translation(); }

	// 拡大率を変更する関数
	void SetScale(float scalar);
	virtual void SetScale(const Math::Vector3& scale);
	virtual Math::Vector3 GetScale() const;

	const Math::Matrix& GetMatrix() const { return m_mWorld; }

	virtual bool IsExpired() const { return m_isExpired; }

	virtual bool IsVisible() const { return false; }

	// 視錐台範囲内に入っているかどうか
	virtual bool CheckInScreen(const DirectX::BoundingFrustum&) const { return false; }

	// カメラからの距離を計算
	virtual void CalcDistSqrFromCamera(const Math::Vector3& camPos);

	float GetDistSqrFromCamera() const { return m_distSqrFromCamera; }

	UINT GetDrawType() const { return m_drawType; }

	bool Intersects(const KdCollider::SphereInfo& targetShape, std::list<KdCollider::CollisionResult>* pResults);
	bool Intersects(const KdCollider::RayInfo& targetShape, std::list<KdCollider::CollisionResult>* pResults);

	virtual void OnHit(Math::Vector3 a_KnocBackvec) {}                         // 通常の当たり判定
	virtual void BlowingAwayAttackOnHit(Math::Vector3 a_KnocBackvec) {}        // 吹き飛ばし攻撃当たり判定
	virtual void IaiKiriAttackOnHit(Math::Vector3 a_KnocBackvec) {}            // 居合切り当たり判定

	virtual void CutRaiseOnHit(Math::Vector3 a_KnocBackvec) {}                 // 切り上げ攻撃当たり判定

	virtual void HasDefense() {}

	void SetAttackHit(bool a_attackHit) { m_attackHit = a_attackHit; }
	const bool GetAttackHit() { return m_attackHit; }

	void SetDefenseSuc(bool a_defenseSuc) { m_bDefenseSuc = a_defenseSuc; }
	bool GetDefenseSuc() { return m_bDefenseSuc; }

protected:

	void Release() {}

	// 描画タイプ・何の描画を行うのかを決める / 最適な描画リスト作成用
	UINT m_drawType = 0;

	// カメラからの距離
	float m_distSqrFromCamera = 0;

	// 存在消滅フラグ
	bool m_isExpired = false;

	// 3D空間に存在する機能
	Math::Matrix	m_mWorld;

	// 当たり判定クラス
	std::unique_ptr<KdCollider> m_pCollider = nullptr;
	std::unique_ptr<KdDebugWireFrame> m_pDebugWire = nullptr;

	bool m_attackHit; // 一つの攻撃が複数回当たらないためのフラグ
	bool m_bDefenseSuc; // 防御に成功したフラグ
};