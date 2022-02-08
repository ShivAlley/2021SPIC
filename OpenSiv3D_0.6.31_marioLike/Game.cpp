#include "stdafx.h"
#include "Game.h"




Game::Game(const InitData& init):IScene(init)
{
	//visitor.inspector[Accessor::first] = [&]() {return foo(); };
	//std::visit(visitor, visitor.inspector.at(visitor.stateMachine.front()));
	player.LVbody() = world.createRect(P2Dynamic, Vec2(0, -100), RectF(CHIP_SIZE, CHIP_SIZE * 2), P2Material());
	Size size = { map.width(), map.height() };//マップの大きさ｛ｘ、ｙ｝
	for (auto p : step(size)) {
		if (map[p.y][p.x] == 1)
			chips << world.createRect(P2Static, Vec2{ p.x * 32 - 1280 / 2,p.y * 32 - 720 / 2 }, RectF(32, 32, 32, 32), P2Material());
	}
	camera.setParameters(Camera2DParameters::MouseOnly());
}

void Game::update()
{

	
	for (accumulatorSec += Scene::DeltaTime(); stepSec <= accumulatorSec; accumulatorSec -= stepSec)
	{
		// 2D 物理演算のワールドを更新
		world.update(stepSec);
	}

	camera.update();
	camera.setCenter(player.body().getPos());
	//{

	//	// 2D カメラの設定から Transformer2D を作成
	//	const auto t = camera.createTransformer();

	//	for (int32 i = 0; i < 8; ++i)
	//	{
	//		Circle{ 0, 0, (50 + i * 50) }.drawFrame(2);
	//	}

	//	Shape2D::Star(50, Vec2{ 200, 200 }).draw(Palette::Yellow);
	//}

	//camera.update();
	//{
	//	const auto t = camera.createTransformer();
	//	for (const auto& chip : chips)
	//	{
	//		chip.draw(HSV{ chip.id() * 10.0 });
	//	}
	//}
}

void Game::draw() const
{
	const auto t = camera.createTransformer();
	player.body().draw();
	for (const auto& chip : chips)
	{
		chip.draw(HSV{ chip.id() * 10.0 });
	}
	
}
