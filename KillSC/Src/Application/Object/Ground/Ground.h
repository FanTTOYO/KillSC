#pragma once

class Ground :public KdGameObject
{
public:
	Ground() { Init(); }
	~Ground() { Release(); }

	void DrawLit()override;
	void GenerateDepthMapFromLight()override;
	void DrawBright()override;
	void Init()override;
	void Release(){};
	std::shared_ptr<KdModelWork> GetModel() { return m_model; }

private:
	
	std::shared_ptr<KdModelWork> m_model;
};