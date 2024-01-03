#pragma once
#include "../BaseScene/BaseScene.h"
class Ui;

class SelectScene : public BaseScene
{
public:
	SelectScene() { Init(); }
	~SelectScene(){};

private:
	void Event()override;
	void Init()override;
	std::weak_ptr<Ui> m_wpUi;
	std::shared_ptr<json11::Json> m_spJsonObj;
};