#pragma once
#include "../BaseScene/BaseScene.h"
class Player;
class Ui;

class TutorialScene :public BaseScene
{
public:
	TutorialScene() { Init(); }
	~TutorialScene() {}

private:

	void Event()override;
	void Init()override;

	std::weak_ptr<Player> m_wpPlayer;
	std::weak_ptr<Ui> m_wpUi;
};