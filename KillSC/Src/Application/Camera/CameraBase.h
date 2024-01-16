#pragma once

class CameraBase : public KdGameObject
{
public:
	CameraBase(){}
	virtual ~CameraBase()	override	{}

	void Init()				override;
	void Update()			override;
	void PreDraw()			override;

	void SetTarget(const std::shared_ptr<KdGameObject>& target);

	void SetStepOnPlayerPos(Math::Vector3 a_stepOnPlayerPos) { m_stepOnPlayerPos = a_stepOnPlayerPos; }

	const std::shared_ptr<const KdCamera>& GetCamera() const // 見るだけ（変更できない）
	{
		if (!m_spCamera) { return nullptr; }
		return m_spCamera;
	}

	std::shared_ptr<KdCamera> WorkCamera() const            // m_spCameraの何かを変更したいとき
	{
		if (!m_spCamera) { return nullptr; }
		return m_spCamera;
	}

	const Math::Matrix GetRotationMatrix() const
	{
		                            // Yaw Y軸回転 Pitch X軸回転 Roll Z軸回転 
		return Math::Matrix::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(m_DegAng.y),
			                                        DirectX::XMConvertToRadians(m_DegAng.x),
			                                        DirectX::XMConvertToRadians(m_DegAng.z));
	}

	const Math::Matrix GetRotationYMatrix() const
	{
		return Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_DegAng.y));
	}

	/*virtual void SetBCameraSet(Math::Vector3 _mobeVec = {}) {}	*/

	const float GetYAng() const
	{
		return m_DegAng.y;
	}

protected:
	std::shared_ptr<KdCamera>			m_spCamera = nullptr;
	std::weak_ptr<KdGameObject>			m_wpTarget;

	Math::Matrix						m_LocalPos;
	Math::Matrix						m_Rotation;

	// カメラ回転用角度
	Math::Vector3                       m_DegAng;

	Math::Vector3 m_stepOnPlayerPos;
};