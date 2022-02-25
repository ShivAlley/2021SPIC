#include "stdafx.h"
#include "Gameover.h"

Gameover::Gameover(const InitData& init)
	:IScene(init)
{

}

void Gameover::update()
{
	if (Scene::Time() > 30)
	{
		changeScene(GameState::Title);
	}
}

void Gameover::draw() const
{
}
