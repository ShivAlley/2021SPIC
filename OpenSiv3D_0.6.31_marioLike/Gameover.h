#pragma once
#include "common.h"
class Gameover : public App::Scene
{
public:

	Gameover(const InitData& init);

	void update() override;

	void draw() const override;

private:
};

