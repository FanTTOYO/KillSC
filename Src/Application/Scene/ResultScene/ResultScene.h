#pragma once
#include "../BaseScene/BaseScene.h"
class Ui;

class ResultScene : public BaseScene
{
public:
	ResultScene() { Init(); }
	~ResultScene(){};

private:
	void Event()override;
	void Init()override;
	std::weak_ptr<Ui> m_wpUi;
};