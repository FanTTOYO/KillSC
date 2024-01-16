#pragma once

class KdCollisionShape;

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 当たり判定を内部で実行し判定結果を返してくれるクラス
// 当たり判定を受けたいゲーム内のオブジェクトにメンバーとして持たせる　※当てる側ではなく当てられる側に持たせる
// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// 運用には形状登録・判定実行の2つの手順が必要
// 形状登録：RegisterColisionSphape()を使って当たり判定の形状を登録しておく。CollisionShapeは形状と衝突タイプ（用途）が必要
// 判定実行：Intersects()で当たり判定を実行する。詳細な結果が欲しい場合にはResultを引数としてセットする事
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
class KdCollider
{
public:

	// 衝突タイプBitフラグ：用途によって使い分ける・Bitフラグなので複数のタイプを付与可能
	enum Type
	{
		TypeGround		 = 1 << 0,	// 地形：上に乗れるオブジェクトに対して付与する
		TypeBump		 = 1 << 1,	// 衝突：横方向で重なりを防止したいオブジェクトに対して付与する
		TypeDamage		 = 1 << 2,	// 攻撃：球形の攻撃判定を受けるオブジェクトに対して付与する
		TypeDamageLine	 = 1 << 3,	// 攻撃：線形の攻撃判定を受けるオブジェクトに対して付与する
		TypeSight		 = 1 << 4,	// 視界：敵がプレイヤーを発見するかどうかなど視界判定を受けるオブジェクトに付与する
		TypeEvent		 = 1 << 5,	// イベント：イベント特有の判定形状が欲しい場合にイベントを所有しているオブジェクトに付与する
		TypeGard         = 1 << 6,  // 防御：球形の防御当たり判定に使用
		TypeBuried       = 1 << 7,  // ビルに埋もれたときに当たる当たり判定
		TypeWeakness     = 1 << 8,  // 弱点 : 通常のダメージ当たり判定より大ダメージを与えられる場所
		TypeRideEnemy    = 1 << 9,  // 乗れる敵 : 上に乗ることが出来る敵オブジェクトに付与する
		TypeAttackDec	 = 1 << 10, // 減速 : 近づくと攻撃移動時の移動速度が急速に落ちるオブジェクトに付与する
		TypeSpeedDec	 = 1 << 11, // 停止 : 近づくと勢いが止まるオブジェクトに付与する
	};

	// 球形の当たり判定情報：当たる側専用
	struct SphereInfo
	{
		SphereInfo() {}

		// BoundingSphereを直接指定
		SphereInfo(UINT type, const DirectX::BoundingSphere sphere)
			: m_type(type), m_sphere(sphere) {}

		// 座標と半径からBoundingSphereを指定
		SphereInfo(UINT type, const Math::Vector3& pos, float radius)
			: m_type(type)
		{
			m_sphere.Center = pos;
			m_sphere.Radius = radius;
		}

		DirectX::BoundingSphere m_sphere;

		UINT m_type = 0;
	};

	// レイの当たり判定情報：当たる側専用
	struct RayInfo
	{
		RayInfo() {}

		// レイの情報を全て指定：自動的に方向ベクトルは正規化
		RayInfo(UINT type, const Math::Vector3& pos, const Math::Vector3& dir, float range) 
			: m_type(type), m_pos(pos), m_dir(dir),m_range(range)
		{
			m_dir.Normalize();
		}

		// 開始地点と終了地点からレイの情報を作成：自動的に方向ベクトルは正規化
		RayInfo(UINT type, const Math::Vector3& start, const Math::Vector3& end)
			: m_type(type), m_pos(start)
		{
			m_dir = end - start;
			m_range = m_dir.Length();
			m_dir.Normalize();
		}

		Math::Vector3 m_pos;		// レイの発射位置
		Math::Vector3 m_dir;		// レイの方向
		float m_range = 0;			// 判定限界距離

		UINT m_type = 0;
	};


	// 詳細な衝突結果
	struct CollisionResult
	{
		Math::Vector3 m_hitPos;			// 衝突した座標
		Math::Vector3 m_hitDir;			// 対象からの方向ベクトル（押し返しなどに使う
		float m_overlapDistance = 0.0f; // 重なり量
	};

	KdCollider() {}
	
	~KdCollider() {}

