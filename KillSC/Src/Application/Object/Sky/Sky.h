#pragma once

class Sky :public KdGameObject
{
public:
	Sky() {Init();}
	~Sky() {}

	void Update()override;
	void Init()override;
	void DrawUnLit()override;
	void Release();

private:
	std::shared_ptr<KdModelWork> m_model;
};