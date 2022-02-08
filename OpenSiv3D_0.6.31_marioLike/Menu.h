#pragma once
#include "common.h"
class Menu : public App::Scene
{
public:

	Menu(const InitData& init);

	void update() override;

	void draw() const override;

private:
};