	// 当たり判定形状形状登録
	void RegisterCollisionShape(std::string_view name, std::unique_ptr<KdCollisionShape> spShape);
	void RegisterCollisionShape(std::string_view name, const DirectX::BoundingSphere& sphere, UINT type);
	void RegisterCollisionShape(std::string_view name, const Math::Vector3& localPos, float radius, UINT type);
	void RegisterCollisionShape(std::string_view name, const std::shared_ptr<KdModelData>& model, UINT type);
	void RegisterCollisionShape(std::string_view name, KdModelData* model, UINT type);
	void RegisterCollisionShape(std::string_view name, const std::shared_ptr<KdModelWork>& model, UINT type);
	void RegisterCollisionShape(std::string_view name, KdModelWork* model, UINT type);
	void RegisterCollisionShape(std::string_view name, const std::shared_ptr<KdPolygon> polygon, UINT type);
	void RegisterCollisionShape(std::string_view name, KdPolygon* polygon, UINT type);

	// 当たり判定実行
	bool Intersects(const SphereInfo& targetShape, const Math::Matrix& ownerMatrix, std::list<KdCollider::CollisionResult>* pResults) const;
	bool Intersects(const RayInfo& targetShape, const Math::Matrix& ownerMatrix, std::list<KdCollider::CollisionResult>* pResults) const;

	// 登録した当たり判定の有効/無効の設定
	void SetEnable(std::string_view name, bool flag);
	void SetEnable(int type, bool flag);
	void SetEnableAll(bool flag);

private:
	std::unordered_map<std::string, std::unique_ptr<KdCollisionShape>> m_collisionShapes;

	int m_disableType = 0;
};


// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// vs球とvsレイの判定を持つ何かしらの形状の基底クラス
// 当たり判定をする用途（type）と球・レイの当たり判定用インターフェースを持つ
// 継承先では任意の形状をメンバーに追加し、その形状とvs球とvsレイ当たり判定関数を作成する
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
class KdCollisionShape
{
public:

	KdCollisionShape(UINT type) { m_type = type; }

	virtual ~KdCollisionShape() {}

	UINT GetType() const { return m_type; }

	virtual bool Intersects(const DirectX::BoundingSphere& target, const Math::Matrix& world, KdCollider::CollisionResult* pRes) = 0;
	virtual bool Intersects(const KdCollider::RayInfo& target, const Math::Matrix& world, KdCollider::CollisionResult* pRes) = 0;

	void SetEnable(bool flag) { m_enable = flag; }

protected:
	bool m_enable = true;

private:
	UINT m_type = 0;		// 衝突タイプ:何用の当たり判定か
};


// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// コライダー：球形状
// 球形状vs特定形状（球・レイ）の当たり判定実行クラス
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
class KdSphereCollision : public KdCollisionShape
{
public:
	KdSphereCollision(const DirectX::BoundingSphere& sphere, UINT type) :
		KdCollisionShape(type), m_shape(sphere) {}
	KdSphereCollision(const Math::Vector3& localPos, float radius, UINT type) :
		KdCollisionShape(type) { m_shape.Center = localPos; m_shape.Radius = radius; }

	virtual ~KdSphereCollision() {}

	bool Intersects(const DirectX::BoundingSphere& target, const Math::Matrix& world, KdCollider::CollisionResult* pRes) override;
	bool Intersects(const KdCollider::RayInfo& target, const Math::Matrix& world, KdCollider::CollisionResult* pRes) override;

private:
	DirectX::BoundingSphere m_shape;
};


// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// コライダー：モデル形状(dynamicAnimationModelWork)
// モデル形状vs特定形状（球・レイ）の当たり判定実行クラス
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
class KdModelCollision : public KdCollisionShape
{
public:
	KdModelCollision(const std::shared_ptr<KdModelData>& model, UINT type) :
		KdCollisionShape(type), m_shape(std::make_shared<KdModelWork>(model)) {}
	KdModelCollision(const std::shared_ptr<KdModelWork>& model, UINT type) :
		KdCollisionShape(type), m_shape(model) {}

	virtual ~KdModelCollision() { m_shape.reset(); }

	bool Intersects(const DirectX::BoundingSphere& target, const Math::Matrix& world, KdCollider::CollisionResult* pRes) override;
	bool Intersects(const KdCollider::RayInfo& target, const Math::Matrix& world, KdCollider::CollisionResult* pRes) override;

private:
	std::shared_ptr<KdModelWork> m_shape;
};


// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// コライダー：ポリゴン形状
// ポリゴン形状vs特定形状（球・レイ）の当たり判定実行クラス
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
class KdPolygonCollision : public KdCollisionShape
{
public:
	KdPolygonCollision(const std::shared_ptr<KdPolygon>& polygon, UINT type) :
		KdCollisionShape(type), m_shape(polygon) {}

	virtual ~KdPolygonCollision() { m_shape.reset(); }

	bool Intersects(const DirectX::BoundingSphere& target, const Math::Matrix& world, KdCollider::CollisionResult* pRes) override;
	bool Intersects(const KdCollider::RayInfo& target, const Math::Matrix& world, KdCollider::CollisionResult* pRes) override;

private:
	std::shared_ptr<KdPolygon> m_shape;
};