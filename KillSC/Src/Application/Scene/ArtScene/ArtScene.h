#pragma once
#include "../BaseScene/BaseScene.h"
class Ui;

class ArtScene : public BaseScene
{
public:
	ArtScene() { Init(); }
	~ArtScene(){};

private:
	void Event()override;
	void Init()override;
	std::weak_ptr<Ui> m_wpUi;
};