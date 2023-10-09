#pragma once
class Ground;

class Bldg : public KdGameObject
{
public:
	Bldg() { Init(); }
	~Bldg() { Release(); }

	void DrawLit()override;
	void GenerateDepthMapFromLight()override;
	void Init()override;
	void Update();
	void Release(){};

	void CreateBldg(std::string a_CollName , Math::Vector3 a_pos ,std::string a_fileName ,Math::Vector3 a_scale = Math::Vector3::One);

private:
	std::shared_ptr<KdModelWork> m_pModel;
};