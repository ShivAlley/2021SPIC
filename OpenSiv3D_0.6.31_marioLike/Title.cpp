#include "Title.h"
#include "LoadCSV.h"

Title::Title(const InitData& init):IScene(init)
{
	AudioAsset(U"title").setVolume(0.7);
	AudioAsset(U"title").play();
}

void Title::update()
{
	getData().Life = 3;
	if (KeyEnter.up())selectFlag = true;
	getData().stageNum = Clamp(getData().stageNum, 0, 2);
	if (KeyS.up()|| KeyDown.up())getData().stageNum++;
	if (KeyW.up()|| KeyUp.up())getData().stageNum--;
	if (KeyEnter.up())selectFlag = true;
	if (KeyEnter.pressed() && selectFlag) {
		AudioAsset(U"title").stop();
		changeScene(GameState::Game);
	}
}

void Title::draw() const
{

	TextureAsset(U"back")
		.scaled(8.0, 4.3)
		.draw(0, 0);
	const double t = Min(stopwatch.sF() * 0.8, 1.0);
	const double e = EaseOutBounce(t);
	const double ee = EaseOutSine(t);
	const Vec2 pos = start.lerp(goal, e);
	const Vec2 posSine = start.lerp(goal, ee);
	TextureAsset(U"titleLogo")
		.draw(posSine.x, pos.y);

	if (!selectFlag) {
		int time = Scene::Time();
		if (time % 2 == 0) {
			FontAsset(U"Title")(U"Start to Enter")
				.drawAt(Scene::Center().x,Scene::Center().y*1.5, Palette::Orange);
		}

	}
	else {
		FontAsset(U"Title")(U"Stage 1")
			.drawAt(Scene::Center().x, 700, Palette::Orange);
		FontAsset(U"Title")(U"Stage 2")
			.drawAt(Scene::Center().x, 800, Palette::Orange);
		FontAsset(U"Title")(U"Stage 3")
			.drawAt(Scene::Center().x, 900, Palette::Orange);


		switch (getData().stageNum)
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
}
