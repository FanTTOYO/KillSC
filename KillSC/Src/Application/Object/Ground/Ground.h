#pragma once

class Ground :public KdGameObject
{
public:
	Ground() {}
	~Ground() { Release(); }

	enum groundType
	{
		trainingRoom,
		street,
	};

	void DrawLit()override;
	void DrawUnLit()override;
	void GenerateDepthMapFromLight()override;
	void DrawBright()override;
	void Init(groundType a_groundType);
	void Release(){};
	//std::shared_ptr<KdModelData> GetModel() { return m_model; }

private:
	groundType m_groundType;
	std::shared_ptr<KdModelData> m_model;
};