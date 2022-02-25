#include "stdafx.h"
#include "GameClear.h"

GameClear::GameClear(const InitData& init)
	:IScene(init)
{
	getData().select = 0;
	AudioAsset(U"gameClear").setVolume(0.7);
	AudioAsset(U"gameClear").play();
}

void GameClear::update()
{
	if (getData().select < 2 && KeyS.up() || KeyDown.up())getData().select++;
	if (getData().select > 0 && KeyW.up() || KeyUp.up())getData().select--;
	if (KeyEnter.pressed())
	{
		if (getData().select == 0) {
			getData().Life = 3;
			AudioAsset(U"gameClear").stop();
			changeScene(GameState::Game);
		}
		if (getData().select == 1) {
			AudioAsset(U"gameClear").stop();
			changeScene(GameState::Title);
		}
		if (getData().select == 2) {
			getData().stageNum++;
			getData().Life = 3;
			if (getData().stageNum <= 2) {
				AudioAsset(U"gameClear").stop();
				changeScene(GameState::Game);
			}
			else {
				AudioAsset(U"gameClear").stop();
				changeScene(GameState::Title);
			}
		}
	}
}

void GameClear::draw() const
{
	TextureAsset(U"back")
		.scaled(8.0, 4.3)
		.draw(0, 0);

	TextureAsset(U"clear")
		.scaled(1, 1)
		.drawAt(Scene::Center().x, 400);

	FontAsset(U"Title")(U"Retry")
		.drawAt(Scene::Center().x, 700, Palette::Orange);
	FontAsset(U"Title")(U"Title")
		.drawAt(Scene::Center().x, 800, Palette::Orange);
	FontAsset(U"Title")(U"Next")
		.drawAt(Scene::Center().x, 900, Palette::Orange);

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
	case 2:
	{
		TextureAsset(U"select")(0, 0, 248)
			.scaled(0.3, 0.3)
			.drawAt(770, 900);
		break;
	}
	}
}
