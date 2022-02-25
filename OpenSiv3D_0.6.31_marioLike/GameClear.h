#pragma once
#include "common.h"
class GameClear : public App::Scene
{
public:

	GameClear(const InitData& init);

	void update() override;

	void draw() const override;

private:
};

