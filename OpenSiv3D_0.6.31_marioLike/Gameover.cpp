#include "stdafx.h"
#include "Gameover.h"

Gameover::Gameover(const InitData& init)
	:IScene(init)
{
	getData().select = 0;
	AudioAsset(U"gameover").setVolume(0.7);
	AudioAsset(U"gameover").play();
}

void Gameover::update()
{
	if (getData().select < 1 && KeyS.up() || KeyDown.up())getData().select++;
	if (getData().select > 0 && KeyW.up() || KeyUp.up())getData().select--;
	if (KeyEnter.pressed())
	{
		if(getData().select==0){
			getData().Life = 3;
			AudioAsset(U"gameover").stop();
			changeScene(GameState::Game);
		}
		if (getData().select == 1) {
			AudioAsset(U"gameover").stop();
			changeScene(GameState::Title);
		}
	}
}

void Gameover::draw() const
{
	TextureAsset(U"back")
		.scaled(8.0, 4.3)
		.draw(0, 0);

	TextureAsset(U"over")
		.scaled(1, 1)
		.drawAt(Scene::Center().x, 400);

	FontAsset(U"Title")(U"Retry")
		.drawAt(Scene::Center().x, 700, Palette::Orange);
	FontAsset(U"Title")(U"Title")
		.drawAt(Scene::Center().x, 800, Palette::Orange);

	switch (getData().select)
	{
	case 0:
	{
		TextureAsset(U"select")(0, 0, 248)
			.scaled(0.3, 0.3)
			.drawAt(770, 700);
		break;
	}
	case 1:
	{
		TextureAsset(U"select")(0, 0, 248)
			.scaled(0.3, 0.3)
			.drawAt(770, 800);
		break;
	}
	}
}
