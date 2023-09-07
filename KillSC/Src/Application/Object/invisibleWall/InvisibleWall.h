#pragma once

class InvisibleWall :public KdGameObject
{
public:
	InvisibleWall() { Init(); }
	~InvisibleWall() { Release(); }

	void Init()override;
	void Release(){};

private:
	
	std::shared_ptr<KdModelWork> m_model;
};