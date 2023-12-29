#pragma once

class KdEffekseerObject;
class CameraBase;

class KdEffekseerManager
{
public:

	void Create(int w, int h);

	void Update();
	void Draw();

	struct PlayEfkInfo
	{
		std::string FileName = "";
		Math::Vector3 Pos = {};
		float Size = 1.0f;
		float Speed = 1.0f;
		bool IsLoop = false;
	};

	// Jsonファイルからデータを取得し、Effekseerエフェクトを再生する
	// ※ キャラクターの位置にエフェクトを再生する場合などがあるため、座標情報を渡している
	std::shared_ptr<KdEffekseerObject> Play(const std::string& effName, const DirectX::SimpleMath::Vector3& pos);

	void StopAllEffect();
	void StopEffect(const std::string& name);

	void OnPauseEfkUpdate()
	{
		m_isPause = true;
	}

	void OnResumeEfkUpdate()
	{
		m_isPause = false;
	}

	void Release();
	void Reset();

	void SetPos(const std::string& name, const Math::Vector3& pos);
	void SetRotation(const std::string& name, const Math::Vector3& axis, const float angle);
	void SetWorldMatrix(const std::string& name, const Math::Matrix& mWorld);

	void SetScale(const std::string& name, const Math::Vector3& scale);
	void SetScale(const std::string& name, const float scale);

	void SetSpeed(const std::string& name, const float speed = 1.0f);

	void SetPause(const std::string& name, const bool isPause);

	// 再生中かどうか
	const bool IsPlaying(const std::string& name) const;

	void SetCamera(const std::shared_ptr<CameraBase>& camera);

	void EffectListClear();

private:

	const Effekseer::Vector3D& GetEfkVec3D(const Math::Vector3& vec) const
	{
		return Effekseer::Vector3D(vec.x, vec.y, vec.z);
	}

	// Effekseerエフェクトの再生
	std::shared_ptr<KdEffekseerObject> Play(const PlayEfkInfo& info);
	std::shared_ptr<KdEffekseerObject> Play(const std::shared_ptr<KdEffekseerObject> spObject);

	void UpdateEffekseerEffect();
	void UpdateEkfCameraMatrix();

	// エフェクトのレンダラー
	EffekseerRendererDX11::RendererRef m_efkRenderer = nullptr;

	// エフェクトのマネージャー
	Effekseer::ManagerRef m_efkManager = nullptr;

	// エフェクトリスト
	std::unordered_map<std::string, std::shared_ptr<KdEffekseerObject>> m_effectMap;

	// カメラ
	std::weak_ptr<CameraBase> m_wpCamera;

	bool m_isPause = false;

public:

	static KdEffekseerManager& GetInstance()
	{
		static KdEffekseerManager instance;

		return instance;
	}

private:

	KdEffekseerManager() {}
	~KdEffekseerManager() { Release(); }
};

class KdEffekseerObject
{
public:

	// 再生中か
	bool IsPlaying();

	// 再生登録したマネージャー設定
	void SetParentManager(Effekseer::ManagerRef& parentManager)
	{
		m_parentManager = parentManager;
	}

	// 再生するエフェクト設定
	void SetEffect(Effekseer::EffectRef& effect) { m_effect = effect; }

	// ハンドル設定
	void SetHandle(Effekseer::Handle& handle) { m_handle = handle; }

	// 座標設定
	void SetPos(const Math::Vector3& pos);

	// サイズ設定
	void SetScale(const float scale = 1.0f);

	// 速度設定
	void SetSpeed(const float speed = 1.0f);

	// ワールド行列設定
	void SetWorldMatrix(const Math::Matrix& mWorld);

	// ループ設定
	void SetLoop(const bool isLoop = false) { m_info.IsLoop = isLoop; }

	// 再生するエフェクトの各種情報設定
	void SetPlayEfkInfo(const KdEffekseerManager::PlayEfkInfo& info) { m_info = info; }

	// エフェクト取得
	const Effekseer::EffectRef& GetEffect() const { return m_effect; }

	// ハンドル取得
	const Effekseer::Handle& GetHandle() const { return m_handle; }

	// 座標取得
	const Math::Vector3 GetPos() { return m_info.Pos; }

	// サイズ取得
	const float GetSize() const { return m_info.Size; }

	// 速度取得
	const float GetSpeed() const { return m_info.Speed; }

	// ループするかどうか
	const bool IsLoop() const { return m_info.IsLoop; }

	// エフェクトの各種情報取得
	KdEffekseerManager::PlayEfkInfo& WorkPlayEfkInfo() { return m_info; }
	const KdEffekseerManager::PlayEfkInfo& GetPlayEfkInfo() const { return m_info; }

private:

	Effekseer::ManagerRef				m_parentManager = nullptr;
	Effekseer::EffectRef				m_effect = nullptr;
	Effekseer::Handle					m_handle = -1;

	KdEffekseerManager::PlayEfkInfo		m_info = {};
};

#define EffekseerPath "Asset/Data/Sample/Effekseer/"