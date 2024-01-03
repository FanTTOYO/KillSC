#pragma once
#include "../BaseScene/BaseScene.h"
class Ui;

class TitleScene : public BaseScene
{
public:
	TitleScene() { Init(); }
	~TitleScene(){};

private:
	void Event()override;
	void Init()override;
	std::weak_ptr<Ui> m_wpUi;
	bool m_bReturn = false;
	std::shared_ptr<json11::Json> m_spJsonObj;
};