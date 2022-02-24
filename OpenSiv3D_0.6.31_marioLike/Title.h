#pragma once
#include "common.h"

class Title : public App::Scene
{
public:

	Title(const InitData& init);

	void update() override;

	void draw() const override;

private:
	Vec2 start{ -1024,-512 };
	Vec2 goal{ 478,100 };
	Stopwatch stopwatch{ StartImmediately::Yes };
	bool selectFlag=false;
	Vec2 selectPos;
	int stageNum = 0;
};



